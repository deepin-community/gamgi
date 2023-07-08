/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_link.h
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

void gamgi_engine_link_text_text (gamgi_text *child, 
gamgi_text *parent);

void gamgi_engine_link_text_orbital (gamgi_text *text, 
gamgi_orbital *orbital);

void gamgi_engine_link_text_bond (gamgi_text *text, 
gamgi_bond *bond);

void gamgi_engine_link_text_atom (gamgi_text *text, 
gamgi_atom *atom);

void gamgi_engine_link_text_direction (gamgi_text *text, 
gamgi_direction *direction);

void gamgi_engine_link_text_plane (gamgi_text *text, 
gamgi_plane *plane);

void gamgi_engine_link_text_group (gamgi_text *text, 
gamgi_group *group);

void gamgi_engine_link_text_molecule (gamgi_text *text, 
gamgi_molecule *molecule);

void gamgi_engine_link_text_cluster (gamgi_text *text, 
gamgi_cluster *cluster);

void gamgi_engine_link_text_cell (gamgi_text *text, 
gamgi_cell *cell);

void gamgi_engine_link_text_arrow (gamgi_text *text, 
gamgi_arrow *arrow);

void gamgi_engine_link_text_shape (gamgi_text *text, 
gamgi_shape *shape);

void gamgi_engine_link_text_graph (gamgi_text *text, 
gamgi_graph *graph);

void gamgi_engine_link_text_assembly (gamgi_text *text, 
gamgi_assembly *assembly);

void gamgi_engine_link_text_layer (gamgi_text *text, 
gamgi_layer *layer);

void gamgi_engine_link_orbital_bond (gamgi_orbital *orbital, 
gamgi_bond *bond);

void gamgi_engine_link_orbital_atom (gamgi_orbital *orbital, 
gamgi_atom *atom);

void gamgi_engine_link_orbital_direction (gamgi_orbital *orbital, 
gamgi_direction *direction);

void gamgi_engine_link_orbital_plane (gamgi_orbital *orbital, 
gamgi_plane *plane);

void gamgi_engine_link_orbital_group (gamgi_orbital *orbital, 
gamgi_group *group);

void gamgi_engine_link_orbital_molecule (gamgi_orbital *orbital, 
gamgi_molecule *molecule);

void gamgi_engine_link_orbital_cluster (gamgi_orbital *orbital, 
gamgi_cluster *cluster);

void gamgi_engine_link_orbital_cell (gamgi_orbital *orbital, 
gamgi_cell *cell);

void gamgi_engine_link_orbital_assembly (gamgi_orbital *orbital, 
gamgi_assembly *assembly);

void gamgi_engine_link_orbital_layer (gamgi_orbital *orbital, 
gamgi_layer *layer);

void gamgi_engine_link_bond_atom (gamgi_bond *bond, 
gamgi_atom *atom);

void gamgi_engine_link_atom_direction (gamgi_atom *atom, 
gamgi_direction *direction);

void gamgi_engine_link_atom_plane (gamgi_atom *atom, 
gamgi_plane *plane);

void gamgi_engine_link_atom_group (gamgi_atom *atom, 
gamgi_group *group);

void gamgi_engine_link_atom_molecule (gamgi_atom *atom, 
gamgi_molecule *molecule);

void gamgi_engine_link_atom_cluster (gamgi_atom *atom, 
gamgi_cluster *cluster);

void gamgi_engine_link_atom_cell (gamgi_atom *atom, 
gamgi_cell *cell);

void gamgi_engine_link_atom_assembly (gamgi_atom *atom, 
gamgi_assembly *assembly);

void gamgi_engine_link_atom_layer (gamgi_atom *atom, 
gamgi_layer *layer);

void gamgi_engine_link_direction_plane (gamgi_direction *direction,
gamgi_plane *plane);

void gamgi_engine_link_direction_group (gamgi_direction *direction, 
gamgi_group *group);

void gamgi_engine_link_direction_molecule (gamgi_direction *direction, 
gamgi_molecule *molecule);

void gamgi_engine_link_direction_cluster (gamgi_direction *direction, 
gamgi_cluster *cluster);

void gamgi_engine_link_direction_cell (gamgi_direction *direction, 
gamgi_cell *cell);

void gamgi_engine_link_direction_assembly (gamgi_direction *direction, 
gamgi_assembly *assembly);

void gamgi_engine_link_direction_layer (gamgi_direction *direction, 
gamgi_layer *layer);

void gamgi_engine_link_plane_group (gamgi_plane *plane, 
gamgi_group *group);

