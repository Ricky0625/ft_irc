#include "PRIVMSG.hpp"

// default constructor
PRIVMSG::PRIVMSG() : _msg(""), _hasMsg(false) {}

void PRIVMSG::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    _targets.clear();

    if (args.empty())
        return;

    if (argCount > 0)
        Parser::splitStr(args[0], _targets, ",", INCLUDE_EMPTY);

    // /PRIVMSG <targets> <msg>
    if (argCount > 1)
    {
        _msg = args[1];
        _hasMsg = true;
        return;
    }

    // /PRIVMSG <targets> :the message
    if (getHasTrailing())
    {
        _msg = getTrailing();
        _hasMsg = true;
    }
}

void PRIVMSG::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Client *recipient;
    Channel *targetChannel;
    ChannelMember *member;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;

    if (_targets.empty())
    {
        client->enqueueBuffer(SEND, ERR_NORECIPIENT(client, _ircMsg.command));
        return;
    }

    if (_hasMsg == false)
    {
        client->enqueueBuffer(SEND, ERR_NOTEXTTOSEND(client));
        return;
    }

    for (size_t i = 0; i < _targets.size(); i++)
    {
        const std::string &target = _targets[i];
        recipient = server.getClientByNickname(target);
        targetChannel = server.getChannel(target);


        if (targetChannel)
        {
            member = targetChannel->getMember(client);
            if (member == NULL || (targetChannel->channelModes.hasMode('m') && !(member->memberMode.hasMode('o') || member->memberMode.hasMode('v'))))
            {
                client->enqueueBuffer(SEND, ERR_CANNOTSENDTOCHAN(client, targetChannel->getName()));
                continue;
            }
            _broadcastMessage(client, targetChannel);
        }
        else if (recipient)
        {
            server.subscribeEvent(recipient->getFd(), POLLOUT);
            recipient->enqueueBuffer(SEND, RPL_MSG(client, recipient->getNickname(), _msg));
        }
        else
            client->enqueueBuffer(SEND, ERR_NOSUCHNICK(client, target));
    }
}

void PRIVMSG::_broadcastMessage(Client *client, Channel *channel)
{
    Channel::MemberTable members = channel->getMembers();
    Client *target;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = members.begin(); it != members.end(); it++)
    {
        target = it->second->getClientInfo();

        if (target->getNickname() == client->getNickname())
            continue;
        server.subscribeEvent(target->getFd(), POLLOUT);
        target->enqueueBuffer(SEND, RPL_MSG(client, channel->getName(), _msg));
    }
}
