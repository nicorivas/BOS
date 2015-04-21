/* 
 * File:   Simulation.h
 * Author: dducks
 *
 * Created on April 20, 2015, 6:44 PM
 */

#ifndef SIMULATION_H
#define	SIMULATION_H

#include <physics/Particle.h>
#include <math/Line.h>
#include <physics/Wall.h>

#include <vector>
#include <functional>

template < unsigned int DIM >
class Simulation {
private:
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    std::vector< std::function<void(Simulation&)> > funcTriggers;
    
    double endTime;
    double rescaleTime;
public:
    
    /**
     * Constructs a Simulation object with a synchronisation / rescale event
     * every rescaleTime seconds, which will end at endTime
     * @param endTime the time at which this simulation will end
     * @param rescaleTime the time at which a rescaleEvent is needed
     */
    Simulation(double endTime, double rescaleTime) : endTime(endTime), rescaleTime(rescaleTime) {
        
    }
    
    std::size_t registerFunction(std::function<void(Simulation&)>&& func) {
        std::size_t idx = funcTriggers.size();
        funcTriggers.emplace_back(func);
        return idx;
    }
    
    void run() {
        
    }
};

#endif	/* SIMULATION_H */

