#ifndef JOIN_HPP
#define JOIN_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class JOIN : public ICommand
{
public:
    JOIN(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::vector<std::string> _chanNames;
    std::vector<std::string> _chanPass;

    void _broadcastNewMember(Client *newMember, Channel *channel);
};

#endif
