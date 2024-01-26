#include "MODE.hpp"

// default constructor
MODE::MODE(void) : _target(""), _modeString(""), _currentModeArgsIndex(0) {}

void MODE::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const Args &args = getArgs();
    size_t argCount = args.size();

    _modeArgs.clear();

    if (args.empty())
        return;

    _target = args[0];

    _currentModeArgsIndex = 0;

    if (argCount > 1)
        _modeString = args[1];

    if (argCount > 2)
        _modeArgs.assign(args.begin() + 2, args.end());
}

void MODE::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);

    if (getArgs().size() < 1)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, _ircMsg.command));
        return;
    }

    if (_target[0] == '#')
        _handleChannelQuery(client);
    else
        _handleUserQuery(client);
}

void MODE::_handleUserQuery(Client *client)
{
    Server &server = *_server;
    size_t argCount = getArgs().size();

    if (server.isNicknameTaken(_target) == false)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHNICK(client, _target));
        return;
    }

    if (_target != client->getNickname())
    {
        client->enqueueBuffer(SEND, ERR_USERSDONTMATCH(client));
        return;
    }

    if (argCount == 1) // MODE <target>
        client->enqueueBuffer(SEND, RPL_UMODEIS(client));
    else if (argCount > 1) // MODE <target> <modestring>
        _appplyUserMode(client);
}

void MODE::_appplyUserMode(Client *client)
{
    Server &server = *_server;
    ModeApplyAction action = NOACTION;

    for (size_t i = 0; i < _modeString.size(); i++)
    {
        char &mode = _modeString[i];

        if (mode == '+')
        {
            action = ADDMODE;
            continue;
        }
        else if (mode == '-')
        {
            action = REMOVEMODE;
            continue;
        }
        else if (action == NOACTION)
            continue;

        // avoid user get network operator access via MODE
        if (mode == 'o' && action == ADDMODE)
            continue;

        if (server.isSupportedMode(mode, Server::USERMODE))
        {
            std::string modeChanged = "";
            if (action == ADDMODE && client->networkMode.hasMode(mode) == false)
                client->networkMode.addMode(mode);
            else if (action == REMOVEMODE && client->networkMode.hasMode(mode) == true)
                client->networkMode.removeMode(mode);
            else
                continue;
            modeChanged += (action == ADDMODE ? "+" : "-") + std::string(1, mode);
            client->enqueueBuffer(SEND, RPL_MODE(client, _target, modeChanged, ""));
        }
        else
            client->enqueueBuffer(SEND, ERR_UMODEUNKNOWNFLAG(client));
    }
}

void MODE::_handleChannelQuery(Client *client)
{
    Server &server = *_server;
    Channel *channel = server.getChannel(_target);
    ChannelMember *sender = channel->getMember(client);
    size_t argCount = getArgs().size();

    // don't know what to do here, documentation didn't mention anything
    if (sender == NULL)
        return;

    if (channel == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, _target));
        return;
    }

    if (argCount == 1) // MODE <target>
        client->enqueueBuffer(SEND, RPL_CHANNELMODEIS(client, channel));
    else if (argCount > 1)
    {
        // if sender is not a chanop
        if (sender->memberMode.hasMode('o') == false)
        {
            client->enqueueBuffer(SEND, ERR_CHANOPRIVSNEEDED(client, channel->getName()));
            return;
        }
        _applyChannelMode(client, channel);
    }
}

void MODE::_applyChannelMode(Client *client, Channel *channel)
{
    Server &server = *_server;
    ModeApplyAction action = NOACTION;

    for (size_t i = 0; i < _modeString.size(); i++)
    {
        char &mode = _modeString[i];

        if (mode == '+')
        {
            action = ADDMODE;
            continue;
        }
        else if (mode == '-')
        {
            action = REMOVEMODE;
            continue;
        }
        else if (action == NOACTION)
            continue;

        // to update membership (hardcode)
        if (mode == 'o' || mode == 'v')
            _updateMemberMembership(client, channel, action, mode);
        else if (server.isSupportedMode(mode, Server::CHANNELMODE))
            _changeChannelSettings(client, channel, action, mode);
        else
            client->enqueueBuffer(SEND, ERR_UMODEUNKNOWNFLAG(client));
    }
}

