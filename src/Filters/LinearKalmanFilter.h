#ifndef LINEAR_KALMAN_FILTER_H
#define LINEAR_KALMAN_FILTER_H

#include "Filter.h"
#include "../Math/Matrix.h"

namespace mmfs {

class LinearKalmanFilter : public Filter {
public:
    LinearKalmanFilter();
    virtual ~LinearKalmanFilter() = default;

    // Virtual getter methods for matrices, to be overridden by subclasses
    virtual Matrix getF(double dt);
    virtual Matrix getG(double dt);
    virtual Matrix getH();
    virtual Matrix getR();
    virtual Matrix getQ();

    Matrix iterate(const Matrix& measurement, const Matrix& control, double dt);

protected:
    void predictState();
    void estimateState(const Matrix& measurement);
    void calculateKalmanGain();
    void covarianceUpdate();
    void covarianceExtrapolate();
};

} // namespace mmfs

#endif // LINEAR_KALMAN_FILTER_H
