#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include "irclib.h"

class Server; // forward declaration

class ICommand
{
public:

    virtual ~ICommand(void);
    virtual void initialize(Server &server, const IRCMessage &ircMsg);
    virtual void execute(void) = 0;

    // getters
    std::string getPrefix(void) const;
    std::string getCommand(void) const;
    std::vector<std::string> getArgs(void) const;
    std::string getTrailing(void) const;
    /**
     * for execute, it will do these things:
     * 1. perform action (arguments checking)
     * 2. write replies into client's _sendBuffer
     */

protected:
    Server *_server;
    IRCMessage _ircMsg;
};

#endif
