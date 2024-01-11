/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:52:45 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/11 21:04:33 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
#define BOT_HPP

#include "botlib.h"
#include "Color.hpp"
#include "Display.hpp"
#include "BotReply.hpp"
#include "Parser.hpp"

#define INVALID_USAGE "Invalid usage! ./bot <ip> <port> <password>"
#define FAIL_TO_INIT_SOCKET "Failed to initialize socket!"
#define BUFFER_SIZE 4096

typedef enum BufferType
{
    READ,
    SEND
} BufferType;

/**
 * last sent to this channel (time_t)
 * message timeout (time_t)
 */

typedef struct MsgInfo
{
    time_t lastSentTime;
    time_t messageTimeout;
} MsgInfo;

class Bot
{
public:
    typedef std::vector<std::string> BotResponse;
    typedef std::map<std::string, MsgInfo> JoinedChannel;

    Bot(const std::string &serverIp, const std::string &port, const std::string &password);
    ~Bot(void);

    void start(void);

private:
    // identity
    int _botFd;
    std::string _password;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    bool _isRegistered;

    // poll-related
    struct pollfd _pollfds[1];

    // buffers
    std::string _readBuffer;
    std::string _sendBuffer;

    // midly annoying responses
    BotResponse _response;

    // channel
    JoinedChannel _channels;

    // connection
    void _createBotSocket(void);
    void _obtainServerInfo(const std::string &ip, const std::string &port, struct addrinfo **servinfo);
    void _connectToServer(struct addrinfo *servinfo);

    // registration
    void _queueRegistrationMessage(void);
    void _autoChangeName(void);

    // midly-annoying-responses-related
    void _storeResponses(void);

    // events
    void _handleSocketEvents(void);
    void _subscribeEvent(short event);
    void _unsubscribeEvent(short event);

    // buffer management
    std::string _getBuffer(BufferType type);
    void _enqueueBuffer(BufferType type, const std::string &msg);
    void _clearBuffer(BufferType type);

    // messaging
    void _readRequest(void);
    void _sendReply(void);
    void _processRequests(void);

    // channel management
    void _initializeNewMsgInfo(MsgInfo &info);
    int _getRandomTimeout(void);
    void _addChannel(const std::string &channel);
    void _removeChannel(const std::string &channel);
    void _updateChannelMsgInfo(const std::string &channel);

    // response back to server
    void _responseBack(const IRCMessage &msg);
    std::string _getRandomResponse();
    bool _canITalk(const MsgInfo &info);
};
#endif