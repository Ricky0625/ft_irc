#include "INVITE.hpp"

// default constructor
INVITE::INVITE() : _nickname(""), _channel("") {}

void INVITE::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    if (argCount > 0)
        _nickname = args[0];

    if (argCount > 1)
        _channel = args[1];
}

/**
 * RPL_INVITING (341) DONE
 * ERR_NEEDMOREPARAMS (461) DONE
 * ERR_NOSUCHNICK (401) DONE
 * ERR_NOSUCHCHANNEL (403) DONE
 * ERR_NOTONCHANNEL (442) DONE
 * ERR_USERONCHANNEL (443) DONE
 * ERR_CHANOPRIVSNEEDED (482) DONE
*/
void INVITE::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Client *targetClient;
    Channel *targetChannel;
    ChannelMember *member;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;

    if (getArgs().size() < 2)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, "INVITE"));
        return;
    }

    targetChannel = server.getChannel(_channel);
    if (targetChannel == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, _channel));
        return;
    }

    targetClient = server.getClientByNickname(_nickname);
    if (targetClient == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHNICK(client, _nickname));
        return;
    }

    member = targetChannel->getMember(client);
    if (member == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOTONCHANNEL(client, targetChannel));
        return;
    }

    if (!member->memberMode.hasMode('o'))
    {
        client->enqueueBuffer(SEND, ERR_CHANOPRIVSNEEDED(client, _channel));
        return;
    }

    if (targetChannel->getMember(targetClient) != NULL)
    {
        client->enqueueBuffer(SEND, ERR_USERONCHANNEL(client, _nickname, targetChannel));
        return;
    }

    client->enqueueBuffer(SEND, RPL_INVITING(client, _nickname, targetChannel));
    server.subscribeEvent(targetClient->getFd(), POLLOUT);
    targetClient->enqueueBuffer(SEND, RPL_INVITE(client, _nickname, targetChannel));
    targetChannel->addInvitation(_nickname);
}