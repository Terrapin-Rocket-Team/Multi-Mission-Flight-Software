#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/RecordData/DataFormatter.h"
#include "../../lib/NativeTestMocks/FakeSensors.h"
#include "../../src/State/State.h"

// ---

// Set up and global variables or mocks for testing here

FakeBarometer baro;
FakeGPS gps;
Sensor *sensors[] = {&baro, &gps};
mmfs::State state(sensors, 2, nullptr);
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

void test_getPackedLen()
{
    TEST_ASSERT_EQUAL_INT(
        sizeof(float) * 10 +
            sizeof(float) * 4 +
            sizeof(double) * 2 +
            sizeof(float) * 4 +
            sizeof(uint8_t) * 1,
        mmfs::DataFormatter::getPackedLen(&state));
}

void test_packUnpackData()
{
    gps.set(0, 0, 0);
    baro.set(1001.28948, 25); // HPA for 100 ft ASL
    state.updateState(1);
    uint8_t dest[500];
    uint8_t *destPtr = dest;
    destPtr = mmfs::DataFormatter::packData(destPtr, &state);

    int expectedSize = 0;
    for (int i = 0; i < state.getNumPackedDataPoints(); i++)
        expectedSize += state.PackedTypeToSize(state.getPackedOrder()[i]);

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < state.getSensors()[i]->getNumPackedDataPoints(); j++)
            expectedSize += state.PackedTypeToSize(state.getSensors()[i]->getPackedOrder()[j]);

    TEST_ASSERT_EQUAL_INT(expectedSize, destPtr - dest);

    // now unpack
    char dest2[500];
    mmfs::DataFormatter::toCSVRow(dest2, 500, &state, dest);
    TEST_ASSERT_EQUAL_STRING(
        "1.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000," // state
        "1001.289,25.000,100.000,0.000,"                               // baro
        "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0",                       // gps
        dest2);
}

void test_packData2()
{
    gps.set(0, 0, 0);
    baro.set(1001.28948, 25); // HPA for 100 ft ASL
    state.updateState(2);
    uint8_t dest[500];
    uint8_t *destPtr = dest;
    destPtr = mmfs::DataFormatter::packData(destPtr, &state);
    int packedDataSize = destPtr - dest;
    baro.set(0, 0);
    gps.set(180, 180, 1000);
    state.updateState(3);
    destPtr = mmfs::DataFormatter::packData(destPtr, &state);

    // now unpack
    char dest2[500];
    mmfs::DataFormatter::toCSVRow(dest2, 500, &state, dest);
    TEST_ASSERT_EQUAL_STRING(
        "2.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000," // state
        "1001.289,25.000,100.000,0.000,"                               // baro
        "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0",                       // gps
        dest2);

    char *newPos = (char *)dest + packedDataSize;
    mmfs::DataFormatter::toCSVRow(dest2, 500, &state, newPos);
    TEST_ASSERT_EQUAL_STRING(
        "3.000,0.000,0.000,40925.168,0.000,0.000,40925.168,0.000,0.000,0.000,"
        "0.000,0.000,44307.691,40925.168,"
        "180.0000000,180.0000000,1000.000,0.000,0.000,666.667,0",
        dest2);
}

void test_setCSVHeader()
{
    char dest[500];
    for (int i = 0; i < 500; i++)
        dest[i] = 0;
    mmfs::DataFormatter::getCSVHeader(dest, 500, &state);
    TEST_ASSERT_EQUAL_STRING(
        "Time (s),PX (m),PY (m),PZ (m),VX (m/s),VY (m/s),VZ (m/s),AX (m/s/s),AY (m/s/s),AZ (m/s/s),"
        "Pres (hPa),Temp (C),Alt ASL (ft),Alt AGL (ft),"
        "Lat,Lon,Alt (m),Disp X (m),Disp Y (m),Disp Z (m),Fix Quality",
        dest);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_getPackedLen);
    RUN_TEST(test_packUnpackData);
    RUN_TEST(test_packData2);
    RUN_TEST(test_setCSVHeader);

    UNITY_END();

    return 0;
}
// ---