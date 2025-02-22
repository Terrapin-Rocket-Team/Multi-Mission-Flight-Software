#include <unity.h>
#include "../src/Math/Quaternion.h"
#include "../src/Math/Matrix.h"
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

void test_quat_to_matrix(void)
{
    // Define a quaternion representing a 45-degree (π/4 rad) rotation about the Y-axis
    double angle = M_PI / 4;  // 45 degrees
    double half_angle = angle / 2;
    Quaternion q(cos(half_angle), 0.0, sin(half_angle), 0.0); // (w, x, y, z)

    // Convert quaternion to DCM
    mmfs::Matrix dcm = q.toMatrix();

    // Expected DCM for 45-degree rotation about Y-axis
    double sqrt2_2 = sqrt(2) / 2;
    mmfs::Matrix expected_dcm(3, 3, new double[9]{sqrt2_2,0,-sqrt2_2,0,1,0,sqrt2_2, 0, sqrt2_2});

    // Compare each element of the DCM (assuming TEST_ASSERT_EQUAL_FLOAT can be used for matrices)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            TEST_ASSERT_EQUAL_FLOAT(expected_dcm.get(i, j), dcm.get(i, j));
        }
    }
}

// Main function
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add test functions here
    RUN_TEST(test_interpolation_lerp);
    RUN_TEST(test_interpolation_slerp);
    RUN_TEST(test_quat_to_matrix);

    UNITY_END();

    return 0;
}