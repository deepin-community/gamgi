/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_filter.h
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

gamgi_dlist *gamgi_engine_filter_bond_length (gamgi_dlist *start,
double min, double max);

gamgi_dlist *gamgi_engine_filter_bond_element (gamgi_dlist *start,
int z1, int z2);

gamgi_dlist *gamgi_engine_filter_atom_element (gamgi_dlist *start, 
int z1, int z2);

gamgi_dlist *gamgi_engine_filter_atom_class (gamgi_dlist *start,
int offset, gamgi_enum class);

gamgi_dlist *gamgi_engine_filter_atom_parent (gamgi_dlist *start,
int offset, gamgi_object *parent);

gamgi_dlist *gamgi_engine_filter_atom_identical (gamgi_dlist *start,
int offset);

gamgi_dlist *gamgi_engine_filter_atom (gamgi_dlist *start,
int offset, gamgi_enum filter, gamgi_enum class, gamgi_object *parent);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
