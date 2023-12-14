#include "ReplyNumeric.hpp"

std::string RPL_NICK(Client *client, const std::string &oldNickname)
{
    return ClientHeader(client, oldNickname) + " NICK " + client->getNickname() + CRLF;
}

std::string RPL_PONG(Client *client, const std::string &oldNickname, const std::string &token)
{
    return ClientHeader(client, oldNickname) + " PONG :" + token + CRLF;
}

std::string RPL_JOIN(Client *newMember, Channel *channel)
{
    const std::string &nickname = newMember->getNickname();
    const std::string &channelName = channel->getName();
    const std::string joiningMessage = nickname + " is joining the channel " + channelName;

    return ClientHeader(newMember, nickname) + " JOIN " + channelName + CRLF;
}

std::string RPL_PART(Client *member, Channel *channel, const std::string &reason)
{
    const std::string &nickname = member->getNickname();
    const std::string &channelName = channel->getName();

    return ClientHeader(member, nickname) + " PART " + channelName + (reason.empty() ? "" + std::string(CRLF) : MessageTrailing(reason));
}