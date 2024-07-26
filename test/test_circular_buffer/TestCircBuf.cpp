#include <unity.h>
#include "../NativeTestMocks/NativeTestHelper.h"

// include other headers you need to test here

#include "../../src/Math/CircBuffer.h"

// ---

// Set up and global variables or mocks for testing here

CircBuffer<double> *cb;

// ---

// These two functions are called before and after each test function, and are required in unity, even if empty.
void setUp(void)
{
    cb->clear();
}

void tearDown(void)
{
    // clean stuff up after each test here, if needed
}
// ---

// Test functions must be void and take no arguments, put them here

void test_clear()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    cb->clear();
    TEST_ASSERT_TRUE(cb->isEmpty());
    TEST_ASSERT_EQUAL_FLOAT(0.0, cb->peek());
}

void test_push()
{
    cb->push(1.0);
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->peek());
}

void test_pop()
{
    cb->push(1.0);
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->pop());
    TEST_ASSERT_TRUE(cb->isEmpty());
}

void test_peek()
{
    cb->push(1.0);
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->peek());
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->pop());
}

void test_isFull()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    cb->push(4.0);
    cb->push(5.0);
    TEST_ASSERT_TRUE(cb->isFull());
}

void test_isEmpty()
{
    TEST_ASSERT_TRUE(cb->isEmpty());
}

void test_push_pop()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    cb->push(4.0);
    cb->push(5.0);
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->pop());
    TEST_ASSERT_EQUAL_FLOAT(2.0, cb->pop());
    TEST_ASSERT_EQUAL_FLOAT(3.0, cb->pop());
    cb->push(6.0);
    cb->push(7.0);
    TEST_ASSERT_EQUAL_FLOAT(4.0, cb->peek());
}

void test_wrap()
{
    cb->push(1.0);
    cb->push(2.0);

    cb->push(3.0);
    cb->push(4.0);
    cb->push(5.0);
    cb->push(6.0);
    cb->push(7.0);
    TEST_ASSERT_EQUAL_FLOAT(3.0, cb->peek());
    TEST_ASSERT_EQUAL_FLOAT(7.0, (*cb)[4]); // (*cb)[4] is the 5th element in the buffer
}

void test_getCount()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    TEST_ASSERT_EQUAL(3, cb->getCount());
}

void test_getSize()
{
    TEST_ASSERT_EQUAL(5, cb->getSize());
}

void test_getItem()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    TEST_ASSERT_EQUAL_FLOAT(1.0, (*cb)[0]);
    TEST_ASSERT_EQUAL_FLOAT(2.0, (*cb)[1]);
    TEST_ASSERT_EQUAL_FLOAT(3.0, (*cb)[2]);
}

void test_setItem()
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    (*cb)[1] = 4.0;
    TEST_ASSERT_EQUAL_FLOAT(1.0, cb->pop());
    TEST_ASSERT_EQUAL_FLOAT(4.0, cb->pop());
    TEST_ASSERT_EQUAL_FLOAT(3.0, cb->pop());
}

void test_iterate_over_front_half() // This test is more because we use this functionality in the sensor code, not because it directly tests the buffer
{
    cb->push(1.0);
    cb->push(2.0);
    cb->push(3.0);
    cb->push(4.0);
    cb->push(5.0);
    for(int i = 0; i < cb->getSize()/2; i++)
    {
        TEST_ASSERT_EQUAL_FLOAT(i+1, (*cb)[i]);
    }

}

// ---

// This is the main function that runs all the tests. It should be the last thing in the file.
int main(int argc, char **argv)
{
    cb = new CircBuffer<double>(5);

    UNITY_BEGIN();

    // Add your tests here
    // RUN_TEST(test_function_name); // no parentheses after function name
    RUN_TEST(test_clear);
    RUN_TEST(test_push);
    RUN_TEST(test_pop);
    RUN_TEST(test_peek);
    RUN_TEST(test_isFull);
    RUN_TEST(test_isEmpty);
    RUN_TEST(test_push_pop);
    RUN_TEST(test_wrap);
    RUN_TEST(test_getCount);
    RUN_TEST(test_getSize);
    RUN_TEST(test_getItem);
    RUN_TEST(test_setItem);
    RUN_TEST(test_iterate_over_front_half);

    UNITY_END();

    return 0;
}
// ---