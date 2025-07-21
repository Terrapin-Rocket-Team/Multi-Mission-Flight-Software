#include "IMU.h"

namespace mmfs
{

    IMU::IMU(const char *name) : Sensor("IMU", name)
    {
        addColumn(DOUBLE, &measuredAcc.x(), "AccX (m/s^2)");
        addColumn(DOUBLE, &measuredAcc.y(), "AccY (m/s^2)");
        addColumn(DOUBLE, &measuredAcc.z(), "AccZ (m/s^2)");
        addColumn(DOUBLE, &measuredGyro.x(), "GyroX (rad/s)");
        addColumn(DOUBLE, &measuredGyro.y(), "GyroY (rad/s)");
        addColumn(DOUBLE, &measuredGyro.z(), "GyroZ (rad/s)");
        addColumn(DOUBLE, &measuredMag.x(), "MagX (uT)");
        addColumn(DOUBLE, &measuredMag.y(), "MagY (uT)");
        addColumn(DOUBLE, &measuredMag.z(), "MagZ (uT)");
        addColumn(DOUBLE, &orientation.x(), "OriX");
        addColumn(DOUBLE, &orientation.y(), "OriY");
        addColumn(DOUBLE, &orientation.z(), "OriZ");
        addColumn(DOUBLE, &orientation.w(), "OriW");
    }
    // Quaternion IMU::getOrientation()
    // {
    //     return orientation;
    // }

    Vector<3> IMU::getAngularVelocity()
    {
        return measuredGyro;
    }

    Vector<3> IMU::getMagField()
    {
        return measuredMag;
    }

    Vector<3> IMU::getAcceleration()
    {
        return measuredAcc;
    }

    // Vector<3> IMU::getAccelerationGlobal()
    // {
    //     Quaternion accelInterial = orientation * Quaternion(0, measuredAcc) * orientation.conjugate();
    //     return Vector<3>(accelInterial.x(), accelInterial.y(), accelInterial.z());
    // }

    // void IMU::quaternionBasedComplimentaryFilterSetup()
    // {
    //     // Find initial absolute orientation which is just q = q_acc * q_mag
    //     // Get Accelerometer Orientation
    //     Quaternion a_b = Quaternion{0, measuredAcc};
    //     if (!(a_b.magnitude() > 0))
    //     {
    //         getLogger().recordLogData(ERROR_, "Acceleration magnitude 0 while running quaternionBasedComplimentaryFilterSetup(). Need to record acceleration vector on setup before running this function.");
    //         return;
    //     }
    //     a_b.normalize();

    //     Quaternion q_acc;
    //     if (a_b.z() > 0)
    //     {
    //         q_acc = Quaternion{sqrt((a_b.z() + 1) / 2), -a_b.y() / sqrt(2 * (a_b.z() + 1)), a_b.x() / sqrt(2 * (a_b.z() + 1)), 0};
    //     }
    //     else
    //     {
    //         q_acc = Quaternion{-a_b.y() / sqrt(2 * (1 - a_b.z())), sqrt((1 - a_b.z()) / 2), 0, a_b.x() / sqrt(2 * (1 - a_b.z()))};
    //     }

    //     // Get Magnetometer Orientation
    //     Quaternion m_b = Quaternion{0, measuredMag};
    //     if (!(m_b.magnitude() > 0))
    //     {
    //         getLogger().recordLogData(ERROR_, "Magnetic magnitude 0 while running quaternionBasedComplimentaryFilterSetup(). Need to record magnetometer vector on setup before running this function.");
    //         return;
    //     }
    //     m_b.normalize();

    //     double Gamma = m_b.x() * m_b.x() + m_b.y() * m_b.y();

    //     Quaternion q_mag;
    //     if (m_b.x() > 0)
    //     {
    //         q_mag = Quaternion{sqrt(Gamma + (m_b.x() * sqrt(Gamma))) / sqrt(2 * Gamma), 0, 0, m_b.y() / sqrt(2 * (Gamma + (m_b.x() * sqrt(Gamma))))};
    //     }
    //     else
    //     {
    //         q_mag = Quaternion{m_b.y() / sqrt(2 * (Gamma - (m_b.x() * sqrt(Gamma)))), 0, 0, sqrt(Gamma - (m_b.x() * sqrt(Gamma))) / sqrt(2 * Gamma)};
    //     }

    //     Quaternion q = q_acc * q_mag;

    //     orientation = q.conjugate();
    // }

    // void IMU::quaternionBasedComplimentaryFilter(double dt)
    // {
    //     // See the Readme for details.
    //     // As a programmer this function takes in dt (seconds), measuredGyro, measuredAcc, measuredMag,
    //     // and current orientation and propagated the orientation to the next time step.

