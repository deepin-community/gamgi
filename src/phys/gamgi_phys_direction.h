/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_direction.h
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

void static_closer (int *uvw, int o1, int o2, int o3,
int signal, int *node_old);

static void static_segment (gamgi_darray *darray);

gamgi_bool static static_point (gamgi_cell *cell,
double *r1, double *r2, double *r3);

static void static_center (gamgi_direction *direction);

static gamgi_darray *static_line (gamgi_direction *direction,
double thickness, double *position, double *normal, gamgi_dlist *dlist);

static void static_copy (gamgi_direction *direction,
double thickness, double *normal, gamgi_dlist *start);

static void static_link (gamgi_direction *direction,
double thickness, double *normal, gamgi_dlist *start);

static int static_order (int *hkl, gamgi_cell *cell, int step);

static static_near (gamgi_direction *direction, 
int o1, int o2, int o3, int *n1, int *n2, int *n3);

static gamgi_bool static_range (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, double thickness, gamgi_dlist *dlist, int *node, int *swap,
double *normal, gamgi_bool link);

static void static_projection (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum model);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_phys_direction_zero (int *a);

gamgi_bool gamgi_phys_direction_higher (int *a, int *b);

gamgi_bool gamgi_phys_direction_weiss (int *a, int *b);

gamgi_bool gamgi_phys_direction_decrease (int *uvw, int *uvw_min);

void gamgi_phys_direction_conventional (gamgi_enum lattice,
int *uvw_p, int *uvw_c);

void gamgi_phys_direction_primitive (gamgi_enum lattice,
int *uvw_c, int *uvw_p);

void gamgi_phys_direction_closer (gamgi_cell *cell,
gamgi_direction *direction, int *node_old);

void gamgi_phys_direction_vector (gamgi_cell *cell,
int *uvw, gamgi_enum vectors, double *vector);

void gamgi_phys_direction_points (gamgi_direction *direction,
double *r1, double *r2);

void gamgi_phys_direction_default (double *p1, double *p2, 
gamgi_bool *inside, gamgi_darray *darray);

gamgi_bool gamgi_phys_direction_cell (gamgi_direction *direction, 
gamgi_cell *cell);

gamgi_object *gamgi_phys_direction_parent (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_window *window);

void gamgi_phys_direction_line (gamgi_direction *direction, double *points);

gamgi_bool gamgi_phys_direction_reference (gamgi_direction *direction,
gamgi_atom **atom1, gamgi_atom **atom2);

gamgi_bool gamgi_phys_direction_length (double *begin, double *end);

gamgi_dlist *gamgi_phys_direction_reorder (gamgi_dlist *start,
gamgi_atom *atom1, gamgi_atom *atom2);

void gamgi_phys_direction_atoms (gamgi_direction *direction, gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_dlist *dlist, gamgi_enum structure, double thickness,
gamgi_bool copy);

void gamgi_phys_direction_polytope (int n_neighbours, gamgi_atom **neighbours,
gamgi_enum structure, float red, float green, float blue, gamgi_object *parent,
gamgi_atom *central);

void gamgi_phys_direction_range (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness, gamgi_dlist *dlist,
int *hkl1, int start1, int end1, int *hkl2, int start2, int end2);

void gamgi_phys_direction_node (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness, gamgi_dlist *dlist);

void gamgi_phys_direction_projection (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum pole, gamgi_enum trace);

void gamgi_phys_direction_name (int u, int v, int w, char *token);

/*************
 * Rationale *
 *************

Last update:
- 26 Oct 13 -

 *************
 * Rationale *
 *************/
