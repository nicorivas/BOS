/* 
 * File:   HDF5.h
 * Author: dducks
 *
 * Created on February 23, 2015, 5:02 PM
 */

#ifndef HDF5_H
#define	HDF5_H

/**
 * A small guild to implementing HDF5 in your drivers / codes
 * 
 * 
 * 
 */


#include <hdf5.h>

#include "HDF5Safe.h"
#include "HDF5Type.h"

#include "HDF5Attribute.h"
#include "HDF5Group.h"

class HDF5File final : public HDF5Attributable<HDF5File>, public HDF5Groupable<HDF5File>
{
public:
    /*
     * \brief Creates or opens an MH5 file
     * In case of a creation, the file will also be populated with a default
     * structure which is required for Mercury simulations
     * 
     * \param filename the filename
     * \param create creates the file if it doesn't exist
     */
    HDF5File(std::string filename, bool create = false);
    
    /*
     * \brief true in case of a valid file
     * Returns true if and only if the file is valid and compatible with
     * this mercury version.
     * \return true in case of valid, false in all other cases
     */
    operator bool() const;
    
    
    /*
     * Returns the raw HID
     */
public:
    hid_t getHID() const;
    
    /*
     * \brief Destroys this HDF5File, closing any open references
     */
    ~HDF5File();
    
private:
    void setupInitialStructure();
    hid_t fileDescriptor_; 
};

#endif	/* HDF5_H */

