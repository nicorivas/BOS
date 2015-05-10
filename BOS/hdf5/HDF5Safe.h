/* 
 * File:   HDF5Safe.h
 * Author: dducks
 *
 * Created on February 26, 2015, 9:05 PM
 */

#ifndef HDF5SAFE_H
#define	HDF5SAFE_H

#include <utility>

class HDF5Attribute
{
public:
    HDF5Attribute (hid_t id) : attrID_(id) { }
    HDF5Attribute (const HDF5Attribute&) = delete;
    HDF5Attribute (HDF5Attribute&& other) : attrID_(other.attrID_)
    {
        other.attrID_ = 0;
    }
    HDF5Attribute& operator =(const HDF5Attribute&) = delete;
    HDF5Attribute& operator =(HDF5Attribute&& other)
    {
        std::swap(attrID_, other.attrID_);
        return *this;
    }

    ~HDF5Attribute()
    {
        if (attrID_ > 0)
            H5Aclose(attrID_);
    }

    operator hid_t() const
    {
        return attrID_;
    }

private:
    hid_t attrID_;
};

class HDF5Space
{
public:
    HDF5Space (hid_t id) : spaceID_(id) { }
    HDF5Space (const HDF5Space&) = delete;
    HDF5Space (HDF5Space&& other) : spaceID_(other.spaceID_)
    {
        other.spaceID_ = 0;
    }
    HDF5Space& operator =(const HDF5Space&) = delete;
    HDF5Space& operator =(HDF5Space&& other)
    {
        std::swap(spaceID_, other.spaceID_);
        return *this;
    }

    ~HDF5Space()
    {
        if (spaceID_ > 0)
            H5Sclose(spaceID_);
    }

    operator hid_t() const
    {
        return spaceID_;
    }

private:
    hid_t spaceID_;
};

class HDF5Type
{
public:
    HDF5Type (hid_t id) : typeID_(id) { }
    HDF5Type (const HDF5Type&) = delete;
    HDF5Type (HDF5Type&& other) : typeID_(other.typeID_)
    {
        other.typeID_ = 0;
    }
    HDF5Type& operator =(const HDF5Type&) = delete;
    HDF5Type& operator =(HDF5Type&& other)
    {
        std::swap(typeID_, other.typeID_);
        return *this;
    }

    ~HDF5Type()
    {
        if (typeID_ > 0)
            H5Tclose(typeID_);
    }

    operator hid_t() const
    {
        return typeID_;
    }

private:
    hid_t typeID_;
};
#endif	/* HDF5SAFE_H */

