#ifndef PASS_HPP
#define PASS_HPP

#include "ICommand.hpp"

class PASS : public ICommand
{
public:
    PASS(void);
    void execute();

private:
    ICommand *command;
};

#endif
 