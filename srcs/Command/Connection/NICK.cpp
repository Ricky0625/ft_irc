#include "Connection/NICK.hpp"

// default constructor
NICK::NICK() : _nickname("") {}

void NICK::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();

    if (args.empty())
        return;
    
    _nickname = args[0];
}

void NICK::execute(int clientFd)
{
    Server &server = *_server;
    Client *target = server.getClient(clientFd);
    std::string oldNickname;

    if (target == NULL || !target->isAuthenticated())
        return;

    if (_nickname.empty())
        target->enqueueBuffer(SEND, ERR_NONICKNAMEGIVEN(target));
    else if (!_isValidNickname(_nickname)) // if not valid
        target->enqueueBuffer(SEND, ERR_ERRONEUSNICKNAME(target, _nickname));
    else if (server.isNicknameTaken(_nickname)) // if taken
        target->enqueueBuffer(SEND, ERR_NICKNAMEINUSE(target, _nickname));
    else
    {
        oldNickname = target->getNickname();
        target->setNickname(_nickname);
        target->enqueueBuffer(SEND, RPL_NICK(target, oldNickname));
    }

    if (target->isWelcomeRequired())
        queueWelcomeMessage(target, server.getUpTime());
}

bool NICK::_isValidNickname(const std::string &newNick) const
{
    char firstChar = newNick[0];

    if (isdigit(firstChar) || firstChar == ':' || firstChar == '#' || firstChar == '&' || newNick.find(' ') != std::string::npos)
        return false;
    return true;
}
