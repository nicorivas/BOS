/* 
 * File:   H5PSimulation.h
 * Author: dducks
 *
 * Created on April 30, 2015, 1:25 PM
 */

#ifndef H5PSIMULATION_H
#define	H5PSIMULATION_H

#include "H5PVariable.h"
#include "H5PContext.h"
#include <hdf5/HDF5.h>

#include <vector>
#include <string>


class H5PSetup
{
    H5PContext ctx;

public:
    H5PVariable< Vector<3> > min {ctx, "minimum"};
    H5PVariable< Vector<3> > max {ctx, "maximum"};

    H5PVariable< double > tEnd {ctx, "end_time"};
    H5PVariable< double > saveTime {ctx, "save_time"};
//    H5PParameterMap mapping {ctx, "parameters"};

    friend class H5PFile;
    H5PSetup(HDF5Group&& g) : ctx(std::move(g)) { }
    
public:
    
} ;

class H5PResult
{
    H5PContext ctx;
public:
    void saveTimestep(const std::vector<Particle<3> >& particles);
} ;


class H5PFile
{
    HDF5File file;
public:

    H5PFile(std::string fileName, bool create = false) : file(fileName, create) { }
    
    std::vector<std::string> getSetupNames() const {
        HDF5Group group = file.getGroup("setups", false);
        if (group) {
            return group.getGroupNames();
        } else {
            return { } ;
        }
    }
    H5PSetup getSetup(std::string name) {
        HDF5Group group = file.getGroup("setups", true);
        HDF5Group leaf = group.getGroup(name, true);
        return H5PSetup{std::move(leaf)};
    }
    
} ;


#endif	/* H5PSIMULATION_H */

