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

void test_packData()
{
    gps.set(0, 0, 0);
    baro.set(0, 0);
    char dest[500];
    uintptr_t destPtr = (uintptr_t)dest;
    destPtr = mmfs::DataFormatter::packData(destPtr, &state);

    int expectedSize = 0;
    for (int i = 0; i < state.getNumPackedDataPoints(); i++)
        expectedSize += state.PackedTypeToSize(state.getPackedOrder()[i]);

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < state.getSensors()[i]->getNumPackedDataPoints(); j++)
            expectedSize += state.PackedTypeToSize(state.getSensors()[i]->getPackedOrder()[j]);

    TEST_ASSERT_EQUAL_INT(expectedSize, destPtr - (uintptr_t)dest);
}

void test_packData2()
{
}

void test_setCSVHeader()
{
    char dest[500];
    for(int i = 0; i < 500; i++)
        dest[i] = 0;
    mmfs::DataFormatter::getCSVHeader(dest, 500, &state);
    printf("%s\n", dest);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_packData);
    RUN_TEST(test_packData2);
    RUN_TEST(test_setCSVHeader);

    UNITY_END();

    return 0;
}
// ---