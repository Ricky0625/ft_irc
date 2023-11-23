#ifndef COMMANDFACTORY_HPP
#define COMMANDFACTORY_HPP

#include "irclib.h"
#include "ICommand.hpp"
#include "commands.h"

class CommandFactory
{
public:
    typedef std::map<std::string, ICommand *> CommandTable; // <command name, command instance>

    CommandFactory(void);
    ~CommandFactory(void);

    ICommand *getCommand(const std::string &commandName);

private:
    CommandTable _commands;
};

#endif
