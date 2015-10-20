#ifndef PARABOLA_H
#define	PARABOLA_H

#include "math/Vector.h"
#include <iostream>

template< unsigned int DIM >
class Parabola {
private:
    Vector<DIM> a, b, c;
public:
    Parabola(Vector<DIM> a, Vector<DIM> b, Vector<DIM> c) : a(a), b(b), c(c) { }
    Vector<DIM>  getOrig() const { return c; }
    Vector<DIM>& getOrig()       { return c; }
    Vector<DIM>  getA() const { return a; }
    Vector<DIM>& getA()       { return a; }
    Vector<DIM>  getB() const { return b; }
    Vector<DIM>& getB()       { return b; }
    Vector<DIM>  getC() const { return c; }
    Vector<DIM>& getC()       { return c; }
    Vector<DIM> operator() (double x) const { 
        return a * x * x + b * x + c;
    }
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Parabola<DIM> p) {
    os << "Parabola { y(x) = " << p.getC() << " + " << p.getB() << "*x " << p.getA() << "*x*x }";
    return os;
}

#endif	/* PARABOLA_H */

