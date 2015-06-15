/* 
 * File:   main.cpp
 * Author: dducks
 *
 * Created on April 20, 2015, 6:41 PM
 */

#include <physics/Simulation.h>
#include <physics/Particle.h>
#include <physics/Wall.h>

#include <math/Intersection.h>


int main(int argc, char** argv) {
    
    Simulation<3> sim("test.json");
    return sim.run();
}
