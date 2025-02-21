#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/UnitTestSensors.h"

// ---

// Set up and global variables or mocks for testing here

FakeGPS gps;
constexpr double lat1 = 38.9869;
constexpr double lon1 = -76.9426; // College Park, MD

constexpr double lat2 = 38.8977;
constexpr double lon2 = -77.0365; // Washington, DC

// Just gotta say its freaky af that CoPilot knew those were CP MD coords, then predicted that I would want DC coords next (and gave me the correct coords). Weird af.

// ---

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    gps.begin(); // reset the gps before each test
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---

// Test functions must be void and take no arguments, put them here

void test_gps_distance_formula()
{   
    gps.begin(false);
    gps.setFixQual(4);
    gps.set(lat1, lon1, 0);
    gps.update();
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    gps.set(lat2, lon2, 0);
    gps.update();
    double distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_FLOAT_WITHIN(12819.0 / 1000.0 * 5.0, 12819, distance);
    // distance between CP and DC in meters, verified with this site:
    // https://www.gpsvisualizer.com/calculators, testing with difference of 0.05% of the "actual" Vincenty formula distance
}

void test_gps_begin()
{
    TEST_ASSERT_TRUE(gps.begin());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());
    TEST_ASSERT_EQUAL(0, gps.getFixQual());
    TEST_ASSERT_EQUAL(true, gps.isInBiasCorrectionMode());
}

void test_gps_set()
{
    gps.set(lat1, lon1, 0);
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());
}

void test_gps_first_fix()
{
    gps.begin(false);
    TEST_ASSERT_FALSE(gps.getHasFix());
    TEST_ASSERT_EQUAL(0, gps.getFixQual());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getHeading());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());

    gps.set(lat1, lon1, 0);
    gps.update();

    TEST_ASSERT_FALSE(gps.getHasFix());
    TEST_ASSERT_EQUAL(0, gps.getFixQual());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getHeading());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());

    gps.setFixQual(4);
    gps.update();

    TEST_ASSERT_TRUE(gps.getHasFix());
    TEST_ASSERT_EQUAL(4, gps.getFixQual());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());

    gps.set(lat2, lon2, 1000);
    gps.update();

    TEST_ASSERT_TRUE(gps.getHasFix());
    TEST_ASSERT_EQUAL(4, gps.getFixQual());
    double distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_FLOAT_WITHIN(12819.0 / 1000.0 * 5.0, 12819, distance);
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(lat2, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon2, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getPos().z());
}

void test_gps_no_bias_correction()
{
    gps.begin(false);
    TEST_ASSERT_FALSE(gps.isInBiasCorrectionMode());
    gps.set(lat1, lon1, 0);
    gps.setFixQual(4);
    gps.update();
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().x());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getPos().z());

    gps.set(lat2, lon2, 1000);
    gps.update();

    double distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_FLOAT_WITHIN(12819.0 / 1000.0 * 5.0, 12819, distance);
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getDisplacement().z());
    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());
    TEST_ASSERT_EQUAL_FLOAT(lat2, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon2, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getPos().z());
}

void test_gps_bias_correction(){
    gps.set(lat1, lon1, 0);
    gps.setFixQual(4);
    gps.update();

    gps.set(lat1, lon1, 0);
    for(int i = 0; i < gps.getMaxBufferLen() / 2; i++){
        gps.update();
    }

    gps.set(lat2, lon2, 1000);
    for (int i = 0; i < gps.getMaxBufferLen() / 2; i++)
    {
        gps.update();
    }
    double distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_FLOAT_WITHIN(12819.0 / 1000.0 * 5.0, 12819, distance);
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getDisplacement().z());

    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());

    TEST_ASSERT_EQUAL_FLOAT(lat2, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon2, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getPos().z());

    gps.update();
 // Unity LESS_THAN and GREATER_THAN macros don't work with floats, so we need to multiply by 1,000,000 to get the same effect
    distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_LESS_THAN_FLOAT(12819.0 - 12819.0 / 1000.0 * 5.0, distance); // should be less than the actual distance -.05% of the distance
    TEST_ASSERT_LESS_THAN_FLOAT(1000, gps.getDisplacement().z());
    TEST_ASSERT_LESS_THAN_FLOAT(lat1, gps.getOrigin().x()); // adding a single lat2 into the origin should make lat1 greater than the origin
    TEST_ASSERT_LESS_THAN_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_GREATER_THAN_FLOAT(0, gps.getOrigin().z());

    TEST_ASSERT_EQUAL_FLOAT(lat2, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon2, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getPos().z());
}

void test_gps_mark_liftoff(){
    gps.set(lat1, lon1, 0);
    gps.setHasFirstFix(true);
    gps.update();

    gps.set(lat1, lon1, 0);
    for (int i = 0; i < gps.getMaxBufferLen() / 2; i++)
    {
        gps.update();
    }

    gps.set(lat2, lon2, 1000);
    for (int i = 0; i < gps.getMaxBufferLen() / 2; i++)
    {
        gps.update();
    }

    gps.markLiftoff();

    for (int i = 0; i < gps.getMaxBufferLen(); i++)
    {
        gps.update(); // throw a bunch of lat2 into the buffer (or more accurately, don't do that that)
    }

    double distance = sqrt(gps.getDisplacement().x() * gps.getDisplacement().x() + gps.getDisplacement().y() * gps.getDisplacement().y());
    TEST_ASSERT_FLOAT_WITHIN(12819.0 / 1000.0 * 5.0, 12819, distance);
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getDisplacement().z());

    TEST_ASSERT_EQUAL_FLOAT(lat1, gps.getOrigin().x());
    TEST_ASSERT_EQUAL_FLOAT(lon1, gps.getOrigin().y());
    TEST_ASSERT_EQUAL_FLOAT(0, gps.getOrigin().z());

    TEST_ASSERT_EQUAL_FLOAT(lat2, gps.getPos().x());
    TEST_ASSERT_EQUAL_FLOAT(lon2, gps.getPos().y());
    TEST_ASSERT_EQUAL_FLOAT(1000, gps.getPos().z());
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_gps_distance_formula);
    RUN_TEST(test_gps_begin);
    RUN_TEST(test_gps_set);
    RUN_TEST(test_gps_first_fix);
    RUN_TEST(test_gps_no_bias_correction);
    RUN_TEST(test_gps_bias_correction);
    RUN_TEST(test_gps_mark_liftoff);

    UNITY_END();
}
// ---