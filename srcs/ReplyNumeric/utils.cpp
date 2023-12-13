#include "ReplyNumeric.hpp"

std::string SourceMessage(const std::string &code, Client *client)
{
    const std::string &nickname = client->getNickname();

    return ":" + std::string(HOST) + " " + code + " " + (nickname.empty() ? "*" : nickname); // does not have space at the end!
}

std::string ClientHeader(Client *client, const std::string &oldNickname)
{
    return ":" + (oldNickname.empty() ? client->getNickname() : oldNickname) + "!" + client->getUsername() + "@" + std::string(HOST);
}

std::string MessageTrailing(const std::string &trailing)
{
    return " :" + trailing + CRLF;
}

std::string SimpleMessage(const std::string &code, Client *client, const std::string &message)
{
    return SourceMessage(code, client) + MessageTrailing(message);
}

std::string ChannelSimpleMessage(const std::string &code, Client *client, const std::string &channelName, const std::string &message)
{
    return SourceMessage(code, client) + " " + channelName + MessageTrailing(message);
}

std::string ErrorMessage(const std::string &code, Client *client, const std::string &command, const std::string &message)
{
    return SourceMessage(code, client) + (command.empty() ? "" : " ") + command + MessageTrailing(message);
}

std::string NicknameError(const std::string &code, Client *client, const std::string &newNickname, const std::string &message)
{
    return SourceMessage(code, client) + " " + newNickname + MessageTrailing(message);
}