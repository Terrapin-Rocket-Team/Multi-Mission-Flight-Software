#include "../../src/Radio/APRS/APRSMsg.h"
#include <unity.h>

using namespace mmfs;

APRSConfig con1 =
    {
        "N0CALL",
        "APRS",
        "WIDE1-1",
        '/',
        'O'};
APRSTelemData data1 =
    {
        .lat = 39.123456,
        .lng = -105.123456,
        .alt = 10000,
        .spd = 100,
        .hdg = 90.0,
        .stage = 1,
        .orientation = {90, 0.0, 180.0},
        .statusFlags = PI_ON | RECORDING_DATA};

APRSCmdData cmd1 =
    {
        .minutesUntilPowerOn = 5,
        .minutesUntilVideoStart = 6,
        .minutesUntilDataRecording = 7,
        .launch = false};

APRSTelemMsg msg1(con1);

APRSCmdMsg msg2(con1);

void test_telem_encode()
{
    msg1.encode();
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("N0CALL>APRS,WIDE1-1:!M:b>43u/X^ 7e**<j<17e!!NN&", msg1.getArr(), msg1.length(), "Telem encode failed");
    // this was found by running the values thru a separate base 91 encoder and appending each value in the right order.
}

void test_telem_decode()
{
    Vector<3> orientation = {data1.orientation.x(), data1.orientation.y(), data1.orientation.z()};
    msg1.decode();
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(data1.lat, msg1.data.lat, "Latitude decode failed");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(data1.lng, msg1.data.lng, "Longitude decode failed");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.04778, data1.alt, msg1.data.alt, "Altitude decode failed"); // 0.04778 is the resolution of the base91 encoding for altitude
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.1208, data1.spd, msg1.data.spd, "Speed decode failed"); // 0.1208 is the resolution of the base91 encoding for speed
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.04348, data1.hdg, msg1.data.hdg, "Heading decode failed"); // 0.04348 is the resolution of the base91 encoding for heading
    TEST_ASSERT_EQUAL_INT_MESSAGE(data1.stage, msg1.data.stage, "Stage decode failed");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.04348, orientation.x(), msg1.data.orientation.x(), "Orientation x decode failed"); // 0.04348 is the resolution of the base91 encoding for orientation
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.04348, orientation.y(), msg1.data.orientation.y(), "Orientation y decode failed");
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.04348, orientation.z(), msg1.data.orientation.z(), "Orientation z decode failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(data1.statusFlags, msg1.data.statusFlags, "Status flags decode failed");
}

void test_cmd_encode()
{
    msg2.encode();
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("N0CALL>APRS,WIDE1-1::N0CALL   :PWR:5 VID:6 DAT:7 L:0", msg2.getArr(), msg2.length(), "Cmd encode failed");
}

void test_cmd_decode()
{
    msg2.decode();
    TEST_ASSERT_EQUAL_INT_MESSAGE(cmd1.minutesUntilPowerOn, msg2.data.minutesUntilPowerOn, "Minutes until power on decode failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(cmd1.minutesUntilVideoStart, msg2.data.minutesUntilVideoStart, "Minutes until video start decode failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(cmd1.minutesUntilDataRecording, msg2.data.minutesUntilDataRecording, "Minutes until data recording decode failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(cmd1.launch, msg2.data.launch, "Launch decode failed");
}

void setUp(void)
{
    // runs before each test
    // must be included for unity to work, even if empty
}

void tearDown(void)
{
    // runs after each test
    // must be included for unity to work, even if empty
}

int main(int argc, char **argv)
{
    msg1.data = data1;

    msg2.data = cmd1;

    UNITY_BEGIN();

    RUN_TEST(test_telem_encode);
    RUN_TEST(test_telem_decode);
    RUN_TEST(test_cmd_encode);
    RUN_TEST(test_cmd_decode);

    UNITY_END();
}