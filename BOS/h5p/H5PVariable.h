/* 
 * File:   H5PVariable.h
 * Author: dducks
 *
 * Created on April 30, 2015, 1:25 PM
 */

#ifndef H5PVARIABLE_H
#define	H5PVARIABLE_H

#include "H5PContext.h"



template< typename T >
class H5PVariable {
    H5PContext& ctx;
    std::string varName;

    
public:
    H5PVariable(H5PContext& ctx, const char* name) : ctx(ctx), varName(name) {
        
    }
    
    T operator()() {
        
    }
    
    H5PVariable<T>& operator=(const T& t) {
        
    }
    
};


#endif	/* H5PVARIABLE_H */

