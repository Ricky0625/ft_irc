/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IOManagement.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 14:42:23 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/26 14:44:40 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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
        else
            target->enqueueBuffer(SEND, ERR_UNKNOWNCOMMAND(target, ircMsg.command));

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
