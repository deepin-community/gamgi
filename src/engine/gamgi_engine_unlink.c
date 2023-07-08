/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_unlink.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_engine_list.h"

void gamgi_engine_unlink_text (gamgi_text *text)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_text *parent_text;
gamgi_orbital *parent_orbital;
gamgi_bond *parent_bond;
gamgi_atom *parent_atom;
gamgi_direction *parent_direction;
gamgi_plane *parent_plane;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_arrow *parent_arrow;
gamgi_shape *parent_shape;
gamgi_graph *parent_graph;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = text->object.object;
dlist = text->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_TEXT:
    parent_text = GAMGI_CAST_TEXT object;
    parent_text->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_ORBITAL:
    parent_orbital = GAMGI_CAST_ORBITAL object;
    parent_orbital->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_BOND:
    parent_bond = GAMGI_CAST_BOND object;
    parent_bond->text_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ATOM:
    parent_atom = GAMGI_CAST_ATOM object;
    parent_atom->text_end = dlist->before; 
    break;

    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->text_end = dlist->before; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->text_end = dlist->before; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_ARROW:
    parent_arrow = GAMGI_CAST_ARROW object;
    parent_arrow->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_SHAPE:
    parent_shape = GAMGI_CAST_SHAPE object;
    parent_shape->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_GRAPH:
    parent_graph = GAMGI_CAST_GRAPH object;
    parent_graph->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->text_end = dlist->before;
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->text_end = dlist->before;
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_TEXT:
    parent_text = GAMGI_CAST_TEXT object;
    parent_text->text_start = dlist->next;
    break;

    case GAMGI_ENGINE_ORBITAL:
    parent_orbital = GAMGI_CAST_ORBITAL object;
    parent_orbital->text_start = dlist->next;
     break;

    case GAMGI_ENGINE_BOND:
    parent_bond = GAMGI_CAST_BOND object;
    parent_bond->text_start = dlist->next;
    break;

    case GAMGI_ENGINE_ATOM:
    parent_atom = GAMGI_CAST_ATOM object;
    parent_atom->text_start = dlist->next;
    break;

    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ARROW:
    parent_arrow = GAMGI_CAST_ARROW object;
    parent_arrow->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_SHAPE:
    parent_shape = GAMGI_CAST_SHAPE object;
    parent_shape->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_GRAPH:
    parent_graph = GAMGI_CAST_GRAPH object;
    parent_graph->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->text_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->text_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
text->object.object = NULL;
text->object.dlist = NULL;

}

void gamgi_engine_unlink_orbital (gamgi_orbital *orbital)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_bond *parent_bond;
gamgi_atom *parent_atom;
gamgi_direction *parent_direction;
gamgi_plane *parent_plane;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = orbital->object.object;
dlist = orbital->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_BOND:
    parent_bond = GAMGI_CAST_BOND object;
    parent_bond->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ATOM:
    parent_atom = GAMGI_CAST_ATOM object;
    parent_atom->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->orbital_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->orbital_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_BOND:
    parent_bond = GAMGI_CAST_BOND object;
    parent_bond->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ATOM:
    parent_atom = GAMGI_CAST_ATOM object;
    parent_atom->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->orbital_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->orbital_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
orbital->object.object = NULL;
orbital->object.dlist = NULL;
}

void gamgi_engine_unlink_bond (gamgi_bond *bond)
{
gamgi_dlist *dlist1, *dlist2;
gamgi_atom *atom1, *atom2;

atom1 = bond->atom1;
atom2 = bond->atom2;
dlist1 = bond->dlist1;
dlist2 = bond->dlist2;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist1->next == NULL)
  atom1->bond_end = dlist1->before;

if (dlist2->next == NULL)
  atom2->bond_end = dlist2->before;

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist1->before == NULL)
  atom1->bond_start = dlist1->next;

if (dlist2->before == NULL)
  atom2->bond_start = dlist2->next;

/************************************
 * remove node from parents list    *
 * and reset child links to parents *
 ************************************/

