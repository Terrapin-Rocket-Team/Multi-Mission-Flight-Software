#include <unity.h>
#include "../src/Math/Matrix.h"
#include "../../lib/NativeTestMocks/NativeTestHelper.h"

using mmfs::Matrix;

Matrix m1(2, 2, new double[4]{1, 2, 3, 4});
Matrix m2(2, 2, new double[4]{5, 6, 7, 8});
Matrix m3(2, 3, new double[6]{1, 2, 3, 4, 5, 6});
Matrix m5(4, 4, new double[16]{0,0,-1,2,0,1,0,0,9,0,0,0,0,0,0,1});



void test_copy_constructor()
{
    Matrix m4(m1);
    m1.disp();
    TEST_ASSERT_NOT_EQUAL(&m1, &m4);
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(1, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(2, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(3, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(4, m4.getArr()[3]);
}

void test_assignment_operator()
{
    Matrix m4 = m1;
    TEST_ASSERT_NOT_EQUAL(&m1, &m4);
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(1, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(2, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(3, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(4, m4.getArr()[3]);
}

void test_multiply_operator_matrix()
{
    Matrix m4 = m1 * m2;
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(19, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(22, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(43, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(50, m4.getArr()[3]);
}

void test_multiply_operator_scalar()
{
    Matrix m4 = m1 * 2;
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(2, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(4, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(6, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(8, m4.getArr()[3]);
}

void test_add_operator_matrix()
{
    Matrix m4 = m1 + m2;
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(6, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(8, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(10, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(12, m4.getArr()[3]);
}
void test_subtract_operator_matrix()
{
    Matrix m4 = m1 - m2;
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(-4, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(-4, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(-4, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(-4, m4.getArr()[3]);
}
void test_transpose()
{
    Matrix m4 = m3.T();
    TEST_ASSERT_EQUAL(3, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(1, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(4, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(2, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(5, m4.getArr()[3]);
    TEST_ASSERT_EQUAL(3, m4.getArr()[4]);
    TEST_ASSERT_EQUAL(6, m4.getArr()[5]);
}
void test_inverse_small()
{
    Matrix m4 = m1.inv();
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_FLOAT_WITHIN(1e-6, -2.0, m4.getArr()[0]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0, m4.getArr()[1]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.5, m4.getArr()[2]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, -0.5, m4.getArr()[3]);
}

void test_inverse_large()
{
    Matrix m4 = m5.inv();
    TEST_ASSERT_EQUAL(4, m4.getRows());
    TEST_ASSERT_EQUAL(4, m4.getCols());
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[0]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[1]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0/9, m4.getArr()[2]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[3]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[4]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0, m4.getArr()[5]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[6]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[7]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, -1.0, m4.getArr()[8]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[9]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[10]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 2.0, m4.getArr()[11]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[12]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[13]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 0, m4.getArr()[14]);
    TEST_ASSERT_FLOAT_WITHIN(1e-6, 1.0, m4.getArr()[15]);
}

void setUp(void)
{
    //runs before each test
    //must be included for unity to work, even if empty
}

void tearDown(void)
{
    //runs after each test
    //must be included for unity to work, even if empty
}


int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_copy_constructor);
    RUN_TEST(test_assignment_operator);
    RUN_TEST(test_multiply_operator_matrix);
    RUN_TEST(test_multiply_operator_scalar);
    RUN_TEST(test_add_operator_matrix);
    RUN_TEST(test_subtract_operator_matrix);
    RUN_TEST(test_transpose);
    RUN_TEST(test_inverse_small);
    RUN_TEST(test_inverse_large);

    UNITY_END();
}

