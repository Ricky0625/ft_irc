#include "MODE.hpp"

// default constructor
MODE::MODE(void) : _target(""), _modeString("") {}

void MODE::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const Args &args = getArgs();
    size_t argCount = args.size();

    _modeArgs.clear();

    if (args.empty())
        return;

    _target = args[0];

    if (argCount > 1)
        _modeString = args[1];

    if (argCount > 2)
        _modeArgs.assign(args.begin() + 2, args.end());
}

void MODE::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);

    /**
     * TODO: at least handle MODE <target> first.
     */

    if (getArgs().size() < 1)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, _ircMsg.command));
        return;
    }

    if (_target[0] == '#')
    {
        // handle channel related queries
    }
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
    (void)client;
}
