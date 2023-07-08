/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_measure.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_position.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_math_polygon.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_reciprocal.h"

/********************** internal function ***********************
 *                                                              *
 *                     STATIC_ATOM_ROTATION                     *
 *                                                              *
 * Return the angle between vectors v1,v2, which is positive    *
 * if v1 is counterclockwise oriented relatively to v2 and      *
 * negative otherwise, according to the usual mathematical      *
 * convention. The two vectors are in 3D space, so its relative *
 * orientation and the resulting signal will deppend on which   *
 * side of the plane defined by the two vectors the user is.    *
 *                                                              *
 ****************************************************************/

double static_atom_rotation (double *v1, double *v2, gamgi_layer *layer)
{
double axis[3], view[3], angle;

/***********************************************
 * Measure the angle between vectors v1 and v2 *
 ***********************************************/

angle = gamgi_math_vector_angle (v1, v2);

/**********************************************
 * Define a vector going from the user to     *
 * the image center. This vector points away  *
 * from the user and is normal to the screen. *
 *********************************************/

gamgi_math_vector_sub (layer->center, layer->eye, view);

/*****************************************************
 * Define a vector perpendicular to vectors v1,v2.   *
 * If direction v1 can be obtained from direction v2 *
 * rotating counterclockwise, this vector will point *
 * to the half space away from the user, otherwise   *
 * it will point to the half space on the user side. *
 *****************************************************/

gamgi_math_vector_cross (v1, v2, axis);

/********************************************************************
 * If vector axis points to the half space away from the user,      *
 * then the angle will be positive, otherwise it wil be negative.   *
 * The angle will be positive when direction v1 can obtained from   *
 * direction v2 rotating counterclockwise, it will be negative when *
 * direction v1 can obtained from direction v2 rotating clockwise.  *
 * This is the ususal mathematical convention for an angle signal.  *
 ********************************************************************/

if (gamgi_math_vector_dot (axis, view) < 0) angle = -angle;

return angle;
}

/*********** external function **********
 *                                      *
 *    GAMGI_MATH_MEASURE_LENGTH_2ATOM   *
 *                                      *
 * Return the length between two atoms. *
 *                                      *
 ****************************************/

gamgi_bool gamgi_math_measure_length_2atom (gamgi_atom *atom1, 
gamgi_atom *atom2, double *length)
{
double vector[3];

/**********************************
 * Set a vector from atom1 to     *
 * atom2, and measure its length. *
 **********************************/

gamgi_math_vector_absolute (vector, 
atom2->position[0] - atom1->position[0], 
atom2->position[1] - atom1->position[1], 
atom2->position[2] - atom1->position[2]);

*length = gamgi_math_vector_length (vector);

return TRUE;
}

gamgi_bool gamgi_math_measure_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *length)
{
double pole[3], translate[3], normal[3];
double size, dot;
 
/***********************************
 * get unit vector along direction *
 ***********************************/

gamgi_math_vector_absolute (pole,
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1],
atom3->position[2] - atom2->position[2]);
 
size = gamgi_math_vector_length (pole);
if (size < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
gamgi_math_vector_scale (pole, pole, 1/size);

/**********************************************
 * get vector translate, from direction to    *
 * atom, and project it normal to unit vector *
 *                                            *
 * the difference between translate and       *
 * projected vectors is a vector normal       *
 * to direction, going from direction to atom *
 **********************************************/

gamgi_math_vector_sub (atom1->position, atom2->position, translate);
dot = gamgi_math_vector_dot (translate, pole);
gamgi_math_vector_scale (pole, pole, dot);

gamgi_math_vector_sub (translate, pole, normal);
*length = gamgi_math_vector_length (normal);

return TRUE;
}

gamgi_bool gamgi_math_measure_length_atom_3atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double *length)
{
double v1[3], v2[3];
double pole[3], translate[3];
double size;

/***********************************
 * get unit vector normal to plane *
 ***********************************/

gamgi_math_vector_absolute (v1,
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1],
atom3->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v2,
atom4->position[0] - atom2->position[0],
atom4->position[1] - atom2->position[1],
atom4->position[2] - atom2->position[2]);

