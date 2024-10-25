#include <unity.h>
#include <iostream>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../lib/NativeTestMocks/LaunchDataReader.h"

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

void test_reader_init() {
	LaunchDataReader reader("test/test_launch_data_reader/test.csv");
    TEST_ASSERT_EQUAL(true, reader.is_initialized());
}

void test_read_header() {
     LaunchDataReader reader("test/test_launch_data_reader/test.csv");
     char *cols[10];
     for(int i = 0; i < 10; i++) {
       cols[i] = (char*) malloc(1024 * sizeof(char));
     }

     int numCols;
     bool res = reader.read_column_header(numCols, cols);
//     TEST_ASSERT_EQUAL(true, reader.is_initialized());

     std::cout << "Number of columns: " << numCols << std::endl;
     std::cout << "col 1: " << cols[0] << std::endl;
     TEST_ASSERT_EQUAL(0, strcmp(cols[0], "col1"));
}

// ---



// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    RUN_TEST(test_reader_init); // no parentheses after function name
    RUN_TEST(test_read_header); // no parentheses after function name


    UNITY_END();
}
// ---
