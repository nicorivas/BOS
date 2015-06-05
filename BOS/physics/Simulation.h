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
#include <math/Intersection.h>
#include <physics/Wall.h>
#include <physics/Grid.h>

#include <PriorityQueue.h>

#include <chrono>
#include <vector>
#include <functional>
//#include <queue>
#include <set>
#include <fstream>
#include <string>

#define EVT_PROFILE_COUNT 100000

template < unsigned int DIM >
class Simulation
{
    friend bool testSynchronize();
private:
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    std::vector< std::function<void(Simulation<DIM>&)> > funcTriggers;

    Grid<DIM> grid;

    int eventCount;
    double endTime;
    double syncTime;
    Event syncEvent;
    double currentTime;
    double globalTime;

    PriorityQueue<Event*> eventQueue;
    std::set<Event*> unownedEvents;
public:

    /**
     * Constructs a Simulation object with a synchronisation event
     * every syncTime seconds, which will end at endTime
     * @param endTime the time at which this simulation will end
     * @param syncTime the time at which a syncEvent is needed
     */
    Simulation(double syncTime, double endTime)
    : endTime(endTime), syncTime(syncTime), globalTime(0),
            syncEvent(syncTime, 0, EventType::SYNC)
    {
    
    }
    
    double getCurrentTime() const
    {
        return currentTime;
    }
    
    double getGlobalTime() const
    {
        return globalTime;
    }
    
    double getEndTime() const
    {
        return endTime;
    }
    
    const std::vector<Particle<DIM> >& getParticles() const
    {
        return particles;
    }

    std::size_t addFunction(std::function<void(Simulation&)>&& func)
    {
        std::size_t idx = funcTriggers.size();
        funcTriggers.emplace_back(func);
        return idx;
    }

    void queueFunction(std::size_t func, double when)
    {
        Event * evt = new Event(when - globalTime, func, EventType::FUNC_EVALUATION);
        unownedEvents.insert(evt);
        eventQueue.push(evt);
    }

    /**
     * \brief Rescales for more numerical precision
     * 
     * This function rescales the local times of the particles to be
     * on average 0. This improves the numerical precision. Any offset will
     * be added to globalTime.
     */
    void rescale()
    {
        double avg = 0;
        for (const Particle<DIM>& p : particles)
        {
            avg += p.getLocalTime();
        }
        avg /= particles.size();

        for (Particle<DIM>& p : particles)
        {
            p.setLocalTime(p.getLocalTime() - avg);
            p.getNextEvent().time -= avg;
        }

        syncEvent.time -= avg;
        for (Event* evt : unownedEvents)
        {
            evt->time -= avg;
        }

        globalTime += avg;
    }

    /**
     * \brief Synchronizes the time so that new 0 is timeZero.
     * 
     * Improves numerical precision as we avoid large numbers in the times
     */
    void synchronise(double timeZero)
    {
        for (Particle<DIM>& p : particles) {
            p.advance(timeZero - p.getLocalTime());
            p.setLocalTime(0);
            p.getNextEvent().time -= timeZero;
            if (p.getNextEvent().time < 0) {
                std::cerr << "Negative time shouldn't happen:" << std::endl;
                std::cerr << "p=" << p << std::endl;
                std::cerr << "p.getNextEvent()=" << p.getNextEvent() << std::endl;
                exit(0);
            }
        }

        for (Event* evt : unownedEvents) {
            evt->time -= timeZero;
        }
        
        currentTime = 0.0;
    }

    // Adds a particle (copy) to the simulation container.
    // Notice that the id of the particle is set here, so that the id always
    // corresponds to the position of the particle in the container. This fact
    // is used in several parts in the algorithm.
    void addParticle(Particle<DIM> p)
    {
        p.id = particles.size();
        particles.push_back(p);
    }

    void addWall(Wall<DIM> wall)
    {
        walls.push_back(wall);
    }

    void createGrid(Vector<DIM> domainOrigin, Vector<DIM> domainEnd, Vector<DIM> cellSize)
    {
        grid = Grid<DIM>(domainOrigin, domainEnd, cellSize);
        grid.init(&particles);
    }
    
