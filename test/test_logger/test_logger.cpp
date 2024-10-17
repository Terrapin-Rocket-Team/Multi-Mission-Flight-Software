#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/State/State.h"
#include "../../lib/NativeTestMocks/FakeSensors.h"

// ---

// Set up and global variables or mocks for testing here

using namespace mmfs;
char *fakepsram = nullptr;
FsFile flightFile, logFile;

FakeGPS gps;
FakeBarometer baro;
Sensor *sensors[2] = {&gps, &baro};
State state(sensors, 2, nullptr);

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

void test_SdFs_mock()
{
    SdFs sd;
    TEST_ASSERT_TRUE(sd.begin(0));
    TEST_ASSERT_TRUE(sd.restart());
    TEST_ASSERT_FALSE(sd.exists("file"));
    FsFile file = sd.open("file", 0);
    TEST_ASSERT_TRUE(sd.exists("file"));
    TEST_ASSERT_TRUE(file.close());
    TEST_ASSERT_TRUE(file.write("data", 1));
    TEST_ASSERT_TRUE(file.print("data"));
    TEST_ASSERT_TRUE(file.println("data"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ddatadata\n", file.arr, 10);
    TEST_ASSERT_EQUAL(10, *file.size);
}

void test_logger_init()
{
    logger.init(&state);
    TEST_ASSERT_TRUE(logger.isPsramReady());
    TEST_ASSERT_TRUE(logger.isSdCardReady());
    TEST_ASSERT_TRUE(logger.isReady());
    TEST_ASSERT_TRUE(logger.getSdFs()->exists("1_FlightData.csv"));
    TEST_ASSERT_TRUE(logger.getSdFs()->exists("1_Log.txt"));
    flightFile = logger.getSdFs()->open("1_FlightData.csv", 0);
    logFile = logger.getSdFs()->open("1_Log.txt", 0);
    SdFs *sd = logger.getSdFs();
    logFile.arr[0] = '1';
    int i;
    printf("%p", sd);
}

void test_recordLogData_on_ground()
{

    logger.recordLogData(1, INFO_, "This is a test", TO_FILE);
    TEST_ASSERT_EQUAL(0, *flightFile.size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1.000 - [INFO] This is a test\n", logFile.arr, 30);
    logger.recordLogData(2, INFO_, "This is another test");
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1.000 - [INFO] This is a test\n2.000 - [INFO] This is another test\n", logFile.arr, 66);
    TEST_ASSERT_EQUAL_STRING("2.000 - [INFO] This is another test\n", Serial.fakeBuffer);
    logger.recordLogData(3, INFO_, "This is a third test", TO_USB);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1.000 - [INFO] This is a test\n2.000 - [INFO] This is another test\n", logFile.arr, 66);
    TEST_ASSERT_EQUAL_STRING("2.000 - [INFO] This is another test\n3.000 - [INFO] This is a third test\n", Serial.fakeBuffer);
    Serial.clearBuffer();
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    ONE_MB = 1024; // temp make this one KB for testing to limit amount of memory to verify
    fakepsram = new char[ONE_MB * external_psram_size];
    PSRAM_STARTING_ADDRESS = (uintptr_t)fakepsram;
    PSRAM_CLUSTER_SIZE = 256; // in bytes

    delete psram;
    psram = new PSRAM();
    logger = Logger();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_SdFs_mock);
    RUN_TEST(test_logger_init);
    RUN_TEST(test_recordLogData_on_ground);

    UNITY_END();

    return 0;
}
// ---