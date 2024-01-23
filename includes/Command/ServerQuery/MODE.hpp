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

    size_t _currentModeArgsIndex;

    // handle user related query
    void _handleUserQuery(Client *client);
    void _appplyUserMode(Client *client);

    // handle channel related query
    void _handleChannelQuery(Client *client);
    void _applyChannelMode(Client *client, Channel *channel);
    void _changeChannelSettings(Client *client, Channel *channel, ModeApplyAction action, char mode);

    // all the logic for channel mode (we only implement theses)
    void _updateMemberMembership(Client *client, Channel *channel, ModeApplyAction action, char mode);
    void _setClientLimitMode(Client *client, Channel *channel, ModeApplyAction action, char mode);
    void _setKeyMode(Client *client, Channel *channel, ModeApplyAction action, char mode);
    void _performModeAction(Client *client, Channel *channel, ModeApplyAction action, char mode);

    void _broadcastModeUpdate(Client *client, Channel *channel, const std::string &modeChanged, const std::string &arg);
};

#endif