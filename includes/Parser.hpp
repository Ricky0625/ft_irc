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
    typedef std::map<std::string, std::map<std::string, std::string> > ConfigMap;

    Parser(void);
    ~Parser(void);

    static void splitStr(const std::string &str, Splitted &vect, const std::string &delimeter, SplittingMode mode = EXCLUDE_EMPTY);
    static std::string &ltrim(std::string &str);
    static std::string &rtrim(std::string &str);
    static std::string &trim(std::string &str);
    static void toLowerCase(std::string& input);
    static IRCMessage parseIRCMessage(const std::string &str);
    static void showMessage(const IRCMessage &msg);
    static std::string getTimeNow(void);
    static std::string getUnixTimeStamp(void);
    static void parseConfigFile(const std::string &filename, ConfigMap &configMap);
    static void showConfig(const ConfigMap &configMap);

    template <typename T>
    static std::string to_string(const T& n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
};

#endif
