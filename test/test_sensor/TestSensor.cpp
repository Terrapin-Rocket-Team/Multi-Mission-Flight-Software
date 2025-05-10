#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/UnitTestSensors.h"

// ---

// Set up and global variables or mocks for testing here

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

void test_constructor() {
    FakeBarometer fake;
    TEST_ASSERT_EQUAL_STRING("FakeBarometer", fake.getName());
    TEST_ASSERT_EQUAL_STRING("Barometer", fake.getTypeString());
    TEST_ASSERT_EQUAL_INT32("Barometer"_i, fake.getType());
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    RUN_TEST(test_constructor); // no parentheses after function name

    UNITY_END();
}
// ---