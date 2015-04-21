/* 
 * File:   Line.h
 * Author: dducks
 *
 * Created on April 20, 2015, 7:26 PM
 */

#ifndef LINE_H
#define	LINE_H

#include "Vector.h"
#include <iostream>

template< unsigned int DIM >
class Line {
private:
    Vector<DIM> a, b;
    
public:
    Line(Vector<DIM> a, Vector<DIM> b) : a(a), b(b) { }
    
    Vector<DIM>  getOrig() const { return a; }
    Vector<DIM>& getOrig()       { return a; }
    Vector<DIM>  getDir() const { return b; }
    Vector<DIM>& getDir()       { return b; }
     
    Vector<DIM> operator() (double d) const { 
        return a + d * b;
    }
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Line<DIM> l) {
    os << "Line { y(x) = " << l.getOrig() << " + " << l.getDir() << "*x }";
    return os;
}
#endif	/* LINE_H */

