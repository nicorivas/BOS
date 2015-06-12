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

//#include <hdf5/HDF5.h>

template < unsigned int DIM >
class Particle
{
    template<unsigned int D2>
    friend class Simulation;
    friend class Event;
    
private:
    int id;
    double localTime;
    Vector<DIM> position;
    Vector<DIM> velocity;
    double radius;
    int cellIndex;
    Event nextEvent;
    
public:
    
    Particle(Vector<DIM> position, Vector<DIM> velocity, double radius, double time = 0)
    : localTime(time), position(position), velocity(velocity), radius(radius) { }

    int getID() const {
        return id;
    }

    void setLocalTime(double time)
    {
        localTime = time;
    }

    double getLocalTime() const
    {
        return localTime;
    }

    Vector<DIM> getPosition() const
    {
        return position;
    }
    
    Vector<DIM> getCurrentPosition(double dt) const
    {
        return position + velocity * dt;
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

    const Event& getNextEvent() const {
        return nextEvent;
    }
    
    Event& getNextEvent() {
        return nextEvent;
    }
    
    void setNextEvent(Event evt) {
        nextEvent = evt;
    } 
    
    int getCellIndex() const {
        return cellIndex;
    }
    
    void setCellIndex(int ci) {
        cellIndex = ci;
    }

    Line<DIM> getTrajectory() const {
        return {position, velocity};
    }
    
    /**
     * Advances the particle by time dt
     * @param dt time to advance by
     */
    void advance(double dt) {
        position += velocity * dt;
        localTime += dt;
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
    /*
    static hid_t getTypeID() {
        HDF5Type idType  = ::getTypeID<decltype(Particle<DIM>::id)>();
        HDF5Type fType   = ::getTypeID<decltype(Particle<DIM>::radius)>();
        HDF5Type vecType = ::getTypeID<Vector<DIM> >();
        
        hid_t retVal = H5Tcreate(H5T_COMPOUND, sizeof(Particle<DIM>));
        
        H5Tinsert(retVal, "id", offsetof(Particle<DIM>, id), idType);
        H5Tinsert(retVal, "position", offsetof(Particle<DIM>, position), vecType);
        H5Tinsert(retVal, "velocity", offsetof(Particle<DIM>, velocity), vecType);
        H5Tinsert(retVal, "radius", offsetof(Particle<DIM>, radius), fType);        
        
        return retVal;
    }
    */
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Particle<DIM> p) {
    os << "Particle {id=" << p.getID() << " x=" << p.getPosition() 
            << " v=" << p.getVelocity()
            << " t=" << p.getLocalTime() << " r=" << p.getRadius();
    
    if (p.getNextEvent().type != EventType::INVALID) {
        os << " e=" << p.getNextEvent();
    }
    
    os << "}";
    
    return os;
}

/**
* This function is potentially evil. Only call this on events ACTUALLY within a particle.
* @return 
*/
template<unsigned int DIM>
Particle<DIM>* Event::getParticle() {
   return (Particle<DIM>*)(((char*)this) - offsetof(Particle<DIM>,nextEvent));
}

/**
* This function is potentially evil. Only call this on events ACTUALLY within a particle.
* @return 
*/
template<unsigned int DIM>
const Particle<DIM> * Event::getParticle() const {
   return (const Particle<DIM>*)(((char*)this) - offsetof(Particle<DIM>,nextEvent));
}

#endif	/* PARTICLE_H */

