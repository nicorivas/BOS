/* 
 * File:   Data.h
 * Author: nicorivas
 *
 * Created on June 13, 2015, 11:23 AM
 */

#ifndef DATA_H
#define	DATA_H

#include <libs/rapidjson/prettywriter.h> // for stringify JSON
#include <libs/rapidjson/filereadstream.h>
#include <libs/rapidjson/document.h>

#include <physics/Models.h>

#include <sstream>
#include <fstream>
#include <random>
#include <map>

template<unsigned int DIM>
class Simulation;

template<unsigned int DIM>
class Data {
public:
    const std::string filename;
    rapidjson::Document docJson;
    
    Vector<DIM> domain;     // Vector with domain limits, for the grid
    int particleNumber;     // Total number of particles
    double particleRadius;  // Radii of the spherical (or disk) particles
    double syncTime;        // Period to call sync (see comments in sync funct.)
    
    double timeEnd;         // Total time of the simulation
    int eventCount;        // Count of number of events called
    int eventCountEnd;   // Number of events at which the simulation should stop
    
    int outputCount;            // Times output has been called, for the filename
    std::string outputFilename; // Filename for output files
    
    Data(const std::string& filename, Simulation<DIM>& sim) :
            filename(filename)
    {
        using namespace rapidjson;
        std::stringstream sstr;

        // We read the file to a string.
        std::ifstream in(filename.c_str());
        sstr << in.rdbuf();
        docJson.Parse(sstr.str().c_str());
        
        // First we check if filetype is DEMDF.
        std::cout << docJson["fileFormat"].GetString() << std::endl;
        if (!(std::string(docJson["fileFormat"].GetString()) == "demdf")) {
            std::cerr << "Data format in " << filename << "is not 'demdf'";
            exit(0);
        };
        
        outputFilename = std::string(docJson["output"][0]["filename"].GetString());
        outputCount = docJson["output"][0]["saveCount"].GetInt();
        
        timeEnd = docJson["timeEnd"].GetDouble();
        eventCount = docJson["eventCount"].GetInt();
        eventCountEnd = docJson["eventCountEnd"].GetInt();
        syncTime = 10000.0;
        
        const Value& jDomain = docJson["domain"];
        for (size_t d = 0; d < DIM; d++) {
            domain[d] = jDomain[1][d].GetDouble();
        }

        assert(docJson.HasMember("walls"));
        const Value& jWalls = docJson["walls"];
        for (SizeType w = 0; w < jWalls.Size(); w++) {// Uses SizeType instead of size_t
            Vector<DIM> normal;
            Vector<DIM> position;
            for (size_t d = 0; d < DIM; d++) {
                normal[d] = jWalls[w]["normal"][d].GetDouble();
                position[d] = jWalls[w]["point"][d].GetDouble();
            }
            sim.addWall({position,normal});
        }
        
        // Adding physical models.
        sim.setParticleCollisionModel(particleElastic);
        sim.setWallCollisionModel(wallElastic);
        /* Models could also be defined as lambda functions, as:
        sim.setParticleCollisionModel([&](Particle<3>& pA, Particle<3>& pB) {
            Vector<3> d = pA.getPosition() - pB.getPosition();
            d /= sqrt(dot(d, d)); //normalise
            Vector<3> v = pA.getVelocity() - pB.getVelocity();
            pA.setVelocity(pA.getVelocity() - d * dot(v, d));
            pB.setVelocity(pB.getVelocity() + d * dot(v, d));
        });
        */
        
        config(sim);
    }
    
    void output(Simulation<DIM>& sim)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);        
        rapidjson::Value& jParticles = docJson["particles"];
        for (auto p : sim.getParticles())
        {
            int i = p.getID();
            jParticles[i][0].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[0]);
            jParticles[i][1].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[1]);
            jParticles[i][2].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[2]);
            jParticles[i][3].SetDouble(p.getVelocity()[0]);
            jParticles[i][4].SetDouble(p.getVelocity()[1]);
            jParticles[i][5].SetDouble(p.getVelocity()[2]);
        }
        docJson["time"].SetDouble(sim.getCurrentTime());
        docJson["output"][0]["saveCount"].SetInt(outputCount);
        
        docJson.Accept(writer);
        std::ofstream out(outputFilename+"_"+std::to_string(outputCount)+".json");
        out << buffer.GetString();
        outputCount++;
    }
    
private:
    
    void config(Simulation<DIM>& sim) {
        // Particles from JSON
        using namespace rapidjson;
        const Value& jParticles = docJson["particles"];
        double px, py, pz, vx, vy, vz;
        particleNumber = jParticles.Size();
        particleRadius = 0.5;
        for (size_t p = 0; p < particleNumber; p++) {
            px = jParticles[p][0].GetDouble();
            py = jParticles[p][1].GetDouble();
            pz = jParticles[p][2].GetDouble();
            vx = jParticles[p][3].GetDouble();
            vy = jParticles[p][4].GetDouble();
            vz = jParticles[p][5].GetDouble();
            sim.addParticle({{px, py, pz}, {vx, vy, vz}, particleRadius});
        }
    }

};

#endif	/* DATA_H */

