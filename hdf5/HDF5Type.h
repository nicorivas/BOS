/* 
 * File:   HDF5Type.h
 * Author: dducks
 *
 * Created on February 23, 2015, 5:02 PM
 */

#ifndef HDF5TYPE_H
#define	HDF5TYPE_H

#include <type_traits>

#define DECLARE_TYPE(TYPE) template<typename T> \
   typename std::enable_if<std::is_same<TYPE,T>::value, hid_t>::type \
   getTypeID()

#define EXPOSE_TYPE(TYPE,NAME) \
     class NAME ## _EXPOSE_DETAILS : public TYPE { \
       template <typename T> \
       friend typename std::enable_if<std::is_same<TYPE,T>::value, hid_t>::type \
       getTypeID(); \
     }

#define MAPPING_START(TYPE) template<typename T> \
   typename std::enable_if<std::is_same<TYPE,T>::value, hid_t>::type \
   getTypeID() { \
   using TYPEID = TYPE; \
   hid_t typeID = H5Tcreate(H5T_COMPOUND, sizeof(TYPE))

#define MAPPING_EXPOSE(TYPE,NAME) template<typename T> \
   typename std::enable_if<std::is_same<TYPE,T>::value, hid_t>::type \
   getTypeID() { \
   using TYPEID = NAME ## _EXPOSE_DETAILS; \
   hid_t typeID = H5Tcreate(H5T_COMPOUND, sizeof(TYPE))

#define FIELD(NAME,FIELDNAME) \
    do { \
      hid_t dtype = getTypeID<decltype(TYPEID::FIELDNAME)>(); \
      H5Tinsert(typeID, #NAME, offsetof(TYPEID,FIELDNAME), dtype); \
      H5Tclose(dtype); \
    } while (false)

#define FIELD_ARRAY(NAME,FIELDNAME,COUNT) \
    do { \
      hid_t dtype = getTypeID<decltype(TYPEID::FIELDNAME[COUNT])>(); \
      H5Tinsert(typeID, #NAME, offsetof(TYPEID,FIELDNAME) + sizeof(decltype(TYPEID::FIELDNAME[COUNT]))*COUNT], dtype); \
      H5Tclose(dtype); \
    } while (false)


#define MAPPING_END() return typeID; \
}

#include "TypeDeclarations.h"
#include "HDF5TypeMeta.h"
#include "TypeDefinitions.h"

#undef DECLARE_TYPE
#undef MAPPING_START
#undef FIELD
#undef MAPPING_END

#endif	/* HDF5TYPE_H */

