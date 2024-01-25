#ifndef KICK_HPP
#define KICK_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class KICK : public ICommand
{
public:
    KICK(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _channel;
    std::vector<std::string> _users;
    std::string _reason;

    void _broadcastKickedMember(Client *member, Channel *channel);
};

#endif
