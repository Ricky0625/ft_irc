#include "Server.hpp"

/**
 * @brief Add new channel
 */
Channel *Server::addNewChannel(const std::string &channelName)
{
    // channel already exists, do nothing
    if (_channels.find(channelName) != _channels.end())
        return NULL;

    _channels[channelName] = new Channel(channelName);

    Display::displayServerAction(_serverFd, "Server added new channel. `Server::_addNewChannel`");

    return _channels[channelName];
}

/**
 * @brief Get channel by name
 */
Channel *Server::getChannel(const std::string &channelName)
{
    ChannelTable::iterator target = _channels.find(channelName);

    if (target == _channels.end())
        return NULL;

    return target->second;
}
