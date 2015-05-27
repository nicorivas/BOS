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
    RESCALE,
    SYNC,
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
    Particle<DIM>* getParticle();
    
    /**
     * This function is potentially evil. Only call this on events ACTUALLY within a particle.
     * @return 
     */
    template<unsigned int DIM>
    const Particle<DIM> * getParticle() const;
};

std::ostream& operator<<(std::ostream& os, const Event& evt) {
    os << "Event {.type=";
    switch(evt.type) {
        case EventType::PARTICLE_COLLISION:
            os << 'P';
            break;
        case EventType::WALL_COLLISION:
            os << 'W';
            break;
        case EventType::CELL_BOUNDARY:
            os << 'C';
            break;
        case EventType::INVALID:
            os << 'I';
            break;
        case EventType::FUNC_EVALUATION:
            os << 'F';
            break;
        case EventType::RESCALE:
            os << 'R';
            break;
        case EventType::SYNC:
            os << 'S';
            break;
    }
    if (   evt.type != EventType::CELL_BOUNDARY 
        && evt.type != EventType::INVALID
        && evt.type != EventType::SYNC
        && evt.type != EventType::RESCALE) {
        os << " .id=" << evt.otherIdx;
    }
    
    os << " .t=" << evt.time << "}";
    return os;
}

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

