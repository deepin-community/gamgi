/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_matrix.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

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

/**********************
 * external functions *
 **********************/

void gamgi_math_matrix_absolute (double *m, double m11, double m12, 
double m13, double m21, double m22, double m23, 
double m31, double m32, double m33);

void gamgi_math_matrix_relative (double *m, double m11, double m12,
double m13, double m21, double m22, double m23,
double m31, double m32, double m33);

void gamgi_math_matrix_zero (double *m);

void gamgi_math_matrix_unit (double *m);

gamgi_bool gamgi_math_matrix_unit_check (double *m);

void gamgi_math_matrix_add (double *m, double *n, double *out);

void gamgi_math_matrix_scale (double *m, double *out, double scale);

void gamgi_math_matrix_copy (double *m, double *n);

void gamgi_math_matrix_vector (double *m, double *v, double *out);

void gamgi_math_matrix_matrix (double *m, double *n, double *out);

double gamgi_math_matrix_determinant (double *m);

void gamgi_math_matrix_transpose (double *m, double *out);

void gamgi_math_matrix_adjunct (double *m, double *out);

void gamgi_math_matrix_inverse (double *m, double *out);

void gamgi_math_matrix_rotation (double angle,
double *axis, double *m);

void gamgi_math_matrix_coordinates (double *base, 
double *position, double *coordinates);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
