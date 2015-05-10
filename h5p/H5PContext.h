/* 
 * File:   H5PContext.h
 * Author: dducks
 *
 * Created on April 30, 2015, 1:25 PM
 */

#ifndef H5PCONTEXT_H
#define	H5PCONTEXT_H

#include <hdf5/HDF5.h>

#include <string>
#include <set>

class H5PParameterMap {
    std::set<std::string> params;
    
public:
    H5PParameterMap(HDF5Group& group, std::string name)
      : H5PParameterMap(group, name.c_str()) { }
    
    H5PParameterMap(HDF5Group& group, const char* name) {
        HDF5Group g = group.getGroup(name,false);
    }
    
};

class H5PContext {
public:
    H5PContext(HDF5Group&& g) : group(std::move(g)) {  }
    
    HDF5Group group;
};

#endif	/* H5PCONTEXT_H */

