#include "USER.hpp"

// default constructor
USER::USER() : _username(""), _realname("") {}

void USER::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    // USER username 0 * realname. ignore `0` & `*`
    // USER username 0 * :realname with space
    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    if (argCount == 0)
        return;

    if (argCount > 0)
        _username = args[0];

    if (argCount > 3)
        _realname = args[3];
    else
        _realname = ircMsg.trailing;
}

void USER::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL || !target->isAuthenticated())
        return;
    
    if (target->isRegistered()) // if already registered
        target->queueBuffer(SEND, ERR_ALREADYREGISTERED(target));
    else if (getArgs().size() < 4)
        target->queueBuffer(SEND, ERR_NEEDMOREPARAMS(target, _ircMsg.command));
    else
    {
        target->setUsername(_username);
        target->setRealname(_realname);
    }

    if (target->isRegistered()) // if after setting username and realname and consider as registered
    {
        target->queueBuffer(SEND, RPL_WELCOME(target));
    }
}
