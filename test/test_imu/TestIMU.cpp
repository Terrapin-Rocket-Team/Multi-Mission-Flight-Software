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

void test_imu_adaptiveAccelGain()
{
    // Test case 1: Error is less than t_1 (should return 1)
    imu.set(Vector<3>{0, 0, 9.8}, Vector<3>{0, 0, 0}, Vector<3>{0, 0, 0}); // Set acceleration to gravitational acceleration
    double result = imu.adaptiveAccelGain(0.9);
    TEST_ASSERT_EQUAL_FLOAT(.9, result);

    // Test case 2: Error is between t_1 and t_2 (should return value between 1 and 0)
    imu.set(Vector<3>{0, 0, 8}, Vector<3>{0, 0, 0}, Vector<3>{0, 0, 0}); // Set acceleration close to gravitational acceleration
    result = imu.adaptiveAccelGain(0.9);
    TEST_ASSERT(result < 1.0 && result > 0.0); // Check that result is in range (0, 1)

    // Test case 3: Error is greater than t_2 (should return 0)
    imu.set(Vector<3>{0, 0, 12}, Vector<3>{0, 0, 0}, Vector<3>{0, 0, 0}); // Set acceleration significantly above gravitational acceleration
    result = imu.adaptiveAccelGain(0.9);
    TEST_ASSERT_EQUAL_FLOAT(0.0, result);
}

void test_quaternion_based_complimentary_filter()
{
    // Test case 1: Propagate orientation with zero angular velocity, and acceleration aligning with gravity
    imu.begin();
    imu.set(Vector<3>{0.0, 0.0, 9.81}, Vector<3>{0.0, 0.0, 0.0}, Vector<3>{0.0, 0.0, 0.0}); // Set accel to gravity and angular velocity to 0
    imu.quaternionBasedComplimentaryFilter((double)UPDATE_INTERVAL/1000); // Run filter with small time step

    // Expected orientation should still be close to identity quaternion (no angular motion)
    Quaternion expected_orientation = Quaternion{1.0, 0.0, 0.0, 0.0};
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.w(), imu.getOrientation().w());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.x(), imu.getOrientation().x());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.y(), imu.getOrientation().y());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.z(), imu.getOrientation().z());

    // Test case 2: Small angular velocity with no external acceleration, identity quaternion
    imu.begin();
    imu.set(Vector<3>{0.0, 0.0, 9.81}, Vector<3>{0.1, 0.0, 0.0}, Vector<3>{0.0, 0.0, 0.0}); // Set small angular velocity in x direction
    imu.quaternionBasedComplimentaryFilter((double)UPDATE_INTERVAL/1000); // Run filter
    
    // Check if the orientation has slightly changed due to angular velocity
    expected_orientation = Quaternion{0.999999875, .000499994313, 0.0, 0.0};
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.w(), imu.getOrientation().w());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.x(), imu.getOrientation().x());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.y(), imu.getOrientation().y());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.z(), imu.getOrientation().z());

    // Test case 3: Large angular velocity and non-gravitational acceleration
    imu.begin();
    imu.set(Vector<3>{2.0, 2.0, 5.0}, Vector<3>{1.0, 1.0, 1.0}, Vector<3>{0.5, 0.5, 0.5}); // Random non-gravity forces
    imu.quaternionBasedComplimentaryFilter((double)UPDATE_INTERVAL/1000); // Run filter

    // The result should deviate significantly from the identity quaternion
    expected_orientation = Quaternion{0.9517171, 0.06399739, 0.02944679, -0.29878383};
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.w(), imu.getOrientation().w());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.x(), imu.getOrientation().x());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.y(), imu.getOrientation().y());
    TEST_ASSERT_EQUAL_FLOAT(expected_orientation.z(), imu.getOrientation().z());
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
    RUN_TEST(test_imu_adaptiveAccelGain);
    //RUN_TEST(test_quaternion_based_complimentary_filter); // TODO: Fix test

    UNITY_END();

    return 0;
}
// ---