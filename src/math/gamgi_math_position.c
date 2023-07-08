/******************************************
 *
 * $GAMGI/src/math/gamgi_math_text.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"

#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_cell.h"

void gamgi_math_position_rotation (gamgi_object *object, double *rotation)
{
gamgi_assembly *assembly;
gamgi_cell *cell;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_text *text;

switch (object->class)
  {
  case GAMGI_ENGINE_LAYER:
  gamgi_math_matrix_unit (rotation);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY object;
  gamgi_math_quaternion_to_matrix (assembly->quaternion, rotation);
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL object;
  gamgi_math_quaternion_to_matrix (cell->quaternion, rotation);
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER object;
  gamgi_math_quaternion_to_matrix (cluster->quaternion, rotation);
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE object;
  gamgi_math_quaternion_to_matrix (molecule->quaternion, rotation);
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP object;
  gamgi_math_quaternion_to_matrix (group->quaternion, rotation);
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE object;
  gamgi_math_quaternion_to_matrix (plane->quaternion, rotation);
  break;

  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION object;
  gamgi_math_quaternion_to_matrix (direction->quaternion, rotation);
  break;

  case GAMGI_ENGINE_TEXT:
  text = GAMGI_CAST_TEXT object;
  gamgi_math_quaternion_to_matrix (text->quaternion, rotation);
  }
}

void gamgi_math_position_translation (gamgi_object *object, double *translation)
{
gamgi_assembly *assembly;
gamgi_cell *cell;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_text *text;

switch (object->class)
  {
  case GAMGI_ENGINE_LAYER:
  gamgi_math_vector_zero (translation);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY object;
  gamgi_math_vector_copy (assembly->origin, translation);
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL object;
  gamgi_math_vector_copy (cell->origin, translation);
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER object;
  gamgi_math_vector_copy (cluster->origin, translation);
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE object;
  gamgi_math_vector_copy (molecule->origin, translation);
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP object;
  gamgi_math_vector_copy (group->origin, translation);
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE object;
  gamgi_math_vector_copy (plane->origin, translation);
  break;

  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION object;
  gamgi_math_vector_copy (direction->origin, translation);
  break;

  case GAMGI_ENGINE_TEXT:
  text = GAMGI_CAST_TEXT object;
  gamgi_math_vector_copy (text->origin, translation);
  }
}

void gamgi_math_position_object_rotation (gamgi_object *object, double *rotation)
{
double matrix[9], inverse[9];
double quaternion[4];
double center[3];
 
gamgi_math_vector_zero (center);

/*********************************************
 * rotate parent backward, to properly reset *
 * rotation, updating also origin and center *
 *********************************************/
  
gamgi_math_position_rotation (object, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_object (object, quaternion, inverse, center);

/****************************************************************
 *   rotate parent foward, to update parent origin and center   *
 *                                                              *
 * backward and forward rotations are applied around an         * 
 * arbitrary point center, as these operations will change      *
 * the parent origin accordingly, which is then used in the     *
 * translation applied next to properly positionate the parent  *
 *                                                              *
 * rotation must be done before translation, to guarantee that  *
 * origin will have the defined origin coordinates. As rotation *
 * changes the origin, the translation then corrects the shift. *
 ****************************************************************/

gamgi_math_quaternion_from_matrix (rotation, quaternion);
gamgi_mesa_rotate_object (object, quaternion, rotation, center);
}

void gamgi_math_position_object_translation (gamgi_object *object, double *translation)
{
double vector[3], translate[3];

/******************************************
 * set the object origin, correcting also *
 * the shift due to the previous rotation *
 ******************************************/

gamgi_math_position_translation (object, vector);
gamgi_math_vector_sub (translation, vector, translate);
gamgi_mesa_translate_object (object, translate);
}

