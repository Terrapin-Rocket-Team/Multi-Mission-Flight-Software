#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <algorithm>
#include <cstdio>
#include <stdint.h>

namespace mmfs
{
    class Matrix
    {
    public:
        Matrix();
        Matrix(uint8_t rows, uint8_t cols, double *array);
        ~Matrix();
        Matrix(const Matrix &other);
        Matrix &operator=(const Matrix &other);
        uint8_t getRows() const;
        uint8_t getCols() const;
        double *getArr() const;
        double get(uint8_t i, uint8_t j);
        Matrix operator*(Matrix other);
        Matrix multiply(Matrix other);
        Matrix operator*(double scalar);
        Matrix multiply(double scalar);
        Matrix operator+(Matrix other);
        Matrix add(Matrix other);
        Matrix operator-(Matrix other);
        Matrix subtract(Matrix other);
        Matrix T();
        Matrix transpose();
        Matrix inv();
        Matrix inverse();
        double trace();
        static Matrix ident(uint8_t n);
        void disp();

        // non-const accessor / mutator
        double &operator()(uint8_t row, uint8_t col)
        {
            if (row < rows && col < cols)
                return array[row * cols + col];
            return array[0];
        }

        // const overload for read‐only access
        const double &operator()(uint8_t row, uint8_t col) const
        {
            if (row < rows && col < cols)
                return array[row * cols + col];
            return array[0];
        }

    private:
        uint8_t rows;
        uint8_t cols;
        double *array;
        void luDecompositionWithPartialPivoting(double *A, int *pivot, int n);
        void solveLU(double *A, int *pivot, double *b, double *x, int n);
    };
} // namespace mmfs
#endif