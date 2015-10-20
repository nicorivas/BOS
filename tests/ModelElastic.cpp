#include <cstdlib>
#include "physics/Simulation.h"
int main(int argc, char** argv) 
{
    if (true) {
        Simulation<3> sim1;
        if (!sim1.readData("test_elastic_1p.json")) {
            sim1.run();
            if (sim1.getEventCount() == sim1.getEventCountEnd()) {
                std::cout << "TEST ONE PARTICLE ELASTIC: FAILED" << std::endl;
            } else {
                std::cout << "TEST ONE PARTICLE ELASTIC: GOOD" << std::endl;
            }
        }
    }
    if (false) {
        Simulation<3> sim2;
        if (!sim2.readData("test_elastic_2p.json")) {
            sim2.run();
            if (sim2.getEventCount() == sim2.getEventCountEnd()) {
                std::cout << "TEST TWO PARTICLES ELASTIC: FAILED" << std::endl;
            } else {
                std::cout << "TEST TWO PARTICLES ELASTIC: GOOD" << std::endl;
            }
        }
    }
    if (false) {
        Simulation<3> sim3;
        if (!sim3.readData("test_elastic_100p.json")) {
            sim3.run();
            if (sim3.getEventCount() == sim3.getEventCountEnd()) {
                std::cout << "TEST 100 PARTICLES ELASTIC: FAILED" << std::endl;
            } else {
                std::cout << "TEST 100 PARTICLES ELASTIC: GOOD" << std::endl;
            }
        }
    }
}

