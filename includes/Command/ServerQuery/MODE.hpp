#ifndef MODE_HPP
#define MODE_HPP

#include "ICommand.hpp"
#include "Server.hpp"

class Server;

class MODE : public ICommand
{
public:
    typedef enum ModeApplyAction
    {
        ADDMODE,
        REMOVEMODE,
        NOACTION
    } ModeApplyAction;

    typedef std::vector<std::string> Args;

    MODE(void);
    void initialize(Server &server, const IRCMessage &ircMsg);
    void execute(int clientFd);

private:
    std::string _target;
    std::string _modeString;
    Args _modeArgs;

    // handle user related query
    void _handleUserQuery(Client *client);
    void _appplyUserMode(Client *client);

    // handle channel related query
    void _handleChannelQuery(Client *client);
};

#endif