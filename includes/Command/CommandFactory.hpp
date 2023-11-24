#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

#include "irclib.h"
#include "ICommand.hpp"
#include "Server.hpp"
#include "AvailableCommands.hpp"

class ICommand;
class Server;

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
