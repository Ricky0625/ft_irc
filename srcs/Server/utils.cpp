/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 14:51:18 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/10 14:51:47 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**
 * @brief Setup a config for our server
 * @details
 * 1. Ensure that the hints is completely empty before initialization
 * 2. Initialize hints
 *      - AF_INET (IPv4)
 *      - SOCK_STREAM (TCP)
 *
 * NOTES: More customization can be done here
 */
void setupHints(struct addrinfo &hints)
{
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
}

/**
 * @brief Obtain a list of suitable addresses that could be used to create socket
 * 1. Setup hints
 * 2. Get addresses info
 */
void obtainSuitableAddresses(const std::string &port, struct addrinfo **servinfo)
{
    struct addrinfo hints;

    setupHints(hints);
    if (getaddrinfo("localhost", port.c_str(), &hints, servinfo) != 0)
        Logger::c_exitLog("getaddrinfo", &gai_strerror);
}

/**
 * @brief A process to create, bind & listen socket
 * @details
 * 1. Try creating a socket
 * 2. Try bind the newly created socket
 * 3. Try listen to the socket for incoming connection
 */
bool createBindListen(struct addrinfo *addr, int &socketfd)
{
    const int MAX_QUEUED_CONNECTIONS = 10;

    if ((socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) == -1)
    {
        Logger::justLog("socket", &strerror);
        return false;
    }

    fcntl(socketfd, F_SETFL, O_NONBLOCK, FD_CLOEXEC); // SET SOCKET TO NON-BLOCKING

    if (bind(socketfd, addr->ai_addr, addr->ai_addrlen) == -1)
    {
        Logger::justLog("bind", &strerror);
        close(socketfd);
        return false;
    }

    if (listen(socketfd, MAX_QUEUED_CONNECTIONS) == -1)
    {
        Logger::justLog("listen", &strerror);
        close(socketfd);
        return false;
    }
    return true;
}