/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelManagement.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wricky-t <wricky-t@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 18:59:54 by wricky-t          #+#    #+#             */
/*   Updated: 2024/01/11 20:50:06 by wricky-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

void Bot::_initializeNewMsgInfo(MsgInfo &info)
{
    info.lastSentTime = std::time(NULL);
    info.messageTimeout = _getRandomTimeout();
}

int Bot::_getRandomTimeout()
{
    // maxSeconds + (rand() % (maxSeconds - minSeconds + 1))
    return 10 + (rand() % (10 - 1 + 1));
}

void Bot::_addChannel(const std::string &channel)
{
    if (_channels.find(channel) != _channels.end())
        return;

    MsgInfo info;

    _initializeNewMsgInfo(info);
    _channels[channel] = info;
    std::cout << BOLD_PURPLE "{BOTACT}          Watching " << channel << RESET << std::endl;
}

void Bot::_removeChannel(const std::string &channel)
{
    _channels.erase(channel);
}

void Bot::_updateChannelMsgInfo(const std::string &channel)
{
    JoinedChannel::iterator entry = _channels.find(channel);

    if (entry == _channels.end())
        return;
    
    MsgInfo &info = entry->second;
    info.lastSentTime = std::time(NULL);
    info.messageTimeout = _getRandomTimeout();
}
