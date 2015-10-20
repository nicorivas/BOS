#include <cstdlib>
#include "physics/Simulation.h"
int main(int argc, char** argv) 
{
    Simulation<3> sim;
    sim.readData("test_output_snapshot.json");
    return 0;
}

