#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/RecordData/DataFormatter.h"
#include "../../lib/NativeTestMocks/UnitTestSensors.h"
#include "../../src/State/State.h"

// ---

// Set up and global variables or mocks for testing here
class TestState : public State
{
public:
    TestState(Sensor **sensors, int numSensors, Filter *filter) : State(sensors, numSensors, filter) {}
    void determineStage() override {}
};

FakeBarometer baro;
FakeGPS gps;
Sensor *sensors[] = {&baro, &gps};
TestState state(sensors, 2, nullptr);
DataReporter *dr[] = {&state, &baro, &gps};
// ---

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

// Test functions must be void and take no arguments, put them here


void test_setCSVHeader()
{
    char dest[2000];
    for (int i = 0; i < 500; i++)
        dest[i] = 0;
    mmfs::DataFormatter::getCSVHeader(dest, 2000, dr, 3);
    TEST_ASSERT_EQUAL_STRING(
        "State - Time (s),State - PX (m),State - PY (m),State - PZ (m),State - VX (m/s),State - VY (m/s),State - VZ (m/s),State - AX (m/s/s),State - AY (m/s/s),State - AZ (m/s/s),"
        "FakeBarometer - Pres (hPa),FakeBarometer - Temp (C),FakeBarometer - Alt ASL (m),FakeBarometer - Alt AGL (m),"
        "FakeGPS - Lat,FakeGPS - Lon,FakeGPS - Alt (m),FakeGPS - Disp X (m),FakeGPS - Disp Y (m),FakeGPS - Disp Z (m),FakeGPS - Fix Quality,FakeGPS - Time of Day",
        dest);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    state.init(true);
    gps.setHasFirstFix(true);
    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_setCSVHeader);
    //TODO: Add test for CSV row fetching

    UNITY_END();

    return 0;
}
// ---