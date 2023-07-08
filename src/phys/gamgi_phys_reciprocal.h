/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_reciprocal.h
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

gamgi_enum gamgi_phys_reciprocal_lattice (gamgi_enum lattice);

void gamgi_phys_reciprocal_vectors (double reciprocal, double *p1, 
double *p2, double *p3, double *b1, double *b2, double *b3);

/**********************
 * internal functions *
 **********************

static void static_cell_copy (gamgi_cell *cell_old, gamgi_cell *cell);

static void static_cell_increase (gamgi_enum lattice, gamgi_cell *cell);

static void static_vectors (double reciprocal, gamgi_enum lattice,
double *p1, double *p2, double *p3, double *b1, double *b2, double *b3);

static void static_lattice (gamgi_cell *cell, double *rotate);

static void static_position (gamgi_cell *cell_old, 
gamgi_cell *cell, double *rotate);

 **********************
 * external functions *
 **********************/

void gamgi_phys_reciprocal_points (gamgi_plane *plane);

gamgi_cell *gamgi_phys_reciprocal_primitive (gamgi_cell *cell_old,
double factor, gamgi_window *window);

gamgi_cell *gamgi_phys_reciprocal (gamgi_cell *cell_old, 
double factor, gamgi_enum planes, gamgi_window *window);

/*************
 * Rationale *
 *************

Needs cleaning.
- 09 Aug 05 -

 *************
 * Rationale *
 *************/
