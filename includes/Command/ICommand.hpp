#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

class ICommand
{
public:
    virtual ~ICommand(void);
    virtual void execute() = 0;
};

#endif
