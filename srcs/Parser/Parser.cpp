#include "Parser.hpp"

// default constructor
Parser::Parser() {}

// destructor
Parser::~Parser() {}

/**
 * @brief Split string by delimeter
 * @attention set mode to INCLUDE_EMPTY if want to include empty string
 */
void Parser::splitStr(const std::string &str, Splitted &vect, const std::string &delimeter, SplittingMode mode)
{
    size_t pos = str.find(delimeter);
    size_t initialPos = 0;

    vect.clear();
    while (pos != std::string::npos)
    {
        std::string substring = str.substr(initialPos, pos - initialPos);
        if (!substring.empty())
            vect.push_back(substring);
        else if (substring.empty() && mode == INCLUDE_EMPTY)
            vect.push_back(substring);
        initialPos = pos + delimeter.size();
        pos = str.find(delimeter, initialPos);
    }

    std::string lastSubstring = str.substr(initialPos, std::min(pos, str.size()) - initialPos + 1);
    if (!lastSubstring.empty())
        vect.push_back(lastSubstring);
    else if (lastSubstring.empty() && mode == INCLUDE_EMPTY)
        vect.push_back(lastSubstring);
}

/**
 * @brief Trim leading whitespaces
*/
std::string &Parser::ltrim(std::string &str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return str;
}

/**
 * @brief Trim trailing whitespaces
*/
std::string &Parser::rtrim(std::string &str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
    return str;
}

/**
 * @brief Trim trailing and leading whitespaces
*/
std::string &Parser::trim(std::string &str)
{
    return ltrim(rtrim(str));
}

/**
 * @brief Convert a string to lowercase. Bruh. C++98 don't have this shit.
*/
void Parser::toLowerCase(std::string& input) {
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
}

/**
 * @brief Parse the request string into a IRCMessage struct
 * @details
 * IRC message typically follow a message structure. Here's a brief overview:
 * 1. Message prefix (optional. starts with `:`)
 * 2. Command. Specifies the action to be taken.
 * 3. Parameters of the command
 * 4. Trailing. Can contain spaces. It's part of the message. starts with `:`.
 *
 * :prefix COMMAND arg1 arg2 :trailing part with spaces
 */
IRCMessage Parser::parseIRCMessage(const std::string &str)
{
    IRCMessage msg = IRCMessage();
    size_t endPos;
    size_t trailingPos;
    std::string msgStr(str);

    msg.hasTrailing = false;
    msg.arguments.clear();
    // Check for prefix
    if (msgStr.at(0) == ':')
    {
        endPos = msgStr.find(' ');
        if (endPos != std::string::npos)
        {
            msg.prefix = msgStr.substr(0, endPos); // copy the string in between colon and space
            msgStr = msgStr.substr(endPos + 1);
        }
        else
            return msg;
    }

    // Get the command
    endPos = msgStr.find(' ');
    if (endPos != std::string::npos)
    {
        msg.command = msgStr.substr(0, endPos);
        msgStr = msgStr.substr(endPos + 1);
    }
    else
    {
        msg.command = msgStr;
        return msg;
    }

    // Check for trailing
    trailingPos = msgStr.find(':');
    if (trailingPos != std::string::npos)
    {
        msg.trailing = msgStr.substr(trailingPos + 1);
        msgStr = msgStr.substr(0, trailingPos);
        msg.hasTrailing = true;
    }

    // Get the arguments
    splitStr(msgStr, msg.arguments, " ");
    return msg;
}

/**
 * @brief Show what's inside the IRCMessage struct. Just for DEBUG purpose.
*/
void Parser::showMessage(const IRCMessage &msg)
{
    std::cout << "= = = = = = = = = = = = = = = = = =" << std::endl;
    if (!msg.prefix.empty())
        std::cout << "[PREFIX ] " << msg.prefix << std::endl;
    if (!msg.command.empty())
        std::cout << "[COMMAND] " << msg.command << std::endl;
    if (!msg.arguments.empty())
    {
        std::cout << "[ARGS   ]" << std::endl;
        for (size_t i = 0; i < msg.arguments.size(); i++)
            std::cout << " - " << msg.arguments[i] << std::endl;;
    }
    if (!msg.trailing.empty())
        std::cout << "[TRAIL  ] " << msg.trailing << std::endl;
    std::cout << "= = = = = = = = = = = = = = = = = =" << std::endl;
}

/**
 * @brief Get the current time in a formatted string format.
*/
std::string Parser::getTimeNow(void)
{
    std::time_t currentTime = std::time(NULL);
    std::tm *localTime = std::localtime(&currentTime);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S", localTime);

    return std::string(buffer);
}

/**
 * @brief Get current time as a raw unix timestamp string
*/
std::string Parser::getUnixTimeStamp(void)
{
    time_t currentTime = std::time(NULL);

    return Parser::to_string(currentTime);
}

void Parser::parseConfigFile(const std::string &filename, ConfigMap &configMap)
{
    std::ifstream configFile(filename.c_str());

    // check if the file can be opened.
    if (configFile.is_open() == false)
        return;
    
    std::string line;
    std::string currentSection;
    Splitted keyValuePair;
    while (std::getline(configFile, line))
    {
        trim(line);

        // if the line is empty or it's a comment, skip.
        if (line.empty() || line[0] == '#')
            continue;
        
        if (line[0] == '[' && line[line.size() - 1] == ']')
        {
            // new section
            currentSection = line.substr(1, line.length() - 2);
            toLowerCase(currentSection);
        }
        else
        {
            // new key value pair for the current section
            keyValuePair.clear();
            splitStr(line, keyValuePair, "=");
            if (keyValuePair.size() != 2)
            {
                std::cerr << BOLD_RED "[PARSER]    :: Ignoring this line > " << line << RESET << std::endl;
                continue;
            }

            toLowerCase(keyValuePair[0]);
            toLowerCase(keyValuePair[1]);

            configMap[currentSection][trim(keyValuePair[0])] = trim(keyValuePair[1]);
        }
    }
}

void Parser::showConfig(const ConfigMap &configMap)
{
    for (ConfigMap::const_iterator sectionIter = configMap.begin(); sectionIter != configMap.end(); ++sectionIter) {
        const std::string& sectionName = sectionIter->first;
        const std::map<std::string, std::string>& keyValues = sectionIter->second;

        std::cout << "[" << sectionName << "]" << std::endl;

        for (std::map<std::string, std::string>::const_iterator keyValueIter = keyValues.begin(); keyValueIter != keyValues.end(); ++keyValueIter) {
            const std::string& key = keyValueIter->first;
            const std::string& value = keyValueIter->second;

            std::cout << key << "=" << value << std::endl;
        }
    }
}
