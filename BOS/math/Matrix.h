/* 
 * File:   Matrix.h
 * Author: dducks
 *
 * Created on May 10, 2015, 9:06 PM
 */

#ifndef MATRIX_H
#define	MATRIX_H

#include "Vector.h"

#include <iostream>
#include <type_traits>

template< unsigned int M, unsigned int N, typename TYPE = double >
class Matrix {
    TYPE array[M*N];
    using Mat_t = Matrix<M,N,TYPE>;
    
public:
    
    Matrix() {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) = n == m ? (M==N ? 1 : 0) : 0;
            }
        }
    }
    
    Matrix(const Mat_t& other) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) = other(m,n);
            }
        }
    }
    
    Mat_t& operator=(const Mat_t& other) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) = other(m,n);
            }
        }
        return *this;
    }
    
    TYPE& operator()(unsigned int m, unsigned int n) {
        return array[n*M+m];
    }
    const TYPE& operator()(unsigned int m, unsigned int n) const {
        return array[n*M+m];
    }
    
    TYPE* data() {
        return array;
    }
    const TYPE * data() const {
        return array;
    }
    
    Mat_t& operator+=(const Mat_t& other) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) += other(m,n);
            }
        }
        return *this;
    }
    
    Mat_t& operator-=(const Mat_t& other) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) -= other(m,n);
            }
        }
        return *this;
    }
    
    Mat_t& operator*=(const TYPE& d) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) *= d;
            }
        }
        return *this;
    }
    
    Mat_t& operator/=(const TYPE& d) {
        for (unsigned int n = 0; n < N; n++) {
            for (unsigned int m = 0; m < M; m++) {
                (*this)(m,n) /= d;
            }
        }
        return *this;
    }
    
    Mat_t operator+(const Mat_t& other) {
        Mat_t retVal = *this;
        retVal += other;
        return retVal;
    }
    
    Mat_t operator-(const Mat_t& other) {
        Mat_t retVal = *this;
        retVal -= other;
        return retVal;
    }
    
    Mat_t operator*(const TYPE& other) {
        Mat_t retVal = *this;
        retVal *= other;
        return retVal;
    }
    
    template<unsigned int P>
    Matrix<M,P,TYPE> operator*(const Matrix<N,P,TYPE>& other) {
        Matrix<M,P,TYPE> retVal;
        for (unsigned int i = 0; i < M; i++) {
            for (unsigned int j = 0; j < P; j++) {
                TYPE val = 0;
                for (unsigned int k = 0; k < N; k++) {
                    val += (*this)(i,k) * other(k,j);
                }
                retVal(i,j) = val;
            }
        }
        return retVal;
    }
    
    Vector<M> operator*(const Vector<N>& other) {
        Vector<M> retVal;
        for (unsigned int i = 0; i < M; i++) {
            TYPE val = 0;
            for (unsigned int j = 0; j < N; j++) {
                val += (*this)(i,j) * other[j];
            }
            retVal[i] = val;
        }
        return retVal;
    }
    
};

template< unsigned int M, unsigned int N, typename T >
std::ostream& operator<<(std::ostream& os, const Matrix<M,N,T>& mat) {
    for (unsigned int m = 0; m < M; m++) {
        if (m != 0)
            os << '\n';
        for (unsigned int n = 0; n < N; n++) {
            os << mat(m,n) << ' ';
        }
    }
    return os;
}


#endif	/* MATRIX_H */

