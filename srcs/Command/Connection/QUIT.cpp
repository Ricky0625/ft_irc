#include "Connection/QUIT.hpp"

// default constructor
QUIT::QUIT() : _msg("") {}

void QUIT::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    if (argCount > 0)
    {
        _msg = args[0];
        return;
    }

    _msg = getTrailing();
}

void QUIT::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL)
        return;

    _sayGoodbyeToChannels(target); // send quit message to all channels (if the user is in there)
    _sendERROR(target);
    server.removeMemberFromChannels(target->getNickname()); // remove user from channels
    server.removeClient(clientFd);
}

void QUIT::_sayGoodbyeToChannels(Client *client)
{
    Server &server = *_server;
    Server::ChannelTable allChannels = server.getChannels();
    Channel *targetChannel;

    for (Server::ChannelTable::iterator it = allChannels.begin(); it != allChannels.end(); it++)
    {
        targetChannel = it->second;
        if (targetChannel && targetChannel->getMember(client) == NULL) // if the user not in channel
            continue;

        _broadcastMemberQuit(client, targetChannel);
    }
    client->enqueueBuffer(SEND, RPL_QUIT(client, _msg)); // send to self
}

void QUIT::_broadcastMemberQuit(Client *client, Channel *channel)
{
    Channel::MemberTable allMembers = channel->getMembers();
    Client *receiver;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = allMembers.begin(); it != allMembers.end(); it++)
    {
        receiver = it->second->getClientInfo();
        if (receiver && receiver->getNickname() == client->getNickname()) // don't send to self
            continue;

        server.subscribeEvent(receiver->getFd(), POLLOUT);
        receiver->enqueueBuffer(SEND, RPL_QUIT(client, _msg));
    }
}

void QUIT::_sendERROR(Client *client)
{
    ssize_t bytesSent;
    Server &server = *_server;
    const std::string &msg = RPL_ERROR("Closing link");

    server.subscribeEvent(client->getFd(), POLLOUT);
    bytesSent = send(client->getFd(), msg.c_str(), msg.size(), 0);
    if (bytesSent > 0)
        Display::displayOutgoing(client->getFd(), msg);
    server.unsubscribeEvent(client->getFd(), POLLOUT);
}