void MODE::_changeChannelSettings(Client *client, Channel *channel, ModeApplyAction action, char mode)
{
    // the logic here are the modes that this IRC server implements
    /**
     * l - client limit (JOIN), got argument
     * k - key channel (JOIN), got argument
     * i - invite only (JOIN)
     * m - moderated channel (PRIVMSG)
     * s - secret channel (LIST)
     * t - topic-locked (TOPIC)
     *
     * update membership
     * v - voice member (PRIVMSG)
     * o - chanops (MODE)
     */

    switch (mode)
    {
    case 'l':
        _setClientLimitMode(client, channel, action, mode);
        break;

    case 'k':
        _setKeyMode(client, channel, action, mode);
        break;

    case 'i':
        _performModeAction(client, channel, action, mode);
        break;

    case 'm':
        _performModeAction(client, channel, action, mode);
        break;

    case 's':
        _performModeAction(client, channel, action, mode);
        break;

    case 't':
        _performModeAction(client, channel, action, mode);
        break;

    default:
        break;
    }
}

void MODE::_updateMemberMembership(Client *client, Channel *channel, ModeApplyAction action, char mode)
{
    std::string modeChanged = "";
    std::string target;
    ChannelMember *member;

    if (_modeArgs.size() == 0 || (_currentModeArgsIndex + 1) > _modeArgs.size())
        return;

    target = _modeArgs[_currentModeArgsIndex++];
    member = channel->getMemberUsingNickname(target);

    if (member == NULL)
        return;

    if (action == ADDMODE && member->memberMode.hasMode(mode) == false)
        member->memberMode.addMode(mode);
    else if (action == REMOVEMODE && member->memberMode.hasMode(mode))
        member->memberMode.removeMode(mode);
    else
        return;
    
    modeChanged += (action == ADDMODE ? "+" : "-") + std::string(1, mode);
    _broadcastModeUpdate(client, channel, modeChanged, target);
}

void MODE::_setClientLimitMode(Client *client, Channel *channel, ModeApplyAction action, char mode)
{
    std::string modeChanged = "";
    std::string arg;

    if (action == ADDMODE)
    {
        if (_modeArgs.size() == 0 || (_currentModeArgsIndex + 1) > _modeArgs.size())
            return;

        arg = _modeArgs[_currentModeArgsIndex++];

        if (Parser::isStringAllDigits(arg) == false)
            return;

        int limit = std::atoi(arg.c_str());
        channel->setMemberLimit(limit);
        channel->channelModes.addMode(mode);
    }
    else if (action == REMOVEMODE && channel->channelModes.hasMode(mode) == true)
    {
        channel->setMemberLimit(0);
        channel->channelModes.removeMode(mode);
    }
    else
        return;

    modeChanged += (action == ADDMODE ? "+" : "-") + std::string(1, mode);
    _broadcastModeUpdate(client, channel, modeChanged, arg);
}

void MODE::_setKeyMode(Client *client, Channel *channel, ModeApplyAction action, char mode)
{
    std::string modeChanged = "";

    if (action == ADDMODE)
    {
        if (_modeArgs.size() == 0 || (_currentModeArgsIndex + 1) > _modeArgs.size())
            return;

        channel->setPassword(_modeArgs[_currentModeArgsIndex++]);
        channel->channelModes.addMode(mode);
    }
    else if (action == REMOVEMODE && channel->channelModes.hasMode(mode) == true)
    {
        channel->setPassword("");
        channel->channelModes.removeMode(mode);
    }
    else
        return;

    modeChanged += (action == ADDMODE ? "+" : "-") + std::string(1, mode);
    _broadcastModeUpdate(client, channel, modeChanged, "");
}

void MODE::_performModeAction(Client *client, Channel *channel, ModeApplyAction action, char mode)
{
    std::string modeChanged = "";

    if (action == ADDMODE && channel->channelModes.hasMode(mode) == false)
        channel->channelModes.addMode(mode);
    else if (action == REMOVEMODE && channel->channelModes.hasMode(mode) == true)
        channel->channelModes.removeMode(mode);
    else
        return;

    modeChanged += (action == ADDMODE ? "+" : "-") + std::string(1, mode);
    _broadcastModeUpdate(client, channel, modeChanged, "");
}

void MODE::_broadcastModeUpdate(Client *client, Channel *channel, const std::string &modeChanged, const std::string &arg)
{
    Channel::MemberTable members = channel->getMembers();
    Client *target;
    Server &server = *_server;

    for (Channel::MemberTable::iterator it = members.begin(); it != members.end(); it++)
    {
        target = it->second->getClientInfo();
        server.subscribeEvent(target->getFd(), POLLOUT);
        target->enqueueBuffer(SEND, RPL_MODE(client, _target, modeChanged, arg));
    }
}
