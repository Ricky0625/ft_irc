#ifndef REPLYNUMERIC_H
#define REPLYNUMERIC_H

#include "irclib.h"
#include "Client.hpp"

class Client;

// command replies
std::string RPL_NICK(Client *client, const std::string &oldNickname);
std::string RPL_PONG(Client *client, const std::string &oldNickname, const std::string &token);


// welcome
std::string RPL_WELCOME(Client *client);
std::string RPL_YOURHOST(Client *client);
std::string RPL_CREATED(Client *client, const std::string &upTime);
std::string RPL_MYINFO(Client *client);
std::string RPL_ISUPPORT();

// ERRORS
std::string ERR_NEEDMOREPARAMS(Client *client, const std::string &command);
std::string ERR_ALREADYREGISTERED(Client *client);
std::string ERR_PASSWDMISMATCH(Client *client);
std::string ERR_NONICKNAMEGIVEN(Client *client);
std::string ERR_ERRONEUSNICKNAME(Client *client, const std::string &newNick);
std::string ERR_NICKNAMEINUSE(Client *client, const std::string &newNick);

#endif
