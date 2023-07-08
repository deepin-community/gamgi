/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_count.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

void gamgi_engine_count_text (gamgi_text *text, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* count text */
dlist = text->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_TEXT) *number += 1;
}

void gamgi_engine_count_orbital (gamgi_orbital *orbital, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_ORBITAL)
  {
  *number += 1;
  return;
  }

/* count text */
dlist = orbital->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_bond (gamgi_bond *bond, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* in case second link does not exist */
if (bond->atom2 == NULL) return;

/* first scan */
if (bond->first == TRUE) bond->first = FALSE;
/* second scan */
else
  {
  bond->first = TRUE;

  if (class == GAMGI_ENGINE_BOND)
    {
    *number += 1;
    return;
    }
  /* draw orbital list */
  dlist = bond->orbital_start;
  while (dlist != NULL)
    { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
      dlist = dlist->next; }

 /* draw text list */
  dlist = bond->text_start;
  while (dlist != NULL)
    { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
      dlist = dlist->next; }

  }
}

void gamgi_engine_count_atom (gamgi_atom *atom, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_ATOM)
  {
  *number += 1;
  return;
  }

/* count bond */
dlist = atom->bond_start;
while (dlist != NULL)
  { gamgi_engine_count_bond (GAMGI_CAST_BOND dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_BOND) return;

/* count orbital */
dlist = atom->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = atom->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_direction (gamgi_direction *direction, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_DIRECTION)
  {
  *number += 1;
  return;
  }

/* count atom */
dlist = direction->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = direction->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = direction->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_plane (gamgi_plane *plane, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_PLANE)
  {
  *number += 1;
  return;
  }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_group (gamgi_group *group, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* count group */
dlist = group->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP)
  {
  *number += 1;
  return;
  }

/* count plane */
dlist = group->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = group->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_molecule (gamgi_molecule *molecule, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_MOLECULE)
  {
  *number += 1;
  return;
  }

/* count group */
dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) return;

/* count plane */
dlist = molecule->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = molecule->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = molecule->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_cluster (gamgi_cluster *cluster, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* count cluster */
dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_engine_count_cluster (GAMGI_CAST_CLUSTER dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CLUSTER)
  {
  *number += 1;
  return;
  }

/* count molecule */
dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_engine_count_molecule (GAMGI_CAST_MOLECULE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_MOLECULE) return;

/* count group */
dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) return;

/* count plane */
dlist = cluster->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = cluster->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = cluster->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_cell (gamgi_cell *cell, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_CELL)
  {
  *number += 1;
  return;
  }

