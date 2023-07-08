/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_matrix.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"

/************ external function *********
 *                                      *
 *       GAMGI_MATH_MATRIX_ABSOLUTE     *
 *                                      *
 *   Return a 3x3 matrix with elements  *
 * m11,m12,m13,m21,m22,m23,m31,m32,m33. *
 *                                      *
 ****************************************/

void gamgi_math_matrix_absolute (double *m, double m11, double m12, double m13, 
double m21, double m22, double m23, double m31, double m32, double m33)
{
m[0] = m11;
m[1] = m12;
m[2] = m13;
m[3] = m21;
m[4] = m22;
m[5] = m23;
m[6] = m31;
m[7] = m32;
m[8] = m33;
}

/************ external function **********
 *                                       *
 *        GAMGI_MATH_MATRIX_RELATIVE     *
 *                                       *
 * Return a 3x3 matrix moved by elements *
 * m11,m12,m13,m21,m22,m23,m31,m32,m33.  *
 *                                       *
 *****************************************/

void gamgi_math_matrix_relative (double *m, double m11, double m12, double m13,
double m21, double m22, double m23, double m31, double m32, double m33)
{
m[0] += m11;
m[1] += m12;
m[2] += m13;
m[3] += m21;
m[4] += m22;
m[5] += m23;
m[6] += m31;
m[7] += m32;
m[8] += m33;
}

/***** external function *****
 *                           *
 *  GAMGI_MATH_MATRIX_ZERO   *
 *                           *
 * Return a 3x3 zero matrix. *
 *                           *
 *****************************/

