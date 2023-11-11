/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:02:15 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/10 15:19:18 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "irclib.h"

#define INVALID_USAGE "Invalid usage! ./ircserv <port> <password>"
#define FAIL_TO_INIT_SOCKET "Failed to initialize socket!"

class Server
{
public:
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, struct pollfd> PollTable; // <socketfd, pollfd>

    Server(const std::string &port, const std::string &password);
    ~Server(void);

    void start(void);

private:
    int _serverFd;
    std::string _password;
    PollFdList _pollList;
    PollTable _pollTable;

    // initialization
    void _createServerSocket(const std::string &port);

    // monitor list management
    void _addSocketToPollTable(int socketToMonitor, short events);
    void _updatePollList(void);

    // events
    void _handleSocketEvents(void);
    void _handleServerEvents(const pollfd &socketInfo);
    void _handleClientEvents(const pollfd &socketInfo);

    // action
    int _acceptConnection(int socketFd);
};

// utils
void setupHints(struct addrinfo &hints);
void obtainSuitableAddresses(const std::string &port, struct addrinfo **servinfo);
bool createBindListen(struct addrinfo *addr, int &socketfd);

#endif
