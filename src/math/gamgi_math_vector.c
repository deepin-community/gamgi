/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_vector.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"

/************ external function ************
 *                                         *
 *        GAMGI_MATH_VECTOR_ABSOLUTE       *
 *                                         *
 * Return a vector with coordinates x,y,z. *
 *                                         *
 *******************************************/

void gamgi_math_vector_absolute (double *v, double x, double y, double z)
{
v[0] = x;
v[1] = y;
v[2] = z;
}

/************** external function **************
 *                                             *
 *          GAMGI_MATH_VECTOR_RELATIVE         *
 *                                             *
 * Return a vector moved by coordinates x,y,z. *
 *                                             *
 ***********************************************/

void gamgi_math_vector_relative (double *v, double x, double y, double z)
{
v[0] += x;
v[1] += y;
v[2] += z;
}

/*********** external function **********
 *                                      *
 *        GAMGI_MATH_VECTOR_ZERO        *
 *                                      *
 * Return a vector initialised to zero. *
 *                                      *
 ****************************************/

void gamgi_math_vector_zero (double *v)
{
v[0] = 0.0;
v[1] = 0.0;
v[2] = 0.0;
}

/************** external function ***********
 *                                          *
 *           GAMGI_MATH_VECTOR_ADD          *
 *                                          *
 * Return the addition between two vectors. *
 *                                          *
 ********************************************/

void gamgi_math_vector_add (const double *v, const double *w, double *out)
{
out[0] = v[0] + w[0];
out[1] = v[1] + w[1];
out[2] = v[2] + w[2];
}

/**************** external function ************
 *                                             *
 *             GAMGI_MATH_VECTOR_SUB           *
 *                                             *
 * Return the subtraction between two vectors. *
 *                                             *
 ***********************************************/

void gamgi_math_vector_sub (const double *v, const double *w, double *out)
{
out[0] = v[0] - w[0];
out[1] = v[1] - w[1];
out[2] = v[2] - w[2];
}

/******** external function *******
 *                                *
 *    GAMGI_MATH_VECTOR_LENGTH    *
 *                                *
 * Return the length of a vector. *
 *                                *
 **********************************/

