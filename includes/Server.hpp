/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:02:15 by wricky-t          #+#    #+#             */
/*   Updated: 2023/12/03 19:55:11 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "irclib.h"
#include "Client.hpp"
#include "CommandFactory.hpp"
#include "Display.hpp"

#define INVALID_USAGE "Invalid usage! ./ircserv <port> <password>"
#define FAIL_TO_INIT_SOCKET "Failed to initialize socket!"
#define BUFFER_SIZE 4096

class CommandFactory; // forward declaration

class Server
{
public:

    typedef std::vector<struct pollfd> PollFdList;  // <pollfd>
    typedef std::map<int, struct pollfd> PollTable; // <socketfd, pollfd>
    typedef std::map<int, Client *> ClientTable;    // <socketfd, client object>
    // typedef std::map<std::string, Channel *> ChannelTable; // <channel name, channel object>

    Server(const std::string &port, const std::string &password);
    ~Server(void);

    // event loop
    void start(void);

    // event management
    void subscribeEvent(int clientFd, short event);
    void unsubscribeEvent(int clientFd, short event);

    // checking
    bool isCorrectPassword(const std::string &password);
    bool isClientAuthenticated(int clientFd);
    bool isNicknameTaken(const std::string &newNick) const;

    // getters
    Client *getClient(int clientFd) const;
    std::string getUpTime(void) const;

private:
    int _serverFd;
    std::string _password;
    std::string _upTime;

    PollFdList _pollList;
    PollTable _pollTable;
    ClientTable _clients;
    CommandFactory *_cmdFactory;

    // initialization
    void _createServerSocket(const std::string &port);
    void _updateUpTime(void);

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
    void _processRequests(int clientFd, Client *target);
    void _sendReply(int clientFd);
};

// utils
void setupHints(struct addrinfo &hints);
void obtainSuitableAddresses(const std::string &port, struct addrinfo **servinfo);
bool createBindListen(struct addrinfo *addr, int &socketfd);

#endif
