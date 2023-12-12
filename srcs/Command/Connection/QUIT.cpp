#include "Connection/QUIT.hpp"

// default constructor
QUIT::QUIT() : _msg("") {}

void QUIT::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    _msg = getTrailing();
}

void QUIT::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL || !(target->isAuthenticated() && target->isRegistered()))
        return;
    
    // no need any checking, just return with the _msg and that's it
    // not sure if need to send to every client? should be?
}
