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

std::filesystem::path TEST_PATH_DATA = std::filesystem::path("test_data") / "test.csv";

// Test functions must be void and take no arguments, put them here

void test_reader_init() {
	LaunchDataReader reader(TEST_PATH_DATA);
    TEST_ASSERT_EQUAL(true, reader.isInit());
}

void test_read_header() {
     LaunchDataReader reader(TEST_PATH_DATA);
     std::string cols[3];
     int numCols;

     bool res = reader.readColumnHeaders(numCols, cols);
     TEST_ASSERT_EQUAL(true, res);

//     std::cout << "Number of columns: " << numCols << std::endl;
     std::string expectedCols[3] = {
       "pres",
       "temp",
       "smth"
     };

     for(int i = 0; i < numCols; i++) {
     	TEST_ASSERT_EQUAL(true, cols[i] == expectedCols[i]);
     }
}

void test_read_line() {
     LaunchDataReader reader(TEST_PATH_DATA);
     std::string cols[3];
     int numCols = 0;

     bool res = reader.readColumnHeaders(numCols, cols);

     float* measurements = (float*) malloc(numCols * sizeof(float));

     int line = 0;
     while(reader.readLine(measurements)) {
        std::cout << "[" << line << "] " << "START" << std::endl;
        std::cout << "[" << line << "] " << measurements[0] << std::endl;
        std::cout << "[" << line << "] " << measurements[1] << std::endl;
        std::cout << "[" << line << "] " << measurements[2] << std::endl;
        std::cout << "[" << line << "] " << "END" << std::endl;

        TEST_ASSERT_EQUAL_FLOAT(1.0f+line, measurements[0]);
        TEST_ASSERT_EQUAL_FLOAT(2.0f+line, measurements[1]);
        TEST_ASSERT_EQUAL_FLOAT(3.0f+line, measurements[2]);
        line++;
     }

     TEST_ASSERT_EQUAL(6, line);
}
// ---



// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    RUN_TEST(test_reader_init); // no parentheses after function name
    RUN_TEST(test_read_header); // no parentheses after function name
    RUN_TEST(test_read_line); // no parentheses after function name


    UNITY_END();
}
// ---
