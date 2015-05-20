#include <GL/glew.h>

#include <math/Vector.h>
#include <hg/MercuryData.h>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <map>
#include <array>

#define NAVG 16

int main(int argc, char** argv) {
    
    if (argc != 3 || std::string(argv[1]) == std::string(argv[2])) {
        std::cerr << "Usage: " << argv[0] << " [infile] [outfile]"
                << "\nWhere infile is a Mercury3D data file,"
                << "\noutfile will be a modified Mercury3D data file,"
                << "and outfile is not infile." << std::endl;
    }
    
    MercuryDataFile file(argv[1]);
    if (!file.isMercuryDataFile<3>()) {
        std::cerr << argv[1] << " doesn't seem to be a mercury data file..." << std::endl;
    }
    
    std::ofstream outfile(argv[2]);
    if (!outfile) {
        std::cerr << argv[2] << " can't be opened for writing" << std::endl;
    }
    
    std::map<std::size_t,std::array<Vector<3>,NAVG> > avgMap;
    
    std::size_t avgIdx = 0;
    std::size_t avgCnt = 1;    
    
    
    for (MercuryTimeStep<3> ts : file.as<3>()) {
        outfile << ts.getNumberOfParticles() << ' ' 
                << ts.getTime() << ' '
                << ts.getMin()[0] << ' '
                << ts.getMin()[1] << ' '
                << ts.getMin()[2] << ' '
                << ts.getMax()[0] << ' '
                << ts.getMax()[1] << ' '
                << ts.getMax()[2] << ' ' << std::endl;
                
        std::size_t partID = 0;
        for (MercuryParticle<3> part : ts) {
            avgMap[partID][avgIdx] = Vector<3>{part.velocity[0], part.velocity[1], part.velocity[2]};
            
         
            
            std::size_t max = (avgCnt < NAVG) ? avgCnt : NAVG;
            
            Vector<3> value = {0,0,0};
            for (std::size_t i = 0; i < max; i++) {
                value += avgMap[partID][i];
            }
            value /= max;
            
            part.velocity[0] = value[0];
            part.velocity[1] = value[1];
            part.velocity[2] = value[2];
            
            outfile << part.position[0] << ' ' << part.position[1] << ' ' << part.position[2] << ' ';
            outfile << part.velocity[0] << ' ' << part.velocity[1] << ' ' << part.velocity[2] << ' ';
            
            outfile << part.radius << ' ';
            
            outfile << part.rotation[0] << ' ' << part.rotation[1] << ' ' << part.rotation[2] << ' ';
            outfile << part.angularV[0] << ' ' << part.angularV[1] << ' ' << part.angularV[2] << ' ';
            
            outfile << part.speciesID << '\n';
            
            partID++;
            
        }
        
        std::cout << ts.getTime() << std::endl;

        avgIdx++;        
        if (avgIdx == NAVG) {
            avgIdx = 0;
        }
        
        avgCnt++;
    }
    outfile << std::endl;
    
}