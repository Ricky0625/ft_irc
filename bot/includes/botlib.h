#ifndef BOTLIB_HPP
#define BOTLIB_HPP

#include <iostream>
#include <iomanip>
#include <fstream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <poll.h>

#include <vector>
#include <map>

#include "Logger.hpp"
#define CRLF "\r\n"
#define SERVER_NAME "capygroove"
#define HOST "localhost"
#define VERSION "v1.0.0"
#define GET_CHANNEL_TIMEOUT 30000
#define RESPONSE_FILE "response.txt"
#define AWKWARD_REPLY "Wonder if plants judge you for being too quiet when you're alone with them?"

#endif