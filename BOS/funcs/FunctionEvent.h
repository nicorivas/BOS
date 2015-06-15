/* 
 * File:   EventFunction.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 14:58 AM
 */

#ifndef FUNCTIONEVENT_H
#define	FUNCTIONEVENT_H

#include <funcs/FunctionProxy.h>

class FunctionEvent {
    
public:
    double time;
    bool doWallCollisionPre;
    bool doWallCollisionPost;
    bool doParticleCollisionPre;
    bool doParticleCollisionPost;
    
    FunctionEvent() : functionEventP_Event(this),
                      functionEventP_WallPre(this),
                      functionEventP_WallPost(this),
                      functionEventP_ParticlePre(this),
                      functionEventP_ParticlePost(this),
                      doWallCollisionPre(false), 
                      doWallCollisionPost(false), 
                      doParticleCollisionPre(false), 
                      doParticleCollisionPost(false) { };
    
    virtual void functionEvent(Simulation<3>& sim) {};
    virtual void wallCollisionPre(Simulation<3>& sim) {};
    virtual void wallCollisionPost(Simulation<3>& sim) {};
    virtual void particleCollisionPre(Simulation<3>& sim) {};
    virtual void particleCollisionPost(Simulation<3>& sim) {};

    // Needs to be after the definition of the function.
    FunctionProxy<FunctionEvent, &FunctionEvent::functionEvent> functionEventP_Event;
    FunctionProxy<FunctionEvent, &FunctionEvent::wallCollisionPre> functionEventP_WallPre;
    FunctionProxy<FunctionEvent, &FunctionEvent::wallCollisionPost> functionEventP_WallPost;
    FunctionProxy<FunctionEvent, &FunctionEvent::particleCollisionPre> functionEventP_ParticlePre;
    FunctionProxy<FunctionEvent, &FunctionEvent::particleCollisionPost> functionEventP_ParticlePost;
    
    
private:

};

#endif

