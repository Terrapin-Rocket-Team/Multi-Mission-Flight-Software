#ifndef KALMANINTERFACE_H
#define KALMANINTERFACE_H

#include "LinearKalmanFilter.h"

namespace mmfs
{
    class KalmanInterface
    {
        // This class is an interface for the Kalman Filter class. Includes a base implementation for demo purposes.
    public:
        KalmanInterface(double inputSize, double measurementSize, double stateSize);
        virtual ~KalmanInterface();
        virtual double *iterate(double dt, double *input, double *measurement);
        virtual void initialize();
        virtual int getInputSize() const { return inputSize; }
        virtual int getMeasurementSize() const { return measurementSize; }
        virtual int getStateSize() const { return stateSize; }
        virtual Matrix setF(double dt);
        virtual Matrix setG(double dt);
        virtual Matrix setH();

    protected:
        LinearKalmanFilter *kf;
        double inputSize;
        double measurementSize;
        double stateSize;

    private:
    };
}

#endif