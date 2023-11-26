#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

#include "irclib.h"
#include "AvailableCommands.hpp"

class Server;   // forward declaration
class ICommand; // forward declaration

class CommandFactory
{
public:
    typedef std::map<std::string, ICommand *> CommandTable; // <command name, command instance>

    CommandFactory(void);
    ~CommandFactory(void);

    ICommand *recognizeCommand(Server &server, const IRCMessage &ircMsg);

private:
    CommandTable _commands;
};

#endif
