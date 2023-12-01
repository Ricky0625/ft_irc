#include "NICK.hpp"

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

    if (target == NULL || !target->isAuthenticated())
        return;

    if (_nickname.empty())
        target->queueBuffer(SEND, ERR_NONICKNAMEGIVEN(target));
    else if (!_isValidNickname(_nickname)) // if not valid
        target->queueBuffer(SEND, ERR_ERRONEUSNICKNAME(target, _nickname));
    else if (server.isNicknameTaken(_nickname)) // if taken
        target->queueBuffer(SEND, ERR_NICKNAMEINUSE(target, _nickname));
    else
        target->setNickname(_nickname); // set nickname here. not sure if i need to send back reply

    // check if registration is completed here
    if (target->isRegistered())
    {
        target->queueBuffer(SEND, RPL_WELCOME(target));
    }
}

bool NICK::_isValidNickname(const std::string &newNick) const
{
    char firstChar = newNick[0];

    if (isdigit(firstChar) || firstChar == ':' || firstChar == '#' || firstChar == '&' || newNick.find(' ') != std::string::npos)
        return false;
    return true;
}
