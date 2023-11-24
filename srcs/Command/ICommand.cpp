#include "ICommand.hpp"

// destructor
ICommand::~ICommand() {}

/**
 * @brief Initialize the concrete command object
 */
void ICommand::initialize(Server &server, const IRCMessage &ircMsg)
{
    _server = &server;
    _prefix = ircMsg.prefix;
    _command = ircMsg.command;
    _args = ircMsg.arguments;
    _trailing = ircMsg.trailing;
}

/**
 * @getter
 * @brief get prefix
*/
std::string ICommand::getPrefix() const
{
    return _prefix;
}

/**
 * @getter
 * @brief get command name
*/
std::string ICommand::getCommand() const
{
    return _command;
}

/**
 * @getter
 * @brief get argument list
*/
std::vector<std::string> ICommand::getArgs() const
{
    return _args;
}

/**
 * @getter
 * @brief get trailing
*/
std::string ICommand::getTrailing() const
{
    return _trailing;
}
