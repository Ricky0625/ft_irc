/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:07:17 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/20 12:25:23 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// default constructor
Server::Server(const std::string &port, const std::string &password) : _password(password), _cmdFactory(new CommandFactory())
{
    _createServerSocket(port);
    if (_serverFd == -1)
        Logger::msgExitLog(FAIL_TO_INIT_SOCKET);
    _addSocketToPollTable(_serverFd, POLLIN);
    _updateUpTime();
    Display::clearScreen();
    Parser::parseConfigFile(CONFIG_PATH, _config);
    Parser::showConfig(_config);
    _initializeSupportedModes(USERMODE);
    _initializeSupportedModes(CHANNELMODE);
    Display::displayServerInfo(port, password, getUpTime());
}

// destructor
Server::~Server()
{
    /**
     * TODO:
     * server cleanup
     */
    for (size_t i = 0; i < _pollList.size(); i++)
    {
        close(_pollList[i].fd);
        _pollTable.erase(_pollList[i].fd);
    }
    delete _cmdFactory;
}

/**
 * @brief Check if the given password matches server's password
 */
bool Server::isCorrectPassword(const std::string &password)
{
    return _password == password;
}

bool Server::isClientAuthenticated(int clientFd)
{
    Client *target = getClient(clientFd);

    if (target == NULL)
        return false;

    return target->isAuthenticated();
}

/**
 * @brief Check if a nickname is taken
 * TODO: not yet check if two names are strictly identical
 */
bool Server::isNicknameTaken(const std::string &newNick) const
{
    for (ClientTable::const_iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        const std::string &clientNickname = it->second->getNickname();
        if (clientNickname.empty())
            continue;
        if (clientNickname == newNick)
            return true;
    }
    return false;
}

/**
 * @brief Start the IRC server
 * @details
 * Intiates the main event loop of the IRC server, where it continuously monitors and handles
 * events on ther server and client sockets. The main roles of this function includes:
 * - Continously polling sockets for events
 * - Handling incoming client connection and server socket events
 * - Invoke `_handleSocketEvents` to manage socket events
 */
void Server::start(void)
{
    int numEvents;

    while (true)
    {
        numEvents = poll(_pollList.data(), _pollList.size(), TIMEOUT);
        if (numEvents == -1)
        {
            Logger::justLog("poll", &strerror);
            break;
        }
        else if (numEvents == 0)
        {
            // timeout management here
            // periodically ping client to check if they're still alive
            _sendPingToClients();
            _checkClientTimeout();
        }

        _handleSocketEvents();
    }
}

/**
 * @brief Subscribe an event for a client
 */
void Server::subscribeEvent(int clientFd, short event)
{
    PollTable::iterator target = _pollTable.find(clientFd);

    if (target == _pollTable.end())
        return;

    struct pollfd &socketInfo = target->second;

    // prevent redundant subscription of events
    if (socketInfo.events & event)
        return;

    socketInfo.events |= event;
    _updatePollList();
}

/**
 * @brief Unsubscribe an event from a designated client's socket event list
 */
void Server::unsubscribeEvent(int clientFd, short event)
{
    PollTable::iterator target = _pollTable.find(clientFd);

    if (target == _pollTable.end())
        return;

    struct pollfd &socketInfo = target->second;

    if ((socketInfo.events & event) == 0)
        return;

    socketInfo.events &= ~event;
    _updatePollList();
}

/**
 * @brief Find a client based on their fd from the client table
 * @return NULL if not found. Pointer to that client object if found.
 */
Client *Server::getClient(int clientFd) const
{
    ClientTable::const_iterator client = _clients.find(clientFd);

    if (client == _clients.end())
        return NULL;

    return client->second;
}

Client *Server::getClientByNickname(const std::string &nickname) const
{
    Client *client;

    for (ClientTable::const_iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        client = it->second;
        if (client && client->getNickname() == nickname)
            return client;
    }

    return NULL;
}

