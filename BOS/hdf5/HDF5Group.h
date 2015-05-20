/* 
 * File:   HDF5Group.h
 * Author: dducks
 *
 * Created on March 1, 2015, 6:44 PM
 */

#ifndef HDF5GROUP_H
#define	HDF5GROUP_H

#include "HDF5Attribute.h"
#include <hdf5.h>
#include <vector>
#include <string>

class HDF5Group;

template<typename SUBCLASS>
class HDF5Groupable
{
    struct OPDataT {
        std::vector<std::string> names;
    };
    
    static herr_t iterate(hid_t gid, const char * name, const H5L_info_t * info, void * op_data) {
        OPDataT& data = *((OPDataT*)op_data);
        
        data.names.emplace_back(name);
        
        return 0;
    }
    
public:

    HDF5Group getGroup(std::string name, bool create = true) const;


    HDF5Group deleteGroup(std::string name);
    
    bool hasGroup(std::string name) const
    {
        hid_t hid = H5Gopen2(((SUBCLASS*)this)->getHID(), name.c_str(), H5P_DEFAULT);
        if (hid > 0) {
            H5Gclose(hid);
        }
        return hid > 0;
    }
    
    std::vector<std::string> getGroupNames() const {
        OPDataT t;
        hsize_t idx = 0;
        H5Literate(((SUBCLASS*)this)->getHID(), H5_INDEX_NAME, H5_ITER_NATIVE,
              &idx, &HDF5Groupable<SUBCLASS>::iterate, (void*)&t );
        return std::move(t.names);
    }
} ;

class HDF5Group : public HDF5Groupable<HDF5Group>, public HDF5Attributable<HDF5Group>
{
//    template<typename T>
    friend class HDF5Groupable<HDF5Group>;

    HDF5Group(hid_t hid) : hid_(hid) { }

public:
    HDF5Group(const HDF5Group& other) = delete;

    HDF5Group(HDF5Group&& other) : hid_(other.hid_)
    {
        other.hid_ = -1;
    }
    HDF5Group& operator =(const HDF5Group& other) = delete;

    HDF5Group& operator =(HDF5Group&& other)
    {
        std::swap(hid_, other.hid_);
        return *this;
    }

    ~HDF5Group()
    {
        H5Gclose(hid_);
    }

    operator bool() const
    {
        return hid_ > 0;
    }

    hid_t getHID() const
    {
        return hid_;
    }
private:
    hid_t hid_;
} ;

template<typename SUBCLASS>
HDF5Group HDF5Groupable<SUBCLASS>::getGroup(std::string name, bool create) const
{
    hid_t hid = 0;
    if (!hasGroup(name) && create)
    {
        hid = H5Gcreate2(((SUBCLASS*) this)->getHID(), name.c_str(),
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    }
    else
    {
        hid = H5Gopen2(((SUBCLASS *) this)->getHID(), name.c_str(), H5P_DEFAULT);
    }

    HDF5Group group ( hid );
    return group;
}

#endif	/* HDF5GROUP_H */

