#include "PART.hpp"

// default constructor
PART::PART() :_reason("") {}

void PART::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    _channels.clear();
    _reason.clear();

    if (argCount == 0)
        return;

    if (argCount > 0)
        Parser::splitStr(args[0], _channels, ",");

    if (argCount > 1)
        _reason = args[1];
    else if (getHasTrailing())
        _reason = getTrailing();
}

void PART::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Channel *targetChannel;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;
    
    if (getArgs().size() < 1)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, _ircMsg.command));
        return;
    }

    for (size_t i = 0; i < _channels.size(); i++)
    {
        const std::string &channelName = _channels[i];

        targetChannel = server.getChannel(channelName);
        if (targetChannel == NULL)
        {
            client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, channelName));
            continue;
        }

        if (targetChannel->getMember(client) == NULL)
        {
            client->enqueueBuffer(SEND, ERR_NOTONCHANNEL(client, targetChannel));
            continue;
        }

        _broadcastMemberParting(client, targetChannel);
        targetChannel->removeMember(client->getNickname());

        if (targetChannel->getMemberTotal() == 0)
            server.removeChannel(targetChannel->getName());
    }
}

void PART::_broadcastMemberParting(Client *member, Channel *channel)
{
    Channel::MemberTable members = channel->getMembers();
    Client *target;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = members.begin(); it != members.end(); it++)
    {
        target = it->second->getClientInfo();
        server.subscribeEvent(target->getFd(), POLLOUT);
        target->enqueueBuffer(SEND, RPL_PART(member, channel, _reason));
    }
}
