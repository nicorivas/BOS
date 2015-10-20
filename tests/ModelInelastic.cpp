#include <cstdlib>
#include "physics/Simulation.h"
int main(int argc, char** argv) 
{
    Simulation<3> sim1;
    sim1.readData("test_inelastic_1p.json");
    sim1.run();

    Simulation<3> sim2;
    sim2.readData("test_inelastic_2p.json");
    sim2.run();

    // Homogeneous free cooling state
    Simulation<3> sim3;
    sim3.readData("test_inelastic_100p.json");
    sim3.run();
}

