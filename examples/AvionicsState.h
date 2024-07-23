#ifndef AVIONICSSTATE_H
#define AVIONICSSTATE_H

// Platformio is such a fucking pile of trash
#include "../src/State/State.h"
#include "../src/BlinkBuzz/BlinkBuzz.h"

namespace mmfs
{
    class AvionicsState : public State
    {
    public:
        AvionicsState(Sensor **sensors, int numSensors, KalmanInterface *kfilter, bool recordData);
        void updateState(double newTime = -1) override;

    private:
        char stages[6][15] = {"Pre-Flight", "Boosting", "Coasting", "Drogue Descent", "Main Descent", "Post-Flight"};
        void determineStage();
        int stage;
        double timeOfLaunch;
        double timeOfLastStage;
        double timeOfDay;
    };
}
#endif