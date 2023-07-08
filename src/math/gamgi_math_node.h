/******************************************
 *
 * $GAMGI/src/math/gamgi_math_node.h
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

static gamgi_bool static_decrease (gamgi_enum lattice, int *in, int *out);

static int static_increase (gamgi_enum lattice, int *in, int *out);

static gamgi_bool static_centered (int *hkl, int order, gamgi_enum value);

static gamgi_bool static_primitive (gamgi_enum lattice,
int *hkl, int order, int o, gamgi_enum value);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_math_node_check (gamgi_enum lattice, gamgi_enum node);

void gamgi_math_node_primitive (gamgi_enum lattice,
int c1, int c2, int c3, int c4, int *p1, int *p2, int *p3);

void gamgi_math_node_conventional (gamgi_enum lattice,
int p1, int p2, int p3, int *c1, int *c2, int *c3, int *c4);

gamgi_bool gamgi_math_node_direction (gamgi_direction *direction, int o, int value);

gamgi_bool gamgi_math_node_plane (gamgi_plane *plane, int o, int value);

/*************
 * Rationale *
 *************

Done.

 *************
 * Rationale *
 *************/
