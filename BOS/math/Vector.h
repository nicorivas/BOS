/* 
 * File:   Vector.h
 * Author: dducks
 *
 * Created on April 20, 2015, 6:42 PM
 */

#ifndef VECTOR_H
#define	VECTOR_H

//#include <hdf5/HDF5.h>

#include <cassert>
#include <cmath>
#include <array>
#include <initializer_list>
#include <iostream>


template<unsigned int DIM>
class Vector final {
    double array[DIM];
public:
    Vector() {
        for (double& d: array)
            d = 0;
    }
    
    Vector(const Vector& other) {
        for (std::size_t i = 0; i < DIM; i++) {
          array[i] = other.array[i];
        }
    }
    
    Vector& operator=(const Vector& other) {
        for (std::size_t i = 0; i < DIM; i++) {
          array[i] = other.array[i];
        }
        return *this;
    }
    
    Vector(std::initializer_list<double> d) {
        assert(d.size() == DIM);
        auto listIt = d.begin();
        
        for (double& v : array) {
            v = *listIt++;
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
    
    bool operator==(const Vector<DIM> &other) const {
        for (unsigned int i = 0; i < DIM; i++) {
            if (array[i] != other[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const Vector<DIM> &other) const {
        for (unsigned int i = 0; i < DIM; i++) {
            if (array[i] != other[i]) return true;
        }
        return false;
    }
    
    double lengthSq() const {
        return dot(*this, *this);
    }
    
    Vector<DIM> normalized() const {
        
        double len = std::sqrt(lengthSq());
        return *this / len;
    }
    /*
    static
    hid_t getTypeID() {
        hid_t typeID = H5Tcreate(H5T_COMPOUND, sizeof(Vector<DIM>));
        hid_t dtype = ::getTypeID<double>();
        if (DIM >= 1)
          H5Tinsert(typeID, "x", offsetof(Vector<DIM>,array[0]), dtype);
        if (DIM >= 2)
          H5Tinsert(typeID, "y", offsetof(Vector<DIM>,array[1]), dtype);
        if (DIM >= 3)
          H5Tinsert(typeID, "z", offsetof(Vector<DIM>,array[2]), dtype);
        if (DIM >= 4)
          H5Tinsert(typeID, "u", offsetof(Vector<DIM>,array[3]), dtype);
        return typeID;
    }
    */  
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

Vector<3> cross(Vector<3> a, Vector<3> b) {
    Vector<3> c;
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
    return c;
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

