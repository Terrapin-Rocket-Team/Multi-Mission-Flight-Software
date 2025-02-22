//  Inertial Measurement Unit Maths Library
//
//  Copyright 2013-2021 Sam Cowen <samuel.cowen@camelsoftware.com>
//  Bug fixes and cleanups by Gé Vissers (gvissers@gmail.com)
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.


// COPIED NEARLY DIRECTLY FROM ADAFRUIT BNO055 LIBRARY
#ifndef QUATERNION_H
#define QUATERNION_H

//#include <math.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <string.h>

#include "Vector.h"
#include "Matrix.h"

namespace mmfs {

class Quaternion {
public:
  Quaternion() : _w(1.0), _x(0.0), _y(0.0), _z(0.0) {}

  Quaternion(double w, double x, double y, double z)
      : _w(w), _x(x), _y(y), _z(z) {}

  Quaternion(double w, Vector<3> vec)
      : _w(w), _x(vec.x()), _y(vec.y()), _z(vec.z()) {}

  double &w() { return _w; }
  double &x() { return _x; }
  double &y() { return _y; }
  double &z() { return _z; }

  double w() const { return _w; }
  double x() const { return _x; }
  double y() const { return _y; }
  double z() const { return _z; }

  double magnitude() const {
    return sqrt(_w * _w + _x * _x + _y * _y + _z * _z);
  }

  void normalize() {
    double mag = magnitude();
    *this = this->scale(1 / mag);
  }

  Quaternion conjugate() const { return Quaternion(_w, -_x, -_y, -_z); }

  void fromAxisAngle(const Vector<3> &axis, double theta) {
    _w = cos(theta / 2);
    // only need to calculate sine of half theta once
    double sht = sin(theta / 2);
    _x = axis.x() * sht;
    _y = axis.y() * sht;
    _z = axis.z() * sht;
  }


  void fromMatrix(Matrix &m) {
    // Ensure that the matrix is 3x3
    if (m.getRows() != 3 || m.getCols() != 3) {
      char formattedMessage[256];
      std::snprintf(formattedMessage, sizeof(formattedMessage), "Tried to convert a non3x3 matrix into a quaterion. Input matrix was a %dx%d. Returning previous quaternion.", m.getRows(), m.getCols());
      errorHandler.addError(SOFTWARE_ERROR, formattedMessage);
      return;
    }

    double tr = m.trace();

    double S;
    if (tr > 0) {
      S = sqrt(tr + 1.0) * 2;
      _w = 0.25 * S;
      _x = (m.get(2, 1) - m.get(1, 2)) / S;
      _y = (m.get(0, 2) - m.get(2, 0)) / S;
      _z = (m.get(1, 0) - m.get(0, 1)) / S;
    } else if (m.get(0, 0) > m.get(1, 1) && m.get(0, 0) > m.get(2, 2)) {
      S = sqrt(1.0 + m.get(0, 0) - m.get(1, 1) - m.get(2, 2)) * 2;
      _w = (m.get(2, 1) - m.get(1, 2)) / S;
      _x = 0.25 * S;
      _y = (m.get(0, 1) + m.get(1, 0)) / S;
      _z = (m.get(0, 2) + m.get(2, 0)) / S;
    } else if (m.get(1, 1) > m.get(2, 2)) {
        S = sqrt(1.0 + m.get(1, 1) - m.get(0, 0) - m.get(2, 2)) * 2;
        _w = (m.get(0, 2) - m.get(2, 0)) / S;
        _x = (m.get(0, 1) + m.get(1, 0)) / S;
        _y = 0.25 * S;
        _z = (m.get(1, 2) + m.get(2, 1)) / S;
    } else {
        S = sqrt(1.0 + m.get(2, 2) - m.get(0, 0) - m.get(1, 1)) * 2;
        _w = (m.get(1, 0) - m.get(0, 1)) / S;
        _x = (m.get(0, 2) + m.get(2, 0)) / S;
        _y = (m.get(1, 2) + m.get(2, 1)) / S;
        _z = 0.25 * S;
    }
  }

  void toAxisAngle(Vector<3> &axis, double &angle) const {
    double sqw = sqrt(1 - _w * _w);
    if (sqw == 0) // it's a singularity and divide by zero, avoid
      return;

    angle = 2 * acos(_w);
    axis.x() = _x / sqw;
    axis.y() = _y / sqw;
    axis.z() = _z / sqw;
  }

