#include "LinearKalmanFilter.h"

namespace mmfs {

LinearKalmanFilter::LinearKalmanFilter() {
    // Initialize as necessary
}

Matrix LinearKalmanFilter::getF(double dt) {
    // Default implementation of F matrix (state transition)
    double *data = new double[36]{
        1.0, 0, 0, dt, 0, 0,
        0, 1.0, 0, 0, dt, 0,
        0, 0, 1.0, 0, 0, dt,
        0, 0, 0, 1.0, 0, 0,
        0, 0, 0, 0, 1.0, 0,
        0, 0, 0, 0, 0, 1.0
    };
    return Matrix(6, 6, data);
}

Matrix LinearKalmanFilter::getG(double dt) {
    // Default implementation of G matrix (control)
    double *data = new double[18]{
        0.5 * dt * dt, 0, 0,
        0, 0.5 * dt * dt, 0,
        0, 0, 0.5 * dt * dt,
        dt, 0, 0,
        0, dt, 0,
        0, 0, dt
    };
    return Matrix(6, 3, data);
}

Matrix LinearKalmanFilter::getH() {
    // Default implementation of H matrix (measurement)
    double *data = new double[18]{
        1.0, 0, 0, 0, 0, 0,
        0, 1.0, 0, 0, 0, 0,
        0, 0, 1.0, 0, 0, 0
    };
    return Matrix(3, 6, data);
}

Matrix LinearKalmanFilter::getR() {
    // Default implementation of R matrix (measurement noise)
    double *data = new double[9]{
        1.0, 0, 0,
        0, 1.0, 0,
        0, 0, 0.5
    };
    return Matrix(3, 3, data);
}

Matrix LinearKalmanFilter::getQ() {
    // Default implementation of Q matrix (process noise)
    double *data = new double[36]{
        0.1, 0, 0, 0, 0, 0,
        0, 0.1, 0, 0, 0, 0,
        0, 0, 0.1, 0, 0, 0,
        0, 0, 0, 0.1, 0, 0,
        0, 0, 0, 0, 0.1, 0,
        0, 0, 0, 0, 0, 0.1
    };
    return Matrix(6, 6, data);
}

Matrix LinearKalmanFilter::iterate(const Matrix& measurement, const Matrix& control, double dt) {
    predictState();
    calculateKalmanGain();
    estimateState(measurement);
    covarianceUpdate();
    covarianceExtrapolate();
    return state.X;
}

void LinearKalmanFilter::predictState() {
    state.X = getF(0.1) * state.X + getG(0.1) * state.U;
}

void LinearKalmanFilter::estimateState(const Matrix& measurement) {
    state.X = state.X + state.K * (measurement - getH() * state.X);
}

void LinearKalmanFilter::calculateKalmanGain() {
    state.K = state.P * getH().transpose() * (getH() * state.P * getH().transpose() + getR()).inverse();
}

void LinearKalmanFilter::covarianceUpdate() {
    int n = state.X.getRows();
    state.P = (Matrix::ident(n) - state.K * getH()) * state.P * (Matrix::ident(n) - state.K * getH()).transpose() + state.K * getR() * state.K.transpose();
}

void LinearKalmanFilter::covarianceExtrapolate() {
    state.P = getF(0.1) * state.P * getF(0.1).transpose() + getQ();
}

} // namespace mmfs
