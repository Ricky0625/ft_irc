#include "NOTICE.hpp"

// default constructor
NOTICE::NOTICE() : _msg(""), _hasMsg(false) {}

void NOTICE::initialize(Server &server, const IRCMessage &ircMsg)
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

void NOTICE::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Client *recipient;
    Channel *targetChannel;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;

    if (_targets.empty())
        return;

    if (_hasMsg == false)
        return;

    for (size_t i = 0; i < _targets.size(); i++)
    {
        const std::string &target = _targets[i];
        recipient = server.getClientByNickname(target);
        targetChannel = server.getChannel(target);

        /**
         * TODO: refine the way to detect whether it's client or channel
         */

        if (targetChannel)
        {
            /**
             * TODO: ERR_CANNOTSENDTOCHAN
             *
             * NOTE:
             * 1. anyone can send a message using PRIVMSG to a channel as long
             *    as the sender is not on the ban list or the designated channel
             *    is not a moderated channel.
             */
            _broadcastMessage(client, targetChannel);
        }
        else if (recipient)
        {
            server.subscribeEvent(recipient->getFd(), POLLOUT);
            recipient->enqueueBuffer(SEND, RPL_MSG(client, recipient->getNickname(), _msg, NOTICE_MSG));
        }
    }
}

void NOTICE::_broadcastMessage(Client *client, Channel *channel)
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
        target->enqueueBuffer(SEND, RPL_MSG(client, channel->getName(), _msg, NOTICE_MSG));
    }
}