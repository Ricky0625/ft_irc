#ifndef PARSER_HPP
#define PARSER_HPP

#include "irclib.h"

// A struct that represents an IRCmessage structure
typedef struct IRCMessage
{
    std::string prefix;                 // Optional prefix
    std::string command;                // Command of the message
    std::vector<std::string> arguments; // Arguments of the message
    bool hasTrailing;
    std::string trailing;               // The trailing part of the message
} IRCMessage;

#define COLON ':'
#define SPACE ' '

typedef enum SplittingMode
{
    INCLUDE_EMPTY,
    EXCLUDE_EMPTY
} SplittingMode;

class Parser
{
public:
    typedef std::vector<std::string> Splitted;

    Parser(void);
    ~Parser(void);

    static void splitStr(const std::string &str, Splitted &vect, const std::string &delimeter, SplittingMode mode = EXCLUDE_EMPTY);
    static IRCMessage parseIRCMessage(const std::string &str);
    static void showMessage(const IRCMessage &msg);
    static std::string getTimeNow(void);
    static std::string getUnixTimeStamp(void);
};

#endif
