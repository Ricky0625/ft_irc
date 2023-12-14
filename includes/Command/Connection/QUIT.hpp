#ifndef QUIT_HPP
#define QUIT_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class QUIT : public ICommand
{
public:
    QUIT(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _msg;

    void _sayGoodbyeToChannels(Client *client);
    void _broadcastMemberQuit(Client *client, Channel *channel);
};

#endif