    void run()
    {
        initialPopulateEvents();
        
        eventCount = 0;
        
        while (globalTime < endTime && eventCount < 300)
        {
            
            Event* evt = eventQueue.top();
            eventQueue.pop();
            
            // Snapshot
            std::ofstream myfile;
            myfile.open ("snapshot_"+std::to_string(eventCount)+".dat");
            for (std::size_t i = 0; i < particles.size(); i++)
            {
                Particle<DIM>& p = particles[i];
                //p.advance(evt->time-p.localTime); //uncomment if you want sync snapshots
                myfile << p.position[0] << " " 
                       << p.position[1] << " "
                       << p.position[2] << std::endl;
            }
            myfile.close();
            
            if (evt->time < currentTime)
            {
                std::cout << "Exiting: event in the past:" << std::endl;
                std::cout << *evt << std::endl;
                exit(0);
            }
            
            switch (evt->type)
            {
                case EventType::PARTICLE_COLLISION:
                {
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::PARTICLE_COLLISION" << std::endl;
                    
                    Particle<DIM>& p2 = particles[evt->otherIdx];
                    Particle<DIM>& p1 = particles[p2.getNextEvent().otherIdx];
                    
                    std::cout << "\t" << p1 << std::endl;
                    std::cout << "\t" << p2 << std::endl;
                    
                    p1.advance(evt->time-p1.localTime);
                    p2.advance(evt->time-p2.localTime);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;

                    std::cout << "\t" << p1 << std::endl;
                    std::cout << "\t" << p2 << std::endl;
                    
                    Vector<DIM> d = p1.getPosition() - p2.getPosition();
                    d /= sqrt(dot(d, d)); //normalise
                    Vector<DIM> v = p1.getVelocity() - p2.getVelocity();
                    p1.setVelocity(p1.getVelocity() - d * dot(v, d));
                    p2.setVelocity(p2.getVelocity() + d * dot(v, d));

                    std::cout << "\t" << p1 << std::endl;
                    std::cout << "\t" << p2 << std::endl;
                    
                    p1.setNextEvent({});
                    p2.setNextEvent({});
                    findNextEvent(p1);
                    findNextEvent(p2);
                    // Wow, that took a time to realise.
                    // -> p1 findNextEvent might in some weird cases find a
                    // combination where p2 actually gets assigned an event already,
                    // because of some three-particle swap. findNextEvent on a
                    // particle WITH an event, is a particularly bad idea :(
                    // Therefore, check.
                    //if (p2.getNextEvent().type == EventType::INVALID)
                    //  findNextEvent(p2);

                }
                    break;
                case EventType::WALL_COLLISION:
                {
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::WALL_COLLISION" << std::endl;
                    
                    Wall<DIM>& w = walls[evt->otherIdx];
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    
                    std::cout << "\t" << p << std::endl;
                    
                    p.advance(evt->time-p.localTime);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;
                    
                    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
                    
                    std::cout << "\t" << p << std::endl;
                    
                    p.setNextEvent({});
                    findNextEvent(p);
                }
                    break;
                case EventType::CELL_BOUNDARY:
                {
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::CELL_BOUNDARY" << std::endl;
                    
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    
                    std::cout << p << std::endl;
                    //std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    // I don't think this is necessary, we should try when
                    // the code is stable. As velocity doesn't change,
                    // collisions will anyway be correctly predicted.
                    p.advance(evt->time-p.localTime);
                    
                    grid.moveParticle(evt);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;
                    
                    std::cout << p << std::endl;
                    //std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    p.setNextEvent({});
                    findNextEvent(p);
                }
                    break;
                case EventType::RESCALE:
                {
                    /*
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::RESCALE" << std::endl;
                    
                    rescale();

                    rescaleEvent.time += rescaleTime;
                    eventQueue.push(&rescaleEvent);
                    */
                }
                    break;
                case EventType::SYNC:
                {
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::SYNC" << std::endl;
                    
                    synchronise(evt->time);
                    
                    syncEvent.time += syncTime;
                    eventQueue.push(&syncEvent);
                }   
                    break;
                case EventType::FUNC_EVALUATION:
                {
                    std::cout << "\n(" << eventCount << "," << evt->time << "," << globalTime << ") ";
                    std::cout << "EventType::FUNC_EVALUATION" << std::endl;
                    
                    funcTriggers[evt->otherIdx](*this);
                    unownedEvents.erase(evt);
                    delete evt;
                }
                    break;
                case EventType::INVALID:
                {
                    std::cerr << globalTime << std::endl;
                    std::cerr << "Invalid events?!?!?!" << std::endl;
                }
                    return;
            }
            eventCount++;
        }
    }

private:

    Event& findWallCollisions(Particle<DIM>& p) {
        std::cout << "findWallCollisions(Particle<DIM>& p)" << std::endl;
        Line<DIM> tr = p.getTrajectory();
        for (std::size_t j = 0; j < walls.size(); j++) {
            // dt is the time of the next collision from now (currentTime)
            double dt = intersection(tr, walls[j], p.getRadius(),
                            p.getLocalTime()-currentTime);
            std::cout << "\twalls[j]=" << walls[j];
            std::cout << " * dt=" << dt << std::endl;
            if (currentTime+dt < p.getNextEvent().time) {
                Event evt;
                evt.type = EventType::WALL_COLLISION;
                evt.time = currentTime+dt;
                evt.otherIdx = j;
                p.setNextEvent(evt);
                std::cout << "\t * " << evt << std::endl;
            }
        }
        return p.getNextEvent();
    }
    
