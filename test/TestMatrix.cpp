#include <unity.h>
#include "../src/Math/Matrix.h"
using mmfs::Matrix;

Matrix m1(2, 2, new double[4]{1, 2, 3, 4});
Matrix m2(2, 2, new double[4]{5, 6, 7, 8});
Matrix m3(2, 3, new double[6]{1, 2, 3, 4, 5, 6});

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_copy_constructor()
{
    Matrix m4(m1);
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
void test_inverse()
{
    Matrix m4 = m1.inv();
    TEST_ASSERT_EQUAL(2, m4.getRows());
    TEST_ASSERT_EQUAL(2, m4.getCols());
    TEST_ASSERT_EQUAL(-2, m4.getArr()[0]);
    TEST_ASSERT_EQUAL(1, m4.getArr()[1]);
    TEST_ASSERT_EQUAL(1.5, m4.getArr()[2]);
    TEST_ASSERT_EQUAL(-0.5, m4.getArr()[3]);
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
    RUN_TEST(test_inverse);

    UNITY_END();
}
