#ifndef DATA_H
#define	DATA_H

#include "libs/rapidjson/prettywriter.h" // for stringify JSON
#include "libs/rapidjson/filereadstream.h"
#include "libs/rapidjson/document.h"

#include "physics/Simulation.h"
#include "physics/ParticleType.h"

//#include "Functionals/Snapshot.h"

//std::vector<FunctionalEvent> fes;

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
    // docJson is a dictionary. See rapidjson doc for specific structure.
    rapidjson::Document docJson;
    // object positions can be specified in other files, which we store here:
    rapidjson::Document docJsonConfig;
    
    std::vector< ParticleType > particleTypes; // See DEMDF format spec.
    
    Vector<2*DIM> domain;     // Vector with domain limits, for the grid
    int particleNumber;     // Total number of particles
    double syncTime;        // Period to call sync (see comments in sync funct.)
    
    // Hold them as strings, actually get initialized in Simulation::readData();
    std::string particleInteractionType;
    std::string wallInteractionType;
    
    // Necessary for grid generation.
    double maxParticleRadius;
    
    bool bodyForceExists;
    Vector<DIM> bodyForce;
    
    double time;            // Current (starting) time of the simulation
    double timeEnd;         // Total time of the simulation
    int eventCount;        // Count of number of events called
    int eventCountEnd;   // Number of events at which the simulation should stop
    
    Data(const std::string& filename, Simulation<DIM>& sim) : filename(filename)
    {
        using namespace rapidjson;
        
        std::cout << "Reading config file '" << filename << "'" << std::endl;
        
        std::stringstream sstr;
        std::ifstream in;
        in.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        // We read the file to a string.
        try {
            in.open(filename.c_str());
            /*
            if (!in.is_open()) {
                throw 
            }
            */
        } catch (std::ifstream::failure e) {
            std::cerr << "File was not open, probably wrong filename given: "
                      << "'" << filename << "'"
                      << std::endl;
            throw e;
        }
        sstr << in.rdbuf();
        docJson.Parse(sstr.str().c_str()); // This is the magic step, JSON->dic
        
        // Check if filetype is DEMDF.
        if (!(std::string(docJson["fileFormat"].GetString()) == "demdf")) {
            std::cerr << "Data format in " << filename << "is not 'demdf'" << std::endl;
            exit(0);
        };
        
        if (docJson.HasMember("time")) {
            time = docJson["time"].GetDouble();
        }
        if (docJson.HasMember("timeEnd")) {
            timeEnd = docJson["timeEnd"].GetDouble();
        }
        if (timeEnd < 0) timeEnd = std::numeric_limits<double>::max();
        if (docJson.HasMember("eventCount")) {
            eventCount = docJson["eventCount"].GetInt();
        }
        if (docJson.HasMember("eventCountEnd")) {
            eventCountEnd = docJson["eventCountEnd"].GetInt();
        }
        syncTime = 10000.0;
        
        if (docJson.HasMember("domain")) {
            const Value& jDomain = docJson["domain"];
            for (size_t d = 0; d < DIM; d++) {
                domain[d] = jDomain[0][d].GetDouble();
                domain[d+DIM] = jDomain[1][d].GetDouble();
            }
        }
        
        bodyForceExists = false;
        if (docJson.HasMember("bodyForce")) {
            const Value& jBodyForce = docJson["bodyForce"];
            for (size_t d = 0; d < DIM; d++) {
                if (jBodyForce[d].GetDouble())
                {
                    bodyForceExists = true;
                }
                bodyForce[d] = jBodyForce[d].GetDouble();
            }
        }
        
        if (docJson.HasMember("objectTypes")) {
            const Value& jObjectTypes = docJson["objectTypes"];
            for (SizeType ob = 0; ob < jObjectTypes.Size(); ob++) {
                if (std::string(docJson["objectTypes"][ob]["type"].GetString()) == "particle")
                {
                    ParticleType particleType(docJson["objectTypes"][ob]);
                    particleTypes.push_back(particleType);
                    if (particleTypes[particleTypes.size()-1].radius > maxParticleRadius)
                        maxParticleRadius = particleTypes[particleTypes.size()-1].radius;
                }
            }
        }
        
        // Strange way of doing it (reassigning), but data format makes sense.
        if (docJson.HasMember("interactions")) {
            const Value& jInteractions = docJson["interactions"];
            for (SizeType i = 0; i < jInteractions.Size(); i++) {
                if (std::string(docJson["interactions"][i]["name"].GetString()) == "particleElastic") {
                    particleInteractionType = std::string(docJson["interactions"][i]["name"].GetString());
                } else if (std::string(docJson["interactions"][i]["name"].GetString()) == "particleInelastic") {
                    particleInteractionType = std::string(docJson["interactions"][i]["name"].GetString());
                } else if (std::string(docJson["interactions"][i]["name"].GetString()) == "wallElastic") {
                    wallInteractionType = std::string(docJson["interactions"][i]["name"].GetString());
                } else if (std::string(docJson["interactions"][i]["name"].GetString()) == "wallInelastic") {
                    wallInteractionType = std::string(docJson["interactions"][i]["name"].GetString());
                } else {
                    std::cout << "Interaction " << docJson["interactions"][i]["name"].GetString() << " was ignored." << std::endl;
                }
            }
        } else {
            std::cerr << "Warning: interaction not found, using elastic" << std::endl;
            particleInteractionType = std::string("particleElastic");
            wallInteractionType = std::string("wallElastic");
        };
        
        if (docJson.HasMember("objects")) {
            const Value& jWalls = docJson["objects"]["walls"];
            for (SizeType w = 0; w < jWalls.Size(); w++) {
                Vector<DIM> normal;
                Vector<DIM> position;
                for (size_t d = 0; d < DIM; d++) {
                    normal[d] = jWalls[w]["normal"][d].GetDouble();
                    position[d] = jWalls[w]["point"][d].GetDouble();
                }
                sim.addWall({position,normal});
            }
        
            Value& jParticles = docJson["objects"]["particles"];
            if (jParticles.IsString()) {
                std::cout << "Reading external config file for particles '"
                        << jParticles.GetString() << "'" << std::endl;
                std::stringstream sstr_config;
                std::ifstream ifstream_config(jParticles.GetString());
                sstr_config << ifstream_config.rdbuf();
                docJsonConfig.Parse(sstr_config.str().c_str());
                jParticles = docJsonConfig["particles"];
            } 
            if (jParticles.IsArray()) {
                double px, py, pz, vx, vy, vz, ax, ay, az;
                int type;
                particleNumber = jParticles.Size();
                for (size_t p = 0; p < particleNumber; p++) {
                    px = jParticles[p][0].GetDouble();
                    py = jParticles[p][1].GetDouble();
                    pz = jParticles[p][2].GetDouble();
                    vx = jParticles[p][3].GetDouble();
                    vy = jParticles[p][4].GetDouble();
                    vz = jParticles[p][5].GetDouble();
                    if (bodyForceExists) {
                        ax = bodyForce[0];
                        ay = bodyForce[1];
                        az = bodyForce[2];
                    } else {
                        ax = 0.0;
                        ay = 0.0;
                        az = 0.0;
                    }
                    type = jParticles[p][6].GetInt();
                    sim.addParticle({{px, py, pz}, {vx, vy, vz}, {ax, ay, az}, 0.5, type});
                }
            } else {
                std::cerr << "Unrecognized data format for field objects.particles" << std::endl;
                exit(0);
            }
        }
    }
    
    void output(Simulation<DIM>& sim, std::string filename)
    {
        // A rapidjson pretty writer is much slower but readable.
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);        
        
        // Update variables in the Json document object:
        rapidjson::Value& jParticles = docJson["objects"]["particles"];
        for (auto p : sim.getParticles())
        {
            //std::cout << p << std::endl;
            //std::cout << "p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[2]="<<p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[2] << std::endl;
            int i = p.getID();
            jParticles[i][0].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[0]);
            jParticles[i][1].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[1]);
            jParticles[i][2].SetDouble(p.getCurrentPosition(sim.getCurrentTime()-p.getLocalTime())[2]);
            jParticles[i][3].SetDouble(p.getVelocity()[0]);
            jParticles[i][4].SetDouble(p.getVelocity()[1]);
            jParticles[i][5].SetDouble(p.getVelocity()[2]);
        }
        docJson["time"].SetDouble(sim.getCurrentTime());
        
        docJson.Accept(writer);
        std::ofstream out(filename);
        out << buffer.GetString();
    }
private:
};

#endif	/* DATA_H */