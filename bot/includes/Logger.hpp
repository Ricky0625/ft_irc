/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:15:39 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/09 14:04:50 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "botlib.h"
#include "Color.hpp"

class Logger
{
public:
    typedef char *(*logger)(int);
    typedef const char *(*c_logger)(int);

    Logger(void);
    ~Logger(void);
    static void msgExitLog(const std::string &msg);
    static void justLog(const std::string &identifier, logger logger);
    static void c_exitLog(const std::string &identifier, c_logger logger);
    static void exitLog(const std::string &identifier, logger logger);
};

#endif
