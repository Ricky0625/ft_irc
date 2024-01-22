#include "LIST.hpp"

// default constructor
LIST::LIST() {}

void LIST::initialize(Server &server, const IRCMessage &ircMsg)
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

void LIST::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Channel *targetChannel;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;

    if (getArgs().size() == 0)
    {
        _listAllChannels(client);
        return;
    }

    /**
     * TODO:
     * - ISUPPORT ELIST
     * - hide secret channel,
     */

    client->enqueueBuffer(SEND, RPL_LISTSTART(client));
    for (size_t i = 0; i < _channels.size(); i++)
    {
        const std::string &channelName = _channels[i];

        targetChannel = server.getChannel(channelName);
        if (targetChannel != NULL)
            client->enqueueBuffer(SEND, RPL_LIST(client, targetChannel));
    }
    client->enqueueBuffer(SEND, RPL_LISTEND(client));
}

void LIST::_listAllChannels(Client *client)
{
    Server &server = *_server;
    Server::ChannelTable allChannels = server.getChannels();
    Channel *targetChannel;

    client->enqueueBuffer(SEND, RPL_LISTSTART(client));
    for (Server::ChannelTable::iterator it = allChannels.begin(); it != allChannels.end(); it++)
    {
        targetChannel = it->second;
        if (targetChannel->channelModes.hasMode('s') && targetChannel->getMember(client) == NULL)
            continue;
        client->enqueueBuffer(SEND, RPL_LIST(client, targetChannel));
    }
    client->enqueueBuffer(SEND, RPL_LISTEND(client));
}