gamgi_math_vector_cross (v1, v2, pole);
size = gamgi_math_vector_length (pole);
if (size < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
gamgi_math_vector_scale (pole, pole, 1/size);

/**************************************
 * project distance along unit vector *
 **************************************/

gamgi_math_vector_sub (atom1->position, atom2->position, translate);
*length = fabs (gamgi_math_vector_dot (translate, pole));

return TRUE;
}

gamgi_bool gamgi_math_measure_length_atom_direction (gamgi_atom *atom, 
gamgi_direction *direction, double *length)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], normal[3];
double translate[3];
double r1[3], r2[3];
double dot;

/**********************************************
 *   get unit vector along rotated direction  *
 *                                            *
 * get vector translate, from direction to    *
 * atom, and project it normal to unit vector *
 *                                            *
 * the difference between translate and       *
 * projected vectors is a vector normal       *
 * to direction, going from direction to atom *
 *                                            *
 *        rotate yes, normalize yes           *
 **********************************************/

if (direction->model == GAMGI_PHYS_LINE)
  {
  /**************************************
   *    a parent cell is not needed     *
   *                                    *
   * line direction: use true direction *
   * given by transformed points r1, r2 *
   **************************************/

  gamgi_phys_direction_points (direction, r1, r2);
  gamgi_math_vector_sub (r2, r1, normal);
  gamgi_math_vector_normal (normal);
  }
else
  {
  /**************************************
   *      a parent cell is needed       *
   *                                    *
   * projected direction: use direction *
   * passing through the cell origin    *
   **************************************/

  object = GAMGI_CAST_OBJECT direction->object.object;
  if (object->class != GAMGI_ENGINE_CELL) return FALSE;
  cell = GAMGI_CAST_CELL object;

  gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, pole);
  gamgi_math_cell_vector_rotate (cell, pole, normal);

  gamgi_math_position_cell_translation_get (cell, r1);
  }

gamgi_math_vector_sub (atom->position, r1, translate);
dot = gamgi_math_vector_dot (translate, normal);
gamgi_math_vector_scale (normal, pole, dot);

gamgi_math_vector_sub (translate, pole, normal);
*length = gamgi_math_vector_length (normal);

return TRUE;
}

gamgi_bool gamgi_math_measure_length_atom_plane (gamgi_atom *atom,
gamgi_plane *plane, double *length)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], normal[3];
double translate[3], origin[3];
double r1[3];

/***************************************
 * get unit vector normal to rotated   *
 * plane (cell can be in direct or     *
 * reciprocal space)                   *
 *                                     *
 * get vector translate, from plane to *
 * atom, and project along unit vector *
 *                                     *
 *     rotate yes, normalize yes       *
 ***************************************/

if (plane->model == GAMGI_PHYS_POLYGON)
  {
  /******************************************************
   * polygon planes: 1) use transformed center to enter *
   * the true plane position, for any plane order; 2)   *
     get plane normal using only the polygon points     *
   ******************************************************/

  gamgi_math_vector_sub (atom->position, plane->center, translate);
  gamgi_phys_plane_normal (plane, normal);
  *length = fabs (gamgi_math_vector_dot (translate, normal));
  }
else
  {
  /***********************************************
   * crystallographic plane (in reciprocal space *
   * or projection): the parent must be a cell   *
   ***********************************************/

  object = GAMGI_CAST_OBJECT plane->object.object;
  if (object->class != GAMGI_ENGINE_CELL) return FALSE;
  cell = GAMGI_CAST_CELL object;

  if (plane->model == GAMGI_PHYS_VECTOR)
    {
    /********************
     * reciprocal space *
     ********************/

    gamgi_phys_plane_point (plane, cell, r1);
    gamgi_math_vector_sub (atom->position, r1, translate);
    *length = gamgi_math_vector_length (translate);
    }
  else
    {
    /***************************************
     * stereographic projection: use plane *
     * passing through the cell origin     *
     ***************************************/

    gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, pole);
    gamgi_math_cell_vector_rotate (cell, pole, normal);

    gamgi_math_position_cell_translation_get (cell, origin);
    gamgi_math_vector_sub (atom->position, origin, translate);
    *length = fabs (gamgi_math_vector_dot (translate, normal));
    }
  }

return TRUE;
}

gamgi_bool gamgi_math_measure_length_direction (gamgi_direction *direction, double *length)
{
if (direction->model != GAMGI_PHYS_LINE) return FALSE;

*length = gamgi_math_polygon_length_lines (direction->loops, direction->points);

return TRUE;
}