/* count cluster */
dlist = cell->cluster_start;
while (dlist != NULL)
  { gamgi_engine_count_cluster (GAMGI_CAST_CLUSTER dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CLUSTER) return;

/* count molecule */
dlist = cell->molecule_start;
while (dlist != NULL)
  { gamgi_engine_count_molecule (GAMGI_CAST_MOLECULE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_MOLECULE) return;

/* count group */
dlist = cell->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) return;

/* count plane */
dlist = cell->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = cell->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = cell->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = cell->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = cell->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_arrow (gamgi_arrow *arrow, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_ARROW)
  {
  *number += 1;
  return;
  }

/* count text */
dlist = arrow->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_shape (gamgi_shape *shape, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_SHAPE)
  {
  *number += 1;
  return;
  }

/* count text */
dlist = shape->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_graph (gamgi_graph *graph, 
int class, int *number) 
{
gamgi_dlist *dlist;

if (class == GAMGI_ENGINE_GRAPH)
  {
  *number += 1;
  return;
  }

/* count text */
dlist = graph->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_assembly (gamgi_assembly *assembly, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* count assembly */
dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_engine_count_assembly (GAMGI_CAST_ASSEMBLY dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ASSEMBLY)
  {
  *number += 1;
  return;
  }

/* count graph */
dlist = assembly->graph_start;
while (dlist != NULL)
  { gamgi_engine_count_graph (GAMGI_CAST_GRAPH dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GRAPH) return;

/* count shape */
dlist = assembly->shape_start;
while (dlist != NULL)
  { gamgi_engine_count_shape (GAMGI_CAST_SHAPE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_SHAPE) return;

/* count arrow */
dlist = assembly->arrow_start;
while (dlist != NULL)
  { gamgi_engine_count_arrow (GAMGI_CAST_ARROW dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ARROW) return;

/* count cell */
dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_engine_count_cell (GAMGI_CAST_CELL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CELL) return;

/* count cluster */
dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_engine_count_cluster (GAMGI_CAST_CLUSTER dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CLUSTER) return;

/* count molecule */
dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_engine_count_molecule (GAMGI_CAST_MOLECULE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_MOLECULE) return;

/* count group */
dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) return;

/* count plane */
dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = assembly->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = assembly->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_layer (gamgi_layer *layer, 
int class, int *number) 
{
gamgi_dlist *dlist;

/* count assembly */
dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_engine_count_assembly (GAMGI_CAST_ASSEMBLY dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ASSEMBLY) return;

/* count graph */
dlist = layer->graph_start;
while (dlist != NULL)
  { gamgi_engine_count_graph (GAMGI_CAST_GRAPH dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GRAPH) return;

/* count shape */
dlist = layer->shape_start;
while (dlist != NULL)
  { gamgi_engine_count_shape (GAMGI_CAST_SHAPE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_SHAPE) return;

/* count arrow */
dlist = layer->arrow_start;
while (dlist != NULL)
  { gamgi_engine_count_arrow (GAMGI_CAST_ARROW dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ARROW) return;

/* count cell */
dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_engine_count_cell (GAMGI_CAST_CELL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CELL) return;

/* count cluster */
dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_engine_count_cluster (GAMGI_CAST_CLUSTER dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CLUSTER) return;

/* count molecule */
dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_engine_count_molecule (GAMGI_CAST_MOLECULE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_MOLECULE) return;

/* count group */
dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_engine_count_group (GAMGI_CAST_GROUP dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) return;

/* count plane */
dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_engine_count_plane (GAMGI_CAST_PLANE dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) return;

/* count direction */
dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_engine_count_direction (GAMGI_CAST_DIRECTION dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) return;

/* count atom */
dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_engine_count_atom (GAMGI_CAST_ATOM dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) return;

/* count orbital */
dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL dlist->data, class, number);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) return;

/* count text */
dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_engine_count_text (GAMGI_CAST_TEXT dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_window (gamgi_window *window,
gamgi_enum class, int *number)
{
gamgi_dlist *dlist;

/***************************************
 * count class objects in window scope *
 ***************************************/

dlist = window->layer_start;
while (dlist != NULL)
  { gamgi_engine_count_layer (GAMGI_CAST_LAYER dlist->data, class, number);
    dlist = dlist->next; }
}

void gamgi_engine_count_gamgi (gamgi_enum class, int *number)
{
gamgi_window *window;

/**************************************
 * count class objects in gamgi scope *
 **************************************/

window = gamgi->window_start;
while (window != NULL)
  { gamgi_engine_count_window (window, class, number);
    window = window->next; }
}

void gamgi_engine_count_object (gamgi_object *object,
gamgi_enum class, int *number)
{

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_count_text (GAMGI_CAST_TEXT object, class, number);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_count_orbital (GAMGI_CAST_ORBITAL object, class, number);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_engine_count_bond (GAMGI_CAST_BOND object, class, number);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_count_atom (GAMGI_CAST_ATOM object, class, number);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_count_direction (GAMGI_CAST_DIRECTION object, class, number);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_count_plane (GAMGI_CAST_PLANE object, class, number);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_count_group (GAMGI_CAST_GROUP object, class, number);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_count_molecule (GAMGI_CAST_MOLECULE object, class, number);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_count_cluster (GAMGI_CAST_CLUSTER object, class, number);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_count_cell (GAMGI_CAST_CELL object, class, number);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_count_arrow (GAMGI_CAST_ARROW object, class, number);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_count_shape (GAMGI_CAST_SHAPE object, class, number);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_count_graph (GAMGI_CAST_GRAPH object, class, number);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_count_assembly (GAMGI_CAST_ASSEMBLY object, class, number);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_engine_count_layer (GAMGI_CAST_LAYER object, class, number);
  break;

  default:
  break;
  }

}
