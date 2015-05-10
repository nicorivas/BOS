/* 
 * File:   HDF5TypeMeta.h
 * Author: dducks
 *
 * Created on March 6, 2015, 8:24 PM
 */

#ifndef HDF5TYPEMETA_H
#define	HDF5TYPEMETA_H

#include <string>

//
// A warning to the poor warrior who reads this. This is the land of the
//   template witches. Thou may wander into this forest, but thou might not
//   make it out alive. Make sure to be well prepared with emergency SFINAE
//   battle-kits, which must include Clang, Gcc, a strong source of caffeine
//   and most likely nicotine.
//
// My thoughts go to thy, my poor fellow warrior
//
//              -- dducks
//
namespace H5 {
    // ------------[ Detail block for the array traits ]------------------------
    namespace Detail {
        template< typename T,
        typename = decltype(std::declval<T>().data()),
        typename = decltype(std::declval<T>().size())>
        static std::true_type isArrayType(T&& t)
        {
            return
            {
            };
        }

        static std::false_type isArrayType(...)
        {
            return
            {
            };
        }

        template< typename T,
        typename = decltype(std::declval<T>().data()),
        typename = decltype(std::declval<T>().size()),
        typename = decltype(std::declval<T>().resize(std::size_t()))>
        static std::true_type isResizableArrayType(T&& t)
        {
            return
            {
            };
        }

        static std::false_type isResizableArrayType(...)
        {
            return
            {
            };
        }
        
                
    }

    /**
     * Type trait for array types.
     * std::vector, std::string and std::array all qualify,
     * as do any type which implements the following function members
     * with the defined behaviours:
     * 
     * std::size_t T::size()  - returns the currently allocated element count
     * U*          T::data()  - returns a (non const) pointer to a continously
     *                           allocated memory element of the backing type
     */
    template< typename T > struct is_array_type :
    std::integral_constant<bool, decltype(Detail::isArrayType(std::declval<T>()))::value>
    {
    } ;

    /**
     * Type trait for resizable array types.
     * std::vector and std::string qualify
     * as do any type which implements the following function members
     * with the defined behaviours:
     * 
     * std::size_t T::size()  - returns the currently allocated element count
     * U*          T::data()  - returns a (non const) pointer to a continously
     *                           allocated memory element of the backing type
     * void        T::resize(std::size_t) - resizes the current array
     */
    template< typename T > struct is_resizable_array_type :
    std::integral_constant<bool, decltype(Detail::isResizableArrayType(std::declval<T>()))::value>
    {
    } ;
    
    /**
     * Type trait for C or C++ standard strings
     * This is often needed to detect the difference between strings
     * and actual containers of characters.
     */
    template< typename T > struct is_string_type :
    std::integral_constant<bool, std::is_same<std::string,T>::value ||
                                std::is_same<char const *,T>::value ||
                 (std::is_array<T>::value && 
                  std::is_same< typename std::remove_extent<T>::type,std::string::value_type>::value)>
    {
    };
    
    template< typename T > struct backing_type;
    // --------------[ Detail block for unwrapping types ]----------------------
    namespace Detail {
                
        template< typename T >
        static
        typename std::enable_if<!std::is_same<bool,T>::value &&
                 std::is_fundamental<T>::value, T>::type
          unpackBackingType(const T& t) {
            return t;
        }
        
        static unsigned char unpackBackingType(bool b) {
            return 0;
        }
        
        static const char * unpackBackingType(const std::string& str) {
            return nullptr;
        }
        
        template< template<typename, typename...> class C,
                  typename VT,
                  typename... OTHER,
                  typename T = C<VT,OTHER...> >
        static
        typename std::enable_if<!H5::is_string_type<T>::value &&
                                (!H5::is_array_type<VT>::value || H5::is_string_type<VT>::value),
                              C<typename ::H5::backing_type<VT>::type,OTHER...> >::type
          unpackBackingType(const C<VT,OTHER...>& t) {
            return std::declval<T>();
        }
        
        template< template<typename, std::size_t> class C,
                  typename VT,
                  std::size_t OTHER,
                  typename T = C<VT,OTHER> >
        static
        typename std::enable_if<!H5::is_string_type<T>::value &&
                                (!H5::is_array_type<VT>::value || H5::is_string_type<VT>::value),
                             C<typename ::H5::backing_type<VT>::type,OTHER> >::type
          unpackBackingType(const C<VT,OTHER>& t) {
            return std::declval<T>();
        }
        
        static
        void unpackBackingType(...) { };
        
        
    }
    
    /**
     * Retrieves the correct backing type for HDF5, which is required in case
     * of strings and other types which need an conversion step in order to
     * be rationally used by C++.
     */
    template< typename T > struct backing_type {
        using type = typename std::conditional<
                std::is_void<decltype(Detail::unpackBackingType(std::declval<T>()))>::value,
                T,
                decltype(Detail::unpackBackingType(std::declval<T>()))
                >::type;
    };
    
    /**
     * True if this function needs a conversion function to change the backing
     * type - we can't use the HDF5 conversion functions because of constructors
     * and C++ functionality...
     */
    template < typename T > struct needs_conversion
        : public std::integral_constant<bool,
            !std::is_same<T,typename backing_type<T>::type>::value> {
    };

