#include "Channel.hpp"

// default constructor
Channel::Channel(const std::string &name) : _name(name), _password(""), _memberLimit(0) {}

// destructor
Channel::~Channel()
{
    removeAllMembers();
}

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

static std::string getMemberPrefix(const Modes &modes)
{
    if (modes.hasMode('o'))
        return "@";
    else if (modes.hasMode('v'))
        return "+";
    return "";
}

std::string Channel::getAllMembersAsString(const std::string &sender) const
{
    std::string membersStr = "";
    Client *clientInfo;
    bool isSenderInsideChannel = _members.find(sender) != _members.end();

    for (MemberTable::const_iterator it = _members.begin(); it != _members.end(); it++)
    {
        Modes &currentMemberModes = it->second->memberMode;
        const std::string &prefix = getMemberPrefix(currentMemberModes);
        clientInfo = it->second->getClientInfo();
        if (clientInfo->networkMode.hasMode('i') && isSenderInsideChannel == false)
            continue;
        membersStr += prefix + clientInfo->getNickname() + " ";
    }

    return membersStr;
}

Channel::MemberTable Channel::getMembers(void) const
{
    return _members;
}

size_t Channel::getMemberTotal(void) const
{
    return _members.size();
}

size_t Channel::getMemberLimit(void) const
{
    return _memberLimit;
}

bool Channel::isCorrectPassword(const std::string &pass) const
{
    return _password == pass;
}

ChannelMember *Channel::getMemberUsingNickname(const std::string &nickname)
{
    MemberTable::iterator entry = _members.find(nickname);

    if (entry == _members.end())
        return NULL;
    
    return entry->second;
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
    _topic = topic;
}

void Channel::setTopicSetBy(const std::string &nickname)
{
    _topicSetBy = nickname;
}

void Channel::setMemberLimit(int limit)
{
    if (limit < 0)
        _memberLimit = 0;
    else
        _memberLimit = limit;
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
    bool isChannelFounder = false;
    const std::string &nickname = client->getNickname();

    if (_members.find(nickname) != _members.end())
        return false;

    if (_members.size() == 0)
        isChannelFounder = true;
    _members[nickname] = new ChannelMember(client);
    if (isChannelFounder)
        _members[nickname]->memberMode.addMode('o');
    return true;
}

void Channel::removeMember(const std::string &nickname)
{
    MemberTable::iterator member = _members.find(nickname);

    if (member == _members.end())
        return;

    delete member->second; // should be fine. just remove ChannelMember not Client
    _members.erase(nickname);
}

void Channel::removeAllMembers(void)
{
    for (MemberTable::iterator it = _members.begin(); it != _members.end(); it++)
        delete it->second;
    _members.clear();
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
