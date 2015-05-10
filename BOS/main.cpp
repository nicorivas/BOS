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

#include <h5p/H5PSimulation.h>

#include <chrono>
#include <thread>

#include <random>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " filename simname [param=value] [param2=value] ...\n"
                << " - filename: MH5 file\n"
                << " - simname:  simulation definition"
                << " - param=value: Assign params required for the simulation" << std::endl;
        std::exit(1);
    }
    std::string filename = argv[1];
    std::string setupName = argv[2];

    std::mt19937_64 gen(0);
    std::normal_distribution<double> randDist(0, 1);

    Simulation<3> sim(0.10, 1000);


    Vector<3> v = {0.123, 1.234, 2.345};
    Particle<3> p = {0,
        {1, 2, 3},
        {3, 4, 5}, 6, 0};


    {
        H5PFile file(filename, true);
        H5PSetup setup = file.getSetup(setupName);

        Vector<3> min = setup.min();
        Vector<3> max = setup.max();
        std::cout << min << std::endl;
        std::cout << max << std::endl;
        sim.addWall({min, { 1, 0, 0}});
        sim.addWall({min, { 0, 1, 0}});
        sim.addWall({min, { 0, 0, 1}});
        sim.addWall({max, {-1, 0, 0}});
        sim.addWall({max, { 0,-1, 0}});
        sim.addWall({max, { 0, 0,-1}});

        return 0;

    }


    double spacing = 0.03;
    double radius = 0.5;
    int n = 0;
    int i = 0, j = 0, k = 0;
    double px, py, pz;
    int nmax = 100;
    double packing_fraction = 0.1;
    double lx = pow(nmax * 4.0 / 3.0 * M_PI * pow(radius + spacing, 3.0) / packing_fraction, 1.0 / 3.0)*1.0;
    double ly = lx * 1.0;
    double lz = lx * 1.0;

    std::cout << lx << std::endl;

    sim.addWall({
        { 0, 0, 0},
        { 1, 0, 0}});
    sim.addWall({
        { 0, 0, 0},
        { 0, 1, 0}});
    sim.addWall({
        { 0, 0, 0},
        { 0, 0, 1}});

    sim.addWall({
        {lx, ly, lz},
        {-1, 0, 0}});
    sim.addWall({
        {lx, ly, lz},
        { 0, -1, 0}});
    sim.addWall({
        {lx, ly, lz},
        { 0, 0, -1}});

    while (n < nmax) {
        px = radius + spacing + 2 * (radius + spacing) * i;
        py = radius + spacing + 2 * (radius + spacing) * j;
        pz = radius + spacing + 2 * (radius + spacing) * k;
        std::cout << px << " " << py << " " << pz << std::endl;
        i++;
        if (px + radius + spacing > lx) {
            i = 0;
            j++;
            continue;
        }
        if (py + radius + spacing > ly) {
            i = 0;
            j = 0;
            k++;
            continue;
        }
        if (pz + radius + spacing > lz) {
            std::cerr << "Particle don't fit inside walls" << std::endl;
            exit(1);
        }
        n++;
        std::cout << n << std::endl;
        sim.addParticle({n,
            {px, py, pz},
            {randDist(gen), randDist(gen), randDist(gen)}, 0.5, 0});
    }

    std::cout << sim.getParticles().size() << std::endl;

    sim.run();

    return 0;
}