    Event& findCellCollisions(Particle<DIM>& p) {
        std::cout << "findCellCollisions(Particle<DIM>& p)" << std::endl;
        // Cell collisions: find the corresponding planes of the cells to check
        
        Line<DIM> tr = p.getTrajectory();
        std::cout << "\tparticle " << p.getID() << " with cells " << std::endl;          
        std::array<int, DIM> ic = grid.getIndexCoordsFromIndex(p.cellIndex);
        std::vector<int> indexes;
        indexes.push_back(ic[0]*2);
        indexes.push_back(ic[0]*2+1);
        indexes.push_back(grid.getNCells(0)*2+ic[1]*2);
        indexes.push_back(grid.getNCells(0)*2+ic[1]*2+1);
        indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2);
        indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2+1);
        double dt;
        for (int i : indexes) {
            std::cout << "\tplane=" << *(grid.getPlane(i)) << std::endl;
            dt = intersection(tr, *(grid.getPlane(i)), 0.0, p.getLocalTime()-currentTime);
            // We ask 't > 0' because after moving a particle to a cell boundary
            // the next collision would be predicted for ever.
            if (dt > 1.0E-11) {
                if (currentTime+dt < p.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::CELL_BOUNDARY;
                    evt.time = currentTime+dt;
                    evt.otherIdx = i;
                    p.setNextEvent(evt);
                    std::cout << "\t" << evt << std::endl;
                }
            }
        }
    }
    
    Event& findParticleCollisions(Particle<DIM>& p1) { 
        std::cout << "findParticleCollisions(Particle<DIM>& p1)" << std::endl;
        
        // TODO: not working for zero particles, creates a bug with the times
        Line<DIM> tr1 = p1.getTrajectory();
        // This event will actually be copied when added to the particle,
        // so it dies with this function.
        Event evt;
        bool found = false;
        
        std::vector<int> neighbors = grid.getNeighbors(&p1);
        std::cout << "neighbors.size()=" << neighbors.size() << std::endl;
        
        for (std::size_t i = 0; i < neighbors.size(); i++)
        {
            Particle<DIM>& p2 = particles[neighbors[i]];
            
            std::cout << "neighbors[i="<<i<<"]=" << neighbors[i] << std::endl;
            std::cout << "p2=" << p2 << std::endl;

            double dt = intersection(tr1, p2.getTrajectory(),
                                    (p1.getRadius() + p2.getRadius()) / 2.0,
                                    p1.getLocalTime() - p2.getLocalTime());
            if (currentTime + dt < p1.getNextEvent().time &&
                currentTime + dt < p2.getNextEvent().time)
            {
                evt.type = EventType::PARTICLE_COLLISION;
                evt.time = currentTime+dt;
                evt.otherIdx = neighbors[i];
                std::cout << evt << std::endl;
                found = true;
            }
        }


        if (found)
        {
            Particle<DIM>& p2 = particles[evt.otherIdx];
            
            // See if the other particle already has a collision scheduled,
            // then we need to cancel that one
            if (p2.getNextEvent().type == EventType::PARTICLE_COLLISION)
            {
                Particle<DIM>& cancelledPartner = particles[p2.getNextEvent().otherIdx];

                //Which event was on the queue? (answer: smallest ID)
                Event* pEvent = (p2.getID() > cancelledPartner.getID())
                        ? &p2.getNextEvent() : &cancelledPartner.getNextEvent();

                //BOOM! GONE!
                eventQueue.erase(pEvent);
                
                p1.setNextEvent(evt);
                evt.otherIdx = p1.getID();
                p2.setNextEvent(evt);

                cancelledPartner.setNextEvent({});

                findNextEvent(cancelledPartner);
            }
            else
            {
                p1.setNextEvent(evt);
                eventQueue.erase(&p2.getNextEvent());
                evt.otherIdx = p1.getID();
                p2.setNextEvent(evt);
            }

            if (p1.getID() < p2.getID())
            {
                return p1.getNextEvent();
            }
            else
            {
                return p2.getNextEvent();
            }
        }
        return p1.getNextEvent();
    }

    void findNextEvent(Particle<DIM>& p) {
        std::cout << "void findNextEvent(Particle<DIM>&)" << std::endl;
        // See comment on initialPopulateEvents().
        Event * evt;
        evt = &findWallCollisions(p);
        evt = &findCellCollisions(p);
        evt = &findParticleCollisions(p);
        eventQueue.push(evt);
    }
    
    void initialPopulateEvents() {
        // Pointer here, as the actual event is contained by the particles,
        // and the queue only contains the pointers.
        Event * evt;
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p = particles[i];
            evt = &findWallCollisions(p);
            evt = &findCellCollisions(p);
            evt = &findParticleCollisions(p);
            eventQueue.push(evt);
        }
        // Other events
        eventQueue.push(&syncEvent);
    }
};

#endif	/* SIMULATION_H */

