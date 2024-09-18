#include "IMU.h"

namespace mmfs
{

    Quaternion IMU::getOrientationGlobal()
    {
        return orientationGlobal;
    }

    Vector<3> IMU::getAccelerationGlobal()
    {
        return accelerationGlobal;
    }

    Quaternion IMU::getOrientationLocal()
    {
        return orientationLocal;
    }

    Vector<3> IMU::getAngularVelocity()
    {
        return angularVelocity;
    }

    Vector<3> IMU::getMagnetometer()
    {
        return magnetometer;
    }

    Vector<3> convertToEuler(const Quaternion &orientation)

    {
        Vector<3> euler = orientation.toEuler();
        // reverse the vector, since it returns in z, y, x
        euler = Vector<3>(euler.x(), euler.y(), euler.z());
        return euler;
    }

    const char *IMU::getCsvHeader() const
    {                                                                                                          // incl I- for IMU
        return "I-AX (m/s/s),I-AY (m/s/s),I-AZ (m/s/s),I-ANGX,I-ANGY,I-ANGZ,I-MAGX,I-MAGY,I-MAGZ,I-QUATX,I-QUATY,I-QUATZ,I-QUATW,"; // trailing comma
    }

    const char *IMU::getDataString() const
    {
        sprintf(data, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", accelerationVec.x(), accelerationVec.y(), accelerationVec.z(), angularVelocity.x(), angularVelocity.y(), angularVelocity.z(), magnetometer.x(), magnetometer.y(), magnetometer.z(), orientation.x(), orientation.y(), orientation.z(), orientation.w()); // trailing comma"
        return data;
    }

    const char *IMU::getStaticDataString() const
    {
        sprintf(data, "Initial Magnetic Field (uT): %.2f,%.2f,%.2f\n", initialMagField.x(), initialMagField.y(), initialMagField.z());
        return data;
    }

    Quaternion IMU::quatInverse(Quaternion q)
    {
        // This is technically the quaternion conjugate. However,
        // for quaternions the conjugate is equivalent to the inverse.
        return Quaternion(q.w(), -q.x(), -q.y(), -q.z());
    }

    Quaternion IMU::orientationComplementaryFilter(double dt)
    {
        double theta = angularVelocity.magnitude() * dt;
        Vector<3> v = angularVelocity/angularVelocity.magnitude();
        Quaternion q_delta = Quaternion(cos(theta/2), v.x()*sin(theta/2), v.y()*sin(theta/2), v.z()*sin(theta/2));
        Quaternion q = q_delta * orientation;

        // Complementary Filter
        Quaternion accel_body = Quaternion(0, accelerationVec.x(), accelerationVec.y(), accelerationVec.z());
        Quaternion accel_interial = q * accel_body * quatInverse(q);
        v = Vector<3>(accel_interial.x()/accel_interial.magnitude(), accel_interial.y()/accel_interial.magnitude(), accel_interial.z()/accel_interial.magnitude());
        
        // To find phi need to divide by cos. Need to check that cos != 0.
        double phi;
        double tol = 1E-6;
        double cos_val = cos(v.x()); 
        if (fabs(cos_val) > tol) {   
            phi = 1 / cos_val;
        } else {
            phi = 3.14159265358979323846;
        }

        Vector<3> n = Vector<3>(v.y(), -v.x(), 0);
        q = Quaternion((1-alpha)*phi, n.x()/n.magnitude(), n.y()/n.magnitude(), n.z()/n.magnitude()) * q;

        return q;
    }
    void IMU::update()
    {
        read();
    }

    bool IMU::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        return init();
    }
}