gamgi_bool gamgi_math_measure_length_node_direction (gamgi_direction *direction, 
double *length)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3];
int *uvw;
int node[3], node_min[3];

/*******************************************
 *   the parent of a crystallographic      *
 *   plane or direction must be a cell     *
 *                                         *
 *        rotate no, normalize no          *
 *******************************************/

object = GAMGI_CAST_OBJECT direction->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

uvw = direction->uvw;
if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice,
  uvw[0], uvw[1], uvw[2], FALSE, &node[0], &node[1], &node[2]);
else { node[0] = uvw[0]; node[1] = uvw[1]; node[2] = uvw[2]; }

gamgi_phys_direction_decrease (node, node_min);
gamgi_phys_direction_vector (cell, node_min, GAMGI_PHYS_PRIMITIVE, pole);
*length = gamgi_math_vector_length (pole);

return TRUE;
}

/*********** external function ********
 *                                    *
 *   GAMGI_MATH_MEASURE_LENGTH_PLANE  *
 *                                    *
 * Return distance between planes hkl *
 *                                    *
 **************************************/

gamgi_bool gamgi_math_measure_length_plane (gamgi_plane *plane,
double *length)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], origin[3];

/******************************************
 *  the parent of a crystallographic      *
 *  plane or direction must be a cell     *
 *                                        *
 * determine reciprocal lattice vector    *
 * (cell can be in direct or reciprocal   *
 * space)                                 *
 *                                        *
 * the distance between planes hkl is     *
 * factor/length of the reciprocal vector *
 *                                        *
 *       rotate no, normalize no          *
 ******************************************/

object = GAMGI_CAST_OBJECT plane->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

if (plane->model == GAMGI_PHYS_VECTOR)
  {
  if (cell->reciprocal == 0.0) return FALSE;
  gamgi_math_cell_offset (cell, origin);
  gamgi_math_vector_sub (plane->points, origin, pole);
  *length = cell->reciprocal / gamgi_math_vector_length (pole);
  }
else
  {
  gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, pole);
  *length = 1.0 / gamgi_math_vector_length (pole);
  }

return TRUE;
}

gamgi_bool gamgi_math_measure_area_plane (gamgi_plane *plane, double *area)
{
if (plane->model != GAMGI_PHYS_POLYGON) return FALSE;

*area = gamgi_math_polygon_area_faces (plane->loops, plane->points);

return TRUE;
}

/*********** external function *********
 *                                     *
 *  GAMGI_MATH_MEASURE_AREA_NODE_PLANE *
 *                                     *
 *  Return area per node in planes hkl *
 *                                     *
 ***************************************/

gamgi_bool gamgi_math_measure_area_node_plane (gamgi_plane *plane, double *area)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], origin[3];
double volume;
int hkl_min[3], hkl_miller[3];
gamgi_enum lattice_new;

/**************************************
 * The parent of a crystallographic   *
 * plane or direction must be a cell. *
 **************************************/

object = GAMGI_CAST_OBJECT plane->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

/*************************************************
 * Planes with indices above the Miller indices  *
 * have no area/node; planes with indices below  *
 * the Miler indices have the same area/node as  *
 * planes with Miller indices.                   *
 *************************************************/

gamgi_phys_direction_decrease (plane->hkl, hkl_min);
if (plane->model == GAMGI_PHYS_VECTOR)
  {
  if (cell->reciprocal == 0.0) return FALSE;
  lattice_new = gamgi_phys_reciprocal_lattice (cell->lattice);
  gamgi_phys_plane_increase (lattice_new, hkl_min, plane->vectors, hkl_miller);
  }
else gamgi_phys_plane_increase (cell->lattice, hkl_min, plane->vectors, hkl_miller);

if (gamgi_phys_direction_higher (plane->hkl, hkl_miller) == TRUE) return FALSE;

/*************************************************
 * Determine reciprocal lattice vector (cell can *
 * be in direct or reciprocal space). The        *
 * distance between planes hkl is factor/length  *
 * of the reciprocal vector.                     *
 *                                               *
 * volume/node = area/node x plane separation    *
 *                                               *
 * The reciprocal space volume/node is equal to  *
 * factor**3 / direct space volume/node          *
 *                                               *
 *            rotate no, normalize no            *
 *************************************************/

