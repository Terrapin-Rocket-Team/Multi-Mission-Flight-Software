#ifndef IMU_H
#define IMU_H

#include "../Sensor.h"
#include "../../Math/Vector.h"
#include "../../Math/Quaternion.h"
#include "../../Constants.h"

namespace mmfs
{
    class IMU : public Sensor
    {
    public:
        virtual ~IMU(){};
        virtual Quaternion getOrientation();
        virtual Vector<3> getAcceleration();
        virtual Vector<3> getOrientationEuler();
        virtual Vector<3> getMagnetometer();
        virtual SensorType getType() const override { return IMU_; }
        virtual const char *getTypeString() const override { return "IMU"; }
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual void update() override;

    protected:
        IMU()
        {                                       // Protected constructor to prevent instantiation
            staticData = new char[30 + MAX_DIGITS_FLOAT * 3]; // 30 chars for the string, 12 chars for the 3 floats
            data = new char[MAX_DIGITS_FLOAT * 10 + 10];      // 10x floats + buffer space
        };
        Vector<3> accelerationVec = Vector<3>(0, 0, 0);
        Vector<3> orientationEuler = Vector<3>(0, 0, 0);
        Quaternion orientation = Quaternion(1, 0, 0, 0);
        Vector<3> magnetometer = Vector<3>(0, 0, 0);
        Vector<3> initialMagField = Vector<3>(0, 0, 0);
    };
  
    Vector<3> convertToEuler(const Quaternion &orientation);
}
#endif