#ifndef GPS_H
#define GPS_H

#include "../Sensor.h"
#include "../../Constants.h"
#include "../../Math/Vector.h"
#include "../../RecordData/Logger.h"

namespace mmfs
{
    class GPS : public Sensor
    {
    public:
        virtual ~GPS();
        virtual Vector<3> getPos() const;
        virtual Vector<3> getOrigin() const;
        virtual Vector<3> getDisplacement() const;
        virtual int getFixQual() const;
        virtual double getHeading() const;
        virtual bool getHasFirstFix() const;
        virtual void update() override;
        virtual bool begin(bool useBiasCorrection = true) override;

        virtual const char *getTypeString() const override;
        virtual const SensorType getType() const override;

        virtual const int getNumPackedDataPoints() const override;
        virtual const PackedType *getPackedOrder() const override;
        virtual const char **getPackedDataLabels() const override;
        virtual void packData() override;

    protected:
        GPS();
        Vector<3> position;     // latitude, longitude, alt
        Vector<3> displacement; // displacement from starting location
        Vector<3> origin;       // lat(deg), long(deg), alt(m) of the original location
        int fixQual;            // number of satellite connections
        bool hasFirstFix;       // whether or not GPS has reached at least 3 satellites since restart
        double heading;

        // Distance-related calculations
        void calcInitialValuesForDistance();
        double kx, ky;
        void calcDistance();
        double wrapLongitude(double val);

        CircBuffer<Vector<3>> originBuffer = CircBuffer<Vector<3>>(CIRC_BUFFER_LENGTH);
    };
}

#endif // GPS_H
