#ifndef SNAPSHOT_H
#define	SNAPSHOT_H

#include "funcs/FunctionalEvent.h"
#include "funcs/Proxy.h"
#include "physics/Data.h"

template<unsigned int DIM>
class Simulation;

template < unsigned int DIM >
class Snapshot : public FunctionalEvent<DIM> {
    
public:
    
    std::size_t count;
    std::string filename; // Filename for output files    
    
    Snapshot(Simulation<DIM>& sim) : count(0)
    {
        this->time = sim.data->docJson["output"]["snapshot"]["savePeriodInTime"].GetDouble();
        filename = sim.data->docJson["output"]["snapshot"]["filename"].GetString();
        this->doEvent = false;
        this->doWallPre = false;
        this->doWallPost = false;
        this->doParticlePre = false;
        this->doParticlePost = false;
        this->doCellPre = false;
        this->doCellPost = true;
        this->doEndOfSimulation = false;
    }
    
    void output(Simulation<DIM>& sim)
    {
        //std::cout << "HOLI" << std::endl;
        sim.data->docJson["output"]["snapshot"]["saveCount"].SetInt(count);
        sim.data->output(sim, filename+"_"+std::to_string(count)+".json");
        count++;        
    }
    
    void event(Simulation<DIM>& sim)
    {
        output(sim);
        sim.queueFunction(0, sim.getCurrentTime()+this->time);
    }
    
    void wallPre(Simulation<DIM>& sim)
    {
        std::cout << "Wall collision Pre" << std::endl;
        output(sim);
    }
    
    void wallPost(Simulation<DIM>& sim)
    {
        std::cout << "Wall collision Post" << std::endl;
        output(sim);
    }
    
    void particlePre(Simulation<DIM>& sim)
    {
        std::cout << "Particle collision Pre" << std::endl;
        output(sim);
    }
    
    void particlePost(Simulation<DIM>& sim)
    {
        std::cout << "Particle collision Post" << std::endl;
        output(sim);
    }
    
    void cellPre(Simulation<DIM>& sim)
    {
        std::cout << "Cell collision Pre" << std::endl;
        output(sim);
    }
    
    void cellPost(Simulation<DIM>& sim)
    {
        std::cout << "Cell collision Post" << std::endl;
        output(sim);
    }
    
    void endOfSimulation(Simulation<DIM>& sim)
    {
        std::cout << "endOfSimulation" << std::endl;
        output(sim);
    }
    
private:

};

#endif	/* SNAPSHOT_H */

