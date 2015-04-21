/* 
 * File:   Event.h
 * Author: dducks
 *
 * Created on April 21, 2015, 1:57 PM
 */

#ifndef EVENT_H
#define	EVENT_H

#include <limits>
#include <iostream>

enum class EventType {
    PARTICLE_COLLISION,
    WALL_COLLISION,
    CELL_BOUNDARY,
    SYNC_RESCALE,
    FUNC_EVALUATION,
    INVALID
};

template<unsigned int DIM>
class Particle;

class Event {
public:
    Event() : time(std::numeric_limits<double>::infinity()),
            otherIdx(0), type(EventType::INVALID) {}
    Event(double time, std::size_t otherIdx, EventType type)
            : time(time), otherIdx(otherIdx), type(type) { }
    
    double time;
    std::size_t otherIdx;
    EventType type;
    
    /**
     * This function is potentially evil. Only call this on events ACTUALLY within a particle.
     * @return 
     */
    template<unsigned int DIM>
    Particle<DIM>* getParticle() {
        return (Particle<DIM>*)(((char*)this) - offsetof(Particle<DIM>,nextEvent));
    }
    
    /**
     * This function is potentially evil. Only call this on events ACTUALLY within a particle.
     * @return 
     */
    template<unsigned int DIM>
    const Particle<DIM> * getParticle() const {
        return (const Particle<DIM>*)(((char*)this) - offsetof(Particle<DIM>,nextEvent));
    }
};

namespace std {
    template<>
    struct less<Event*> {
        bool operator()(const Event* a, const Event* b) {
            return a->time > b->time;
        }
    };
    
    template<>
    struct less<Event> {
        bool operator()(const Event& a, const Event& b) {
            return a.time > b.time;
        }
    };
}

#endif	/* EVENT_H */

