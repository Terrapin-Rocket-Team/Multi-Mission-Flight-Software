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

        //local represents the orientation off of the intial angle. Global represents the orientation with repesct to gravity
        virtual Quaternion getOrientation();
        virtual Vector<3> getAngularVelocity();
        virtual Vector<3> getMagField();
        virtual Vector<3> getAcceleration();
        virtual Vector<3> getAccelerationGlobal();
        virtual double getAccelBestFilteringAtStatic() {return accel_best_filtering_at_static;};
        virtual void setAccelBestFilteringAtStatic(double a) {accel_best_filtering_at_static = a;};
        virtual double getMagBestFilteringAtStatic() {return mag_best_filtering_at_static;};
        virtual void setMagBestFilteringAtStatic(double m) {mag_best_filtering_at_static = m;};
        virtual SensorType getType() const override { return IMU_; }
        virtual const char *getTypeString() const override { return "IMU"; }
        virtual const char *getCsvHeader() const override;
        virtual const char *getDataString() const override;
        virtual const char *getStaticDataString() const override;
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;

        double adaptiveAccelGain(double alphaBar, double t_1 = .1, double t_2 = .2);
        virtual void quaternionBasedComplimentaryFilter(double dt);

    protected:
        IMU()
        {                                       // Protected constructor to prevent instantiation
            staticData = new char[30 + MAX_DIGITS_FLOAT * 3]; // 30 chars for the string, 12 chars for the 3 floats
            data = new char[MAX_DIGITS_FLOAT * 10 + 10];      // 10x floats + buffer space
        };

        Vector<3> accelerationVec = Vector<3>(0, 0, 0); // Body frame acceleration in m/s/s
        Vector<3> orientationEuler = Vector<3>(0, 0, 0);
        Quaternion orientation = Quaternion(1, 0, 0, 0); // This is the mapping from the body to interial frame as a quaternion and can be used as such: r_I = q * r_B * q^-1
        Vector<3> angularVelocity = Vector<3>(0, 0, 0); // Rad/s
        Vector<3> magField = Vector<3>(0, 0, 0); // Body frame mag field in uT
        Vector<3> initialMagField = Vector<3>(0, 0, 0);
        double accel_best_filtering_at_static = 0.9; // [0, 1] Higher this number, the more you trust the accelerameter
        double mag_best_filtering_at_static = 0.9; // [0, 1] Higher this number, the more you trust the magnatometer
    };
  
    Vector<3> convertToEuler(const Quaternion &orientation);
}
#endif