#ifndef INTERACTIONINELASTIC_H
#define	INTERACTIONINELASTIC_H

#include "Interaction.h"

class InteractionInelastic: public Interactions {
public:
    InteractionInelastic() {};
    
    void fParticleParticle(Particle<3>& pA, Particle<3>& pB, Data<3>& data) {
        double massA = data.particleTypes[pA.getType()].mass;
        double massB = data.particleTypes[pB.getType()].mass;
        double rA = data.particleTypes[pA.getType()].restitutionCoefficient;
        double rB = data.particleTypes[pB.getType()].restitutionCoefficient;
        //std::cout << massA << std::endl;
        //std::cout << massB << std::endl;
        //std::cout << rA << std::endl;
        //std::cout << rB << std::endl;
        double r = (rA+rB)/2.0;
        double reducedMass = massA*massB/(massA + massB);

        Vector<3> n = pB.getPosition() - pA.getPosition();
        Vector<3> rv = pB.getVelocity() - pA.getVelocity();
        n /= sqrt(dot(n, n)); //normalise
        double vn = dot(rv, n);
        double pn = (1+r)/2.0;
        double qn = reducedMass * 2.0 * pn * vn;
        Vector<3> q = qn*n;
        pA.setVelocity(pA.getVelocity() + q/massA);
        pB.setVelocity(pB.getVelocity() - q/massB);
    }
    
    void fParticleWall(Particle<3>& p, Wall<3>& w, Data<3>& data) {
        Vector<3> n =  w.getNormal();
        // A readable but inefficient form:
        /*
        double mass = data.particleTypes[p.getType()].mass;
        double r = data.particleTypes[p.getType()].restitutionCoefficient;

        double pn = (1+r)/2.0;
        double qn = mass * 2.0 * pn * vn;
        Vector<3> q = qn*n;
        p.setVelocity(p.getVelocity() - q/mass);
        */
        // An somewhat optimized form:
        double r = data.particleTypes[p.getType()].restitutionCoefficient;
        p.setVelocity(p.getVelocity() - 2.0 * ((1+r)/2.0) * dot(p.getVelocity(), n) * n);
    };
    
private:

};

#endif	/* INTERACTIONINELASTIC_H */

