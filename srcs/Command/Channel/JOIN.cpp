#include "JOIN.hpp"

// default constructor
JOIN::JOIN() {}

void JOIN::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    _chanNames.clear();
    _chanPass.clear();

    if (argCount == 0)
        return;

    // normal parameters
    // JOIN <channel>{,<channel>} [<key>{,<key>}]
    if (argCount > 0)
        Parser::splitStr(args[0], _chanNames, ",", INCLUDE_EMPTY);

    if (argCount > 1)
        Parser::splitStr(args[1], _chanPass, ",", INCLUDE_EMPTY);
}

/**
 * @brief numeric replies
 * @details
 * ERR_TOOMANYCHANNELS (405) (ISUPPORT)
 * ERR_BADCHANNELKEY (475)
 * ERR_BANNEDFROMCHAN (474)
 * ERR_CHANNELISFULL (471) (ISUPPORT)
 * ERR_INVITEONLYCHAN (473) (ISUPPORT)
 *
 * NOTE: not doing alt params
 *
 * @flow
 * If the JOIN command is successful, the server MUST send, in this order
 * 1. A JOIN message with the client as the message <source> and the channel they have joined as the first parameter of the message.
 * 2. The channel's topic (332 and optionally 333), and no message if channel does not have a topic
 * 3. A list of users currently joined to the channel (with one or more 353 followed by a single 366).
 *    MUST include the requesting client that has just joined the channel.
 */

void JOIN::execute(int clientFd)
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

    // if the given password list size is not 0 and not the same size as the channel name list
    if (_chanPass.size() != 0 && _chanPass.size() != _chanNames.size())
        return;

    for (size_t i = 0; i < _chanNames.size(); i++)
    {
        const std::string &channelName = _chanNames[i];
        const std::string &channelPass = _chanPass.size() == 0 ? "" : _chanPass[i];

        if (channelName[0] != '#')
        {
            client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, channelName));
            continue;
        }

        targetChannel = server.getChannel(channelName);
        if (targetChannel == NULL)
            targetChannel = server.addNewChannel(channelName);

        // if client-limit mode is enabled
        if (targetChannel->channelModes.hasMode('l') && targetChannel->getMemberTotal() >= targetChannel->getMemberLimit())
        {
            client->enqueueBuffer(SEND, ERR_CHANNELISFULL(client, targetChannel->getName()));
            continue;
        }

        // if invite-only mode is enabled
        // if (targetChannel->channelModes.hasMode('i'))
        // {
        //     client->enqueueBuffer(SEND, ERR_INVITEONLYCHAN(client, targetChannel->getName()));
        //     continue;
        // }

        // if key mode is enabled
        if (targetChannel->channelModes.hasMode('k') && targetChannel->isCorrectPassword(channelPass) == false)
        {
            client->enqueueBuffer(SEND, ERR_BAD_CHANNELKEY(client, targetChannel->getName()));
            continue;
        }

        if (targetChannel->addMember(client))
        {
            _broadcastNewMember(client, targetChannel);
            queueJoinWelcomeMessage(client, targetChannel);
        }
    }
}

void JOIN::_broadcastNewMember(Client *newMember, Channel *channel)
{
    Channel::MemberTable members = channel->getMembers();
    Client *target;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = members.begin(); it != members.end(); it++)
    {
        target = it->second->getClientInfo();
        server.subscribeEvent(target->getFd(), POLLOUT);
        target->enqueueBuffer(SEND, RPL_JOIN(newMember, channel));
    }
}
