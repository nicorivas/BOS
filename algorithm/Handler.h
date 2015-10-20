/* 
 * File:   Handler.h
 * Author: nicorivas
 *
 * Created on August 1, 2015, 11:53 AM
 */

#ifndef HANDLER_H
#define	HANDLER_H

#include "physics/Simulation.h"
#include "physics/Event.h"
#include "algorithm/PriorityQueue.h"

#include <vector>
#include <iomanip>
#include <set>

template < unsigned int DIM >
class Handler {
public:
    std::vector< Particle<DIM> >* particles;
    std::vector< Wall<DIM> >* walls;
    Grid<DIM>* grid;
    Data<DIM>* data;
    PriorityQueue<Event*>* eventQueue;
    std::set<Event*>* unownedEvents;
    double* timeCurrent;
    double* timeGlobal;
    
    // See MercuryED::synchronise for doc on what sync is.
    Event syncEvent;
    
    // Interaction is a FunctionalProxy
    Interactions* interaction = NULL;
    std::function<void(Particle<DIM>&, Particle<DIM>&, Data<DIM>&)> particleCollision;
    std::function<void(Particle<DIM>&, Wall<DIM>&, Data<DIM>&)> wallCollision;
        
    Handler(std::vector< Particle<DIM> > * particles,
            std::vector< Wall<DIM> > * walls,
            Grid<DIM> * grid,
            Data<DIM> * data,
            PriorityQueue<Event*>* eventQueue,
            std::set<Event*>* unownedEvents,
            double * timeCurrent,
            double * timeGlobal):
            particles(particles),
            walls(walls),
            grid(grid),
            data(data),
            eventQueue(eventQueue),
            unownedEvents(unownedEvents),
            timeCurrent(timeCurrent),
            timeGlobal(timeGlobal)
    {
        // We need to init interactions here (and not in Data), as the
        // Interactions class needs a Data object, and the proxy's need a 
        // SimulationED object.
        if (data->wallInteractionType == "wallElastic" && 
            data->particleInteractionType == "particleElastic") {
            interaction = new InteractionElastic();
            particleCollision = interaction->funcProxy_ParticleParticle;
            wallCollision = interaction->funcProxy_ParticleWall;
        } else if (data->wallInteractionType == "wallInelastic" && 
                   data->particleInteractionType == "particleInelastic") {
            interaction = new InteractionInelastic();
            particleCollision = interaction->funcProxy_ParticleParticle;
            wallCollision = interaction->funcProxy_ParticleWall;
        } else {
            std::cerr << "Error: combination of interaction types not supported" << std::endl;
            exit(0);
        }
        
        syncEvent= Event(data->syncTime, 0, 0, EventType::SYNC);
        
    };
            
    int handle(Event* evt, Simulation<DIM>& sim)
    {    
        std::cout << "* (" << sim.getEventCount() << " " << *timeGlobal << ") " << *evt << std::endl;
        
        if (evt->time < sim.getCurrentTime()) {
            std::cerr << std::setprecision(16);
            std::cerr << "Exiting: event in the past:" << std::endl \
                    << "currentTime=" << sim.getCurrentTime() \
                    << " globalTime=" << sim.getGlobalTime() \
                    << " " << *evt << std::endl;
            //exit(0);
        }

        switch (evt->type)
        {
            case EventType::PARTICLE_COLLISION:
            {   
                handleParticleCollision(evt, sim);
                break;
            }
            case EventType::WALL_COLLISION:
            {
                handleWallCollision(evt, sim);
                break;
            }
            case EventType::CELL_BOUNDARY:
            {
                handleCellCollision(evt, sim);
                break;
            }
            case EventType::SYNC:
            {
                handleSync(evt, sim);
                break;
            }   
            case EventType::FUNCTION:
            {
                handleFunction(evt, sim);
                break;
            }
            case EventType::INVALID:
            {
                handleInvalid(evt, sim);
                break;
            }
        }
        return 0;
    };
    
