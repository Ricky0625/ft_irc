#include "Channel.hpp"

// default constructor
Channel::Channel(const std::string &name) : _name(name), _password("") {}

// destructor
Channel::~Channel() {}

/**
 * @brief get channel name
*/
std::string Channel::getName(void) const
{
    return _name;
}

/**
 * @brief get channel topic
*/
std::string Channel::getTopic(void) const
{
    return _topic;
}

std::string Channel::getTopicSetBy(void) const
{
    return _topicSetBy;
}

std::string Channel::getTopicSetAt(void) const
{
    return _topicSetAt;
}

std::string Channel::getAllMembersAsString(void) const
{
    std::string membersStr = "";
    Client *clientInfo;

    for (MemberTable::const_iterator it = _members.begin(); it != _members.end(); it++)
    {
        clientInfo = it->second->getClientInfo();
        /**
         * TODO:
         * 1. Check their highest channel membership prefix and append to it
         * 
         * Idea:
         * With multiple if else to check if a prefix exists, from the lowest to highest
        */
       membersStr += clientInfo->getNickname() + " ";
    }

    return membersStr;
}

Channel::MemberTable Channel::getMembers(void) const
{
    return _members;
}

/**
 * @brief set channel password
*/
void Channel::setPassword(const std::string &password)
{
    _password = password;
}

/**
 * @brief set channel topic
*/
void Channel::setTopic(const std::string &topic)
{
    std::cout << "set topic" << std::endl;
    _topic = topic;
}

void Channel::setTopicSetBy(const std::string &nickname)
{
    std::cout << "set nickname" << std::endl;
    _topicSetBy = nickname;
}

void Channel::updateTopicSetAt(void)
{
    _topicSetAt = Parser::getUnixTimeStamp();
}

/**
 * @brief Add member to channel
 * @return true if added succesful. false if member already existed.
*/
bool Channel::addMember(Client *client)
{
    const std::string &nickname = client->getNickname();

    if (_members.find(nickname) != _members.end())
        return false;
    
    _members[nickname] = new ChannelMember(client);
    return true;
}

ChannelMember *Channel::getMember(Client *target)
{
    for (MemberTable::iterator it = _members.begin(); it != _members.end(); it++)
    {
        Client *clientInfo = it->second->getClientInfo();
        if (clientInfo->getNickname() == target->getNickname())
            return it->second;
    }
    return NULL;
}
