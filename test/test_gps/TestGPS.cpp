#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/FakeSensors.h"

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

void test_distance_formula()
{
    gps.setFixQual(3);
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

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_distance_formula);

    UNITY_END();

    return 0;
}
// ---