/*********************************************
 *
 * $GAMGI/src/engine/gamgi_engine_copy_out.h
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

static void static_copy (gamgi_object *object, gamgi_object *object_new);

 **********************
 * external functions *
 **********************/

void gamgi_engine_copy_out_text (gamgi_text *text, gamgi_text *text_new);

void gamgi_engine_copy_out_orbital (gamgi_orbital *orbital, gamgi_orbital *orbital_new);

void gamgi_engine_copy_out_bond (gamgi_bond *bond, gamgi_bond *bond_new);

void gamgi_engine_copy_out_atom (gamgi_atom *atom, gamgi_atom *atom_new);

void gamgi_engine_copy_out_direction (gamgi_direction *direction, gamgi_direction *direction_new);

void gamgi_engine_copy_out_plane (gamgi_plane *plane, gamgi_plane *plane_new);

void gamgi_engine_copy_out_group (gamgi_group *group, gamgi_group *group_new);

void gamgi_engine_copy_out_molecule (gamgi_molecule *molecule, gamgi_molecule *molecule_new);

void gamgi_engine_copy_out_cluster (gamgi_cluster *cluster, gamgi_cluster *cluster_new);

void gamgi_engine_copy_out_cell (gamgi_cell *cell, gamgi_cell *cell_new);

void gamgi_engine_copy_out_arrow (gamgi_arrow *arrow, gamgi_arrow *arrow_new);

void gamgi_engine_copy_out_shape (gamgi_shape *shape, gamgi_shape *shape_new);

void gamgi_engine_copy_out_graph (gamgi_graph *graph, gamgi_graph *graph_new);

void gamgi_engine_copy_out_assembly (gamgi_assembly *assembly, gamgi_assembly *assembly_new);

void gamgi_engine_copy_out_light (gamgi_light *light, gamgi_light *light_new);

void gamgi_engine_copy_out_layer (gamgi_layer *layer, gamgi_layer *layer_new);

void gamgi_engine_copy_out_window (gamgi_window *window, gamgi_window *window_new);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
