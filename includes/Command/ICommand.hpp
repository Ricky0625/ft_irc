#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include "irclib.h"
#include "ReplyNumeric.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server; // forward declaration
class Client;
class Channel;

class ICommand
{
public:
    typedef void (ICommand::*RPL_CHANNEL_QUEUE)(Client *receiver, Client *newMember, Channel *channel);

    virtual ~ICommand(void);
    virtual void initialize(Server &server, const IRCMessage &ircMsg);
    virtual void execute(int clientFd) = 0;

    // getters
    std::string getPrefix(void) const;
    std::string getCommand(void) const;
    std::vector<std::string> getArgs(void) const;
    bool getHasTrailing(void) const;
    std::string getTrailing(void) const;
    IRCMessage getIRCMsg(void) const;

    // queue utils
    void queueWelcomeMessage(Client *client, const std::string &upTime);
    void queueJoinWelcomeMessage(Client *client, Channel *channel);

protected:
    Server *_server;
    IRCMessage _ircMsg;
};

#endif
