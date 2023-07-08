/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_voronoi.h
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

static double static_border (int shell, double r2, double border);

static void static_pbc (int n, double width_i,
int *n_i, double *shift_i, double border)

static int static_compare (const void *sa, const void *sb);

static void static_expectancy (double *average, double *deviation, int n);

static double static_angle (double l1, double l2, double dot);

static gamgi_edge *static_edge (gamgi_edge *edge);

static gamgi_vertex *static_vertex (gamgi_vertex *vertex);

static gamgi_vertex *static_vertex_create (double x, double y, double z);

static void static_vertex_remove (gamgi_vertex *vertex);

static gamgi_edge *static_edge_create (double x, 
double y, double z, double length);

static void static_edge_remove (gamgi_edge *edge);

static gamgi_face *static_face_create (int local, 
int global, double x, double y, double z);

static void static_face_remove (gamgi_face *face);

static gamgi_polyhedron *static_polyhedron_create (int local, 
double *position, float red, float green, float blue);

static void static_polyhedron_remove (gamgi_polyhedron *polyhedron);

static gamgi_voronoi *static_start (gamgi_object *object, 
gamgi_bool periodic, double offset, double variancy, double apex, 
double area, double vertex, float *color, gamgi_enum borders);

static void static_end (gamgi_voronoi *voronoi);

static void static_face_orientate (gamgi_face *face);

static void static_update_add (int offset, gamgi_voronoi *voronoi);

static void static_neighbour_add (double x, double y, 
double z, int local, int global, gamgi_voronoi *voronoi);

static void static_vertex_add (int neighbour1, int neighbour2, int neighbour3, 
double x, double y, double z, gamgi_voronoi *voronoi);

static gamgi_bool static_vertex_find (double *n1, double *n2, double *n3, double *v);

static gamgi_bool static_face_hide (double *n, double *v, double tolerance);

static void static_vertex_shift (gamgi_voronoi *voronoi);

static void static_polyhedron_add (double *size, gamgi_voronoi *voronoi);

static void static_polyhedron_update (gamgi_voronoi *voronoi);

static void static_size_update (gamgi_voronoi *voronoi);

static void static_polyhedron_copy (gamgi_polyhedron *polyhedron, gamgi_voronoi *voronoi);

static void static_polyhedron_orientate (gamgi_polyhedron *polyhedron, gamgi_voronoi *voronoi);

static void static_neighbour_scan (gamgi_voronoi *voronoi);

static int static_vertex_scan (double *position, gamgi_voronoi *voronoi);

static void static_neighbour_try (double *position0, double radius0, 
int mark, double *position1, double radius1, int local, int global, 
gamgi_voronoi *voronoi);

static void static_neighbour_raw (gamgi_slist *slist, double *position0,
double *position1, double *radius1, int *local, int *global);

static void static_neighbour_pbc (gamgi_slist *slist, double *shift,
double *position0, double *position1, double *radius1, int *local, int *global);

static void static_build_central (gamgi_slist *start, gamgi_slist *seed,
double *position0, double radius0, int mark, gamgi_voronoi *voronoi);

static void static_raw_x0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell,
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);
  
static void static_raw_x1y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x0y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_raw_x1y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi);

static void static_pbc_x0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);
  
static void static_pbc_x1y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x0y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_pbc_x1y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi);

static void static_build_polyhedron_raw (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_slist *seed, gamgi_voronoi *voronoi);

static void static_build_polyhedron_pbc (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_slist *seed, gamgi_voronoi *voronoi);

static void static_vertex_list (gamgi_voronoi *voronoi);

static void static_edge_list (gamgi_voronoi *voronoi);

static void static_neighbour_list (gamgi_voronoi *voronoi);

static void static_unlist (gamgi_voronoi *voronoi);

static void static_cell (double position, double min,
double width, int n_cells, int *cell);

static void static_vertex_equal (gamgi_vertex *vertex0, 
gamgi_vertex *vertex1, double tolerance);

static void static_vertex_near (gamgi_vertex *vertex, int index,
int cell_x, int cell_y, int cell_z, gamgi_voronoi *voronoi);

static void static_vertex_central (int cell_x, int cell_y, int cell_z,
double min_x, double min_y, double min_z, double width_x, double width_y,
double width_z, gamgi_voronoi *voronoi);

static void static_edge_local (gamgi_edge *edge0, gamgi_edge *edge1);

static void static_edge_global (gamgi_edge *edge0, gamgi_edge *edge1);

static void static_edge_central (int cell_x, int cell_y, int cell_z,
gamgi_voronoi *voronoi);

static void static_vertex_order (gamgi_voronoi *voronoi);

static void static_edge_build (gamgi_voronoi *voronoi);

static void static_merge (gamgi_voronoi *voronoi);

static void static_build_start (gamgi_voronoi *voronoi);

static void static_build_end (gamgi_voronoi *voronoi);

static gamgi_bool static_build (gamgi_voronoi *voronoi);

static void static_scan_vertices (gamgi_voronoi *voronoi);

static void static_measure_polyhedrons (int *real,
int *border, int *total, gamgi_voronoi *voronoi);

static void static_measure_faces (int *real,
int *border, int *total, gamgi_voronoi *voronoi);

static void static_measure_edges (int *real,
int *border, int *total, gamgi_voronoi *voronoi);

static void static_measure_vertices (int *real,
int *border, int *total, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_faces (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_edges (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_vertices (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_face_vertices (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_scan_edge_polyhedrons (gamgi_voronoi *voronoi);

static void static_measure_edge_polyhedrons (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_scan_edge_faces (gamgi_voronoi *voronoi);

static void static_measure_edge_faces (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_scan_vertex_polyhedrons (gamgi_voronoi *voronoi);

static void static_measure_vertex_polyhedrons (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_scan_vertex_faces (gamgi_voronoi *voronoi);

static void static_measure_vertex_faces (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_scan_vertex_edges (gamgi_voronoi *voronoi);

static void static_measure_vertex_edges (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_volumes (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_areas (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_lengths (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_anisotropy (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_polyhedron_aberration (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_face_areas (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_face_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_face_angles (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_face_aberration (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_edge_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_edge_angles (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_measure_seed_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi);

static void static_report (GtkWidget *text, GdkFont *font,
GdkColor color_title, GdkColor color_foreground,
GdkColor color_bold, GdkColor color_link, gamgi_voronoi *voronoi);

static void static_export_start (gamgi_cluster *cluster);

static void static_export_color (gamgi_cluster *cluster,
float *color, int n_loops, int n_polyhedrons);

static void static_export_loop (gamgi_cluster *cluster,
gamgi_face *face, int *n_loops, int *n_points);

static void static_export_point (gamgi_cluster *cluster,
double *position, int *n_points);

static void static_export_seed (gamgi_cluster *cluster,
gamgi_polyhedron *polyhedron, int *n_polyhedrons, int *n_points,
gamgi_voronoi *voronoi);

static void static_export_polyhedron (gamgi_cluster *cluster,
gamgi_polyhedron *polyhedron, float *color, int *n_loops, 
int *n_points, int *n_polyhedrons, gamgi_voronoi *voronoi);

static void static_export_end (gamgi_cluster *cluster,
int n_loops, int n_points, int n_polyhedrons, int n_colors);

static void static_export (gamgi_cluster *cluster, gamgi_voronoi *voronoi);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_phys_voronoi (gamgi_object *object,
gamgi_cluster *cluster, gamgi_bool periodic, double offset,
double variancy, double apex, double area, double vertex, 
float *color, gamgi_enum borders, GtkWidget *text);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
