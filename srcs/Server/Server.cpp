/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:07:17 by wricky-t          #+#    #+#             */
/*   Updated: 2023/12/01 21:06:08 by wricky-t         ###   ########.fr       */
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
    Display::displayServerInfo(port, password, getUpTime());
}

// destructor
Server::~Server()
{
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
        numEvents = poll(_pollList.data(), _pollList.size(), -1);
        if (numEvents == -1)
        {
            Logger::justLog("poll", &strerror);
            break;
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
    socketInfo.events &= event;
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

std::string Server::getUpTime() const
{
    return _upTime;
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
    std::time_t currentTime = std::time(NULL);
    std::tm *localTime = std::localtime(&currentTime);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S", localTime);

    _upTime = buffer;
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

/**
 * @brief Handle client's events
 */
void Server::_handleClientEvents(const pollfd &socketInfo)
{
    if (socketInfo.revents & POLLOUT)
    {
        // server can write response without blocking
        // std::cout << "writing to " << socketInfo.fd << std::endl;
        _sendReply(socketInfo.fd);
    }
    else if (socketInfo.revents & POLLIN)
    {
        // server can listen request without blocking
        _readRequest(socketInfo.fd);
    }
    else if (socketInfo.revents & POLLERR)
    {
        // an error has occurred on this socket
        std::cout << "Bro, " << socketInfo.fd << " has some issues. Please investigate." << std::endl;
    }
    else if (socketInfo.revents & POLLHUP)
    {
        // the remote side of the connection hung up
        _removeClient(socketInfo.fd);
    }
    else if (socketInfo.revents & POLLNVAL)
    {
        // not sure if this will ever happen. this means that there's something wrong with the socket initialization
        std::cout << "Bro, there's an issue with this client's socket initialization." << std::endl;
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
void Server::_removeClient(int clientFd)
{
    ClientTable::iterator client = _clients.find(clientFd);

    if (client != _clients.end())
    {
        delete client->second;    // clean up the client instance
        _clients.erase(client);   // remove from ClientTable
        _stopListening(clientFd); // stop listening from this client
        std::cout << "Disconnect client " << clientFd << std::endl;
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

    if (target == NULL) // immature error handling here
        return;

    std::string readBuffer = target->getBuffer(READ);

    bytesRead = recv(clientFd, buffer, BUFFER_SIZE, 0);
    if (bytesRead == -1)
    {
        Logger::justLog("recv", &strerror);
        return;
    }
    else if (bytesRead == 0)
    {
        _processRequests(clientFd, readBuffer); // might be flawed
        _removeClient(clientFd);
        return;
    }

    // concat string & process string
    readBuffer.append(buffer, bytesRead); // might be flawed
    _processRequests(clientFd, readBuffer);
}

/**
 * @brief Process the request string.
 * @details
 * As long as the request string has a CRLF, extract the string before it
 * and parse the request string. CRLF marks the end of a client request.
 */
void Server::_processRequests(int clientFd, std::string &readBuffer)
{
    size_t crlfPos;
    IRCMessage ircMsg;
    ICommand *command;
    std::string singleRequest;

    Display::displayIncoming(clientFd, readBuffer);
    while ((crlfPos = readBuffer.find(CRLF)) != std::string::npos)
    {
        singleRequest = readBuffer.substr(0, crlfPos);
        ircMsg = Parser::parseIRCMessage(singleRequest);

        command = _cmdFactory->recognizeCommand(*this, ircMsg);
        if (command != NULL)
        {
            command->execute(clientFd);
            subscribeEvent(clientFd, POLLOUT); // is it ok to subscribe the event in a loop?
        }
        readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
    }
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
    if (bytesSent == -1)
        return;
    
    Display::displayOutgoing(clientFd, sendBuffer);
    target->clearBuffer(SEND);
    target->queueBuffer(SEND, sendBuffer.substr(bytesSent));

    if (!target->getBuffer(SEND).empty())
        return;

    if (target->isAuthenticated() == false || target->isRegistered() == false)
        _removeClient(clientFd);
}
