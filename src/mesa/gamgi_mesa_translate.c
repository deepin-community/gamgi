/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_translate.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_find.h"
#include "gamgi_mesa_translate_out.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_global_selection.h"

static void static_translate (gamgi_layer *layer, double *shift, double *translate)
{
double *referential;
double x[3], y[3], z[3];

referential = layer->referential;
gamgi_math_vector_absolute (x, referential[0], referential[3], referential[6]);
gamgi_math_vector_absolute (y, referential[1], referential[4], referential[7]);
gamgi_math_vector_absolute (z, referential[2], referential[5], referential[8]);

/*****************************************
 * apply translation along vectors x,y,z *
 *****************************************/

gamgi_math_vector_scale (x, x, shift[0]);
gamgi_math_vector_scale (y, y, shift[1]);
gamgi_math_vector_scale (z, z, shift[2]);

/*******************************************
 * add x,y,z to get the global translation *
 *******************************************/

translate[0] = x[0] + y[0] + z[0];
translate[1] = x[1] + y[1] + z[1];
translate[2] = x[2] + y[2] + z[2];
}

static void static_layer (gamgi_object *object, gamgi_layer **layer_old,
double *shift, double *translate)
{
gamgi_layer *layer;

/***************************************************************
 * translate must be recalculated only when layer changes, as  *
 * shift remains the same throughout the translation operation *
 ***************************************************************/

layer = gamgi_engine_find_layer (object);
if (layer != *layer_old)
  {
  *layer_old = layer;
  static_translate (layer, shift, translate);
  }
}

