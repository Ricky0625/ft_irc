#ifndef PASS_HPP
#define PASS_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class PASS : public ICommand
{
public:
    PASS(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _password;
};

#endif
