#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "irclib.h"

typedef enum BufferType
{
    READ,
    SEND
} BufferType;

class Client
{
public:
    Client(int fd, const std::string ip);
    ~Client(void);

    // setter
    void setRegistered(const bool &status);

    // getter
    bool isRegistered(void) const;
    std::string getNickname(void) const;
    std::string getRealname(void) const;
    std::string getBuffer(BufferType type);

    // buffer management
    void queueBuffer(BufferType type, const std::string &msg);
    void clearBuffer(BufferType type);

private:
    int _fd;
    std::string _ip;

    // booleans
    bool _isRegistered;

    // identity
    std::string _nickname;
    std::string _realname;

    // buffers
    std::string _readBuffer;
    std::string _sendBuffer;
};

std::ostream &operator<<(std::ostream &out, const Client &obj);

#endif
