#include "NAMES.hpp"

// default constructor
NAMES::NAMES() {}

void NAMES::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    _channels.clear();

    if (argCount == 0)
        return;

    if (argCount > 0)
        Parser::splitStr(args[0], _channels, ",", INCLUDE_EMPTY);
}

/**
 * TODO: secret channel mode, invisible user mode
*/
void NAMES::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Channel *targetChannel;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;
    
    if (getArgs().size() == 0)
    {
        _listAllChannelMembers(client);
        return;
    }

    for (size_t i = 0; i < _channels.size(); i++)
    {
        const std::string &channelName = _channels[i];

        targetChannel = server.getChannel(channelName);
        if (targetChannel != NULL)
            client->enqueueBuffer(SEND, RPL_NAMREPLY(client, targetChannel));
        client->enqueueBuffer(SEND, RPL_ENDOFNAMES(client, channelName));
    }
}

void NAMES::_listAllChannelMembers(Client *client)
{
    Server &server = *_server;
    Server::ChannelTable allChannels = server.getChannels();
    Channel *targetChannel;

    for (Server::ChannelTable::iterator it = allChannels.begin(); it != allChannels.end(); it++)
    {
        /**
         * TODO:
         * 1. filter out channels that has secret mode, unless the user is one of the member
         * 2. filter out all the invisible users in the channel, unless the user is one the of member
        */
        targetChannel = it->second;
        client->enqueueBuffer(SEND, RPL_NAMREPLY(client, targetChannel));
        client->enqueueBuffer(SEND, RPL_ENDOFNAMES(client, targetChannel->getName()));
    }
}
