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

#define EVT_PROFILE_COUNT 100000

template < unsigned int DIM >
class Simulation
{
    friend bool testSynchronize();
private:
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    std::vector< std::function<void(Simulation<DIM>&)> > funcTriggers;

    Vector<DIM> domainOrigin;
    Vector<DIM> domainEnd;

    Grid<DIM> grid;

    double endTime;
    double rescaleTime;
    double globalTime;

    double mostRecentEvent;

    PriorityQueue<Event*> eventQueue;
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
            mostRecentEvent(0), rescaleEvent(rescaleTime, 0, EventType::SYNC_RESCALE)
    {
    
    }

    std::size_t addFunction(std::function<void(Simulation&)>&& func)
    {
        std::size_t idx = funcTriggers.size();
        funcTriggers.emplace_back(func);
        return idx;
    }

    /*
    void init() {
        // Create grid
        Grid<2> grid({2*radii, 2*radii});
        
        // Determine particle's cell index
        for (const Particle* particle : particles)
        {
            Vector<DIM> pos = particle->getPosition();
            Vector<DIM> cellSize = grid->getCellSize();
            for (unsigned int i = 0; i < DIM; i++)
            {
                particle.cellIndex[i] = (int)(pos[i]/cellSize[i]);
            }
        }
    }
     */

    void queueFunction(std::size_t func, double when)
    {
        Event * evt = new Event(when - globalTime, func, EventType::FUNC_EVALUATION);
        unownedEvents.insert(evt);
        eventQueue.push(evt);
    }

    double getLastEvenTime() const
    {
        return mostRecentEvent;
    }
    
    double getGlobalTime() const
    {
        return globalTime;
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

        rescaleEvent.time -= avg;
        for (Event* evt : unownedEvents)
        {
            evt->time -= avg;
        }

        globalTime += avg;
    }

    /**
     * Synchronizes the time so that localtime of particles will be 0.
     * It will be synchronized on the most recent event, which will be the 
     * current event if this is called as a callback/event.
     */
    void synchronise()
    {
        double dt = mostRecentEvent - globalTime;
        for (Particle<DIM>& p : particles)
        {
            p.advance(dt - p.getLocalTime());
            p.setLocalTime(0);
            p.getNextEvent().time -= dt;
        }

        rescaleEvent.time -= dt;
        for (Event* evt : unownedEvents)
        {
            evt->time -= dt;
        }

        globalTime = mostRecentEvent;
    }

    void addParticle(Particle<DIM> p)
    {
        p.id = particles.size();
        particles.push_back(p);
    }

    void addWall(Wall<DIM> wall)
    {
        walls.push_back(wall);
    }

    const std::vector<Particle<DIM> >& getParticles() const
    {
        return particles;
    }

