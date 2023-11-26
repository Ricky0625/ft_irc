#include "ICommand.hpp"

// destructor
ICommand::~ICommand() {}

/**
 * @brief Initialize the concrete command object
 */
void ICommand::initialize(Server &server, const IRCMessage &ircMsg)
{
    _server = &server;
    _ircMsg = ircMsg;
}

/**
 * @getter
 * @brief get prefix
*/
std::string ICommand::getPrefix() const
{
    return _ircMsg.prefix;
}

/**
 * @getter
 * @brief get command name
*/
std::string ICommand::getCommand() const
{
    return _ircMsg.command;
}

/**
 * @getter
 * @brief get argument list
*/
std::vector<std::string> ICommand::getArgs() const
{
    return _ircMsg.arguments;
}

/**
 * @getter
 * @brief get trailing
*/
std::string ICommand::getTrailing() const
{
    return _ircMsg.trailing;
}
