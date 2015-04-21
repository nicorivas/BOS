/* 
 * File:   Vector.h
 * Author: dducks
 *
 * Created on April 20, 2015, 6:42 PM
 */

#ifndef VECTOR_H
#define	VECTOR_H

#include <cassert>
#include <cmath>
#include <array>
#include <initializer_list>
#include <iostream>


template<unsigned int DIM>
class Vector final {
    std::array<double, DIM> array;
public:
    Vector() {
        for (double& d: array)
            d = 0;
    }
    
    Vector(const Vector& other) {
        array = other.array;
    }
    
    Vector& operator=(const Vector& other) {
        array = other.array;
        return *this;
    }
    
    Vector(std::initializer_list<double> d) {
        assert(d.size() == DIM);
        auto listIt = d.begin();
        auto arrayIt = array.begin();
        
        for (; listIt != d.end(); listIt++, arrayIt++) {
            *arrayIt = *listIt;
        }
    }
    
    double operator[] (unsigned int idx) const {
        return array[idx];
    }
    
    double& operator[] (unsigned int idx) { 
        return array[idx];
    }
    
    Vector<DIM>& operator+=(Vector<DIM> v) {
        
        for (unsigned int i = 0; i < DIM; i++) {
            array[i] += v[i];
        }
        
        return *this;
    }
    
    Vector<DIM>& operator*=(double d) {
        for (double& elem : array)
            elem *= d;
        
        return *this;
    }
    
    Vector<DIM>& operator-=(Vector<DIM> v) {
        return (*this += -v);
    }
    
    Vector<DIM>& operator/=(double d) {
        return *this *= (1.0/d);
    }
    
    Vector<DIM> operator-() {
        Vector<DIM> retVal = *this;
        retVal *= -1;
        return retVal;
    }
    
    Vector<DIM> operator+(Vector<DIM> other) const {
        Vector<DIM> retVal = *this;
        retVal += other;
        return retVal;
    }
    
    Vector<DIM> operator-(Vector<DIM> other) const {
        Vector<DIM> retVal = *this;
        retVal += -other;
        return retVal;
    }
    
    Vector<DIM> operator*(double d) const {
        Vector<DIM> retVal = *this;
        retVal *= d;
        return retVal;
    }
    
    Vector<DIM> operator/(double d) const {
        return (*this) * (1/d);
    }
    
};

template<unsigned int DIM>
Vector<DIM> operator*(double d, Vector<DIM> v) {
    return v*d;
}

template<unsigned int DIM>
Vector<DIM> ewise(Vector<DIM> a, Vector<DIM> b) {
    Vector<DIM> retVal;
    for (unsigned int i = 0; i < DIM; i++) {
        retVal[i] = a[i] * b[i];
    }
    return retVal;
}

template<unsigned int DIM>
Vector<DIM> abs(Vector<DIM> a) {
    Vector<DIM> retVal;
    for (unsigned int i = 0; i < DIM; i++) {
        retVal[i] = a[i] > 0 ? a[i] : -a[i];
    }
    return retVal;
}

template<unsigned int DIM>
double dot(Vector<DIM> a, Vector<DIM> b) {
    double d = 0;
    for (unsigned int i = 0; i < DIM; i++) {
        d += a[i] * b[i];
    }
    return d;
}

template<unsigned int DIM>
std::ostream& operator<<(std::ostream& os, Vector<DIM> v) {
    os << '(';
    for (unsigned int i = 0; i < DIM; i++) {
        if (i != 0)
            os << ',';
        
        os << v[i];
    }
    os << ')';
    return os;
}

#endif	/* VECTOR_H */