    void run()
    {
        grid = Grid<DIM>({1.0,1.0,1.0});
        grid.init(&domainOrigin, &domainEnd, &particles);
        
        initialPopulateEvents();
        
        
        //initialPopulateEvents();
        //for (Particle<DIM>& p : particles)
        //    findNextEvent(p);

        unsigned long evtCount = 0;
        auto t1 = std::chrono::high_resolution_clock::now();

        while (mostRecentEvent < endTime)
        {
            if (evtCount == EVT_PROFILE_COUNT)
            {
                auto t2 = std::chrono::high_resolution_clock::now();
                auto tDiff = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                std::cout << "EvtTimer: " << tDiff.count()
                        << "\nEvt/ms: " << ((double) (evtCount)) / tDiff.count()
                        << std::endl;
                t1 = t2;
                evtCount = 0;
            }
            evtCount++;
            /*

            std::cout << "\n\n\n\nNEW EVENT";
            std::cout << &eventQueue << std::endl;
            std::cout << "\n---------- Event List ----------\n";
            eventQueue.eval([](Event* evt) { std::cout << evt << ": " << *evt << '\n'; });
            std::cout << "-------[count=" << eventQueue.size() << "]" << std::endl;

            */
            Event* evt = eventQueue.top();
            eventQueue.pop();
            /*
            std::cout << &eventQueue << std::endl;
            
            std::cout << "\n--[APOP]-- Event List --[APOP]--\n";
            eventQueue.eval([](Event* evt) { std::cout << evt << ": " << *evt << '\n'; });
            std::cout << "-------[count=" << eventQueue.size() << "]" << std::endl;
             */
            mostRecentEvent = evt->time + globalTime;
            //std::cout << "Processing: " << evt << ": " << *evt << std::endl;
            //std::cout << '.'; std::cout.flush();
            //std::cout << *evt << std::endl;
            switch (evt->type)
            {
                case EventType::PARTICLE_COLLISION:
                {
                    Particle<DIM>& p2 = particles[evt->otherIdx];
                    Particle<DIM>& p1 = particles[p2.getNextEvent().otherIdx];
                    p1.advance(evt->time);
                    p2.advance(evt->time);

                    Vector<DIM> d = p1.getPosition() - p2.getPosition();
                    d /= sqrt(dot(d, d)); //normalise

                    Vector<DIM> v = p1.getVelocity() - p2.getVelocity();

                    p1.setVelocity(p1.getVelocity() - d * dot(v, d));
                    p2.setVelocity(p2.getVelocity() + d * dot(v, d));

                    p1.setNextEvent({});
                    p2.setNextEvent({});

                    findNextEvent(p1);
                    // Wow, that took a time to realise.
                    // -> p1 findNextEvent might in some weird cases find a
                    // combination where p2 actually gets assigned an event already,
                    // because of some three-particle swap. findNextEvent on a
                    // particle WITH an event, is a particularly bad idea :(
                    // Therefore, check.
                    if (p2.getNextEvent().type == EventType::INVALID)
                      findNextEvent(p2);

                }
                    break;
                case EventType::WALL_COLLISION:
                {
                    std::cout << "\nEventType::WALL_COLLISION" << std::endl;
                    Wall<DIM>& w = walls[evt->otherIdx];
                    //So, we know there's a particle around this event.
                    //Find it.
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    std::cout << p << std::endl;
                    p.advance(evt->time);
                    std::cout << p << std::endl;
                    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
                    std::cout << p << std::endl;
                    p.setNextEvent({});
                    findNextEvent(p);
                }
                    break;
                case EventType::CELL_BOUNDARY:
                    std::cerr << "Unhandled CELL_BOUNDARY" << std::endl;
                    
                    Particle<DIM>& p = *(evt->getParticle<DIM>());
                    
                    std::cout << p << std::endl;
                    std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    synchronise_all(evt->time - globalTime);
                    globalTime = evt->time;
                    
                    grid.moveParticle(evt);
                                      
                    std::cout << p << std::endl;
                    std::cout << "p.cellIndex=" << p.cellIndex << std::endl;
                    
                    p.setNextEvent({});
                    
                    eventQueue.print();
                    
                    for (Particle<DIM>& pa : particles) {
                        if (pa.getNextEvent().type != EventType::INVALID)
                            eventQueue.erase(&(pa.getNextEvent()));
                        pa.setNextEvent({});
                        findNextEvent(pa);
                    }
                    
                    eventQueue.print();
                    break;
                case EventType::SYNC_RESCALE:
                    rescale();
                    rescaleEvent.time += rescaleTime;
                    eventQueue.push(&rescaleEvent);
                    break;
                case EventType::FUNC_EVALUATION:
                    std::cout << *evt << std::endl;
                    funcTriggers[evt->otherIdx](*this);
                    unownedEvents.erase(evt);
                    delete evt;
                    break;
                case EventType::INVALID:
                    std::cerr << globalTime << std::endl;
                    std::cerr << "Invalid events?!?!?!" << std::endl;
                    return;
            }

        }



        auto t2 = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        std::cout << "EvtTimer: " << tDiff.count()
                << "\nEvt/ms: " << ((double) (evtCount)) / tDiff.count()
                << std::endl;
    }

private:

