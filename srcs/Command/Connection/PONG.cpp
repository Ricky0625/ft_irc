#include "Connection/PONG.hpp"

// default constructor
PONG::PONG() : _token("") {}

void PONG::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();

    if (args.empty())
        return;
    
    _token = args[0];
}

void PONG::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL || !(target->isAuthenticated() && target->isRegistered()))
        return;

    target->updateLastPing();
    Display::displayServerAction(clientFd, "update last ping `PONG::execute`");
}
