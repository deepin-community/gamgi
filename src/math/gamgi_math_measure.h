/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_measure.h
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

double static_atom_rotation (double *v1, double *v2, gamgi_layer *layer);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_math_measure_length_2atom (gamgi_atom *atom1, 
gamgi_atom *atom2, double *length);

gamgi_bool gamgi_math_measure_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *length);

gamgi_bool gamgi_math_measure_length_atom_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double *length);

gamgi_bool gamgi_math_measure_length_atom_direction (gamgi_atom *atom, 
gamgi_direction *direction, double *length);

gamgi_bool gamgi_math_measure_length_atom_plane (gamgi_atom *atom,
gamgi_plane *plane, double *length);

gamgi_bool gamgi_math_measure_length_direction (gamgi_direction *direction,
double *length);

gamgi_bool gamgi_math_measure_length_node_direction (gamgi_direction *direction,
double *length);

gamgi_bool gamgi_math_measure_length_plane (gamgi_plane *plane,
double *length);

gamgi_bool gamgi_math_measure_area_plane (gamgi_plane *plane, double *area);

gamgi_bool gamgi_math_measure_area_node_plane (gamgi_plane *plane, double *area);

gamgi_bool gamgi_math_measure_angle_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *angle, gamgi_layer *layer);

gamgi_bool gamgi_math_measure_angle_4atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4,
double *angle, gamgi_layer *layer);

gamgi_bool gamgi_math_measure_angle_2direction (gamgi_direction *direction1,
gamgi_direction *direction2, double *angle);

gamgi_bool gamgi_math_measure_angle_2plane (gamgi_plane *plane1, 
gamgi_plane *plane2, double *angle);

gamgi_bool gamgi_math_measure_angle_plane_direction (gamgi_plane *plane,
gamgi_direction *direction, double *angle);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
