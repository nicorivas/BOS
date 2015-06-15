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
#include <input/Data.h>

#include <PriorityQueue.h>

#include <chrono>
#include <vector>
#include <functional>
//#include <queue>
#include <set>
//#include <fstream>
//#include <sstream>
#include <string>
#include <cstdio>


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

    int eventNumber;
    int eventNumberLimit;
    double endTime;
    Event syncEvent;
    double currentTime;
    double globalTime;
    
    Data<3> data;

    PriorityQueue<Event*> eventQueue;
    std::set<Event*> unownedEvents;
public:

    /**
     * Constructs a Simulation object, data read from "file".
     */
    Simulation(const std::string& file) : data(file, *this)
    {
        syncEvent= Event(data.syncTime, 0, 0, EventType::SYNC);
        endTime = data.endTime;
        globalTime = 0;
        createGrid({0.0,0.0,0.0},
        {data.domain[0],data.domain[1],data.domain[2]},
        {data.particleRadius*2.0,data.particleRadius*2.0,data.particleRadius*2.0});
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
        //std::cout << "Particle added=" << particles[particles.size()-1] << std::endl;
    }

    void addWall(Wall<DIM> wall)
    {
        std::cout << wall << std::endl;
        walls.push_back(wall);
    }

    void createGrid(Vector<DIM> domainOrigin, Vector<DIM> domainEnd, Vector<DIM> cellSize)
    {
        std::cout << domainOrigin << std::endl;
        std::cout << domainEnd << std::endl;
        std::cout << cellSize << std::endl;
        grid = Grid<DIM>(domainOrigin, domainEnd, cellSize);
        grid.init(&particles);
    }
    
    void printParticles()
    {
        std::cout << "\nParticles state:" << std::endl;
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            std::cout << particles[i] << std::endl;
        }
    }
    
    void printQueue()
    {
        std::cout << "\nQueue:" << std::endl;
        std::vector<Event*> el = eventQueue.getlist();
        for (auto en : el)
        {
            std::cout << en << " " << (*en) << std::endl;
        }
    }
    
    void snapshot()
    {
        std::ofstream myfile;
        myfile.open ("snapshot_"+std::to_string(eventNumber)+".dat");
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p = particles[i];
            Vector<DIM> pos = p.getPosition();
            //p.advance(currentTime-p.localTime); //uncomment if you want sync snapshots
            myfile << pos[0] << " " 
                   << pos[1] << " "
                   << pos[2] << " ";
            pos = p.getCurrentPosition(currentTime-p.localTime);
            myfile << pos[0] << " " 
                   << pos[1] << " "
                   << pos[2] << std::endl;
        }
        myfile.close();
    }
    
    int run()
    {
        initialPopulateEvents();
        
        eventNumber = 0;
        
        while (globalTime < endTime && eventNumber < eventNumberLimit)
        {          
            Event* evt = eventQueue.top();
            eventQueue.pop();
            
            //printParticles();
            //printQueue();   
            snapshot();
            
            if (evt->time < currentTime)
            {
                std::cout << "Exiting: event in the past:" << std::endl;
                std::cout << "currentTime=" << currentTime;
                std::cout << " globalTime=" << globalTime << std::endl;
                std::cout << *evt << std::endl;
                return 1;
            }
            
            //std::cout << "\n*** (" << eventCount << ") " << *evt << std::endl;
            
            switch (evt->type)
            {
                case EventType::PARTICLE_COLLISION:
                {   
                    Particle<DIM>& pB = particles[evt->otherId];
                    Particle<DIM>& pA = particles[evt->ownerId];

                    //std::cout << "\t" << pA << std::endl;
                    //std::cout << "\t" << pB << std::endl;
                    
                    pA.advance(evt->time-pA.localTime);
                    pB.advance(evt->time-pB.localTime);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;

                    //std::cout << "\t" << pA << std::endl;
                    //std::cout << "\t" << pB << std::endl;
                    
                    Vector<DIM> d = pA.getPosition() - pB.getPosition();
                    d /= sqrt(dot(d, d)); //normalise
                    Vector<DIM> v = pA.getVelocity() - pB.getVelocity();
                    pA.setVelocity(pA.getVelocity() - d * dot(v, d));
                    pB.setVelocity(pB.getVelocity() + d * dot(v, d));

                    //std::cout << "\t" << pA << std::endl;
                    //std::cout << "\t" << pB << std::endl;
                    
                    eventQueue.erase(&(pB.getNextEvent()));
                    pA.setNextEvent({});
                    pB.setNextEvent({});
                    
                    //printQueue();
                    
                    findNextEvent(pA);
                    findNextEvent(pB);
                }
                    break;
                case EventType::WALL_COLLISION:
                {
                    Wall<DIM>& w = walls[evt->otherId];
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    
                    //std::cout << "\t" << p << std::endl;
                    
                    p.advance(evt->time-p.localTime);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;
                    
                    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
                    
                    //std::cout << "\t" << p << std::endl;
                    
                    p.setNextEvent({});
                    findNextEvent(p);
                }
                    break;
                case EventType::CELL_BOUNDARY:
                {
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    
                    //std::cout << p << std::endl;
                    //std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    // I don't think this is necessary, we should try when
                    // the code is stable. As velocity doesn't change,
                    // collisions will anyway be correctly predicted.
                    double etime = evt->time;
                    p.advance(etime-p.localTime);
                    
                    grid.moveParticle(evt);
                    globalTime += evt->time-currentTime;
                    currentTime = evt->time;
                    
                    //std::cout << p << std::endl;
                    //std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    p.setNextEvent({});
                    findNextEvent(p);
                }
                    break;
                case EventType::RESCALE:
                {
                    //
                }
                    break;
                case EventType::SYNC:
                {
                    synchronise(evt->time);
                    
                    syncEvent.time += data.syncTime;
                    eventQueue.push(&syncEvent);
                }   
                    break;
                case EventType::FUNC_EVALUATION:
                {
                    funcTriggers[evt->otherId](*this);
                    unownedEvents.erase(evt);
                    delete evt;
                }
                    break;
                case EventType::INVALID:
                {
                    std::cerr << globalTime << std::endl;
                    std::cerr << "Invalid events?!?!?!" << std::endl;
                }
                    return 1;
            }
            eventNumber++;
        }
        return 0;
    }

