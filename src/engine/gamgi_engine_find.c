/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_find.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

gamgi_bool gamgi_engine_find_object (gamgi_object *object,
gamgi_object *parent)
{
gamgi_bond *bond;
int class = parent->class;

/***********************************************************************
 * handle this case separately because the normal algorithm does not   *
 * work (object->object = NULL) and any object belongs to gamgi anyway *
 ***********************************************************************/

if (class == GAMGI_ENGINE_GAMGI) return TRUE;

/**************************************
 * when object is a bond, try to find *
 * one of the two atoms in parent     *
 **************************************/

do
  {
  if (object == parent) return TRUE;
  if (object->class == GAMGI_ENGINE_BOND)
    {
    bond = GAMGI_CAST_BOND object;
    if (gamgi_engine_find_object (GAMGI_CAST_OBJECT 
    bond->atom1, parent) == TRUE) return TRUE;
    if (gamgi_engine_find_object (GAMGI_CAST_OBJECT
    bond->atom2, parent) == TRUE) return TRUE;
    return FALSE;
    }
  else object = object->object;
  } while (object != NULL && object->class <= class);

return FALSE;
}

gamgi_layer *gamgi_engine_find_layer (gamgi_object *object)
{
if (object->class > GAMGI_ENGINE_LAYER) return NULL;

while (object->class < GAMGI_ENGINE_LAYER)
  {
  if (object->class == GAMGI_ENGINE_BOND)
    object = (GAMGI_CAST_BOND object)->atom1->object.object;
  else
    object = object->object;
  }

return GAMGI_CAST_LAYER object;
}

gamgi_window *gamgi_engine_find_window (gamgi_object *object)
{
gamgi_layer *layer = gamgi_engine_find_layer (object);

return GAMGI_CAST_WINDOW layer->object.object;
}

