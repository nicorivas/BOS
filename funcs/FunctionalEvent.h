/* 
 * File:   FunctionalEvent.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 14:58 AM
 */

#ifndef FUNCTIONALEVENT_H
#define	FUNCTIONALEVENT_H

#include <funcs/FunctionalProxy.h>

/*
 * Functional events are events that trigger function calls, either at specific
 * times or event positions. They work by defining the functions available, and
 * then the creator of functional events registers this functions in the 
 * appropriate lists in the Simulation object. See each function comment so see
 * where is this called. 
 */
class FunctionalEvent {
    
public:
    double time;
    bool doEvent;
    bool doWallPre;
    bool doWallPost;
    bool doParticlePre;
    bool doParticlePost;
    bool doEndOfSimulation;
    
    FunctionalEvent() : funcProxy_Event(this),
                      funcProxy_WallPre(this),
                      funcProxy_WallPost(this),
                      funcProxy_ParticlePre(this),
                      funcProxy_ParticlePost(this),
                      funcProxy_EndOfSimulation(this),
                      doWallPre(false), 
                      doWallPost(false), 
                      doParticlePre(false), 
                      doParticlePost(false),
                      doEndOfSimulation(false) { };
    
    double getTime() const {
        return time;
    }
                      
    // Called at specified 'time' as the distinct event type 'FUNCTION'
    virtual void fEvent(Simulation<3>& sim) {};
    // Called before particle-wall collisions (after updating positions but not velocities)
    virtual void fWallPre(Simulation<3>& sim) {};
    // Called after particle-wall collisions
    virtual void fWallPost(Simulation<3>& sim) {};
    // Called before particle-particle collisions (after updating positions but not velocities)
    virtual void fParticlePre(Simulation<3>& sim) {};
    // Called after particle-particle collisions
    virtual void fParticlePost(Simulation<3>& sim) {};
    // Called after particle-particle collisions
    virtual void fEndOfSimulation(Simulation<3>& sim) {};

    // Here, because it needs to be after the definition of the function.
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fEvent> funcProxy_Event;
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fWallPre> funcProxy_WallPre;
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fWallPost> funcProxy_WallPost;
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fParticlePre> funcProxy_ParticlePre;
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fParticlePost> funcProxy_ParticlePost;
    FunctionProxy<FunctionalEvent, &FunctionalEvent::fParticlePost> funcProxy_EndOfSimulation;
    
    
private:

};

#endif

