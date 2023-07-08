/*******************************************
 *
 * $GAMGI/src/math/gamgi_math.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_MATH
#define GAMGI_MATH

enum { GAMGI_MATH_POLYTOPE_1, GAMGI_MATH_POLYTOPE_2,
GAMGI_MATH_POLYTOPE_3, GAMGI_MATH_POLYTOPE_4 };

/************* macros ************
 *********************************/

#define GAMGI_MATH_PI 3.14159265358979323846

/*******************************
 * Converts radians to degrees *
 *******************************/

#define GAMGI_MATH_RAD_DEG (180.0/GAMGI_MATH_PI)

/*******************************
 * Converts degrees to radians *
 *******************************/

#define GAMGI_MATH_DEG_RAD (GAMGI_MATH_PI/180.0)

/**************************************************
 * quaternions are rescaled after these rotations *
 **************************************************/

#define GAMGI_MATH_RENORM_COUNT 97

/******************************************
 * dynamic array: initial size, increment *
 ******************************************/

#define GAMGI_MATH_PLANE_SIZE 10
#define GAMGI_MATH_PLANE_STEP 10

/***********************************************
 * tolerance: defines a general default for a  *
 * safe threshold for float values around zero *
 *                                             *
 * cell tolerance: offset around cells, used   *
 * to check if an object is inside the cell    *
 *                                             *
 * vector tolerance: the length of a rotation  *
 * axis in group_modify must be larger than    *
 * this value, otherwise the result is flagged *
 * as undefined. The cell length parameters    *
 * in cell_create and cell_modify must be      *
 * larger than this value.                     *
 *                                             *
 * angle tolerance: the angles between cell    *
 * vectors must be larger than this value.     *
 ***********************************************/

#define GAMGI_MATH_TOLERANCE           1.0E-4
#define GAMGI_MATH_TOLERANCE_CELL      1.0E-4
#define GAMGI_MATH_TOLERANCE_STRUCTURE 1.0E-2
#define GAMGI_MATH_TOLERANCE_LENGTH    1.0E-4
#define GAMGI_MATH_TOLERANCE_ANGLE     1.0E-4
#define GAMGI_MATH_TOLERANCE_MASS      1.0E-4
#define GAMGI_MATH_TOLERANCE_ORBITAL   1.0E-8

#define GAMGI_MATH_TOLERANCE_DOT      1.0E-10

/******************************************
 * minimum length to represent directions *
 * minimum radius to represent circles    *
 *                                        *
 * minimum area to represent planes       *
 ******************************************/

#define GAMGI_MATH_DIRECTION_LENGTH 0.1
#define GAMGI_MATH_DIRECTION_AREA   0.1
#define GAMGI_MATH_PLANE_AREA    1.0E-4

/**********************************************
 * minimum length of the cross product of two *
 * plane vectors describing a direction range *
 **********************************************/

#define GAMGI_MATH_DIRECTION_RANGE 0.1

/***********************************
 * Number of decimal figures used  *
 * to show the various quantities  *
 * (for length,angle,charge,mass   *
 *  these values are configurable) *
 ***********************************/

#define GAMGI_MATH_DECIMAL_MAX         5
#define GAMGI_MATH_DECIMAL_LENGTH      4
#define GAMGI_MATH_DECIMAL_ANGLE       2
#define GAMGI_MATH_DECIMAL_MASS        4
#define GAMGI_MATH_DECIMAL_CHARGE      4
#define GAMGI_MATH_DECIMAL_OCCUPANCY   4
#define GAMGI_MATH_DECIMAL_COLOR       3
#define GAMGI_MATH_DECIMAL_SCALE       3
#define GAMGI_MATH_DECIMAL_ORDER       2
#define GAMGI_MATH_DECIMAL_COVALENCY   2
#define GAMGI_MATH_DECIMAL_SIZE        2
#define GAMGI_MATH_DECIMAL_VARIANCY    2
#define GAMGI_MATH_DECIMAL_ATTENUATION 2
#define GAMGI_MATH_DECIMAL_SHININESS   4
#define GAMGI_MATH_DECIMAL_ABUNDANCE   4
#define GAMGI_MATH_DECIMAL_DECAY       2
#define GAMGI_MATH_DECIMAL_DENSITY     2

/*************************************
 * starting seed in random processes *
 *************************************/

#define GAMGI_MATH_SEED 1

#endif
