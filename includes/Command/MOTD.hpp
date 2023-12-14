#ifndef MOTD_HPP
#define MOTD_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class MOTD : public ICommand
{
public:
    MOTD(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);
};

#endif
