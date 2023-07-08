/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_number.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_engine_list.h"

/******************************************
 * Initialise numbers for all objects.    *
 * These numbers are static, so they are  *
 * kept in memory untill GAMGI is closed. *
 ******************************************/

static unsigned int text_number = 1;
static unsigned int orbital_number = 1;
static unsigned int bond_number = 1;
static unsigned int atom_number = 1;
static unsigned int direction_number = 1;
static unsigned int plane_number = 1;
static unsigned int group_number = 1;
static unsigned int molecule_number = 1;
static unsigned int cluster_number = 1;
static unsigned int cell_number = 1;
static unsigned int arrow_number = 1;
static unsigned int shape_number = 1;
static unsigned int graph_number = 1;
static unsigned int assembly_number = 1;
static unsigned int light_number = 1;
static unsigned int layer_number = 1;
static unsigned int window_number = 1;

/***************************************************
 * Initialise recycling lists for all objects.     *
 * Everytime an object is removed, its number is   *
 * added to the corresponding list. When an object *
 * is created, its number will be taken from the   *
 * recycled list, unless the list is empty.        *
 ***************************************************/
 
static gamgi_slist *text_start = NULL, *text_end = NULL;
static gamgi_slist *orbital_start = NULL, *orbital_end = NULL;
static gamgi_slist *bond_start = NULL, *bond_end = NULL;
static gamgi_slist *atom_start = NULL, *atom_end = NULL;
static gamgi_slist *direction_start = NULL, *direction_end = NULL;
static gamgi_slist *plane_start = NULL, *plane_end = NULL;
static gamgi_slist *group_start = NULL, *group_end = NULL;
static gamgi_slist *molecule_start = NULL, *molecule_end = NULL;
static gamgi_slist *cluster_start = NULL, *cluster_end = NULL;
static gamgi_slist *cell_start = NULL, *cell_end = NULL;
static gamgi_slist *arrow_start = NULL, *arrow_end = NULL;
static gamgi_slist *shape_start = NULL, *shape_end = NULL;
static gamgi_slist *graph_start = NULL, *graph_end = NULL;
static gamgi_slist *assembly_start = NULL, *assembly_end = NULL;
static gamgi_slist *light_start = NULL, *light_end = NULL;
static gamgi_slist *layer_start = NULL, *layer_end = NULL;
static gamgi_slist *window_start = NULL, *window_end = NULL;

unsigned int gamgi_engine_number_text_get (void)
{
unsigned int number;

if (text_start == NULL) return text_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (text_start->data);
  text_start = gamgi_engine_slist_remove_start (text_start);
  if (text_start == NULL) text_end = NULL;
  return number;
  }
}

void gamgi_engine_number_text_recycle (unsigned int number)
{
text_end = gamgi_engine_slist_add_end (text_end);
if (text_start == NULL) text_start = text_end;
text_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_text_reset (void)
{
gamgi_engine_slist_remove_all (text_start);

text_start = NULL;
text_end = NULL;
text_number = 1;
}

unsigned int gamgi_engine_number_orbital_get (void)
{
unsigned int number;

if (orbital_start == NULL) return orbital_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (orbital_start->data);
  orbital_start = gamgi_engine_slist_remove_start (orbital_start);
  if (orbital_start == NULL) orbital_end = NULL;
  return number;
  }
}

void gamgi_engine_number_orbital_recycle (unsigned int number)
{
orbital_end = gamgi_engine_slist_add_end (orbital_end);
if (orbital_start == NULL) orbital_start = orbital_end;
orbital_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_orbital_reset (void)
{
gamgi_engine_slist_remove_all (orbital_start);

orbital_start = NULL;
orbital_end = NULL;
orbital_number = 1;
}

unsigned int gamgi_engine_number_bond_get (void)
{
unsigned int number;

if (bond_start == NULL) return bond_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (bond_start->data);
  bond_start = gamgi_engine_slist_remove_start (bond_start);
  if (bond_start == NULL) bond_end = NULL;
  return number;
  }
}

void gamgi_engine_number_bond_recycle (unsigned int number)
{
bond_end = gamgi_engine_slist_add_end (bond_end);
if (bond_start == NULL) bond_start = bond_end;
bond_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_bond_reset (void)
{
gamgi_engine_slist_remove_all (bond_start);

bond_start = NULL;
bond_end = NULL;
bond_number = 1;
}

unsigned int gamgi_engine_number_atom_get (void)
{
unsigned int number;

if (atom_start == NULL) return atom_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (atom_start->data);
  atom_start = gamgi_engine_slist_remove_start (atom_start);
  if (atom_start == NULL) atom_end = NULL;
  return number;
  }
}

