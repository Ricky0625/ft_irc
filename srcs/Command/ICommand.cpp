#include "ICommand.hpp"

// destructor
ICommand::~ICommand() {}

/**
 * @brief Initialize the concrete command object
 */
void ICommand::initialize(Server &server, const IRCMessage &ircMsg)
{
    _server = &server;
    _ircMsg = ircMsg;
}

/**
 * @getter
 * @brief get prefix
 */
std::string ICommand::getPrefix() const
{
    return _ircMsg.prefix;
}

/**
 * @getter
 * @brief get command name
 */
std::string ICommand::getCommand() const
{
    return _ircMsg.command;
}

/**
 * @getter
 * @brief get argument list
 */
std::vector<std::string> ICommand::getArgs() const
{
    return _ircMsg.arguments;
}

bool ICommand::getHasTrailing() const
{
    return _ircMsg.hasTrailing;
}

/**
 * @getter
 * @brief get trailing
 */
std::string ICommand::getTrailing() const
{
    return _ircMsg.trailing;
}

IRCMessage ICommand::getIRCMsg() const
{
    return _ircMsg;
}

void ICommand::queueWelcomeMessage(Client *client, const std::string &upTime)
{
    client->enqueueBuffer(SEND, RPL_WELCOME(client));
    client->enqueueBuffer(SEND, RPL_YOURHOST(client));
    client->enqueueBuffer(SEND, RPL_CREATED(client, upTime));
    client->enqueueBuffer(SEND, RPL_MYINFO(client));
    /**
     * TODO:
     * 1. complete RPL_MYINFO
     * 2. complete RPL_MYINFO
     */
    queueMOTD(client);
}

void ICommand::queueJoinWelcomeMessage(Client *client, Channel *channel)
{
    if (channel->getTopic().empty() == false)
    {
        client->enqueueBuffer(SEND, RPL_TOPIC(client, channel));
        client->enqueueBuffer(SEND, RPL_TOPICWHOTIME(client, channel));
    }
    client->enqueueBuffer(SEND, RPL_NAMREPLY(client, channel));
    client->enqueueBuffer(SEND, RPL_ENDOFNAMES(client, channel->getName()));
}

void ICommand::queueMOTD(Client *client)
{
    std::ifstream data;
    std::string line;

    data.open(MOTD_PATH);
    if (data.is_open() == false)
    {
        client->enqueueBuffer(SEND, ERR_NOMOTD(client));
        return;
    }

    client->enqueueBuffer(SEND, RPL_MOTDSTART(client));
    while (std::getline(data, line))
        client->enqueueBuffer(SEND, RPL_MOTD(client, line)); // motd content
    client->enqueueBuffer(SEND, RPL_ENDOFMOTD(client));
}
