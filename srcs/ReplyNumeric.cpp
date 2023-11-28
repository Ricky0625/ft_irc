#include "ReplyNumeric.hpp"

std::string ErrorMessage(const std::string &errorCode, Client *client, const std::string &command, const std::string &message)
{
    // add nickname before command
    const std::string &nickname = client->getNickname();

    return std::string(SERVER_PREFIX) + " " + errorCode + " " + (nickname.empty() ? "*" : nickname) + (command.empty() ? "" : " ") + command + " :" + message + CRLF;
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
