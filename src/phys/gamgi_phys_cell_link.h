/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_cell_link.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_range_half (double *u);

static void static_range_positive (double *u);

static void static_wigner_min (gamgi_cell *cell,
double *absolute, int *equal, int *node_min);

static int *static_wigner_replicas (int *replicas,
int *equal, int position, int node_min, int *offset);

static void static_translate_in (gamgi_cell *cell, int n_positions,
double *positions, double *translation);

static void static_rotate_in (gamgi_cell *cell, int n_positions,
double *positions, double *rotation, double *center);

static void static_conventional (gamgi_cell *cell, 
gamgi_enum vectors, double *xyz);

static void static_primitive (gamgi_cell *cell,
int n_positions, double *relative);

static double *static_absolute (gamgi_cell *cell,
int n_positions, double *relative);

static int *static_wigner_absolute (gamgi_cell *cell,
gamgi_enum cutoff, int n_positions, double *absolute);

static void static_volume (gamgi_cell *cell, double *inverse);

static void static_node_coordinates (int node,
int n1, int n2, int n3, int *i, int *j, int *k);

static gamgi_bool static_node_inside (int n1, int n2, int n3,
int i, int j, int k);

static gamgi_bool static_wigner_inside (gamgi_cell *cell,
double *absolute, int node, int position, int *replicas);

static gamgi_bool static_object_inside (gamgi_cell *cell, 
double *absolute, double *inverse, int node, int position, 
int *replicas);

static gamgi_bool static_point_inside (gamgi_cell *cell, int x,
int y, double coord_x, double coord_y, int *triangles);

static gamgi_bool static_inside_triangles (gamgi_cell *cell,
int node, double *coord, int *triangles);

static int *static_triangles_inside (gamgi_cell *cell);

static void static_link (gamgi_cell *cell, double *points, gamgi_atom *atom, 
int *nodes, int n_nodes, int n_positions, double *relative, double *absolute, 
gamgi_enum cutoff, double *rotate, double *translate, double *inverse, 
int *index, int *triangles, int *replicas);

 ************
 * patterns *
 ************ 

static int static_sequence (gamgi_char *pattern, int *sequence);

static void static_direction_a (int *nodes, int *sequence, int length, 
int direction, int plane, int k0, int k1, int j, int i);

static void static_direction_b (int *nodes, int *sequence, int length, 
int direction, int plane, int k, int j0, int j1, int i);

static void static_direction_c (int *nodes, int *sequence, int length, 
int direction, int plane, int k, int j, int i0, int i1);

static int static_plane_a (int *nodes, gamgi_char *pattern, 
int start, int end, int n1, int n2, int n3);

static int static_plane_b (int *nodes, gamgi_char *pattern, 
int start, int end, int n1, int n2, int n3);

static int static_plane_c (int *nodes, gamgi_char *pattern, 
int start, int end, int n1, int n2, int n3);

static void static_volume_a (int *nodes, char *pattern, int n1, int n2, int n3);

static void static_volume_b (int *nodes, char *pattern, int n1, int n2, int n3);

static void static_volume_c (int *nodes, char *pattern, int n1, int n2, int n3);

static int *static_mark (int n1, int n2, int n3, 
char *pattern_a, char *pattern_b, char *pattern_c);

static int *static_replicate (int n1, int n2, int n3, int *nodes_in);

static int *static_copy (int *index, int o1, int o2, int o3,
int n_nodes, int *nodes_in, int n1, int n2, int n3);

static int static_index_max (int min, int max, int origin);

static int *static_nodes (int *index, int *range, int n_nodes, int o1,
int o2, int o3, char *pattern_a, char *pattern_b, char *pattern_c);

 ********
 * link *
 ********

static void static_hexagonal_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c,
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_conventional_A_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_conventional_B_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse);

static void static_conventional_C_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_conventional_I_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_conventional_R_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_wigner_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse);

static void static_primitive_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c,
int n_positions, double *relative, double *absolute,
double *translate, double *inverse);

static void static_parallelepiped_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c,
int n_positions, double *relative, double *absolute,
double *translate, double *inverse);

static void static_sphere_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c,
int n_positions, double *relative, double *absolute,
double *translate, double *inverse);

 **********************
 * external functions *
 **********************/

void gamgi_phys_cell_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum vectors, const char **wyckoff, const int *driver, int site,
double *xyz, double *translation, double *rotation, gamgi_enum cutoff,
int nodes_P, int nodes_I, int nodes_A, int nodes_B, int nodes_C,
int nodes_R, const char *rule_a, const char *rule_b, const char *rule_c,
gamgi_window *window);

/*************
 * Rationale *
 *************

Cleaned.
- 04 Oct 05 -

 *************
 * Rationale *
 *************/
