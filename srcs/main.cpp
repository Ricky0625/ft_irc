/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:11:50 by wricky-t          #+#    #+#             */
/*   Updated: 2023/11/22 17:56:46 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Parser.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
        Logger::msgExitLog(INVALID_USAGE);
    Server server(av[1], av[2]);
    server.start();
    std::string msg(":prefix COMMAND arg1 arg2 arg3:trailing part with spaces");
    IRCMessage ircMsg = Parser::parseIRCMessage(msg);
    std::cout << ircMsg.prefix << std::endl;
    std::cout << ircMsg.command << std::endl;
    for (size_t i = 0; i < ircMsg.arguments.size(); i++)
        std::cout << "- " << ircMsg.arguments[i] << std::endl;
    std::cout << ircMsg.trailing << std::endl;
}