void gamgi_engine_link_plane_molecule (gamgi_plane *plane, 
gamgi_molecule *molecule);

void gamgi_engine_link_plane_cluster (gamgi_plane *plane, 
gamgi_cluster *cluster);

void gamgi_engine_link_plane_cell (gamgi_plane *plane, 
gamgi_cell *cell);

void gamgi_engine_link_plane_assembly (gamgi_plane *plane, 
gamgi_assembly *assembly);

void gamgi_engine_link_plane_layer (gamgi_plane *plane, 
gamgi_layer *layer);

void gamgi_engine_link_group_group (gamgi_group *child, 
gamgi_group *parent);

void gamgi_engine_link_group_molecule (gamgi_group *group, 
gamgi_molecule *molecule);

void gamgi_engine_link_group_cluster (gamgi_group *group, 
gamgi_cluster *cluster);

void gamgi_engine_link_group_cell (gamgi_group *group, 
gamgi_cell *cell);

void gamgi_engine_link_group_assembly (gamgi_group *group, 
gamgi_assembly *assembly);

void gamgi_engine_link_group_layer (gamgi_group *group, 
gamgi_layer *layer);

void gamgi_engine_link_molecule_cluster (gamgi_molecule *molecule, 
gamgi_cluster *cluster);

void gamgi_engine_link_molecule_cell (gamgi_molecule *molecule, 
gamgi_cell *cell);

void gamgi_engine_link_molecule_assembly (gamgi_molecule *molecule, 
gamgi_assembly *assembly);

void gamgi_engine_link_molecule_layer (gamgi_molecule *molecule, 
gamgi_layer *layer);

void gamgi_engine_link_cluster_cluster (gamgi_cluster *child, 
gamgi_cluster *parent);

void gamgi_engine_link_cluster_cell (gamgi_cluster *cluster, 
gamgi_cell *cell);

void gamgi_engine_link_cluster_assembly (gamgi_cluster *cluster, 
gamgi_assembly *assembly);

void gamgi_engine_link_cluster_layer (gamgi_cluster *cluster, 
gamgi_layer *layer);

void gamgi_engine_link_cell_assembly (gamgi_cell *cell, 
gamgi_assembly *assembly);

void gamgi_engine_link_cell_layer (gamgi_cell *cell, 
gamgi_layer *layer);

void gamgi_engine_link_arrow_assembly (gamgi_arrow *arrow, 
gamgi_assembly *assembly);

void gamgi_engine_link_arrow_layer (gamgi_arrow *arrow, 
gamgi_layer *layer);

void gamgi_engine_link_shape_assembly (gamgi_shape *shape, 
gamgi_assembly *assembly);

void gamgi_engine_link_shape_layer (gamgi_shape *shape, 
gamgi_layer *layer);

void gamgi_engine_link_graph_assembly (gamgi_graph *graph, 
gamgi_assembly *assembly);

void gamgi_engine_link_graph_layer (gamgi_graph *graph, 
gamgi_layer *layer);

void gamgi_engine_link_assembly_assembly (gamgi_assembly *child, 
gamgi_assembly *parent);

void gamgi_engine_link_assembly_layer (gamgi_assembly *assembly, 
gamgi_layer *layer);

void gamgi_engine_link_light_layer (gamgi_light *light, 
gamgi_layer *layer);

void gamgi_engine_link_layer_window (gamgi_layer *layer, 
gamgi_window *window);

void gamgi_engine_link_object_bond (gamgi_object *object, 
gamgi_bond *bond);

void gamgi_engine_link_object_atom (gamgi_object *object, 
gamgi_atom *atom);

void gamgi_engine_link_object_direction (gamgi_object *object, 
gamgi_direction *direction);

void gamgi_engine_link_object_plane (gamgi_object *object, 
gamgi_plane *plane);

void gamgi_engine_link_object_group (gamgi_object *object, 
gamgi_group *group);
  
void gamgi_engine_link_object_molecule (gamgi_object *object, 
gamgi_molecule *molecule);

void gamgi_engine_link_object_cluster (gamgi_object *object, 
gamgi_cluster *cluster);

void gamgi_engine_link_object_cell (gamgi_object *object, 
gamgi_cell *cell);

void gamgi_engine_link_object_assembly (gamgi_object *object, 
gamgi_assembly *assembly);

void gamgi_engine_link_object_layer (gamgi_object *object, 
gamgi_layer *layer);

void gamgi_engine_link_object_object (gamgi_object *child, 
gamgi_object *parent);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