    void findNextEvent(Particle<DIM>& p1)
    {
        Line<DIM> tr1 = p1.getTrajectory();
        
        //Wall collisions
        for (std::size_t j = 0; j < walls.size(); j++)
        {
            double t = intersection(tr1, walls[j], p1.getRadius(), p1.getLocalTime());
            std::cout << "p1.getLocalTime()=" << p1.getLocalTime() << std::endl;
            if (t < p1.getNextEvent().time)
            {
                Event evt;
                evt.type = EventType::WALL_COLLISION;
                evt.time = t;
                evt.otherIdx = j;
                p1.setNextEvent(evt);
                std::cout << evt << std::endl;
            }
        }
        
        //Cell collisions
        std::cout << "particle " << p1.getID() << " with cells " << std::endl;          
        std::array<int, DIM> ic = grid.getIndexCoordsFromIndex(p1.cellIndex);
        std::cout << "ic=(" << ic[0] << "," << ic[1] << "," << ic[2] << ")" << std::endl;
        std::vector<int> indexes;
        indexes.push_back(ic[0]*2);
        indexes.push_back(ic[0]*2+1);
        indexes.push_back(grid.getNCells(0)*2+ic[1]*2);
        indexes.push_back(grid.getNCells(0)*2+ic[1]*2+1);
        indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2);
        indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2+1);
        double t;
        for (int i : indexes) {
            std::cout << "i=" << i << std::endl;
            std::cout << "plane=" << *(grid.getPlane(i)) << std::endl;
            t = intersection(tr1, *(grid.getPlane(i)), 0.0, 0.0);
            // We ask 't > 0' because after moving a particle to a cell boundary
            // the next collision would be predicted for ever.
            if (t > 1.0E-11) {
                t += globalTime;
                if (t < p1.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::CELL_BOUNDARY;
                    evt.time = t;
                    evt.otherIdx = i;
                    p1.setNextEvent(evt);
                }
            }
        }
        
        //Particle collisions
        Event smallestEvent = p1.getNextEvent();
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p2 = particles[i];
            if (p1.getID() == i)
                continue;

            double t = intersection(tr1, p2.getTrajectory(),
                                    (p1.getRadius() + p2.getRadius()) / 2.0,
                                    p1.getLocalTime() - p2.getLocalTime());
            if (t < smallestEvent.time && t < p2.getNextEvent().time)
            {
                smallestEvent.otherIdx = i;
                smallestEvent.time = t;
                smallestEvent.type = EventType::PARTICLE_COLLISION;
            }
        }


        if (smallestEvent.type == EventType::PARTICLE_COLLISION)
        {
            Particle<DIM>& p2 = particles[smallestEvent.otherIdx];
            //Ah shit, the other particle already has a collision scheduled,
            //so we're just going to cancel that stuff


            
            if (p2.getNextEvent().type == EventType::PARTICLE_COLLISION)
            {
                Particle<DIM>& cancelledPartner = particles[p2.getNextEvent().otherIdx];

                //Who owned the event? (answer: smallest ID)
                Event* pEvent = (p2.getID() > cancelledPartner.getID())
                        ? &p2.getNextEvent() : &cancelledPartner.getNextEvent();

                //BOOM! GONE!
                eventQueue.erase(pEvent);
                
                p1.setNextEvent(smallestEvent);
                smallestEvent.otherIdx = p1.getID();
                p2.setNextEvent(smallestEvent);

                cancelledPartner.setNextEvent({});

                findNextEvent(cancelledPartner);
            }
            else
            {
                p1.setNextEvent(smallestEvent);
                smallestEvent.otherIdx = p1.getID();
                eventQueue.erase(&p2.getNextEvent());
                p2.setNextEvent(smallestEvent);
            }

            if (p1.getID() > p2.getID())
            {
                eventQueue.push(&p1.getNextEvent());
            }
            else
            {
                eventQueue.push(&p2.getNextEvent());
            }

        }
        else
        {
            eventQueue.push(&p1.getNextEvent());
        }


    }

    void initialPopulateEvents()
    {
        // Particle-wall
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p = particles[i];
            Line<DIM> tr1 = p.getTrajectory();
            for (std::size_t j = 0; j < walls.size(); j++)
            {
                double t = intersection(tr1, walls[j], p.getRadius(), p.getLocalTime());
                if (t < p.getNextEvent().time)
                {
                    Event evt;
                    evt.type = EventType::WALL_COLLISION;
                    evt.time = t;
                    evt.otherIdx = j;
                    p.setNextEvent(evt);
                    std::cout << evt << std::endl;
                }
            }
        }
        
        // Particle-particle
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            Particle<DIM>& p1 = particles[i];
            Line<DIM> tr1 = p1.getTrajectory();
            for (std::size_t j = i; j < particles.size(); j++)
            {
                Particle<DIM>& p2 = particles[j];
                Line<DIM> tr2 = p2.getTrajectory();
                double t = intersection(tr1, tr2, (p1.getRadius() + p2.getRadius()) / 2.0, p1.getLocalTime() - p2.getLocalTime());
                if (t < p1.getNextEvent().time && t < p2.getNextEvent().time)
                {
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
        for (Particle<DIM>& p1 : particles)
        {
            if (p1.getNextEvent().type == EventType::PARTICLE_COLLISION)
            {
                Particle<DIM>& p2 = particles[p1.getNextEvent().otherIdx];
                if (p2.getNextEvent().type != EventType::PARTICLE_COLLISION ||
                    p2.getNextEvent().otherIdx != p1.getID())
                {
                    std::cout << "No longer a pair! (" << p1.getID() << " - " << p2.getID() << "\n\t" << p1 << "\n\t" << p2 << std::endl;
                    p1.setNextEvent({});
                }
            }
        }
        for (Particle<DIM>& p1 : particles)
        {
            if (p1.getNextEvent().type != EventType::INVALID)
            {
                if (p1.getNextEvent().type == EventType::PARTICLE_COLLISION &&
                    p1.getNextEvent().otherIdx > p1.getID())
                    continue; //skip double entries
                eventQueue.push(&p1.getNextEvent());
            }
        }
        
        // Particle-cell
        for (std::size_t i = 0; i < particles.size(); i++) {
            
            std::cout << "particle " << i << " with cells " << std::endl;
            
            Particle<DIM>& p1 = particles[i];
            Line<DIM> tr1 = p1.getTrajectory();
            
            std::array<int, DIM> ic = grid.getIndexCoordsFromIndex(p1.cellIndex);

            std::cout << "ic=(" << ic[0] << "," << ic[1] << "," << ic[2] << ")" << std::endl;
            
            std::vector<int> indexes;
            indexes.push_back(ic[0]);
            indexes.push_back(ic[0]+1);
            indexes.push_back(grid.getNCells(0)*2+ic[1]);
            indexes.push_back(grid.getNCells(0)*2+ic[1]+1);
            indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]);
            indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]+1);
            
            double t;
            for (int i : indexes) {
                std::cout << "i=" << i << std::endl;
                t = intersection(tr1, *(grid.getPlane(i)), 0.0, 0.0);
                if (t < p1.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::CELL_BOUNDARY;
                    evt.time = t;
                    evt.otherIdx = i;
                    p1.setNextEvent(evt);
                }
            }
        }
    }
} ;

#endif	/* SIMULATION_H */

