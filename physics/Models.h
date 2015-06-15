/* 
 * File:   Models.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 2:50 PM
 */

#ifndef MODELS_H
#define	MODELS_H

void particleElastic(Particle<3>& pA, Particle<3>& pB)
{
    Vector<3> d = pA.getPosition() - pB.getPosition();
    d /= sqrt(dot(d, d)); //normalise
    Vector<3> v = pA.getVelocity() - pB.getVelocity();
    pA.setVelocity(pA.getVelocity() - d * dot(v, d));
    pB.setVelocity(pB.getVelocity() + d * dot(v, d));
}

void wallElastic(Particle<3>& p, Wall<3>& w)
{
    p.setVelocity(p.getVelocity() - 2 * dot(p.getVelocity(), w.getNormal()) * w.getNormal());
}

#endif	/* MODELS_H */

