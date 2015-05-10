/* 
 * File:   HDF5Attribute.h
 * Author: dducks
 *
 * Created on February 24, 2015, 5:28 PM
 */

#ifndef HDF5ATTRIBUTE_H
#define	HDF5ATTRIBUTE_H

#include <type_traits>
#include <string>
#include <vector>

#include "HDF5Safe.h"
#include "HDF5TypeMeta.h"

namespace H5 {
    namespace Detail {

        template<typename T>
        typename std::enable_if<H5::is_string_type<T>::value, void>::type
        writeAttribute(hid_t attrID, const T& buffer)
        {
            std::string buff = buffer;
            const char * buffSz = buff.c_str();
            HDF5Type memTypeID = /*H5::Detail::*/::getTypeID<T>();
            H5Awrite(attrID, memTypeID, &buffSz);
        }
        
        template<typename T>
        typename std::enable_if<!H5::is_array_type<T>::value &&
                                !H5::is_string_type<T>::value, void>::type
        writeAttribute(hid_t attrID, const T& buffer)
        {
            hid_t memTypeID = /*H5::Detail::*/::getTypeID<T>();
            H5Awrite(attrID, memTypeID, &buffer);
            H5Tclose(memTypeID);

        }

        template<typename T>
        typename std::enable_if<H5::is_array_type<T>::value &&
                                !H5::is_string_type<T>::value, void>::type
        writeAttribute(hid_t attrID, const T& buffer)
        {
            hid_t memTypeID = /*H5::Detail::*/::getTypeID<T>();
            H5Awrite(attrID, memTypeID, buffer.data());
            H5Tclose(memTypeID);

        }

        template<typename T>
        typename std::enable_if<!H5::is_array_type<T>::value
                               || H5::is_string_type<T>::value, hid_t>::type
        buildDataSpace(const T& t)
        {
            return H5Screate(H5S_SCALAR);
        }

        template<typename T>
        typename std::enable_if<H5::is_array_type<T>::value
                             && !H5::is_string_type<T>::value, hid_t>::type
        buildDataSpace(const T& t)
        {
            hid_t space = H5Screate(H5S_SIMPLE);
            hsize_t simple = t.size();
            H5Sset_extent_simple(space, 1, &simple, &simple);
            return space;
        }

    }
}

/**
 */
template<typename SUBCLASS>
class HDF5Attributable
{
    static constexpr char const * up = "HDF5Exception";
    
    struct OPDataT {
        std::vector<std::string> names;
    };
    
    static herr_t iterate(hid_t lid, const char * name, const H5A_info_t * info, void * op_data) {
        OPDataT& data = *((OPDataT*)op_data);
        
        data.names.emplace_back(name);
        
        return 0;
    }
    
public:
    
    std::vector<std::string> getAttributeNames() const {
        OPDataT t;
        hsize_t idx = 0;
        H5Aiterate(((SUBCLASS*)this)->getHID(), H5_INDEX_NAME, H5_ITER_NATIVE,
              &idx, &HDF5Attributable<SUBCLASS>::iterate, (void*)&t );
        return std::move(t.names);
    }

    /**
     * Gets the attribute
     * @param name Name of the attribute
     * @return the opened attribute's value
     */
    
    template<typename T>
    typename std::enable_if<!H5::needs_conversion<T>::value,T>::type
    getAttribute(std::string name)
    {
        T buffer;

        //open the attribute by name
        HDF5Attribute attrID = H5Aopen(((SUBCLASS*) (this))->getHID(), name.c_str(), H5P_DEFAULT);

        //find it's space..
        HDF5Space space = H5Aget_space(attrID);
        if (!H5::checkAndResize(buffer, space))
            throw up;
        
        HDF5Type memTypeID = getTypeID<T>();
        H5Aread(attrID, memTypeID, H5::findDataPointer(buffer));
        return buffer;
    }
    
    template<typename T>
    typename std::enable_if<H5::needs_conversion<T>::value,T>::type
    getAttribute(std::string name)
    {
        using btype = typename H5::backing_type<T>::type;
        btype buffer;
        

        //open the attribute by name
        HDF5Attribute attrID = H5Aopen(((SUBCLASS*) (this))->getHID(), name.c_str(), H5P_DEFAULT);

        //find it's space..
        HDF5Space space = H5Aget_space(attrID);
        if (!H5::checkAndResize(buffer, space))
            throw up;
        
        HDF5Type memTypeID = getTypeID<btype>();
        H5Aread(attrID, memTypeID, H5::findDataPointer(buffer));
        return buffer;
    }

    /**
     * 
     * @param attr Adds an attribute
     */
    template<typename T>
    void setAttribute(std::string name, const T& value, bool replace = false)
    {
        //sanity checking if it already exists
        if (hasAttribute(name) && replace)
        {
            H5Adelete(((SUBCLASS*)this)->getHID(), name.c_str());
        }
        HDF5Space space = H5::Detail::buildDataSpace<T>(value);
        HDF5Type type = /*H5::Detail::*/getTypeID<T>();
        HDF5Attribute attr = H5Acreate2(((SUBCLASS*)this)->getHID(), name.c_str(), type, space, H5P_DEFAULT, H5P_DEFAULT);
        H5::Detail::writeAttribute(attr, value);
    }

    /**
     * Returns true if a parameter exists already
     * @param name
     */
    bool hasAttribute(std::string name)
    {
        int result = H5Aexists(((SUBCLASS*)this)->getHID(), name.c_str());
        return result > 0;
    }

    /**
     * 
     * @param name
     */
    void deleteAttribute(std::string name)
    {
        H5Adelete(((SUBCLASS*)this)->getHID(), name.c_str());
    }

} ;
#endif	/* HDF5ATTRIBUTE_H */

