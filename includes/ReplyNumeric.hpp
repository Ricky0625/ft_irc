#ifndef REPLYNUMERIC_H
#define REPLYNUMERIC_H

#include "irclib.h"
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

// utils
std::string SourceMessage(const std::string &code, Client *client);
std::string ClientHeader(Client *client, const std::string &oldNickname);
std::string MessageTrailing(const std::string &trailing);
std::string SimpleMessage(const std::string &code, Client *client, const std::string &message);
std::string ChannelSimpleMessage(const std::string &code, Client *client, const std::string &channelName, const std::string &message);
std::string ErrorMessage(const std::string &code, Client *client, const std::string &command, const std::string &message);
std::string NicknameError(const std::string &code, Client *client, const std::string &newNickname, const std::string &message);

// command replies
std::string RPL_NICK(Client *client, const std::string &oldNickname);
std::string RPL_PONG(Client *client, const std::string &oldNickname, const std::string &token);
std::string RPL_JOIN(Client *newMember, Channel *channel);
std::string RPL_PART(Client *client, Channel *channel, const std::string &reason);

// reply numerics
std::string RPL_WELCOME(Client *client);                                    // 001
std::string RPL_YOURHOST(Client *client);                                   // 002
std::string RPL_CREATED(Client *client, const std::string &upTime);         // 003
std::string RPL_MYINFO(Client *client);                                     // 004
std::string RPL_ISUPPORT();                                                 // 005
std::string RPL_NOTOPIC(Client *client, Channel *channel);                  // 331
std::string RPL_TOPIC(Client *client, Channel *channel);                    // 332
std::string RPL_TOPICWHOTIME(Client *client, Channel *channel);             // 333
std::string RPL_NAMREPLY(Client *client, Channel *channel);                 // 353
std::string RPL_ENDOFNAMES(Client *client, const std::string &channelName); // 366

// ERRORS
std::string ERR_NOSUCHCHANNEL(Client *client, const std::string &channelName); // 403
std::string ERR_TOOMANYCHANNELS(Client *client);                               // 405
std::string ERR_NONICKNAMEGIVEN(Client *client);                               // 431
std::string ERR_ERRONEUSNICKNAME(Client *client, const std::string &newNick);  // 432
std::string ERR_NICKNAMEINUSE(Client *client, const std::string &newNick);     // 433
std::string ERR_NOTONCHANNEL(Client *client, Channel *channel);                // 442
std::string ERR_NEEDMOREPARAMS(Client *client, const std::string &command);    // 461
std::string ERR_ALREADYREGISTERED(Client *client);                             // 462
std::string ERR_PASSWDMISMATCH(Client *client);                                // 464
std::string ERR_CHANNELISFULL(Client *client);                                 // 471
std::string ERR_INVITEONLYCHAN(Client *client);                                // 473
std::string ERR_BANNEDFROMCHAN(Client *client);                                // 474
std::string ERR_BAD_CHANNELKEY(Client *client);                                // 475
std::string ERR_BADCHANMASK(Client *client);                                   // 476

#endif
