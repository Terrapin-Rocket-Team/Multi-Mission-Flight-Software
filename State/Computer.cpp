#include "Computer.h"

Computer::Computer(State *state, Radio *radio)
{
    this->state = state;
    this->radio = radio;
}

bool Computer::init()
{
    radio->begin();
    return state->init();
}

void Computer::update()
{
    state->updateState();
    transmit();
}

bool Computer::transmit()
{
    char data[200];
    snprintf(data, 200, "%f,%f,%i,%i,%i,%c,%i,%s", state->getCoordinates().x(), state->getCoordinates().y(), (int)(state->getPosition().z() * 3.28), (int)(state->getVelocity().magnitude() * 3.28), (int)state->getHeading(), 'H', 0, "00:00:00");
    bool b = radio->send(data, ENCT_TELEMETRY);
    return b;
}