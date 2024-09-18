#ifndef LINEAR_KALMAN_FILTER_H
#define LINEAR_KALMAN_FILTER_H

#include "Filter.h"
#include "../Math/Matrix.h"

namespace mmfs {

class LinearKalmanFilter : public Filter {
public:

    int measurementSize;
    int controlSize;
    int stateSize;

    // Constructors
    LinearKalmanFilter(int measurementSize, int controlSize, int stateSize);
    LinearKalmanFilter(Matrix X, Matrix P);
    virtual ~LinearKalmanFilter() = default;

    // Virtual getter methods for matrices, to be overridden by subclasses
    virtual void initialize() = 0;
    virtual Matrix getF(double dt) = 0;
    virtual Matrix getG(double dt) = 0;
    virtual Matrix getH() = 0;
    virtual Matrix getR() = 0;
    virtual Matrix getQ() = 0;

    Matrix iterate(double dt, Matrix measurement, Matrix control);

    // Override core interface methods
    int getMeasurementSize() const override { return measurementSize; }
    int getInputSize() const override { return controlSize; }
    int getStateSize() const override { return stateSize; }
    double* iterate(double dt, double* state, double* measurements, double* controlVars) override;





protected:
    // Instance variables to store matrices
    Matrix X; // State vector
    Matrix P; // Error covariance matrix
    Matrix K; // Kalman gain
    
    void predictState(double dt, Matrix control);
    void estimateState(Matrix measurement);
    void calculateKalmanGain();
    void covarianceUpdate();
    void covarianceExtrapolate(double dt);

};

} // namespace mmfs

#endif // LINEAR_KALMAN_FILTER_H
