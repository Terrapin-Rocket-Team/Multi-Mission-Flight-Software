#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <algorithm>
#include <cstdio>

namespace mmfs
{
    class Matrix
    {
    public:
        Matrix();
        Matrix(int rows, int cols, double *array);
        ~Matrix();
        Matrix(const Matrix &other);
        Matrix &operator=(const Matrix &other);
        int getRows() const;
        int getCols() const;
        double *getArr();
        double get(int i, int j);
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
        static Matrix ident(int n);
        void disp();

    private:
        int rows;
        int cols;
        double *array;
        void luDecompositionWithPartialPivoting(double *A, int *pivot, int n);
        void solveLU(double *A, int *pivot, double *b, double *x, int n);
    };
} // namespace mmfs
#endif