#include "IMU.h"
#include <stdio.h>

namespace mmfs
{
    void IMU::update()
    {
        read();
    }

    bool IMU::begin(bool useBiasCorrection)
    {
        biasCorrectionMode = useBiasCorrection;
        return init();
    }

    Quaternion IMU::getOrientation()
    {
        return orientation;
    }

    Vector<3> IMU::getAngularVelocity()
    {
        return angularVelocity;
    }

    Vector<3> IMU::getAngularVelocity()
    {
        return angularVelocity;
    }

    Vector<3> IMU::getMagField()
    {
        return magField;
    }

    Vector<3> IMU::getAcceleration()
    {
        return accelerationVec;
    }

    Vector<3> IMU::getAccelerationGlobal()
    {
        Quaternion accelBody = Quaternion(0, accelerationVec.x(), accelerationVec.y(), accelerationVec.z());
        Quaternion accelInterial = orientation.conjugate() * accelBody * orientation;
        return Vector<3>(accelInterial.x(), accelInterial.y(), accelInterial.z());
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
        sprintf(data, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", accelerationVec.x(), accelerationVec.y(), accelerationVec.z(), angularVelocity.x(), angularVelocity.y(), angularVelocity.z(), magField.x(), magField.y(), magField.z(), orientation.x(), orientation.y(), orientation.z(), orientation.w()); // trailing comma"
        return data;
    }

    const char *IMU::getStaticDataString() const
    {
        sprintf(data, "Initial Magnetic Field (uT): %.2f,%.2f,%.2f\n", initialMagField.x(), initialMagField.y(), initialMagField.z());
        return data;
    }

    void IMU::quaternionBasedComplimentaryFilter(double dt)
    {
        // See the Readme for details.
        // As a programmer this function takes in dt, angularVelocity, accelerationVec, magField,
        // and current orientation and propgated the orientation to the next time step.

        //----------------- GYRO ORIENTATION --------------//
        // 1. Determine rate of change of quaternion
        Quaternion w_b = Quaternion{0, angularVelocity};
        Quaternion q_w_BI = orientation.conjugate(); // Map from Inertia to body r_B = q_w_BI * r_I * q_w_BI^-1
        Quaternion q_w_BI_dot = w_b * q_w_BI * -.5;

        // 2. Propogate gyro orientation
        q_w_BI = q_w_BI + q_w_BI_dot * dt;
        q_w_BI.normalize();
        //-------------------------------------------------//

        //----------------- ACCEL ORIENTATION --------------//
        // 3. Find the predicted gravity vector in the inertial frame
        Quaternion a_b = Quaternion{0, accelerationVec};
        a_b.normalize();
        Quaternion g_p_I = q_w_BI.conjugate() * a_b * q_w_BI;

        // 4. Determine delta acc orientation
        if(g_p_I.z() == -1){
            // TODO this shouldn't happen though
            // This prob needs some tolerance too

            // Temp solution (I just made this up lol)
            g_p_I.z() = -.9;
            g_p_I.w() = .01;
            g_p_I.normalize();
        }

        Quaternion delta_q_acc = Quaternion{sqrt((g_p_I.z()+1)/2), -g_p_I.y()/sqrt(2*(g_p_I.z()+1)), g_p_I.x()/sqrt(2*(g_p_I.z()+1)), 0};

        // 5. Adaptive gain interpolation to reduce high freq noise
        double accel_best_filtering_at_static = .9; // TODO this is tunable parameter that needs to be validated
        double alpha = adaptiveAccelGain(accel_best_filtering_at_static, .1, .2);
        delta_q_acc = delta_q_acc.interpolation(Quaternion{1,0,0,0}, alpha, .9);

        // 6. Combine with gryo orientation to correct roll and pitch
        Quaternion q_wa_BI = q_w_BI * delta_q_acc;
        q_wa_BI.normalize();
        //-------------------------------------------------//

        //----------------- MAG ORIENTATION --------------//

        // 7. Get magnetic field vector in interial frame
        Quaternion m_b = Quaternion{0, magField};
        Quaternion m_I = q_wa_BI.conjugate() * m_b * q_wa_BI;

        // 8. Determine delta mag orientation
        double Gamma = m_I.x()*m_I.x() + m_I.y()*m_I.y();
        if(Gamma == 0){
            // Mag Field has no yaw contribution to be made
            orientation = q_wa_BI.conjugate();
            return;
        }
        Quaternion delta_q_mag = Quaternion{sqrt(Gamma + (m_I.x()*sqrt(Gamma)))/sqrt(2*Gamma), 0, 0, m_I.y()/sqrt(2*(Gamma+ (m_I.x()*sqrt(Gamma))))};

        // 9. Interpolation to reduce high freq noise 
        double mag_best_filtering_at_static = .9; // TODO this is tunable parameter that needs to be validated
        delta_q_mag = delta_q_mag.interpolation(Quaternion{1,0,0,0}, mag_best_filtering_at_static, .9);

        // 10. Combine with gryo/acc orientation to correct yaw
        Quaternion q_BI = q_wa_BI * delta_q_mag;
        q_BI.normalize();
        //-------------------------------------------------//

        orientation = q_BI.conjugate();
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
}