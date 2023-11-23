/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:02:15 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/23 22:39:13 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "irclib.h"
#include "Client.hpp"
#include "ICommand.hpp"
#include "CommandFactory.hpp"

#define INVALID_USAGE "Invalid usage! ./ircserv <port> <password>"
#define FAIL_TO_INIT_SOCKET "Failed to initialize socket!"
#define BUFFER_SIZE 1024

class Server
{
public:
    typedef std::vector<struct pollfd> PollFdList;         // <pollfd>
    typedef std::map<int, struct pollfd> PollTable;        // <socketfd, pollfd>
    typedef std::map<int, Client *> ClientTable;           // <socketfd, client object>
    // typedef std::map<std::string, Channel *> ChannelTable; // <channel name, channel object>

    Server(const std::string &port, const std::string &password);
    ~Server(void);

    // event loop
    void start(void);

    // checking
    bool isCorrectPassword(const std::string &password);

private:
    int _serverFd;
    std::string _password;
    PollFdList _pollList;
    PollTable _pollTable;
    ClientTable _clients;
    CommandFactory *_cmdFactory;

    // initialization
    void _createServerSocket(const std::string &port);

    // monitor list management
    void _addSocketToPollTable(int socketToMonitor, short events);
    void _updatePollList(void);
    void _stopListening(int clientFd);

    // events
    void _handleSocketEvents(void);
    void _handleServerEvents(const pollfd &socketInfo);
    void _handleClientEvents(const pollfd &socketInfo);

    // action
    int _acceptConnection(int socketFd);
    void _removeClient(int clientFd);
    void _readRequest(int clientFd);
    void _processRequests(int clientFd, std::string &requestStr);
};

// utils
void setupHints(struct addrinfo &hints);
void obtainSuitableAddresses(const std::string &port, struct addrinfo **servinfo);
bool createBindListen(struct addrinfo *addr, int &socketfd);

#endif
