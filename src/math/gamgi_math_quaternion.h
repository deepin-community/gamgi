/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_quaternion.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_math_quaternion_normalize (double q[4]);

void gamgi_math_quaternion_add (double *q1, double *q2, double *out);

void gamgi_math_quaternion_from_axis (double phi, double axis[3], double quat[4]);

void gamgi_math_quaternion_to_axis (double quat[4], double *phi, double axis[3]);

void gamgi_math_quaternion_from_matrix (double m[9], double q[4]);

void gamgi_math_quaternion_to_matrix (double q[4], double m[9]);

void gamgi_math_quaternion_to_matrix4 (double q[4], double m[16]);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
