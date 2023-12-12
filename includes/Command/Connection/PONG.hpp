#ifndef PONG_HPP
#define PONG_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class PONG : public ICommand
{
public:
    PONG(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _token;
};

#endif
