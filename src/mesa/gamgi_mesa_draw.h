/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_draw.h
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

void gamgi_mesa_draw_text (gamgi_text *text);

void gamgi_mesa_draw_orbital (gamgi_orbital *orbital);

void gamgi_mesa_draw_bond (gamgi_bond *bond, gamgi_atom *atom);

void gamgi_mesa_draw_atom (gamgi_atom *atom);

void gamgi_mesa_draw_direction (gamgi_direction *direction);

void gamgi_mesa_draw_plane (gamgi_plane *plane);

void gamgi_mesa_draw_group (gamgi_group *group);

void gamgi_mesa_draw_molecule (gamgi_molecule *molecule);

void gamgi_mesa_draw_cluster (gamgi_cluster *cluster);

void gamgi_mesa_draw_cell (gamgi_cell *cell);

void gamgi_mesa_draw_arrow (gamgi_arrow *arrow);

void gamgi_mesa_draw_shape (gamgi_shape *shape);

void gamgi_mesa_draw_graph (gamgi_graph *graph);

void gamgi_mesa_draw_assembly (gamgi_assembly *assembly);

void gamgi_mesa_draw_light (gamgi_light *light, int offset);

void gamgi_mesa_draw_layer (gamgi_layer *layer);

void gamgi_mesa_draw_window (gamgi_window *window);

void gamgi_mesa_draw_gamgi (void);

/*************
 * Rationale *
 *************

Always in progress. These functions are the basic GAMGI graphic engine.
_draw_gamgi draws everything by calling _draw_window for each window, 
which calls _draw_layer for each layer, which in turn call all of the 
other functions, from _draw_light to _draw_text. Each parent calls the 
required functions to draw its child, which are always located above, 
so gamgi_mesa_draw.c doesn't include gamgi_mesa_draw.h.

 *************
 * Rationale *
 *************/