    //Catch for fixed size arrays (std::array)    
    template < typename T >
    typename std::enable_if<H5::is_array_type<T>::value
                            && !H5::is_resizable_array_type<T>::value, bool>::type
    checkAndResize(T& t, const HDF5Space& space) {
        //So, we need to check that we're one-dimensional and correct size.
        H5S_class_t type = H5Sget_simple_extent_type(space);
        if (type == H5S_SIMPLE) {
            if (H5Sget_simple_extent_ndims(space) != 1)
                return false; //we're not 1D....
            
            hsize_t size, maxSize;
            H5Sget_simple_extent_dims(space, &size, &maxSize);
            if (t.size() != size)
                return false;
            return true;
        } else if (type == H5S_SCALAR) {
            if (t.size() != 1)
                return false;
            return true;
        }
        return false;
    }
    
    //Catch for dynamic size arrays (std::vector), resizes if possible!
    template < typename T >
    typename std::enable_if<H5::is_resizable_array_type<T>::value
                            && !H5::is_string_type<T>::value, bool>::type
    checkAndResize(T& t, const HDF5Space& space) {
        //So, we need to check that we're one-dimensional and resize when needed.
        H5S_class_t type = H5Sget_simple_extent_type(space);
        if (type == H5S_SIMPLE) {
            if (H5Sget_simple_extent_ndims(space) != 1)
                return false; //we're not 1D....
            
            hsize_t size, maxSize;
            H5Sget_simple_extent_dims(space, &size, &maxSize);
            if (t.size() != size)
                t.resize(size);
            return true;
        } else if (type == H5S_SCALAR) {
            if (t.size() != 1)
                t.resize(1);
            return true;
        }
        return false;
    }
    
    //Catch for primitives + strings, this can't resize
    template < typename T >
    typename std::enable_if<!H5::is_array_type<T>::value
                            || H5::is_string_type<T>::value,bool>::type
    checkAndResize(T& t, const HDF5Space& space) {
        //So, we need exactly one element. This is either a
        //H5S_SIMPLE space or a H5S_SCALAR
        H5S_class_t type = H5Sget_simple_extent_type(space);
        if (type == H5S_SIMPLE) {
            //1D?
            if (H5Sget_simple_extent_ndims(space) != 1)
                return false;
            
            //size 1?
            hsize_t size, maxSize;
            H5Sget_simple_extent_dims(space, &size, &maxSize);
            if (size != 1)
                return false;
        } else if (type == H5S_SCALAR) {
            return true;
        }
        return false;
    }
    
    
    /**
     * These functions return the correct backing data types...
     */
    template< typename T >
    typename std::enable_if<!H5::is_array_type<T>::value,T*>::type
    findDataPointer(T& t) {
        return &t;
    }
    
    template< typename T >
    typename std::enable_if<H5::is_array_type<T>::value,typename T::value_type*>::type
    findDataPointer(T& t) {
        return t.data();
    }

    //namespace Detail {
}
#define PRIMITIVE(TYPE,H5TYPE)        \
        template<typename T> \
        typename std::enable_if<std::is_same<TYPE,T>::value,hid_t>::type \
            getTypeID() { \
            return H5Tget_native_type(H5TYPE, H5T_DIR_DEFAULT); \
        }
        /* Ints */
        PRIMITIVE(signed short int, H5T_NATIVE_SHORT);
        PRIMITIVE(unsigned short int, H5T_NATIVE_USHORT);

        PRIMITIVE(signed int, H5T_NATIVE_INT);
        PRIMITIVE(unsigned int, H5T_NATIVE_UINT);

        PRIMITIVE(signed long int, H5T_NATIVE_LONG);
        PRIMITIVE(unsigned long int, H5T_NATIVE_ULONG);

        PRIMITIVE(signed long long int, H5T_NATIVE_LLONG)
        PRIMITIVE(unsigned long long int, H5T_NATIVE_ULLONG)

        /* Floats */
        PRIMITIVE(float, H5T_NATIVE_FLOAT);
        PRIMITIVE(double, H5T_NATIVE_DOUBLE);
        PRIMITIVE(long double, H5T_NATIVE_LDOUBLE);

        PRIMITIVE(bool, H5T_NATIVE_UCHAR);

#undef PRIMITIVE
        
        template<typename T>
        typename std::enable_if<H5::is_string_type<T>::value, hid_t>::type
        getTypeID()
        {
            hid_t typeID = H5Tcopy(H5T_C_S1);
            H5Tset_size(typeID, H5T_VARIABLE);
            return typeID;
        }
        
        template<typename T>
        typename std::enable_if<H5::is_array_type<T>::value &&
                                !H5::is_string_type<T>::value, hid_t>::type
        getTypeID()
        {
            return getTypeID<typename T::value_type > ();
        }
        
        template<typename T,
                 typename U = decltype(T::getTypeID())> \
        hid_t getTypeID()
        {
            return T::getTypeID();
        }
        
//    }
//}




#endif	/* HDF5TYPEMETA_H */

