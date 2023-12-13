#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class TOPIC : public ICommand
{
public:
    TOPIC(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _channel;
    std::string _msg;
    bool _hasMsg;
};

#endif
