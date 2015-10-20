#ifndef EVENT_H
#define	EVENT_H

#include <limits>
#include <iostream>

enum class EventType {
    PARTICLE_COLLISION,
    WALL_COLLISION,
    CELL_BOUNDARY,
    SYNC,
    FUNCTION,
    INVALID
};

template<unsigned int DIM>
class Particle;

class Event {
public:
    Event() : time(std::numeric_limits<double>::infinity()),
            ownerId(0), otherId(0), type(EventType::INVALID) {}
    Event(double time, std::size_t ownerId, std::size_t otherId, EventType type)
            : time(time), ownerId(ownerId), otherId(otherId), type(type) { }
    
    double time;
    std::size_t ownerId;
    std::size_t otherId;
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
    os << "Event {t=";
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
        case EventType::FUNCTION:
            os << 'F';
            break;
        case EventType::SYNC:
            os << 'S';
            break;
    }
    if (   evt.type != EventType::CELL_BOUNDARY 
        && evt.type != EventType::INVALID
        && evt.type != EventType::SYNC) {
        os << " own=" << evt.ownerId;
        os << " oth=" << evt.otherId;
    }
    os << " t=" << evt.time << "}";
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

