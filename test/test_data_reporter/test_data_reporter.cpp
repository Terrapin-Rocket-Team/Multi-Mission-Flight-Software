#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "testDR.cpp"

// ---

// Set up and global variables or mocks for testing here

using namespace mmfs;
testDR obj;

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

void test_constructor(){
    TEST_ASSERT_EQUAL_STRING(obj.getName(), "Reporter #1");
    TEST_ASSERT_NOT_NULL(obj.getPackedData());
    TEST_ASSERT_EQUAL_INT(obj.getPackedDataSize(), sizeof(int) + sizeof(float) + sizeof(double));
    TEST_ASSERT_EQUAL_INT(obj.getNumColumns(), 3);
    TEST_ASSERT_NOT_NULL(obj.getPackedInfo());
}

void test_pack(){
    obj.packData();
    TEST_ASSERT_EQUAL(0, *reinterpret_cast<int *>(&obj.getPackedData()[0]));
    TEST_ASSERT_EQUAL(0, *reinterpret_cast<float *>(&obj.getPackedData()[4]));
    TEST_ASSERT_EQUAL(0, *reinterpret_cast<double *>(&obj.getPackedData()[8]));
    obj.var1 = 55;
    obj.var2= 1000.01;
    obj.packData();
    TEST_ASSERT_EQUAL(55, *reinterpret_cast<int *>(&obj.getPackedData()[0]));
    TEST_ASSERT_EQUAL_FLOAT(1000.01, *reinterpret_cast<float *>(&obj.getPackedData()[4]));
    TEST_ASSERT_EQUAL_FLOAT(1000.01, *reinterpret_cast<double *>(&obj.getPackedData()[8]));
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_constructor);
    RUN_TEST(test_pack);
    UNITY_END();
}
// ---