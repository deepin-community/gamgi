/********************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_translate.h
 *
 * Copyright (C); 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_translate (gamgi_layer *layer, double *shift, double *translate);

static void static_layer (gamgi_object *object, gamgi_layer **layer_old,
double *shift, double *translate);

 **********************
 * external functions *
 **********************/

void gamgi_mesa_translate_text (gamgi_text *text, double *translate);

void gamgi_mesa_translate_orbital (gamgi_orbital *orbital, double *translate);

void gamgi_mesa_translate_bond (gamgi_bond *bond, double *translate);

void gamgi_mesa_translate_atom (gamgi_atom *atom, double *translate);

void gamgi_mesa_translate_direction (gamgi_direction *direction, double *translate);

void gamgi_mesa_translate_plane (gamgi_plane *plane, double *translate);

void gamgi_mesa_translate_group (gamgi_group *group, double *translate);

void gamgi_mesa_translate_molecule (gamgi_molecule *molecule, double *translate);

void gamgi_mesa_translate_cluster (gamgi_cluster *cluster, double *translate);

void gamgi_mesa_translate_cell (gamgi_cell *cell, double *translate);

void gamgi_mesa_translate_arrow (gamgi_arrow *arrow, double *translate);

void gamgi_mesa_translate_shape (gamgi_shape *shape, double *translate);

void gamgi_mesa_translate_graph (gamgi_graph *graph, double *translate);

void gamgi_mesa_translate_assembly (gamgi_assembly *assembly, double *translate);

void gamgi_mesa_translate_light (gamgi_light *light, double *translate);

void gamgi_mesa_translate_layer (gamgi_layer *layer, double *translate);

void gamgi_mesa_translate_object (gamgi_object *object, double *translate);

void gamgi_mesa_translate_text_list (double *shift);

void gamgi_mesa_translate_orbital_list (double *shift);

void gamgi_mesa_translate_bond_list (double *shift);

void gamgi_mesa_translate_atom_list (double *shift);

void gamgi_mesa_translate_direction_list (double *shift);

void gamgi_mesa_translate_plane_list (double *shift);

void gamgi_mesa_translate_group_list (double *shift);

void gamgi_mesa_translate_molecule_list (double *shift);

void gamgi_mesa_translate_cluster_list (double *shift);

void gamgi_mesa_translate_cell_list (double *shift);

void gamgi_mesa_translate_arrow_list (double *shift);

void gamgi_mesa_translate_shape_list (double *shift);

void gamgi_mesa_translate_graph_list (double *shift);

void gamgi_mesa_translate_assembly_list (double *shift);

void gamgi_mesa_translate_light_list (double *shift);

void gamgi_mesa_translate_layer_in (gamgi_layer *layer,
gamgi_object *object, double *shift);

void gamgi_mesa_translate_layer_out (gamgi_layer *layer, double *shift);

void gamgi_mesa_translate_layer_list (double *shift);

void gamgi_mesa_translate_window_in (gamgi_window *window, double *shift);

void gamgi_mesa_translate_window_out (gamgi_window *window, double *shift);

void gamgi_mesa_translate_window_list (double *shift);

void gamgi_mesa_translate_gamgi_list (double *shift);

void gamgi_mesa_translate_list (double *shift);

void gamgi_mesa_translate (gamgi_window *window,
gamgi_object *object, double *shift);

/*************
 * Rationale *
 *************

Implemented. Last update: May 2011.

 *************
 * Rationale *
 *************/