void gamgi_mesa_translate_text (gamgi_text *text, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_text (text, translate);

dlist = text->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_orbital (gamgi_orbital *orbital, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_orbital (orbital, translate);

dlist = orbital->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_bond (gamgi_bond *bond, double *translate)
{
gamgi_dlist *dlist;

dlist = bond->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = bond->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_atom (gamgi_atom *atom, double *translate)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

gamgi_mesa_translate_out_atom (atom, translate);

/************************************************
 * translate bond elements only the second time *
 ************************************************/

dlist = atom->bond_start;
while (dlist != NULL)
  { 
  bond = GAMGI_CAST_BOND dlist->data;
  dlist = dlist->next;

  if (bond->atom2 == NULL) continue;
  if (bond->first == TRUE) { bond->first = FALSE; continue; }
  else bond->first = TRUE;

  gamgi_mesa_translate_bond (bond, translate);
  }

dlist = atom->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = atom->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_direction (gamgi_direction *direction, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_direction (direction, translate);

dlist = direction->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = direction->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = direction->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_plane (gamgi_plane *plane, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_plane (plane, translate);

dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_group (gamgi_group *group, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_group (group, translate);

dlist = group->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = group->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; }

dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = group->text_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_molecule (gamgi_molecule *molecule, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_molecule (molecule, translate);

dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = molecule->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; } 

dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = molecule->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = molecule->text_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; } 
}

void gamgi_mesa_translate_cluster (gamgi_cluster *cluster, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_cluster (cluster, translate);

dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cluster (GAMGI_CAST_CLUSTER dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_translate_molecule (GAMGI_CAST_MOLECULE dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->plane_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = cluster->text_start;
while (dlist != NULL) 
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_cell (gamgi_cell *cell, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_cell (cell, translate);

dlist = cell->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cluster (GAMGI_CAST_CLUSTER dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_translate_molecule (GAMGI_CAST_MOLECULE dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->plane_start;
while (dlist != NULL)
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = cell->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_arrow (gamgi_arrow *arrow, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_arrow (arrow, translate);

dlist = arrow->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_shape (gamgi_shape *shape, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_shape (shape, translate);

dlist = shape->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_graph (gamgi_graph *graph, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_graph (graph, translate);

dlist = graph->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_assembly (gamgi_assembly *assembly, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_assembly (assembly, translate);

dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_translate_assembly (GAMGI_CAST_ASSEMBLY dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->graph_start;
while (dlist != NULL)
  { gamgi_mesa_translate_graph (GAMGI_CAST_GRAPH dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->shape_start;
while (dlist != NULL)
  { gamgi_mesa_translate_shape (GAMGI_CAST_SHAPE dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->arrow_start;
while (dlist != NULL)
  { gamgi_mesa_translate_arrow (GAMGI_CAST_ARROW dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cell (GAMGI_CAST_CELL dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cluster (GAMGI_CAST_CLUSTER dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_translate_molecule (GAMGI_CAST_MOLECULE dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = assembly->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_light (gamgi_light *light, double *translate)
{
gamgi_mesa_translate_out_light (light, translate);
}

void gamgi_mesa_translate_layer (gamgi_layer *layer, double *translate)
{
gamgi_dlist *dlist;

gamgi_mesa_translate_out_layer (layer, translate);

dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_translate_assembly (GAMGI_CAST_ASSEMBLY dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->graph_start;
while (dlist != NULL)
  { gamgi_mesa_translate_graph (GAMGI_CAST_GRAPH dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->shape_start;
while (dlist != NULL)
  { gamgi_mesa_translate_shape (GAMGI_CAST_SHAPE dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->arrow_start;
while (dlist != NULL)
  {  gamgi_mesa_translate_arrow (GAMGI_CAST_ARROW dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cell (GAMGI_CAST_CELL dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_translate_cluster (GAMGI_CAST_CLUSTER dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_translate_molecule (GAMGI_CAST_MOLECULE dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_mesa_translate_group (GAMGI_CAST_GROUP dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_mesa_translate_plane (GAMGI_CAST_PLANE dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_mesa_translate_direction (GAMGI_CAST_DIRECTION dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_mesa_translate_atom (GAMGI_CAST_ATOM dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL dlist->data, translate);
    dlist = dlist->next; }

dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_mesa_translate_text (GAMGI_CAST_TEXT dlist->data, translate);
    dlist = dlist->next; }
}

void gamgi_mesa_translate_object (gamgi_object *object, double *translate)
{
gamgi_plane *plane;
gamgi_direction *direction;

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_translate_text (GAMGI_CAST_TEXT object, translate);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_translate_orbital (GAMGI_CAST_ORBITAL object, translate);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_translate_out_bond (GAMGI_CAST_BOND object, translate);
  gamgi_mesa_translate_bond (GAMGI_CAST_BOND object, translate);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_translate_atom (GAMGI_CAST_ATOM object, translate);
  break;

  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION object;
  if (direction->model == GAMGI_PHYS_LINE)
    gamgi_mesa_translate_direction (direction, translate);
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE object;
  if (plane->model == GAMGI_PHYS_POLYGON)
    gamgi_mesa_translate_plane (plane, translate);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_translate_group (GAMGI_CAST_GROUP object, translate);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_translate_molecule (GAMGI_CAST_MOLECULE object, translate);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_translate_cluster (GAMGI_CAST_CLUSTER object, translate);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_translate_cell (GAMGI_CAST_CELL object, translate);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_translate_arrow (GAMGI_CAST_ARROW object, translate);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_translate_shape (GAMGI_CAST_SHAPE object, translate);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_translate_graph (GAMGI_CAST_GRAPH object, translate);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_translate_assembly (GAMGI_CAST_ASSEMBLY object, translate);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_translate_light (GAMGI_CAST_LIGHT object, translate);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_translate_layer (GAMGI_CAST_LAYER object, translate);
  break;

  default:
  break;
  }

}

void gamgi_mesa_translate_text_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_text *text;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, TRUE);

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  if (text->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT text, &layer, shift, translate);
    gamgi_mesa_translate_text (text, translate);
    }
  }

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, FALSE);
}

void gamgi_mesa_translate_orbital_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_orbital *orbital;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->orbitals; dlist != NULL; dlist = dlist->next)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;

  static_layer (GAMGI_CAST_OBJECT orbital, &layer, shift, translate);
  gamgi_mesa_translate_orbital (orbital, translate);
  }
}

void gamgi_mesa_translate_bond_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_bond *bond;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  { 
  bond = GAMGI_CAST_BOND dlist->data;

  static_layer (GAMGI_CAST_OBJECT bond, &layer, shift, translate);
  gamgi_mesa_translate_out_bond (bond, translate);
  gamgi_mesa_translate_bond (bond, translate);
  } 
}

void gamgi_mesa_translate_atom_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_atom *atom;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->atoms; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  static_layer (GAMGI_CAST_OBJECT atom, &layer, shift, translate);
  gamgi_mesa_translate_atom (atom, translate);
  }
}

void gamgi_mesa_translate_direction_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_direction *direction;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->directions; dlist != NULL; dlist = dlist->next)
  { 
  direction = GAMGI_CAST_DIRECTION dlist->data;
  if (direction->model != GAMGI_PHYS_LINE) continue;

  static_layer (GAMGI_CAST_OBJECT direction, &layer, shift, translate);
  gamgi_mesa_translate_direction (direction, translate);
  }
}

void gamgi_mesa_translate_plane_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_plane *plane;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->planes; dlist != NULL; dlist = dlist->next)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  if (plane->model != GAMGI_PHYS_POLYGON) continue;

  static_layer (GAMGI_CAST_OBJECT plane, &layer, shift, translate);
  gamgi_mesa_translate_plane (plane, translate);
  }
}

void gamgi_mesa_translate_group_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_group *group;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, TRUE);

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  if (group->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT group, &layer, shift, translate);
    gamgi_mesa_translate_group (group, translate);
    }
  }

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, FALSE);
}

void gamgi_mesa_translate_molecule_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_molecule *molecule;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->molecules; dlist != NULL; dlist = dlist->next)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;

  static_layer (GAMGI_CAST_OBJECT molecule, &layer, shift, translate);
  gamgi_mesa_translate_molecule (molecule, translate);
  }
}

void gamgi_mesa_translate_cluster_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_cluster *cluster;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, TRUE);

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  if (cluster->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT cluster, &layer, shift, translate);
    gamgi_mesa_translate_cluster (cluster, translate);
    }
  }

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, FALSE);
}

void gamgi_mesa_translate_cell_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_cell *cell;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  {
  cell = GAMGI_CAST_CELL dlist->data;

  static_layer (GAMGI_CAST_OBJECT cell, &layer, shift, translate);
  gamgi_mesa_translate_cell (cell, translate);
  }
}

void gamgi_mesa_translate_arrow_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_arrow *arrow;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->arrows; dlist != NULL; dlist = dlist->next)
  {
  arrow = GAMGI_CAST_ARROW dlist->data;

  static_layer (GAMGI_CAST_OBJECT arrow, &layer, shift, translate);
  gamgi_mesa_translate_arrow (arrow, translate);
  }
}

void gamgi_mesa_translate_shape_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_shape *shape;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->shapes; dlist != NULL; dlist = dlist->next)
  {
  shape = GAMGI_CAST_SHAPE dlist->data;

  static_layer (GAMGI_CAST_OBJECT shape, &layer, shift, translate);
  gamgi_mesa_translate_shape (shape, translate);
  }
}

