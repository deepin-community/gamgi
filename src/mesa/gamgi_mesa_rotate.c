/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_rotate.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_find.h"
#include "gamgi_mesa_center.h"
#include "gamgi_mesa_rotate_out.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_global_selection.h"

static void static_center (gamgi_object *object, double *center)
{
unsigned int number;

/*************************************************
 * get geometric center: when object is an empty *
 * layer, the total number of objects is 0.      *
 *************************************************/

number = 0;
gamgi_math_vector_zero (center);
gamgi_mesa_center_object (object, center, &number);
if (number != 0)
  gamgi_math_vector_scale (center, center, 1.0 / number);
}

static void static_rotate (gamgi_layer *layer, double angle,
double *axis, double *quaternion, double *rotate)
{
double axis_new[3];

/*******************************************
 * transform axis to the layer referential *
 *******************************************/

gamgi_math_matrix_vector (layer->referential, axis, axis_new);

/**************************************
 * get rotation matrix and quaternion *
 **************************************/

gamgi_math_quaternion_from_axis (angle, axis_new, quaternion);
gamgi_math_quaternion_to_matrix (quaternion, rotate);
}

static void static_layer (gamgi_object *object, gamgi_layer **layer_old,
double angle, double *axis, double *quaternion, double *rotate)
{
gamgi_layer *layer;

/********************************************************************
 * quaternion, rotate must be recalculated only when layer changes, *
 * as angle, axis remain the same throughout the rotation operation *
 ********************************************************************/

layer = gamgi_engine_find_layer (object);
if (layer != *layer_old)
  {
  *layer_old = layer;
  static_rotate (layer, angle, axis, quaternion, rotate);
  }
}

