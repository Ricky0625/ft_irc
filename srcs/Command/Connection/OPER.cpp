#include "OPER.hpp"

OPER::OPER() :_name(""), _password("") {}

void OPER::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    if (argCount == 0)
        return;

    _name = args[0];
    _password = args[1];
}

/**
 * @brief Request to be network operator
*/
void OPER::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;
    
    if (getArgs().size() < 2)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, _ircMsg.command));
        return;
    }

    if (client->networkMode.hasMode('o'))
        return;

    try {
        std::string operatorName = server.getValueFromSection("OPERATOR", "name");
        std::string operatorPassword = server.getValueFromSection("OPERATOR", "password");
        
        if (_name != operatorName || _password != operatorPassword)
        {
            client->enqueueBuffer(SEND, ERR_PASSWDMISMATCH(client));
            return;
        }

        client->enqueueBuffer(SEND, RPL_YOUREOPER(client));
        client->networkMode.addMode('o'); // add operator mode
        client->enqueueBuffer(SEND, RPL_MODE(client, client->getNickname(), "+o", ""));
    }
    catch (const std::exception &ex) {
        (void)ex;
        client->enqueueBuffer(SEND, ERR_NOOPERHOST(client));
    }
}