  Matrix toMatrix() const {
    double dcm00 = 1 - 2 * _y * _y - 2 * _z * _z;
    double dcm01 = 2 * _x * _y - 2 * _w * _z;
    double dcm02 = 2 * _x * _z + 2 * _w * _y;
    double dcm10 = 2 * _x * _y + 2 * _w * _z;
    double dcm11 = 1 - 2 * _x * _x - 2 * _z * _z;
    double dcm12 = 2 * _y * _z - 2 * _w * _x;
    double dcm20 = 2 * _x * _z - 2 * _w * _y;
    double dcm21 = 2 * _y * _z + 2 * _w * _x;
    double dcm22 = 1 - 2 * _x * _x - 2 * _y * _y;

    double* dcmArray = new double[9]{dcm00, dcm10, dcm20, dcm01, dcm11, dcm21, dcm02, dcm12, dcm22};
    Matrix dcm(3, 3, dcmArray);
    return dcm;
  }

  // Returns euler angles that represent the quaternion.  Angles are
  // returned in rotation order and right-handed about the specified
  // axes:
  //
  //   v[0] is applied 1st about z (ie, roll)
  //   v[1] is applied 2nd about y (ie, pitch)
  //   v[2] is applied 3rd about x (ie, yaw)
  //
  // Note that this means result.x() is not a rotation about x;
  // similarly for result.z().
  //
  Vector<3> toEuler() const {
    // TODO what Euler is this? See comment above, I think it is 1-2-3
    Vector<3> ret;
    double sqw = _w * _w;
    double sqx = _x * _x;
    double sqy = _y * _y;
    double sqz = _z * _z;

    ret.x() = atan2(2.0 * (_x * _y + _z * _w), (sqx - sqy - sqz + sqw));
    ret.y() = asin(-2.0 * (_x * _z - _y * _w) / (sqx + sqy + sqz + sqw));
    ret.z() = atan2(2.0 * (_y * _z + _x * _w), (-sqx - sqy + sqz + sqw));

    return ret;
  }

  Vector<3> toAngularVelocity(double dt) const {
    Vector<3> ret;
    Quaternion one(1.0, 0.0, 0.0, 0.0);
    Quaternion delta = one - *this;
    Quaternion r = (delta / dt);
    r = r * 2;
    r = r * one;

    ret.x() = r.x();
    ret.y() = r.y();
    ret.z() = r.z();
    return ret;
  }

  Vector<3> rotateVector(const Vector<2> &v) const {
    return rotateVector(Vector<3>(v.x(), v.y()));
  }

  Vector<3> rotateVector(const Vector<3> &v) const {
    Vector<3> qv(_x, _y, _z);
    Vector<3> t = qv.cross(v) * 2.0;
    return v + t * _w + qv.cross(t);
  }

  Quaternion operator*(const Quaternion &q) const {
    return Quaternion(_w * q._w - _x * q._x - _y * q._y - _z * q._z,
                      _w * q._x + _x * q._w + _y * q._z - _z * q._y,
                      _w * q._y - _x * q._z + _y * q._w + _z * q._x,
                      _w * q._z + _x * q._y - _y * q._x + _z * q._w);
  }

  Quaternion operator+(const Quaternion &q) const {
    return Quaternion(_w + q._w, _x + q._x, _y + q._y, _z + q._z);
  }

  Quaternion operator-(const Quaternion &q) const {
    return Quaternion(_w - q._w, _x - q._x, _y - q._y, _z - q._z);
  }

  Quaternion operator/(double scalar) const {
    return Quaternion(_w / scalar, _x / scalar, _y / scalar, _z / scalar);
  }

  Quaternion operator*(double scalar) const { return scale(scalar); }

  Quaternion scale(double scalar) const {
    return Quaternion(_w * scalar, _x * scalar, _y * scalar, _z * scalar);
  }

  Quaternion interpolation(Quaternion q, double alpha, double epsilon = .9)
  {
    // Source: https://ahrs.readthedocs.io/en/latest/filters/aqua.html#ahrs.filters.aqua.slerp_I
    // Takes the current quaternion and an input quaternion (q) and performs either a 
    // Linear intERPolation (LERP) or Spherical Linear intERPolation (SLERP) based on a threshold value.
    // Inputs
    // q: Input quaternion to interpolate the current quaterion with. Generally this is the identity quaternion [1 0 0 0] (w first)
    // alpha: Gain [0 1]. Decides how close the output should be to the input quaternion. A lower alpha means closer to q.
    // epsilon: Cutoff. Generally .9. Lower than the threshold, use SLERP. Higher use LERP.

    Quaternion p = Quaternion{_w, _x, _y, _z}; // current quaternion
    Quaternion r; // result

    if(p.w() > epsilon)
    {
      // LERP
      r = q*(1-alpha) + p*alpha;
      r.normalize();
      return r;
    }

    double omega = acos(p.w());
    r = q*((sin((1-alpha)*omega))/(sin(omega))) + p*(sin(alpha*omega)/sin(omega));
    return r;
  }

private:
  double _w, _x, _y, _z;
};

} // namespace imu

#endif
