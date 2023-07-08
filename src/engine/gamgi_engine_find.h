/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_find.h
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

gamgi_bool gamgi_engine_find_object (gamgi_object *object,
gamgi_object *parent);

gamgi_layer *gamgi_engine_find_layer (gamgi_object *object);

gamgi_window *gamgi_engine_find_window (gamgi_object *object);

gamgi_object *gamgi_engine_find_root (gamgi_object *object1,
gamgi_object *object2, gamgi_object *scope);

gamgi_text *gamgi_engine_find_text_text (unsigned int number, 
gamgi_text *text);

gamgi_text *gamgi_engine_find_text_orbital (unsigned int number, 
gamgi_orbital *orbital);

gamgi_text *gamgi_engine_find_text_bond (unsigned int number, 
gamgi_bond *bond);

gamgi_text *gamgi_engine_find_text_atom (unsigned int number, 
gamgi_atom *atom);

gamgi_text *gamgi_engine_find_text_direction (unsigned int number, 
gamgi_direction *direction);

gamgi_text *gamgi_engine_find_text_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_text *gamgi_engine_find_text_group (unsigned int number, 
gamgi_group *group);

gamgi_text *gamgi_engine_find_text_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_text *gamgi_engine_find_text_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_text *gamgi_engine_find_text_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_text *gamgi_engine_find_text_arrow (unsigned int number, 
gamgi_arrow *arrow);

gamgi_text *gamgi_engine_find_text_shape (unsigned int number, 
gamgi_shape *shape);

gamgi_text *gamgi_engine_find_text_graph (unsigned int number, 
gamgi_graph *graph);

gamgi_text *gamgi_engine_find_text_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_text *gamgi_engine_find_text_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_text *gamgi_engine_find_text_window (unsigned int number, 
gamgi_window *window);

gamgi_text *gamgi_engine_find_text_gamgi (unsigned int number); 

gamgi_orbital *gamgi_engine_find_orbital_orbital (unsigned int number, 
gamgi_orbital *orbital);

gamgi_orbital *gamgi_engine_find_orbital_bond (unsigned int number, 
gamgi_bond *bond);

gamgi_orbital *gamgi_engine_find_orbital_atom (unsigned int number, 
gamgi_atom *atom);

gamgi_orbital *gamgi_engine_find_orbital_direction (unsigned int number, 
gamgi_direction *direction);

gamgi_orbital *gamgi_engine_find_orbital_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_orbital *gamgi_engine_find_orbital_group (unsigned int number, 
gamgi_group *group);

gamgi_orbital *gamgi_engine_find_orbital_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_orbital *gamgi_engine_find_orbital_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_orbital *gamgi_engine_find_orbital_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_orbital *gamgi_engine_find_orbital_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_orbital *gamgi_engine_find_orbital_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_orbital *gamgi_engine_find_orbital_window (unsigned int number, 
gamgi_window *window);

gamgi_orbital *gamgi_engine_find_orbital_gamgi (unsigned int number); 

gamgi_atom *gamgi_engine_find_atom_atom (unsigned int number, 
gamgi_atom *atom);

gamgi_atom *gamgi_engine_find_atom_direction (unsigned int number, 
gamgi_direction *direction);

gamgi_atom *gamgi_engine_find_atom_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_atom *gamgi_engine_find_atom_group (unsigned int number, 
gamgi_group *group);

gamgi_atom *gamgi_engine_find_atom_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_atom *gamgi_engine_find_atom_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_atom *gamgi_engine_find_atom_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_atom *gamgi_engine_find_atom_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_atom *gamgi_engine_find_atom_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_atom *gamgi_engine_find_atom_window (unsigned int number, 
gamgi_window *window);

gamgi_atom *gamgi_engine_find_atom_gamgi (unsigned int number); 

gamgi_bond *gamgi_engine_find_bond_atom (unsigned int number, 
gamgi_atom *atom);

gamgi_bond *gamgi_engine_find_bond_direction (unsigned int number, 
gamgi_direction *direction);

gamgi_bond *gamgi_engine_find_bond_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_bond *gamgi_engine_find_bond_group (unsigned int number, 
gamgi_group *group);

gamgi_bond *gamgi_engine_find_bond_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_bond *gamgi_engine_find_bond_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_bond *gamgi_engine_find_bond_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_bond *gamgi_engine_find_bond_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_bond *gamgi_engine_find_bond_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_bond *gamgi_engine_find_bond_window (unsigned int number, 
gamgi_window *window);

gamgi_bond *gamgi_engine_find_bond_gamgi (unsigned int number); 

gamgi_direction *gamgi_engine_find_direction_direction (unsigned int number, 
gamgi_direction *direction);

gamgi_direction *gamgi_engine_find_direction_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_direction *gamgi_engine_find_direction_group (unsigned int number, 
gamgi_group *group);

gamgi_direction *gamgi_engine_find_direction_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_direction *gamgi_engine_find_direction_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_direction *gamgi_engine_find_direction_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_direction *gamgi_engine_find_direction_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_direction *gamgi_engine_find_direction_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_direction *gamgi_engine_find_direction_window (unsigned int number, 
gamgi_window *window);

