#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "botlib.h"

class Display
{
public:
    Display(void);
    ~Display(void);

    static void clearScreen(void);
    static void displayBotInfo(const std::string &ip, const std::string &port);
    static void displayIncoming(int fromFd, const std::string &msg);
    static void displayOutgoing(int toFd, const std::string &msg);
    static void displayServerAction(int targetFd, const std::string &msg);
    static void displayServerError(int targetFd, const std::string &msg);
};

#endif