gamgi_object *gamgi_engine_find_root (gamgi_object *object1,
gamgi_object *object2, gamgi_object *scope)
{
gamgi_object *i, *j;
int class = scope->class;

for (i = object1; i != NULL && i->class <= class; i = i->object)
  {
  for (j = object2; j != NULL && j->class <= class; j = j->object)
    {
    if (i->class > j->class) continue;

    if (i->class < j->class) break;

    if (i == j)
      {
      /******************************
       * a common parent was found, *
       *  check if it is in scope   *
       ******************************/

      if (gamgi_engine_find_object (i, scope) == TRUE) return j;

      return NULL; 
      }
    }
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_text (unsigned int number, gamgi_text *text)
{
gamgi_text *new_text;
gamgi_dlist *dlist;

if (text->object.number == number) return text;

dlist = text->text_start;
while (dlist != NULL)
  {
  new_text = gamgi_engine_find_text_text (number, GAMGI_CAST_TEXT dlist->data);
  if (new_text != NULL) return new_text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_orbital (unsigned int number,
gamgi_orbital *orbital)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = orbital->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number, 
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_bond (unsigned int number,
gamgi_bond *bond)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = bond->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = bond->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_atom (unsigned int number,
gamgi_atom *atom)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = atom->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = atom->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = atom->bond_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_bond (number,
  GAMGI_CAST_BOND dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_direction (unsigned int number,
gamgi_direction *direction)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = direction->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = direction->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = direction->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_plane (unsigned int number,
gamgi_plane *plane)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = plane->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = plane->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = plane->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = plane->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_group (unsigned int number,
gamgi_group *group)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = group->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = group->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = group->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = group->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = molecule->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = molecule->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = molecule->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = cluster->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = cell->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_arrow (unsigned int number,
gamgi_arrow *arrow)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = arrow->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_shape (unsigned int number,
gamgi_shape *shape)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = shape->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_graph (unsigned int number,
gamgi_graph *graph)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = graph->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = assembly->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = assembly->arrow_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_arrow (number,
  GAMGI_CAST_ARROW dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = assembly->shape_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_shape (number,
  GAMGI_CAST_SHAPE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = assembly->graph_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_graph (number,
  GAMGI_CAST_GRAPH dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = layer->text_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_text (number,
  GAMGI_CAST_TEXT dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->orbital_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->atom_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->direction_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = layer->arrow_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_arrow (number,
  GAMGI_CAST_ARROW dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = layer->shape_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_shape (number,
  GAMGI_CAST_SHAPE dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }
  
dlist = layer->graph_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_graph (number,
  GAMGI_CAST_GRAPH dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  text = gamgi_engine_find_text_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_window (unsigned int number,
gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  text = gamgi_engine_find_text_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (text != NULL) return text;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_text *gamgi_engine_find_text_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_text *text;

window = gamgi->window_start;
while (window != NULL)
  {
  text = gamgi_engine_find_text_window (number, window);
  if (text != NULL) return text;
  window = window->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_orbital (unsigned int number,
gamgi_orbital *orbital)
{
if (orbital->object.number == number) return orbital;

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_bond (unsigned int number,
gamgi_bond *bond)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = bond->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_atom (unsigned int number,
gamgi_atom *atom)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = atom->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = atom->bond_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_bond (number,
  GAMGI_CAST_BOND dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_direction (unsigned int number,
gamgi_direction *direction)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = direction->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = direction->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_plane (unsigned int number,
gamgi_plane *plane)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = plane->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = plane->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = plane->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_group (unsigned int number,
gamgi_group *group)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = group->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = group->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = group->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = molecule->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = molecule->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = cluster->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = cell->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = assembly->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  orbital = gamgi_engine_find_orbital_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = layer->orbital_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_orbital (number,
  GAMGI_CAST_ORBITAL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->atom_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->direction_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  orbital = gamgi_engine_find_orbital_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  orbital = gamgi_engine_find_orbital_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_window (unsigned int number,
gamgi_window *window)
{
gamgi_orbital *orbital;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  orbital = gamgi_engine_find_orbital_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (orbital != NULL) return orbital;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_orbital *gamgi_engine_find_orbital_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_orbital *orbital;

window = gamgi->window_start;
while (window != NULL)
  {
  orbital = gamgi_engine_find_orbital_window (number, window);
  if (orbital != NULL) return orbital;
  window = window->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_atom (unsigned int number,
gamgi_atom *atom)
{
if (atom->object.number == number) return atom;

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_direction (unsigned int number,
gamgi_direction *direction)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = direction->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_plane (unsigned int number,
gamgi_plane *plane)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = plane->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = plane->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_group (unsigned int number,
gamgi_group *group)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = group->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = group->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = molecule->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = cluster->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = cell->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cell->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = assembly->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  atom = gamgi_engine_find_atom_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = layer->atom_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->direction_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  atom = gamgi_engine_find_atom_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  atom = gamgi_engine_find_atom_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_window (unsigned int number,
gamgi_window *window)
{
gamgi_atom *atom;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  atom = gamgi_engine_find_atom_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (atom != NULL) return atom;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_atom *gamgi_engine_find_atom_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_atom *atom;

window = gamgi->window_start;
while (window != NULL)
  {
  atom = gamgi_engine_find_atom_window (number, window);
  if (atom != NULL) return atom;
  window = window->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_atom (unsigned int number,
gamgi_atom *atom)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = atom->bond_start;
while (dlist != NULL)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  if (bond->object.number == number) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_direction (unsigned int number,
gamgi_direction *direction)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = direction->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_plane (unsigned int number,
gamgi_plane *plane)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = plane->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = plane->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_group (unsigned int number,
gamgi_group *group)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = group->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = group->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = molecule->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = cluster->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = cell->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cell->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = assembly->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  bond = gamgi_engine_find_bond_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = layer->atom_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_atom (number,
  GAMGI_CAST_ATOM dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->direction_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  bond = gamgi_engine_find_bond_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  bond = gamgi_engine_find_bond_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_window (unsigned int number,
gamgi_window *window)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  bond = gamgi_engine_find_bond_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (bond != NULL) return bond;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_bond *gamgi_engine_find_bond_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_bond *bond;

window = gamgi->window_start;
while (window != NULL)
  {
  bond = gamgi_engine_find_bond_window (number, window);
  if (bond != NULL) return bond;
  window = window->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_direction (unsigned int number,
gamgi_direction *direction)
{
if (direction->object.number == number) return direction;

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_plane (unsigned int number,
gamgi_plane *plane)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = plane->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_group (unsigned int number,
gamgi_group *group)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = group->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = cell->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  direction = gamgi_engine_find_direction_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = layer->direction_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_direction (number,
  GAMGI_CAST_DIRECTION dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  direction = gamgi_engine_find_direction_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  direction = gamgi_engine_find_direction_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_window (unsigned int number,
gamgi_window *window)
{
gamgi_direction *direction;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  direction = gamgi_engine_find_direction_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (direction != NULL) return direction;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_direction *gamgi_engine_find_direction_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_direction *direction;

window = gamgi->window_start;
while (window != NULL)
  {
  direction = gamgi_engine_find_direction_window (number, window);
  if (direction != NULL) return direction;
  window = window->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_plane (unsigned int number,
gamgi_plane *plane)
{
if (plane->object.number == number) return plane;

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_group (unsigned int number,
gamgi_group *group)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = group->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = group->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = molecule->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = cell->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  plane = gamgi_engine_find_plane_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = layer->plane_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_plane (number,
  GAMGI_CAST_PLANE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  plane = gamgi_engine_find_plane_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  plane = gamgi_engine_find_plane_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_window (unsigned int number,
gamgi_window *window)
{
gamgi_plane *plane;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  plane = gamgi_engine_find_plane_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (plane != NULL) return plane;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_plane *gamgi_engine_find_plane_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_plane *plane;

window = gamgi->window_start;
while (window != NULL)
  {
  plane = gamgi_engine_find_plane_window (number, window);
  if (plane != NULL) return plane;
  window = window->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_group (unsigned int number,
gamgi_group *group)
{
gamgi_group *new_group;
gamgi_dlist *dlist;

if (group->object.number == number) return group;

dlist = group->group_start;
while (dlist != NULL)
  {
  new_group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (new_group != NULL) return new_group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_molecule (unsigned int number,
gamgi_molecule *molecule)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = molecule->group_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = cluster->group_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = cell->group_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = assembly->group_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  group = gamgi_engine_find_group_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = layer->group_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_group (number,
  GAMGI_CAST_GROUP dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  group = gamgi_engine_find_group_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  group = gamgi_engine_find_group_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_window (unsigned int number,
gamgi_window *window)
{
gamgi_group *group;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  group = gamgi_engine_find_group_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (group != NULL) return group;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_group *gamgi_engine_find_group_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_group *group;

window = gamgi->window_start;
while (window != NULL)
  {
  group = gamgi_engine_find_group_window (number, window);
  if (group != NULL) return group;
  window = window->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_molecule (unsigned int number,
gamgi_molecule *molecule)
{
if (molecule->object.number == number) return molecule;

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_molecule *molecule;
gamgi_dlist *dlist;

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_molecule *molecule;
gamgi_dlist *dlist;

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_molecule *molecule;
gamgi_dlist *dlist;

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  molecule = gamgi_engine_find_molecule_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_molecule *molecule;
gamgi_dlist *dlist;

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_molecule (number,
  GAMGI_CAST_MOLECULE dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  molecule = gamgi_engine_find_molecule_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  molecule = gamgi_engine_find_molecule_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_window (unsigned int number,
gamgi_window *window)
{
gamgi_molecule *molecule;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  molecule = gamgi_engine_find_molecule_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (molecule != NULL) return molecule;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_molecule *gamgi_engine_find_molecule_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_molecule *molecule;

window = gamgi->window_start;
while (window != NULL)
  {
  molecule = gamgi_engine_find_molecule_window (number, window);
  if (molecule != NULL) return molecule;
  window = window->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_cluster (unsigned int number,
gamgi_cluster *cluster)
{
gamgi_cluster *cluster_child;
gamgi_dlist *dlist;

if (cluster->object.number == number) return cluster;

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  cluster_child = gamgi_engine_find_cluster_cluster (number, 
  GAMGI_CAST_CLUSTER dlist->data);
  if (cluster_child != NULL) return cluster_child;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_cell (unsigned int number,
gamgi_cell *cell)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  cluster = gamgi_engine_find_cluster_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  cluster = gamgi_engine_find_cluster_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  cluster = gamgi_engine_find_cluster_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  cluster = gamgi_engine_find_cluster_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  cluster = gamgi_engine_find_cluster_cluster (number,
  GAMGI_CAST_CLUSTER dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  cluster = gamgi_engine_find_cluster_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  cluster = gamgi_engine_find_cluster_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_window (unsigned int number,
gamgi_window *window)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  cluster = gamgi_engine_find_cluster_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (cluster != NULL) return cluster;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cluster *gamgi_engine_find_cluster_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_cluster *cluster;

window = gamgi->window_start;
while (window != NULL)
  {
  cluster = gamgi_engine_find_cluster_window (number, window);
  if (cluster != NULL) return cluster;
  window = window->next;
  }

return NULL;
}

gamgi_cell *gamgi_engine_find_cell_cell (unsigned int number,
gamgi_cell *cell)
{
if (cell->object.number == number) return cell;

return NULL;
}

gamgi_cell *gamgi_engine_find_cell_assembly (unsigned int number,
gamgi_assembly *assembly)
{
gamgi_cell *cell;
gamgi_dlist *dlist;

dlist = assembly->cell_start;
while (dlist != NULL) 
  {
  cell = gamgi_engine_find_cell_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (cell != NULL) return cell;
  dlist = dlist->next;
  }
  
dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  cell = gamgi_engine_find_cell_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (cell != NULL) return cell;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cell *gamgi_engine_find_cell_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_cell *cell;
gamgi_dlist *dlist;

dlist = layer->cell_start;
while (dlist != NULL) 
  {
  cell = gamgi_engine_find_cell_cell (number,
  GAMGI_CAST_CELL dlist->data);
  if (cell != NULL) return cell;
  dlist = dlist->next;
  }
  
dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  cell = gamgi_engine_find_cell_assembly (number,
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (cell != NULL) return cell;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cell *gamgi_engine_find_cell_window (unsigned int number,
gamgi_window *window)
{
gamgi_cell *cell;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  cell = gamgi_engine_find_cell_layer (number,
  GAMGI_CAST_LAYER dlist->data);
  if (cell != NULL) return cell;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_cell *gamgi_engine_find_cell_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_cell *cell;

window = gamgi->window_start;
while (window != NULL)
  {
  cell = gamgi_engine_find_cell_window (number, window);
  if (cell != NULL) return cell;
  window = window->next;
  }

return NULL;
}

gamgi_arrow *gamgi_engine_find_arrow_arrow (unsigned int number,
gamgi_arrow *arrow)
{
if (arrow->object.number == number) return arrow;

return NULL;
}

gamgi_arrow *gamgi_engine_find_arrow_assembly (unsigned int number, 
gamgi_assembly *assembly)
{
gamgi_arrow *arrow;
gamgi_dlist *dlist;

dlist = assembly->arrow_start;
while (dlist != NULL)
  {
  arrow = gamgi_engine_find_arrow_arrow (number, 
  GAMGI_CAST_ARROW dlist->data);
  if (arrow != NULL) return arrow;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_arrow *gamgi_engine_find_arrow_layer (unsigned int number, 
gamgi_layer *layer)
{
gamgi_arrow *arrow;
gamgi_dlist *dlist;

dlist = layer->arrow_start;
while (dlist != NULL)
  {
  arrow = gamgi_engine_find_arrow_arrow (number, 
  GAMGI_CAST_ARROW dlist->data);
  if (arrow != NULL) return arrow;
  dlist = dlist->next;
  }

dlist = layer->assembly_start;
while (dlist != NULL)
  {
  arrow = gamgi_engine_find_arrow_assembly (number, 
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (arrow != NULL) return arrow;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_arrow *gamgi_engine_find_arrow_window (unsigned int number, 
gamgi_window *window)
{
gamgi_arrow *arrow;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  arrow = gamgi_engine_find_arrow_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (arrow != NULL) return arrow;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_arrow *gamgi_engine_find_arrow_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_arrow *arrow;

window = gamgi->window_start;
while (window != NULL)
  {
  arrow = gamgi_engine_find_arrow_window (number, window);
  if (arrow != NULL) return arrow;
  window = window->next;
  }

return NULL;
}

gamgi_shape *gamgi_engine_find_shape_shape (unsigned int number, 
gamgi_shape *shape)
{
if (shape->object.number == number) return shape;

return NULL;
}

gamgi_shape *gamgi_engine_find_shape_assembly (unsigned int number, 
gamgi_assembly *assembly)
{
gamgi_shape *shape;
gamgi_dlist *dlist;

dlist = assembly->shape_start;
while (dlist != NULL)
  {
  shape = gamgi_engine_find_shape_shape (number, 
  GAMGI_CAST_SHAPE dlist->data);
  if (shape != NULL) return shape;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_shape *gamgi_engine_find_shape_layer (unsigned int number, 
gamgi_layer *layer)
{
gamgi_shape *shape;
gamgi_dlist *dlist;

dlist = layer->shape_start;
while (dlist != NULL)
  {
  shape = gamgi_engine_find_shape_shape (number, 
  GAMGI_CAST_SHAPE dlist->data);
  if (shape != NULL) return shape;
  dlist = dlist->next;
  }

dlist = layer->assembly_start;
while (dlist != NULL)
  {
  shape = gamgi_engine_find_shape_assembly (number, 
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (shape != NULL) return shape;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_shape *gamgi_engine_find_shape_window (unsigned int number, 
gamgi_window *window)
{
gamgi_shape *shape;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  shape = gamgi_engine_find_shape_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (shape != NULL) return shape;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_shape *gamgi_engine_find_shape_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_shape *shape;

window = gamgi->window_start;
while (window != NULL)
  {
  shape = gamgi_engine_find_shape_window (number, window);
  if (shape != NULL) return shape;
  window = window->next;
  }

return NULL;
}

gamgi_graph *gamgi_engine_find_graph_graph (unsigned int number, 
gamgi_graph *graph)
{
if (graph->object.number == number) return graph;

return NULL;
}

gamgi_graph *gamgi_engine_find_graph_assembly (unsigned int number, 
gamgi_assembly *assembly)
{
gamgi_graph *graph;
gamgi_dlist *dlist;

dlist = assembly->graph_start;
while (dlist != NULL)
  {
  graph = gamgi_engine_find_graph_graph (number, 
  GAMGI_CAST_GRAPH dlist->data);
  if (graph != NULL) return graph;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_graph *gamgi_engine_find_graph_layer (unsigned int number, 
gamgi_layer *layer)
{
gamgi_graph *graph;
gamgi_dlist *dlist;

dlist = layer->graph_start;
while (dlist != NULL)
  {
  graph = gamgi_engine_find_graph_graph (number, 
  GAMGI_CAST_GRAPH dlist->data);
  if (graph != NULL) return graph;
  dlist = dlist->next;
  }

dlist = layer->assembly_start;
while (dlist != NULL)
  {
  graph = gamgi_engine_find_graph_assembly (number, 
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (graph != NULL) return graph;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_graph *gamgi_engine_find_graph_window (unsigned int number, 
gamgi_window *window)
{
gamgi_graph *graph;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  graph = gamgi_engine_find_graph_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (graph != NULL) return graph;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_graph *gamgi_engine_find_graph_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_graph *graph;

window = gamgi->window_start;
while (window != NULL)
  {
  graph = gamgi_engine_find_graph_window (number, window);
  if (graph != NULL) return graph;
  window = window->next;
  }

return NULL;
}

gamgi_assembly *gamgi_engine_find_assembly_assembly (unsigned int number, 
gamgi_assembly *assembly)
{
gamgi_assembly *assembly_child;
gamgi_dlist *dlist;

if (assembly->object.number == number) return assembly;

dlist = assembly->assembly_start;
while (dlist != NULL) 
  {
  assembly_child = gamgi_engine_find_assembly_assembly (number, 
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (assembly_child != NULL) return assembly_child;
  dlist = dlist->next;
  }
  
return NULL;
}

gamgi_assembly *gamgi_engine_find_assembly_layer (unsigned int number,
gamgi_layer *layer)
{
gamgi_assembly *assembly;
gamgi_dlist *dlist;

dlist = layer->assembly_start;
while (dlist != NULL) 
  {
  assembly = gamgi_engine_find_assembly_assembly (number, 
  GAMGI_CAST_ASSEMBLY dlist->data);
  if (assembly != NULL) return assembly;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_assembly *gamgi_engine_find_assembly_window (unsigned int number, 
gamgi_window *window)
{
gamgi_assembly *assembly;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  assembly = gamgi_engine_find_assembly_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (assembly != NULL) return assembly;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_assembly *gamgi_engine_find_assembly_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_assembly *assembly;

window = gamgi->window_start;
while (window != NULL)
  {
  assembly = gamgi_engine_find_assembly_window (number, window);
  if (assembly != NULL) return assembly;
  window = window->next;
  }

return NULL;
}

gamgi_light *gamgi_engine_find_light_light (unsigned int number, 
gamgi_light *light)
{
if (light->object.number == number) return light;

return NULL;
}

gamgi_light *gamgi_engine_find_light_layer (unsigned int number, 
gamgi_layer *layer)
{
gamgi_light *light;
gamgi_dlist *dlist;

dlist = layer->light_start;
while (dlist != NULL)
  {
  light = gamgi_engine_find_light_light (number, 
  GAMGI_CAST_LIGHT dlist->data);
  if (light != NULL) return light;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_light *gamgi_engine_find_light_window (unsigned int number, 
gamgi_window *window)
{
gamgi_light *light;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  light = gamgi_engine_find_light_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (light != NULL) return light;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_light *gamgi_engine_find_light_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_light *light;

window = gamgi->window_start;
while (window != NULL)
  {
  light = gamgi_engine_find_light_window (number, window);
  if (light != NULL) return light;
  window = window->next;
  }

return NULL;
}

gamgi_layer *gamgi_engine_find_layer_layer (unsigned int number, 
gamgi_layer *layer)
{
if (layer->object.number == number) return layer;

return NULL;
}

gamgi_layer *gamgi_engine_find_layer_window (unsigned int number, 
gamgi_window *window)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

dlist = window->layer_start;
while (dlist != NULL)
  {
  layer = gamgi_engine_find_layer_layer (number, 
  GAMGI_CAST_LAYER dlist->data);
  if (layer != NULL) return layer;
  dlist = dlist->next;
  }

return NULL;
}

gamgi_layer *gamgi_engine_find_layer_gamgi (unsigned int number)
{
gamgi_window *window;
gamgi_layer *layer;

window = gamgi->window_start;
while (window != NULL)
  {
  layer = gamgi_engine_find_layer_window (number, window);
  if (layer != NULL) return layer;
  window = window->next;
  }

return NULL;
}

gamgi_window *gamgi_engine_find_window_window (unsigned int number, 
gamgi_window *window)
{
if (window->object.number == number) return window;

return NULL;
}

gamgi_window *gamgi_engine_find_window_gamgi (unsigned int number)
{
gamgi_window *window, *new_window;

window = gamgi->window_start;
while (window != NULL)
  {
  new_window = gamgi_engine_find_window_window (number, window);
  if (new_window != NULL) return new_window;
  window = window->next;
  }

return NULL;
}
