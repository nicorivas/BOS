#ifndef PARTICLE_H
#define	PARTICLE_H

#include "math/Vector.h"
#include "math/Line.h"
#include "math/Parabola.h"

#include "Event.h"

template < unsigned int DIM >
class Particle
{
    template<unsigned int D2>
    friend class Simulation;
    friend class Event;
    
private:
    int id;
    int type;
    double localTime;
    Vector<DIM> position;
    Vector<DIM> velocity;
    Vector<DIM> acceleration;
    double radius;
    int cellIndex;
    int cellWallIndex;
    Event nextEvent;
    
public:
    
    Particle(Vector<DIM> position, Vector<DIM> velocity, Vector<DIM> acceleration,
            double radius, int type, double time = 0) : type(type), localTime(time), position(position),
                    velocity(velocity), acceleration(acceleration), radius(radius)
            { }

    int getID() const
    {
        return id;
    }

    double getLocalTime() const
    {
        return localTime;
    }
    
    void setLocalTime(double time)
    {
        localTime = time;
    }
    
    void addLocalTime(double dt)
    {
        localTime += time;
    }

    Vector<DIM> getPosition() const
    {
        return position;
    }
    
    Vector<DIM> getCurrentPosition(double dt) const
    {
        return position + velocity * dt + (acceleration/2.0)*dt*dt;
    }

    void setPosition(Vector<DIM> newPosition)
    {
        position = newPosition;
    }

    Vector<DIM> getVelocity() const
    {
        return velocity;
    }
    
    Vector<DIM> getCurrentVelocity(double dt) const
    {
        return velocity + acceleration * dt;
    }

    void setVelocity(Vector<DIM> newVelocity)
    {
        velocity = newVelocity;
    }

    Vector<DIM> getAcceleration() const
    {
        return acceleration;
    }
    
    void setAcceleration(Vector<DIM> newAcceleration)
    {
        acceleration = newAcceleration;
    }
    
    double getRadius() const
    {
        return radius;
    }
    
    int getType() const
    {
        return type;
    }

    void setType(int newType)
    {
        type = newType;
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
    
    void setCellWallIndex(int ci) {
        cellWallIndex = ci;
    }

    Line<DIM> getTrajectory() const {
        return {position, velocity};
    }
    
    Parabola<DIM> getTrajectoryGravity() const {
        return {acceleration/2.0, velocity, position};
    }
    
    /**
     * Advances the particle by time dt
     * @param dt time to advance by
     */
    void advance(double dt) {
        position += velocity * dt + (acceleration/2.0) * dt * dt;
        velocity += acceleration * dt;
        localTime += dt;
    }
};

template< unsigned int DIM >
std::ostream& operator<<(std::ostream& os, Particle<DIM> p) {
    os << "Particle {id=" << p.getID() << " x=" << p.getPosition() 
            << " v=" << p.getVelocity()
            << " t=" << p.getLocalTime() << " ty=" << p.getType();
    
    if (p.getNextEvent().type != EventType::INVALID) {
        os << " e=" << p.getNextEvent();
    }
    
    os << "}";
    
    return os;
}

#endif	/* PARTICLE_H */