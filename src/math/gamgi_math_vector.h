/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_vector.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_math_vector_absolute (double *v, double x, double y, double z);

void gamgi_math_vector_relative (double *v, double x, double y, double z);

void gamgi_math_vector_zero (double *v);

void gamgi_math_vector_add (const double *v, const double *w, double *out);

void gamgi_math_vector_sub (const double *v, const double *w, double *out);

double gamgi_math_vector_length (const double *v);

void gamgi_math_vector_fmod (const double *v, double *out);

void gamgi_math_vector_scale (double *v, double *out, double scale);

void gamgi_math_vector_normal (double *v);

void gamgi_math_vector_copy (const double *v, double *out);

double gamgi_math_vector_dot (const double *v, const double *w);

void gamgi_math_vector_cross (const double *v, const double *w, double *out);

double gamgi_math_vector_mix (const double *u, const double *v, const double *w);

double gamgi_math_vector_angle (double *v1, double *v2);

void gamgi_math_vector_translation (double x, double y, 
double z, double *v1, double *v2, double *v3, double *out);

void gamgi_math_vector_orthogonal (const double *u, const double *v, double *out);

void gamgi_math_vector_box (double *r, double *min, double *max);

void gamgi_math_vector_swap (double *u, double *v);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
