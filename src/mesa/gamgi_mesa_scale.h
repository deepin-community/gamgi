/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_scale.h
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

void gamgi_mesa_scale_default (gamgi_object *object, double scale);

void gamgi_mesa_scale_light (gamgi_light *light, double scale);

void gamgi_mesa_scale_window (gamgi_window *window, double scale);

void gamgi_mesa_scale_object (gamgi_object *object, double scale);

void gamgi_mesa_scale_default_list (gamgi_dlist *start, double scale);

void gamgi_mesa_scale_text_list (double scale);

void gamgi_mesa_scale_group_list (double scale);

void gamgi_mesa_scale_cluster_list (double scale);

void gamgi_mesa_scale_assembly_list (double scale);

void gamgi_mesa_scale_light_list (double scale);

void gamgi_mesa_scale_window_list (double scale);

void gamgi_mesa_scale_gamgi_list (double scale);

void gamgi_mesa_scale_list (double scale);

void gamgi_mesa_scale (gamgi_window *window,
gamgi_object *object, double scale);

/*************
 * Rationale *
 *************

Implemented. Last update: May 2011.

 *************
 * Rationale *
 *************/