std::string Server::getUpTime() const
{
    return _upTime;
}

bool Server::isSupportedMode(char mode, ModeType type) const
{
    const Modes &modeObj = (type == USERMODE ? _supportedUserModes : _supportedChannelModes);

    return modeObj.hasMode(mode);
}

/**
 * @brief Create a server socket based on the given port
 * @details
 * 1. Obtain a list of suitable addresses that the server can use to create sockets and bind them
 * 2. For each suitable addresses, seach for ONE address that can be used to create socket, bind & listen.
 * 3. Store the newly created socket fd to a list
 * 4. Free the linked list of suitabe addresses
 */
void Server::_createServerSocket(const std::string &port)
{
    int socketfd = -1;
    struct addrinfo *servinfo;

    obtainSuitableAddresses(port, &servinfo);
    for (struct addrinfo *addr = servinfo; addr != NULL; addr = addr->ai_next)
    {
        if (createBindListen(addr, socketfd) == false)
        {
            _serverFd = -1;
            continue;
        }
        _serverFd = socketfd;
        break;
    }
    freeaddrinfo(servinfo);
}

void Server::_updateUpTime()
{
    _upTime = Parser::getTimeNow();
}

/**
 * @brief Read the value from the config file and intialize the supported modes
*/
void Server::_initializeSupportedModes(ModeType type)
{
    const std::string &key = (type == USERMODE ? "user" : "channel");
    Modes &modeObj = (type == USERMODE ? _supportedUserModes : _supportedChannelModes);

    try
    {
        const std::string &allModes = getValueFromSection("MODE", key);

        for (size_t i = 0; i < allModes.size(); i++)
            modeObj.addMode(allModes[i]);
    }
    catch (const std::exception &ex)
    {
        return;
    }
}

/**
 * @brief Add a socket into server's monitor table
 * @details
 * 1. Check if we should add this fd to _pollTable (check if it's already been monitored)
 * 2. Add socket as `struct pollfd` if can be added
 * 3. Update the monitor list
 */
void Server::_addSocketToPollTable(int socketToMonitor, short events)
{
    // if this socket has already been monitored, do nothing
    if (_pollTable.find(socketToMonitor) != _pollTable.end())
        return;

    struct pollfd socketInfo;
    socketInfo.fd = socketToMonitor;
    socketInfo.events = events;
    _pollTable[socketToMonitor] = socketInfo;
    _updatePollList(); // update the _pollList
    Display::displayServerAction(socketToMonitor, "Add new client. `Server::_addSocketToPollTable`");
}

/**
 * @brief Update the monitor list based on the poll table
 */
void Server::_updatePollList(void)
{
    _pollList.clear();
    for (PollTable::iterator it = _pollTable.begin(); it != _pollTable.end(); ++it)
        _pollList.push_back(it->second);
}

/**
 * @brief Stop listening to a client
 * @details
 * Close the client fd and erase the client from the pollTable.
 */
void Server::_stopListening(int clientFd)
{
    PollTable::iterator socket = _pollTable.find(clientFd);

    if (socket != _pollTable.end())
    {
        close(clientFd);
        _pollTable.erase(socket);
        _updatePollList();
    }
}

/**
 * @brief Check each of the monitored socket if there's an event.
 *
 * If the monitored fd has an event and is server fd, handle server event.
 * Else handle it as a client.
 */
void Server::_handleSocketEvents(void)
{
    for (size_t i = 0; i < _pollList.size(); i++)
    {
        pollfd &socketInfo = _pollList[i];
        socketInfo.fd == _serverFd
            ? _handleServerEvents(socketInfo)
            : _handleClientEvents(socketInfo);
    }
}

/**
 * @brief Handle server's events
 */
void Server::_handleServerEvents(const pollfd &socketInfo)
{
    int clientFd;

    if (socketInfo.revents & POLLIN)
    {
        if ((clientFd = _acceptConnection(socketInfo.fd)) == -1)
            return;
        _addSocketToPollTable(clientFd, POLLIN);
    }
}

