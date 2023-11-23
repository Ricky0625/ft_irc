#ifndef NICK_HPP
#define NICK_HPP

#include "ICommand.hpp"

class NICK : public ICommand
{
public:
    NICK(void);
    void execute();
};

#endif
