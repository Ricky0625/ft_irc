#ifndef CHANNELMEMBER_HPP
#define CHANNELMEMBER_HPP

#include "irclib.h"
#include "Client.hpp"

class ChannelMember
{
public:
    ChannelMember(Client *client);
    ~ChannelMember(void);

    Client *getClientInfo(void) const;
    Modes memberMode;

private:
    Client *_client;
};

#endif
