/* 
 * File:   Wall.h
 * Author: dducks
 *
 * Created on April 20, 2015, 10:12 PM
 */

#ifndef WALL_H
#define	WALL_H

#include <math/Plane.h>

/// A wall is just a plane.
/// (for now, as it may also include collision properties and movement)

template< unsigned int DIM >
using Wall = Plane<DIM>;

#endif	/* WALL_H */

