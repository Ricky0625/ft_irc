#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irclib.h"

class Client
{
public:
    Client(int fd, const std::string ip);
    ~Client(void);

    // string representation
    std::string toString(void) const;

private:
    int _fd;
    std::string _ip;
};

std::ostream &operator<<(std::ostream &out, const Client &obj);

#endif
