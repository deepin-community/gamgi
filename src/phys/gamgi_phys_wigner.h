/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_wigner.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 ********************** 

static void static_sort (double a, double b,
double *pa, double *pb, double ac, double bc,
double *pac, double *pbc, double *rc, double *m);

static void static_clean_wigner (gamgi_cell *cell);

static void static_body (gamgi_cell *cell);

static void static_faces (gamgi_cell *cell);

static void static_edges (gamgi_cell *cell);

static void static_orthorhombic_p (gamgi_cell *cell);

static void static_orthorhombic_c (gamgi_cell *cell, double a,
double b, double c, double *m);

static void static_orthorhombic_f (gamgi_cell *cell, double a,
double b, double c, double *m);

static void static_orthorhombic_i_high (gamgi_cell *cell,
double a, double b, double c, double *m);

static void static_orthorhombic_i_low (gamgi_cell *cell,
double a, double b, double c, double *m);

static void static_rhombohedral_high (gamgi_cell *cell,
double a, double b, double c, double *m);

static void static_rhombohedral_low (gamgi_cell *cell,
double a, double b, double c, double *m);

 **********************
 * external functions *
 **********************/

void gamgi_phys_wigner_create (gamgi_cell *cell);

/*************
 * Rationale *
 *************

Cleaned. Wigner-Seitz cells for monoclinic 
and triclinic lattices are still missing. 
- 09 Aug 05 -

 *************
 * Rationale *
 *************/