void gamgi_mesa_translate_graph_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_graph *graph;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->graphs; dlist != NULL; dlist = dlist->next)
  {
  graph = GAMGI_CAST_GRAPH dlist->data;

  static_layer (GAMGI_CAST_OBJECT graph, &layer, shift, translate);
  gamgi_mesa_translate_graph (graph, translate);
  }
}

void gamgi_mesa_translate_assembly_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_assembly *assembly;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, TRUE);

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  {
  assembly = GAMGI_CAST_ASSEMBLY dlist->data;
  if (assembly->mark == FALSE)
    {
    static_layer (GAMGI_CAST_OBJECT assembly, &layer, shift, translate);
    gamgi_mesa_translate_assembly (assembly, translate);
    }
  }

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, FALSE);
}

void gamgi_mesa_translate_light_list (double *shift)
{
gamgi_layer *layer = NULL;
gamgi_light *light;
gamgi_dlist *dlist;
double translate[3];

for (dlist = gamgi->lights; dlist != NULL; dlist = dlist->next)
  {
  light = GAMGI_CAST_LIGHT dlist->data;

  static_layer (GAMGI_CAST_OBJECT light, &layer, shift, translate);
  gamgi_mesa_translate_light (light, translate);
  }
}

void gamgi_mesa_translate_layer_in (gamgi_layer *layer, 
gamgi_object *object, double *shift)
{
double translate[3];

/****************************************************
 * get translation as function of layer positioning *
 ****************************************************/

static_translate (layer, shift, translate);
gamgi_mesa_translate_object (object, translate);
}

void gamgi_mesa_translate_layer_out (gamgi_layer *layer, double *shift)
{
double *referential;
double x[3], y[3], z[3];
double translate[3];

referential = layer->referential;
gamgi_math_vector_absolute (x, referential[0], referential[3], referential[6]);
gamgi_math_vector_absolute (y, referential[1], referential[4], referential[7]);
gamgi_math_vector_absolute (z, referential[2], referential[5], referential[8]);

/*****************************************
 * apply translation along vectors x,y,z *
 *****************************************/

gamgi_math_vector_scale (x, x, shift[0]);
gamgi_math_vector_scale (y, y, shift[1]);
gamgi_math_vector_scale (z, z, shift[2]);

/*******************************************
 * add x,y,z to get the global translation *
 *******************************************/

translate[0] = x[0] + y[0] + z[0];
translate[1] = x[1] + y[1] + z[1];
translate[2] = x[2] + y[2] + z[2];

/**********************************
 * translate both eye and center, *
 * so the viewing direction and   *
 * distance remain the same       *
 **********************************/

gamgi_math_vector_add (layer->eye, translate, layer->eye);
gamgi_math_vector_add (layer->eye, translate, layer->eye);
gamgi_math_vector_add (layer->eye, translate, layer->eye);

gamgi_math_vector_add (layer->center, translate, layer->center);
gamgi_math_vector_add (layer->center, translate, layer->center);
gamgi_math_vector_add (layer->center, translate, layer->center);
}

