/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:07:17 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/28 21:53:21 by wricky-t         ###   ########.fr       */
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

void Server::subscribeEvent(int clientFd, short event)
{
    PollTable::iterator target = _pollTable.find(clientFd);

    if (target == _pollTable.end())
        return;

    struct pollfd &socketInfo = target->second;
    socketInfo.events |= event;
    _updatePollList();
}

void Server::unsubscribeEvent(int clientFd, short event)
{
    PollTable::iterator target = _pollTable.find(clientFd);

    if (target == _pollTable.end())
        return;

    struct pollfd &socketInfo = target->second;
    socketInfo.events &= event;
    _updatePollList();
}

Client *Server::getClient(int clientFd) const
{
    ClientTable::const_iterator client = _clients.find(clientFd);

    if (client == _clients.end())
        return NULL;

    return client->second;
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

    while (true)
    {
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
        if (readBuffer.empty())
            break;
    }
    close(_serverFd);
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

    while ((crlfPos = readBuffer.find(CRLF)) != std::string::npos)
    {
        std::string singleRequest = readBuffer.substr(0, crlfPos);
        ircMsg = Parser::parseIRCMessage(singleRequest);
        std::cout << BOLD_YELLOW << singleRequest << RESET << std::endl;
        command = _cmdFactory->recognizeCommand(*this, ircMsg);
        if (command != NULL)
        {
            command->execute(clientFd);
            subscribeEvent(clientFd, POLLOUT); // is it ok to subscribe the event in a loop?
            delete command;
        }
        readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
    }
}

void Server::_sendReply(int clientFd)
{
    Client *target = getClient(clientFd);
    ssize_t bytesSent;

    if (target == NULL || target->getBuffer(SEND).empty())
    {
        unsubscribeEvent(clientFd, POLLOUT);
        return;
    }

    std::string sendBuffer = target->getBuffer(SEND);
    bytesSent = send(clientFd, sendBuffer.c_str(), sendBuffer.size(), 0);
    if (bytesSent == -1)
    {
        unsubscribeEvent(clientFd, POLLOUT);
        return;
    }
    
    target->clearBuffer(SEND);
    target->queueBuffer(SEND, sendBuffer.substr(bytesSent));

    if (!target->getBuffer(SEND).empty())
        return;
    unsubscribeEvent(clientFd, POLLOUT);
}
