#include "ReplyNumeric.hpp"

// 001
std::string RPL_WELCOME(Client *client)
{
    std::string welcomeMsg = "Welcome to the " + std::string(SERVER_NAME) + " Internet Relay Chat Network, " + client->getNickname();
    return SimpleMessage("001", client, welcomeMsg);
}

// 002
std::string RPL_YOURHOST(Client *client)
{
    std::string hostMsg = "Your host is " + std::string(SERVER_NAME) + ", running version " + std::string(VERSION);
    return SimpleMessage("002", client, hostMsg);
}

// 003
std::string RPL_CREATED(Client *client, const std::string &upTime)
{
    /**
     * TODO:
     * 1. Available user modes
     * 2. Available channel modes
     */
    std::string createdMsg = "This server was created " + upTime;
    return SimpleMessage("003", client, createdMsg);
}

// 004
std::string RPL_MYINFO(Client *client)
{
    std::string nickname = client->getNickname();

    return ":" + std::string(HOST) + " 004 " + (nickname.empty() ? "*" : nickname) + " " + std::string(HOST) + " " + std::string(VERSION) + " io kost k" + CRLF;
}

// 321
std::string RPL_LISTSTART(Client *client)
{
    return SourceMessage("321", client) + " Channel :Users  Name" + CRLF;
}

// 322
std::string RPL_LIST(Client *client, Channel *channel)
{
    return SourceMessage("322", client) + " " + channel->getName() + " " + std::to_string(channel->getMemberTotal()) + MessageTrailing(channel->getTopic());
}

// 323
std::string RPL_LISTEND(Client *client)
{
    return SourceMessage("323", client) + MessageTrailing(":End of /LIST");
}

// 331
std::string RPL_NOTOPIC(Client *client, Channel *channel)
{
    return ChannelSimpleMessage("331", client, channel->getName(), "No topic is set");
}

// 332
std::string RPL_TOPIC(Client *client, Channel *channel)
{
    return ChannelSimpleMessage("332", client, channel->getName(), channel->getTopic());
}

// 333
std::string RPL_TOPICWHOTIME(Client *client, Channel *channel)
{
    return SourceMessage("333", client) + " " + channel->getName() + " " + channel->getTopicSetBy() + " " + channel->getTopicSetAt() + CRLF;
}

// 353
std::string RPL_NAMREPLY(Client *client, Channel *channel)
{
    /**
     * TODO: channel symbol '='
     */
    return SourceMessage("353", client) + " = " + channel->getName() + MessageTrailing(channel->getAllMembersAsString());
}

// 366
std::string RPL_ENDOFNAMES(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("366", client, channelName, "End of /NAMES list");
}

// 372
std::string RPL_MOTD(Client *client, const std::string &motdLine)
{
    return SourceMessage("372", client) + MessageTrailing(motdLine);
}

// 375
std::string RPL_MOTDSTART(Client *client)
{
    return SourceMessage("375", client) + MessageTrailing("- " + std::string(HOST) + " Message of the day - ");
}

// 376
std::string RPL_ENDOFMOTD(Client *client)
{
    return SourceMessage("376", client) + MessageTrailing("End of /MOTD command.");
}
