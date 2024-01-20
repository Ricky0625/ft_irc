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
    void setAuthenticated(const bool &status);
    void setIsNetworkOperator(const bool &status);
    void setNickname(const std::string &name);
    void setRealname(const std::string &name);
    void setUsername(const std::string &name);

    // getter
    bool isRegistered(void);
    bool isAuthenticated(void) const;
    bool isNetworkOperator(void) const;
    std::string getNickname(void) const;
    std::string getRealname(void) const;
    std::string getUsername(void) const;
    std::string getBuffer(BufferType type);
    time_t getLastPing(void) const;
    int getFd(void) const;

    // checking
    void checkRegistered(void);
    bool isWelcomeRequired(void);

    // buffer management
    void enqueueBuffer(BufferType type, const std::string &msg);
    void clearBuffer(BufferType type);

    // timeout management
    void updateLastPing(void);
    bool isTimeout(void);

    std::string toString(void);

    // network user mode
    Modes networkMode;

private:
    int _fd;
    std::string _ip;
    time_t _lastPing;

    // booleans
    bool _isRegistered;
    bool _isAuthenticated;
    bool _isNetworkOperator;

    // identity
    std::string _nickname;
    std::string _realname;
    std::string _username;

    // buffers
    std::string _readBuffer;
    std::string _sendBuffer;
};

std::ostream &operator<<(std::ostream &out, Client &obj);

#endif
