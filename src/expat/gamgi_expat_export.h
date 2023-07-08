/*****************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export.h
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

static void static_space (FILE *fp, int space, gamgi_bool *error);

static int static_end (FILE *fp, char *string, gamgi_bool *error);

static int static_middle (FILE *fp, char *string, gamgi_bool *error);

static int static_start (FILE *fp, char *string, int depth, gamgi_bool *error);

 **********************
 * external functions *
 **********************/

void gamgi_expat_export_cdata (FILE *fp,
char *string, int depth, int *column, gamgi_bool *error);

void gamgi_expat_export_attribute (FILE *fp,
char *string, int depth, int *column, gamgi_bool *error);

void gamgi_expat_export_element (FILE *fp, char *element,
char *string, int depth_parent, int *depth_last, int *column,
gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_bond (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error, 
int element1, int element2, double min, double max);

void gamgi_expat_export_atom (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *error, int element,
double mass, double radius, float red, float green, float blue);

/**********************
 * internal functions *
 **********************

static void static_cdata_double (int size, double *array,
FILE *fp, int depth, gamgi_bool *error);

static void static_cdata_float (int size, float *array,
FILE *fp, int depth, gamgi_bool *error);

static void static_cdata_int (int size, int *array,
FILE *fp, int depth, gamgi_bool *error);

static void static_cdata (char *name, char *type, int size,
void *array, gamgi_window *window, FILE *fp, int depth_parent,
int *depth_last, gamgi_bool *error);

static void static_config_help (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_text (gamgi_window *window, 
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_orbital (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_bond (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_atom (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_direction (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_plane (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_group (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_molecule (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_cluster (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_cell (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_arrow (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_shape (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_graph (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_assembly (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_light (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_layer (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_window (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config_gamgi (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error);

static void static_config (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error);

static void static_bond_add (gamgi_object *parent, gamgi_bond *bond);

static void static_bond_parent (gamgi_bond *bond, gamgi_window *window,
gamgi_object *scope, int *id, gamgi_bool *error);

static void static_object_text (gamgi_text *text, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_orbital (gamgi_orbital *orbital, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_bond (gamgi_bond *bond, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_atom (gamgi_atom *atom, gamgi_window *window,
gamgi_object *scope, FILE *fp, int depth_parent, int *depth_last, int *id,
gamgi_bool *error);

static void static_object_direction (gamgi_direction *direction, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_plane (gamgi_plane *plane, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_group (gamgi_group *group, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_molecule (gamgi_molecule *molecule, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_cluster (gamgi_cluster *cluster, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_cell (gamgi_cell *cell, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_arrow (gamgi_arrow *arrow, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_shape (gamgi_shape *shape, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_graph (gamgi_graph *graph, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_assembly (gamgi_assembly *assembly, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_light (gamgi_light *light, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_layer (gamgi_layer *layer, 
gamgi_window *window, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error);

static void static_object_window (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_bond_atom (gamgi_atom *atom, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_bond_id (gamgi_bond *bond, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_bond_single (gamgi_bond *bond, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_text_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_orbital_list (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_bond_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_atom_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_direction_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_plane_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_group_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_molecule_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_cluster_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_cell_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_arrow_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_shape_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_graph_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_assembly_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_light_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_layer_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object_window_list (FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error);

static void static_object_gamgi_list (FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error);

void static_object_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error);

static void static_object (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error);

static void static_header_xml (FILE *fp, gamgi_bool *error);

static void static_header_gml (FILE *fp, gamgi_bool *error);

static void static_header_x3d (FILE *fp, gamgi_bool *error);

**********************
 * external functions *
 **********************/

int gamgi_expat_export_length (gamgi_text *text);

void gamgi_expat_export_string (gamgi_text *text, char *buffer);

void gamgi_expat_export_gml (FILE *fp, gamgi_window *window, gamgi_bool *error);

void gamgi_expat_export_x3d (FILE *fp, gamgi_window *window, gamgi_bool *error);

/*************
 * Rationale *
 *************

Last update (lists of objects): July 2011.

 *************
 * Rationale *
 *************/
