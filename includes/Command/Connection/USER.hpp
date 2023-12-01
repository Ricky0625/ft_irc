#ifndef USER_HPP
#define USER_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class USER : public ICommand
{
public:
    USER(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _username;
    std::string _realname;
};

#endif
