/* 
 * File:   Snapshot.h
 * Author: nrivas
 *
 * Created on June 15, 2015, 11:38 AM
 */

#ifndef SNAPSHOT_H
#define	SNAPSHOT_H

#include <funcs/FunctionEvent.h>
#include <funcs/FunctionProxy.h>
#include <input/Data.h>

class Snapshot: public FunctionEvent {
    
public:
    
    std::size_t count;
    double period;
    
    Snapshot(Simulation<3>& sim) : count(0) {
        period = sim.data.docJson["measure"]["mFields1D"]["frequency"].GetDouble();
        doWallCollisionPre = true;
        doWallCollisionPost = true;
        doParticleCollisionPre = true;
        doParticleCollisionPost = true;
    }
    
    void wallCollisionPre(Simulation<3>& sim)
    {
        std::cout << "Wall collision Pre" << std::endl;
    }
    /*
    void particleCollisionPre(Simulation<3>& sim)
    {
        std::cout << "Particle collision Pre" << std::endl;
    }
    */
    
    void functionEvent(Simulation<3>& sim)
    {
        std::ofstream myfile;
        myfile.open ("snapshot_"+std::to_string(count)+".dat");
        const std::vector<Particle<3> >& particles = sim.getParticles();
        for (std::size_t i = 0; i < particles.size(); i++)
        {
            const Particle<3>& p = particles[i];
            Vector<3> pos = p.getPosition();
            //p.advance(currentTime-p.localTime); //uncomment if you want sync snapshots
            myfile << pos[0] << " " 
                   << pos[1] << " "
                   << pos[2] << " ";
            pos = p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime());
            myfile << pos[0] << " " 
                   << pos[1] << " "
                   << pos[2] << std::endl;
        }
        myfile.close();
        sim.queueFunction(0, sim.getCurrentTime()+period);
        count++;
    }    
    
private:

};

#endif	/* SNAPSHOT_H */

