
#include <MockGPS.h>
#ifndef PIO_UNIT_TESTING
#include <iostream>
#include <MockBarometer.h>
#include <MockIMU.h>
#include <State/State.h>
#include "AvionicsState.h"
#include "AvionicsKF.h"

int main()
{
    const std::string dataPath = "./test_data/185_FlightData.csv";

    std::string accColNames[3] {
        "AX", "AY", "AZ"
    };
    std::string gyroColNames[3] {
        "I-ULRX", "I-ULRY", "I-ULRZ"
    };
    std::string magColNames[3] {
        "_", "_", "_"
    };

    MockBarometer baro(dataPath, "B-Pres (hPa)", "B-Temp (C)");
    MockGPS gps(dataPath, "G-Lat (deg)", "G-Lon (deg)", "G-Alt (m)", "_", "G-# of Sats");
    MockIMU imu(dataPath, accColNames, gyroColNames, magColNames);

    mmfs::Sensor *sensors[3] = {
        &baro,
        &gps,
        &imu
    };

    mmfs::AvionicsKF kf = mmfs::AvionicsKF();
    mmfs::AvionicsState avState = mmfs::AvionicsState(sensors, 3, &kf, true);
    avState.init();

    while(baro.isInitialized() && gps.isInitialized() && imu.isInitialized()) {
        avState.updateState();
        std::cout << "accZ: " << avState.getAcceleration().z() << " (m/s/s)\t|" << "vz: " << avState.getVelocity().z() << " (m/s)\t|" << " pz: " << avState.getPosition().z() << std::endl;
    }

    return 0;
}

#endif