/********************************************
 *
 * $GAMGI/src/phys/gamgi_phys_cell.h
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

int gamgi_phys_cell_conventional_I (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points);

int gamgi_phys_cell_conventional_A (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points);

int gamgi_phys_cell_conventional_B (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points);

int gamgi_phys_cell_conventional_C (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points);

int gamgi_phys_cell_conventional_R (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points);

/**********************
 * internal functions *
 **********************

static int static_conventional_P (gamgi_cell *cell);

static void static_primitive_P (gamgi_cell *cell);

static void static_conventional (gamgi_cell *cell);

 **********************
 * external functions *
 **********************/

void gamgi_phys_cell_clean (gamgi_cell *cell, 
int *mark, int *loops_new, int *lines_new);

gamgi_enum gamgi_phys_cell_primitive (gamgi_enum lattice);

void gamgi_phys_cell_parameters (gamgi_cell *cell,
double a, double b, double c, double ab, double ac, double bc);

void gamgi_phys_cell_primitive_index (int *range, int *index);

void gamgi_phys_cell_rombohedral_conventional (double primitive_a,
double primitive_bc, double *conventional_a, double *conventional_c);

void gamgi_phys_cell_rombohedral_primitive (double conventional_a,
double conventional_c, double *primitive_a, double *primitive_bc);

void gamgi_phys_cell_hexagonal_clean (gamgi_cell *cell);

void gamgi_phys_cell_hexagonal_create (gamgi_cell *cell);

void gamgi_phys_cell_hexagonal_index (gamgi_cell *cell, int *index);

void gamgi_phys_cell_parallelepiped_contour (gamgi_cell *cell);

void gamgi_phys_cell_parallelepiped_range (double *p1, double *p2,
double *p3, double *v1, double *v2, double *v3, int *range);

gamgi_bool gamgi_phys_cell_parallelepiped_inside (double *inverse, double *point);

int gamgi_phys_cell_parallelepiped_count (gamgi_cell *cell);

void gamgi_phys_cell_parallelepiped_create (gamgi_cell *cell, int n);

void gamgi_phys_cell_parallelepiped_index (gamgi_cell *cell, int *index);

void gamgi_phys_cell_sphere_contour (gamgi_cell *cell);

void gamgi_phys_cell_sphere_range (double *p1,
double *p2, double *p3, double r, int *range);

int gamgi_phys_cell_sphere_count (gamgi_cell *cell);

void gamgi_phys_cell_sphere_create (gamgi_cell *cell, int n);

void gamgi_phys_cell_sphere_index (gamgi_cell *cell, int *index);

void gamgi_phys_cell_projection_contour (gamgi_cell *cell);

void gamgi_phys_cell_create (gamgi_cell *cell);

gamgi_cell *gamgi_phys_cell_polygons (gamgi_cell *cell_old);

/*************
 * Rationale *
 *************

Cleaned.
- 04 Oct 05 -

 *************
 * Rationale *
 *************/