volume = gamgi_math_vector_mix (cell->p1, cell->p2, cell->p3);

if (plane->model == GAMGI_PHYS_VECTOR)
  {
  volume = pow (cell->reciprocal, 2) / volume;
  gamgi_math_cell_offset (cell, origin);
  gamgi_math_vector_sub (plane->points, origin, pole);
  }
else gamgi_phys_plane_vector (cell, hkl_miller, plane->vectors, pole);

*area = volume * gamgi_math_vector_length (pole);

return TRUE;
}

/************** external function ****************
 *                                               *
 *         GAMGI_MATH_MEASURE_ANGLE_3ATOM        *
 *                                               *
 * Return the angle formed by atom1,atom2,atom3. *
 *                                               *
 *************************************************/

gamgi_bool gamgi_math_measure_angle_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *angle, gamgi_layer *layer)
{
double v21[3], v23[3];
double length;

/******************************************
 * Set vectors v21,v23, which start at    *
 * atom2 and define the angle to measure. *
 ******************************************/

gamgi_math_vector_absolute (v21, 
atom1->position[0] - atom2->position[0], 
atom1->position[1] - atom2->position[1], 
atom1->position[2] - atom2->position[2]);
length = gamgi_math_vector_length (v21);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

gamgi_math_vector_absolute (v23, 
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1], 
atom3->position[2] - atom2->position[2]);
length = gamgi_math_vector_length (v23);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

/************************************************************
 * Return the angle between v21 and v23, with a signal      *
 * that is a function of the orientation of the two vectors *
 * v21,v23 relatively to the user viewing position.         *
 ************************************************************/

*angle = static_atom_rotation (v21, v23, layer);

return TRUE;
}

/************************ external function ********************
 *                                                             *
 *                 GAMGI_MATH_MEASURE_ANGLE_4ATOM              *
 *                                                             *
 * Return the torsion angle formed by atom1,atom2,atom3,atom4. *
 *                                                             *
 ***************************************************************/

gamgi_bool gamgi_math_measure_angle_4atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4,
double *angle, gamgi_layer *layer)
{
double v23[3], v21[3], v34[3], v32[3];
double cross1[3], cross2[3];
double length;

/********************************************************
 * Set vectors v23,v21, which start at atom2 and belong *
 * to torsion plane 1, therefore defining this plane.   *
 ********************************************************/

gamgi_math_vector_absolute (v23, 
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1], 
atom3->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v21, 
atom1->position[0] - atom2->position[0],
atom1->position[1] - atom2->position[1], 
atom1->position[2] - atom2->position[2]);

/********************************************************
 * Set vectors v34,v32, which start at atom3 and belong *
 * to torsion plane 2, therefore defining this plane.   *
 ********************************************************/

gamgi_math_vector_absolute (v34, 
atom4->position[0] - atom3->position[0],
atom4->position[1] - atom3->position[1], 
atom4->position[2] - atom3->position[2]);

gamgi_math_vector_absolute (v32, 
atom2->position[0] - atom3->position[0],
atom2->position[1] - atom3->position[1], 
atom2->position[2] - atom3->position[2]);

/***************************************************
 * Define vectors w1,w2, perpendicular to torsion  *
 * planes 1,2. The torsion angle, between torsion  *
 * planes 1 and 2, is the angle between w1 and w2. *
 *                                                 *
 * If atom1,atom2,atom3 or atom2,atom3,atom4 are   *
 * aligned then length will be zero and angle is   *
 * undefined.                                      *
 ***************************************************/

gamgi_math_vector_cross (v23, v21, cross1);
length = gamgi_math_vector_length (cross1);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

gamgi_math_vector_cross (v34, v32, cross2);
length = gamgi_math_vector_length (cross2);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

/**********************************************************
 * Return the angle between w1 and w2, with a signal that *
 * is a function of the orientation of the two vectors    *
 * w1,w2 relatively to the user viewing position.         *
 **********************************************************/

*angle = static_atom_rotation (cross1, cross2, layer);

return TRUE;
}

