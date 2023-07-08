/**********************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_translate_out.c
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

void gamgi_mesa_translate_out_text (gamgi_text *text, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (text->origin, translate, text->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (text->center, translate, text->center);
}

void gamgi_mesa_translate_out_orbital (gamgi_orbital *orbital, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (orbital->origin, translate, orbital->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (orbital->center, translate, orbital->center);
}

void gamgi_mesa_translate_out_atom (gamgi_atom *atom, 
double *translate)
{
atom->position[0] += translate[0];
atom->position[1] += translate[1];
atom->position[2] += translate[2];
}

void gamgi_mesa_translate_out_bond (gamgi_bond *bond, 
double *translate)
{
gamgi_mesa_translate_out_atom (bond->atom1, translate);
gamgi_mesa_translate_out_atom (bond->atom2, translate);
}

void gamgi_mesa_translate_out_direction (gamgi_direction *direction, 
double *translate)
{
gamgi_cell *cell;

if (direction->model == GAMGI_PHYS_LINE)
  {
  /******************************
   * atomic or crystallographic *
   * direction, in direct space *
   ******************************/

  gamgi_math_vector_add (direction->origin, translate, direction->origin);
  gamgi_math_vector_add (direction->center, translate, direction->center);
  }
else
  {
  /***************************
   * sterographic projection *
   ***************************/

  cell = GAMGI_CAST_CELL direction->object.object;
  gamgi_math_vector_copy (cell->origin, direction->origin);
  }

}

void gamgi_mesa_translate_out_plane (gamgi_plane *plane, 
double *translate)
{
gamgi_cell *cell;

if (plane->model == GAMGI_PHYS_POLYGON || plane->model == GAMGI_PHYS_VECTOR)
  {
  /*************************************
   * atomic or crystallographic plane, *
   * in direct or reciprocal space     *
   *************************************/

  gamgi_math_vector_add (plane->origin, translate, plane->origin);
  gamgi_math_vector_add (plane->center, translate, plane->center);
  }
else
  {
  /***************************
   * sterographic projection *
   ***************************/

  cell = GAMGI_CAST_CELL plane->object.object;
  gamgi_math_vector_copy (cell->origin, plane->origin);
  }

}

void gamgi_mesa_translate_out_group (gamgi_group *group, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (group->origin, translate, group->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (group->center, translate, group->center);
}

void gamgi_mesa_translate_out_molecule (gamgi_molecule *molecule, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (molecule->origin, translate, molecule->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (molecule->center, translate, molecule->center);
}

void gamgi_mesa_translate_out_cluster (gamgi_cluster *cluster, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (cluster->origin, translate, cluster->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (cluster->center, translate, cluster->center);
}

void gamgi_mesa_translate_out_cell (gamgi_cell *cell, 
double *translate)
{
/********************
 * translate origin *
 ********************/

gamgi_math_vector_add (cell->origin, translate, cell->origin);

/********************
 * translate center *
 ********************/

gamgi_math_vector_add (cell->center, translate, cell->center);
}

void gamgi_mesa_translate_out_arrow (gamgi_arrow *arrow, 
double *translate)
{
}

void gamgi_mesa_translate_out_shape (gamgi_shape *shape, 
double *translate)
{
}

void gamgi_mesa_translate_out_graph (gamgi_graph *graph, 
double *translate)
{
}

void gamgi_mesa_translate_out_assembly (gamgi_assembly *assembly, 
double *translate)
{
}

void gamgi_mesa_translate_out_light (gamgi_light *light, 
double *translate)
{
/****************************
 * positional or spot light *
 ****************************/

if (light->position[3] == 1.0)
  {
  light->position[0] += translate[0];
  light->position[1] += translate[1];
  light->position[2] += translate[2];
  }
}

void gamgi_mesa_translate_out_layer (gamgi_layer *layer, 
double *translate)
{
}
