#ifndef NAMES_HPP
#define NAMES_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class NAMES : public ICommand
{
public:
    NAMES(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::vector<std::string> _channels;

    void _listAllChannelMembers(Client *client);
};

#endif
