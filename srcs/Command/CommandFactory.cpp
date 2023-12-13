#include "CommandFactory.hpp"

// default constructor
CommandFactory::CommandFactory()
{
    // connection
    _commands["PASS"] = new PASS();
    _commands["NICK"] = new NICK();
    _commands["USER"] = new USER();
    _commands["PING"] = new PING();
    _commands["PONG"] = new PONG();
    // _commands["OPER"] = new OPER();
    // _commands["QUIT"] = new QUIT();

    // channel operations
    _commands["JOIN"] = new JOIN();
    // _commands["PART"] = new PART();
    _commands["TOPIC"] = new TOPIC();
    // _commands["NAMES"] = new NAMES();
    // _commands["LIST"] = new LIST();
    // _commands["INVITE"] = new INVITE();
    // _commands["KICK"] = new KICK();

    // Server queries
    // _commands["MODE"] = new MODE();

    // messages
    // _commands["PRIVMSG"] = new PRIVMSG();
    // _commands["NOTICE"] = new NOTICE();
}

// destructor
CommandFactory::~CommandFactory() {}

/**
 * @brief Dynamically create and initialize concrete command object based on command.
 */
ICommand *CommandFactory::recognizeCommand(Server &server, const IRCMessage &ircMsg)
{
    CommandTable::iterator command = _commands.find(ircMsg.command);
    ICommand *commandObj = (command != _commands.end() ? command->second : NULL);

    if (commandObj != NULL)
        commandObj->initialize(server, ircMsg);

    return commandObj;
}
