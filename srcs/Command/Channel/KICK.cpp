#include "KICK.hpp"

// default constructor
KICK::KICK() : _channel(""), _reason("") {}

void KICK::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    _reason.clear();

    if (argCount == 0)
        return;

    // normal parameters
    // KICK <channel> <user>{,<user>} [<comment>]
    if (argCount > 0)
        _channel = args[0];

    if (argCount > 1)
        Parser::splitStr(args[1], _users, ",", INCLUDE_EMPTY);

    if (argCount > 2)
        _reason = args[2];
    else if (getHasTrailing())
        _reason = getTrailing();
}

/**
 * ERR_NEEDMOREPARAMS (461) DONE
 * ERR_NOSUCHCHANNEL (403) DONE
 * ERR_USERNOTINCHANNEL (441) DONE
 * ERR_NOTONCHANNEL (442) DONE
 * ERR_CHANOPRIVSNEEDED (482) DONE
*/
void KICK::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Channel *targetChannel;
    ChannelMember *member;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;

    if (getArgs().size() < 2)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, "KICK"));
        return;
    }

    targetChannel = server.getChannel(_channel);
    if (targetChannel == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, _channel));
        return;
    }

    if (!targetChannel->getMember(client))
    {
        client->enqueueBuffer(SEND, ERR_NOTONCHANNEL(client, targetChannel));
        return;
    }

    if (!targetChannel->getMember(client)->memberMode.hasMode('o'))
    {
        client->enqueueBuffer(SEND, ERR_CHANOPRIVSNEEDED(client, _channel));
        return;
    }

    for (std::vector<std::string>::iterator it = _users.begin(); it != _users.end(); it++)
    {
        Client *targetClient = server.getClientByNickname(*it);
        if (targetClient == NULL)
        {
            client->enqueueBuffer(SEND, ERR_NOSUCHNICK(client, *it));
            continue;
        }

        member = targetChannel->getMember(targetClient);
        if (member == NULL)
        {
            client->enqueueBuffer(SEND, ERR_USERNOTINCHANNEL(client, *it, targetChannel));
            continue;
        }

        _broadcastKickedMember(client, member->getClientInfo(), targetChannel);
        targetChannel->removeMember(member->getClientInfo()->getNickname());
    }
}

void KICK::_broadcastKickedMember(Client *source, Client *member, Channel *channel)
{
    Channel::MemberTable members = channel->getMembers();
    Client *target;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = members.begin(); it != members.end(); it++)
    {
        target = it->second->getClientInfo();
        server.subscribeEvent(target->getFd(), POLLOUT);
        target->enqueueBuffer(SEND, RPL_KICK(source, channel, member->getNickname(), _reason));
    }
}