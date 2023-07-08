/*********************************************
 *
 * $GAMGI/src/math/gamgi_math_modify.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static int static_mark (gamgi_atom *atom, gamgi_slist *slist_neighbour);

static void static_unmark (gamgi_slist *slist_neighbour);

static void static_translate (gamgi_atom *atom, double *translate);

static void static_translate_list (gamgi_slist *slist_neighbour, double *translate);

static void static_rotate (gamgi_atom *atom, double *origin, double *rotate);

static void static_rotate_list (gamgi_slist *slist_neighbour, double *origin, double *rotate);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_math_modify_get_length_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, double length, double *translate, gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double length, double *translate,
gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_length_atom_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double length, 
double *translate, gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_length_atom_direction (gamgi_atom *atom1,
gamgi_direction *direction1, double length, double *translate, gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_length_atom_plane (gamgi_atom *atom1,
gamgi_plane *plane1, double length, double *translate, gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_angle_3atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, double new,
double *origin, double *transform, gamgi_layer *layer);

gamgi_bool gamgi_math_modify_get_angle_4atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, 
double new, double *origin, double *transform, gamgi_layer *layer);

void gamgi_math_modify_set_length_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, double *translate);

void gamgi_math_modify_set_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *translate);

void gamgi_math_modify_set_length_atom_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double *translate);

void gamgi_math_modify_set_length_atom_direction (gamgi_atom *atom1,
gamgi_direction *direction1, double *translate);

void gamgi_math_modify_set_length_atom_plane (gamgi_atom *atom1,
gamgi_plane *plane1, double *translate);

void gamgi_math_modify_set_angle_3atom (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, double *origin, double *rotate);

void gamgi_math_modify_set_angle_4atom (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_atom *atom4, double *origin, double *rotate);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
