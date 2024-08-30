#include "LinearKalmanFilter.h"

namespace mmfs {

LinearKalmanFilter::LinearKalmanFilter(int measurementSize, int controlSize, int stateSize)
{
    if (measurementSize <= 0 || controlSize <= 0 || stateSize <= 0) {
        throw std::invalid_argument("Matrix sizes must be positive integers.");
    }

    // Initialize with zeros
    X = Matrix(stateSize, 1, new double[stateSize]()); // State vector
    P = Matrix(stateSize, stateSize, new double[stateSize * stateSize]()); // Error covariance matrix

    // Identity matrices
    K = Matrix(stateSize, measurementSize, new double[stateSize * measurementSize]()); // Kalman gain

    this->measurementSize = measurementSize;
    this->controlSize = controlSize;
    this->stateSize = stateSize;
}

LinearKalmanFilter::LinearKalmanFilter(Matrix X, Matrix P, Matrix K)
{
    this->X = X;
    this->P = P;
    this->K = K;

    this->measurementSize = K.getCols();
    this->controlSize = K.getRows();
}

LinearKalmanFilter::LinearKalmanFilter(Matrix X, Matrix P)
{
    this->X = X;
    this->P = P;
    this->K = Matrix(X.getRows(), P.getRows(), new double[X.getRows() * P.getRows()]());

    this->measurementSize = K.getCols();
    this->controlSize = K.getRows();
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

Matrix LinearKalmanFilter::iterate(Matrix measurement, Matrix control, double dt) {
    predictState(control);
    calculateKalmanGain();
    estimateState(measurement);
    covarianceUpdate();
    covarianceExtrapolate();
    return X;
}

double* LinearKalmanFilter::iterate(double time, double* state, double* measurements, double* controlVars) {
    // Convert arrays to matrices
    Matrix stateMatrix(stateSize, 1, state);
    Matrix measurementMatrix(measurementSize, 1, measurements);
    Matrix controlMatrix(controlSize, 1, controlVars);

    // Kalman Filter steps
    predictState(controlMatrix);
    calculateKalmanGain();
    estimateState(measurementMatrix);
    covarianceUpdate();
    covarianceExtrapolate();

    // Update the state array with the new state values
    for (int i = 0; i < stateSize; ++i) {
        state[i] = X.get(i, 0);
    }

    return state;
}

void LinearKalmanFilter::predictState(Matrix U) {
    X = getF(0.1) * X + getG(0.1) * U;
}

void LinearKalmanFilter::estimateState(Matrix measurement) {
    X = X + K * (measurement - getH() * X);
}

void LinearKalmanFilter::calculateKalmanGain() {
    K = P * getH().transpose() * (getH() * P * getH().transpose() + getR()).inverse();
}

void LinearKalmanFilter::covarianceUpdate() {
    int n = X.getRows();
    P = (Matrix::ident(n) - K * getH()) * P * (Matrix::ident(n) - K * getH()).transpose() + K * getR() * K.transpose();
}

void LinearKalmanFilter::covarianceExtrapolate() {
    P = getF(0.1) * P * getF(0.1).transpose() + getQ();
}

} // namespace mmfs
