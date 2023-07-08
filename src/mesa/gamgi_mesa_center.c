/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_center.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"

#include "gamgi_math_vector.h"

void gamgi_mesa_center_text (gamgi_text *text, double *sum, unsigned int *number)
{
double center[3];

/**********************************************************
 * the text contribution to the global center is given    *
 * only by the text center, weighted by a specific factor *
 **********************************************************/

gamgi_math_vector_scale (text->center, center, GAMGI_MESA_TEXT_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_TEXT_WEIGHT;
}

void gamgi_mesa_center_orbital (gamgi_orbital *orbital, double *sum, unsigned int *number)
{
double center[3];

/**********************************************************
 * the orbital contribution to the global center is given    *
 * only by the orbital center, weighted by a specific factor *
 **********************************************************/

gamgi_math_vector_scale (orbital->center, center, GAMGI_MESA_ORBITAL_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_ORBITAL_WEIGHT;
}

void gamgi_mesa_center_atom (gamgi_atom *atom, double *sum, unsigned int *number)
{
double center[3];

/**********************************************************
 * the atom contribution to the global center is given    *
 * only by the atom center, weighted by a specific factor *
 **********************************************************/

gamgi_math_vector_scale (atom->position, center, GAMGI_MESA_ATOM_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_ATOM_WEIGHT;
}

void gamgi_mesa_center_bond (gamgi_bond *bond, double *sum, unsigned int *number)
{
gamgi_mesa_center_atom (bond->atom1, sum, number);
gamgi_mesa_center_atom (bond->atom2, sum, number);
}

void gamgi_mesa_center_direction (gamgi_direction *direction, 
double *sum, unsigned int *number)
{
double center[3];

/***************************************************************
 * the direction contribution to the global center is given    *
 * only by the direction center, weighted by a specific factor *
 ***************************************************************/

gamgi_math_vector_scale (direction->center, center, GAMGI_MESA_DIRECTION_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_DIRECTION_WEIGHT;
}

void gamgi_mesa_center_plane (gamgi_plane *plane, 
double *sum, unsigned int *number)
{
double center[3];

/***********************************************************
 * the plane contribution to the global center is given    *
 * only by the plane center, weighted by a specific factor *
 ***********************************************************/

gamgi_math_vector_scale (plane->center, center, GAMGI_MESA_PLANE_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_PLANE_WEIGHT;
}

void gamgi_mesa_center_group (gamgi_group *group, double *sum, unsigned int *number)
{
gamgi_dlist *dlist;

gamgi_math_vector_add (sum, group->center, sum);
*number += 1;

dlist = group->group_start;
while (dlist != NULL)
  { gamgi_mesa_center_group (GAMGI_CAST_GROUP dlist->data, sum, number);
    dlist = dlist->next; }

dlist = group->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_center_plane (GAMGI_CAST_PLANE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION dlist->data, sum, number);
    dlist = dlist->next; }

dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_mesa_center_atom (GAMGI_CAST_ATOM dlist->data, sum, number);
    dlist = dlist->next; }
}

void gamgi_mesa_center_molecule (gamgi_molecule *molecule, double *sum, unsigned int *number)
{
gamgi_dlist *dlist;

dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_mesa_center_group (GAMGI_CAST_GROUP dlist->data, sum, number);
    dlist = dlist->next; }

dlist = molecule->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_center_plane (GAMGI_CAST_PLANE dlist->data, sum, number);
    dlist = dlist->next; } 

dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION dlist->data, sum, number);
    dlist = dlist->next; }

dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_mesa_center_atom (GAMGI_CAST_ATOM dlist->data, sum, number);
    dlist = dlist->next; }
}

void gamgi_mesa_center_cluster (gamgi_cluster *cluster, double *sum, unsigned int *number)
{
gamgi_dlist *dlist;

gamgi_math_vector_add (sum, cluster->center, sum);
*number += 1;

dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_center_cluster (GAMGI_CAST_CLUSTER dlist->data, sum, number);
    dlist = dlist->next; }

dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_center_molecule (GAMGI_CAST_MOLECULE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_mesa_center_group (GAMGI_CAST_GROUP dlist->data, sum, number);
    dlist = dlist->next; }

