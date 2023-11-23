#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

class ICommand
{
public:
    virtual ~ICommand(void);
    virtual void execute() = 0;
    /**
     * for execute, it will do these things:
     * 1. perform action (arguments checking)
     * 2. write replies into client's _sendBuffer
     */
};

#endif
