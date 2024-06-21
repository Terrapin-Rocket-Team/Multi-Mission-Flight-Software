#ifndef AVIONICSSTATE_H
#define AVIONICSSTATE_H

//Platformio is such a fucking pile of trash
#include <State/State.h>
#include <BlinkBuzz.h>

class AvionicsState : public State
{
    public:
        // TODO: Change the SensorType enum to just an array of sensors, removing the addSensor() method.
        AvionicsState(int numSensors, SensorType *sensorOrder, KalmanInterface *kfilter, bool recordData);
        void updateState(double newTime = -1) override;

    private:
        char stages[6][15] = {"Pre-Flight", "Boosting", "Coasting", "Drogue Descent", "Main Descent", "Post-Flight"};
        void determineStage();
        int stage;
        double timeOfLaunch;
        double timeOfLastStage;
        double timeOfDay;

};
#endif