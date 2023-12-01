#ifndef REPLYNUMERIC_H
#define REPLYNUMERIC_H

#include "irclib.h"
#include "Client.hpp"

class Client;
// welcome
std::string RPL_WELCOME(Client *client);
std::string RPL_YOURHOST();
std::string RPL_CREATED();
std::string RPL_MYINFO();
std::string RPL_ISUPPORT();

// ERRORS
std::string ERR_NEEDMOREPARAMS(Client *client, const std::string &command);
std::string ERR_ALREADYREGISTERED(Client *client);
std::string ERR_PASSWDMISMATCH(Client *client);
std::string ERR_NONICKNAMEGIVEN(Client *client);
std::string ERR_ERRONEUSNICKNAME(Client *client, const std::string &newNick);
std::string ERR_NICKNAMEINUSE(Client *client, const std::string &newNick);

#endif
