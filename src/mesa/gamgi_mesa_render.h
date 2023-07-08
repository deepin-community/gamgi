/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_render.h
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

static const char unsigned cross5[24];
static const char unsigned chess5[24];
static const char unsigned cross4[24];
static const char unsigned chess4[24];
static const char unsigned cross3[18];
static const char unsigned chess3[18];
static const char unsigned cross2[6];
static const char unsigned chess2[6];
static const char unsigned cross1[3];
static const char unsigned chess1[3];

static void static_mask (int n_points, double *points, 
int width, const char unsigned *bitmap);

 **********************
 * external functions *
 **********************/

void gamgi_mesa_render_points_cross (int n_points, double *points, int width);

void gamgi_mesa_render_points_chess (int n_points, double *points, int width);

void gamgi_mesa_render_points (int n_points, double *points);

void gamgi_mesa_render_line_strip (int n_points, double *points);

void gamgi_mesa_render_lines (int *lines, double *points);

void gamgi_mesa_render_loops (int *loops, double *points);

void gamgi_mesa_render_polygons (int *loops, double *points);

void gamgi_mesa_render_polygons_color (int *loops, 
double *points, int *paints, float *colors);

void gamgi_mesa_render_triangles (int n_triangles, double *points);

void gamgi_mesa_render_triangle_fan (int *loops, double *points);

/*************
 * Rationale *
 *************

These are the functions that actually draw the cells,
based on the 3 arrays stored on cell. _polygon draw the
solid faces, _points draw the nodes, _lines and _loops
draw the wires.

 *************
 * Rationale *
 *************/
