/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/08 14:05:21 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/11 21:17:55 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

Bot::Bot(const std::string &serverIp, const std::string &port, const std::string &password) : _password(password), _isRegistered(false)
{
    struct addrinfo *servinfo;

    _createBotSocket();
    _obtainServerInfo(serverIp, port, &servinfo);
    _connectToServer(servinfo);

    _nickname = "itchyliki";
    _username = "capybara00";
    _realname = "capybara the great";

    // monitor botFd
    _pollfds[0].fd = _botFd;
    _pollfds[0].events = POLLIN;

    Display::displayBotInfo(serverIp, port);

    // store all the response
    _storeResponses();
}

Bot::~Bot()
{
    _enqueueBuffer(SEND, BOTRPL_QUIT());
    _subscribeEvent(POLLOUT);
    _sendReply();
    close(_botFd);
}

void Bot::start(void)
{
    int numEvents;

    // register and get all channel name
    _queueRegistrationMessage();
    _enqueueBuffer(SEND, BOTRPL_LIST());
    _subscribeEvent(POLLOUT);

    /**
     * 1. Call LIST periodically.
     * 2. Join all the channel it can. If cannot, ignore. (only public, can just append all channel name into one string and join)
     * 3. Waiting for incoming message.
     */

    while (true)
    {
        numEvents = poll(_pollfds, 1, GET_CHANNEL_TIMEOUT);
        if (numEvents == -1)
        {
            Logger::justLog("poll", &strerror);
            break;
        }
        else if (numEvents == 0 && _isRegistered)
        {
            _enqueueBuffer(SEND, BOTRPL_LIST());
            _subscribeEvent(POLLOUT);
        }
        _handleSocketEvents();
    }
}

void Bot::_createBotSocket()
{
    int socketfd;

    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        Logger::exitLog("socket", &strerror);

    // delete this
    int optvalue = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue)) == -1)
        Logger::exitLog("setsockopt", &strerror);
    // delete this

    _botFd = socketfd;
}

void Bot::_obtainServerInfo(const std::string &ip, const std::string &port, struct addrinfo **servinfo)
{
    struct addrinfo hints;

    // setup hints
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(ip.c_str(), port.c_str(), &hints, servinfo) != 0)
        Logger::c_exitLog("getaddrinfo", &gai_strerror);
}

