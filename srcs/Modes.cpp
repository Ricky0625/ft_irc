/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Modes.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 14:25:25 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/18 16:16:42 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Modes.hpp"

// default constructor
Modes::Modes(void)
{
    _modes.clear();
}

// destructor
Modes::~Modes()
{
    _modes.clear();
}

void Modes::addMode(char mode)
{
    _modes.insert(mode);
}

void Modes::removeMode(char mode)
{
    _modes.erase(mode);
}

bool Modes::hasMode(char mode) const
{
    return (_modes.find(mode) != _modes.end());
}

std::string Modes::getModeString() const
{
    std::stringstream ss;

    ss << "+";
    for (ModeSet::iterator it = _modes.begin(); it != _modes.end(); it++)
        ss << *it;
    
    return ss.str();
}

void Modes::displayModes(const std::string &category) const
{
    std::cout << category << " modes: " << getModeString() << std::endl;
}
