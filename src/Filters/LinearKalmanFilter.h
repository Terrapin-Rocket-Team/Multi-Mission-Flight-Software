#ifndef LINEARKALMANFILTER_H
#define LINEARKALMANFILTER_H

#include "Filter.h"
#include "../Math/Matrix.h"

namespace mmfs {

class LinearKalmanFilter : public Filter {
protected:
    struct State {
        Matrix X; // State vector
        Matrix U; // Control input
        Matrix P; // Covariance matrix
        Matrix F; // State transition matrix
        Matrix G; // Control input model
        Matrix Q; // Process noise covariance
        Matrix R; // Measurement noise covariance
        Matrix H; // Measurement model
        Matrix K; // Kalman gain
    } state;

    // Customizable methods for subclasses
    virtual Matrix setF(double dt) = 0;
    virtual Matrix setG(double dt) = 0;
    virtual Matrix setH() = 0;
    virtual void calculate_initial_values() = 0;

public:
    LinearKalmanFilter::LinearKalmanFilter(Matrix X, Matrix U, Matrix P, Matrix F, Matrix G, Matrix R, Matrix Q);

    void predict_state();
    void estimate_state(Matrix measurement);
    void calculate_kalman_gain();
    void covariance_update();
    void covariance_extrapolate();

    double* iterate(double time, double* controlVars, double* measurements, double* stateArr) override;

    int getMeasurementSize() const override;
    int getInputSize() const override;
};

} // namespace mmfs

#endif // LINEARKALMANFILTER_H