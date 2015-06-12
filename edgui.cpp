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

#include <chrono>
#include <thread>
#include <random>

int main(int argc, char** argv) {
    
    std::mt19937_64 gen(1);
    std::uniform_real_distribution<double> randDist(-1,1);
    
    Simulation<3> sim(1000.0, 10000.0);
    
    double spacing = 0.03;
    double radius = 0.5;
    int n = 0;
    int i = 0, j = 0, k = 0;
    double px, py, pz;
    int nmax = 16;
    double lx = 5.0;
    double ly = 5.0;
    double lz = 5.0;
    
    sim.addWall({{ 0, 0, 0},{ 1, 0, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 1, 0}});
    sim.addWall({{ 0, 0, 0},{ 0, 0, 1}});
    sim.addWall({{lx,ly,lz},{-1, 0, 0}});
    sim.addWall({{lx,ly,lz},{ 0,-1, 0}});
    sim.addWall({{lx,ly,lz},{ 0, 0,-1}});
    
    while (n < nmax) {
        px = radius+spacing+2*(radius+spacing)*i;
        py = radius+spacing+2*(radius+spacing)*j;
        pz = radius+spacing+2*(radius+spacing)*k;
        std::cout << px << " " << py << " " << pz << std::endl;
        i++;
        if (px+radius+spacing > lx) {
            i = 0;
            j++;
            continue;
        }
        if (py+radius+spacing > ly) {
            i = 0;
            j = 0;
            k++;
            continue;
        }
        if (pz+radius+spacing > lz) {
            std::cerr << "Particle don't fit inside walls" << std::endl;
            exit(1);
        }
        sim.addParticle({{px, py, 0.75}, {randDist(gen), randDist(gen), 0.0}, radius, 0});
        n++;
        //sim.addParticle({n, {2.5+randDist(gen), 2.5, 2.5+randDist(gen)}, {randDist(gen), 0.0, randDist(gen)}, 0.5, 0});
        //sim.addParticle({n, {2.5, 2.5, 0.75}, {0.0, 0.0, 1.0}, 0.5, 0});
    }
    
    std::cout << "Total number of particles=" << sim.getParticles().size() << std::endl;
    
    //sim.addParticle({{3.2, 2.5, 2.5}, {-0.4, 0.0, 0.0}, 0.5, 0});
    //sim.addParticle({{3.2, 3.8, 2.5}, { 0.0,-2.0, 0.0}, 0.5, 0});
    //sim.addParticle({{1.9, 2.5, 2.5}, {+0.5, 0.0, 0.0}, 0.5, 0});
    
    sim.createGrid({0.0,0.0,0.0},{lx,ly,lz},{radius*2.0,radius*2.0,radius*2.0});
    sim.run();

    return 0;
}

