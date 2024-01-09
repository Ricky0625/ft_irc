/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:52:45 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/09 14:35:52 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include "botlib.h"
#include "Color.hpp"
#include "Display.hpp"

#define INVALID_USAGE "Invalid usage! ./bot <ip> <port> <password>"
#define FAIL_TO_INIT_SOCKET "Failed to initialize socket!"

class Bot
{
public:
    Bot(const std::string &serverIp, const std::string &port, const std::string &password);
    ~Bot(void);

    void start(void);

private:
    //identity
    int _botFd;
    std::string _password;
    std::string _nickname;
    std::string _username;
    std::string _realname;

    // connection
    void _createBotSocket(void);
    void _obtainServerInfo(const std::string &ip, const std::string & port, struct addrinfo **servinfo);
    void _connectToServer(struct addrinfo *servinfo);
};
#endif