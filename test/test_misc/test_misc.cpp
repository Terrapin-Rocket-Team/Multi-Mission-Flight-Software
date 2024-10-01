#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/Constants.h"

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

// void test_function_name() {
//     TEST_ASSERT_EQUAL(expected, actual);
//     TEST_ASSERT_EQUAL_FLOAT(expected, actual);
// }

//verify redefining of constants

void test_constants() {

    SENSOR_BIAS_CORRECTION_DATA_LENGTH = 10;
    SENSOR_BIAS_CORRECTION_DATA_IGNORE = 50;
    
    TEST_ASSERT_EQUAL_INT(10, SENSOR_BIAS_CORRECTION_DATA_LENGTH);
    TEST_ASSERT_EQUAL_INT(50, SENSOR_BIAS_CORRECTION_DATA_IGNORE);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name

    UNITY_END();

    return 0;
}
// ---