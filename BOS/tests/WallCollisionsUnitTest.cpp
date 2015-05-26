#include <physics/Simulation.h>
#include <physics/Particle.h>
#include <physics/Wall.h>

#include <math/Intersection.h>

#include <iostream>

int main(int argc, char** argv) {
    Simulation<3> sim(0.10, 1000);
    double lx = 5.0;
    double ly = 5.0;
    double lz = 5.0;
    sim.addWall({{ 0, 0, 0},{ 1, 0, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 1, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 0, 1}});
    sim.addWall({{lx,ly,lz},{-1, 0, 0}});
    sim.addWall({{lx,ly,lz},{ 0,-1, 0}});
    sim.addWall({{lx,ly,lz},{ 0, 0,-1}});
    sim.addParticle({0, {1.1, 1.2, 1.3}, {0.0, 0.0, 0.2}, 0.5, 0});
    sim.run();
}
