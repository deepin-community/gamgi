/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_rotate_out.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_phys.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_cell.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_projection.h"

void gamgi_mesa_rotate_out_text (gamgi_text *text, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/**********************
 * rotate text origin *
 **********************/

gamgi_math_vector_sub (text->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, text->origin);

/**********************
 * rotate text center *
 **********************/

gamgi_math_vector_sub (text->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, text->center);

gamgi_math_quaternion_add (quaternion, text->quaternion, text->quaternion);
}

void gamgi_mesa_rotate_out_orbital (gamgi_orbital *orbital, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/*************************
 * rotate orbital origin *
 *************************/

gamgi_math_vector_sub (orbital->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, orbital->origin);

/*************************
 * rotate orbital center *
 *************************/

gamgi_math_vector_sub (orbital->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, orbital->center);

gamgi_math_quaternion_add (quaternion, orbital->quaternion, orbital->quaternion);
}

void gamgi_mesa_rotate_out_atom (gamgi_atom *atom, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

gamgi_math_vector_sub (atom->position, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, atom->position);
}

void gamgi_mesa_rotate_out_bond (gamgi_bond *bond, 
double *quaternion, double *rotate, double *center)
{
gamgi_mesa_rotate_out_atom (bond->atom1, quaternion, rotate, center);
gamgi_mesa_rotate_out_atom (bond->atom2, quaternion, rotate, center);
}

void gamgi_mesa_rotate_out_direction (gamgi_direction *direction, 
double *quaternion, double *rotate, double *center)
{
gamgi_cell *cell;
double pole[3], normal[3];
double v1[3], v2[3];

if (direction->model == GAMGI_PHYS_LINE)
  {
  /******************************
   * atomic or crystallographic *
   * direction, in direct space *
   ******************************/

  /*****************
   * rotate origin *
   *****************/

  gamgi_math_vector_sub (direction->origin, center, v1);
  gamgi_math_matrix_vector (rotate, v1, v2);
  gamgi_math_vector_add (v2, center, direction->origin);

  /*****************
   * rotate center *
   *****************/

  gamgi_math_vector_sub (direction->center, center, v1);
  gamgi_math_matrix_vector (rotate, v1, v2);
  gamgi_math_vector_add (v2, center, direction->center);

  gamgi_math_quaternion_add (quaternion, direction->quaternion, direction->quaternion);
  }
else
  {
  /***********************************************
   * sterographic projection: rotate pole, trace *
   ***********************************************/

  cell = GAMGI_CAST_CELL direction->object.object;
  gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, pole);
  gamgi_math_cell_vector_rotate (cell, pole, normal);

  if (direction->model == GAMGI_PHYS_POLE)
    gamgi_phys_projection_pole (direction->net,
    normal, direction->points, direction->loops);

  else if (direction->model == GAMGI_PHYS_TRACE)
    gamgi_phys_projection_trace (direction->net,
    normal, direction->points, direction->loops);

  /*************
   * translate *
   *************/

  gamgi_math_vector_copy (cell->origin, direction->origin);
  }
}

void gamgi_mesa_rotate_out_plane (gamgi_plane *plane, 
double *quaternion, double *rotate, double *center)
{
gamgi_cell *cell;
double pole[3], normal[3];
double v1[3], v2[3];
  
if (plane->model == GAMGI_PHYS_POLYGON || plane->model == GAMGI_PHYS_VECTOR)
  {
  /***********************************************
   * atomic or crystallographic plane, in direct *
   * or reciprocal space: rotate origin, center  *
   ***********************************************/

  /*****************
   * rotate origin *
   *****************/

  gamgi_math_vector_sub (plane->origin, center, v1);
  gamgi_math_matrix_vector (rotate, v1, v2);
  gamgi_math_vector_add (v2, center, plane->origin);

  /*****************
   * rotate center *
   *****************/

  gamgi_math_vector_sub (plane->center, center, v1);
  gamgi_math_matrix_vector (rotate, v1, v2);
  gamgi_math_vector_add (v2, center, plane->center);

  gamgi_math_quaternion_add (quaternion, plane->quaternion, plane->quaternion);
  }
else
  {
  /***********************************************
   * sterographic projection: rotate pole, trace *
   ***********************************************/

  cell = GAMGI_CAST_CELL plane->object.object;
  gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, pole);
  gamgi_math_cell_vector_rotate (cell, pole, normal);

  if (plane->model == GAMGI_PHYS_POLE)
    gamgi_phys_projection_pole (plane->net,
    normal, plane->points, plane->loops);

  else if (plane->model == GAMGI_PHYS_TRACE)
    gamgi_phys_projection_trace (plane->net,
    normal, plane->points, plane->loops);

  /*************
   * translate *
   *************/

  gamgi_math_vector_copy (cell->origin, plane->origin);
  }
}

