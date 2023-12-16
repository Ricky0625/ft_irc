#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class PRIVMSG : public ICommand
{
public:
    PRIVMSG(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::vector<std::string> _targets;
    std::string _msg;
    bool _hasMsg;

    void _broadcastMessage(Client *client, Channel *channel);
};

#endif
