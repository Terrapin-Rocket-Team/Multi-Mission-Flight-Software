#include "KalmanInterface.h"

namespace mmfs
{

  KalmanInterface::KalmanInterface(double inputSize, double measurementSize, double stateSize)
  {
    this->inputSize = inputSize;
    this->measurementSize = measurementSize;
    this->stateSize = stateSize;
  }
  KalmanInterface::~KalmanInterface()
  {
    delete kf;
  }
  ::Matrix KalmanInterface::setF(double dt)
  {
    double *result = new double[36]{
        1.0, 0, 0, dt, 0, 0,
        0, 1.0, 0, 0, dt, 0,
        0, 0, 1.0, 0, 0, dt,
        0, 0, 0, 1.0, 0, 0,
        0, 0, 0, 0, 1.0, 0,
        0, 0, 0, 0, 0, 1.0};
    return ::Matrix(6, 6, result);
  }

  ::Matrix KalmanInterface::setG(double dt)
  {
    double *result = new double[18]{
        0.5 * dt * dt, 0, 0,
        0, 0.5 * dt * dt, 0,
        0, 0, 0.5 * dt * dt,
        dt, 0, 0,
        0, dt, 0,
        0, 0, dt};
    return ::Matrix(6, 3, result);
  }

  ::Matrix KalmanInterface::setH()
  {
    double *result = new double[18]{
        1.0, 0, 0, 0, 0, 0,
        0, 1.0, 0, 0, 0, 0,
        0, 0, 1.0, 0, 0, 0};
    return ::Matrix(3, 6, result);
  }

  void KalmanInterface::initialize()
  {
    double pv = 100.0;
    double *x = new double[6]{0, 0, 0, 0, 0, 0};
    ::Matrix *X = new ::Matrix(6, 1, x);
    double *u = new double[3]{0, 0, 0};
    ::Matrix *U = new ::Matrix(3, 1, u);
    double *p = new double[36]{
        pv, 0, 0, pv, 0, 0,
        0, pv, 0, 0, pv, 0,
        0, 0, pv, 0, 0, pv,
        pv, 0, 0, pv, 0, 0,
        0, pv, 0, 0, pv, 0,
        0, 0, pv, 0, 0, pv};
    ::Matrix *P = new ::Matrix(6, 6, p);
    double *r = new double[16]{
        1.0, 0, 0,
        0, 1.0, 0,
        0, 0, 0.5};

    ::Matrix *R = new ::Matrix(3, 3, r);
    double qv = 0.1;
    double *q = new double[36]{
        qv, 0, 0, 0, 0, 0,
        0, qv, 0, 0, 0, 0,
        0, 0, qv, 0, 0, 0,
        0, 0, 0, qv, 0, 0,
        0, 0, 0, 0, qv, 0,
        0, 0, 0, 0, 0, qv};
    // ::Matrix *Q = new ::Matrix(6, 6, q);
    kf = new LinearKalmanFilter(*X, *U, *P, setG(0.1), setF(0.1), *R);
  }

  double *KalmanInterface::iterate(double dt, double *input, double *measurement)
  {
    ::Matrix meas = ::Matrix(3, 1, measurement);
    ::Matrix inp = ::Matrix(3, 1, input);
    ::Matrix state = kf->iterate(meas, inp, setF(dt), setG(dt), setH());
    double *ret = new double[6];
    double *st = state.getArr();
    for (int i = 0; i < 6; ++i)
    {
      ret[i] = st[i];
    }
    return ret;
  }
}