    void handleParticleCollision(Event* evt, Simulation<DIM>& sim)
    {
        Particle<DIM>& pA = (*particles)[evt->ownerId];
        Particle<DIM>& pB = (*particles)[evt->otherId];

        pA.advance(evt->time-pA.getLocalTime());
        pB.advance(evt->time-pB.getLocalTime());
        
        std::cout << "handleParticleCollision-" << pA << std::endl;
        std::cout << "handleParticleCollision-" << pB << std::endl;
#ifdef DEBUG
        if (sqrt(dot(pA.getPosition() - pB.getPosition(),pA.getPosition() - pB.getPosition())) < pA.getRadius() + pB.getRadius())
        {
            std::cerr << "Overlap mister" << std::endl;
            exit(0);
        }
#endif
        
        sim.addGlobalTime(evt->time-sim.getCurrentTime());
        sim.setCurrentTime(evt->time);

        for (auto fe : sim.fEvents_ParticlePre)
            fe(sim);

        particleCollision(pA, pB, *(sim.data));
        
        for (auto fe : sim.fEvents_ParticlePost)
            fe(sim);

        eventQueue->erase(&(pB.getNextEvent()));
        
        std::cout << "handleParticleCollision-" << pA << std::endl;
        std::cout << "handleParticleCollision-" << pB << std::endl;
        
        pA.setNextEvent({});
        pB.setNextEvent({});                    
        findNextEvent(pA);
        findNextEvent(pB);
    };
    
    void handleWallCollision(Event* evt, Simulation<DIM>& sim)
    {
        Particle<DIM>& p = (*particles)[evt->ownerId];
        Wall<DIM>& w = (*walls)[evt->otherId];
        
        p.advance(evt->time-p.getLocalTime());
        sim.addGlobalTime(evt->time-sim.getCurrentTime());
        sim.setCurrentTime(evt->time);
        
        std::cout << "handleWallCollision-" << p << std::endl;

        for (auto fe : sim.fEvents_WallPre)
            fe(sim);

        wallCollision(p, w, *(sim.data));

        for (auto fe : sim.fEvents_WallPost)
            fe(sim);

        p.setNextEvent({});
        findNextEvent(p);
    };
    
    void handleCellCollision(Event* evt, Simulation<DIM>& sim)
    {
        Particle<DIM>& p = (*particles)[evt->ownerId];

        std::cout << std::setprecision(16);
        std::cout << "handleCellCollision-" << p << std::endl;
        std::cout << "handleCellCollision-" << p.getCurrentPosition(evt->time) << std::endl;
        
        for (auto fe : sim.fEvents_CellPre)
            fe(sim);
        
        // I don't think this is necessary, we should try when
        // the code is stable. As velocity doesn't change,
        // collisions will anyway be correctly predicted.
        p.advance(evt->time-p.getLocalTime());
        //std::cout << "WTF" << p << std::endl;
        //p.setLocalTime(evt->time-p.getLocalTime());
        
        grid->moveParticle(p, evt->otherId);

        sim.addGlobalTime(evt->time-sim.getCurrentTime());
        sim.setCurrentTime(evt->time);
        
        for (auto fe : sim.fEvents_CellPost)
            fe(sim);

        p.setNextEvent({});
        findNextEvent(p);
    };
    
