#include "MOTD.hpp"

// default constructor
MOTD::MOTD() {}

void MOTD::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);
}

void MOTD::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;
    
    queueMOTD(client);
}
