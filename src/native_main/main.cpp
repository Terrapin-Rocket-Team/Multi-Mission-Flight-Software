
#include <MockGPS.h>
#ifndef PIO_UNIT_TESTING
#include <iostream>
#include <MockBarometer.h>
#include <MockIMU.h>
#include <State/State.h>

#include "AvionicsKF.h"

#include "AvionicsState.h"

int main()
{
    const std::string dataPath = "./test_data/185_FlightData.csv";

    std::string accColNames[3] {
        "I-AX (m/s/s)", "I-AY (m/s/s)", "I-AZ (m/s/s)"
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

    mmfs::AvionicsState avState = mmfs::AvionicsState(sensors, 3, nullptr, true);
    avState.init();

    while(baro.isInitialized() && gps.isInitialized() && imu.isInitialized()) {
        avState.updateState();
        std::cout << "bPres: " << baro.getPressure() << " px: " << avState.getPosition().x() << " py: " << avState.getPosition().y()  << " pz: " << avState.getPosition().z() << std::endl;
    }

    return 0;
}

// This file exists to allow native to build without being in unit test mode.
// I wanted a way to click the build button and just know if would work without having to go thru the whole testing thing.
// also clicking the build button saves the files

#endif