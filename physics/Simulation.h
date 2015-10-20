#ifndef SIMULATION_H
#define	SIMULATION_H

#define DEBUG

#include "physics/Particle.h"
#include "math/Line.h"
#include "math/Intersection.h"
#include "physics/Wall.h"
#include "physics/Grid.h"
#include "Data.h"

#include "funcs/FunctionalEvent.h"
#include "funcs/Snapshot.h"
#include "physics/Interaction.h"
#include "physics/InteractionElastic.h"
#include "physics/InteractionInelastic.h"

#include "funcs/Snapshot.h"

#include "algorithm/Handler.h"
#include "algorithm/PriorityQueue.h"

#include <chrono>
#include <vector>
#include <functional>
#include <set>
#include <string>
#include <cstdio>
#include <iomanip>

template < unsigned int DIM >
class Simulation
{
public:
    typedef long long int lint;
    // Vector of functions that get called at specific moments in the sim,
    // through the FunctionalEvent class. See FunctionalEvent.h for doc.
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_Event;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_WallPre;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_WallPost;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_ParticlePre;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_ParticlePost;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_CellPre;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_CellPost;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_EndOfSimulation;
    std::vector< std::function<void(Simulation<DIM>&)> > fEvents_StartOfSimulation;
    
private:

    PriorityQueue<Event*> eventQueue;
    std::set<Event*> unownedEvents;
    
    Grid<DIM> grid;
    
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    
    // See the FunctionEvent doc to see what functionalEvents are,
    // this container is needed for storing each fevent globally.
    std::vector<FunctionalEvent<DIM>* > functionalEvents;
    
    Handler<DIM>* handler;

    lint eventCount;
    lint eventCountEnd;
    double timeEnd;
    double timeCurrent;
    double timeGlobal;

public:
    
    // Should this be public?
    Data<3>* data;
        
    // We don't use a constructor but the readData function, as at some points
    // we need to reference MercuryED objects (and we don't like forward defs)
    Simulation() { }
    
    // After reading variable to Data object (from Json), some variables are
    // transfered to local versions in this class, for speed and readibility.
    int readData(const std::string& file)
    {
        if (file == "") {
            std::cerr << "Null string passed as config filename" << std::endl;
            return 1;
        } try {
            data = new Data<3>(file, *this);
        } catch (...) {
            std::cerr << "Error creating 'Data' class" << std::endl;
            return 1;
        }
        
        handler = new Handler<DIM>(&particles, &walls, &grid, data, &eventQueue,
                &unownedEvents, &timeCurrent, &timeGlobal);
        
        eventCount = data->eventCount;
        eventCountEnd = data->eventCountEnd;
        timeGlobal = data->time;
        timeCurrent = data->time;
        timeEnd = data->timeEnd;
        
        createGrid({data->domain[0],data->domain[1],data->domain[2]},
        {data->domain[3],data->domain[4],data->domain[5]},
        {data->maxParticleRadius*2.0,data->maxParticleRadius*2.0,data->maxParticleRadius*2.0});
        
        if (data->docJson.HasMember("output")) {
            loadOutputs();
        }
        return 0;
    }
    
    // Create functional events depending on outputs defined in demdf file.
    void loadOutputs() 
    {
        if (data->docJson["output"].HasMember("snapshot")) {
            functionalEvents.push_back(new Snapshot<3>(*this));
            addFunctionalEvent(*functionalEvents[0]);
        }
    }

    lint getEventCount() const
    {
        return eventCount;
    }
    
    lint getEventCountEnd() const
    {
        return eventCountEnd;
    }
    
    double getCurrentTime() const
    {
        return timeCurrent;
    }
    
    void setCurrentTime(double t)
    {
        timeCurrent = t;
    }
    
    double getGlobalTime() const
    {
        return timeGlobal;
    }
    
    void setGlobalTime(double t)
    {
        timeGlobal = t;
    }
    
