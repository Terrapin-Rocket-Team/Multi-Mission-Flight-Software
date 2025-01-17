#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/State/State.h"
#include "../../lib/NativeTestMocks/UnitTestSensors.h"
#include "TestingLogger.h"
#include "../../src/RecordData/DataFormatter.h"
#include "../../src/RecordData/PSRAMFile.h"

// ---

// Set up and global variables or mocks for testing here

using namespace mmfs;
char *fakepsram = nullptr;
FsFileData *flightFile, *logFile, *preFlightFile;

FakeGPS gps;
FakeBarometer baro;
Sensor *sensors[2] = {&gps, &baro};
State state(sensors, 2, nullptr);
DataReporter *arr[] = {&state, &gps, &baro};
TestingLogger *testLogger;

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
    SdFs *sd = testLogger->getSdFs();
    TEST_ASSERT_TRUE(sd->begin(0));
    TEST_ASSERT_TRUE(sd->restart());
    TEST_ASSERT_FALSE(sd->exists("file"));
    FsFile file = sd->open("file", 0);
    TEST_ASSERT_TRUE(sd->exists("file"));
    TEST_ASSERT_TRUE(file.close());
    TEST_ASSERT_TRUE(file.write("data", 1));
    TEST_ASSERT_TRUE(file.print("data"));
    TEST_ASSERT_TRUE(file.println("data"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ddatadata\n", file.data->arr, 10);
    TEST_ASSERT_EQUAL(10, file.data->size);
}

void test_testLogger_init()
{
    testLogger->init(arr, 3, 1, 1);
    TEST_ASSERT_TRUE(testLogger->isPsramReady());
    TEST_ASSERT_TRUE(testLogger->isSdCardReady());
    TEST_ASSERT_TRUE(testLogger->isReady());
    TEST_ASSERT_TRUE(testLogger->getSdFs()->exists("1_FlightData.csv"));
    TEST_ASSERT_TRUE(testLogger->getSdFs()->exists("1_Log.txt"));
    TEST_ASSERT_TRUE(testLogger->getSdFs()->exists("1_PreFlightData.csv"));
    flightFile = testLogger->getFlightDataFile()->data;
    logFile = testLogger->getLogFile()->data;
    preFlightFile = testLogger->getPreFlightFile()->data;
}

void test_recordLogData_on_ground()
{

    testLogger->recordLogData(.1, INFO_, TO_FILE, "This is a test");
    TEST_ASSERT_EQUAL(0, flightFile->size);
    TEST_ASSERT_EQUAL(0, preFlightFile->size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0.100 - [INFO] This is a test\n", logFile->arr, 30);
    testLogger->recordLogData(.2, INFO_, "This is another test");
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0.100 - [INFO] This is a test\n0.200 - [INFO] This is another test\n", logFile->arr, 66);
    TEST_ASSERT_EQUAL_STRING("0.200 - [INFO] This is another test\n", Serial.fakeBuffer);
    testLogger->recordLogData(.3, INFO_, TO_USB, "This is a third test");
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0.100 - [INFO] This is a test\n0.200 - [INFO] This is another test\n", logFile->arr, 66);
    TEST_ASSERT_EQUAL_STRING("0.200 - [INFO] This is another test\n0.300 - [INFO] This is a third test\n", Serial.fakeBuffer);
    Serial.clearBuffer();
    testLogger->recordLogData(.4, WARNING_, TO_USB, 50, "This is a warning! #%d did not %s!", 1, "work");
    TEST_ASSERT_EQUAL_CHAR_ARRAY("0.100 - [INFO] This is a test\n0.200 - [INFO] This is another test\n", logFile->arr, 66);
    TEST_ASSERT_EQUAL_STRING("0.400 - [WARNING] This is a warning! #1 did not work!\n", Serial.fakeBuffer);
    Serial.clearBuffer();
}

void test_recordFlightData_on_ground()
{
    state.init(); // will record log data, so don't do this earlier in order to test log data.
    baro.set(0, 0);
    gps.set(0, 0, 0);
    state.updateState(1);
    testLogger->recordFlightData();
    TEST_ASSERT_EQUAL(0, flightFile->size); // no data should be written to the flight data file
    TEST_ASSERT_EQUAL(0, preFlightFile->size);
    PSRAMFile *ramBufferFile = testLogger->getRamBufferFile();
    TEST_ASSERT_EQUAL(DataFormatter::getPackedLen(arr, 3), ramBufferFile->getSize());
    for (int i = 1; i < 10; i++) // write once to the SD card
    {
        state.updateState((i + 1) * .1);
        testLogger->recordFlightData();
    }
    TEST_ASSERT_EQUAL(143, preFlightFile->size);
    TEST_ASSERT_EQUAL(0, flightFile->size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000," // state
                                 "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0,00:00:00,"      // gps
                                 "0.000,0.000,44307.691,0.000\n",                               // baro
                                 preFlightFile->arr, 143);

    gps.set(180, 180, 1000);
    for (int i = 10; i < 20; i++)
    {
        state.updateState((i + 1) * .1);
        testLogger->recordFlightData(); // at this point, this string is all that the buffer should have in it.
    }
    TEST_ASSERT_EQUAL_CHAR_ARRAY(
        "1.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,"
        "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0,00:00:00,"
        "0.000,0.000,44307.691,0.000\n"
        "2.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,"
        "180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,"
        "0.000,0.000,44307.691,0.000\n",
        preFlightFile->arr, 275 + 18);
    TEST_ASSERT_EQUAL(275 + 18, preFlightFile->size);
    TEST_ASSERT_EQUAL(DataFormatter::getPackedLen(arr, 3) * 10, ramBufferFile->getSize());
}

void test_recordFlightData_in_flight()
{
    TEST_ASSERT_EQUAL(0, testLogger->getRamFlightDataFile()->getSize());
    testLogger->setRecordMode(FLIGHT);
    for (int i = 30; i < 40; i++)
    {
        state.updateState((i + 1) * .1);
        testLogger->recordFlightData();
    }
    TEST_ASSERT_EQUAL(275 + 18, preFlightFile->size); // no new data should be written to the flight data file
    TEST_ASSERT_EQUAL(DataFormatter::getPackedLen(arr, 3) * 10, testLogger->getRamFlightDataFile()->getSize());
}

void test_dumpData()
{
    testLogger->setRecordMode(GROUND); // dump the data
    flightFile->arr[flightFile->size] = '\0';
    printf("%s\n", flightFile->arr);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(
        "1.100,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.200,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.300,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.400,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.500,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.600,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.700,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.800,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "1.900,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "2.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.100,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.200,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.300,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.400,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.500,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.600,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.700,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.800,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "3.900,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n"
        "4.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.691,0.000\n",
        flightFile->arr, 3000);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    gps.setFixQual(3);
    // ONE_MB = 1024; // temp make this one KB for testing to limit amount of memory to verify
    fakepsram = new char[ONE_MB * external_psram_size];
    PSRAM_STARTING_ADDRESS = (uintptr_t)fakepsram;
    PSRAM_CLUSTER_SIZE = 32768; // in bytes

    testLogger = new TestingLogger();
    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_SdFs_mock);
    RUN_TEST(test_testLogger_init);
    RUN_TEST(test_recordLogData_on_ground);
    RUN_TEST(test_recordFlightData_on_ground);
    RUN_TEST(test_recordFlightData_in_flight);
    RUN_TEST(test_dumpData);

    UNITY_END();

    return 0;
}
// ---