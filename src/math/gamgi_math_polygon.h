/******************************************
 *
 * $GAMGI/src/math/gamgi_math_polygon.h
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

static int static_compare (const void *a, const void *b);

static void static_intersect (double a, double b, double c,
double *r2, double *s1, double *s2, gamgi_darray *darray);

 **********************
 * external functions *
 **********************/

void gamgi_math_polygon_normal (double *r1,
double *r2, double *r3, double *normal);

gamgi_bool gamgi_math_polygon_check_loops (int *loops,
int size, double *points, int n_points);

gamgi_bool gamgi_math_polygon_check_lines (int *lines,
int n_lines, double *points, int n_points);

gamgi_bool gamgi_math_polygon_check_colors (int *loops,
int n_loops, float *colors, int n_colors, int *paints, int n_paints);

double gamgi_math_polygon_area (int n_points, double *points);

double gamgi_math_polygon_area_faces (int *faces, double *points);

double gamgi_math_polygon_length_lines (int *lines, double *points);

void gamgi_math_polygon_lines (int n_points, int **lines, int *n_lines);

void gamgi_math_polygon_loops (int n_points, int **loops, int *n_loops);

void gamgi_math_polygon_parallelepiped (double *array, int k0, int k1, 
int j0, int j1, int i0, int i1, double *v1, double *v2, double *v3);

void gamgi_math_polygon_body (int base,
int n1, int n2, int n3, int **lines, int *n_lines);

void gamgi_math_polygon_faces (int base,
int n1, int n2, int n3, int **lines, int *n_lines);

void gamgi_math_polygon_edges (int base,
int n1, int n2, int n3, int **loops, int *n_loops);

void gamgi_math_polygon_lines_v12 (int *lines, int *lines_offset,
int points_offset, gamgi_bool line1, gamgi_bool line2, gamgi_bool line3,
gamgi_bool line4, gamgi_bool line5, gamgi_bool line6, gamgi_bool line7,
gamgi_bool line8, gamgi_bool line9, gamgi_bool line10, gamgi_bool line11,
gamgi_bool line12, gamgi_bool line13, gamgi_bool line14, gamgi_bool line15,
gamgi_bool line16, gamgi_bool line17, gamgi_bool line18);

void gamgi_math_polygon_loops_v12 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3,
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7,
gamgi_bool loop8);

void gamgi_math_polygon_loops_v14 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3,
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7,
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11,
gamgi_bool loop12);

void gamgi_math_polygon_loops_v18 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3,
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7,
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11,
gamgi_bool loop12);

void gamgi_math_polygon_loops_v24 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3,
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7,
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11,
gamgi_bool loop12, gamgi_bool loop13, gamgi_bool loop14);

void gamgi_math_polygon_circumpherence_yz (int *loops, double *points,
double r, int slices, int *loops_offset, int *points_offset);

void gamgi_math_polygon_circumpherence_xz (int *loops, double *points,
double r, int slices, int *loops_offset, int *points_offset);

void gamgi_math_polygon_circumpherence_xy (int *loops, double *points,
double r, int slices, int *loops_offset, int *points_offset);

gamgi_darray *gamgi_math_polygon_diameter (gamgi_cell *cell,
gamgi_direction *direction, double *r1, double *r2);

gamgi_darray *gamgi_math_polygon_circle (gamgi_cell *cell,
gamgi_plane *plane, double *normal, int slices, 
double *r1, double *r2, double *r3);

gamgi_darray *gamgi_math_polygon_scan (int *loops, 
double *points, double *r1, double *r2, double *r3);

void gamgi_math_polygon_orientate (gamgi_darray *darray, double *normal);

void gamgi_math_polygon_convex (gamgi_darray *darray, double *normal);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
