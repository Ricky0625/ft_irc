#ifndef PART_HPP
#define PART_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class PART : public ICommand
{
public:
    PART(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::vector<std::string> _channels;
    std::string _reason;

    void _broadcastMemberParting(Client *member, Channel *channel);
};

#endif
