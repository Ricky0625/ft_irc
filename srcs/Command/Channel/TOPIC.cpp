#include "TOPIC.hpp"

// default constructor
TOPIC::TOPIC() : _channel(""), _msg(""), _hasMsg(false) {}

void TOPIC::initialize(Server &server, const IRCMessage &ircMsg)
{
    ICommand::initialize(server, ircMsg);

    const std::vector<std::string> &args = getArgs();
    size_t argCount = args.size();

    if (argCount == 0)
        return;

    if (argCount > 0)
        _channel = args[0];

    if (argCount > 1)
    {
        _hasMsg = true;
        _msg = args[1];
        return;
    }

    _hasMsg = getHasTrailing();
    if (_hasMsg)
        _msg = getTrailing();
}

/**
 * ERR_NEEDMOREPARAMS (461) DONE
 * ERR_NOSUCHCHANNEL (403) DONE
 * ERR_NOTONCHANNEL (442) DONE
 * ERR_CHANOPRIVSNEEDED (482) (ISUPPORT)
 * RPL_NOTOPIC (331)
 * RPL_TOPIC (332)
 * RPL_TOPICWHOTIME (333)
*/
void TOPIC::execute(int clientFd)
{
    Server &server = *_server;
    Client *client = server.getClient(clientFd);
    Channel *targetChannel;

    if (client == NULL || !(client->isAuthenticated() && client->isRegistered()))
        return;
    
    if (getArgs().size() < 1)
    {
        client->enqueueBuffer(SEND, ERR_NEEDMOREPARAMS(client, "TOPIC"));
        return;
    }

    targetChannel = server.getChannel(_channel);
    if (targetChannel == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOSUCHCHANNEL(client, _channel));
        return;
    }

    if (targetChannel->getMember(client) == NULL)
    {
        client->enqueueBuffer(SEND, ERR_NOTONCHANNEL(client, targetChannel));
        return;
    }

    // if has message, update TOPIC
    if (_hasMsg)
    {
        targetChannel->setTopic(_msg);
        targetChannel->setTopicSetBy(client->getNickname());
        targetChannel->updateTopicSetAt();
    }

    if (targetChannel->getTopicSetBy().empty() && targetChannel->getTopicSetAt().empty())
    {
        client->enqueueBuffer(SEND, RPL_NOTOPIC(client, targetChannel));
        return;
    }

    client->enqueueBuffer(SEND, RPL_TOPIC(client, targetChannel));
    client->enqueueBuffer(SEND, RPL_TOPICWHOTIME(client, targetChannel)); 
}
