/* 
 * File:   Grid.h
 * Author: nrivas
 *
 * Created on April 21, 2015, 1:26 PM
 */

#ifndef GRID_H
#define	GRID_H

#include <math/Plane.h>
#include <physics/Particle.h>
#include <physics/Event.h>
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
    Vector<DIM> cellSize;                   // Sizes of the cells in each dir.
    std::array<unsigned int, DIM> nCells;   // Number of cells in each direction
    std::vector<unsigned int> list;         // Particles in each cell.
    std::vector<Plane<DIM> > planes;        // Planes for each cell (for intersection)
    unsigned int maxParticlesPerCell;
    unsigned long int numberOfCells;
private:
    
    // Given a position in physical space, return an index (one dimensional),
    // of the corresponding position in list (cell index + number of particles).
    unsigned int getCellIndexFromPosition(Vector<DIM> pos) const {
        std::array<unsigned int, DIM> arg;
        for (int i = 0; i < DIM; i++) {
            arg[i] = (int)(std::floor(pos[i]/cellSize[i]));
            std::cout << arg[i] << std::endl;
        }
        return hash(arg);
    }
        
    // Hash function where the keys is a DIM-dimensional array of unsigned
    // ints (the indexes of cell to which the particle corresponds to), and
    // returns a unique id.
    int hash(std::array<unsigned int, DIM> in) const {
        int r = 0;
        int tmp;
        for (int i = 0; i < DIM; i++) {
            tmp = in[i];
            for (int j = 0; j < i; j++) {
                tmp *= nCells[j]*maxParticlesPerCell;
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
public:
    Grid() { }
    
    Grid(const Grid& grid_) {
        cellSize = grid_.cellSize;
    }
    
    Grid(Vector<DIM> cellSize): cellSize(cellSize) {
        //assert(cellSize.size() == DIM);
    }
    
    Vector<DIM> getCellSize() const {
        return cellSize;
    }
    
    std::array<unsigned int, DIM> getNCells() const {
        return nCells;
    }
    
    unsigned int getNCells(unsigned int i) const {
        return nCells[i];
    }
    
    Plane<DIM> * getPlane(int index) {
        return &planes[index];
    }
    
    unsigned int getMaxParticlesPerCell() const {
        return maxParticlesPerCell;
    }
    
    unsigned long int getNumberOfCells() const {
        return numberOfCells;
    }
    
    std::array<int, DIM> getIndexCoordsFromIndex(int cellIndex) {
        std::array<int, DIM> ret;
        ret[2] = (int)(std::floor(cellIndex/(nCells[0]*nCells[1])));
        ret[1] = (int)(std::floor((cellIndex-nCells[0]*nCells[1]*ret[2])/nCells[0]));
        ret[0] = (int)(std::floor(cellIndex-nCells[0]*ret[1]-nCells[0]*nCells[1]*ret[2]));
        return ret;
    }
    
    // Compute the number of elements obtained by the domain tessellation with 
    // n-orthotopes of sizes given by cellSize;
    int init(Vector<DIM> * domainOrigin, Vector<DIM> * domainEnd,
            std::vector< Particle<DIM> > * particles) {
           
        // This would need the size of the smallest particle.
        maxParticlesPerCell = 3;
        
        // Determine number of cells in each direction
        Vector<DIM> length;
        for (int i = 0; i < DIM; i++) {
            // abs because some fucker could create domains with negative regions
            length[i] = std::abs((*domainEnd)[i]-(*domainOrigin)[i]);
        }
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
        // numberOfCells *= maxParticlesPerCell;
        
        // I use specifically 'unsigned int' for the limit, as the size
        // type somehow didn't work (it gave different results in this line
        // and when comparing, just ahead in the while loop)
        //list.assign(numberOfCells, std::numeric_limits<unsigned int>::max());
        list.assign(numberOfCells*maxParticlesPerCell, std::numeric_limits<unsigned int>::max());
        
        // Locate particles in cell
        unsigned int listIndex; // use 2 to avoid confusion
        for (Particle<DIM>& p : *particles) {
            //std::cout << p.getPosition() << std::endl;
            unsigned int cellIndex = getCellIndexFromPosition(p.getPosition());
            p.cellIndex = cellIndex;
            listIndex = cellIndex;
            int c = 0; // collision counter
            while (list[listIndex] != std::numeric_limits<unsigned int>::max()) {
                listIndex++;
                c++;
            }
            list[listIndex] = p.getID();
        }
        return 0;
    }
    
    // Given an event of a collision of a particle with a cell boundary, 
    // change the cell to which the particle belongs to
    int moveParticle(Event* event) {
        
        unsigned int listIndex, cellIndex;
        
        // Remove particle from the list
        cellIndex = event->getParticle<DIM>()->cellIndex;
        listIndex = cellIndex;
        while (list[listIndex] != event->getParticle<DIM>()->getID() && listIndex < numberOfCells*maxParticlesPerCell) {
            listIndex++;
        }
        list[listIndex] = std::numeric_limits<std::size_t>::max();

        // Get direction of moving from wall normal
        Vector<DIM> dirs = {1.0, 1.0*nCells[0], 1.0*nCells[0]*nCells[1]};
        std::cout << "listIndex=" << listIndex << std::endl;
        cellIndex = cellIndex + (int)(dot(planes[event->otherIdx].getNormal(),-dirs));
        std::cout << "listIndex=" << listIndex << std::endl;
        
        event->getParticle<DIM>()->cellIndex = cellIndex;
        
        // Add to list at proper position
        int c = 0;
        listIndex = cellIndex;
        while (list[listIndex] != std::numeric_limits<unsigned int>::max()) {
            listIndex++;
            c++;
        }
        if (c > maxParticlesPerCell) {
            // This either shouldn't be allowed, or something special
            // should be done, as inserting a link to other list oder so;
            std::cerr << "A collision occurred in the list of cells" << std::endl;
            return -1;
        }
        
        list[listIndex] = event->getParticle<DIM>()->getID();
        
        return 1;
    }
    
    // getNeighbors, given a particle (pointer), returns a list of
    // particles (pointers) which are in the vicinity of particle's current cell
    std::vector<Particle<DIM>* > getNeighbors(Particle<DIM>* particle)
    {
        std::vector<Particle<DIM>* > neighbors;
        Vector<DIM> currentPosition = particle->getPosition();
        std::array<unsigned int, DIM> in;
        
        for (unsigned int i = 0; i < DIM; i++)
        {
            in[i] = (int)(std::round(currentPosition[i]/cellSize[i]));
        }
        
        // the 26(27) directions
        std::array<std::array<int, DIM>, 3> dira;
        for (int dim = 0; dim < DIM; dim++) {
            for (int dir = -1; dir <= 1; dir++) {
                dira[dim][dir+1] = dir;
            }
        }
        
        std::array<unsigned int, DIM> position;
        for (int i = 0; i < 3; i++) // 3 directions
        {
            for (int j = 0; j < DIM; j++)
            {
                position[j] = in[j]+dira[j][i];
            }
            int listIndex = hash(position);
            int c = 0;
            while (list[listIndex] != 0 && c < maxParticlesPerCell) {
                neighbors.push_back(list[listIndex]);
                listIndex++;
                c++;
            }
        }
        return neighbors;
    }
};

#endif	/* GRID_H */

