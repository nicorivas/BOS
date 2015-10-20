#ifndef INTERACTION_H
#define	INTERACTION_H

#include "funcs/Proxy.h"

class Interactions {
public:
    Interactions() : funcProxy_ParticleParticle(this),
                     funcProxy_ParticleWall(this) { };
                     
    virtual void fParticleParticle(Particle<3>& p1, Particle<3>& p2, Data<3>& data) {};
    virtual void fParticleWall(Particle<3>& p, Wall<3>& w, Data<3>& data) {};
                     
    ParticleInteractionProxy<Interactions, &Interactions::fParticleParticle> funcProxy_ParticleParticle;
    WallInteractionProxy<Interactions, &Interactions::fParticleWall> funcProxy_ParticleWall;
            
};

#endif	/* INTERACTIONED_H */