gamgi_bool gamgi_math_measure_angle_2direction (gamgi_direction *direction1,
gamgi_direction *direction2, double *angle)
{
gamgi_cell *cell;
gamgi_object *object;
double normal1[3], normal2[3];
double pole[3];

/*************************************
 * the parent of a crystallographic  *
 * plane or direction must be a cell *
 * (in direct or reciprocal space)   *
 *                                   *
 * get direct lattice unit vector    *
 * along rotated direction 1         *
 *                                   *
 * get direct lattice unit vector    *
 * along rotated direction 2         *
 *                                   *
 *    rotate yes, normalize yes      *
 *************************************/

/**************
 * direction1 *
 **************/

object = GAMGI_CAST_OBJECT direction1->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

gamgi_phys_direction_vector (cell, direction1->uvw, direction1->vectors, pole);
gamgi_math_cell_vector_rotate (cell, pole, normal1);

/**************
 * direction2 *
 **************/

object = GAMGI_CAST_OBJECT direction2->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

gamgi_phys_direction_vector (cell, direction2->uvw, direction2->vectors, pole);
gamgi_math_cell_vector_rotate (cell, pole, normal2);

*angle = gamgi_math_vector_angle (normal1, normal2);

return TRUE;
}

gamgi_bool gamgi_math_measure_angle_2plane (gamgi_plane *plane1, 
gamgi_plane *plane2, double *angle)
{
gamgi_cell *cell;
gamgi_object *object;
double normal1[3], normal2[3];
double pole[3], origin[3];

/**************************************
 * the parent of a crystallographic   *
 * plane or direction must be a cell  *
 * (in direct or reciprocal space)    *
 *                                    *
 * get reciprocal lattice unit vector *
 * perpendicular to rotated plane 1   *
 *                                    *
 * get reciprocal lattice unit vector *
 * perpendicular to rotated plane 2   *
 *                                    *
 *    rotate yes, normalize yes       *
 **************************************/

/***********
 * plane 1 *
 ***********/

object = GAMGI_CAST_OBJECT plane1->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

if (plane1->model == GAMGI_PHYS_VECTOR)
  {
  gamgi_math_cell_offset (cell, origin);
  gamgi_math_vector_sub (plane1->points, origin, pole);
  }
else gamgi_phys_plane_vector (cell, plane1->hkl, plane1->vectors, pole);

gamgi_math_cell_vector_rotate (cell, pole, normal1);

/***********
 * plane 2 *
 ***********/

object = GAMGI_CAST_OBJECT plane2->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

if (plane2->model == GAMGI_PHYS_VECTOR)
  {
  gamgi_math_cell_offset (cell, origin);
  gamgi_math_vector_sub (plane2->points, origin, pole);
  }
else gamgi_phys_plane_vector (cell, plane2->hkl, plane2->vectors, pole);

gamgi_math_cell_vector_rotate (cell, pole, normal2);

*angle = gamgi_math_vector_angle (normal1, normal2);

return TRUE;
}

gamgi_bool gamgi_math_measure_angle_plane_direction (gamgi_plane *plane,
gamgi_direction *direction, double *angle)
{
gamgi_cell *cell;
gamgi_object *object;
double normal1[3], normal2[3];
double pole[3], origin[3];

/**************************************
 * the parent of a crystallographic   *
 * plane or direction must be a cell  *
 * (in direct or reciprocal space)    *
 *                                    *
 * get reciprocal lattice unit vector *
 * perpendicular to rotated plane     *
 *                                    *
 * get direct lattice unit vector     *
 * along rotated direction            *
 *                                    *
 *    rotate yes, normalize yes       *
 **************************************/

/*********
 * plane *
 *********/

object = GAMGI_CAST_OBJECT plane->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

if (plane->model == GAMGI_PHYS_VECTOR)
  {
  gamgi_math_cell_offset (cell, origin);
  gamgi_math_vector_sub (plane->points, origin, pole);
  }
else gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, pole);

gamgi_math_cell_vector_rotate (cell, pole, normal1);

/*************
 * direction *
 *************/

object = GAMGI_CAST_OBJECT direction->object.object;
if (object->class != GAMGI_ENGINE_CELL) return FALSE;
cell = GAMGI_CAST_CELL object;

gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, pole);
gamgi_math_cell_vector_rotate (cell, pole, normal2);

*angle = gamgi_math_vector_angle (normal1, normal2);

return TRUE;
}
