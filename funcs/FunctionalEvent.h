#ifndef FUNCTIONALEVENT_H
#define	FUNCTIONALEVENT_H

#include "Proxy.h"

/*
 * Functional events are events that trigger function calls, either at specific
 * times or event positions. They work by defining the functions available, and
 * then the creator of functional events registers these functions in the 
 * appropriate lists in the Simulation object. See each function comment so see
 * where is this called. As we want to call internal class methods from the
 * Simulation class, we use functional proxys. See FunctionalProxy.h for doc.
 */
template<unsigned int DIM>
class FunctionalEvent {
    
public:
    double time;
    bool doEvent;
    bool doWallPre;
    bool doWallPost;
    bool doParticlePre;
    bool doParticlePost;
    bool doCellPre;
    bool doCellPost;
    bool doEndOfSimulation;
    bool doStartOfSimulation;
    
    FunctionalEvent() : proxyEvent(this),
                      proxyWallPre(this),
                      proxyWallPost(this),
                      proxyParticlePre(this),
                      proxyParticlePost(this),
                      proxyCellPre(this),
                      proxyCellPost(this),
                      proxyEndOfSimulation(this),
                      proxyStartOfSimulation(this),
                      doEvent(false),
                      doWallPre(false), 
                      doWallPost(false), 
                      doParticlePre(false), 
                      doParticlePost(false),
                      doCellPre(false), 
                      doCellPost(false),
                      doEndOfSimulation(false),
                      doStartOfSimulation(false) { };
    
    double getTime() const {
        return time;
    }
                      
    // Called at specified 'time' as the distinct event type 'FUNCTION'
    virtual void event(Simulation<DIM>& sim) {};
    
    // Called before particle-wall collisions (after updating positions but not velocities)
    virtual void wallPre(Simulation<DIM>& sim) {};
    
    // Called after particle-wall collisions
    virtual void wallPost(Simulation<DIM>& sim) {};
    
    // Called before particle-particle collisions (after updating positions but not velocities)
    virtual void particlePre(Simulation<DIM>& sim) {};
    
    // Called after particle-particle collisions
    virtual void particlePost(Simulation<DIM>& sim) {};
    
    // Called before particle-cell "collisions"
    virtual void cellPre(Simulation<DIM>& sim) {};
    
    // Called after particle-cell "collisions"
    virtual void cellPost(Simulation<DIM>& sim) {};
    
    // Called just after handling last event.
    virtual void endOfSimulation(Simulation<DIM>& sim) {};
    
    // Called just before handling first event.
    virtual void startOfSimulation(Simulation<DIM>& sim) {};

    // These here, because they need to be after the definition of the function.
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::event> proxyEvent;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::wallPre> proxyWallPre;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::wallPost> proxyWallPost;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::particlePre> proxyParticlePre;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::particlePost> proxyParticlePost;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::cellPre> proxyCellPre;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::cellPost> proxyCellPost;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::endOfSimulation> proxyEndOfSimulation;
    FunctionalEventProxy<FunctionalEvent<DIM>, &FunctionalEvent<DIM>::startOfSimulation> proxyStartOfSimulation;
    
    
private:

};

#endif	/* FUNCTIONALEVENT_H */

