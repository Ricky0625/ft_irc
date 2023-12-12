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

    if (target == NULL)
        return;

    if (getArgs().size() < 1)
        target->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(target, _ircMsg.command));
    else if (target->isAuthenticated() && target->isRegistered())
        target->enqueueBuffer(SEND, ERR_ALREADYREGISTERED(target));
    else if (!server.isCorrectPassword(_password))
        target->enqueueBuffer(SEND, ERR_PASSWDMISMATCH(target));
    else
        target->setAuthenticated(true);
}
