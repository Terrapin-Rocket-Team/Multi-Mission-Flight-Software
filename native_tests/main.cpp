
#ifndef PIO_UNIT_TESTING
#include <MockGPS.h>
#include <iostream>
#include <MockBarometer.h>
#include <MockIMU.h>
#include <State/State.h>
#include "AvionicsState.h"
#include "AvionicsKF.h"

template <int N> void printVec(mmfs::Vector<N> vec) {
    for (int i = 0; i < N; i++) {
        printf("%7.2f ", vec[i]);
    }
}

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

    mmfs::Logger logger;
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
    logger.init(&avState);
    avState.init();

    printf("%7s %7s %7s %7s %7s %7s %7s %7s %7s\n",
        "PX", "PY", "PZ",
        "VX", "VY", "VZ",
        "AX", "AY", "AZ");

    while(baro.isInitialized() && gps.isInitialized() && imu.isInitialized()) {
        avState.updateState();
        printVec<3>(avState.getPosition());
        printVec<3>(avState.getAcceleration());
        printVec<3>(avState.getAcceleration());
        printf("\n");
    }

    printf("Flight data finished!\n");

    return 0;
}

#endif