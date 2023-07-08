/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_plane.h
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

static void static_create (gamgi_cell *cell,
gamgi_enum vectors, int h, int k, int l, double *origin);

static void static_create_all (gamgi_cell *cell, int *index,
double *transformation, double *orientation);

static void static_conventional_F (gamgi_cell *cell, int *index);

static void static_conventional_I (gamgi_cell *cell, int *index);

static void static_conventional_tI (gamgi_cell *cell, int *index);

static void static_conventional_C (gamgi_cell *cell, int *index);

static void static_conventional_hR (gamgi_cell *cell, int *index);

static void static_primitive (gamgi_cell *cell, int *index);

static void static_wigner (gamgi_cell *cell, int *index);

static int *static_index_nodes (gamgi_cell *cell);

static void static_index_origin (gamgi_cell *cell, int *offset);

static void static_index_translate (gamgi_cell *cell, int *offset, int *index);

static void static_plane_h00 (double *r1, double *a2, double *a3, double *out2, 
double *out3);

static void static_plane_hk0 (double *r1, double *r2, double *a3, double *out3);

static void static_plane_hkl (gamgi_cell *cell, gamgi_plane *plane, double *r1, 
double *r2, double *r3);

static void static_center (gamgi_plane *plane);

static gamgi_darray *static_polygon_dlist (gamgi_plane *plane,
double thickness, double *position, double *normal, gamgi_dlist *dlist);

static gamgi_darray *static_polygon_array (gamgi_plane *plane,
int n_neighbours, gamgi_atom **neighbours);

static void static_copy (gamgi_plane *plane, double thickness,
gamgi_dlist *start, double *normal);

static void static_link (gamgi_plane *plane, double thickness,
gamgi_dlist *start, double *normal);

static gamgi_bool static_normal (gamgi_plane *plane, double *normal);

static gamgi_bool static_range (gamgi_plane *plane, gamgi_cell *cell,
gamgi_enum structure, double thickness, gamgi_dlist *dlist, int order,
double *normal, gamgi_bool link);

static void static_projection (gamgi_plane *plane,
gamgi_cell *cell, gamgi_enum model);

 **********************
 * external functions *
 **********************/

int gamgi_phys_plane_order (gamgi_cell *cell,
int *hkl, gamgi_enum vectors, int o1, int o2, int o3);

void gamgi_phys_plane_vector (gamgi_cell *cell,
int *hkl, gamgi_enum vectors, double *vector);

int gamgi_phys_plane_increase (gamgi_enum lattice,
int *hkl, gamgi_enum vectors, int *hkl_multiple);

gamgi_bool gamgi_phys_plane_miller (gamgi_enum lattice,
gamgi_enum vectors, int *hkl);

gamgi_bool gamgi_phys_plane_laue (gamgi_enum lattice,
gamgi_enum vectors, int *hkl);

void gamgi_phys_plane_conventional (gamgi_enum lattice,
int *hkl_p, int *hkl_c);

void gamgi_phys_plane_primitive (gamgi_enum lattice,
int *hkl_c, int *hkl_p);

void gamgi_phys_plane_reciprocal (gamgi_cell *cell, 
double *transformation, double *orientation);

void gamgi_phys_plane_point (gamgi_plane *plane,
gamgi_cell *cell, double *point);

gamgi_bool gamgi_phys_plane_normal (gamgi_plane *plane, double *normal);

void gamgi_phys_plane_default (double *p1, double *p2, 
double *p3, gamgi_bool *inside, gamgi_darray *darray);

gamgi_bool gamgi_phys_plane_cell (gamgi_plane *plane, gamgi_cell *cell);

gamgi_object *gamgi_phys_plane_parent (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_window *window);

gamgi_bool gamgi_phys_plane_polygon (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3);

gamgi_dlist *gamgi_phys_plane_reorder (gamgi_dlist *start,
gamgi_atom *atom1, gamgi_atom *atom2, gamgi_atom *atom3);

void gamgi_phys_plane_atoms (gamgi_plane *plane, gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_dlist *dlist,
gamgi_enum structure, double thickness);

void gamgi_phys_plane_polytope (int n_neighbours, gamgi_atom **neighbours,
gamgi_enum structure, float red, float green, float blue, gamgi_object *parent,
gamgi_atom *central);

void gamgi_phys_plane_range (gamgi_plane *plane, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness,
gamgi_dlist *dlist, int start, int end);

void gamgi_phys_plane_projection (gamgi_plane *plane,
gamgi_cell *cell, gamgi_enum pole, gamgi_enum trace);

void gamgi_phys_plane_name (int h, int k, int l, char *token);

/*************
 * Rationale *
 *************

Last update:
- 26 Oct 13 -

 *************
 * Rationale *
 *************/
