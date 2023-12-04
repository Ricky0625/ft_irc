#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include "irclib.h"
#include "ReplyNumeric.hpp"
#include "Client.hpp"

class Server; // forward declaration
class Client;

class ICommand
{
public:

    virtual ~ICommand(void);
    virtual void initialize(Server &server, const IRCMessage &ircMsg);
    virtual void execute(int clientFd) = 0;

    // getters
    std::string getPrefix(void) const;
    std::string getCommand(void) const;
    std::vector<std::string> getArgs(void) const;
    std::string getTrailing(void) const;
    IRCMessage getIRCMsg(void) const;

    void queueWelcomeMessage(Client *client, const std::string &upTime);

protected:
    Server *_server;
    IRCMessage _ircMsg;
};

#endif
