#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Parser.hpp"
#include "ChannelMember.hpp"

class Channel
{
public:
    typedef std::map<std::string, ChannelMember *> MemberTable;

    Channel(const std::string &name);
    ~Channel(void);

    // getters
    std::string getName(void) const;
    std::string getTopic(void) const;
    std::string getTopicSetBy(void) const;
    std::string getTopicSetAt(void) const;
    std::string getAllMembersAsString(const std::string &sender) const;
    MemberTable getMembers(void) const;
    int getMemberTotal(void) const;

    // setters
    void setPassword(const std::string &password);
    void setTopic(const std::string &topic);
    void setTopicSetBy(const std::string &nickname);
    void updateTopicSetAt(void);

    // member management
    bool addMember(Client *client);
    void removeMember(const std::string &nickname);
    void removeAllMembers(void);
    ChannelMember *getMember(Client *target);

private:
    // channel credentials
    std::string _name;
    std::string _password;

    Modes _channelMode;

    std::string _topic;
    std::string _topicSetBy;
    std::string _topicSetAt;

    MemberTable _members;
};

#endif
