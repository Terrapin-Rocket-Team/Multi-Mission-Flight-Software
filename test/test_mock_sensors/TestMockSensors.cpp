#include "../src/Sensors/Baro/MockBarometer.h"
#include "../src/Sensors/GPS/MockGPS.h"
#include "../src/Sensors/IMU/MockIMU.h"
// #include <unity.h>
#include <iostream>

#include "../../.pio/libdeps/native/Unity/src/unity.h"
#include "../../lib/NativeTestMocks/NativeTestHelper.h"
#include <string.h>
// include other headers you need to test here

// ---

// Set up and global variables or mocks for testing here

using namespace mmfs;
// ---

const char header[] =
    "pres,temp,smth,lat,lon,alt,head,fixQ,accX,accY,accZ,gyroX,gyroY,gyroZ,magX,magY,magZ";

const char *data[] =
    {"1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,",
     "2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,",
     "3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,",
     "4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,",
     "5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,",
     "6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,"};

char *dataPtr;
char badstr[] =
    "foo,bar,baz";

MockBarometer *mockBarometer;
MockGPS *mockGPS;

std::string accCols[3] = {
    "accX",
    "accY",
    "accZ"};
std::string gyroCols[3] = {
    "gyroX",
    "gyroY",
    "gyroZ"};
std::string magColNames[3] = {
    "magX",
    "magY",
    "magZ"};

MockIMU *mockIMU;

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

void test_baro_init()
{
    strcpy(dataPtr, header);
    TEST_ASSERT_EQUAL(true, mockBarometer->init());
}

void test_baro_init_fail()
{
    MockBarometer badMockBaro(badstr, "pres", "temp");
    TEST_ASSERT_EQUAL(false, badMockBaro.init());
}

void test_baro_read()
{
    for (int i = 0; i < 6; i++)
    {
        strcpy(dataPtr, data[i]);

        mockBarometer->read();
        TEST_ASSERT_EQUAL_FLOAT(1.0f + i, mockBarometer->getPressure());
        TEST_ASSERT_EQUAL_FLOAT(2.0f + i, mockBarometer->getTemp());
    }
}

// ---

void test_gps_init()
{
    strcpy(dataPtr, header);
    TEST_ASSERT_EQUAL(true, mockGPS->init());
}

void test_gps_init_fail()
{
    MockGPS badMockGPS(badstr,
                       "lat",
                       "lon",
                       "alt",
                       "head",
                       "fixQ");
    TEST_ASSERT_EQUAL(false, badMockGPS.init());
}

void test_gps_read()
{
    for (int i = 0; i < 6; i++)
    {

        strcpy(dataPtr, data[i]);
        mockGPS->read();
        TEST_ASSERT_EQUAL_FLOAT(4.0f + i, mockGPS->getPos().x());
        TEST_ASSERT_EQUAL_FLOAT(5.0f + i, mockGPS->getPos().y());
        TEST_ASSERT_EQUAL_FLOAT(6.0f + i, mockGPS->getPos().z());
        TEST_ASSERT_EQUAL_FLOAT(7.0f + i, (float)mockGPS->getHeading());
        TEST_ASSERT_EQUAL_FLOAT(8.0f + i, (float)mockGPS->getFixQual());
    }
}

void test_imu_init()
{
    strcpy(dataPtr, header);
    TEST_ASSERT_EQUAL(true, mockIMU->init());
}

void test_imu_init_fail()
{
    MockIMU badMockIMU(badstr, accCols, gyroCols, magColNames);
    TEST_ASSERT_EQUAL(false, badMockIMU.init());
}

void test_imu_read()
{
    for (int i = 0; i < 6; i++)
    {
        strcpy(dataPtr, data[i]);

        mockIMU->read();

        for (int j = 0; j < 3; j++)
        {
            TEST_ASSERT_EQUAL_FLOAT(9.0f + i + j, mockIMU->getAcceleration()[j]);
            TEST_ASSERT_EQUAL_FLOAT(12.0f + i + j, mockIMU->getAngularVelocity()[j]);
            TEST_ASSERT_EQUAL_FLOAT(15.0f + i + j, mockIMU->getMagField()[j]);
        }
    }
}

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    dataPtr = new char[5000];
    strcpy(dataPtr, header);
    mockBarometer = new MockBarometer(dataPtr, "pres", "temp");
    mockGPS = new MockGPS(dataPtr,
                          "lat",
                          "lon",
                          "alt",
                          "head",
                          "fixQ");

    mockIMU = new MockIMU(dataPtr, accCols, gyroCols, magColNames);
    // baro
    RUN_TEST(test_baro_init);
    RUN_TEST(test_baro_init_fail);
    RUN_TEST(test_baro_read);

    // gps
    RUN_TEST(test_gps_init);
    RUN_TEST(test_gps_init_fail);
    RUN_TEST(test_gps_read);

    // imu
    RUN_TEST(test_imu_init);
    RUN_TEST(test_imu_init_fail);
    RUN_TEST(test_imu_read);

    UNITY_END();
}
// ---
