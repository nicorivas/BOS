#include <cstdlib>
#include "physics/Simulation.h"
int main(int argc, char** argv) 
{
    Simulation<3> simED1a;
    if (!simED1a.readData("")) {
        return 1;
    }
    Simulation<3> simED1b;
    if (!simED1b.readData("a")) {
        return 1;
    };
    return 0;
}

