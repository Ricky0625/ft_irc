#ifndef BOTREPLY_HPP
#define BOTREPLY_HPP

#include "botlib.h"

std::string BOTRPL_PASS(const std::string &pass);
std::string BOTRPL_NICK(const std::string &nick);
std::string BOTRPL_USER(const std::string &username, const std::string &realname);
std::string BOTRPL_LIST(void);
std::string BOTRPL_JOIN(const std::string &channels);
std::string BOTRPL_PRIVMSG(const std::string &channelName, const std::string &msg);
std::string BOTRPL_QUIT(void);

#endif