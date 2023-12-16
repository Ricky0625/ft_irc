#ifndef NOTICE_HPP
#define NOTICE_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class NOTICE : public ICommand
{
public:
    NOTICE(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

protected:
private:
    std::vector<std::string> _targets;
    std::string _msg;
    bool _hasMsg;

    void _broadcastMessage(Client *client, Channel *channel);
};

#endif
