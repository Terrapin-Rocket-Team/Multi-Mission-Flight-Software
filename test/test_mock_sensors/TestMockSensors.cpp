#include "../../lib/NativeTestMocks/MockBarometer.h"
#include "../../lib/NativeTestMocks/MockGPS.h"
#include "../../lib/NativeTestMocks/MockIMU.h"
//#include <unity.h>
#include <iostream>

#include "../../.pio/libdeps/native/Unity/src/unity.h"
#include "../../lib/NativeTestMocks/NativeTestHelper.h"
// include other headers you need to test here

// ---



// Set up and global variables or mocks for testing here



// ---


std::filesystem::path TEST_PATH_DATA = std::filesystem::path("test_data") / "test_new.csv";

MockBarometer mockBarometer(TEST_PATH_DATA, "pres", "temp");
MockGPS mockGPS(TEST_PATH_DATA,
        "lat",
        "lon",
        "alt",
        "head",
        "fixQ");

std::string accCols[3] = {
    "accX",
    "accY",
    "accZ"
};
std::string gyroCols[3] = {
    "gyroX",
    "gyroY",
    "gyroZ"
};
std::string magColNames[3] = {
    "magX",
    "magY",
    "magZ"
};

MockIMU mockIMU(TEST_PATH_DATA, accCols, gyroCols, magColNames);

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---



// Test functions must be void and take no arguments, put them here

void test_baro_init() {
    TEST_ASSERT_EQUAL(true, mockBarometer.init());
}

void test_baro_read() {
  	for(int i = 0; i < 6; i++) {
        mockBarometer.read();
        TEST_ASSERT_EQUAL_FLOAT(1.0f+i, mockBarometer.getPressure());
        TEST_ASSERT_EQUAL_FLOAT(2.0f+i, mockBarometer.getTemp());
    }
}

// ---

void test_gps_init() {
    TEST_ASSERT_EQUAL(true, mockGPS.init());
}

void test_gps_read() {
  	for(int i = 0; i < 6; i++) {
        mockGPS.read();
        TEST_ASSERT_EQUAL_FLOAT(4.0f+i, mockGPS.getPos().x());
        TEST_ASSERT_EQUAL_FLOAT(5.0f+i, mockGPS.getPos().y());
        TEST_ASSERT_EQUAL_FLOAT(6.0f+i, mockGPS.getPos().z());
        TEST_ASSERT_EQUAL_FLOAT(7.0f+i, (float) mockGPS.getHeading());
        TEST_ASSERT_EQUAL_FLOAT(8.0f+i, (float) mockGPS.getFixQual());
    }
}

void test_imu_init() {
    TEST_ASSERT_EQUAL(true, mockIMU.init());
}

void test_imu_read() {
  	for(int i = 0; i < 6; i++) {
        mockIMU.read();

  	    for(int j = 0; j < 3; j++) {
  	        TEST_ASSERT_EQUAL_FLOAT(9.0f+i+j, mockIMU.getAccReading()[j]);
  	        TEST_ASSERT_EQUAL_FLOAT(12.0f+i+j, mockIMU.getGyroReading()[j]);
  	        TEST_ASSERT_EQUAL_FLOAT(15.0f+i+j, mockIMU.getMagnetometerReading()[j]);
  	    }
    }

}

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    RUN_TEST(test_baro_init); // no parentheses after function name
    RUN_TEST(test_baro_read); // no parentheses after function name
    RUN_TEST(test_gps_init);
    RUN_TEST(test_gps_read);
    RUN_TEST(test_imu_init);
    RUN_TEST(test_imu_read);

    UNITY_END();
}
// ---
