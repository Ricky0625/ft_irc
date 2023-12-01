/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irclib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 13:19:02 by wricky-t          #+#    #+#             */
/*   Updated: 2023/12/01 15:17:00 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCLIB_HPP
#define IRCLIB_HPP

#include <iostream>
#include <string>
#include <string.h>
#include <ctime>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <stdlib.h>

#include <poll.h>

#include <vector>
#include <map>

#include "Parser.hpp"
#include "Logger.hpp"
#include "Color.hpp"

#define CRLF "\r\n"
#define SERVER_NAME "capygroove"
#define HOST "localhost"
#define VERSION "v1.0.0"

#endif
