#include "PASS.hpp"

// default constructor
PASS::PASS() {}

void PASS::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();

    if (args.empty())
        return;
    
    // map each parameters to corresponding variable
    _password = args[0];
}

void PASS::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);

    if (target == NULL) return;

    if (getArgs().size() < 1)
    {
        target->queueBuffer(SEND, ERR_NEEDMOREPARAMS(target, _ircMsg.command));
        return;
    }

    if (target->isRegistered())
    {
        target->queueBuffer(SEND, ERR_ALREADYREGISTERED(target));
        return;
    }

    if (!server.isCorrectPassword(_password))
    {
        target->queueBuffer(SEND, ERR_PASSWDMISMATCH(target));
        return;
    }

    // if successful
    // target->setRegistered(true);
}