private:

    Event& findWallCollisions(Particle<DIM>& p) {
        //std::cout << "findWallCollisions(Particle<DIM>& p="<<p.getID()<<")" << std::endl;
        Line<DIM> tr = p.getTrajectory();
        for (std::size_t j = 0; j < walls.size(); j++) {
            // dt is the time of the next collision from now (currentTime)
            double dt = intersection(tr, walls[j], p.getRadius(),
                            currentTime-p.getLocalTime());
            //std::cout << "\twalls[j]=" << walls[j] << " -> dt=" << dt << std::endl;
            if (currentTime+dt < p.getNextEvent().time) {
                Event evt;
                evt.type = EventType::WALL_COLLISION;
                evt.time = currentTime+dt;
                evt.ownerId = p.getID();
                evt.otherId = j;
                p.setNextEvent(evt);
                //std::cout << "\t* PREDICTED * " << evt << std::endl;
            }
        }
        return p.getNextEvent();
    }
    
    // Finds collisions with the corresponding planes in the grid.
    Event& findCellCollisions(Particle<DIM>& p) {
        //std::cout << "findCellCollisions(Particle<DIM>& p="<<p.getID()<<")" << std::endl;
        
        Line<DIM> tr = p.getTrajectory();
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
            dt = intersection(tr, *(grid.getPlane(i)), 0.0, currentTime-p.getLocalTime());
            // We ask 't > 0' because after moving a particle to a cell boundary
            // the next collision would be predicted for ever. This is fragile;
            // there must be a better way to do it.
            if (dt > 1.0E-13) {
                if (currentTime+dt < p.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::CELL_BOUNDARY;
                    evt.time = currentTime+dt;
                    evt.ownerId = p.getID();
                    evt.otherId = i;
                    p.setNextEvent(evt);
                    //std::cout << "\t* PREDICTED * " << evt << std::endl;
                }
            }
        }
        return p.getNextEvent();
    }
    
    Event& findParticleCollisions(Particle<DIM>& p1) { 
        //std::cout << "findParticleCollisions(Particle<DIM>& p1)" << std::endl;
        Event evt; // This event dies, as it is copied when added to particles
        Line<DIM> tr1 = p1.getTrajectory();
        
        std::vector<int> neighbors = grid.getNeighbors(&p1);
        for (std::size_t i = 0; i < neighbors.size(); i++)
        {
            Particle<DIM>& p2 = particles[neighbors[i]];
            
            if (p2.getID() == p1.getID())
                continue;
            
            //std::cout << "\tneighbors[i="<<i<<"]=" << neighbors[i] << std::endl;
            //std::cout << "\tp2=" << p2 << std::endl;

            double dt = intersection(tr1, p2.getTrajectory(),
                                    (p1.getRadius() + p2.getRadius()) / 2.0,
                                     p1.getLocalTime() - p2.getLocalTime());
            
            //std::cout << "\tcurrentTime + dt="<<(currentTime+dt)<<" evt.time="<<evt.time<<" p1.getNextEvent().time="<<(p1.getNextEvent().time)<<std::endl;
            if (currentTime + dt < p1.getNextEvent().time &&
                currentTime + dt < p2.getNextEvent().time)
            {
                evt.type = EventType::PARTICLE_COLLISION;
                evt.time = currentTime+dt;
                evt.ownerId = p1.getID();
                evt.otherId = p2.getID();
                p1.setNextEvent(evt);
                //std::cout << "\t* PREDICTED * " << evt << std::endl;
            }
        }
        /*
        if (found)
        {
            Particle<DIM>& p2 = particles[evt.otherId];
            
            //std::cout << "\t p2="<<p2<<std::endl;
            
            // See if the other particle already has a collision scheduled,
            // then we need to cancel that one
            if (p2.getNextEvent().type == EventType::PARTICLE_COLLISION)
            {
                std::cout << "\tCONFLICT OF PARTICLE COLLISION EVENTS" << std::endl;
                std::cout << "\tevt=" << evt << std::endl;
                std::cout << "\tp2.getNextEvent()=" << p2.getNextEvent() << std::endl;
                if (p1.getID() == p2.getNextEvent().otherId &&
                        fabs(evt.time - p2.getNextEvent().time) < std::numeric_limits<double>::epsilon()*10 &&
                        evt.type == p2.getNextEvent().type)
                {
                    std::cout << "\tEvent was actually the same one" << std::endl;
                    // It's the same event, leave the one with the lower ID.
                    if (evt.otherId > p2.id)
                    {
                        Event * tmp = &p2.getNextEvent();
                        eventQueue.erase(tmp);
                        p2.setNextEvent({});
                        p1.setNextEvent(evt);
                        return p1.getNextEvent();
                    }
                    else
                    {
                        Event * tmp = &p2.getNextEvent();
                        eventQueue.erase(tmp);
                        return p2.getNextEvent();
                    }
                }
                else
                {
                    std::cout << "Crazy shit" << std::endl;
                    Particle<DIM>& cancelledPartner = particles[p2.getNextEvent().otherId];

                    //Which event was on the queue? (answer: smallest ID)
                    Event* pEvent = (p2.getID() < cancelledPartner.getID())
                            ? &p2.getNextEvent() : &cancelledPartner.getNextEvent();

                    //BOOM! GONE!
                    eventQueue.erase(pEvent);
                
                    p1.setNextEvent(evt);
                    evt.ownerId = p2.getID();
                    evt.otherId = p1.getID();
                    p2.setNextEvent(evt);

                    cancelledPartner.setNextEvent({});

                    findNextEvent(cancelledPartner);
                }
            }
            else
            {
                p1.setNextEvent(evt);
                if (p2.getNextEvent().type != EventType::INVALID)
                    eventQueue.erase(&p2.getNextEvent());
                evt.ownerId = p2.getID();
                evt.otherId = p1.getID();
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
        */
        return p1.getNextEvent();
    }

    void findNextEvent(Particle<DIM>& p) {
        //std::cout << "void findNextEvent(p.getID()="<<p.getID()<<")" << std::endl;
        // See comment on initialPopulateEvents().
        Event * evt;
        evt = &findWallCollisions(p);
        evt = &findCellCollisions(p);
        evt = &findParticleCollisions(p);
        //std::cout << "eventQueue.push(evt=" << *evt << ")" << std::endl;
        eventQueue.push(evt);
    }
    
    void initialPopulateEvents() {
        // Pointer here, as the actual event is contained by the particles,
        // and the queue only contains the pointers.
        //Event * evt;
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p = particles[i];
            findNextEvent(p);
            /*
            evt = &findWallCollisions(p);
            evt = &findCellCollisions(p);
            evt = &findParticleCollisions(p);
            eventQueue.push(evt);
            */
        }
        // Other events
        eventQueue.push(&syncEvent);
    }
};

#endif	/* SIMULATION_H */

