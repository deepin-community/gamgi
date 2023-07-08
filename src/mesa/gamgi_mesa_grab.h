/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_region.h
 *
 * Copyright (C) 2010 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

int gamgi_mesa_grab_text (gamgi_text *text, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_orbital (gamgi_orbital *orbital, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_bond (gamgi_bond *bond, gamgi_atom *atom,
int class, int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_atom (gamgi_atom *atom, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_direction (gamgi_direction *direction, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_plane (gamgi_plane *plane, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_group (gamgi_group *group, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_molecule (gamgi_molecule *molecule, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_cluster (gamgi_cluster *cluster, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_cell (gamgi_cell *cell, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_arrow (gamgi_arrow *arrow, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_shape (gamgi_shape *shape, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_graph (gamgi_graph *graph, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_assembly (gamgi_assembly *assembly, int class,
int level, int offset, gamgi_object **objects, int *levels);

int gamgi_mesa_grab_layer (gamgi_layer *layer, gamgi_window *window,
int class, int level, int offset, gamgi_object **objects, int *levels);

void gamgi_mesa_grab_window (gamgi_window *window, int class,
int level, int offset, gamgi_object **objects, int *levels);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