void gamgi_mesa_rotate_out_group (gamgi_group *group, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/***********************
 * rotate group origin *
 ***********************/

gamgi_math_vector_sub (group->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, group->origin);

/***********************
 * rotate group center *
 ***********************/

gamgi_math_vector_sub (group->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, group->center);

gamgi_math_quaternion_add (quaternion, group->quaternion, group->quaternion);
}

void gamgi_mesa_rotate_out_molecule (gamgi_molecule *molecule, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/*************************
 * rotate molecule origin *
 *************************/

gamgi_math_vector_sub (molecule->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, molecule->origin);

/*************************
 * rotate molecule center *
 *************************/

gamgi_math_vector_sub (molecule->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, molecule->center);

gamgi_math_quaternion_add (quaternion, molecule->quaternion, molecule->quaternion);
}

void gamgi_mesa_rotate_out_cluster (gamgi_cluster *cluster, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/*************************
 * rotate cluster origin *
 *************************/

gamgi_math_vector_sub (cluster->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, cluster->origin);

/*************************
 * rotate cluster center *
 *************************/

gamgi_math_vector_sub (cluster->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, cluster->center);

gamgi_math_quaternion_add (quaternion, cluster->quaternion, cluster->quaternion);
}

void gamgi_mesa_rotate_out_cell (gamgi_cell *cell, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];

/**********************
 * rotate cell origin *
 **********************/

gamgi_math_vector_sub (cell->origin, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, cell->origin);

/**********************
 * rotate cell center *
 **********************/

gamgi_math_vector_sub (cell->center, center, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, center, cell->center);

gamgi_math_quaternion_add (quaternion, cell->quaternion, cell->quaternion);
}

void gamgi_mesa_rotate_out_arrow (gamgi_arrow *arrow, 
double *quaternion, double *rotate, double *center)
{
}

void gamgi_mesa_rotate_out_shape (gamgi_shape *shape, 
double *quaternion, double *rotate, double *center)
{
}

void gamgi_mesa_rotate_out_graph (gamgi_graph *graph, 
double *quaternion, double *rotate, double *center)
{
}

void gamgi_mesa_rotate_out_assembly (gamgi_assembly *assembly, 
double *quaternion, double *rotate, double *center)
{
}

void gamgi_mesa_rotate_out_light (gamgi_light *light, 
double *quaternion, double *rotate, double *center)
{
double v1[3], v2[3];
double inverse[9];

/*********************
 * directional light *
 *********************/

if (light->position[3] == 0.0)
  {
  /************************
   * rotate around origin *
   ************************/

  v1[0] = light->position[0];
  v1[1] = light->position[1];
  v1[2] = light->position[2];

  gamgi_math_matrix_vector (rotate, v1, v2);

  light->position[0] = v2[0];
  light->position[1] = v2[1];
  light->position[2] = v2[2];
  }

/**************
 * spot light *
 **************/

else if (light->angle != 180)
  {
  /*************************************************************
   * In OpenGL, the direction vector has a different meaning,  *
   * essentially opposite, for directional and spot lights:    *
   * For directional lights, the vector (0,0,0) -> (direction) *
   * defines the direction from where the light is coming.     *
   * For spot lights, the vector (position) -> (direction)     *
   * defines the direction to where the light is going.        *
   *************************************************************/
     
  gamgi_math_matrix_inverse (rotate, inverse);

  /************************
   * rotate around origin *
   ************************/

  v1[0] = light->direction[0];
  v1[1] = light->direction[1];
  v1[2] = light->direction[2];

  gamgi_math_matrix_vector (inverse, v1, v2);

  light->direction[0] = v2[0];
  light->direction[1] = v2[1];
  light->direction[2] = v2[2];
  }
}

void gamgi_mesa_rotate_out_layer (gamgi_layer *layer, 
double *quaternion, double *rotate, double *center)
{
}
