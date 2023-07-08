/******************************************
 *
 * $GAMGI/src/math/gamgi_math_sort.h
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

long gamgi_math_sort_max (long a0, long b0);

long gamgi_math_sort_min (long a0, long b0);

void gamgi_math_sort_int2 (int a0, int b0, int *a1, int *b1);

void gamgi_math_sort_long2 (long a0, long b0, long *a1, long *b1);

void gamgi_math_sort_double2 (double a0, double b0, double *a1, double *b1);

void gamgi_math_sort_int3 (int a0, int b0, int c0, int *a1, int *b1, int *c1);

void gamgi_math_sort_long3 (long a0, long b0, long c0,
long *a1, long *b1, long *c1);

void gamgi_math_sort_double3 (double a0, double b0, double c0, 
double *a1, double *b1, double *c1);

void gamgi_math_sort_double2v (double *a0, double *b0,
double *a1, double *b1);

void gamgi_math_sort_double3v (double *a0, double *b0, double *c0,
double *a1, double *b1, double *c1);

void gamgi_math_sort_direction (double a, double b,
double *pa, double *pb, double ac, double bc,
double *pac, double *pbc, double *rc, double *m);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
