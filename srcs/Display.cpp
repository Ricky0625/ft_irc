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
    clearScreen();
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
    // size_t crlfPos = msg.find(CRLF);
    // std::string incoming;

    // std::cout << BOLD_PURPLE "[CLIENT] " << fromFd << " << " << msg << RESET << std::endl;
    (void)fromFd;
    std::cout << std::endl << BOLD_PURPLE << msg << RESET;
}

void Display::displayOutgoing(int toFd, const std::string &msg)
{
    // std::istringstream buf(msg);
    // std::string line;

    // while (getline(buf, line))
        // std::cout << BOLD_CYAN "[SERVER] " << toFd << " >> " << line << RESET << std::endl;
    (void)toFd;
    std::cout << std::endl << BOLD_CYAN << msg << RESET;
}
