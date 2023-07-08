/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_pick.h
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

gamgi_object **gamgi_mesa_pick_text (gamgi_text *text, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_orbital (gamgi_orbital *orbital, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_bond (gamgi_bond *bond, 
gamgi_atom *atom, int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_atom (gamgi_atom *atom, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_direction (gamgi_direction *direction, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_plane (gamgi_plane *plane, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_group (gamgi_group *group, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_molecule (gamgi_molecule *molecule, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_cluster (gamgi_cluster *cluster, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_cell (gamgi_cell *cell, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_arrow (gamgi_arrow *arrow, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_shape (gamgi_shape *shape, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_graph (gamgi_graph *graph, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_assembly (gamgi_assembly *assembly, 
int class, gamgi_object **array);

gamgi_object **gamgi_mesa_pick_layer (gamgi_layer *layer,
gamgi_window *window, int class, gamgi_object **array);

void gamgi_mesa_pick_window (gamgi_window *window,
int class, gamgi_object **array);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
