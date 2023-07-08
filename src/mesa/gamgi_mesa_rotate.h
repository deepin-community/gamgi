/********************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_rotate.h
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

static void static_center (gamgi_object *object, double *center);

static void static_rotate (gamgi_layer *layer, double angle,
double *axis, double *quaternion, double *rotate);

static void static_layer (gamgi_object *object, gamgi_layer **layer_old,
double angle, double *axis, double *quaternion, double *rotate);

**********************
 * external functions *
 **********************/

void gamgi_mesa_rotate_text (gamgi_text *text,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_orbital (gamgi_orbital *orbital,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_bond (gamgi_bond *bond,
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_atom (gamgi_atom *atom,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_direction (gamgi_direction *direction,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_plane (gamgi_plane *plane,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_group (gamgi_group *group,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_molecule (gamgi_molecule *molecule,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_cluster (gamgi_cluster *cluster,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_cell (gamgi_cell *cell,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_arrow (gamgi_arrow *arrow,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_shape (gamgi_shape *shape,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_graph (gamgi_graph *graph,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_assembly (gamgi_assembly *assembly,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_light (gamgi_light *light,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_layer (gamgi_layer *layer,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_object (gamgi_object *object,
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_text_list (double angle, double *axis);

void gamgi_mesa_rotate_orbital_list (double angle, double *axis);

void gamgi_mesa_rotate_bond_list (double angle, double *axis);

void gamgi_mesa_rotate_atom_list (double angle, double *axis);

void gamgi_mesa_rotate_direction_list (double angle, double *axis);

void gamgi_mesa_rotate_plane_list (double angle, double *axis);

void gamgi_mesa_rotate_group_list (double angle, double *axis);

void gamgi_mesa_rotate_molecule_list (double angle, double *axis);

void gamgi_mesa_rotate_cluster_list (double angle, double *axis);

void gamgi_mesa_rotate_cell_list (double angle, double *axis);

void gamgi_mesa_rotate_arrow_list (double angle, double *axis);

void gamgi_mesa_rotate_shape_list (double angle, double *axis);

void gamgi_mesa_rotate_graph_list (double angle, double *axis);

void gamgi_mesa_rotate_assembly_list (double angle, double *axis);

void gamgi_mesa_rotate_light_list (double angle, double *axis);

void gamgi_mesa_rotate_layer_in (gamgi_layer *layer, 
gamgi_object *object, double angle, double *axis);

void gamgi_mesa_rotate_layer_out (gamgi_layer *layer, 
double angle, double *axis);

void gamgi_mesa_rotate_layer_list (double angle, double *axis);

void gamgi_mesa_rotate_window_in (gamgi_window *window, double angle,
double *axis);

void gamgi_mesa_rotate_window_out (gamgi_window *window, double angle,
double *axis);

void gamgi_mesa_rotate_window_list (double angle, double *axis);

void gamgi_mesa_rotate_gamgi_list (double angle, double *axis);

void gamgi_mesa_rotate_list (double angle, double *axis);

void gamgi_mesa_rotate (gamgi_window *window,
gamgi_object *object, double angle, double *axis);

/*************
 * Rationale *
 *************

Implemented. Last update: May 2011,

 *************
 * Rationale *
 *************/
