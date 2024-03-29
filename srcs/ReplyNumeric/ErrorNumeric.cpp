#include "ReplyNumeric.hpp"

// 401
std::string ERR_NOSUCHNICK(Client *client, const std::string &nickname)
{
    return SourceMessage("401", client) + " " + nickname + MessageTrailing("No such nick/channel");
}

// 403
std::string ERR_NOSUCHCHANNEL(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("403", client, channelName, "No such channel");
}

// 404
std::string ERR_CANNOTSENDTOCHAN(Client *client, const std::string &channelName)
{
    return SourceMessage("404", client) + " " + channelName + MessageTrailing("Cannot send to channel");
}

// 405

// 411
std::string ERR_NORECIPIENT(Client *client, const std::string &command)
{
    return SourceMessage("411", client) + MessageTrailing("No recipient given (" + command + ")");
}

// 412
std::string ERR_NOTEXTTOSEND(Client *client)
{
    return SourceMessage("412", client) + MessageTrailing("No text to send");
}

// 421
std::string ERR_UNKNOWNCOMMAND(Client *client, const std::string &command)
{
    return SourceMessage("421", client) + " " + command + MessageTrailing("Unknown command");
}

// 422
std::string ERR_NOMOTD(Client *client)
{
    return SourceMessage("422", client) + MessageTrailing("MOTD File is missing");
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

// 441
std::string ERR_USERNOTINCHANNEL(Client *client, const std::string &nickname, Channel *channel)
{
    return SourceMessage("441", client) + " " + channel->getName() + " " + nickname + ":They are not on that channel" + CRLF;
}

// 442
std::string ERR_NOTONCHANNEL(Client *client, Channel *channel)
{
    return ChannelSimpleMessage("442", client, channel->getName(), "You're not on that channel");
}

// 443
std::string ERR_USERONCHANNEL(Client *client, const std::string &nickname, Channel *channel)
{
    return SourceMessage("443", client) + " " + nickname + " " + channel->getName() + ":is already on channel" + CRLF;
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

// 471
std::string ERR_CHANNELISFULL(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("471", client, channelName, "Cannot join channel (+l)");
}

// 473
std::string ERR_INVITEONLYCHAN(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("473", client, channelName, "Cannot join channel (+i)");
}

// 475
std::string ERR_BAD_CHANNELKEY(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("475", client, channelName, "Cannot join channel (+k)");
}

// 482
std::string ERR_CHANOPRIVSNEEDED(Client *client, const std::string &channelName)
{
    return ChannelSimpleMessage("482", client, channelName, "You're not channel operator");
}

// 491
std::string ERR_NOOPERHOST(Client *client)
{
    return ErrorMessage("491", client, "", "No O-lines for your host");
}

// 501
std::string ERR_UMODEUNKNOWNFLAG(Client *client)
{
    return ErrorMessage("501", client, "", "Unknown MODE flag");
}

// 502
std::string ERR_USERSDONTMATCH(Client *client)
{
    return ErrorMessage("502", client, "", "Cant change mode for other users");
}
