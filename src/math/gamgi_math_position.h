/******************************************
 *
 * $GAMGI/src/math/gamgi_math_position.h
 *
 * Copyright (C) 2007 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

 **********************
 * external functions *
 **********************/

void gamgi_math_position_rotation (gamgi_object *object, double *rotation);

void gamgi_math_position_translation (gamgi_object *object, double *translation);

void gamgi_math_position_object_rotation (gamgi_object *object, double *rotation);

void gamgi_math_position_object_translation (gamgi_object *object, double *translation);

void gamgi_math_position_text_rotation_set (gamgi_text *text, double *euler);

void gamgi_math_position_text_rotation_get (gamgi_text *text, double *euler);

void gamgi_math_position_text_translation_set (gamgi_text *text, double *origin);

void gamgi_math_position_text_translation_get (gamgi_text *text, double *origin);

void gamgi_math_position_orbital_rotation_set (gamgi_orbital *orbital, double *euler);

void gamgi_math_position_orbital_rotation_get (gamgi_orbital *orbital, double *euler);

void gamgi_math_position_orbital_translation_set (gamgi_orbital *orbital, double *origin);

void gamgi_math_position_orbital_translation_get (gamgi_orbital *orbital, double *origin);

void gamgi_math_position_direction_rotation_set (gamgi_direction *direction, 
double *euler);

void gamgi_math_position_direction_rotation_get (gamgi_direction *direction, 
double *euler);

void gamgi_math_position_direction_translation_set (gamgi_direction *direction, 
double *relative);

void gamgi_math_position_direction_translation_get (gamgi_direction *direction, 
double *relative);

void gamgi_math_position_plane_rotation_set (gamgi_plane *plane, double *euler);

void gamgi_math_position_plane_rotation_get (gamgi_plane *plane, double *euler);

void gamgi_math_position_plane_translation_set (gamgi_plane *plane, double *relative);

void gamgi_math_position_plane_translation_get (gamgi_plane *plane, double *relative);

void gamgi_math_position_group_rotation_set (gamgi_group *group, double *euler);

void gamgi_math_position_group_rotation_get (gamgi_group *group, double *euler);

void gamgi_math_position_group_translation_set (gamgi_group *group, double *relative);

void gamgi_math_position_group_translation_get (gamgi_group *group, double *relative);

void gamgi_math_position_cell_rotation_set (gamgi_cell *cell, double *euler);

void gamgi_math_position_cell_rotation_get (gamgi_cell *cell, double *euler);

void gamgi_math_position_cell_translation_set (gamgi_cell *cell, double *origin);

void gamgi_math_position_cell_translation_get (gamgi_cell *cell, double *origin);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
