#include "BNO055.h"
#include "../../RecordData/Logging/Logger.h"

namespace mmfs
{

    BNO055::BNO055(const char *name, uint8_t address, TwoWire *theWire) : bno(-1, address, theWire) //-1 is the default sensor ID
    {
        setName(name);
        this->address = address;
    }

    BNO055::BNO055(uint8_t address, TwoWire *theWire) : bno(-1, address, theWire) //-1 is the default sensor ID
    {
        setName("BNO055");
        this->address = address;
    }

    bool BNO055::init()
    {
        if (!bno.begin())
            return initialized = false;

        bno.setExtCrystalUse(true);

        initialMagField = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER));
        return initialized = true;
    }

    void BNO055::read()
    {
        measuredAcc = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL));
        measuredGyro = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE));
        measuredMag = convertIMUtoMMFS(bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER));

        orientation = convertIMUtoMMFS(bno.getQuat());
        // check the i2c bus to make sure the BNO didn't misbehave
        Wire.beginTransmission(address);
        byte b = Wire.endTransmission();
        if (b != 0x00)
        {
            Wire.end();
            Wire.begin();
            getLogger().recordLogData(ERROR_, "BNO055 I2C Error");
        }
    }

    void BNO055::calibrateBno() // not used in flight, used with a separate main file to calibrate the BNO055. BNO does not store these valeus between power cycles.
    {
        uint8_t system, gyro, accel, mag, i = 0;
        while ((system != 3) || (gyro != 3) || (accel != 3) || (mag != 3))
        {
            bno.getCalibration(&system, &gyro, &accel, &mag);
            i = i + 1;
            if (i == 10)
            {
                i = 0;
            }
            delay(10);
        }
    }
}