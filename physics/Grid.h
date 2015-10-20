/* 
 * File:   Grid.h
 * Author: nrivas
 *
 * Created on April 21, 2015, 1:26 PM
 */

#ifndef GRID_H
#define	GRID_H

#include "math/Plane.h"
#include "physics/Particle.h"
#include "physics/Event.h"

#include <array>
#include <cassert>
#include <limits>
#include <vector>

template <unsigned int DIM>
class Grid {
    /*
     * A grid is a collection of cells
     * 
     * the cell index coordinates are (ix, iy, iz) in the coordinates below:
     * 
     * first floor
     * -------------------------------------------------------
     * |2*nCells[0] + 0|2*nCells[0] + 1|2*nCells[0] + 2| ... |
     * -------------------------------------------------------
     * | nCells[0] + 0 | nCells[0] + 1 | nCells[0] + 2 | ... |
     * -------------------------------------------------------
     * |      0        |       1       |       2       | ... |
     * -------------------------------------------------------
     * 
     * k-floor
     * ------------------------------------------------
     * |nCells[0]*nCells[1]*k + 2*nCells[0] + 0 | ... |
     * ------------------------------------------------
     * |nCells[0]*nCells[1]*k + 1*nCells[0] + 0 | ... |
     * ------------------------------------------------
     * |nCells[0]*nCells[1]*k + 0*nCells[0] + 0 | ... |
     * ------------------------------------------------
     * 
     */
    Vector<DIM> domainOrigin;
    Vector<DIM> domainEnd;
    Vector<DIM> cellSize;                   // Sizes of the cells in each dir.
    std::array<unsigned int, DIM> nCells;   // Number of cells in each direction
    std::vector<unsigned int> list;         // Particles in each cell.
    std::vector<unsigned int> neighbors;    // Map of cellIndex to all neighbors
    std::vector<Plane<DIM> > planes;        // Planes for each cell (for intersection)
    unsigned int maxParticlesPerCell;
    unsigned long int numberOfCells;
    
private:
    
    // Hash function where the keys is a DIM-dimensional array of unsigned
    // ints (the indexes of cell to which the particle belongs to), and
    // returns a unique id.
    int hash(std::array<unsigned int, DIM> in) const 
    {
        int r = 0;
        int tmp;
        for (int i = 0; i < DIM; i++) {
            tmp = in[i];
            for (int j = 0; j < i; j++) {
                tmp *= nCells[j];
            }
            r += tmp;
        }
        return r;
        /*
        return ix +
               iy*nCells[0]*maxParticlesPerCell + 
               iz*nCells[0]*nCells[1]*maxParticlesPerCell*maxParticlesPerCell;
        */
    }
    
    int hash(int ix, int iy, int iz) const 
    {
        if (ix < 0 || ix >= nCells[0])
            return numberOfCells;
        if (iy < 0 || iy >= nCells[1])
            return numberOfCells;
        if (iz < 0 || iz >= nCells[2])
            return numberOfCells;

        int r = ix + nCells[0]*iy + nCells[0]*nCells[1]*iz;
        return r;
        /*
        return ix +
               iy*nCells[0]*maxParticlesPerCell + 
               iz*nCells[0]*nCells[1]*maxParticlesPerCell*maxParticlesPerCell;
        */
    }
    
    // Given a position in physical space, return an index (one dimensional),
    // of the corresponding position in list (cell index + number of particles).
    unsigned int getCellIndexFromPosition(Vector<DIM> pos) const 
    {
        std::array<unsigned int, DIM> arg;
        for (int i = 0; i < DIM; i++) {
            arg[i] = (int)(std::floor(pos[i]/cellSize[i]));
        }
        return hash(arg);
    }
    
public:
    
    Grid() { }
    
    Grid(const Grid& grid_) 
    {
        cellSize = grid_.cellSize;
    }
    
    Grid(Vector<DIM> dO, Vector<DIM> dE, Vector<DIM> cellSize):
        domainOrigin(dO), domainEnd(dE), cellSize(cellSize) 
    {
        //assert(cellSize.size() == DIM);
    }
    
    Vector<DIM> getCellSize() const 
    {
        return cellSize;
    }
    
    std::array<unsigned int, DIM> getNCells() const 
    {
        return nCells;
    }
    
    unsigned int getNCells(unsigned int i) const 
    {
        return nCells[i];
    }
    
    Plane<DIM> * getPlane(int index) 
    {
        return &planes[index];
    }
    
    unsigned int getMaxParticlesPerCell() const 
    {
        return maxParticlesPerCell;
    }
    
    unsigned long int getNumberOfCells() const 
    {
        return numberOfCells;
    }
    
