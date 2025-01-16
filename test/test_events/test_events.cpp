#include <unity.h>
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/Events/DefaultEvents.h"

// ---

// Set up and global variables or mocks for testing here
using namespace mmfs;

class EventListener : public IEventListener
{
public:
    EventListener() {};
    bool a = false;
    int i = 0;

    void onEvent(const Event *e) override
    {
        switch (e->ID)
        {
        case "Case1"_i:
            i = 5;
            break;
        case "GPS_FIX"_i:
            a = static_cast<const GPSFix *>(e)->hasFix;
            break;
        default:
            break;
        }
    }
};
EventListener e1, e2;
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

void testCase1()
{
    getEventManager().invoke(Event{"Case1"_i});
    TEST_ASSERT_EQUAL(5, e1.i);
    TEST_ASSERT_EQUAL(5, e2.i);
    TEST_ASSERT_FALSE(e1.a);
    TEST_ASSERT_FALSE(e2.a);
}

void testCase2()
{
    getEventManager().invoke(GPSFix{"GPS_FIX"_i, true});
    TEST_ASSERT_EQUAL(5, e1.i);
    TEST_ASSERT_EQUAL(5, e2.i);
    TEST_ASSERT_TRUE(e1.a);
    TEST_ASSERT_TRUE(e2.a);
}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name

    RUN_TEST(testCase1);
    RUN_TEST(testCase2);

    UNITY_END();
}
// ---