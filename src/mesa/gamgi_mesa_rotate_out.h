/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_rotate_out.h
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

void gamgi_mesa_rotate_out_text (gamgi_text *text,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_orbital (gamgi_orbital *orbital,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_atom (gamgi_atom *atom,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_bond (gamgi_bond *bond,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_direction (gamgi_direction *direction,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_plane (gamgi_plane *plane,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_group (gamgi_group *group,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_molecule (gamgi_molecule *molecule,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_cluster (gamgi_cluster *cluster,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_cell (gamgi_cell *cell,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_arrow (gamgi_arrow *arrow,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_shape (gamgi_shape *shape,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_graph (gamgi_graph *graph,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_assembly (gamgi_assembly *assembly,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_light (gamgi_light *light,  
double *quaternion, double *rotate, double *center);

void gamgi_mesa_rotate_out_layer (gamgi_layer *layer,  
double *quaternion, double *rotate, double *center);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
