#ifndef OPER_HPP
#define OPER_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class OPER : public ICommand
{
public:
    OPER(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _name;
    std::string _password;

};

#endif