gamgi_direction *gamgi_engine_find_direction_gamgi (unsigned int number); 

gamgi_plane *gamgi_engine_find_plane_plane (unsigned int number, 
gamgi_plane *plane);

gamgi_plane *gamgi_engine_find_plane_group (unsigned int number, 
gamgi_group *group);

gamgi_plane *gamgi_engine_find_plane_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_plane *gamgi_engine_find_plane_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_plane *gamgi_engine_find_plane_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_plane *gamgi_engine_find_plane_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_plane *gamgi_engine_find_plane_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_plane *gamgi_engine_find_plane_window (unsigned int number, 
gamgi_window *window);

gamgi_plane *gamgi_engine_find_plane_gamgi (unsigned int number); 

gamgi_group *gamgi_engine_find_group_group (unsigned int number, 
gamgi_group *group);

gamgi_group *gamgi_engine_find_group_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_group *gamgi_engine_find_group_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_group *gamgi_engine_find_group_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_group *gamgi_engine_find_group_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_group *gamgi_engine_find_group_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_group *gamgi_engine_find_group_window (unsigned int number, 
gamgi_window *window);

gamgi_group *gamgi_engine_find_group_gamgi (unsigned int number); 

gamgi_molecule *gamgi_engine_find_molecule_molecule (unsigned int number, 
gamgi_molecule *molecule);

gamgi_molecule *gamgi_engine_find_molecule_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_molecule *gamgi_engine_find_molecule_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_molecule *gamgi_engine_find_molecule_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_molecule *gamgi_engine_find_molecule_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_molecule *gamgi_engine_find_molecule_window (unsigned int number, 
gamgi_window *window);

gamgi_molecule *gamgi_engine_find_molecule_gamgi (unsigned int number); 

gamgi_cluster *gamgi_engine_find_cluster_cluster (unsigned int number, 
gamgi_cluster *cluster);

gamgi_cluster *gamgi_engine_find_cluster_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_cluster *gamgi_engine_find_cluster_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_cluster *gamgi_engine_find_cluster_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_cluster *gamgi_engine_find_cluster_window (unsigned int number, 
gamgi_window *window);

gamgi_cluster *gamgi_engine_find_cluster_gamgi (unsigned int number); 

gamgi_cell *gamgi_engine_find_cell_cell (unsigned int number, 
gamgi_cell *cell);

gamgi_cell *gamgi_engine_find_cell_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_cell *gamgi_engine_find_cell_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_cell *gamgi_engine_find_cell_window (unsigned int number, 
gamgi_window *window);

gamgi_cell *gamgi_engine_find_cell_gamgi (unsigned int number); 

gamgi_arrow *gamgi_engine_find_arrow_arrow (unsigned int number, 
gamgi_arrow *arrow);

gamgi_arrow *gamgi_engine_find_arrow_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_arrow *gamgi_engine_find_arrow_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_arrow *gamgi_engine_find_arrow_window (unsigned int number, 
gamgi_window *window);

gamgi_arrow *gamgi_engine_find_arrow_gamgi (unsigned int number); 

gamgi_shape *gamgi_engine_find_shape_shape (unsigned int number, 
gamgi_shape *shape);

gamgi_shape *gamgi_engine_find_shape_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_shape *gamgi_engine_find_shape_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_shape *gamgi_engine_find_shape_window (unsigned int number, 
gamgi_window *window);

gamgi_shape *gamgi_engine_find_shape_gamgi (unsigned int number); 

gamgi_graph *gamgi_engine_find_graph_graph (unsigned int number, 
gamgi_graph *graph);

gamgi_graph *gamgi_engine_find_graph_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_graph *gamgi_engine_find_graph_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_graph *gamgi_engine_find_graph_window (unsigned int number, 
gamgi_window *window);

gamgi_graph *gamgi_engine_find_graph_gamgi (unsigned int number); 

gamgi_assembly *gamgi_engine_find_assembly_assembly (unsigned int number, 
gamgi_assembly *assembly);

gamgi_assembly *gamgi_engine_find_assembly_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_assembly *gamgi_engine_find_assembly_window (unsigned int number, 
gamgi_window *window);

gamgi_assembly *gamgi_engine_find_assembly_gamgi (unsigned int number); 

gamgi_light *gamgi_engine_find_light_light (unsigned int number, 
gamgi_light *light);

gamgi_light *gamgi_engine_find_light_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_light *gamgi_engine_find_light_window (unsigned int number, 
gamgi_window *window);

gamgi_light *gamgi_engine_find_light_gamgi (unsigned int number); 

gamgi_layer *gamgi_engine_find_layer_layer (unsigned int number, 
gamgi_layer *layer);

gamgi_layer *gamgi_engine_find_layer_window (unsigned int number, 
gamgi_window *window);

gamgi_layer *gamgi_engine_find_layer_gamgi (unsigned int number); 

gamgi_window *gamgi_engine_find_window_window (unsigned int number, 
gamgi_window *window);

gamgi_window *gamgi_engine_find_window_gamgi (unsigned int number);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