void gamgi_mesa_translate_layer_list (double *shift)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_dlist *dlist;

for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  window = GAMGI_CAST_WINDOW layer->object.object;
  if (window->axes == FALSE)
    gamgi_mesa_translate_layer_in (layer, GAMGI_CAST_OBJECT layer, shift);
  else
    gamgi_mesa_translate_layer_out (layer, shift);
  }
}

void gamgi_mesa_translate_window_in (gamgi_window *window, double *shift)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

/************************
 * translate all layers *
 ************************/

for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_mesa_translate_layer_in (layer, GAMGI_CAST_OBJECT layer, shift);
  }
}

void gamgi_mesa_translate_window_out (gamgi_window *window, double *shift)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

/************************
 * translate all layers *
 ************************/

for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_mesa_translate_layer_out (layer, shift);
  }
}

void gamgi_mesa_translate_window_list (double *shift)
{
gamgi_window *window;
gamgi_dlist *dlist;

for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  {
  window = GAMGI_CAST_WINDOW dlist->data;

  if (window->axes == FALSE)
    gamgi_mesa_translate_window_in (window, shift);
  else
    gamgi_mesa_translate_window_out (window, shift);
  }
}

void gamgi_mesa_translate_gamgi_list (double *shift)
{
gamgi_window *window;

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  if (window->axes == FALSE)
    gamgi_mesa_translate_window_in (window, shift);
  else
    gamgi_mesa_translate_window_out (window, shift);
  }
}

void gamgi_mesa_translate_list (double *shift)
{
switch (gamgi->focus)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_translate_text_list (shift);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_translate_orbital_list (shift);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_translate_bond_list (shift);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_translate_atom_list (shift);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_mesa_translate_direction_list (shift);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_mesa_translate_plane_list (shift);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_translate_group_list (shift);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_translate_molecule_list (shift);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_translate_cluster_list (shift);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_translate_cell_list (shift);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_translate_arrow_list (shift);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_translate_shape_list (shift);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_translate_graph_list (shift);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_translate_assembly_list (shift);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_translate_light_list (shift);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_translate_layer_list (shift);
  break;

  case GAMGI_ENGINE_WINDOW:
  gamgi_mesa_translate_window_list (shift);
  break;

  case GAMGI_ENGINE_GAMGI:
  gamgi_mesa_translate_gamgi_list (shift);
  break;

  default:
  return;
  }

/****************************
 * redraw all OpenGL images *
 ****************************/

gamgi_mesa_draw_gamgi ();
}

void gamgi_mesa_translate (gamgi_window *window,
gamgi_object *object, double *shift)
{
/****************************************************************
 * translate list of objects, including the Gamgi global object *
 ****************************************************************/

if (gamgi->focus != FALSE)
  { gamgi_mesa_translate_list (shift); return; }
  
/***********************************************
 * translate object with focus, which can be a *
 * window, a layer or any other local object   *
 ***********************************************/

switch (object->class)
  {
  /******************************************
   * translate all layers in current window *
   ******************************************/

  case GAMGI_ENGINE_WINDOW:
  if (window->axes == FALSE)
    gamgi_mesa_translate_window_in (window, shift);
  else
    gamgi_mesa_translate_window_out (window, shift);
  break;

  /*********************************************
   * translate current layer in current window *
   *********************************************/

  case GAMGI_ENGINE_LAYER:
  if (window->axes == FALSE)
    gamgi_mesa_translate_layer_in (window->layer, object, shift);
  else
    gamgi_mesa_translate_layer_out (window->layer, shift);
  break;

  /*********************************************
   * translate current object in current layer *
   *********************************************/

  default:
  gamgi_mesa_translate_layer_in (window->layer, object, shift);
  break;
  }
  
/***********************
 * redraw OpenGL image *
 ***********************/

gtk_widget_queue_draw (window->area);
}
