/* 
 * File:   Simulation.h
 * Author: dducks
 *
 * Created on April 20, 2015, 6:44 PM
 */

#ifndef SIMULATION_H
#define	SIMULATION_H

#include <physics/Particle.h>
#include <math/Line.h>
#include <physics/Wall.h>

#include <vector>
#include <functional>
#include <queue>
#include <set>

template < unsigned int DIM >
class Simulation {
private:
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    std::vector< std::function<void(Simulation&)> > funcTriggers;
    
    
    
    double endTime;
    double rescaleTime;
    double globalTime;
    
    double mostRecentEvent;
    
    std::priority_queue<Event*> eventQueue;
    std::set<Event*> unownedEvents;
    Event rescaleEvent;
public:
    
    /**
     * Constructs a Simulation object with a synchronisation / rescale event
     * every rescaleTime seconds, which will end at endTime
     * @param endTime the time at which this simulation will end
     * @param rescaleTime the time at which a rescaleEvent is needed
     */
    Simulation(double rescaleTime, double endTime)
       : endTime(endTime), rescaleTime(rescaleTime), globalTime(0),
         rescaleEvent(rescaleTime, 0, EventType::SYNC_RESCALE) {
           
    }
    
    std::size_t addFunction(std::function<void(Simulation&)>&& func) {
        std::size_t idx = funcTriggers.size();
        funcTriggers.emplace_back(func);
        return idx;
    }
    
    void queueFunction(std::size_t func, double when) {
        Event * evt = new Event(when - globalTime, func, EventType::FUNC_EVALUATION);
        unownedEvents.insert(evt);
        eventQueue.push(evt);
    }
    
    double getLastEvenTime() const {
        return mostRecentEvent;
    }
    
    /**
     * \brief Rescales for more numerical precision
     * 
     * This function rescales the local times of the particles to be
     * on average 0. This improves the numerical precision. Any offset will
     * be added to globalTime.
     */
    void rescale() {
        double avg = 0;
        for (const Particle<DIM>& p : particles) {
            avg += p.getLocalTime();
        }
        avg /= particles.size();
        
        for (Particle<DIM>& p : particles) {
            p.setLocalTime(p.getLocalTime() - avg);
            p.getNextEvent().time -= avg;
        }
        
        rescaleEvent.time -= avg;
        for (Event* evt : unownedEvents) {
            evt->time -= avg;
        }
        
        globalTime += avg;
    }
    
    /**
     * Synchronizes the time so that localtime of particles will be 0.
     * It will be synchronized on the most recent event, which will be the 
     * current event if this is called as a callback/event.
     */
    void synchronise() {
        double dt = mostRecentEvent - globalTime;
        for (Particle<DIM>& p : particles) {
            p.advance(dt - p.getLocalTime());
            p.setLocalTime(0);
            p.getNextEvent().time -= dt;
        }
        
        rescaleEvent.time -= dt;
        for (Event* evt : unownedEvents) {
            evt->time -= dt;
        }
        
        globalTime = mostRecentEvent;
    }
    
    void addParticle(Particle<DIM> p) {
        p.id = particles.size();
        particles.push_back(p);
    }
    void addWall(Wall<DIM> wall) {
        walls.push_back(wall);
    }
    
    const std::vector<Particle<DIM>>& getParticles() const {
        return particles;
    }
    
    void run() {
        initialPopulateEvents();
        
        while (mostRecentEvent < endTime) {
            Event* evt = eventQueue.top(); eventQueue.pop();
            mostRecentEvent = evt->time + globalTime;
            std::cout << mostRecentEvent << " - " << globalTime << " - " << endTime << std::endl;
            switch (evt->type) {
                case EventType::PARTICLE_COLLISION:
                {
                    Particle<DIM>& p2 = particles[evt->otherIdx];
                    Particle<DIM>& p1 = particles[p2.getNextEvent().otherIdx];
                    p1.advance(evt->time);
                    p2.advance(evt->time);
                    
                    Vector<DIM> d = p1.getPosition() - p2.getPosition();
                    d /= sqrt(dot(d,d)); //normalise
                    
                    Vector<DIM> v = p1.getVelocity() - p2.getVelocity();
                    
                    p1.setVelocity(p1.getVelocity() - 2*d*dot(v,d));
                    p2.setVelocity(p2.getVelocity() + 2*d*dot(v,d));
                    
                    p1.setNextEvent({});
                    p2.setNextEvent({});
                    
                    findNextEvent(p1);
                    findNextEvent(p2);
                }
                    break;
                case EventType::WALL_COLLISION:
                {
                    Wall<DIM>& w = walls[evt->otherIdx];
                    //So, we know there's a particle around this event.
                    //Find it.
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    p.advance(evt->time);
                    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(),w.getNormal()) * w.getNormal());
                    p.setNextEvent({});
                    
                    findNextEvent(p);
                }
                    break;
                case EventType::CELL_BOUNDARY:
                    std::cerr << "Unhandled CELL_BOUNDARY" << std::endl;
                    break;
                case EventType::SYNC_RESCALE:
                    rescale();
                    break;
                case EventType::FUNC_EVALUATION:
                    funcTriggers[evt->otherIdx](*this);
                    unownedEvents.erase(evt);
                    delete evt;
                    break;
                case EventType::INVALID:
                    std::cerr << "Invalid events?!?!?!";
                    return;
            }
        }
    }
    