void gamgi_engine_number_atom_recycle (unsigned int number)
{
atom_end = gamgi_engine_slist_add_end (atom_end);
if (atom_start == NULL) atom_start = atom_end;
atom_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_atom_reset (void)
{
gamgi_engine_slist_remove_all (atom_start);

atom_start = NULL;
atom_end = NULL;
atom_number = 1;
}

unsigned int gamgi_engine_number_direction_get (void)
{
unsigned int number;

if (direction_start == NULL) return direction_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (direction_start->data);
  direction_start = gamgi_engine_slist_remove_start (direction_start);
  if (direction_start == NULL) direction_end = NULL;
  return number;
  }
}

void gamgi_engine_number_direction_recycle (unsigned int number)
{
direction_end = gamgi_engine_slist_add_end (direction_end);
if (direction_start == NULL) direction_start = direction_end;
direction_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_direction_reset (void)
{
gamgi_engine_slist_remove_all (direction_start);

direction_start = NULL;
direction_end = NULL;
direction_number = 1;
}

unsigned int gamgi_engine_number_plane_get (void)
{
unsigned int number;

if (plane_start == NULL) return plane_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (plane_start->data);
  plane_start = gamgi_engine_slist_remove_start (plane_start);
  if (plane_start == NULL) plane_end = NULL;
  return number;
  }
}

void gamgi_engine_number_plane_recycle (unsigned int number)
{
plane_end = gamgi_engine_slist_add_end (plane_end);
if (plane_start == NULL) plane_start = plane_end;
plane_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_plane_reset (void)
{
gamgi_engine_slist_remove_all (plane_start);

plane_start = NULL;
plane_end = NULL;
plane_number = 1;
}

unsigned int gamgi_engine_number_group_get (void)
{
unsigned int number;

if (group_start == NULL) return group_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (group_start->data);
  group_start = gamgi_engine_slist_remove_start (group_start);
  if (group_start == NULL) group_end = NULL;
  return number;
  }
}

void gamgi_engine_number_group_recycle (unsigned int number)
{
group_end = gamgi_engine_slist_add_end (group_end);
if (group_start == NULL) group_start = group_end;
group_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_group_reset (void)
{
gamgi_engine_slist_remove_all (group_start);

group_start = NULL;
group_end = NULL;
group_number = 1;
}

unsigned int gamgi_engine_number_molecule_get (void)
{
unsigned int number;

if (molecule_start == NULL) return molecule_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (molecule_start->data);
  molecule_start = gamgi_engine_slist_remove_start (molecule_start);
  if (molecule_start == NULL) molecule_end = NULL;
  return number;
  }
}

void gamgi_engine_number_molecule_recycle (unsigned int number)
{
molecule_end = gamgi_engine_slist_add_end (molecule_end);
if (molecule_start == NULL) molecule_start = molecule_end;
molecule_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_molecule_reset (void)
{
gamgi_engine_slist_remove_all (molecule_start);

molecule_start = NULL;
molecule_end = NULL;
molecule_number = 1;
}

unsigned int gamgi_engine_number_cluster_get (void)
{
unsigned int number;

if (cluster_start == NULL) return cluster_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (cluster_start->data);
  cluster_start = gamgi_engine_slist_remove_start (cluster_start);
  if (cluster_start == NULL) cluster_end = NULL;
  return number;
  }
}

void gamgi_engine_number_cluster_recycle (unsigned int number)
{
cluster_end = gamgi_engine_slist_add_end (cluster_end);
if (cluster_start == NULL) cluster_start = cluster_end;
cluster_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_cluster_reset (void)
{
gamgi_engine_slist_remove_all (cluster_start);

cluster_start = NULL;
cluster_end = NULL;
cluster_number = 1;
}

unsigned int gamgi_engine_number_cell_get (void)
{
unsigned int number;

if (cell_start == NULL) return cell_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (cell_start->data);
  cell_start = gamgi_engine_slist_remove_start (cell_start);
  if (cell_start == NULL) cell_end = NULL;
  return number;
  }
}

void gamgi_engine_number_cell_recycle (unsigned int number)
{
cell_end = gamgi_engine_slist_add_end (cell_end);
if (cell_start == NULL) cell_start = cell_end;
cell_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_cell_reset (void)
{
gamgi_engine_slist_remove_all (cell_start);

cell_start = NULL;
cell_end = NULL;
cell_number = 1;
}

unsigned int gamgi_engine_number_arrow_get (void)
{
unsigned int number;

if (arrow_start == NULL) return arrow_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (arrow_start->data);
  arrow_start = gamgi_engine_slist_remove_start (arrow_start);
  if (arrow_start == NULL) arrow_end = NULL;
  return number;
  }
}