gamgi_engine_dlist_remove_start (dlist1);
gamgi_engine_dlist_remove_start (dlist2);
bond->atom1 = NULL;
bond->atom2 = NULL;
bond->dlist1 = NULL;
bond->dlist2 = NULL;
}


void gamgi_engine_unlink_atom (gamgi_atom *atom)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_direction *parent_direction;
gamgi_plane *parent_plane;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = atom->object.object;
dlist = atom->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->atom_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->atom_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_DIRECTION:
    parent_direction = GAMGI_CAST_DIRECTION object;
    parent_direction->atom_start = dlist->next; 
    break;

    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->atom_start = dlist->next; 
    break;

    case GAMGI_ENGINE_GROUP: 
    parent_group = GAMGI_CAST_GROUP object; 
    parent_group->atom_start= dlist->next; 
    break;
    
    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->atom_start = dlist->next; 
    break;
  
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->atom_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->atom_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->atom_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->atom_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
atom->object.object = NULL;
atom->object.dlist = NULL;
}

void gamgi_engine_unlink_direction (gamgi_direction *direction)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_plane *parent_plane;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = direction->object.object;
dlist = direction->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->direction_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->direction_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_PLANE:
    parent_plane = GAMGI_CAST_PLANE object;
    parent_plane->direction_start = dlist->next; 
    break;

    case GAMGI_ENGINE_GROUP: 
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->direction_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->direction_start = dlist->next; 
    break;
  
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->direction_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->direction_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->direction_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->direction_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
direction->object.object = NULL;
direction->object.dlist = NULL;
}

void gamgi_engine_unlink_plane (gamgi_plane *plane)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = plane->object.object;
dlist = plane->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->plane_end = dlist->before; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->plane_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->plane_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->plane_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->plane_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->plane_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_GROUP: 
    parent_group = GAMGI_CAST_GROUP object; 
    parent_group->plane_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->plane_start = dlist->next; 
    break;
  
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->plane_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->plane_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->plane_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->plane_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
plane->object.object = NULL;
plane->object.dlist = NULL;
}

void gamgi_engine_unlink_group (gamgi_group *group)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_group *parent_group;
gamgi_molecule *parent_molecule;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = group->object.object;
dlist = group->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->group_end = dlist->before; 
    break;

    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->group_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->group_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->group_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->group_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->group_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_GROUP:
    parent_group = GAMGI_CAST_GROUP object;
    parent_group->group_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_MOLECULE:
    parent_molecule = GAMGI_CAST_MOLECULE object;
    parent_molecule->group_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->group_start = dlist->next; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->group_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->group_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->group_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
group->object.object = NULL;
group->object.dlist = NULL;
}

void gamgi_engine_unlink_molecule (gamgi_molecule *molecule)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = molecule->object.object;
dlist = molecule->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->molecule_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->molecule_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->molecule_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->molecule_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->molecule_start = dlist->next; 
    break;
  
    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->molecule_start = dlist->next; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->molecule_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->molecule_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
molecule->object.object = NULL;
molecule->object.dlist = NULL;
}

void gamgi_engine_unlink_cluster (gamgi_cluster *cluster)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_cluster *parent_cluster;
gamgi_cell *parent_cell;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = cluster->object.object;
dlist = cluster->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->cluster_end = dlist->before; 
    break;

    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->cluster_end = dlist->before; 
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->cluster_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->cluster_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_CLUSTER:
    parent_cluster = GAMGI_CAST_CLUSTER object;
    parent_cluster->cluster_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_CELL:
    parent_cell = GAMGI_CAST_CELL object;
    parent_cell->cluster_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->cluster_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->cluster_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
cluster->object.object = NULL;
cluster->object.dlist = NULL;
}

void gamgi_engine_unlink_cell (gamgi_cell *cell)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = cell->object.object;
dlist = cell->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->cell_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->cell_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->cell_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->cell_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
cell->object.object = NULL;
cell->object.dlist = NULL;
}

void gamgi_engine_unlink_arrow (gamgi_arrow *arrow)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = arrow->object.object;
dlist = arrow->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->arrow_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->arrow_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->arrow_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->arrow_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
arrow->object.object = NULL;
arrow->object.dlist = NULL;
}

