/* 
 * File:   Particle.h
 * Author: dducks
 *
 * Created on April 20, 2015, 6:45 PM
 */

#ifndef PARTICLE_H
#define	PARTICLE_H

#include <math/Vector.h>
#include <math/Line.h>

template < unsigned int DIM >
class Particle
{
private:
    int id;
    double localTime;

    Vector<DIM> position;
    Vector<DIM> velocity;
    double radius;
public:

    Particle(int id, Vector<DIM> position, Vector<DIM> velocity, double radius, double time = 0)
    : id(id), position(position), velocity(velocity), radius(radius), localTime(time) { }

    int getID() const {
        return id;
    }
    
    double getTime() const
    {
        return localTime;
    }

    void setTime(double localTime_)
    {
        localTime = localTime_;
    }

    Vector<DIM> getPosition() const
    {
        return position;
    }

    void setPosition(Vector<DIM> newPosition)
    {
        position = newPosition;
    }

    Vector<DIM> getVelocity() const
    {
        return velocity;
    }

    void setVelocity(Vector<DIM> newVelocity)
    {
        velocity = newVelocity;
    }

    double getRadius() const
    {
        return radius;
    }

    void setRadius(double newRadius)
    {
        radius = newRadius;
    }

    void setLocalTime(double time)
    {
        localTime = time;
    }

    double getLocalTime() const
    {
        return localTime;
    }

    Line<DIM> getTrajectory() const {
        return {position, velocity};
    }
    
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Particle<DIM> p) {
    os << "Particle {.id=" << p.getID() << " .pos=" << p.getPosition() 
            << " .vel=" << p.getVelocity()
            << " .t=" << p.getLocalTime() << " .r=" << p.getRadius() << "}";
    
    return os;
}

#endif	/* PARTICLE_H */

