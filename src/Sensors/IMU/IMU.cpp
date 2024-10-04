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

    Vector<3> IMU::getAccelerationLocal()
    {
        return accelerationLocal;
    }

    Vector<3> IMU::getMagnetometerReading()
    {
        return measuredMag;
    }

    Vector<3> IMU::getGyroReading()
    {
        return measuredGyro;
    }

    Vector<3> IMU::getAccReading()
    {
        return measuredAcc;
    }

    const char *IMU::getCsvHeader() const
    {                                                                                                          // incl I- for IMU
        return "I-AX (m/s/s),I-AY (m/s/s),I-AZ (m/s/s),I-QUATX,I-QUATY,I-QUATZ,I-QUATW,"; // trailing comma
    }

    const char *IMU::getDataString() const
    {
        sprintf(data, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", measuredAcc.x(), measuredAcc.y(), measuredAcc.z(), orientationGlobal.x(), orientationGlobal.y(), orientationGlobal.z(), orientationGlobal.w()); // trailing comma"
        return data;
    }

    const char *IMU::getStaticDataString() const
    {
        sprintf(data, "Initial Magnetic Field (uT): %.2f,%.2f,%.2f\n", initialMagField.x(), initialMagField.y(), initialMagField.z());
        return data;
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

    void IMU::packData()
    {
        _Float16 accX = _Float16(measuredAcc.x());
        _Float16 accY = _Float16(measuredAcc.y());
        _Float16 accZ = _Float16(measuredAcc.z());
        _Float16 quatX = _Float16(orientationGlobal.x());
        _Float16 quatY = _Float16(orientationGlobal.y());
        _Float16 quatZ = _Float16(orientationGlobal.z());
        _Float16 quatW = _Float16(orientationGlobal.w());

        int offset = 0;
        memcpy(packedData + offset, &accX, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &accY, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &accZ, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &quatX, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &quatY, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &quatZ, sizeof(_Float16));
        offset += sizeof(_Float16);
        memcpy(packedData + offset, &quatW, sizeof(_Float16));
    }
}