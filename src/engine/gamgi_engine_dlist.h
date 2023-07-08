/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_dlist.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

gamgi_dlist *gamgi_engine_dlist_text_text (gamgi_text *text, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_orbital (gamgi_orbital *orbital, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_bond (gamgi_bond *bond, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_atom (gamgi_atom *atom, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_direction (gamgi_direction *direction, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_arrow (gamgi_arrow *arrow, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_shape (gamgi_shape *shape, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_graph (gamgi_graph *graph, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_text_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_orbital_orbital (gamgi_orbital *orbital, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_bond (gamgi_bond *bond, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_atom (gamgi_atom *atom, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_direction (gamgi_direction *direction, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_orbital_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_bond_bond (gamgi_bond *bond, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_atom (gamgi_atom *atom, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_direction (gamgi_direction *direction, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_bond_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_atom_atom (gamgi_atom *atom, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_direction (gamgi_direction *direction, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_atom_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_bond_gamgi (gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_direction (gamgi_direction *direction, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_direction_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_plane_plane (gamgi_plane *plane, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_plane_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_group_group (gamgi_group *group, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_group_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_molecule_molecule (gamgi_molecule *molecule, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_molecule_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_cluster_cluster (gamgi_cluster *cluster, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cluster_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cluster_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cluster_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cluster_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cluster_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_cell_cell (gamgi_cell *cell, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cell_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cell_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cell_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_cell_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_arrow_arrow (gamgi_arrow *arrow, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_arrow_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_arrow_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_arrow_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_arrow_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_shape_shape (gamgi_shape *shape, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_shape_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_shape_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_shape_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_shape_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_graph_graph (gamgi_graph *graph, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_graph_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_graph_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_graph_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_graph_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_assembly_assembly (gamgi_assembly *assembly, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_assembly_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_assembly_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_assembly_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_light_light (gamgi_light *light, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_light_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_light_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_light_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_layer_layer (gamgi_layer *layer, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_layer_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_layer_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_window_window (gamgi_window *window, 
gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_window_gamgi (gamgi_dlist *dlist_start);


gamgi_dlist *gamgi_engine_dlist_atom_object (gamgi_object *object,
gamgi_dlist *dlist_start);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
