/* 
 * File:   Event.h
 * Author: dducks
 *
 * Created on April 21, 2015, 1:57 PM
 */

#ifndef EVENT_H
#define	EVENT_H

enum class EventType {
    PARTICLE_COLLISION,
    WALL_COLLISION,
    CELL_BOUNDARY,
    SYNC_RESCALE,
    FUNC_EVALUATION
};

class Event {
public:
    double time;
    std::size_t otherIdx;
    EventType type;
};

#endif	/* EVENT_H */

