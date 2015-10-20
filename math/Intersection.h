#ifndef INTERSECTION_H
#define	INTERSECTION_H

#include "math/Line.h"
#include "math/Plane.h"
#include "math/Parabola.h"
#include <limits>

/**
 * Returns the distance traveled on the line, to get to an intersection
 * @param l the line
 * @param plane the plane
 * @param d the radius of the line (so it becomes a cylinder)
 * @return the distance to get to the intersection, viewed from the line
 */
template< unsigned int DIM >
double intersection(Line<DIM> l, Plane<DIM> plane, double d, double dt) {
    // first, we use a translation such that the plane is on the origin
    Vector<DIM> lineInPlaneSpace = l.getOrig() + l.getDir() * dt - plane.getPoint();
    
    // we project the origin of the line onto the normal of the plane (distance)
    double lineDist =   dot(lineInPlaneSpace, plane.getNormal());
    double directDist = dot(l.getDir()      , plane.getNormal());
    
    //std::cout << lineDist << std::endl;
    //std::cout << directDist << std::endl;
    
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
    
    //std::cout << "Intersection time=" << t << std::endl;
    
    //In case we're slightly smaller than 0, it's because we're within the
    //radius of the cylinder. In that case there will still be a collision
    //but we need it ASAP. (and we can't go back in time)
    if (t > 0.0) {
        return t;
    } else {
        //std::cout << "line =" << l << std::endl;
        //std::cout << "line =" << l.getOrig() + l.getDir() * dt << std::endl;
        //std::cout << "plane=" << plane << std::endl;
        return 0.0;
    }
}

/**
 * Returns the distance traveled on the parabola at the intersection point
 * @param p the parabola
 * @param plane the plane
 * @param d the radius of the parabola (so it becomes kind of a cylinder)
 * @return the distance to get to the intersection, viewed from the parabola
 */
template< unsigned int DIM >
double intersection(Parabola<DIM> p, Plane<DIM> plane, double d, double dt) {
    
    // We need to solve the quadratic equation:
    // With * dot product, parabola a + b * t + c * t^2 = x, plane n * (x - x0) = 0
    // n * (a - x0) - d + n * b t + n * c t^2.
    // qa t^2 + qb t + qc.
    double qct, qa, qb, qc;
    // Projection of the relative distance on the plane normal;
    Vector<DIM> planeNormal = plane.getNormal();
    qct = dot(planeNormal, p.getC()+p.getB()*dt+p.getA()*dt*dt-plane.getPoint());
    // If the projection is negative, we are colliding from the other side, then:
    if (qct < 0) {
        planeNormal = -planeNormal;
        qct = -qct;
    }
    //std::cout << "d=" << d << std::endl;
    qc = qct - d;
    qb = dot(planeNormal, p.getB());
    qa = dot(planeNormal, p.getA());
    //std::cout << "qc=" << qc << " qb=" << qb << " qa=" << qa << std::endl;
    if (qa == 0) {
        // linear equation
        //std::cout << - qb/qc << std::endl;
        return intersection({p.getC(), p.getB()}, plane, d, dt);
    } else {
        double deter = qb * qb - 4 * qa * qc;
        if (deter < 0) return std::numeric_limits<double>::infinity();
        else
        {
            double x1 = (- qb + std::sqrt(deter))/(2*qa);
            double x2 = (- qb - std::sqrt(deter))/(2*qa);
            //std::cout << "x1=" << x1 << " x2=" << x2 << std::endl;
            if (qa > 0)
            {
                if (x1 > 0.0) {
                    return x1;
                } else {
                    return 0.0;
                }
            }else{
                if (x2 > 0.0) {
                    return x2;
                } else {
                    return 0.0;
                }
            }
        }
        /*
        std::cout << "\tdeter=" << deter << std::endl;
        if (qa < 0) {
            if (deter <= 0) return std::numeric_limits<double>::infinity();
            if (qb > 0) {
                std::cout << "\tqb=" << qb << std::endl;
                return std::max(0.0, (-qb-std::sqrt(deter)/qa));
            } else {
                std::cout << "\tqb=" << qb << std::endl;
                return std::max(0.0, 2 * qc/(-qb+std::sqrt(deter)));
            }
        } else {
            if (qb >= 0 || deter <= 0) return std::numeric_limits<double>::infinity();
            return std::max(0.0, 2 * qc/(-qb+std::sqrt(deter)));
        }
        */
    }
}

/**
 * Returns the time at which the first line hits the seconds, while being able
 * to deal with a difference in timeframe. Timeframe is relative from the second
 * line to the first line:
 * line1 (t=0.1): (0,0) - (1,0)
 * line2 (t=1.1): (1,-2)- (0, 1)
 * with d = infinitely small, leads to the call:
 * 
 * 1 = intersection(line1, line2, 0.0000001, -1)
 * @param line The first line (this timeframe)
 * @param otherLine The second line (this timeframe + dt)
 * @param d Distance between them which is allowed
 * @param dt Difference in timebase
 * @return Time relative to the first line
 */
template< unsigned int DIM >
double intersection(Line<DIM> line, Line<DIM> otherLine, double d, double dt) {
    // first, we translate into the frame of the first particle
    Vector<DIM> positionRel = otherLine.getOrig() + otherLine.getDir()*dt - line.getOrig();
    Vector<DIM> velocityRel = otherLine.getDir() - line.getDir();
    
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