void Bot::_connectToServer(struct addrinfo *servinfo)
{
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
    {
        if (connect(_botFd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(_botFd);
            continue;
        }
        // if connection successful, break.
        break;
    }
    freeaddrinfo(servinfo);
}

void Bot::_queueRegistrationMessage(void)
{
    _enqueueBuffer(SEND, BOTRPL_PASS(_password));
    _enqueueBuffer(SEND, BOTRPL_NICK(_nickname));
    _enqueueBuffer(SEND, BOTRPL_USER(_username, _realname));
}

void Bot::_autoChangeName(void)
{
    char &lastChar = _nickname[_nickname.size() - 1];

    if (lastChar >= 'a' && lastChar < 'z')
        lastChar++;
    else if (lastChar >= 'A' && lastChar < 'Z')
        lastChar++;
    else
        _nickname = _nickname + 'a';
}

void Bot::_storeResponses(void)
{
    std::ifstream file(RESPONSE_FILE);

    if (file.is_open() == false)
        Logger::exitLog("ifstream open", &strerror);

    std::string line;
    while (std::getline(file, line))
        _response.push_back(line);

    file.close();
}

void Bot::_handleSocketEvents(void)
{
    const pollfd &socketInfo = _pollfds[0];

    if (socketInfo.revents & POLLOUT) // client can write response without blocking
        _sendReply();
    else if (socketInfo.revents & POLLIN) // client can listen request without blocking
        _readRequest();
    // i don't care about POLLERR, POLLHUP, POLLNVAL here. hopefully nothing breaks.
}

void Bot::_subscribeEvent(short event)
{
    struct pollfd &socketInfo = _pollfds[0];

    if (socketInfo.events & event)
        return;

    socketInfo.events |= event;
}

void Bot::_unsubscribeEvent(short event)
{
    struct pollfd &socketInfo = _pollfds[0];

    if ((socketInfo.events & event) == 0)
        return;

    socketInfo.events &= ~event;
}

std::string Bot::_getBuffer(BufferType type)
{
    switch (type)
    {
    case READ:
        return _readBuffer;
    case SEND:
        return _sendBuffer;
    default:
        return "";
    }
}

void Bot::_enqueueBuffer(BufferType type, const std::string &msg)
{
    switch (type)
    {
    case READ:
        _readBuffer += msg;
        break;
    case SEND:
        _sendBuffer += msg;
        break;
    }
}

void Bot::_clearBuffer(BufferType type)
{
    switch (type)
    {
    case READ:
        _readBuffer.clear();
        break;
    case SEND:
        _sendBuffer.clear();
        break;
    }
}

void Bot::_readRequest()
{
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    bytesRead = recv(_botFd, buffer, BUFFER_SIZE - 1, 0);
    if (bytesRead == -1)
    {
        Logger::justLog("recv", &strerror);
        exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0) // server disconnected
        exit(EXIT_FAILURE);

    buffer[bytesRead] = '\0';

    _enqueueBuffer(READ, buffer);
    _processRequests();
}

void Bot::_processRequests()
{
    size_t crlfPos;
    IRCMessage ircMsg;
    std::string singleRequest;
    std::string readBuffer = _getBuffer(READ);

    while ((crlfPos = readBuffer.find(CRLF)) != std::string::npos)
    {
        singleRequest = readBuffer.substr(0, crlfPos);
        if (singleRequest.empty())
        {
            readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
            continue;
        }
        ircMsg = Parser::parseIRCMessage(singleRequest);
        Display::displayIncoming(_botFd, singleRequest);

        _responseBack(ircMsg);

        readBuffer = readBuffer.substr(crlfPos + strlen(CRLF));
    }

    if (_getBuffer(SEND).empty() == false)
        _subscribeEvent(POLLOUT);

    // there's a buffer issue somewhere... like the one i fixed in server before
    _clearBuffer(READ);
    if (!readBuffer.empty())
        _enqueueBuffer(READ, readBuffer);
}

void Bot::_sendReply()
{
    ssize_t bytesSent;
    std::string reply = _getBuffer(SEND);

    if (reply.empty())
        return;

    bytesSent = send(_botFd, reply.c_str(), reply.size(), 0);
    Display::displayOutgoing(_botFd, reply.substr(0, bytesSent));

    if (bytesSent == -1)
    {
        Logger::justLog("send", &strerror);
        return;
    }

    _clearBuffer(SEND);
    _enqueueBuffer(SEND, reply.substr(bytesSent));
    _unsubscribeEvent(POLLOUT);
}

/**
 * It's always something like this:
 *
 * :server statusCode someArgs :trailing
 *
 * 464: password incorrect, exit
 * 433: nickname in use, auto create another nickname
 *
 * 001: means registration done
 *
 * 322: means got channel
 */
void Bot::_responseBack(const IRCMessage &msg)
{
    const std::string &command = msg.command;
    std::vector<std::string> args = msg.arguments;

    if (command == "464") // wrong server password
        Logger::msgExitLog("Password incorrect!");

    if (command == "433") // nickname in use
    {
        _autoChangeName();
        _enqueueBuffer(SEND, BOTRPL_NICK(_nickname));
        _enqueueBuffer(SEND, BOTRPL_LIST());
        return;
    }

    if (command == "001" && _isRegistered == false) // registration complete
    {
        _isRegistered = true;
        return;
    }

    if (command == "322") // channel list, add to map
    {
        size_t numOfChannel = _channels.size();
        const std::string &channelName = args[1];
        _addChannel(channelName);

        // listen and join
        if (numOfChannel != _channels.size())
            _enqueueBuffer(SEND, BOTRPL_JOIN(channelName));
        return;
    }

    if (command == "PRIVMSG")
    {
        const std::string &channelName = args[0];
        JoinedChannel::iterator entry = _channels.find(channelName);

        if (entry == _channels.end())
            return;

        if (_canITalk(entry->second))
        {
            _enqueueBuffer(SEND, BOTRPL_PRIVMSG(channelName, _getRandomResponse()));
            _updateChannelMsgInfo(channelName);
        }
        return;    
    }
}

std::string Bot::_getRandomResponse()
{
    if (_response.empty())
        return std::string(AWKWARD_REPLY);

    std::srand(std::time(0));
    int randomIndex = std::rand() % _response.size();
    return _response[randomIndex];
}

bool Bot::_canITalk(const MsgInfo &info)
{
    std::time_t currentTime = std::time(NULL);
    return (currentTime - info.lastSentTime) > info.messageTimeout;
}
