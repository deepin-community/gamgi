/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export_object.h
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

void gamgi_expat_export_object_text (gamgi_text *text, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_orbital (gamgi_orbital *orbital, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_bond (gamgi_bond *bond, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_atom (gamgi_atom *atom, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_direction (gamgi_direction *direction, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_plane (gamgi_plane *plane, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_group (gamgi_group *group, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_molecule (gamgi_molecule *molecule, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_cluster (gamgi_cluster *cluster, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_cell (gamgi_cell *cell, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_arrow (gamgi_arrow *arrow, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_shape (gamgi_shape *shape, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_graph (gamgi_graph *graph, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_assembly (gamgi_assembly *assembly, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_light (gamgi_light *light, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_layer (gamgi_layer *layer, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

void gamgi_expat_export_object_window (gamgi_window *window, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