    void addGlobalTime(double dt)
    {
        timeGlobal += dt;
    }
    
    double getEndTime() const
    {
        return timeEnd;
    }
    
    const std::vector<Particle<DIM> >& getParticles() const
    {
        return particles;
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
    
    const std::vector<Wall<DIM> >& getWalls() const
    {
        return walls;
    }
    
    void addWall(Wall<DIM> wall)
    {
        walls.push_back(wall);
    }
    
    void addFunctionalEvent(const FunctionalEvent<DIM>& fe)
    {
        // Add functional events:
        if (fe.doEvent) {
            std::cout << fe.getTime() << std::endl;
            std::size_t idx = fEvents_Event.size();
            fEvents_Event.emplace_back(fe.proxyEvent);
            queueFunction(idx, fe.getTime());
        }
        if (fe.doWallPre) {
            fEvents_WallPre.emplace_back(fe.proxyWallPre);
        }
        if (fe.doWallPost) {
            fEvents_WallPost.emplace_back(fe.proxyWallPost);
        }
        if (fe.doParticlePre) {
            fEvents_ParticlePre.emplace_back(fe.proxyParticlePre);
        }
        if (fe.doParticlePost) {
            fEvents_ParticlePost.emplace_back(fe.proxyParticlePost);
        }
        if (fe.doCellPre) {
            fEvents_CellPre.emplace_back(fe.proxyCellPre);
        }
        if (fe.doCellPost) {
            fEvents_CellPost.emplace_back(fe.proxyCellPost);
        }
        if (fe.doEndOfSimulation) {
            fEvents_EndOfSimulation.emplace_back(fe.proxyEndOfSimulation);
        }
        if (fe.doStartOfSimulation) {
            fEvents_StartOfSimulation.emplace_back(fe.proxyStartOfSimulation);
        }
    }
    
    std::size_t addFunction(const std::function<void(Simulation&)>& func)
    {
        std::size_t idx = fEvents_Event.size();
        fEvents_Event.emplace_back(func);
        return idx;
    }

    void queueFunction(std::size_t id, double when)
    {
        Event * evt = new Event(when, id, 0, EventType::FUNCTION);
        unownedEvents.insert(evt);
        eventQueue.push(evt);
    }

    // Not quite rigorous, but a solution that works for now. 
    int reset()
    {
        particles.clear();
        walls.clear();
        fEvents_Event.clear();
        fEvents_WallPre.clear();
        fEvents_WallPost.clear();
        fEvents_ParticlePre.clear();
        fEvents_ParticlePost.clear();
        fEvents_EndOfSimulation.clear();
        fEvents_StartOfSimulation.clear();
        grid.reset();
        eventQueue.clear();
        unownedEvents.clear();
        return 0;
    }
    
    int run()
    {
        //for (auto fe : fEvents_StartOfSimulation)
        //    fe(*this);
        
        handler->findInitialEvents();
        
        while (timeGlobal < timeEnd && eventCount < eventCountEnd) {
            Event* evt = eventQueue.top();
            eventQueue.pop();
            handler->handle(evt, *this);
            eventCount++;
        }
        
        //for (auto fe : fEvents_EndOfSimulation)
        //    fe(*this);
        
        return 0;
    }
    
private:
    
    void createGrid(Vector<DIM> domainOrigin, Vector<DIM> domainEnd, Vector<DIM> cellSize) 
    {
        grid = Grid<DIM>(domainOrigin, domainEnd, cellSize);
        grid.init(&particles);
    }
    
    // For debugging
    void printParticles()
    {
        std::cout << "\nParticles state:" << std::endl;
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            std::cout << particles[i] << std::endl;
        }
    }
    
    // For debugging
    void printQueue()
    {
        std::cout << "\nQueue:" << std::endl;
        std::vector<Event*> el = eventQueue.getlist();
        for (auto en : el)
        {
            std::cout << en << " " << (*en) << std::endl;
        }
    }
    
};
#endif	/* SIMULATION_H */


