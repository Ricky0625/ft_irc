#include "PING.hpp"

// default constructor
PING::PING() : _token("") {}

void PING::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();

    if (args.empty())
        return;
    
    _token = args[0];
}

void PING::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL || !(target->isAuthenticated() && target->isRegistered()))
        return;

    target->queueBuffer(SEND, RPL_PONG(target, target->getNickname(), _token));
}