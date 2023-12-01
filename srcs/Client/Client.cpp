#include "Client.hpp"

// named constructor
Client::Client(int fd, const std::string ip) : _fd(fd), _ip(ip), _isRegistered(false), _isAuthenticated(false), _nickname(""), _realname(""), _username(""), _readBuffer(""), _sendBuffer("") {}

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
    
    // use nickname as fallback
    // if (_realname.empty())
    //     setRealname(_nickname);
    
    // use nickname as fallback
    // if (_username.empty())
    //     setUsername(_nickname);
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
    if (_isRegistered) // if is registered, then return true
        return _isRegistered;
    
    // if not, check if the current state of the user completed the registration
    if (!(_nickname.empty() && _realname.empty() && _username.empty()))
    {
        // if nickname, realname and username are not empty
        setRegistered(true);
        return _isRegistered;
    }

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

void Client::queueBuffer(BufferType type, const std::string &msg)
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
