/******************************************
 *
 * $GAMGI/src/math/gamgi_math_cell.h
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

void gamgi_math_cell_range_half (double *in, double *out);

void gamgi_math_cell_range_positive (double *in, double *out);

void gamgi_math_cell_vectors (double a, double b, double c, 
double ab, double ac, double bc, double *a1, double *a2, double *a3);

void gamgi_math_cell_parameters (double *a1, double *a2, double *a3,
double *a, double *b, double *c, double *ab, double *ac, double *bc);

void gamgi_math_cell_primitive (gamgi_enum lattice,
double *a1, double *a2, double *a3, double *p1, double *p2, double *p3);

void gamgi_math_cell_conventional (gamgi_enum lattice,
double *p1, double *p2, double *p3, double *a1, double *a2, double *a3);

void gamgi_math_cell_offset (gamgi_cell *cell, double *offset);

void gamgi_math_cell_center (gamgi_cell *cell, double *center);

void gamgi_math_cell_vector_rotate (gamgi_cell *cell,
double *pole, double *normal);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
