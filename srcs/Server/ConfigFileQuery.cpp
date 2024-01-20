/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileQuery.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 14:23:20 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/20 12:19:19 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::string Server::getValueFromSection(const std::string &section, const std::string &key)
{
    Parser::ConfigSection configSection = getConfigSection(section);
    Parser::ConfigSection::iterator entry = configSection.find(key);

    if (entry == configSection.end())
        throw Server::NoSuchConfigKey();

    return entry->second;
}

Parser::ConfigSection Server::getConfigSection(const std::string &section)
{
    Parser::ConfigMap::iterator targetSection = _config.find(section);

    if (targetSection == _config.end())
        throw Server::NoSuchConfigSection();
    
    return targetSection->second;
}

const char *Server::NoSuchConfigSection::what() const throw()
{
    return "No such config section.";
}

const char *Server::NoSuchConfigKey::what() const throw()
{
    return "No such config key in this section";
}
