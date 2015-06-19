/* 
 * File:   main.cpp
 * Author: dducks
 *
 * Created on April 20, 2015, 6:41 PM
 */

#include <physics/Simulation.h>
#include <physics/Models.h>

#include <funcs/Snapshot.h>

int main(int argc, char** argv) {
    
    Simulation<3> sim("test.json");
    
    // Adding functional events (measurements or etc.)
    Snapshot s(sim);
    sim.addFunctionalEvent(s);
   
    return sim.run();
}
