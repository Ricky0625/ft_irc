#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "irclib.h"
#include "Server.hpp"

class Server;

class Display
{
public:
    Display(void);
    ~Display(void);

    static void clearScreen(void);
    static void displayServerInfo(const std::string &port, const std::string &pass, const std::string &upTime);
    static void displayIncoming(int fromFd, const std::string &msg);
    static void displayOutgoing(int toFd, const std::string &msg);

private:
};

#endif
