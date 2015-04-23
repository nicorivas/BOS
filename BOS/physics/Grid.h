/* 
 * File:   Grid.h
 * Author: nrivas
 *
 * Created on April 21, 2015, 1:26 PM
 */

#ifndef GRID_H
#define	GRID_H

#include "Vector.h"
#include "Particle.h"
#include "Event.h"
#include <array>
#include <cassert>
#include <limits>

template <unsigned int DIM>
class Grid {
    Vector<DIM> cellSize;
    std::array<unsigned int, DIM> nCells; // number of cells in each direction
    std::vector<unsigned int> list;
    unsigned int maxParticlesPerCell;
    unsigned long int numberOfCells;
private:
    unsigned int getExpectedListIndexFromPosition(Vector<DIM>* pos) {
        std::array<unsigned int, DIM> arg;
        for (int i = 0; i < DIM; i++) {
            arg[i] = (int)(std::round(pos[i]/cellSize[i]));
        }
        return hash(arg);
    }
public:
    Grid() { }
    
    Grid(const Grid& grid_) {
        cellSize = grid_.cellSize;
    }
    
    Grid(Vector<DIM> cellSize): cellSize(cellSize) {
        assert(cellSize.size() == DIM);
    }
    
    Vector<DIM> getCellSize() const {
        return cellSize;
    }
    
    Vector<DIM> getNCells() const {
        return nCells;
    }
    
    unsigned int getMaxParticlesPerCell() const {
        return maxParticlesPerCell;
    }
    
    unsigned long int getNumberOfCells() const {
        return numberOfCells;
    }
    
    // Compute the number of elements of the domain tessellation by 
    // n-orthotopes of sizes given by cellSize;
    int init(Vector<DIM>* domainOrigin, Vector<DIM>* domainEnd) {
           
        // This would need the size of the smallest particle.
        maxParticlesPerCell = 3;
        
        Vector<DIM> length;
        for (int i = 0; i < DIM; i++)
        {
            // abs because some fucker could create domains with negative regions
            length[i] = std::abs(domainEnd[i]-domainOrigin[i]);
        }
        
        // This is where we need the size of the domain.
        numberOfCells = 1;
        for (unsigned int i = 0; i < DIM; i++) {
            nCells[i] = (unsigned long int)(std::ceil(length[i]/cellSize[i]));
            numberOfCells *= nCells[i];
        }
        
        // We need this until we figure out a way to determine how many
        // entities are in each cell; this is not trivial due to hash collisions
        // and the list being contiguous.
        numberOfCells *= maxParticlesPerCell;
        
        list.assign(numberOfCells, std::numeric_limits<std::size_t>::max());
    }
    
    // Given an event of a collision of a particle with a cell boundary, 
    // change the cell to which the particle belongs to
    int moveParticle(Event* event) {
        // We could check this
        if (event->type == EventType.CELL_BOUNDARY)
        {
            // how the monkeys did we manage to do this
            return -1;
        };
        
        // Remove particle from the list
        unsigned int listIndex = getExpectedListIndexFromPosition(event.getParticle().getPosition());
        while (list[listIndex] != event.getParticle().getId() && listIndex < numberOfCells) {
            listIndex++;
        }
        list[listIndex] = std::numeric_limits<std::size_t>::max();

        // Update to the position where the event takes place!
        event.getParticle().advance(event.time)
        
        // This would work if it weren't because we could end up in the same
        // cell as before, due to numerical precision.
        unsigned int listIndex = getExpectedListIndexFromPosition(event.getParticle().getPosition());
        int c = 0; // collision counter
        while (list[listIndex] != std::numeric_limits<std::size_t>::max())
        {
            listIndex++;
            c++;
        }
        
        // We could test this
        if (c > maxParticlesPerCell)
        {
            // This either shouldn't be allowed, or something special
            // should be done, as inserting a link to other list oder so;
            return -1;
        }
        
        list[listIndex] = event.otherIdx;
        
        return 1;
    }
    
    // getNeighbors gives me a particle (pointer) and returns me a list of
    // particle (pointers) which are in the vicinity of particle's current cell
    std::vector<Particle<DIM>* > getNeighbors(Particle<DIM>* particle)
    {
        std::vector<Particle<DIM>* > neighbors;
        Vector<DIM> currentPosition = particle.getPosition();
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
                neighbors.push_back(list[listIndex])
                listIndex++;
                c++;
            }
        }
        return neighbors;
        /*
        int dx, dy, dz;
        int c;
        for (dx=-1; dx<=1; dx+=1) {
            for (dy=-1; dy<=1; dy+=1) {
                for (dz=-1; dz<=1; dz+=1) {
                    // should we ignore the {0,0,0} or just return also
                    // the particles in the same cell as the main particle?
                    int position = hash(ix+dx,iy+dy,iz+dz);
                    c = 0;
                    while (list[position] != 0 && c < maxParticlesPerCell) {
                        neighbors.push_back(list[position])
                        position++;
                        c++;
                    }
                }
            }
        }
        */
    }
    
    // Hash function where the keys are DIM-dimensional arrays of unsigned
    // ints (the indexes of cell to which the particle corresponds to), and
    // returns a unique id.
    int hash(std::array<unsigned int, DIM> in) {
        // something like this
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
};

#endif	/* GRID_H */

