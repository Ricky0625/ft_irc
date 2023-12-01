#include "ReplyNumeric.hpp"

static std::string ClientMessage(const std::string &code, Client *client)
{
    const std::string &nickname = client->getNickname();

    return ":" + std::string(SERVER_NAME) + " " + code + " " + (nickname.empty() ? "*" : nickname); // does not have space at the end!
}

static std::string MessageTrailing(const std::string &trailing)
{
    return " :" + trailing + CRLF;
}

static std::string SimpleMessage(const std::string &code, Client *client, const std::string &message)
{
    return ClientMessage(code, client) + MessageTrailing(message);
}

static std::string ErrorMessage(const std::string &code, Client *client, const std::string &command, const std::string &message)
{
    // add nickname before command
    const std::string &nickname = client->getNickname();

    return ClientMessage(code, client) + (nickname.empty() ? "*" : nickname) + (command.empty() ? "" : " ") + command + MessageTrailing(message);
}

static std::string NicknameError(const std::string &code, Client *client, const std::string &newNickname, const std::string &message)
{
    return ClientMessage(code, client) + " " + newNickname + " " + MessageTrailing(message);
}

// 001
std::string RPL_WELCOME(Client *client)
{
    std::string welcomeMsg = "Welcome to the " + std::string(SERVER_NAME) + " Internet Relay Chat Network, " + client->getNickname();
    return SimpleMessage("001", client, welcomeMsg);
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
