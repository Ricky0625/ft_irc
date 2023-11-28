#include "Client.hpp"

// named constructor
Client::Client(int fd, const std::string ip) : _fd(fd), _ip(ip), _isRegistered(false), _readBuffer(""), _sendBuffer("") {}

// destructor
Client::~Client() {}

void Client::setRegistered(const bool &status)
{
    _isRegistered = status;
}

bool Client::isRegistered(void) const
{
    return _isRegistered;
}

std::string Client::getNickname(void) const
{
    return _nickname;
}

std::string Client::getRealname(void) const
{
    return _realname;
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
// std::string Client::toString() const
// {
//     std::string strRepr;

//     strRepr = "fd: " + std::string(_fd) + "\n";
//     strRepr += "ip: " + _ip + "\n";
//     return strRepr;
// }

// overload insertion operator
// std::ostream &operator<<(std::ostream &out, const Client &obj)
// {
//     out << obj.toString();
//     return out;
// }
