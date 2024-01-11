#include "BotReply.hpp"

std::string BOTRPL_PASS(const std::string &pass)
{
    return "PASS " + pass + CRLF;
}

std::string BOTRPL_NICK(const std::string &nick)
{
    return "NICK " + nick + CRLF;
}

std::string BOTRPL_USER(const std::string &username, const std::string &realname)
{
    return "USER " + username + " 0 * :" + realname + CRLF;
}

std::string BOTRPL_LIST()
{
    return "LIST" + std::string(CRLF);
}

std::string BOTRPL_JOIN(const std::string &channels)
{
    return "JOIN " + channels + CRLF;
}

std::string BOTRPL_PRIVMSG(const std::string &channelName, const std::string &msg)
{
    return "PRIVMSG " + channelName + " :" + msg + CRLF;
}

std::string BOTRPL_QUIT()
{
    return "QUIT :See you guys next time!" + std::string(CRLF);
}