static void pollEventErrorMessage(short event, int clientFd)
{
    switch (event)
    {
    case POLLERR:
        Display::displayServerError(clientFd, "An error has occured. `POLLERR`");
        break;
    case POLLHUP:
        Display::displayServerError(clientFd, "Client hang up. `POLLHUP`");
        break;
    case POLLNVAL:
        Display::displayServerError(clientFd, "Client socket not initialized properly. `POLLNVAL`");
        break;
    }
}

/**
 * @brief Handle client's events
 */
void Server::_handleClientEvents(const pollfd &socketInfo)
{
    if (socketInfo.revents & POLLOUT) // server can write response without blocking
    {
        _sendReply(socketInfo.fd);
    }
    else if (socketInfo.revents & POLLIN) // server can listen request without blocking
    {
        _readRequest(socketInfo.fd);
    }
    else if (socketInfo.revents & POLLERR) // an error has occurred on this socket
    {
        pollEventErrorMessage(POLLERR, socketInfo.fd);
        removeClient(socketInfo.fd, ERR_POLLERR); // ERR_POLLERR
    }
    else if (socketInfo.revents & POLLHUP) // the remote side of the connection hung up
    {
        pollEventErrorMessage(POLLHUP, socketInfo.fd);
        removeClient(socketInfo.fd, ERR_POLLHUP); // ERR_POLLHUP
    }
    else if (socketInfo.revents & POLLNVAL) // not sure if this will ever happen. this means that there's something wrong with the socket initialization
    {
        pollEventErrorMessage(POLLNVAL, socketInfo.fd);
        removeClient(socketInfo.fd, ERR_POLLNVAL); // ERR_POLLNVAL
    }
}

/**
 * @brief Check if any of the client timeout. If so, remove them.
 * @details
 * Iterate through each of the client to check if their last ping exceeds the timeout.
 * If so, push to a list. This is to prevent we modifying the map while iterating through it.
 * Lastly, iterate through the list and remove the client using their socketfd.
 */
void Server::_checkClientTimeout(void)
{
    std::vector<int> toBeRemoved;

    for (ClientTable::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        Client *client = it->second;
        if (client->isTimeout())
            toBeRemoved.push_back(it->first);
    }

    for (std::vector<int>::iterator it = toBeRemoved.begin(); it != toBeRemoved.end(); it++)
        removeClient(*it, PING_TIMEOUT); // PING_TIMEOUT
}

/**
 * @brief Send PING to designated socketfd check the liveliness of the client
 */
void Server::_sendPing(int clientFd)
{
    std::string pingMsg = "PING :" + std::string(HOST) + CRLF;

    send(clientFd, pingMsg.c_str(), pingMsg.size(), 0);
}

/**
 * @brief Send PING to each of the client and update server last ping time
 */
void Server::_sendPingToClients(void)
{
    for (ClientTable::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        _sendPing(it->first);
        Display::displayServerAction(it->first, "PING! `Server::_sendPingToClients`");
    }
}

/**
 * @brief Accept a new connection
 * @details
 * Try accept a new connection. If can, create a new Client instance and store it into the ClientTable.
 * @return Client fd if successful. -1 if error.
 */
int Server::_acceptConnection(int socketFd)
{
    struct sockaddr_storage incoming_addr;
    socklen_t addr_size = sizeof(incoming_addr);
    int clientfd;

    if ((clientfd = accept(socketFd, (struct sockaddr *)&incoming_addr, &addr_size)) == -1)
    {
        Logger::justLog("accept", &strerror);
        return -1;
    }

    // set client socket to non-blocking
    if (fcntl(clientfd, F_SETFL, O_NONBLOCK) == -1)
    {
        Logger::justLog("fcntl", &strerror);
        close(clientfd);
        return -1;
    }

    // get client's address information & create a new client
    struct sockaddr_in *clientAddr = (struct sockaddr_in *)&incoming_addr;
    _clients[clientfd] = new Client(clientfd, inet_ntoa(clientAddr->sin_addr));
    return clientfd;
}

