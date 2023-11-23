#ifndef USER_HPP
#define USER_HPP

#include "ICommand.hpp"

class USER : public ICommand
{
public:
    USER(void);
    void execute();
};

#endif