private:
    void findNextEvent(Particle<DIM>& p1) {
        Line<DIM> tr1 = p1.getTrajectory();
        for (std::size_t j = 0; j < walls.size(); j++) {
            double t = intersection(tr1, walls[j], p1.getRadius(), p1.getLocalTime());
            if (t < p1.getNextEvent().time) {
                Event evt;
                evt.type = EventType::WALL_COLLISION;
                evt.time = t;
                evt.otherIdx = j;
                p1.setNextEvent(evt);
            }
        }
        
        Event smallestEvent = p1.getNextEvent();
        for (std::size_t i = 0; i < particles.size(); i++) {
            Particle<DIM>& p2 = particles[i];
            if (p1.getID() == i)
                continue;
            
            double t = intersection(tr1, p2.getTrajectory(),
                                    (p1.getRadius() + p2.getRadius()) / 2.0,
                                    p1.getLocalTime() - p2.getLocalTime());
            if (t < smallestEvent.time && t < p2.getNextEvent().time) {
                smallestEvent.otherIdx = i;
                smallestEvent.time = i;
                smallestEvent.type = EventType::PARTICLE_COLLISION;
            }
        }
        if (smallestEvent.type == EventType::PARTICLE_COLLISION) {
            Particle<DIM>& p2 = particles[smallestEvent.otherIdx];
        }
    }
    void initialPopulateEvents() {
        for (std::size_t i = 0; i < particles.size(); i++) {
            Particle<DIM>& p = particles[i];
            Line<DIM> tr1 = p.getTrajectory();
            for (std::size_t j = 0; j < walls.size(); j++) {
                double t = intersection(tr1, walls[j], p.getRadius(), p.getLocalTime());
                if (t < p.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::WALL_COLLISION;
                    evt.time = t;
                    evt.otherIdx = j;
                    p.setNextEvent(evt);
                }
            }
        }
        for (std::size_t i = 0; i < particles.size(); i++) {
            Particle<DIM>& p1 = particles[i];
            Line<DIM> tr1 = p1.getTrajectory();
            for (std::size_t j = i; j < particles.size(); j++) {
                Particle<DIM>& p2 = particles[j];
                Line<DIM> tr2 = p2.getTrajectory();
                double t = intersection(tr1, tr2, (p1.getRadius() + p2.getRadius()) / 2.0, p1.getLocalTime() - p2.getLocalTime());
                if (t < p1.getNextEvent().time && t < p2.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::PARTICLE_COLLISION;
                    evt.time = t;
                    
                    evt.otherIdx = j;
                    p1.setNextEvent(evt);
                    
                    evt.otherIdx = i;
                    p2.setNextEvent(evt);
                }
            }
        }
        for (Particle<DIM>& p1 : particles) {
            if (p1.getNextEvent().type == EventType::PARTICLE_COLLISION) {
                Particle<DIM>& p2 = particles[p1.getNextEvent().otherIdx];
                if (p2.getNextEvent().type != EventType::PARTICLE_COLLISION ||
                    p2.getNextEvent().otherIdx != p1.getID()) {
                    std::cout << "No longer a pair! (" << p1.getID() << " - " << p2.getID() << "\n\t" << p1 << "\n\t" << p2 << std::endl;
                    p1.setNextEvent({});
                }
            }
        }
        for (Particle<DIM>& p1 : particles) {
            if (p1.getNextEvent().type != EventType::INVALID) {
                if (p1.getNextEvent().type == EventType::PARTICLE_COLLISION &&
                    p1.getNextEvent().otherIdx < p1.getID())
                    continue; //skip double entries
                eventQueue.push(&p1.getNextEvent());
            }
        }
    }
};

#endif	/* SIMULATION_H */

