#ifndef INTERACTIONELASTIC_H
#define	INTERACTIONELASTIC_H

#include "Interaction.h"

class InteractionElastic: public Interactions {
public:
    InteractionElastic() {};
    
    void fParticleParticle(Particle<3>& pA, Particle<3>& pB, Data<3>& data) {
        Vector<3> d = pA.getPosition() - pB.getPosition();
        Vector<3> v = pA.getVelocity() - pB.getVelocity();
        d /= sqrt(dot(d, d)); //normalise
        pA.setVelocity(pA.getVelocity() - d * dot(v, d));
        pB.setVelocity(pB.getVelocity() + d * dot(v, d));
    }
    
    void fParticleWall(Particle<3>& p, Wall<3>& w, Data<3>& data) {
        p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
    };
    
private:

};

#endif	/* INTERACTIONELASTIC_H */

