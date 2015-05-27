/* 
 * File:   GridUnitTest.cpp
 * Author: nicorivas
 *
 * Created on May 26, 2015, 10:35 PM
 */

//#include <physics/Simulation.h>
#include <physics/Grid.h>
#include <physics/Particle.h>
#include <math/Vector.h>

#include <cstdlib>
#include <random>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    std::mt19937_64 gen(0);
    std::normal_distribution<double> randDist(0,1);
    
    Vector<3> domainOrigin;
    Vector<3> domainEnd;
    std::vector<Particle<3> > particles;
    
    particles.push_back({0, {1.0, 1.0, 1.0}, {randDist(gen), randDist(gen), randDist(gen)}, 0.5, 0});
    
    Grid<3> grid({1.0,1.0,1.0});
    grid.init(&domainOrigin, &domainEnd, &particles);
    
    /*
    Simulation<3> sim(100,1000);
    
    double lx = 5.0;
    double ly = 5.0;
    double lz = 5.0;
    
    sim.addWall({{ 0, 0, 0},{ 1, 0, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 1, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 0, 1}});
    sim.addWall({{lx,ly,lz},{-1, 0, 0}});
    sim.addWall({{lx,ly,lz},{ 0,-1, 0}});
    sim.addWall({{lx,ly,lz},{ 0, 0,-1}});
    
    sim.addParticle({0,
        {randDist(gen)*4.5+0.5, randDist(gen)*4.5+0.5, randDist(gen)*4.5+0.5},
        {randDist(gen), randDist(gen), randDist(gen)},
        0.5, 0});
    */
    return 0;
}

