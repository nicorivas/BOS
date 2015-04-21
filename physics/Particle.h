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

#include <physics/Event.h>

#ifndef NO_TOOLS
#include <GL/glew.h>
#endif

template < unsigned int DIM >
class Particle
{
    template<unsigned int D2>
    friend class Simulation;
    
    friend class Event;
//private:
    public:
    int id;
    double localTime;

    Vector<DIM> position;
    Vector<DIM> velocity;
    double radius;
    
    Event nextEvent;
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
    
    /**
     * Advances the particle by time t
     * @param t time to advance by
     */
    void advance(double t) {
        position += velocity * t;
        localTime += t;
    }
    
    const Event& getNextEvent() const {
        return nextEvent;
    }
    
    Event& getNextEvent() {
        return nextEvent;
    }
    
    void setNextEvent(Event evt) {
        nextEvent = evt;
    }
    
#ifndef NO_TOOLS
    static void defineVertexAttributePointers() {
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        
        glVertexAttribDivisor(2, 1); //Tell that these arrays are instanced!
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        
        glVertexAttribPointer(2, 3, GL_DOUBLE, false, sizeof(Particle<DIM>), (void*)offsetof(Particle<DIM>,velocity));
        glVertexAttribPointer(3, 3, GL_DOUBLE, false, sizeof(Particle<DIM>), (void*)offsetof(Particle<DIM>,position));
        glVertexAttribPointer(4, 1, GL_DOUBLE, false, sizeof(Particle<DIM>), (void*)offsetof(Particle<DIM>,radius));
        
    }
#endif
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Particle<DIM> p) {
    os << "Particle {.id=" << p.getID() << " .pos=" << p.getPosition() 
            << " .vel=" << p.getVelocity()
            << " .t=" << p.getLocalTime() << " .r=" << p.getRadius() << "}";
    
    return os;
}

#endif	/* PARTICLE_H */

