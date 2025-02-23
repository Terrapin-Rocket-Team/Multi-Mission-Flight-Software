#include "LinearKalmanFilter.h"

namespace mmfs {

LinearKalmanFilter::LinearKalmanFilter(int measurementSize, int controlSize, int stateSize)
{

    // Initialize with zeros (new keyword does value-initialization in C++03 5.3.4)
    X = Matrix(stateSize, 1, new double[stateSize]()); // State vector
    P = Matrix(stateSize, stateSize, new double[stateSize * stateSize]()); // Error covariance matrix
    K = Matrix(stateSize, measurementSize, new double[stateSize * measurementSize]()); // Kalman gain

    this->measurementSize = measurementSize;
    this->controlSize = controlSize;
    this->stateSize = stateSize;
}

LinearKalmanFilter::LinearKalmanFilter(Matrix X, Matrix P)
{
    this->X = X;
    this->P = P;
    this->K = Matrix(X.getRows(), P.getRows(), new double[X.getRows() * P.getRows()]());

    this->measurementSize = K.getCols();
    this->controlSize = K.getRows();
}


Matrix LinearKalmanFilter::iterate(double dt, Matrix measurement, Matrix control) {
    predictState(dt, control);
    calculateKalmanGain();
    estimateState(measurement);
    covarianceUpdate();
    covarianceExtrapolate(dt);
    return X;
}

double* LinearKalmanFilter::iterate(double dt, double* state, double* measurements, double* controlVars) {
    // Convert arrays to matrices
    // Matrix stateMatrix(stateSize, 1, state);
    Matrix measurementMatrix(measurementSize, 1, measurements);
    Matrix controlMatrix(controlSize, 1, controlVars);

    // Kalman Filter steps
    predictState(dt, controlMatrix);
    calculateKalmanGain();
    estimateState(measurementMatrix);
    covarianceUpdate();
    covarianceExtrapolate(dt);

    // Update the state array with the new state values
    for (int i = 0; i < stateSize; ++i) {
        state[i] = X.get(i, 0);
    }

    return state;
}

void LinearKalmanFilter::predictState(double dt, Matrix U) {
    X = getF(dt) * X + getG(dt) * U;
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

void LinearKalmanFilter::covarianceExtrapolate(double dt) {
    P = getF(dt) * P * getF(dt).transpose() + getQ(dt);
}

} // namespace mmfs
