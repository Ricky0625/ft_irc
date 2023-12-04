#ifndef PING_HPP
#define PING_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class PING : public ICommand
{
public:
    PING(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _token;
};

#endif
