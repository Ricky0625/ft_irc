#include "ChannelMember.hpp"

// default constructor
ChannelMember::ChannelMember(Client *client) : _client(client) {}

// destructor
ChannelMember::~ChannelMember() {}

Client *ChannelMember::getClientInfo(void) const
{
    return _client;
}