void gamgi_math_position_text_rotation_set (gamgi_text *text, double *euler)
{
double matrix[9], inverse[9], rotate[9];
double quaternion[4];
double center[3];
 
gamgi_math_vector_zero (center);

/*********************************************
 * rotate text backward, to properly reset   *
 * rotation, updating also origin and center *
 *********************************************/
  
gamgi_math_quaternion_to_matrix (text->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_text (text, quaternion, inverse, center);

/*************************************************************
 *   rotate text foward, to update text origin and center    *
 *                                                           *
 * backward and forward rotations are applied around an      *
 * arbitrary point center, as these operations will change   *
 * the text origin accordingly, which is then used in the    *
 * translation applied next to properly positionate the text *
 *************************************************************/

gamgi_math_euler_to_matrix (euler, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_text (text, quaternion, rotate, center);

/*****************************************************
 * rotation must be done before translation,         *
 * to guarantee that origin will have the            *
 * defined origin coordinates. As rotation is done   *
 * around the center, the origin coordinates will    *
 * change. The translation then corrects this shift. *
 *****************************************************/
}

void gamgi_math_position_text_rotation_get (gamgi_text *text, double *euler)
{
double rotate[9];

gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);
}

void gamgi_math_position_text_translation_set (gamgi_text *text, double *origin)
{
double translate[3];

/*****************************************************************
 * correct the text origin change due to the previous rotation:  *
 * as this is applied around the text center, the origin changes *
 *****************************************************************/

gamgi_math_vector_sub (origin, text->origin, translate);
gamgi_mesa_translate_text (text, translate);
}

void gamgi_math_position_text_translation_get (gamgi_text *text, double *origin)
{
gamgi_math_vector_copy (text->origin, origin);
}

void gamgi_math_position_orbital_rotation_set (gamgi_orbital *orbital, double *euler)
{
double matrix[9], inverse[9], rotate[9];
double quaternion[4];
double center[3];
 
gamgi_math_vector_zero (center);

/**********************************************
 * rotate orbital backward, to properly reset *
 * rotation, updating also origin and center  *
 **********************************************/
  
gamgi_math_quaternion_to_matrix (orbital->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_orbital (orbital, quaternion, inverse, center);

/****************************************************************
 * rotate orbital foward, to update orbital origin and center   *
 *                                                              *
 * backward and forward rotations are applied around an         *
 * arbitrary point center, as these operations will change      *
 * the orbital origin accordingly, which is then used in the    *
 * translation applied next to properly positionate the orbital *
 ****************************************************************/

gamgi_math_euler_to_matrix (euler, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_orbital (orbital, quaternion, rotate, center);

/*****************************************************
 * rotation must be done before translation,         *
 * to guarantee that origin will have the            *
 * defined origin coordinates. As rotation is done   *
 * around the center, the origin coordinates will    *
 * change. The translation then corrects this shift. *
 *****************************************************/
}

void gamgi_math_position_orbital_rotation_get (gamgi_orbital *orbital, double *euler)
{
double rotate[9];

gamgi_math_quaternion_to_matrix (orbital->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);
}

void gamgi_math_position_orbital_translation_set (gamgi_orbital *orbital, double *origin)
{
double translate[3];

/********************************************************************
 * correct the orbital origin change due to the previous rotation:  *
 * as this is applied around the orbital center, the origin changes *
 ********************************************************************/

gamgi_math_vector_sub (origin, orbital->origin, translate);
gamgi_mesa_translate_orbital (orbital, translate);
}

void gamgi_math_position_orbital_translation_get (gamgi_orbital *orbital, double *origin)
{
gamgi_math_vector_copy (orbital->origin, origin);
}

void gamgi_math_position_direction_rotation_set (gamgi_direction *direction, 
double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];
double quaternion[4];
double center[3];

/******************************************************************
 * backward and forward rotations are applied around an arbitrary *
 * point center, as the rotation itself is independent of the     *
 * rotation center and the origin is then used in the translation *
 * applied next to properly positionate the direction             *
 ******************************************************************/

gamgi_math_vector_zero (center);

/************************************************
 * rotate direction backward, to properly reset *
 * rotation, updating also origin and center    *
 ************************************************/

gamgi_math_quaternion_to_matrix (direction->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_direction (direction, quaternion, inverse, center);

/**************************************************************
 *    rotate direction foward, after rotating parent,         *
 *    to properly update direction origin and center          *
 *                                                            *
 * The direction euler angles show only the difference        *
 * between the direction and parent rotation, while the       *
 * direction quaternion stores the whole direction rotation.  *
 *                                                            *
 * first apply the cell rotation, then the direction rotation *
 **************************************************************/

gamgi_math_position_rotation (direction->object.object, matrix);
gamgi_math_euler_to_matrix (euler, relative);
gamgi_math_matrix_matrix (relative, matrix, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_direction (direction, quaternion, rotate, center);
}

void gamgi_math_position_direction_rotation_get (gamgi_direction *direction, 
double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];

/*****************************************************************
 * The direction euler angles show only the rotation             *
 * difference between the direction and the parent, while        *
 * the direction quaternion stores the whole direction rotation. *
 *                                                               *
 * first apply the cell rotation, then the direction rotation.   *
 *****************************************************************/

gamgi_math_position_rotation (direction->object.object, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_to_matrix (direction->quaternion, rotate);
gamgi_math_matrix_matrix (rotate, inverse, relative);
gamgi_math_euler_from_matrix (relative, euler);
}

void gamgi_math_position_direction_translation_set (gamgi_direction *direction, 
double *relative)
{
double vector[3], absolute[3], translate[3];

/**********************************************************************
 * correct the direction origin change due to the previous rotation:  *
 * as this is applied around the direction center, its origin changes *
 **********************************************************************/

gamgi_math_position_translation (direction->object.object, vector);
gamgi_math_vector_add (vector, relative, absolute);
gamgi_math_vector_sub (absolute, direction->origin, translate);
gamgi_mesa_translate_direction (direction, translate);
}

void gamgi_math_position_direction_translation_get (gamgi_direction *direction, 
double *relative)
{
double vector[3];

gamgi_math_position_translation (direction->object.object, vector);
gamgi_math_vector_sub (direction->origin, vector, relative);
}

void gamgi_math_position_plane_rotation_set (gamgi_plane *plane, double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];
double quaternion[4];
double center[3];

/******************************************************************
 * backward and forward rotations are applied around an arbitrary *
 * point center, as the rotation itself is independent of the     *
 * rotation center and the origin is then used in the translation *
 * applied next to properly positionate the direction             *
 ******************************************************************/

gamgi_math_vector_zero (center);

/*********************************************
 * rotate plane backward, to properly reset  *
 * rotation, updating also origin and center *
 *********************************************/

gamgi_math_quaternion_to_matrix (plane->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_plane (plane, quaternion, inverse, center);

/***********************************************************
 *    rotate plane foward, after rotating parent,          *
 *    to properly update plane origin and center           *
 *                                                         *
 * The plane euler angles show only the difference         *
 * between the plane and cell rotation, while the          *
 * plane quaternion stores the whole plane rotation.       *
 *                                                         *
 * first apply the cell rotation, then the plane rotation. *
 ***********************************************************/

gamgi_math_position_rotation (plane->object.object, matrix);
gamgi_math_euler_to_matrix (euler, relative);
gamgi_math_matrix_matrix (relative, matrix, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_plane (plane, quaternion, rotate, center);
}

void gamgi_math_position_plane_rotation_get (gamgi_plane *plane, double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];

/***********************************************************
 * The plane euler angles show only the rotation           *
 * difference between the plane and the parent, while      *
 * the plane quaternion stores the whole plane rotation.   *
 *                                                         *
 * first apply the cell rotation, then the plane rotation. *
 ***********************************************************/

gamgi_math_position_rotation (plane->object.object, matrix);

gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_to_matrix (plane->quaternion, rotate);
gamgi_math_matrix_matrix (rotate, inverse, relative);
gamgi_math_euler_from_matrix (relative, euler);
}

void gamgi_math_position_plane_translation_set (gamgi_plane *plane, double *relative)
{
double vector[3], absolute[3], translate[3];

/******************************************************************
 * correct the plane origin change due to the previous rotation:  *
 * as this is applied around the plane center, its origin changes *
 ******************************************************************/

gamgi_math_position_translation (plane->object.object, vector);
gamgi_math_vector_add (vector, relative, absolute);
gamgi_math_vector_sub (absolute, plane->origin, translate);
gamgi_mesa_translate_plane (plane, translate);
}

void gamgi_math_position_plane_translation_get (gamgi_plane *plane, double *relative)
{
double vector[3];

gamgi_math_position_translation (plane->object.object, vector);
gamgi_math_vector_sub (plane->origin, vector, relative);
}

void gamgi_math_position_group_rotation_set (gamgi_group *group, double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];
double quaternion[4];
double center[3];

/******************************************************************
 * backward and forward rotations are applied around an arbitrary *
 * point center, as the rotation itself is independent of the     *
 * rotation center and the origin is then used in the translation *
 * applied next to properly positionate the direction             *
 ******************************************************************/

gamgi_math_vector_zero (center);

/*********************************************
 * rotate group backward, to properly reset  *
 * rotation, updating also origin and center *
 *********************************************/

gamgi_math_quaternion_to_matrix (group->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_group (group, quaternion, inverse, center);

/***********************************************************
 *    rotate group foward, after rotating parent,          *
 *    to properly update group origin and center           *
 *                                                         *
 * The group euler angles show only the difference         *
 * between the group and cell rotation, while the          *
 * group quaternion stores the whole group rotation.       *
 *                                                         *
 * first apply the cell rotation, then the group rotation. *
 ***********************************************************/

gamgi_math_position_rotation (group->object.object, matrix);
gamgi_math_euler_to_matrix (euler, relative);
gamgi_math_matrix_matrix (relative, matrix, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_group (group, quaternion, rotate, center);
}

void gamgi_math_position_group_rotation_get (gamgi_group *group, double *euler)
{
double matrix[9], inverse[9], relative[9], rotate[9];

/***********************************************************
 * The group euler angles show only the rotation           *
 * difference between the group and the parent, while      *
 * the group quaternion stores the whole group rotation.   *
 *                                                         *
 * first apply the cell rotation, then the group rotation. *
 ***********************************************************/

gamgi_math_position_rotation (group->object.object, matrix);

gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_to_matrix (group->quaternion, rotate);
gamgi_math_matrix_matrix (rotate, inverse, relative);
gamgi_math_euler_from_matrix (relative, euler);
}

void gamgi_math_position_group_translation_set (gamgi_group *group, double *relative)
{
double vector[3], absolute[3], translate[3];

/******************************************************************
 * correct the group origin change due to the previous rotation:  *
 * as this is applied around the group center, its origin changes *
 ******************************************************************/

gamgi_math_position_translation (group->object.object, vector);
gamgi_math_vector_add (vector, relative, absolute);
gamgi_math_vector_sub (absolute, group->origin, translate);
gamgi_mesa_translate_group (group, translate);
}

void gamgi_math_position_group_translation_get (gamgi_group *group, double *relative)
{
double vector[3];

gamgi_math_position_translation (group->object.object, vector);
gamgi_math_vector_sub (group->origin, vector, relative);
}

void gamgi_math_position_cell_rotation_set (gamgi_cell *cell, double *euler)
{
double matrix[9], inverse[9], rotate[9];
double quaternion[4];
double center[3];
 
gamgi_math_vector_zero (center);

/*********************************************
 * rotate cell backward, to properly reset   *
 * rotation, updating also origin and center *
 *********************************************/
  
gamgi_math_quaternion_to_matrix (cell->quaternion, matrix);
gamgi_math_matrix_inverse (matrix, inverse);
gamgi_math_quaternion_from_matrix (inverse, quaternion);
gamgi_mesa_rotate_cell (cell, quaternion, inverse, center);

/*************************************************************
 *   rotate cell foward, to update cell origin and center    *
 *                                                           *
 * backward and forward rotations are applied around an      *
 * arbitrary point center, as these operations will change   *
 * the cell origin accordingly, which is then used in the    *
 * translation applied next to properly positionate the cell *
 *************************************************************/

gamgi_math_euler_to_matrix (euler, rotate);
gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_cell (cell, quaternion, rotate, center);

/*****************************************************
 * rotation must be done before translation,         *
 * to guarantee that origin will have the            *
 * defined origin coordinates. As rotation is done   *
 * around the center, the origin coordinates will    *
 * change. The translation then corrects this shift. *
 *****************************************************/
}

void gamgi_math_position_cell_rotation_get (gamgi_cell *cell, double *euler)
{
double rotate[9];

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);
}

void gamgi_math_position_cell_translation_set (gamgi_cell *cell, double *origin)
{
double rotate[9];
double translate[3], old[3], new[3];

/*****************************************************************
 * correct the cell origin change due to the previous rotation:  *
 * as this is applied around the cell center, the origin changes *
 *****************************************************************/

gamgi_math_vector_sub (origin, cell->origin, translate);

/************************************************************************
 * correct the cell origin change due to the node (000) redefinition:   *
 * cell->origin contains always the physical origin, the node at the    *
 * bottom-left corner (conventional, primitive, wigner, parallelepiped) *
 * or at the center (sphere). When the node (000) is redefined, its     *
 * position, the oficial requested origin, is no longer the same as     *
 * the physical cell origin and this must be translated accordingly.    *
 ************************************************************************/
 
gamgi_math_cell_offset (cell, old);
gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_matrix_vector (rotate, old, new);
gamgi_math_vector_sub (translate, new, translate);

gamgi_mesa_translate_cell (cell, translate);
}

void gamgi_math_position_cell_translation_get (gamgi_cell *cell, double *origin)
{
double rotate[9];
double old[3], new[3];

gamgi_math_cell_offset (cell, old);
gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_matrix_vector (rotate, old, new);

gamgi_math_vector_add (cell->origin, new, origin);
}
