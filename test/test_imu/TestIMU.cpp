#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/FakeSensors.h"

// ---



// Set up and global variables or mocks for testing here

FakeIMU imu;

// ---



// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    imu.begin(); // reset the imu before each test
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---



// Test functions must be void and take no arguments, put them here

// void test_function_name() {
//     TEST_ASSERT_EQUAL(expected, actual);
//     TEST_ASSERT_EQUAL_FLOAT(expected, actual);
// }

void test_imu_begin()
{
    TEST_ASSERT_TRUE(imu.begin());
    TEST_ASSERT_TRUE(imu.begin(false));

    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAcceleration().x());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAcceleration().y());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAcceleration().z());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAngularVelocity().x());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAngularVelocity().y());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAngularVelocity().z());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getMagField().x());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getMagField().y());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getMagField().z());
}

void test_imu_set()
{
    imu.set(Vector<3>{0, 0, 9.8}, Vector<3>{.01, .1, .05}, Vector<3>{1, 0, 0});
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAcceleration().x());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getAcceleration().y());
    TEST_ASSERT_EQUAL_FLOAT(9.8, imu.getAcceleration().z());
    TEST_ASSERT_EQUAL_FLOAT(.01, imu.getAngularVelocity().x());
    TEST_ASSERT_EQUAL_FLOAT(.1, imu.getAngularVelocity().y());
    TEST_ASSERT_EQUAL_FLOAT(.05, imu.getAngularVelocity().z());
    TEST_ASSERT_EQUAL_FLOAT(1, imu.getMagField().x());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getMagField().y());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getMagField().z());
}

// TODO complimentary filter test
void test_imu_orientation()
{
    double g = 9.8;
    double interval = .1; // This is set by UPDATE_INTERVAL in NativeTestHelpers.cpp
    double rad = 5*interval;
    Vector<3> accelBody = {g*sin(rad), 0, g*cos(rad)};
    Vector<3> angVelo = {0, rad/interval, 0};
    Vector<3> magField = {0, 0, 0};
    imu.set(accelBody, angVelo, magField);
    imu.update();

    TEST_ASSERT_EQUAL_FLOAT(0.5518532, imu.getOrientation().w());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getOrientation().x());
    TEST_ASSERT_EQUAL_FLOAT(-0.9736356, imu.getOrientation().y());
    TEST_ASSERT_EQUAL_FLOAT(0, imu.getOrientation().z());
}

// ---



// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_imu_begin);
    RUN_TEST(test_imu_set);
    RUN_TEST(test_imu_orientation);

    UNITY_END();

    return 0;
}
// ---