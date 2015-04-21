/* 
 * File:   Wall.h
 * Author: dducks
 *
 * Created on April 20, 2015, 10:12 PM
 */

#ifndef WALL_H
#define	WALL_H

#include <math/Plane.h>

template< unsigned int DIM >
using Wall = Plane<DIM>;

#endif	/* WALL_H */

