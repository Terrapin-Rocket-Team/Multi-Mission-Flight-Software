#include "LinearKalmanFilter.h"

namespace mmfs
{

    // Constructor initializes sizes
    LinearKalmanFilter::LinearKalmanFilter(Matrix X, Matrix U, Matrix P, Matrix F, Matrix G, Matrix R, Matrix Q)
    {
        state.X = X;
        state.U = U;
        state.P = P;
        state.F = F;
        state.G = G;
        state.R = R;
        state.Q = Q;
        predict_state();
        covariance_extrapolate();
    }

    // Predicts the next state
    void LinearKalmanFilter::predict_state()
    {
        state.X = (state.F * state.X) + (state.G * state.U);
    }

    // Estimates the state based on measurements
    void LinearKalmanFilter::estimate_state(Matrix measurement)
    {
        state.X = state.X + state.K * (measurement - state.H * state.X);
    }

    // Calculates the Kalman gain
    void LinearKalmanFilter::calculate_kalman_gain()
    {
        state.K = state.P * state.H.T() * (state.H * state.P * state.H.T() + state.R).inv();
    }

    // Updates the covariance matrix
    void LinearKalmanFilter::covariance_update()
    {
        int n = state.X.getRows();
        state.P = (Matrix::ident(n) - state.K * state.H) * state.P * (Matrix::ident(n) - state.K * state.H).T() + state.K * state.R * state.K.T();
    }

    // Extrapolates the covariance matrix
    void LinearKalmanFilter::covariance_extrapolate()
    {
        state.P = state.F * state.P * state.F.T() + state.Q;
    }

    // Iterate through the Kalman Filter steps
    double *LinearKalmanFilter::iterate(double time, double *controlVars, double *measurements, double *stateArr)
    {
        // Convert the raw data into matrices
        Matrix control = Matrix(state.U.getRows(), 1, controlVars);
        Matrix measurement = Matrix(state.H.getRows(), 1, measurements);

        // Apply Kalman Filter steps
        predict_state();
        covariance_extrapolate();
        calculate_kalman_gain();
        estimate_state(measurement);
        covariance_update();

        // Update the stateArr pointer with the new state values
        double *result = state.X.getArr();
        for (int i = 0; i < state.X.getRows(); ++i)
        {
            stateArr[i] = result[i];
        }

        return stateArr; // Return the updated state
    }

    // Returns the measurement size
    int LinearKalmanFilter::getMeasurementSize() const
    {
        return state.H.getRows();
    }

    // Returns the input size
    int LinearKalmanFilter::getInputSize() const
    {
        return state.G.getCols();
    }

} // namespace mmfs
