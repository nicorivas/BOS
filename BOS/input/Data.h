/* 
 * File:   Data.h
 * Author: nicorivas
 *
 * Created on June 13, 2015, 11:23 AM
 */

#ifndef DATA_H
#define	DATA_H

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>

#include <sstream>
#include <fstream>
#include <random>
#include <map>

template<unsigned int DIM>
class Simulation;

template<unsigned int DIM>
class Data {
public:
    Vector<DIM> domain;     // Vector with domain limits, for the grid
    int particleNumber;     // Total number of particles
    double particleRadius;  // Radii of the spherical (or disk) particles
    double syncTime;        // Period to call sync (see comments in sync funct.)
    double endTime;         // Total time of the simulation
    rapidjson::Document docJson;
    Data(const std::string& file, Simulation<DIM>& sim)
    {
        using namespace rapidjson;
        std::stringstream sstr;

        // We read the file to a string.
        std::ifstream in(file.c_str());
        sstr << in.rdbuf();
        docJson.Parse(sstr.str().c_str());

        assert(docJson.HasMember("particleNumber"));
        particleNumber = docJson["particleNumber"].GetInt();
        
        assert(docJson.HasMember("particleRadius"));
        particleRadius = docJson["particleRadius"].GetDouble();
        
        assert(docJson.HasMember("length"));
        const Value& lengthJson = docJson["length"];
        for (size_t d = 0; d < DIM; d++) {
            domain[d] = lengthJson[d].GetDouble();
        }
        std::cout << domain << std::endl;

        assert(docJson.HasMember("boundaries"));
        const Value& boundariesJson = docJson["boundaries"];
        assert(boundariesJson.IsArray());
        for (SizeType i = 0; i < boundariesJson.Size(); i++) {// Uses SizeType instead of size_t
            Vector<DIM> normal;
            Vector<DIM> position;
            for (size_t d = 0; d < DIM; d++) {
                normal[d] = boundariesJson[i]["normal"][d].GetDouble();
                position[d] = boundariesJson[i]["position"][d].GetDouble();
            }
            sim.addWall({position,normal});
        }
        
        assert(docJson.HasMember("config"));
        
        syncTime = 10000.0;
        endTime = 100.0;
        
        config(sim);
    }
    
    void config(Simulation<DIM>& sim) {
        std::mt19937_64 gen(1);
        std::uniform_real_distribution<double> randDist(-1,1);
        double radius = particleRadius;
        int n = 0;
        int i = 0, j = 0, k = 0;
        double px, py, pz;
        double spacing = 0.03;
        while (n < particleNumber) {
            px = radius+spacing+2*(radius+spacing)*i;
            py = radius+spacing+2*(radius+spacing)*j;
            pz = radius+spacing+2*(radius+spacing)*k;
            //std::cout << px << " " << py << " " << pz << std::endl;
            i++;
            if (px+radius+spacing > domain[0]) {
                i = 0;
                j++;
                continue;
            }
            if (py+radius+spacing > domain[1]) {
                i = 0;
                j = 0;
                k++;
                continue;
            }
            if (pz+radius+spacing > domain[2]) {
                std::cerr << "Particle don't fit inside walls" << std::endl;
                exit(1);
            }
            //sim.addParticle({{px, py, 0.75}, {randDist(gen), randDist(gen), 0.0}, radius, 0});
            sim.addParticle({{px, py, pz}, {randDist(gen), randDist(gen), randDist(gen)}, radius, 0});
            n++;
        }
        //sim.addParticle({{3.2, 2.5, 2.5}, {-0.4, 0.0, 0.0}, 0.5, 0});
        //sim.addParticle({{3.2, 3.8, 2.5}, { 0.0,-2.0, 0.0}, 0.5, 0});
        //sim.addParticle({{1.9, 2.5, 2.5}, {+0.5, 0.0, 0.0}, 0.5, 0});
        std::cout << "Particles added=" << sim.getParticles().size() << std::endl;
    }
private:

};

#endif	/* DATA_H */