void gamgi_engine_unlink_shape (gamgi_shape *shape)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = shape->object.object;
dlist = shape->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->shape_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->shape_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->shape_start = dlist->next; 
    break;
    
    case GAMGI_ENGINE_LAYER: 
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->shape_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
shape->object.object = NULL;
shape->object.dlist = NULL;
}

void gamgi_engine_unlink_graph (gamgi_graph *graph)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = graph->object.object;
dlist = graph->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->graph_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->graph_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  { 
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->graph_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->graph_start = dlist->next; 
    break; 
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
graph->object.object = NULL;
graph->object.dlist = NULL;
}

void gamgi_engine_unlink_assembly (gamgi_assembly *assembly)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_assembly *parent_assembly;
gamgi_layer *parent_layer;

object = assembly->object.object;
dlist = assembly->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->assembly_end = dlist->before; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->assembly_end = dlist->before; 
    break;
    }
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  switch (object->class)
    {
    case GAMGI_ENGINE_ASSEMBLY:
    parent_assembly = GAMGI_CAST_ASSEMBLY object;
    parent_assembly->assembly_start = dlist->next; 
    break;

    case GAMGI_ENGINE_LAYER:
    parent_layer = GAMGI_CAST_LAYER object;
    parent_layer->assembly_start = dlist->next; 
    break;
    }
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
assembly->object.object = NULL;
assembly->object.dlist = NULL;
}

void gamgi_engine_unlink_light (gamgi_light *light)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_layer *parent_layer;

object = light->object.object;
dlist = light->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  parent_layer = GAMGI_CAST_LAYER object;
  parent_layer->light_end = dlist->before;
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  parent_layer = GAMGI_CAST_LAYER object;
  parent_layer->light_start = dlist->next;
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
light->object.object = NULL;
light->object.dlist = NULL;
}

void gamgi_engine_unlink_layer (gamgi_layer *layer)
{
gamgi_dlist *dlist;
gamgi_object *object;
gamgi_window *parent_window;

object = layer->object.object;
dlist = layer->object.dlist;

/******************************************************
 *   If this is the last object in the parent list    *
 * then the queue end must point to the object before *
 ******************************************************/

if (dlist->next == NULL)
  {
  parent_window = GAMGI_CAST_WINDOW object;
  parent_window->layer_end = dlist->before;
  }

/******************************************************
 *   If this is the first object in the parent list   *
 * then the queue start must point to the object next *
 ******************************************************/

if (dlist->before == NULL)
  {
  parent_window = GAMGI_CAST_WINDOW object;
  parent_window->layer_start = dlist->next;
  }

/************************************
 * remove node from parent list and *
 *   reset child links to parent    *
 ************************************/

gamgi_engine_dlist_remove_start (dlist);
layer->object.object = NULL;
layer->object.dlist = NULL;
}

void gamgi_engine_unlink_object (gamgi_object *object)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_unlink_text (GAMGI_CAST_TEXT object);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_unlink_orbital (GAMGI_CAST_ORBITAL object);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_engine_unlink_bond (GAMGI_CAST_BOND object);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_unlink_atom (GAMGI_CAST_ATOM object);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_unlink_direction (GAMGI_CAST_DIRECTION object);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_unlink_plane (GAMGI_CAST_PLANE object);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_unlink_group (GAMGI_CAST_GROUP object);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_unlink_molecule (GAMGI_CAST_MOLECULE object);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_unlink_cluster (GAMGI_CAST_CLUSTER object);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_unlink_cell (GAMGI_CAST_CELL object);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_unlink_arrow (GAMGI_CAST_ARROW object);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_unlink_shape (GAMGI_CAST_SHAPE object);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_unlink_graph (GAMGI_CAST_GRAPH object);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_unlink_assembly (GAMGI_CAST_ASSEMBLY object);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_engine_unlink_light (GAMGI_CAST_LIGHT object);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_engine_unlink_layer (GAMGI_CAST_LAYER object);
  break;

  default:
  break;
  }

}
