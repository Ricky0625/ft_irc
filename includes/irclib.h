/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irclib.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/08 13:19:02 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/22 16:46:35 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCLIB_H
#define IRCLIB_H

#include <iostream>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <poll.h>

#include <vector>
#include <map>

#include "Parser.hpp"
#include "Logger.hpp"
#include "Color.hpp"

#define CRLF "\r\n"

#endif