    std::array<int, DIM> getIndexCoordsFromIndex(int cellIndex) 
    {
        std::array<int, DIM> ret;
        ret[2] = (int)(std::floor(cellIndex/(nCells[0]*nCells[1])));
        ret[1] = (int)(std::floor((cellIndex-nCells[0]*nCells[1]*ret[2])/nCells[0]));
        ret[0] = (int)(std::floor(cellIndex-nCells[0]*ret[1]-nCells[0]*nCells[1]*ret[2]));
        return ret;
    }
    
    // Compute the number of elements obtained by the domain tessellation with 
    // n-orthotopes of sizes given by cellSize;
    int init(std::vector< Particle<DIM> > * particles) 
    {
        // This would need the size of the smallest particle.
        maxParticlesPerCell = 4;
        // Determine number of cells in each direction
        Vector<DIM> length;
        for (int i = 0; i < DIM; i++) {
            // abs because some fucker could create domains with negative regions
            length[i] = std::abs((domainEnd)[i]-(domainOrigin)[i]);
        }
        // Compute total number of cells
        numberOfCells = 1;
        for (unsigned int i = 0; i < DIM; i++) {
            nCells[i] = (unsigned long int)(std::ceil(length[i]/cellSize[i]));
            numberOfCells *= nCells[i];
        }
        // Create planes (for computing intersections)
        if (DIM == 2) {
            std::cerr << "I'm sorry, your princess is in another dimension" << std::endl;
            exit(0);
        } else {
            Vector<DIM> pos, normal;
            for (unsigned int ix = 0; ix < nCells[0]; ix++) {
                pos = {ix*cellSize[0], 0*cellSize[1], 0*cellSize[2]};
                normal = {1.0, 0.0, 0.0};
                planes.emplace_back(pos, normal);
                pos = {(ix+1)*cellSize[0], 0*cellSize[1], 0*cellSize[2]};
                normal = {-1.0, 0.0, 0.0};
                planes.emplace_back(pos, normal);
            }
            for (unsigned int iy = 0; iy < nCells[1]; iy++) {
                pos = {0*cellSize[0], iy*cellSize[1], 0*cellSize[2]};
                normal = {0.0, 1.0, 0.0};
                planes.emplace_back(pos, normal);
                pos = {0*cellSize[0], (iy+1)*cellSize[1], 0*cellSize[2]};
                normal = {0.0, -1.0, 0.0};
                planes.emplace_back(pos, normal);
            }
            for (unsigned int iz = 0; iz < nCells[2]; iz++) {
                pos = {0*cellSize[0], 0*cellSize[1], iz*cellSize[2]};
                normal = {0.0, 0.0, 1.0};
                planes.emplace_back(pos, normal);
                pos = {0*cellSize[0], 0*cellSize[1], (iz+1)*cellSize[2]};
                normal = {0.0, 0.0, -1.0};
                planes.emplace_back(pos, normal);
            }
        }
        
        // We need this until we figure out a way to determine how many
        // entities are in each cell; this is not trivial due to hash collisions
        // and the list being contiguous.
        // I use specifically 'unsigned int' for the limit, as the size
        // type somehow didn't work (it gave different results in this line
        // and when comparing, just ahead in the while loop)
        // We add +1 because we want cells that are always empty, so that out
        // of bounds indexes point to this cell (three of them to simplify fors)
        list.assign((numberOfCells+1)*maxParticlesPerCell, std::numeric_limits<unsigned int>::max());
        
        // Determine neighbors
        for (unsigned int iz = 0; iz < nCells[2]; iz++) {
            for (unsigned int iy = 0; iy < nCells[1]; iy++) {
                for (unsigned int ix = 0; ix < nCells[0]; ix++) {
                    neighbors.push_back(hash(ix-1,iy-1,iz-1));
                    neighbors.push_back(hash(ix+0,iy-1,iz-1));
                    neighbors.push_back(hash(ix+1,iy-1,iz-1));
                    neighbors.push_back(hash(ix-1,iy+0,iz-1));
                    neighbors.push_back(hash(ix+0,iy+0,iz-1));
                    neighbors.push_back(hash(ix+1,iy+0,iz-1));
                    neighbors.push_back(hash(ix-1,iy+1,iz-1));
                    neighbors.push_back(hash(ix+0,iy+1,iz-1));
                    neighbors.push_back(hash(ix+1,iy+1,iz-1));
                    
                    neighbors.push_back(hash(ix-1,iy-1,iz+0));
                    neighbors.push_back(hash(ix+0,iy-1,iz+0));
                    neighbors.push_back(hash(ix+1,iy-1,iz+0));
                    neighbors.push_back(hash(ix-1,iy+0,iz+0));
                    neighbors.push_back(hash(ix+0,iy+0,iz+0));
                    neighbors.push_back(hash(ix+1,iy+0,iz+0));
                    neighbors.push_back(hash(ix-1,iy+1,iz+0));
                    neighbors.push_back(hash(ix+0,iy+1,iz+0));
                    neighbors.push_back(hash(ix+1,iy+1,iz+0));
                    
                    neighbors.push_back(hash(ix-1,iy-1,iz+1));
                    neighbors.push_back(hash(ix+0,iy-1,iz+1));
                    neighbors.push_back(hash(ix+1,iy-1,iz+1));
                    neighbors.push_back(hash(ix-1,iy+0,iz+1));
                    neighbors.push_back(hash(ix+0,iy+0,iz+1));
                    neighbors.push_back(hash(ix+1,iy+0,iz+1));
                    neighbors.push_back(hash(ix-1,iy+1,iz+1));
                    neighbors.push_back(hash(ix+0,iy+1,iz+1));
                    neighbors.push_back(hash(ix+1,iy+1,iz+1));
                }
            }
        }
        
        // Locate particles in cell
        unsigned int listIndex; // use 2 to avoid confusion
        for (Particle<DIM>& p : *particles) {
            unsigned int cellIndex = getCellIndexFromPosition(p.getPosition());
            p.setCellIndex(cellIndex);
            listIndex = cellIndex*maxParticlesPerCell;
            int c = 0; // collision counter
            while (list[listIndex+c] != std::numeric_limits<unsigned int>::max() && c < maxParticlesPerCell) {
                c++;
            }
            list[listIndex+c] = p.getID();
        }
        return 0;
    }
 
