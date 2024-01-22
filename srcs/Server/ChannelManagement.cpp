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

    // default mode is topic-locked mode, but if the config file does not have this, ok loh nvm.
    if (_supportedChannelModes.hasMode('t'))
        _channels[channelName]->channelModes.addMode('t');

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

Server::ChannelTable Server::getChannels(void) const
{
    return _channels;
}

void Server::removeChannel(const std::string &channelName)
{
    ChannelTable::iterator channel = _channels.find(channelName);

    if (channel == _channels.end())
        return;

    delete channel->second;
    _channels.erase(channelName);
}

void Server::removeMemberFromChannels(const std::string &nickname)
{
    Channel *targetChannel;
    std::vector<std::string> emptyChannel;

    for (ChannelTable::iterator it = _channels.begin(); it != _channels.end(); it++)
    {
        targetChannel = it->second;
        targetChannel->removeMember(nickname);

        if (targetChannel->getMemberTotal() == 0)
            emptyChannel.push_back(targetChannel->getName());
    }

    // remove empty channel
    for (size_t i = 0; i < emptyChannel.size(); i++)
        removeChannel(emptyChannel[i]);
}