void gamgi_engine_number_arrow_recycle (unsigned int number)
{
arrow_end = gamgi_engine_slist_add_end (arrow_end);
if (arrow_start == NULL) arrow_start = arrow_end;
arrow_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_arrow_reset (void)
{
gamgi_engine_slist_remove_all (arrow_start);

arrow_start = NULL;
arrow_end = NULL;
arrow_number = 1;
}

unsigned int gamgi_engine_number_shape_get (void)
{
unsigned int number;

if (shape_start == NULL) return shape_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (shape_start->data);
  shape_start = gamgi_engine_slist_remove_start (shape_start);
  if (shape_start == NULL) shape_end = NULL;
  return number;
  }
}

void gamgi_engine_number_shape_recycle (unsigned int number)
{
shape_end = gamgi_engine_slist_add_end (shape_end);
if (shape_start == NULL) shape_start = shape_end;
shape_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_shape_reset (void)
{
gamgi_engine_slist_remove_all (shape_start);

shape_start = NULL;
shape_end = NULL;
shape_number = 1;
}

unsigned int gamgi_engine_number_graph_get (void)
{
unsigned int number;

if (graph_start == NULL) return graph_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (graph_start->data);
  graph_start = gamgi_engine_slist_remove_start (graph_start);
  if (graph_start == NULL) graph_end = NULL;
  return number;
  }
}

void gamgi_engine_number_graph_recycle (unsigned int number)
{
graph_end = gamgi_engine_slist_add_end (graph_end);
if (graph_start == NULL) graph_start = graph_end;
graph_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_graph_reset (void)
{
gamgi_engine_slist_remove_all (graph_start);

graph_start = NULL;
graph_end = NULL;
graph_number = 1;
}

unsigned int gamgi_engine_number_assembly_get (void)
{
unsigned int number;

if (assembly_start == NULL) return assembly_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (assembly_start->data);
  assembly_start = gamgi_engine_slist_remove_start (assembly_start);
  if (assembly_start == NULL) assembly_end = NULL;
  return number;
  }
}

void gamgi_engine_number_assembly_recycle (unsigned int number)
{
assembly_end = gamgi_engine_slist_add_end (assembly_end);
if (assembly_start == NULL) assembly_start = assembly_end;
assembly_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_assembly_reset (void)
{
gamgi_engine_slist_remove_all (assembly_start);

assembly_start = NULL;
assembly_end = NULL;
assembly_number = 1;
}

unsigned int gamgi_engine_number_light_get (void)
{
unsigned int number;

if (light_start == NULL) return light_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (light_start->data);
  light_start = gamgi_engine_slist_remove_start (light_start);
  if (light_start == NULL) light_end = NULL;
  return number;
  }
}

void gamgi_engine_number_light_recycle (unsigned int number)
{
light_end = gamgi_engine_slist_add_end (light_end);
if (light_start == NULL) light_start = light_end;
light_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_light_reset (void)
{
gamgi_engine_slist_remove_all (light_start);

light_start = NULL;
light_end = NULL;
light_number = 1;
}

unsigned int gamgi_engine_number_layer_get (void)
{
unsigned int number;

if (layer_start == NULL) return layer_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (layer_start->data);
  layer_start = gamgi_engine_slist_remove_start (layer_start);
  if (layer_start == NULL) layer_end = NULL;
  return number;
  }
}

void gamgi_engine_number_layer_recycle (unsigned int number)
{
layer_end = gamgi_engine_slist_add_end (layer_end);
if (layer_start == NULL) layer_start = layer_end;
layer_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_layer_reset (void)
{
gamgi_engine_slist_remove_all (layer_start);

layer_start = NULL;
layer_end = NULL;
layer_number = 1;
}

unsigned int gamgi_engine_number_window_get (void)
{
unsigned int number;

if (window_start == NULL) return window_number++;
else
  {
  number = GAMGI_POINTER_TO_INT (window_start->data);
  window_start = gamgi_engine_slist_remove_start (window_start);
  if (window_start == NULL) window_end = NULL;
  return number;
  }
}

void gamgi_engine_number_window_recycle (unsigned int number)
{
window_end = gamgi_engine_slist_add_end (window_end);
if (window_start == NULL) window_start = window_end;
window_end->data = GAMGI_INT_TO_POINTER (number);
}

void gamgi_engine_number_window_reset (void)
{
gamgi_engine_slist_remove_all (window_start);
window_start = NULL;
window_end = NULL;
window_number = 1;
}