    //     //----------------- GYRO ORIENTATION --------------//
    //     // 1. Determine rate of change of quaternion
    //     Quaternion w_b = Quaternion{0, measuredGyro};
    //     Quaternion q_w_BI = orientation.conjugate(); // Map from Inertia to body r_B = q_w_BI * r_I * q_w_BI^-1
    //     Quaternion q_w_BI_dot = w_b * q_w_BI * -.5;

    //     // 2. Propagate gyro orientation
    //     q_w_BI = q_w_BI + q_w_BI_dot * dt;
    //     q_w_BI.normalize();
    //     //-------------------------------------------------//

    //     //----------------- ACCEL ORIENTATION --------------//
    //     // 3. Find the predicted gravity vector in the inertial frame
    //     Quaternion a_b = Quaternion{0, measuredAcc};
    //     if (!(a_b.magnitude() > 0))
    //     {
    //         // Accel has no roll or pitch contribution to be made
    //         orientation = q_w_BI.conjugate();
    //         return;
    //     }
    //     a_b.normalize();
    //     Quaternion g_p_I = q_w_BI.conjugate() * a_b * q_w_BI;
    //     // 4. Determine delta acc orientation
    //     Quaternion delta_q_acc;
    //     if (g_p_I.z() > 0)
    //     {
    //         delta_q_acc = Quaternion{sqrt((g_p_I.z() + 1) / 2), -g_p_I.y() / sqrt(2 * (g_p_I.z() + 1)), g_p_I.x() / sqrt(2 * (g_p_I.z() + 1)), 0};
    //     }
    //     else
    //     {
    //         delta_q_acc = Quaternion{-g_p_I.y() / sqrt(2 * (1 - g_p_I.z())), sqrt((1 - g_p_I.z()) / 2), 0, g_p_I.x() / sqrt(2 * (1 - g_p_I.z()))};
    //     }

    //     // 5. Adaptive gain interpolation to reduce high freq noise
    //     double alpha = adaptiveAccelGain(accel_best_filtering_at_static, .1, .2);
    //     delta_q_acc = delta_q_acc.interpolation(Quaternion{1, 0, 0, 0}, alpha, .9);

    //     // 6. Combine with gyro orientation to correct roll and pitch
    //     Quaternion q_wa_BI = q_w_BI * delta_q_acc;
    //     q_wa_BI.normalize();
    //     //-------------------------------------------------//

    //     //----------------- MAG ORIENTATION --------------//
    //     orientation = q_wa_BI.conjugate(); // TODO come back to at some point
    //     return;
    //     // 7. Get magnetic field vector in interial frame
    //     Quaternion m_B = Quaternion{0, measuredMag};
    //     if (!(m_B.magnitude() > 0))
    //     {
    //         // Mag Field has no yaw contribution to be made
    //         orientation = q_wa_BI.conjugate();
    //         return;
    //     }
    //     m_B.normalize();
    //     Quaternion m_I = q_wa_BI.conjugate() * m_B * q_wa_BI;

    //     // 8. Determine delta mag orientation
    //     double Gamma = m_I.x() * m_I.x() + m_I.y() * m_I.y();

    //     Quaternion delta_q_mag;
    //     if (m_I.x() > 0)
    //     {
    //         delta_q_mag = Quaternion{sqrt(Gamma + (m_I.x() * sqrt(Gamma))) / sqrt(2 * Gamma), 0, 0, m_I.y() / sqrt(2 * (Gamma + (m_I.x() * sqrt(Gamma))))};
    //     }
    //     else
    //     {
    //         delta_q_mag = Quaternion{m_I.y() / sqrt(2 * (Gamma - (m_I.x() * sqrt(Gamma)))), 0, 0, sqrt(Gamma - (m_I.x() * sqrt(Gamma))) / sqrt(2 * Gamma)};
    //     }

    //     // 9. Interpolation to reduce high freq noise
    //     delta_q_mag = delta_q_mag.interpolation(Quaternion{1, 0, 0, 0}, mag_best_filtering_at_static, .9);

    //     // 10. Combine with gyro/acc orientation to correct yaw
    //     Quaternion q_BI = q_wa_BI * delta_q_mag;
    //     q_BI.normalize();
    //     //-------------------------------------------------//

    //     orientation = q_BI.conjugate();
    // }

    // double IMU::adaptiveAccelGain(double alphaBar, double t_1, double t_2)
    // {
    //     // alphaBar: constant value that gives the best filtering result in static conditions
    //     // t_1: threshold 1 - Below, gravitation accel dominates
    //     // t_2: threshold 2 - Below, gain factor decreases linearly as non grav accel increase
    //     double g = 9.81; // m/s^2
    //     double error = abs(measuredAcc.magnitude() - g) / g;

    //     double f = 0;
    //     if (error < t_1)
    //     {
    //         f = 1;
    //     }
    //     else if (error < t_2)
    //     {
    //         f = (t_2 - error) / t_1;
    //     }
    //     return f * alphaBar;
    // }
}