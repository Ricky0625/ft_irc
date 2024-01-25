#ifndef INVITE_HPP
#define INVITE_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class INVITE : public ICommand
{
public:
    INVITE(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _nickname;
    std::string _channel;
};

#endif