dlist = cluster->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_center_plane (GAMGI_CAST_PLANE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION dlist->data, sum, number);
    dlist = dlist->next; }

dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_mesa_center_atom (GAMGI_CAST_ATOM dlist->data, sum, number);
    dlist = dlist->next; }
}

void gamgi_mesa_center_cell (gamgi_cell *cell, double *sum, unsigned int *number)
{
double center[3];

/**********************************************************
 * the cell contribution to the global center is given    *
 * only by the cell center, weighted by a specific factor *
 **********************************************************/

gamgi_math_vector_scale (cell->center, center, GAMGI_MESA_CELL_WEIGHT);
gamgi_math_vector_add (sum, center, sum);
*number += GAMGI_MESA_CELL_WEIGHT;
}

void gamgi_mesa_center_arrow (gamgi_arrow *arrow, double *sum, unsigned int *number)
{
}

void gamgi_mesa_center_shape (gamgi_shape *shape, double *sum, unsigned int *number)
{
}

void gamgi_mesa_center_graph (gamgi_graph *graph, double *sum, unsigned int *number)
{
}

void gamgi_mesa_center_assembly (gamgi_assembly *assembly, double *sum, unsigned int *number)
{
gamgi_dlist *dlist;

dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_center_assembly (GAMGI_CAST_ASSEMBLY dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_mesa_center_cell (GAMGI_CAST_CELL dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_center_cluster (GAMGI_CAST_CLUSTER dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_center_molecule (GAMGI_CAST_MOLECULE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_mesa_center_group (GAMGI_CAST_GROUP dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_mesa_center_plane (GAMGI_CAST_PLANE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION dlist->data, sum, number);
    dlist = dlist->next; }

dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_mesa_center_atom (GAMGI_CAST_ATOM dlist->data, sum, number);
    dlist = dlist->next; }
}

void gamgi_mesa_center_light (gamgi_light *light, double *sum, unsigned int *number)
{
}

void gamgi_mesa_center_layer (gamgi_layer *layer, double *sum, unsigned int *number)
{
gamgi_dlist *dlist;

dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_center_assembly (GAMGI_CAST_ASSEMBLY dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_mesa_center_cell (GAMGI_CAST_CELL dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_center_cluster (GAMGI_CAST_CLUSTER dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_center_molecule (GAMGI_CAST_MOLECULE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_mesa_center_group (GAMGI_CAST_GROUP dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_mesa_center_plane (GAMGI_CAST_PLANE dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_mesa_center_atom (GAMGI_CAST_ATOM dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_center_orbital (GAMGI_CAST_ORBITAL dlist->data, sum, number);
    dlist = dlist->next; }

dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_mesa_center_text (GAMGI_CAST_TEXT dlist->data, sum, number);
    dlist = dlist->next; }
}

void gamgi_mesa_center_object (gamgi_object *object, double *sum, unsigned int *number)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_center_text (GAMGI_CAST_TEXT object, sum, number);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_center_orbital (GAMGI_CAST_ORBITAL object, sum, number);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_center_bond (GAMGI_CAST_BOND object, sum, number);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_center_atom (GAMGI_CAST_ATOM object, sum, number);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_mesa_center_direction (GAMGI_CAST_DIRECTION object, sum, number);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_mesa_center_plane (GAMGI_CAST_PLANE object, sum, number);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_center_group (GAMGI_CAST_GROUP object, sum, number);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_center_molecule (GAMGI_CAST_MOLECULE object, sum, number);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_center_cluster (GAMGI_CAST_CLUSTER object, sum, number);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_center_cell (GAMGI_CAST_CELL object, sum, number);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_center_arrow (GAMGI_CAST_ARROW object, sum, number);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_center_shape (GAMGI_CAST_SHAPE object, sum, number);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_center_graph (GAMGI_CAST_GRAPH object, sum, number);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_center_assembly (GAMGI_CAST_ASSEMBLY object, sum, number);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_center_light (GAMGI_CAST_LIGHT object, sum, number);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_center_layer (GAMGI_CAST_LAYER object, sum, number);
  break;

  default:
  break;
  }

}
