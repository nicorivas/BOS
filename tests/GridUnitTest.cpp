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
#include <iostream>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    std::mt19937_64 gen(0);
    std::normal_distribution<double> randDist(0,1);
    
    Vector<3> domainOrigin({0.0, 0.0, 0.0});
    Vector<3> domainEnd({5.0, 10.0, 15.0});
    Vector<3> cellSize({1.0, 1.0, 1.0});
    
    std::vector<Particle<3> > particles;
    
    particles.push_back({0, {0.5, 0.5, 0.5}, {randDist(gen), randDist(gen), randDist(gen)}, 0.5, 0});
    
    Grid<3> grid(cellSize);
    grid.init(&domainOrigin, &domainEnd, &particles);
    
    if (grid.getCellSize() == cellSize)
        std::cout << "grid.getCellSize() OK" << std::endl;
    else
        return 1;
    
    std::cout << "grid.getNCells() = (" << grid.getNCells()[0] << "," << grid.getNCells()[1] << "," << grid.getNCells()[2] << ")" << std::endl;
    std::cout << "grid.getNCells(n) = (" << grid.getNCells(0) << "," << grid.getNCells(1) << "," << grid.getNCells(2) << ")" << std::endl;
    unsigned int n;
    for (n = 0; n < grid.getNCells(0)*2+grid.getNCells(1)*2+grid.getNCells(2)*2; n++) {
        std::cout << "grid.getPlane() = (" << (*grid.getPlane(n)) << std::endl;
    }

    std::cout << "grid.getMaxParticlesPerCell() = " << grid.getMaxParticlesPerCell() << std::endl;
    
    std::cout << "grid.getNumberOfCells() = " << grid.getNumberOfCells();
    if (grid.getNumberOfCells() == grid.getNCells(0)*grid.getNCells(1)*grid.getNCells(2)) {
        std::cout << " OK" << std::endl;
    } else {
        std::cout << " Error" << std::endl;
        return 1;
    }
    
    for (n = 0; n < grid.getNumberOfCells(); n++) {
        std::cout << "(" <<
                grid.getIndexCoordsFromIndex(n)[0] << "," <<
                grid.getIndexCoordsFromIndex(n)[1] << "," <<
                grid.getIndexCoordsFromIndex(n)[2] << ")" << std::endl;
    }
    
    // Check getCellIndexFromPosition
    Vector<3> pos({0.1,0.1,0.1});
    int ci = 0;//grid.getCellIndexFromPosition(pos);
    std::cout << pos << "=" << ci << "=" << "(" <<
                grid.getIndexCoordsFromIndex(ci)[0] << "," <<
                grid.getIndexCoordsFromIndex(ci)[1] << "," <<
                grid.getIndexCoordsFromIndex(ci)[2] << ")" << std::endl;
            
    
    // Check if particles are correctly located
    ci = particles[0].cellIndex;
    std::cout << ci << "=" << "(" <<
                grid.getIndexCoordsFromIndex(ci)[0] << "," <<
                grid.getIndexCoordsFromIndex(ci)[1] << "," <<
                grid.getIndexCoordsFromIndex(ci)[2] << ")" << std::endl;
    
    return 0;
}

