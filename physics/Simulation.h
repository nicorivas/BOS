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

template < unsigned int DIM >
class Simulation {
private:
    std::vector< Particle<DIM> > particles;
    std::vector< Wall<DIM> > walls;
    
    Vector<DIM> cellSize;
public:
    
};

#endif	/* SIMULATION_H */