/**
 * @brief Disconnect a client and remove them from the ClientTable.
 */
void Server::removeClient(int clientFd, QuitReason reason)
{
    ClientTable::iterator client = _clients.find(clientFd);

    (void)reason;
    if (client != _clients.end())
    {
        delete client->second;    // clean up the client instance
        _clients.erase(client);   // remove from ClientTable
        _stopListening(clientFd); // stop listening from this client
        Display::displayServerAction(clientFd, "Removed! `Server::_removeClient`");
    }
}

/**
 * @brief Read request from client. Try process request in each iteration.
 */
void Server::_readRequest(int clientFd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    Client *target = getClient(clientFd);

    if (target == NULL) // no such client
        return;

    bytesRead = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead == -1)
    {
        Logger::justLog("recv", &strerror);
        removeClient(clientFd, RECV_FAILED); // RECV_FAILED
        return;
    }
    else if (bytesRead == 0) // client disconnected
    {
        removeClient(clientFd, DISCONNECTED); // DISCONNECTED
        return;
    }

    // null terminated the string
    buffer[bytesRead] = '\0';

    // concat string & process string
    target->enqueueBuffer(READ, buffer);
    _processRequests(clientFd, target);
}

/**
 * @brief Process the request string.
 * @details
 * As long as the request string has a CRLF, extract the string before it
 * and parse the request string. CRLF marks the end of a client request.
 */
void Server::_processRequests(int clientFd, Client *target)
{
    size_t crlfPos;
    IRCMessage ircMsg;
    ICommand *command;
    std::string singleRequest;
    std::string readBuffer = target->getBuffer(READ);

    while ((crlfPos = readBuffer.find(CRLF)) != std::string::npos)
    {
        singleRequest = readBuffer.substr(0, crlfPos);
        if (singleRequest.empty())
        {
            readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
            continue;
        }
        ircMsg = Parser::parseIRCMessage(singleRequest);
        Display::displayIncoming(clientFd, singleRequest);

        command = _cmdFactory->recognizeCommand(*this, ircMsg);
        if (command != NULL)
            command->execute(clientFd);

        readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
    }

    // subscribe to POLLOUT if send buffer is not empty
    if (target->getBuffer(SEND).empty() == false)
        subscribeEvent(clientFd, POLLOUT);

    // if after processing all the request, readBuffer still have something, clear client's read buffer
    // and enqueue the remaining read buffer
    target->clearBuffer(READ);
    if (!readBuffer.empty())
        target->enqueueBuffer(READ, readBuffer);
}

/**
 * @brief Send back replies to designated client
 * @details
 * If any of the following occurs, the client will be unsubscribe to POLLOUT event
 * 1. target not found
 * 2. target's send buffer is empty
 * Othewise, try to send everything in the buffer to the client. In case where the
 * send function only sent a part of the message, queue send buffer with the
 * remaining message.
 * If after sending the message, the sendbuffer is empty, unsubscribe to POLLOUT.
 */
void Server::_sendReply(int clientFd)
{
    Client *target = getClient(clientFd);
    ssize_t bytesSent;

    if (target == NULL || target->getBuffer(SEND).empty())
        return;

    std::string sendBuffer = target->getBuffer(SEND);
    bytesSent = send(clientFd, sendBuffer.c_str(), sendBuffer.size(), 0);

    Display::displayOutgoing(clientFd, sendBuffer.substr(0, bytesSent));

    if (bytesSent == -1)
    {
        Logger::justLog("send", &strerror);
        return;
    }

    target->clearBuffer(SEND);
    target->enqueueBuffer(SEND, sendBuffer.substr(bytesSent));
    unsubscribeEvent(clientFd, POLLOUT);
}
