/* 
 * File:   Snapshot.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 11:38 AM
 */

#ifndef SNAPSHOT_H
#define	SNAPSHOT_H

#include <funcs/FunctionalEvent.h>
#include <funcs/FunctionalProxy.h>
#include <input/Data.h>

class Snapshot: public FunctionalEvent {
    
public:
    
    std::size_t count;
    
    Snapshot(Simulation<3>& sim) : count(0) {
        time = sim.data.docJson["output"][0]["savePeriodInTime"].GetDouble();
        doEvent = true;
        doWallPre = false;
        doWallPost = false;
        doParticlePre = false;
        doParticlePost = false;
        doEndOfSimulation = true;
    }
    
    void fEvent(Simulation<3>& sim)
    {
        sim.data.output(sim);
        sim.queueFunction(0, sim.getCurrentTime()+time);
        count++;
    }
    
    void fWallPre(Simulation<3>& sim)
    {
        std::cout << "Wall collision Pre" << std::endl;
    }
    
    void fWallPost(Simulation<3>& sim)
    {
        std::cout << "Wall collision Post" << std::endl;
    }
    
    void fParticlePre(Simulation<3>& sim)
    {
        std::cout << "Particle collision Pre" << std::endl;
    }
    
    void fParticlePost(Simulation<3>& sim)
    {
        std::cout << "Particle collision Post" << std::endl;
    }
    
    void fEndOfSimulation(Simulation<3>& sim)
    {
        sim.data.output(sim);
    }
    
private:

};

#endif	/* SNAPSHOT_H */

