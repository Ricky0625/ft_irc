#include "Client.hpp"

// named constructor
Client::Client(int fd, const std::string ip) : _fd(fd), _ip(ip), _readBuffer(""), _sendBuffer("") {}

// destructor
Client::~Client() {}

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
