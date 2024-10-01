#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/Error/ErrorHandler.h"

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

void test_error_add() {
    // ErrorType type, const char *message, int errorLocation
    errorHandler.addError(mmfs::ErrorType::CRITICAL_ERROR, "Test error message", 0);
    mmfs::Error *error = errorHandler.getFirstError();
    TEST_ASSERT_NOT_NULL(error);
    TEST_ASSERT_EQUAL(mmfs::ErrorType::CRITICAL_ERROR, error->type);
    TEST_ASSERT_EQUAL_STRING("Test error message", error->getMessage());
    TEST_ASSERT_EQUAL(0, error->getErrorLocation());
}

void test_error_add_no_location() {
    // ErrorType type, const char *message
    errorHandler.addError(mmfs::ErrorType::NONCRITICAL_WARNING, "Test error message 2");
    mmfs::Error *error = errorHandler.getLastError();
    TEST_ASSERT_NOT_NULL(error);
    TEST_ASSERT_EQUAL(mmfs::ErrorType::NONCRITICAL_WARNING, error->type);
    TEST_ASSERT_EQUAL_STRING("Test error message 2", error->getMessage());
    TEST_ASSERT_EQUAL(-1, error->getErrorLocation());
}

void test_error_add_existing() {
    // Error *error
    mmfs::Error *error = new mmfs::Error(mmfs::ErrorType::HARDWARE_ERROR, "Test error message 3", 5);
    errorHandler.addError(error);
    mmfs::Error *error2 = errorHandler.getLastError();
    TEST_ASSERT_NOT_NULL(error2);
    TEST_ASSERT_EQUAL(mmfs::ErrorType::HARDWARE_ERROR, error2->type);
    TEST_ASSERT_EQUAL_STRING("Test error message 3", error2->getMessage());
    TEST_ASSERT_EQUAL(5, error2->getErrorLocation());
}

void test_error_to_string() {
    // Error *error
    mmfs::Error *error = new mmfs::Error(mmfs::ErrorType::SOFTWARE_ERROR, "Test error message 4", 515);
    TEST_ASSERT_EQUAL_STRING("SOFTWARE_ERROR: Test error message 4 at 515", error->toString());
    delete error;
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_error_add);
    RUN_TEST(test_error_add_no_location);
    RUN_TEST(test_error_add_existing);
    RUN_TEST(test_error_to_string);


    UNITY_END();

    return 0;
}
// ---