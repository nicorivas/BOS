/* 
 * File:   Intersection.h
 * Author: dducks
 *
 * Created on April 20, 2015, 7:31 PM
 */

#ifndef INTERSECTION_H
#define	INTERSECTION_H

#include "Line.h"
#include "Plane.h"
#include <limits>


/**
 * Returns the distance traveled on the line, to get to an intersection
 * @param l the line
 * @param plane the plane
 * @param d the radius of the line (so it becomes a cylinder)
 * @return the distance to get to the intersection, viewed from the line
 */
template< unsigned int DIM >
double intersection(Line<DIM> l, Plane<DIM> plane, double d) {
    // first, we use a translation such that the plane is on the origin
    Vector<DIM> lineInPlaneSpace = l.getOrig() - plane.getPoint();
    
    // we project the origin of the line onto the normal of the plane (distance)
    double lineDist =   dot(lineInPlaneSpace, plane.getNormal());
    double directDist = dot(l.getDir()      , plane.getNormal());
    
    //lineDist is negative, so we need to take the distance from the other side
    if (lineDist < 0) {
        lineDist *= -1;
        directDist *= -1;
    }
    
    //In this case they are intersecting in the past, so we'll just report
    //infinite, to make sure that we're not being flagged as an event!
    if (directDist >= 0) {
        return std::numeric_limits<double>::infinity();
    }
    
    double t = -(lineDist - d) / directDist;
    
    //In case we're slightly smaller than 0, it's because we're within the
    //radius of the cylinder. In that case there will still be a collision
    //but we need it ASAP. (and we can't go back in time)
    return t > 0.0 ? t : 0.0;
}

template< unsigned int DIM >
double intersection(Line<DIM> line, Line<DIM> otherLine, double d) {
    //TODO: Can we optimise this? Check the math?
    
    // first, we translate into the frame of the first particle
    Vector<DIM> positionRel = otherLine.getOrig() - line.getOrig();
    Vector<DIM> velocityRel = otherLine.getDir()  - line.getDir();
    
    //q, = quadratic! :D So, we're solving something similar to qa*x^2 + qb*x + qc = 0;
    
    double qa = dot(velocityRel, velocityRel);
    double qb = dot(positionRel, velocityRel);
    
    // we have to be approaching
    if (qb < 0.0) {
        double qc = -4.0 * d * d + dot(positionRel, positionRel);
        double qdet = qb * qb - qa * qc;
        
        //we have a real solution
        if (qdet > 0) {
            return (-qb - sqrt(qdet))/qa; //ABC, tralalala
        }
        
        //Nope!
    }
    //Nope! Infty!
    return std::numeric_limits<double>::infinity();
}



#endif	/* INTERSECTION_H */

