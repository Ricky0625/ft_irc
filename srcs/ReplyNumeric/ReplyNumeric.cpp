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

// 403
std::string ERR_NOSUCHCHANNEL(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("403", client, channelName, "No such channel");
}

// 431
std::string ERR_NONICKNAMEGIVEN(Client *client)
{
    return SimpleMessage("431", client, "No nickname given");
}

// 432
std::string ERR_ERRONEUSNICKNAME(Client *client, const std::string &newNick)
{
    return NicknameError("432", client, newNick, "Erroneus nickname");
}

// 433
std::string ERR_NICKNAMEINUSE(Client *client, const std::string &newNick)
{
    return NicknameError("433", client, newNick, "Nickname is already in use");
}

// 442
std::string ERR_NOTONCHANNEL(Client *client, Channel *channel)
{
    return ChannelSimpleMessage("442", client, channel->getName(), "You're not on that channel");
}

// 461
std::string ERR_NEEDMOREPARAMS(Client *client, const std::string &command)
{
    return ErrorMessage("461", client, command, "Not enough parameters");
}

// 462
std::string ERR_ALREADYREGISTERED(Client *client)
{
    return ErrorMessage("462", client, "", "You may not reregister");
}

// 464
std::string ERR_PASSWDMISMATCH(Client *client)
{
    return ErrorMessage("464", client, "", "Password incorrect");
}
