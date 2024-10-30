#include "../../lib/NativeTestMocks/MockBarometer.h"
#include <unity.h>
#include <iostream>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"
// include other headers you need to test here

// ---



// Set up and global variables or mocks for testing here



// ---


std::filesystem::path TEST_PATH_DATA = std::filesystem::path("test_data") / "test.csv";

MockBarometer mockBarometer(TEST_PATH_DATA, "pres", "temp");

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---



// Test functions must be void and take no arguments, put them here

void test_baro_init() {
    TEST_ASSERT_EQUAL(true, mockBarometer.init());
}

void test_baro_read() {
  	mockBarometer.read();
    TEST_ASSERT_EQUAL_FLOAT(1.0f, mockBarometer.getPressure());
    TEST_ASSERT_EQUAL_FLOAT(2.0f, mockBarometer.getTemp());
}

// ---



// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    RUN_TEST(test_baro_init); // no parentheses after function name
    RUN_TEST(test_baro_read); // no parentheses after function name


    UNITY_END();
}
// ---
