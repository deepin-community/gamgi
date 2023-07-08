/******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_polytope.h
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_atom (gamgi_atom *atom, static_polytope *polytope);

static int *static_export_points (gamgi_group *group, static_polytope *polytope);

static void static_export_loops (int *index, 
gamgi_group *group, static_polytope *polytope);

static void static_export (static_polytope *polytope);

static int static_dot (int v1, int v2,
double tolerance, static_polytope *polytope);

static int static_cross (int v1, int v2, int v3,
double tolerance, static_polytope *polytope);

static int static_mix (int v1, int v2, int v3, int v4,
double tolerance, static_polytope *polytope);

static int static_conflict_face_increase (int f, static_polytope *polytope);

static int static_conflict_vertex_increase (int v, static_polytope *polytope);

static int static_conflict_find (int v1, int v2, 
int v3, int v, int f, static_polytope *polytope);

static int static_conflict_retrieve (int f, int f_old, 
int v_start, int v_end, int v_new, static_polytope *polytope);

static int static_conflict_recreate (int f, int e, int v, 
static_polytope *polytope);

static int static_conflict_create (int v1, int v2, int v3,
int f, static_polytope *polytope);

static void static_edge_start (int e, static_polytope *polytope);

static static_edge *static_edge_create (int e, static_polytope *polytope);

static void static_edge_remove (int e, static_polytope *polytope);

static void static_edges_mark (int v_new, static_polytope *polytope);

static int static_edges_increase (static_polytope *polytope);

static void static_edges_clean (int v_new, static_polytope *polytope);

static void static_face_start (int f, static_polytope *polytope);

static static_face *static_face_create (int f, static_polytope *polytope);

static int static_face_increase (int f, static_polytope *polytope);

static void static_face_remove (int f, static_polytope *polytope);

static void static_faces_mark (int v_new, static_polytope *polytope);

static int static_faces_increase (static_polytope *polytope);

static void static_face_merge (int f, int v1, int v2, int v3, 
int e, static_polytope *polytope);

static void static_faces_merge (static_polytope *polytope);

static void static_faces_clean (int v_new, static_polytope *polytope);

static void static_face_build (int v_new, int e_visible,
static_polytope *polytope);

static void static_vertex_build (int v_new, static_polytope *polytope);

static void static_vertex_start (int v, static_polytope *polytope);

static static_vertex *static_vertex_create (int v, static_polytope *polytope);

static void static_vertices_increase (static_polytope *polytope);

static gamgi_bool static_inside_local (int x, int y, int z, static_polytope *polytope);

static gamgi_bool static_inside_global (int shell, static_polytope *polytope);

void static_neighbour_get (int n_x, int n_y, int n_z, static_polytope *polytope);

static void static_raw_x0 (int cell_x, int cell_y, int cell_z, 
int shell, static_polytope *polytope);

static void static_raw_x1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x0y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static void static_raw_x1y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope);

static gamgi_bool static_neighbours_find (int cell_x, int cell_y, int cell_z,
gamgi_slist *start, gamgi_atom *atom, static_polytope *polytope);

static void static_neighbours_swap (static_polytope *polytope);

static int static_neighbours_independent (static_polytope *polytope);

static void static_arrays_create (static_polytope *polytope);

static void static_arrays_start (static_polytope *polytope);

static void static_arrays_remove (static_polytope *polytope);

static void static_cells_create (static_polytope *polytope);

static void static_cells_remove (static_polytope *polytope);

static void static_build_tetrahedron (static_polytope *polytope);

static void static_build_polyhedron (static_polytope *polytope);

static int static_build_central (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_atom *atom, static_polytope *polytope);

static void static_measure (int n_vertices, static_polytope *polytope);

static void static_report_start (int *n_polyhedrons, int *n_faces, int *n_edges,
int *n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles);

static void static_report_add (int *n_polyhedrons, int *n_faces, int *n_edges,
int *n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles,
static_polytope *polytope);

static void static_report_end (GtkWidget *text, int n_polyhedrons, int n_faces,
int n_edges, int n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles);

static void static_build (int *out, GtkWidget *text, static_polytope *polytope);

static static_polytope *static_start (gamgi_dlist *atoms, gamgi_group *group,
gamgi_atom *atom, gamgi_bool global, gamgi_enum structure, double visible,
double coplanar, int element, int number, double radius);

static void static_end (static_polytope *polytope);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_chem_polytope (gamgi_dlist *atoms, gamgi_group *group,
gamgi_atom *atom, gamgi_bool global, gamgi_bool structure, double visible,
double coplanar, int element, int number, double radius, int *out,
GtkWidget *text);

/*************
 * Rationale *
 *************

Implemented. Last update: August 2009.

 *************
 * Rationale *
 *************/

