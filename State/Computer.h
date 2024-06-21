#ifndef COMPUTER_H
#define COMPUTER_H

#include "State.h"
#include "../Radio/Radio.h"

class Computer
{
public:
    Computer(State *state, Radio *radio);
    virtual ~Computer() {};
    virtual bool init();
    virtual void update();
    virtual bool transmit();
protected:
    State *state;
    Radio *radio;

};

#endif