    void handleSync(Event* evt, Simulation<DIM>& sim) {
        std::cout << "synchronise" << std::endl;
        double timeZero = *timeCurrent;
        for (Particle<DIM>& p : *particles) {
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
        for (Event* evt : *unownedEvents) {
            evt->time -= timeZero;
        }
        *timeCurrent = 0.0;
        eventQueue->erase(&syncEvent);
        syncEvent.time += data->syncTime;
        eventQueue->push(&syncEvent);
    };
    
    void handleFunction(Event* evt, Simulation<DIM>& sim) {
        sim.addGlobalTime(evt->time-sim.getCurrentTime());
        sim.setCurrentTime(evt->time);
        sim.fEvents_Event[evt->otherId](sim);
        unownedEvents->erase(evt);
        delete evt;
    };
    
    void handleInvalid(Event* evt, Simulation<DIM>& sim)
    {
        std::cerr << "Invalid event: " << *evt << std::endl;
        exit(0);
    };
    
    Event& findWallCollisions(Particle<DIM>& p)
    {
        //std::cout << "findWallCollisions(Particle<DIM>& p="<<p.getID()<<")" << std::endl;
        if (!data->bodyForceExists) {
            Line<3> tr = p.getTrajectory();
            for (std::size_t j = 0; j < (*walls).size(); j++) {
                // dt is the time of the next collision from now (currentTime)
                double dt = intersection(tr, (*walls)[j], p.getRadius(),
                                *timeCurrent-p.getLocalTime());
                if (*timeCurrent+dt < p.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::WALL_COLLISION;
                    evt.time = *timeCurrent+dt;
                    evt.ownerId = p.getID();
                    evt.otherId = j;
                    p.setNextEvent(evt);
                }
            }
            return p.getNextEvent();
        } else {
            Parabola<3> tr = p.getTrajectoryGravity();
            for (std::size_t j = 0; j < (*walls).size(); j++) {
                double dt = intersection(tr, (*walls)[j], p.getRadius(),
                                *timeCurrent-p.getLocalTime());
                if (*timeCurrent+dt < p.getNextEvent().time) {
                    Event evt;
                    evt.type = EventType::WALL_COLLISION;
                    evt.time = *timeCurrent+dt;
                    evt.ownerId = p.getID();
                    evt.otherId = j;
                    p.setNextEvent(evt);
                }
            }
            return p.getNextEvent();
        }
    }
    
    // Finds collisions with the corresponding planes in the grid.
    Event& findCellCollisions(Particle<DIM>& p)
    {
        //std::cout << "findCellCollisions(Particle<DIM>& p="<<p.getID()<<")" << std::endl;
        if (!data->bodyForceExists) {
            Line<DIM> tr = p.getTrajectory();
            std::array<int, DIM> ic = (*grid).getIndexCoordsFromIndex(p.getCellIndex());
            std::vector<int> indexes;
            // TODO: This is inefficient.
            // We have to do this, otherwise we can get collisions with 
            // the cells boundaries we are currently at. Consider that the 
            // particle position is updated at every cell collision. The due
            // to numerical errors we can have the particle behind the actual
            // cell it just collided, and thus it will predict this collision
            // at time zero and this will happen for ever.
            
            Vector<3> vel = p.getVelocity();
            if (vel[0] < 0) {
                indexes.push_back(ic[0]*2);
            } else {
                indexes.push_back(ic[0]*2+1);
            }
            if (vel[1] < 0) {
                indexes.push_back((*grid).getNCells(0)*2+ic[1]*2);
            } else {
                indexes.push_back((*grid).getNCells(0)*2+ic[1]*2+1);
            }
            if (vel[2] < 0) {
                indexes.push_back((*grid).getNCells(0)*2+(*grid).getNCells(1)*2+ic[2]*2);
            } else {
                indexes.push_back((*grid).getNCells(0)*2+(*grid).getNCells(1)*2+ic[2]*2+1);
            }
            /*
            indexes.push_back(ic[0]*2);
            indexes.push_back(ic[0]*2+1);
            indexes.push_back(grid.getNCells(0)*2+ic[1]*2);
            indexes.push_back(grid.getNCells(0)*2+ic[1]*2+1);
            indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2);
            indexes.push_back(grid.getNCells(0)*2+grid.getNCells(1)*2+ic[2]*2+1);
            */
            double dt;
            for (int i : indexes) {
                dt = intersection(tr, *((*grid).getPlane(i)), 0.0, *timeCurrent-p.getLocalTime());
                // We ask 't > 0' because after moving a particle to a cell boundary
                // the next collision would be predicted for ever. This is fragile;
                // there must be a better way to do it.
                //if (dt > 1.0E-13) {
                    if (*timeCurrent+dt < p.getNextEvent().time) {
                        Event evt;
                        evt.type = EventType::CELL_BOUNDARY;
                        evt.time = *timeCurrent+dt;
                        evt.ownerId = p.getID();
                        evt.otherId = i;
                        p.setNextEvent(evt);
                        std::cout << "\t* PREDICTED * " << evt << std::endl;
                    }
                //}
            }
            return p.getNextEvent();
        }else{
            Parabola<DIM> tr = p.getTrajectoryGravity();
            std::array<int, DIM> ic = grid->getIndexCoordsFromIndex(p.getCellIndex());
            std::vector<int> indexes;
            indexes.push_back(ic[0]*2);
            indexes.push_back(ic[0]*2+1);
            indexes.push_back(grid->getNCells(0)*2+ic[1]*2);
            indexes.push_back(grid->getNCells(0)*2+ic[1]*2+1);
            indexes.push_back(grid->getNCells(0)*2+grid->getNCells(1)*2+ic[2]*2);
            indexes.push_back(grid->getNCells(0)*2+grid->getNCells(1)*2+ic[2]*2+1);
            double dt;
            for (int i : indexes) {
                dt = intersection(tr, *(grid->getPlane(i)), 0.0, *timeCurrent-p.getLocalTime());
                // We ask 't > 0' because after moving a particle to a cell boundary
                // the next collision would be predicted for ever. This is fragile;
                // there must be a better way to do it.
                if (dt > 1.0E-13) {
                    //std::cout << "dt=" << dt << std::endl;
                    if (*timeCurrent+dt < p.getNextEvent().time) {
                        Event evt;
                        evt.type = EventType::CELL_BOUNDARY;
                        //std::cout << "timeCurrent=" << *timeCurrent << std::endl;
                        evt.time = *timeCurrent+dt;
                        //std::cout << "timeCurrent=" << *timeCurrent << std::endl;
                        evt.ownerId = p.getID();
                        evt.otherId = i;
                        p.setNextEvent(evt);
                        //std::cout << "\t* PREDICTED * " << evt << std::endl;
                    }
                }
            }
            return p.getNextEvent();
        }
    }
    
    Event& findParticleCollisions(Particle<DIM>& p1) 
    { 
        //std::cout << "findParticleCollisions(Particle<DIM>& p1)" << std::endl;
        
        Event evt; // This event dies, as it is copied when added to particles
        Line<DIM> tr1 = p1.getTrajectory();
        
        std::vector<int> neighbors = (*grid).getNeighbors(&p1);
        //std::vector<int> neighbors = {0, 1};
        //std::cout << "neighbors.size()=" << neighbors.size() << std::endl;
        for (std::size_t i = 0; i < neighbors.size(); i++)
        {
            Particle<DIM>& p2 = (*particles)[neighbors[i]];
            
            if (p2.getID() == p1.getID())
                continue;
            
            //std::cout << "\tneighbors[i="<<i<<"]=" << neighbors[i] << std::endl;
            //std::cout << "\tp2=" << p2 << std::endl;
            double r1 = data->particleTypes[p1.getType()].radius;
            double r2 = data->particleTypes[p2.getType()].radius;
            double dt = intersection(tr1, p2.getTrajectory(), (r1+r2) / 2.0,
                                     p1.getLocalTime() - p2.getLocalTime());
            
            //std::cout << "\tcurrentTime + dt="<<(currentTime+dt)<<" evt.time="<<evt.time<<" p1.getNextEvent().time="<<(p1.getNextEvent().time)<<std::endl;
            if (*timeCurrent + dt < p1.getNextEvent().time &&
                *timeCurrent + dt < p2.getNextEvent().time)
            {
                evt.type = EventType::PARTICLE_COLLISION;
                evt.time = *timeCurrent+dt;
                evt.ownerId = p1.getID();
                evt.otherId = p2.getID();
                p1.setNextEvent(evt);
                //std::cout << "\t* PREDICTED * " << evt << std::endl;
            }
        }
        return p1.getNextEvent();
    }

    void findNextEvent(Particle<DIM>& p) 
    {
        //std::cout << "void findNextEvent(p.getID()="<<p.getID()<<")" << std::endl;
        // Pointer here, as the actual event is contained by the particles,
        // and the queue only contains the pointers.
        Event * evt;
        evt = &findWallCollisions(p);
        evt = &findCellCollisions(p);
        std::cout << "findCellCollisions.after" << std::endl;
        evt = &findParticleCollisions(p);
        (*eventQueue).push(evt);
        std::cout << "findNextEvent.end" << std::endl;
    }
    
    void findInitialEvents() 
    {
        for (std::size_t i = 0; i < (*particles).size(); i++)
        {
            Particle<DIM>& p = (*particles)[i];
            findNextEvent(p);
        }
        //Other events
        (*eventQueue).push(&syncEvent);
        
        std::cout << "findInitialEvents.end" << std::endl;
    }
private:

};

#endif	/* HANDLER_H */