double gamgi_math_vector_length (const double *v)
{
return sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

/***************** external function *************
 *                                               *
 *               GAMGI_MATH_VECTOR_FMOD          *
 *                                               *
 * Return a vector reduced to the [-1,+1] range. *
 *                                               *
 *************************************************/

void gamgi_math_vector_fmod (const double *v, double *out)
{
out[0] = fmod (v[0], 1.0);
out[1] = fmod (v[1], 1.0);
out[2] = fmod (v[2], 1.0);
}

/******************* external function *************
 *                                                 *
 *               GAMGI_MATH_VECTOR_SCALE           *
 *                                                 *
 * Return a vector multiplied by a scaling factor. *
 *                                                 *
 ***************************************************/

void gamgi_math_vector_scale (const double *v, double *out, double scale)
{
out[0] = v[0] * scale;
out[1] = v[1] * scale;
out[2] = v[2] * scale;
}

/********* external function ********
 *                                  *
 *      GAMGI_MATH_VECTOR_NORMAL    *
 *                                  *
 *   Return a normalised vector.    *
 * The initial vector is destroyed! *
 *                                  *
 ************************************/

void gamgi_math_vector_normal (double *v)
{
gamgi_math_vector_scale (v, v, 1.0/gamgi_math_vector_length (v));
}

/****** external function *****
 *                            *
 *   GAMGI_MATH_VECTOR_COPY   *
 *                            *
 * Return a copy of a vector. *
 *                            *
 ******************************/

void gamgi_math_vector_copy (const double *v, double *out)
{
out[0] = v[0];
out[1] = v[1];
out[2] = v[2];
}

/************* external function **********
 *                                        *
 *          GAMGI_MATH_VECTOR_DOT         *
 *                                        *
 * Return the dot product of two vectors. *
 *                                        *
 ******************************************/

double gamgi_math_vector_dot (const double *v, const double *w)
{
return v[0]*w[0] + v[1]*w[1] + v[2]*w[2];
}

/************* external function ************
 *                                          *
 *          GAMGI_MATH_VECTOR_CROSS         *
 *                                          *
 * Return the cross product of two vectors. *
 *                                          *
 ********************************************/

void gamgi_math_vector_cross (const double *v, 
const double *w, double *out)
{
out[0] = (v[1] * w[2]) - (v[2] * w[1]);
out[1] = (v[2] * w[0]) - (v[0] * w[2]);
out[2] = (v[0] * w[1]) - (v[1] * w[0]);
}

/************* external function ************
 *                                          *
 *           GAMGI_MATH_VECTOR_MIX          *
 *                                          *
 * Return the mix product of three vectors. *
 *                                          *
 ********************************************/

double gamgi_math_vector_mix (const double *u, const double *v, const double *w)
{
return u[0] * ((v[1] * w[2]) - (v[2] * w[1])) +
       u[1] * ((v[2] * w[0]) - (v[0] * w[2])) + 
       u[2] * ((v[0] * w[1]) - (v[1] * w[0]));
}

/************ external function *********
 *                                      *
 *        GAMGI_MATH_VECTOR_ANGLE       *
 *                                      *
 * Return the angle betwen two vectors. *
 *                                      *
 ****************************************/

double gamgi_math_vector_angle (double *v1, double *v2)
{
double dot, d1, d2, cos;

dot = gamgi_math_vector_dot (v1, v2);
d1 = gamgi_math_vector_length (v1);
d2 = gamgi_math_vector_length (v2);

cos = dot/(d1 * d2);

/************************************************
 * This is needed because the limited precision *
 * obtained with floating point calculations    *
 * can produce impossible values for cosinus    *
 ************************************************/

if (cos > 1.0) cos = 1.0;
if (cos < -1.0) cos = -1.0;

return GAMGI_MATH_RAD_DEG * acos (cos);
}

void gamgi_math_vector_translation (double x, double y, 
double z, double *v1, double *v2, double *v3, double *out)
{
out[0] = x * v1[0] + y * v2[0] + z * v3[0];
out[1] = x * v1[1] + y * v2[1] + z * v3[1];
out[2] = x * v1[2] + y * v2[2] + z * v3[2];
}

/**************** external function *************
 *                                              *
 *          GAMGI_MATH_VECTOR_ORTHOGONAL        *
 *                                              *
 * Returns a vector orthogonal to input vector. *
 *                                              *
 ************************************************/

/*
void gamgi_math_vector_orthogonal (const double *u, double *v)
{
double u0, u1, u2;
double max;

u0 = fabs (u[0]);
u1 = fabs (u[1]);
u2 = fabs (u[2]);

max = u0;
if (u1 > max) max = u1;
if (u2 > max) max = u2;

if (max == u0)
  { v[0] = -u[1]; v[1] = u[0]; v[2] = 0; }
if (max == u1)
  { v[1] = -u[2]; v[2] = u[1]; v[0] = 0; }
if (max == u2)
  { v[2] = -u[0]; v[0] = u[2]; v[1] = 0; }
}
*/

void gamgi_math_vector_orthogonal (const double *u, const double *v, double *out)
{
double sub[3];
double u0, u1, u2;
double max;

gamgi_math_vector_sub (v, u, sub);

u0 = fabs (sub[0]);
u1 = fabs (sub[1]);
u2 = fabs (sub[2]);

max = u0;
if (u1 > max) max = u1;
if (u2 > max) max = u2;

if (max == u0)
  { out[0] = -sub[1]; out[1] = sub[0]; out[2] = 0; }
if (max == u1)
  { out[1] = -sub[2]; out[2] = sub[1]; out[0] = 0; }
if (max == u2)
  { out[2] = -sub[0]; out[0] = sub[2]; out[1] = 0; }

gamgi_math_vector_add (out, u, out);
}

/**************** external function *************
 *                                              *
 *              GAMGI_MATH_VECTOR_BOX           *
 *                                              *
 * Returns two vectors with minimum and maximum *
 * coordinates, defining a convex hull box      *
 *                                              *
 ************************************************/

void gamgi_math_vector_box (double *r, double *min, double *max)
{
if (r[0] < min[0]) min[0] = r[0];
if (r[0] > max[0]) max[0] = r[0];

if (r[1] < min[1]) min[1] = r[1];
if (r[1] > max[1]) max[1] = r[1];

if (r[2] < min[2]) min[2] = r[2];
if (r[2] > max[2]) max[2] = r[2];
}

/**************** external function *************
 *                                              *
 *             GAMGI_MATH_VECTOR_SWAP           *
 *                                              *
 *      swap the coordinates of two vectors     *
 *                                              *
 ************************************************/

void gamgi_math_vector_swap (double *u, double *v)
{
double temp;

temp = u[0]; 
u[0] = v[0]; 
v[0] = temp;

temp = u[1];
u[1] = v[1];
v[1] = temp;

temp = u[2];
u[2] = v[2];
v[2] = temp;
}
