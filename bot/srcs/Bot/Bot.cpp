/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:05:21 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/09 14:35:43 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot(const std::string &serverIp, const std::string &port, const std::string &password) : _password(password)
{
    /**
     * 1. client creates a socket using `socket` to establish a communication endpoint
     * 2. client obtains the server's address information using getaddrinfo
     * 3. client iterates through the results and tries to connect to the server using connect
     * 4. if the connection is sucessful, the client has a socket file desciriptor that it can use to communicate with the server
     */

    struct addrinfo *servinfo;

    _createBotSocket();
    _obtainServerInfo(serverIp, port, &servinfo);
    _connectToServer(servinfo);

    _nickname = "capybot";
    _username = "capybara00";
    _realname = "capybara the great";

    Display::displayBotInfo(serverIp, port);
}

Bot::~Bot()
{
    close(_botFd);
}

void Bot::start(void)
{
    // connect to server here.
}

void Bot::_createBotSocket()
{
    int socketfd;

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        Logger::exitLog("socket", &strerror);

    // delete this
    int optvalue = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue)) == -1)
        Logger::exitLog("setsockopt", &strerror);
    // delete this

    _botFd = socketfd;
}

void Bot::_obtainServerInfo(const std::string &ip, const std::string &port, struct addrinfo **servinfo)
{
    struct addrinfo hints;

    // setup hints
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(ip.c_str(), port.c_str(), &hints, servinfo) != 0)
        Logger::c_exitLog("getaddrinfo", &gai_strerror);
}

void Bot::_connectToServer(struct addrinfo *servinfo)
{
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
    {
        if (connect(_botFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(_botFd);
            continue;
        }
        // if connection successful, break.
        break;
    }
    freeaddrinfo(servinfo);
}