void gamgi_mesa_rotate_text (gamgi_text *text,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_text (text, quaternion, rotate, center);

dlist = text->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_orbital (gamgi_orbital *orbital,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_orbital (orbital, quaternion, rotate, center);

dlist = orbital->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_bond (gamgi_bond *bond,
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

dlist = bond->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = bond->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_atom (gamgi_atom *atom,  
double *quaternion, double *rotate, double *center)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_atom (atom, quaternion, rotate, center);

/*********************************************
 * rotate bond elements only the second time *
 *********************************************/

dlist = atom->bond_start;
while (dlist != NULL)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  dlist = dlist->next;

  if (bond->atom2 == NULL) continue;
  if (bond->first == TRUE) { bond->first = FALSE; continue; }
  else bond->first = TRUE;

  gamgi_mesa_rotate_bond (bond, quaternion, rotate, center);
  }

dlist = atom->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = atom->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_direction (gamgi_direction *direction,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_direction (direction, quaternion, rotate, center);

dlist = direction->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = direction->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = direction->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_plane (gamgi_plane *plane,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_plane (plane, quaternion, rotate, center);

dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_group (gamgi_group *group,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_group (group, quaternion, rotate, center);

dlist = group->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = group->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = group->text_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_molecule (gamgi_molecule *molecule,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_molecule (molecule, quaternion, rotate, center);

dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = molecule->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; } 

dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = molecule->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = molecule->text_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; } 
}

void gamgi_mesa_rotate_cluster (gamgi_cluster *cluster,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_cluster (cluster, quaternion, rotate, center);

dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cluster (GAMGI_CAST_CLUSTER dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_molecule (GAMGI_CAST_MOLECULE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cluster->text_start;
while (dlist != NULL) 
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_cell (gamgi_cell *cell,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_cell (cell, quaternion, rotate, center);

dlist = cell->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cluster (GAMGI_CAST_CLUSTER dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_molecule (GAMGI_CAST_MOLECULE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->plane_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = cell->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_arrow (gamgi_arrow *arrow,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_arrow (arrow, quaternion, rotate, center);

dlist = arrow->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_shape (gamgi_shape *shape,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_shape (shape, quaternion, rotate, center);

dlist = shape->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_graph (gamgi_graph *graph,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_graph (graph, quaternion, rotate, center);

dlist = graph->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_assembly (gamgi_assembly *assembly,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_assembly (assembly, quaternion, rotate, center);

dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_assembly (GAMGI_CAST_ASSEMBLY dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->graph_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_graph (GAMGI_CAST_GRAPH dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->shape_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_shape (GAMGI_CAST_SHAPE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->arrow_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_arrow (GAMGI_CAST_ARROW dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cell (GAMGI_CAST_CELL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cluster (GAMGI_CAST_CLUSTER dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_molecule (GAMGI_CAST_MOLECULE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = assembly->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_light (gamgi_light *light,  
double *quaternion, double *rotate, double *center)
{
gamgi_mesa_rotate_out_light (light, quaternion, rotate, center);
}

void gamgi_mesa_rotate_layer (gamgi_layer *layer,  
double *quaternion, double *rotate, double *center)
{
gamgi_dlist *dlist;

gamgi_mesa_rotate_out_layer (layer, quaternion, rotate, center);

dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_assembly (GAMGI_CAST_ASSEMBLY dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->graph_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_graph (GAMGI_CAST_GRAPH dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->shape_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_shape (GAMGI_CAST_SHAPE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->arrow_start;
while (dlist != NULL)
  {  gamgi_mesa_rotate_arrow (GAMGI_CAST_ARROW dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cell (GAMGI_CAST_CELL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_cluster (GAMGI_CAST_CLUSTER dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_molecule (GAMGI_CAST_MOLECULE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_group (GAMGI_CAST_GROUP dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }

dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_mesa_rotate_text (GAMGI_CAST_TEXT dlist->data, quaternion, rotate, center);
    dlist = dlist->next; }
}

void gamgi_mesa_rotate_object (gamgi_object *object,
double *quaternion, double *rotate, double *center)
{
gamgi_direction *direction;
gamgi_plane *plane;

/*****************************************
 * rotate object around geometric center *
 *****************************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_rotate_text (GAMGI_CAST_TEXT object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_rotate_orbital (GAMGI_CAST_ORBITAL object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_rotate_out_bond (GAMGI_CAST_BOND object, quaternion, rotate, center);
  gamgi_mesa_rotate_bond (GAMGI_CAST_BOND object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_rotate_atom (GAMGI_CAST_ATOM object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION object;
  if (direction->model == GAMGI_PHYS_LINE)
    gamgi_mesa_rotate_direction (GAMGI_CAST_DIRECTION object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE object;
  if (plane->model == GAMGI_PHYS_POLYGON)
    gamgi_mesa_rotate_plane (GAMGI_CAST_PLANE object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_rotate_group (GAMGI_CAST_GROUP object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_rotate_molecule (GAMGI_CAST_MOLECULE object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_rotate_cluster (GAMGI_CAST_CLUSTER object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_rotate_cell (GAMGI_CAST_CELL object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_rotate_arrow (GAMGI_CAST_ARROW object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_rotate_shape (GAMGI_CAST_SHAPE object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_rotate_graph (GAMGI_CAST_GRAPH object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_rotate_assembly (GAMGI_CAST_ASSEMBLY object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_rotate_light (GAMGI_CAST_LIGHT object, quaternion, rotate, center);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_rotate_layer (GAMGI_CAST_LAYER object, quaternion, rotate, center);
  break;

  default:
  break;
  }
}

void gamgi_mesa_rotate_text_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_text *text;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, TRUE);

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  if (text->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT text,
    &layer, angle, axis, quaternion, rotate);
    gamgi_mesa_rotate_text (text, quaternion, rotate, text->center);
    }
  }

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, FALSE);
}

void gamgi_mesa_rotate_orbital_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_orbital *orbital;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->orbitals; dlist != NULL; dlist = dlist->next)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;

  static_layer (GAMGI_CAST_OBJECT orbital,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_orbital (orbital, quaternion, rotate, orbital->center);
  }
}

void gamgi_mesa_rotate_bond_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_atom *atom1, *atom2;
gamgi_bond *bond;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];
double center[3];

for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  { 
  bond = GAMGI_CAST_BOND dlist->data;
  atom1 = GAMGI_CAST_ATOM bond->atom1;
  atom2 = GAMGI_CAST_ATOM bond->atom2;
  gamgi_math_vector_add (atom1->position, atom2->position, center);
  gamgi_math_vector_scale (center, center, 0.5);

  static_layer (GAMGI_CAST_OBJECT bond,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_out_bond (bond, quaternion, rotate, center);
  gamgi_mesa_rotate_bond (bond, quaternion, rotate, center);
  } 
}

void gamgi_mesa_rotate_atom_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_atom *atom;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->atoms; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  static_layer (GAMGI_CAST_OBJECT atom,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_atom (atom, quaternion, rotate, atom->position);
  }
}

void gamgi_mesa_rotate_direction_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_direction *direction;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->directions; dlist != NULL; dlist = dlist->next)
  { 
  direction = GAMGI_CAST_DIRECTION dlist->data;
  if (direction->model != GAMGI_PHYS_LINE) continue;

  static_layer (GAMGI_CAST_OBJECT direction,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_direction (direction, quaternion, rotate, direction->center);
  }
}

void gamgi_mesa_rotate_plane_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_plane *plane;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->planes; dlist != NULL; dlist = dlist->next)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  if (plane->model != GAMGI_PHYS_POLYGON) continue;

  static_layer (GAMGI_CAST_OBJECT plane,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_plane (plane, quaternion, rotate, plane->center);
  }
}

void gamgi_mesa_rotate_group_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_group *group;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, TRUE);

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  if (group->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT group,
    &layer, angle, axis, quaternion, rotate);
    gamgi_mesa_rotate_group (group, quaternion, rotate, group->center);
    }
  }

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, FALSE);
}

void gamgi_mesa_rotate_molecule_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_molecule *molecule;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->molecules; dlist != NULL; dlist = dlist->next)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;

  static_layer (GAMGI_CAST_OBJECT molecule,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_molecule (molecule, quaternion, rotate, molecule->center);
  }
}

void gamgi_mesa_rotate_cluster_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_cluster *cluster;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, TRUE);

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  if (cluster->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT cluster,
    &layer, angle, axis, quaternion, rotate);
    gamgi_mesa_rotate_cluster (cluster, quaternion, rotate, cluster->center);
    }
  }

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, FALSE);
}

void gamgi_mesa_rotate_cell_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_cell *cell;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  {
  cell = GAMGI_CAST_CELL dlist->data;

  static_layer (GAMGI_CAST_OBJECT cell,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_cell (cell, quaternion, rotate, cell->center);
  }
}

void gamgi_mesa_rotate_arrow_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_arrow *arrow;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];
double center[3];

for (dlist = gamgi->arrows; dlist != NULL; dlist = dlist->next)
  {
  arrow = GAMGI_CAST_ARROW dlist->data;
  static_center (GAMGI_CAST_OBJECT arrow, center);

  static_layer (GAMGI_CAST_OBJECT arrow,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_arrow (arrow, quaternion, rotate, center);
  }
}

void gamgi_mesa_rotate_shape_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_shape *shape;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];
double center[3];

for (dlist = gamgi->shapes; dlist != NULL; dlist = dlist->next)
  {
  shape = GAMGI_CAST_SHAPE dlist->data;
  static_center (GAMGI_CAST_OBJECT shape, center);

  static_layer (GAMGI_CAST_OBJECT shape,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_shape (shape, quaternion, rotate, center);
  }
}

void gamgi_mesa_rotate_graph_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_graph *graph;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];
double center[3];

for (dlist = gamgi->graphs; dlist != NULL; dlist = dlist->next)
  {
  graph = GAMGI_CAST_GRAPH dlist->data;
  static_center (GAMGI_CAST_OBJECT graph, center);

  static_layer (GAMGI_CAST_OBJECT graph,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_graph (graph, quaternion, rotate, center);
  }
}

void gamgi_mesa_rotate_assembly_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_assembly *assembly;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, TRUE);

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  {
  assembly = GAMGI_CAST_ASSEMBLY dlist->data;
  if (assembly->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT assembly,
    &layer, angle, axis, quaternion, rotate);
    gamgi_mesa_rotate_assembly (assembly, quaternion, rotate, assembly->center);
    }
  }

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, FALSE);
}

void gamgi_mesa_rotate_light_list (double angle, double *axis)
{
gamgi_layer *layer = NULL;
gamgi_light *light;
gamgi_dlist *dlist;
double rotate[9];
double quaternion[4];
double center[3];

gamgi_math_vector_zero (center);
for (dlist = gamgi->lights; dlist != NULL; dlist = dlist->next)
  {
  light = GAMGI_CAST_LIGHT dlist->data;

  static_layer (GAMGI_CAST_OBJECT light,
  &layer, angle, axis, quaternion, rotate);
  gamgi_mesa_rotate_light (light, quaternion, rotate, center);
  }
}

void gamgi_mesa_rotate_layer_in (gamgi_layer *layer, 
gamgi_object *object, double angle, double *axis)
{
double rotate[9];
double center[3];
double quaternion[4];

/*************************************************
 * get rotation as function of layer positioning *
 *************************************************/

static_rotate (layer, angle, axis, quaternion, rotate);
static_center (object, center);
gamgi_mesa_rotate_object (object, quaternion, rotate, center);
}

void gamgi_mesa_rotate_layer_out (gamgi_layer *layer, 
double angle, double *axis)
{
double rotate_old[9], rotate_new[9], z[3];
double *referential; 
double length;

/******************************
 * get new referential matrix *
 ******************************/

gamgi_math_vector_normal (axis);
gamgi_math_matrix_rotation (angle * GAMGI_MATH_RAD_DEG, axis, rotate_old);

referential = layer->referential;
gamgi_math_matrix_matrix (referential, rotate_old, rotate_new);
gamgi_math_matrix_copy (rotate_new, referential);

/*********************
 * get new up vector *
 *********************/

gamgi_math_vector_absolute (layer->up,
referential[1], referential[4], referential[7]);

/*************************
 * get new center vector *
 *************************/

gamgi_math_vector_sub (layer->eye, layer->center, z);
length = gamgi_math_vector_length (z);

gamgi_math_vector_absolute (z,
referential[2], referential[5], referential[8]);
gamgi_math_vector_scale (z, z, length);
gamgi_math_vector_sub (layer->eye, z, layer->center);
}

void gamgi_mesa_rotate_layer_list (double angle, double *axis)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_dlist *dlist;

for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  window = GAMGI_CAST_WINDOW layer->object.object;
  if (window->axes == FALSE)
    gamgi_mesa_rotate_layer_in (layer, GAMGI_CAST_OBJECT layer, angle, axis);
  else
    gamgi_mesa_rotate_layer_out (layer, angle, axis);
  }
}

void gamgi_mesa_rotate_window_in (gamgi_window *window, double angle,
double *axis)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

/*********************
 * rotate all layers *
 *********************/

for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_mesa_rotate_layer_in (layer, GAMGI_CAST_OBJECT layer, angle, axis);
  }
}

void gamgi_mesa_rotate_window_out (gamgi_window *window, double angle,
double *axis)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

/*********************
 * rotate all layers *
 *********************/

for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_mesa_rotate_layer_out (layer, angle, axis);
  }
}

void gamgi_mesa_rotate_window_list (double angle, double *axis)
{
gamgi_window *window;
gamgi_dlist *dlist;

for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  {
  window = GAMGI_CAST_WINDOW dlist->data;

  if (window->axes == FALSE)
    gamgi_mesa_rotate_window_in (window, angle, axis);
  else
    gamgi_mesa_rotate_window_out (window, angle, axis);
  }
}

void gamgi_mesa_rotate_gamgi_list (double angle, double *axis)
{
gamgi_window *window;

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  if (window->axes == FALSE)
    gamgi_mesa_rotate_window_in (window, angle, axis);
  else
    gamgi_mesa_rotate_window_out (window, angle, axis);
  }
}

void gamgi_mesa_rotate_list (double angle, double *axis)
{
switch (gamgi->focus)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_rotate_text_list (angle, axis);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_rotate_orbital_list (angle, axis);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_rotate_bond_list (angle, axis);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_rotate_atom_list (angle, axis);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_mesa_rotate_direction_list (angle, axis);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_mesa_rotate_plane_list (angle, axis);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_rotate_group_list (angle, axis);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_rotate_molecule_list (angle, axis);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_rotate_cluster_list (angle, axis);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_rotate_cell_list (angle, axis);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_rotate_arrow_list (angle, axis);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_rotate_shape_list (angle, axis);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_rotate_graph_list (angle, axis);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_rotate_assembly_list (angle, axis);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_rotate_light_list (angle, axis);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_rotate_layer_list (angle, axis);
  break;

  case GAMGI_ENGINE_WINDOW:
  gamgi_mesa_rotate_window_list (angle, axis);
  break;

  case GAMGI_ENGINE_GAMGI:
  gamgi_mesa_rotate_gamgi_list (angle, axis);
  break;

  default:
  return;
  }

/****************************
 * redraw all OpenGL images *
 ****************************/

gamgi_mesa_draw_gamgi ();
}

void gamgi_mesa_rotate (gamgi_window *window,
gamgi_object *object, double angle, double *axis)
{
/*************************************************************
 * rotate list of objects, including the Gamgi global object *
 *************************************************************/

if (gamgi->focus != FALSE)
  { gamgi_mesa_rotate_list (angle, axis); return; }
  
/*********************************************
 * rotate object with focus, which can be a  *
 * window, a layer or any other local object *
 *********************************************/

switch (object->class)
  {
  /***************************************
   * rotate all layers in current window *
   ***************************************/

  case GAMGI_ENGINE_WINDOW:
  if (window->axes == FALSE)
    gamgi_mesa_rotate_window_in (window, angle, axis);
  else
    gamgi_mesa_rotate_window_out (window, angle, axis);
  break;

  /******************************************
   * rotate current layer in current window *
   ******************************************/

  case GAMGI_ENGINE_LAYER:
  if (window->axes == FALSE)
    gamgi_mesa_rotate_layer_in (window->layer, object, angle, axis);
  else
    gamgi_mesa_rotate_layer_out (window->layer, angle, axis);
  break;

  /******************************************
   * rotate current object in current layer *
   ******************************************/

  default:
  gamgi_mesa_rotate_layer_in (window->layer, object, angle, axis);
  break;
  }
  
/***********************
 * redraw OpenGL image *
 ***********************/

gtk_widget_queue_draw (window->area);
}
