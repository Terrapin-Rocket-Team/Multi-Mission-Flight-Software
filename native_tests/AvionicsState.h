#ifndef AVIONICSSTATE_H
#define AVIONICSSTATE_H

#include <State/State.h>
#include <BlinkBuzz/BlinkBuzz.h>
#include <Filters/LinearKalmanFilter.h>

namespace mmfs
{
    class AvionicsState : public State
    {
    public:
        AvionicsState(Sensor **sensors, int numSensors, LinearKalmanFilter *kfilter, bool stateRecordsOwnData = true);
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