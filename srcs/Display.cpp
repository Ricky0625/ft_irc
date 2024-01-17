#include "Display.hpp"

// default constructor
Display::Display(void) {}

// destructor
Display::~Display(void) {}

void Display::clearScreen()
{
    std::cout << "\033[2J" << std::flush;
}

void Display::displayServerInfo(const std::string &port, const std::string &pass, const std::string &upTime)
{
    std::cout << BOLD_WHITE "==+==+==+==+==+==+==+==+==+==+==+==+==+==\n" << std::endl;
    std::cout << " [Server " BOLD_YELLOW SERVER_NAME << "(" << HOST << ")" << BOLD_WHITE "]" << std::endl;
    std::cout << " Created: " << upTime << std::endl;
    std::cout << " Version: " << VERSION << std::endl;
    std::cout << " Port   : " BOLD_GREEN << port << BOLD_WHITE << std::endl;
    std::cout << " Pass   : " BOLD_GREEN << pass << BOLD_WHITE << std::endl;
    std::cout << "\n==+==+==+==+==+==+ LOG +==+==+==+==+==+==\n" RESET << std::endl;
}

void Display::displayIncoming(int fromFd, const std::string &msg)
{
    std::cout << BOLD_PURPLE "[CLIENT] " << std::setw(4) << fromFd << " << " << msg << RESET << std::endl;
}

void Display::displayOutgoing(int toFd, const std::string &msg)
{
    size_t crlfPos;
    std::string singleReply;
    std::string copy(msg);

    while ((crlfPos = copy.find(CRLF)) != std::string::npos)
    {
        singleReply = copy.substr(0, crlfPos);
        std::cout << BOLD_CYAN "[SERVER] " << std::setw(4) << toFd << " >> " << singleReply << RESET << std::endl;
        copy = copy.substr(crlfPos + strlen(CRLF));
    }
}

void Display::displayServerAction(int targetFd, const std::string &msg)
{
    std::cout << BOLD_YELLOW "[ACTION] " << std::setw(4) << targetFd << " :: " << msg << RESET << std::endl;
}

void Display::displayServerError(int targetFd, const std::string &msg)
{
    std::cout << BOLD_RED "[" << std::setw(6) << "ERROR" << "] " << std::setw(4) << targetFd << " @@ " << msg << RESET << std::endl;
}
