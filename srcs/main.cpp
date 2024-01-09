/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:11:50 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/08 22:31:03 by wricky-t         ###   ########.fr       */
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
}