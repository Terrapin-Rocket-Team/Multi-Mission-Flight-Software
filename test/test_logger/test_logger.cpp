#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/State/State.h"
#include "../../lib/NativeTestMocks/UnitTestSensors.h"
#include "../../lib/NativeTestMocks/MockLoggingBackend.h"
#include "../../src/RecordData/DataReporter/DataFormatter.h"

// ---

// Set up and global variables or mocks for testing here

class TestState : public State
{
public:
    TestState(Sensor **sensors, int numSensors, Filter *filter) : State(sensors, numSensors, filter) {}
    void determineStage() override {}
};

using namespace mmfs;
char *fakepsram = nullptr;
MockFileData *flightFile, *logFile, *preFlightFile;

FakeGPS gps;
FakeBarometer baro;
Sensor *sensors[2] = {&gps, &baro};
TestState state(sensors, 2, nullptr);
DataReporter *arr[] = {&state, &gps, &baro};
Logger *testLogger;

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
    LoggingBackendFileMock *file = static_cast<LoggingBackendFileMock *>(testLogger->backend->open("file"));
    TEST_ASSERT_TRUE(testLogger->backend->exists("file"));
    TEST_ASSERT_TRUE(file->write("data", 1));
    TEST_ASSERT_TRUE(file->print("data"));
    TEST_ASSERT_TRUE(file->println("data"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("ddatadata\n", file->data->arr, 10);
    TEST_ASSERT_EQUAL(10, file->data->size);
}

void test_testLogger_init()
{
    LoggingBackendMock *m = (LoggingBackendMock *)testLogger->backend;
    testLogger->init(arr, 3);
    TEST_ASSERT_TRUE(testLogger->isReady());
    TEST_ASSERT_TRUE(testLogger->backend->exists("1_FlightData.csv"));
    TEST_ASSERT_TRUE(testLogger->backend->exists("1_Log.txt"));
    TEST_ASSERT_TRUE(testLogger->backend->exists("1_PreFlightData.csv"));
    flightFile = ((LoggingBackendFileMock *)(testLogger->flightDataFile))->data;
    logFile = ((LoggingBackendFileMock *)(testLogger->logFile))->data;
    preFlightFile = ((LoggingBackendFileMock *)(testLogger->preFlightFile))->data;
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
    TEST_ASSERT_EQUAL(145, preFlightFile->size);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("1.000,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000," // state
                                 "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0,00:00:00,"        // gps
                                 "0.000,0.000,44307.690,0.000\n",                                 // baro
                                 preFlightFile->arr, 143);

    gps.set(180, 180, 1000);
    state.updateState(2);
    testLogger->recordFlightData(); // at this point, this string is all that the buffer should have in it.
    TEST_ASSERT_EQUAL_CHAR_ARRAY(
        "1.000,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,"
        "0.0000000,0.0000000,0.000,0.000,0.000,0.000,0,00:00:00,"
        "0.000,0.000,44307.690,0.000\n"
        "2.000,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,"
        "180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,"
        "0.000,0.000,44307.690,0.000\n",
        preFlightFile->arr, 297);
    TEST_ASSERT_EQUAL(297, preFlightFile->size);
}

void test_recordFlightData_in_flight()
{
    testLogger->setRecordMode(FLIGHT);
    for (int i = 30; i < 40; i++)
    {
        state.updateState((i + 1) * .1);
        testLogger->recordFlightData();
    }
    TEST_ASSERT_EQUAL(297, preFlightFile->size); // no new data should be written to the preflight data file
    TEST_ASSERT_EQUAL(152 * 10, flightFile->size);
}

void test_dumpData()
{
    testLogger->setRecordMode(GROUND); // dump the data
    flightFile->arr[flightFile->size] = '\0';
    printf("%s\n", flightFile->arr);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(
        "3.100,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.200,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.300,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.400,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.500,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.600,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.700,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.800,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "3.900,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n"
        "4.000,0,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,180.0000000,180.0000000,1000.000,0.000,0.000,0.000,0,00:00:00,0.000,0.000,44307.690,0.000\n",
        flightFile->arr, 1520);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    gps.setFixQual(3);

    testLogger = &getLogger();
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