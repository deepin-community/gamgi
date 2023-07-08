/******************************************
 *
 * $GAMGI/src/io/gamgi_io_x3d.h
 *
 * Copyright (C) 2008 Tiago Farto
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_scale (FILE *fp, double scale, double *center, 
gamgi_bool *error);

static void static_unscale (FILE *fp, double scale, gamgi_bool *error);

static void static_line (FILE *fp, gamgi_bool lights, float *color,
double *start, double *end, int width, gamgi_bool *error);

static void static_cylinder (FILE *fp, gamgi_bool lights, float *color,
double length, double radius, gamgi_bool *error);

static void static_text (FILE *fp, gamgi_text *text, 
gamgi_bool lights, gamgi_bool *error);

void static_orbital (FILE *fp, gamgi_orbital *orbital, 
gamgi_bool lights, gamgi_bool *error);

static void static_bond (FILE *fp, gamgi_bond *bond, gamgi_atom *atom, 
gamgi_bool lights, gamgi_bool *error);

static void static_atom (FILE *fp, gamgi_atom *orbital, 
gamgi_bool lights, gamgi_bool *error);

static void static_direction (FILE *fp, gamgi_direction *direction, 
gamgi_bool lights, gamgi_bool *error);

static void static_plane (FILE *fp, gamgi_plane *plane, 
gamgi_bool lights, gamgi_bool *error);

static void static_group (FILE *fp, gamgi_group *group, 
gamgi_bool lights, gamgi_bool *error);

static void static_molecule (FILE *fp, gamgi_molecule *molecule, 
gamgi_bool lights, gamgi_bool *error);

static void static_cluster (FILE *fp, gamgi_cluster *cluster, 
gamgi_bool lights, gamgi_bool *error);

static void static_cell (FILE *fp, gamgi_cell *cell, 
gamgi_bool lights, gamgi_bool *error);

static void static_arrow (FILE *fp, gamgi_arrow *arrow, 
gamgi_bool lights, gamgi_bool *error);

static void static_shape (FILE *fp, gamgi_shape *shape, 
gamgi_bool lights, gamgi_bool *error);

static void static_graph (FILE *fp, gamgi_graph *graph, 
gamgi_bool lights, gamgi_bool *error);

static void static_assembly (FILE *fp, gamgi_assembly *assembly, 
gamgi_bool lights, gamgi_bool *error);

static void static_light (FILE *fp, gamgi_light *light, gamgi_bool *error);

static void static_layer (FILE * fp, gamgi_layer *layer, gamgi_bool *error);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_io_x3d (FILE *fp, gamgi_window *window, gamgi_bool *error);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
