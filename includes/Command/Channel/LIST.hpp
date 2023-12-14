#ifndef LIST_HPP
#define LIST_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class LIST : public ICommand
{
public:
    LIST(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::vector<std::string> _channels;

    void _listAllChannels(Client *client);
};

#endif
