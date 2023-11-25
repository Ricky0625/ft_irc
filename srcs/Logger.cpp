/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:22:50 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/25 12:54:38 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

// default constructor
Logger::Logger() {}

// destructor
Logger::~Logger() {}

void Logger::msgExitLog(const std::string &msg)
{
    std::cerr << BOLD_RED << "[IRCSERV] " <<  msg << RESET << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * @brief Takes in a custom message identifier & a logger function
 * 
 * Log error and exit.
 * Logger function must return a const char *
*/
void Logger::c_exitLog(const std::string &identifier, c_logger logger)
{
    std::cerr << BOLD_RED << identifier << ": " << logger(errno) << RESET << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * @brief Takes in a custom message identifier & a logger function
 * 
 * Log error and exit.
 * Logger function must return a char *
*/
void Logger::exitLog(const std::string &identifier, logger logger)
{
    std::cerr << BOLD_RED << identifier << ": " << logger(errno) << RESET << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * @brief Takes in a custom message identifier & a logger function
 * 
 * Just log the error out.
*/
void Logger::justLog(const std::string &identifier, logger logger)
{
    std::cerr << identifier << ": " << logger(errno) << std::endl;
}
