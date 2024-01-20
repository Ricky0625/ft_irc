/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Modes.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42kl.edu.my>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 14:25:27 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/20 12:31:39 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODES_H
#define MODES_H

#include "irclib.h"

typedef std::set<char> ModeSet;

class Modes
{
public:
    Modes(void);
    ~Modes(void);

    // actions
    void addMode(char mode);
    void removeMode(char mode);
    bool hasMode(char mode) const;
    std::string getModeString(void) const;
    void displayModes(const std::string &category) const;

private:
    ModeSet _modes;
};

#endif
