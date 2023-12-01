#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class NICK : public ICommand
{
public:
    NICK(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _nickname;

    // check nickname
    bool _isValidNickname(const std::string &newNickname) const;

};

#endif