    int reset()
    {
        //nCells.clear();
        list.clear();
        neighbors.clear();
        planes.clear();
        maxParticlesPerCell = 0;
        numberOfCells = 0;
        return 0;
    }
    
    // Given an event of a collision of a particle with a cell boundary, 
    // change the cell to which the particle belongs to
    int moveParticle(Particle<DIM>& p, int planeId) 
    {
        unsigned int listIndex, cellIndex;
                
#ifdef DEBUG
        // Check if position and index make sense, for debugging
        /*
        if (getCellIndexFromPosition(p.getPosition()) != p.getCellIndex()) {
            std::cerr << "Error: particle cellIndex and Position don't match" << std::endl;
            std::cerr << p << std::endl;
            std::cerr << "p.getCellIndex()=" << p.getCellIndex() << std::endl;
            exit(0);
        }
        */
#endif
        
        // Remove particle from the list
        cellIndex = p.getCellIndex();
        listIndex = cellIndex*maxParticlesPerCell;
        unsigned int c = 0;
        while (list[listIndex+c] != p.getID() && c < maxParticlesPerCell) {
            c++;
        }
        if (c == maxParticlesPerCell) {
            std::cout << "We tried to remove an unfound particle" << std::endl;
            exit(0);
        }
        list[listIndex+c] = std::numeric_limits<unsigned int>::max();

        // Get direction of moving from wall normal
        Vector<DIM> dirs = {1.0, 1.0*nCells[0], 1.0*nCells[0]*nCells[1]};
        //std::cout << "cellIndex=" << cellIndex << std::endl;
        cellIndex = cellIndex + (int)(dot(planes[planeId].getNormal(),-dirs));
        //std::cout << "cellIndex=" << cellIndex << std::endl;
        
        p.setCellIndex(cellIndex);
        p.setCellWallIndex(planeId);
        
        // Add to list at proper position
        c = 0;
        listIndex = cellIndex*maxParticlesPerCell;
        while (list[listIndex+c] != std::numeric_limits<unsigned int>::max() && c < maxParticlesPerCell) {
            c++;
        }
        if (c == maxParticlesPerCell) {
            // This either shouldn't be allowed, or something special
            // should be done, as inserting a link to other list oder so;
            std::cerr << "A collision occurred in the list of cells" << std::endl;
            std::cout << "p=" << p << std::endl;
            std::cout << "planeId=" << planeId << std::endl;
            std::cout << "cellIndex=" << cellIndex << std::endl;
            std::cout << "listIndex=" << listIndex << std::endl;
            exit(0);
        }
        list[listIndex+c] = p.getID();
        
        return 1;
    }
    
    // getNeighbors, given a particle (pointer), returns a list of
    // particles (pointers) which are in the vicinity of particle's current cell
    std::vector<int> getNeighbors(Particle<DIM>* particle)
    {
        //std::cout << "\tgetNeighbors(Particle<DIM>* particle)" << std::endl;
        std::vector<int> neighborsIndex;       
        for (int i = 0; i < 27; i++) {
            int cell = neighbors[(particle->getCellIndex())*27+i];
            //std::cout << "\t cell=" << cell << std::endl;
            for (int c = 0; c < maxParticlesPerCell; c++) {
                if (list[cell*maxParticlesPerCell+c] != std::numeric_limits<unsigned int>::max()) {
                    neighborsIndex.push_back(list[cell*maxParticlesPerCell+c]);
                    //std::cout << "\t list[cell*maxParticlesPerCell+c="<<cell*maxParticlesPerCell+c<<"]=" << list[cell*maxParticlesPerCell+c] << std::endl;
                }
            }
        }
        return neighborsIndex;
    }
};

#endif	/* GRID_H */

