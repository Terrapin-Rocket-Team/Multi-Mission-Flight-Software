#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/UnitTestSensors.h"

// ---

// Set up and global variables or mocks for testing here

FakeBarometer baro;
const double ALT_AT_500_HPA = 5572.1; // altitude at 500 hPa in meters
const double ALT_AT_750_HPA = 2465.2; // altitude at 750 hPa in meters

// ---

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    baro.fakeP = 0;
    baro.fakeT = 0;
    baro.begin(); // reset the barometer before each test
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---

// Test functions must be void and take no arguments, put them here

void test_baro_begin()
{
    TEST_ASSERT_TRUE(baro.begin());

    TEST_ASSERT_EQUAL_FLOAT(0, baro.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getTemp());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM());

    baro.set(500, 25);

    TEST_ASSERT_TRUE(baro.begin(false));

    TEST_ASSERT_EQUAL_FLOAT(500, baro.fakeP);
    TEST_ASSERT_EQUAL_FLOAT(25, baro.fakeT);
    TEST_ASSERT_EQUAL(false, baro.isInBiasCorrectionMode());
    TEST_ASSERT_EQUAL_FLOAT(500, baro.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(25, baro.getTemp());
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM());
}

void test_baro_set()
{
    baro.set(1000, 25);
    TEST_ASSERT_EQUAL_FLOAT(1000, baro.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(25, baro.getTemp());
}

void test_baro_alt()
{
    baro.set(1013.25, 25);
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getASLAltM());
    baro.set(500, 25);
    baro.update();
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
}

void test_baro_conversions()
{
    baro.set(1013.25, 25);
    baro.begin(false);
    baro.update();
    TEST_ASSERT_EQUAL_FLOAT(1013.25, baro.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(1, baro.getPressureAtm());
    TEST_ASSERT_EQUAL_FLOAT(25, baro.getTemp());
    TEST_ASSERT_EQUAL_FLOAT(77, baro.getTempF());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getASLAltFt());
    baro.set(500, 25);
    baro.update();
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA * 3.28084, baro.getASLAltFt());
}

// test initialize from sea level + a single update step at 500 hpa (no bias correction)
void test_baro_no_bias_correction_1()
{
    baro.set(1013.25, 25); // set to standard sea level pressure
    baro.begin(false);     // reset baro with no bias correction

    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM());
    baro.set(500, 25);                                          // set to 500 hPa
    baro.update();                                              // update altitude
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getAGLAltM()); // AGL and ASL should be equal
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
}

// test init from 750 hpa + a single update step at 500 hpa (no bias correction)

void test_baro_no_bias_correction_2()
{
    baro.set(750, 25);                                                           // set to 750 hPa
    baro.begin(false);                                                           // reset baro with no bias correction
    baro.update();                                                               // update altitude
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM());                               // AGL should be 0
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_750_HPA, baro.getASLAltM());                  // ASL should be the altitude at 750 hPa
    baro.set(500, 25);                                                           // set to 500 hPa
    baro.update();                                                               // update altitude
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA - ALT_AT_750_HPA, baro.getAGLAltM()); // AGL should be the difference between the two altitudes
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());                  // ASL should be the altitude at 500 hPa
}

void test_baro_bias_correction_init()
{
    // method: feed fake start data and test if average is correct

    // set up fake data
    for (int i = 0; i < baro.getMaxBufferLen(); i++)
    {
        baro.set(500, 25);
        baro.update(); // update altitude and buffer
    }
    TEST_ASSERT_EQUAL_FLOAT(500, baro.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(25, baro.getTemp());
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM()); // AGL should be 0
}

void test_baro_bias_correction_no_GL_effect()
{
    // set up fake data
    for (int i = 0; i < baro.getMaxBufferLen(); i++)
    {
        baro.set(750, 25);
        baro.update(); // update altitude and buffer
    }
    for (int i = 0; i < baro.getMaxBufferLen() / 2; i++) // update the most recent half of the data, which should not affect the ground average
    {
        baro.set(500, 25);
        baro.update(); // update altitude and buffer
    }
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA - ALT_AT_750_HPA, baro.getAGLAltM()); // AGL should be the EXACT difference between the two altitudes (hasnt started affecting the ground level)
}

void test_baro_bias_correction_GL_effect()
{
    // set up fake data
    for (int i = 0; i < baro.getMaxBufferLen(); i++)
    {
        baro.set(750, 25);
        baro.update(); // update altitude and buffer
    }
    for (int i = 0; i < baro.getMaxBufferLen() / 2 + 1; i++) // affect the ground average by 1
    {
        baro.set(500, 25);
        baro.update(); // update altitude and buffer
    }
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_LESS_THAN(ALT_AT_500_HPA - ALT_AT_750_HPA, baro.getAGLAltM()); // AGL should be the difference between the two altitudes, but less than the exact difference
}

void test_baro_bias_correction_full_GL_effect()
{
    // set up fake data
    for (int i = 0; i < baro.getMaxBufferLen(); i++)
    {
        baro.set(750, 25);
        baro.update(); // update altitude and buffer
    }
    for (int i = 0; i < baro.getMaxBufferLen(); i++) // affect the ground average by 100%
    {
        baro.set(500, 25);
        baro.update(); // update altitude and buffer
    }
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(0, baro.getAGLAltM()); // AGL should now be 0
}

//verify that mark liftoff correctly stops the new readings from affecting the ground average
void test_mark_liftoff()
{
    // set up fake data
    for (int i = 0; i < baro.getMaxBufferLen(); i++)
    {
        baro.set(750, 25);
        baro.update(); // update altitude and buffer
    }

    baro.markLiftoff(); // mark liftoff

    for (int i = 0; i < baro.getMaxBufferLen(); i++) // affect the ground average by 100%
    {
        baro.set(500, 25);
        baro.update(); // update altitude and buffer
    }

    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA, baro.getASLAltM());
    TEST_ASSERT_EQUAL_FLOAT(ALT_AT_500_HPA - ALT_AT_750_HPA, baro.getAGLAltM()); // AGL should be the difference between the two altitudes
}
// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_baro_begin);
    RUN_TEST(test_baro_set);
    RUN_TEST(test_baro_alt);
    RUN_TEST(test_baro_conversions);
    RUN_TEST(test_baro_no_bias_correction_1);
    RUN_TEST(test_baro_no_bias_correction_2);
    RUN_TEST(test_baro_bias_correction_init);
    RUN_TEST(test_baro_bias_correction_no_GL_effect);
    RUN_TEST(test_baro_bias_correction_GL_effect);
    RUN_TEST(test_baro_bias_correction_full_GL_effect);
    RUN_TEST(test_mark_liftoff);

    UNITY_END();

    return 0;
}
// ---