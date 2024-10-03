#include <unity.h>
#include "../src/Math/Quaternion.h"
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

using mmfs::Quaternion;


// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    // set stuff up before each test here, if needed
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---


// Unit test for the LERP case
void test_interpolation_lerp(void)
{
    // Create two quaternions
    Quaternion p = Quaternion{0.95, 0.31224989992, 0.0, 0.0}; // current quaternion (identity)
    Quaternion q = Quaternion{1.0, 0.0, 0.0, 0.0}; // input quaternion

    double alpha = 0.5; // Interpolation factor
    double epsilon = 0.9; // Threshold for LERP vs SLERP

    // Perform interpolation
    Quaternion result = p.interpolation(q, alpha, epsilon);

    // Determined by running same number through python script based on ahrs library
    Quaternion expected_result = Quaternion{0.98742088, 0.15811388, 0.0, 0.0};

    // Test for equality (you may need to adjust precision due to floating-point errors)
    TEST_ASSERT_EQUAL_FLOAT(expected_result.w(), result.w());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.x(), result.x());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.y(), result.y());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.z(), result.z());
}

// Unit test for the SLERP case
void test_interpolation_slerp(void)
{
    // Create two quaternions
    Quaternion p = Quaternion{0.5, 0.5, 0.5, 0.5}; // current quaternion
    Quaternion q = Quaternion{1.0, 0.0, 0.0, 0.0}; // input quaternion

    double alpha = 0.5; // Interpolation factor
    double epsilon = 0.9; // Threshold for LERP vs SLERP

    // Perform interpolation
    Quaternion result = p.interpolation(q, alpha, epsilon);

    // Determined by running same number through python script based on ahrs library
    Quaternion expected_result = Quaternion{0.8660254, 0.28867513, 0.28867513, 0.28867513};

    TEST_ASSERT_EQUAL_FLOAT(expected_result.w(), result.w());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.x(), result.x());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.y(), result.y());
    TEST_ASSERT_EQUAL_FLOAT(expected_result.z(), result.z());
}

// Main function
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add test functions here
    RUN_TEST(test_interpolation_lerp);
    RUN_TEST(test_interpolation_slerp);

    UNITY_END();

    return 0;
}