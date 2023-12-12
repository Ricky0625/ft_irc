#include "Channel/JOIN.hpp"

// default constructor
JOIN::JOIN() {}

void JOIN::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);
}

void JOIN::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL || !(target->isAuthenticated() && target->isRegistered()))
        return;
}
