/* 
 * File:   main.cpp
 * Author: dducks
 *
 * Created on April 20, 2015, 6:41 PM
 */

#include <physics/Simulation.h>
#include <physics/Particle.h>
#include <physics/Wall.h>
#include <physics/Models.h>

#include <funcs/Snapshot.h>

#include <math/Intersection.h>

int main(int argc, char** argv) {
    
    Simulation<3> sim("test.json");

    // Adding physical models.
    sim.setParticleCollisionModel(particleElastic);
    sim.setWallCollisionModel(wallElastic);
    /* Models could also be defined as lambda functions, as:
    sim.setParticleCollisionModel([&](Particle<3>& pA, Particle<3>& pB) {
        Vector<3> d = pA.getPosition() - pB.getPosition();
        d /= sqrt(dot(d, d)); //normalise
        Vector<3> v = pA.getVelocity() - pB.getVelocity();
        pA.setVelocity(pA.getVelocity() - d * dot(v, d));
        pB.setVelocity(pB.getVelocity() + d * dot(v, d));
    });
    */
    
    // Adding function events (measurements or etc.)
    Snapshot s(sim);
    sim.addFunctionEvent(s);
    
    //int funcNum = sim.addFunctionEvent(s.functionEventP);
    //sim.queueFunction(funcNum, 10.0);
    

    
    return sim.run();
}
