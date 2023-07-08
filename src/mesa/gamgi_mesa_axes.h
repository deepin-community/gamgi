/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_axes.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_sphere (float size);

static void static_axis (double *v, float size);

 **********************
 * external functions *
 **********************/

void gamgi_mesa_axes (double *origin,
double *a, double *b, double *c, float size);

void gamgi_mesa_axes_color (double *origin,
double *a, double *b, double *c, float size);

void gamgi_mesa_axes_orbital (gamgi_orbital *orbital,
gamgi_bool color, float size);

void gamgi_mesa_axes_cell (gamgi_cell *cell, gamgi_bool color, float size);

void gamgi_mesa_axes_layer (gamgi_layer *layer, gamgi_bool color, float size);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
