#include "Client.hpp"

// named constructor
Client::Client(int fd, const std::string ip) :
    _fd(fd), _ip(ip),
    _lastPing(std::time(0)),
    _isRegistered(false), _isAuthenticated(false),
    _nickname(""), _realname(""), _username(""),
    _readBuffer(""), _sendBuffer("")
{
    (void)_fd;
    (void)_lastPing;
}

// destructor
Client::~Client() {}

void Client::setRegistered(const bool &status)
{
    _isRegistered = status;
}

void Client::setAuthenticated(const bool &status)
{
    _isAuthenticated = status;
}

void Client::setNickname(const std::string &name)
{
    _nickname = name;
}

void Client::setRealname(const std::string &name)
{
    _realname = name;
}

void Client::setUsername(const std::string &name)
{
    _username = name;
}

bool Client::isRegistered(void)
{
    return _isRegistered;
}

void Client::checkRegistered(void)
{
    if (!_nickname.empty() && !_username.empty() && !_realname.empty())
        setRegistered(true);
}

bool Client::isWelcomeRequired(void)
{
    // if user registered
    if (_isRegistered)
        return false;

    // if all the names are set, set as registered, return true to send welcome
    if (!_nickname.empty() && !_username.empty() && !_realname.empty())
    {
        setRegistered(true);
        return true;
    }

    // if one of the name is not set, don't send welcome
    return false;
}

bool Client::isAuthenticated(void) const
{
    return _isAuthenticated;
}

std::string Client::getNickname(void) const
{
    return _nickname;
}

std::string Client::getRealname(void) const
{
    return _realname;
}

std::string Client::getUsername(void) const
{
    return _username;
}

std::string Client::getBuffer(BufferType type)
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

time_t Client::getLastPing(void) const
{
    return _lastPing;
}

int Client::getFd(void) const
{
    return _fd;
}

void Client::enqueueBuffer(BufferType type, const std::string &msg)
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

void Client::clearBuffer(BufferType type)
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

void Client::updateLastPing()
{
    _lastPing = std::time(0);
}

bool Client::isTimeout()
{
    time_t timeNow = std::time(0);

    return ((timeNow - _lastPing) > TIMEOUT);
}

/**
 * @brief String representation of an object
 */
std::string Client::toString()
{
    std::string strRepr;

    strRepr = "SEND BUFFER: " + getBuffer(SEND) + "\n";
    strRepr += "READ BUFFER: " + getBuffer(READ) + "\n";
    return strRepr;
}

// overload insertion operator
std::ostream &operator<<(std::ostream &out, Client &obj)
{
    out << obj.toString();
    return out;
}