void gamgi_math_matrix_zero (double *m)
{
gamgi_math_matrix_absolute (m, 
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}

/******** external function ******
 *                               *
 *     GAMGI_MATH_MATRIX_UNIT    *
 *                               *
 * Return a 3x3 identity matrix. *
 *                               *
 *********************************/

void gamgi_math_matrix_unit (double *m)
{
gamgi_math_matrix_absolute (m, 
1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
}

gamgi_bool gamgi_math_matrix_unit_check (double *m)
{
if (m[0] == 1.0 && m[1] == 0.0 && m[2] == 0.0 &&
    m[3] == 0.0 && m[4] == 1.0 && m[5] == 0.0 &&
    m[6] == 0.0 && m[7] == 0.0 && m[8] == 1.0) return TRUE;

return FALSE;
}

/************* external function ********
 *                                      *
 *          GAMGI_MATH_MATRIX_ADD       *
 *                                      *
 * Return the addition of two matrices. *
 *                                      *
 ****************************************/

void gamgi_math_matrix_add (double *m, double *n, double *out)
{
out[0] = m[0] + n[0];
out[1] = m[1] + n[1];
out[2] = m[2] + n[2];
out[3] = m[3] + n[3];
out[4] = m[4] + n[4];
out[5] = m[5] + n[5];
out[6] = m[6] + n[6];
out[7] = m[7] + n[7];
out[8] = m[8] + n[8];
}

/******************* external function *************
 *                                                 *
 *               GAMGI_MATH_MATRIX_SCALE           *
 *                                                 *
 * Return a matrix multiplied by a scaling factor. *
 *                                                 *
 ***************************************************/

void gamgi_math_matrix_scale (double *m, double *out, double scale)
{
out[0] = m[0] * scale;
out[1] = m[1] * scale;
out[2] = m[2] * scale;
out[3] = m[3] * scale;
out[4] = m[4] * scale;
out[5] = m[5] * scale;
out[6] = m[6] * scale;
out[7] = m[7] * scale;
out[8] = m[8] * scale;
}

/****** external function *****
 *                            *
 *   GAMGI_MATH_MATRIX_COPY   *
 *                            *
 * Return a copy of a matrix. *
 *                            *
 ******************************/

void gamgi_math_matrix_copy (double *m, double *out)
{
out[0] = m[0];
out[1] = m[1];
out[2] = m[2];
out[3] = m[3];
out[4] = m[4];
out[5] = m[5];
out[6] = m[6];
out[7] = m[7];
out[8] = m[8];
}

/***************** external function *************
 *                                               *
 *             GAMGI_MATH_MATRIX_VECTOR          *
 *                                               *
 * Return the product of a matrix with a vector. *
 *                                               *
 *************************************************/

void gamgi_math_matrix_vector (double *m, double *v, double *out)
{
out[0] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2];
out[1] = m[3] * v[0] + m[4] * v[1] + m[5] * v[2];
out[2] = m[6] * v[0] + m[7] * v[1] + m[8] * v[2];
}

/************* external function *******
 *                                     *
 *        GAMGI_MATH_MATRIX_MATRIX     *
 *                                     *
 * Return the product of two matrices. *
 *                                     *
 ***************************************/

void gamgi_math_matrix_matrix (double *m, double *n, double *out)
{
out[0] = m[0] * n[0] + m[1] * n[3] + m[2] * n[6];
out[1] = m[0] * n[1] + m[1] * n[4] + m[2] * n[7];
out[2] = m[0] * n[2] + m[1] * n[5] + m[2] * n[8];

out[3] = m[3] * n[0] + m[4] * n[3] + m[5] * n[6];
out[4] = m[3] * n[1] + m[4] * n[4] + m[5] * n[7];
out[5] = m[3] * n[2] + m[4] * n[5] + m[5] * n[8];

out[6] = m[6] * n[0] + m[7] * n[3] + m[8] * n[6];
out[7] = m[6] * n[1] + m[7] * n[4] + m[8] * n[7];
out[8] = m[6] * n[2] + m[7] * n[5] + m[8] * n[8];
}

double gamgi_math_matrix_determinant (double *m)
{
return m[0] * (m[4] * m[8] - m[7] * m[5])
     + m[1] * (m[6] * m[5] - m[3] * m[8])
     + m[2] * (m[3] * m[7] - m[6] * m[4]);
}

void gamgi_math_matrix_transpose (double *m, double *out)
{
out[0] = m[0];
out[1] = m[3];
out[2] = m[6];

out[3] = m[1];
out[4] = m[4];
out[5] = m[7];

out[6] = m[2];
out[7] = m[5];
out[8] = m[8];
}

void gamgi_math_matrix_adjunct (double *m, double *out)
{
out[0] = m[4] * m[8] - m[7] * m[5];
out[1] = m[7] * m[2] - m[1] * m[8];
out[2] = m[1] * m[5] - m[4] * m[2];

out[3] = m[6] * m[5] - m[3] * m[8];
out[4] = m[0] * m[8] - m[6] * m[2];
out[5] = m[3] * m[2] - m[0] * m[5];

out[6] = m[3] * m[7] - m[6] * m[4];
out[7] = m[6] * m[1] - m[0] * m[7];
out[8] = m[0] * m[4] - m[3] * m[1];
}

void gamgi_math_matrix_inverse (double *m, double *out)
{
gamgi_math_matrix_adjunct (m, out);
gamgi_math_matrix_scale (out, out, 1/gamgi_math_matrix_determinant (m));
}

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/****************************************
 * Code taken from the library MESA 3.0 *
 *                       Carlos Pereira *
 ****************************************/

/*********** external function **********
 *                                      *
 *       GAMGI_MATH_MATRIX_ROTATION     *
 *                                      *
 ****************************************/

void gamgi_math_matrix_rotation (double angle, 
double *axis, double *rotate)
{
double s, c, cc, xx, yy, zz, xy, yz, zx, xs, ys, zs;

/********************************
 * convert input angle (assumed *
 * to be in degrees) to radians *
 ********************************/

s = sin (angle * GAMGI_MATH_DEG_RAD);
c = cos (angle * GAMGI_MATH_DEG_RAD);
cc = 1.0 - c;

/*
 *    This function was contributed to Mesa by Erich Boleyn
 *
 *              Arbitrary axis rotation matrix.
 *
 *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
 *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
 *  (which is about the X-axis), and the two composite transforms
 *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
 *  from the arbitrary axis to the X-axis then back.  They are
 *  all elementary rotations.
 *
 *  Rz' is a rotation about the Z-axis, to bring the axis vector
 *  into the x-z plane.  Then Ry' is applied, rotating about the
 *  Y-axis to bring the axis vector parallel with the X-axis.  The
 *  rotation about the X-axis is then performed.  Ry and Rz are
 *  simply the respective inverse transforms to bring the arbitrary
 *  axis back to it's original orientation.  The first transforms
 *  Rz' and Ry' are considered inverses, since the data from the
 *  arbitrary axis gives you info on how to get to it, not how
 *  to get away from it, and an inverse must be applied.
 *
 *  The basic calculation used is to recognize that the arbitrary
 *  axis vector (x, y, z), since it is of unit length, actually
 *  represents the sines and cosines of the angles to rotate the
 *  X-axis to the same orientation, with theta being the angle about
 *  Z and phi the angle about Y (in the order described above)
 *  as follows:
 *
 *  cos ( theta ) = x / sqrt ( 1 - z^2 )
 *  sin ( theta ) = y / sqrt ( 1 - z^2 )
 *
 *  cos ( phi ) = sqrt ( 1 - z^2 )
 *  sin ( phi ) = z
 *
 *  Note that cos ( phi ) can further be inserted to the above
 *  formulas:
 *
 *  cos ( theta ) = x / cos ( phi )
 *  sin ( theta ) = y / sin ( phi )
 *
 *  ...etc.  Because of those relations and the standard trigonometric
 *  relations, it is pssible to reduce the transforms down to what
 *  is used below.  It may be that any primary axis chosen will give the
 *  same results (modulo a sign convention) using this method.
 *
 *  Particularly nice is to notice that all divisions that might
 *  have caused trouble when parallel to certain planes or
 *  axis go away with care paid to reducing the expressions.
 *  After checking, it does perform correctly under all cases, since
 *  in all the cases of division where the denominator would have
 *  been zero, the numerator would have been zero as well, giving
 *  the expected result.
 */
  
/*******************************
 * axis must be a unit vector! *
 *******************************/

xx = axis[0] * axis[0];
yy = axis[1] * axis[1];
zz = axis[2] * axis[2];
xy = axis[0] * axis[1];
yz = axis[1] * axis[2];
zx = axis[2] * axis[0];
xs = axis[0] * s;
ys = axis[1] * s;
zs = axis[2] * s;

gamgi_math_matrix_absolute (rotate,
(cc * xx) + c, (cc * xy) - zs, (cc * zx) + ys,
(cc * xy) + zs, (cc * yy) + c, (cc * yz) - xs,
(cc * zx) - ys, (cc * yz) + xs, (cc * zz) + c);
}
