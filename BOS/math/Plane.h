/* 
 * File:   Plane.h
 * Author: dducks
 *
 * Created on April 20, 2015, 7:19 PM
 */

#ifndef PLANE_H
#define	PLANE_H

#include "Vector.h"
#include <iostream>

template< unsigned int DIM >
class Plane {
private:
    Vector<DIM> point, normal;
        
public:
    Plane(Vector<DIM> point, Vector<DIM> normal) : point(point), normal(normal) { }
    Vector<DIM>  getPoint()  const { return point; };
    Vector<DIM>& getPoint()        { return point; };
    Vector<DIM>  getNormal() const { return normal; };
    Vector<DIM>& getNormal()       { return normal; };
};

template<unsigned int DIM>
std::ostream& operator<<(std::ostream& os, const Plane<DIM>& p) {
    os << "Plane {.point=" << p.getPoint() << " .normal=" << p.getNormal() << "}";
    return os;
}

#endif	/* PLANE_H */

