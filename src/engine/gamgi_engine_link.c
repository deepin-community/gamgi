/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_link.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_engine_list.h"

void gamgi_engine_link_text_text (gamgi_text *child, 
gamgi_text *parent)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (parent->text_end);
parent->text_end = dlist;
if (dlist->before == NULL) parent->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = child;
child->object.dlist = dlist;
child->object.object = GAMGI_CAST_OBJECT parent;
}

void gamgi_engine_link_text_orbital (gamgi_text *text, gamgi_orbital *orbital)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (orbital->text_end);
orbital->text_end = dlist;
if (dlist->before == NULL) orbital->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT orbital;
}

void gamgi_engine_link_text_bond (gamgi_text *text, 
gamgi_bond *bond)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (bond->text_end);
bond->text_end = dlist;
if (dlist->before == NULL) bond->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT bond;
}

void gamgi_engine_link_text_atom (gamgi_text *text, 
gamgi_atom *atom)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (atom->text_end);
atom->text_end = dlist;
if (dlist->before == NULL) atom->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT atom;
}

void gamgi_engine_link_text_direction (gamgi_text *text, 
gamgi_direction *direction)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (direction->text_end);
direction->text_end = dlist;
if (dlist->before == NULL) direction->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT direction;
}

void gamgi_engine_link_text_plane (gamgi_text *text, 
gamgi_plane *plane)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (plane->text_end);
plane->text_end = dlist;
if (dlist->before == NULL) plane->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT plane;
}

void gamgi_engine_link_text_group (gamgi_text *text, 
gamgi_group *group)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (group->text_end);
group->text_end = dlist;
if (dlist->before == NULL) group->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT group;
}

void gamgi_engine_link_text_molecule (gamgi_text *text, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->text_end);
molecule->text_end = dlist;
if (dlist->before == NULL) molecule->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_text_cluster (gamgi_text *text, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->text_end);
cluster->text_end = dlist;
if (dlist->before == NULL) cluster->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_text_cell (gamgi_text *text, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->text_end);
cell->text_end = dlist;
if (dlist->before == NULL) cell->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_text_arrow (gamgi_text *text, 
gamgi_arrow *arrow)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (arrow->text_end);
arrow->text_end = dlist;
if (dlist->before == NULL) arrow->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT arrow;
}

void gamgi_engine_link_text_shape (gamgi_text *text, 
gamgi_shape *shape)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (shape->text_end);
shape->text_end = dlist;
if (dlist->before == NULL) shape->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT shape;
}

void gamgi_engine_link_text_graph (gamgi_text *text, 
gamgi_graph *graph)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (graph->text_end);
graph->text_end = dlist;
if (dlist->before == NULL) graph->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT graph;
}

void gamgi_engine_link_text_assembly (gamgi_text *text, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->text_end);
assembly->text_end = dlist;
if (dlist->before == NULL) assembly->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_text_layer (gamgi_text *text, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->text_end);
layer->text_end = dlist;
if (dlist->before == NULL) layer->text_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = text;
text->object.dlist = dlist;
text->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_orbital_bond (gamgi_orbital *orbital, 
gamgi_bond *bond)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (bond->orbital_end);
bond->orbital_end = dlist;
if (dlist->before == NULL) bond->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT bond;
}

void gamgi_engine_link_orbital_atom (gamgi_orbital *orbital, 
gamgi_atom *atom)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (atom->orbital_end);
atom->orbital_end = dlist;
if (dlist->before == NULL) atom->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT atom;
}

void gamgi_engine_link_orbital_direction (gamgi_orbital *orbital, 
gamgi_direction *direction)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (direction->orbital_end);
direction->orbital_end = dlist;
if (dlist->before == NULL) direction->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT direction;
}

void gamgi_engine_link_orbital_plane (gamgi_orbital *orbital, 
gamgi_plane *plane)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (plane->orbital_end);
plane->orbital_end = dlist;
if (dlist->before == NULL) plane->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT plane;
}

void gamgi_engine_link_orbital_group (gamgi_orbital *orbital, 
gamgi_group *group)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (group->orbital_end);
group->orbital_end = dlist;
if (dlist->before == NULL) group->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT group;
}

void gamgi_engine_link_orbital_molecule (gamgi_orbital *orbital, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->orbital_end);
molecule->orbital_end = dlist;
if (dlist->before == NULL) molecule->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_orbital_cluster (gamgi_orbital *orbital, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->orbital_end);
cluster->orbital_end = dlist;
if (dlist->before == NULL) cluster->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_orbital_cell (gamgi_orbital *orbital, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->orbital_end);
cell->orbital_end = dlist;
if (dlist->before == NULL) cell->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_orbital_assembly (gamgi_orbital *orbital, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->orbital_end);
assembly->orbital_end = dlist;
if (dlist->before == NULL) assembly->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_orbital_layer (gamgi_orbital *orbital, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->orbital_end);
layer->orbital_end = dlist;
if (dlist->before == NULL) layer->orbital_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = orbital;
orbital->object.dlist = dlist;
orbital->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_bond_atom (gamgi_bond *bond, gamgi_atom *atom)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (atom->bond_end);
atom->bond_end = dlist;
if (dlist->before == NULL) atom->bond_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data =  bond;
if (bond->atom1 == NULL)
  {
  bond->atom1 = atom;
  bond->dlist1 = dlist;
  }
else
  {
  bond->atom2 = atom;
  bond->dlist2 = dlist;
  }
}

void gamgi_engine_link_atom_direction (gamgi_atom *atom, 
gamgi_direction *direction)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (direction->atom_end);
direction->atom_end = dlist;
if (dlist->before == NULL) direction->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT direction;
}

void gamgi_engine_link_atom_plane (gamgi_atom *atom, 
gamgi_plane *plane)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (plane->atom_end);
plane->atom_end = dlist;
if (dlist->before == NULL) plane->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT plane;
}

void gamgi_engine_link_atom_group (gamgi_atom *atom, 
gamgi_group *group)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (group->atom_end);
group->atom_end = dlist;
if (dlist->before == NULL) group->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT group;
}

void gamgi_engine_link_atom_molecule (gamgi_atom *atom, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->atom_end);
molecule->atom_end = dlist;
if (dlist->before == NULL) molecule->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_atom_cluster (gamgi_atom *atom, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->atom_end);
cluster->atom_end = dlist;
if (dlist->before == NULL) cluster->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_atom_cell (gamgi_atom *atom, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->atom_end);
cell->atom_end = dlist;
if (dlist->before == NULL) cell->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_atom_assembly (gamgi_atom *atom, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->atom_end);
assembly->atom_end = dlist;
if (dlist->before == NULL) assembly->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_atom_layer (gamgi_atom *atom, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->atom_end);
layer->atom_end = dlist;
if (dlist->before == NULL) layer->atom_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = atom;
atom->object.dlist = dlist;
atom->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_direction_plane (gamgi_direction *direction,
gamgi_plane *plane)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (plane->direction_end);
plane->direction_end = dlist;
if (dlist->before == NULL) plane->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT plane;
}

void gamgi_engine_link_direction_group (gamgi_direction *direction, 
gamgi_group *group)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (group->direction_end);
group->direction_end = dlist;
if (dlist->before == NULL) group->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT group;
}

void gamgi_engine_link_direction_molecule (gamgi_direction *direction, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->direction_end);
molecule->direction_end = dlist;
if (dlist->before == NULL) molecule->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_direction_cluster (gamgi_direction *direction, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->direction_end);
cluster->direction_end = dlist;
if (dlist->before == NULL) cluster->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_direction_cell (gamgi_direction *direction, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->direction_end);
cell->direction_end = dlist;
if (dlist->before == NULL) cell->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_direction_assembly (gamgi_direction *direction, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->direction_end);
assembly->direction_end = dlist;
if (dlist->before == NULL) assembly->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_direction_layer (gamgi_direction *direction, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->direction_end);
layer->direction_end = dlist;
if (dlist->before == NULL) layer->direction_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = direction;
direction->object.dlist = dlist;
direction->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_plane_group (gamgi_plane *plane, 
gamgi_group *group)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (group->plane_end);
group->plane_end = dlist;
if (dlist->before == NULL) group->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT group;
}

void gamgi_engine_link_plane_molecule (gamgi_plane *plane, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->plane_end);
molecule->plane_end = dlist;
if (dlist->before == NULL) molecule->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_plane_cluster (gamgi_plane *plane, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->plane_end);
cluster->plane_end = dlist;
if (dlist->before == NULL) cluster->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_plane_cell (gamgi_plane *plane, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->plane_end);
cell->plane_end = dlist;
if (dlist->before == NULL) cell->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_plane_assembly (gamgi_plane *plane, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->plane_end);
assembly->plane_end = dlist;
if (dlist->before == NULL) assembly->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_plane_layer (gamgi_plane *plane, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->plane_end);
layer->plane_end = dlist;
if (dlist->before == NULL) layer->plane_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = plane;
plane->object.dlist = dlist;
plane->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_group_group (gamgi_group *child, 
gamgi_group *parent)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (parent->group_end);
parent->group_end = dlist;
if (dlist->before == NULL) parent->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = child;
child->object.dlist = dlist;
child->object.object = GAMGI_CAST_OBJECT parent;
}

void gamgi_engine_link_group_molecule (gamgi_group *group, 
gamgi_molecule *molecule)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (molecule->group_end);
molecule->group_end = dlist;
if (dlist->before == NULL) molecule->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = group;
group->object.dlist = dlist;
group->object.object = GAMGI_CAST_OBJECT molecule;
}

void gamgi_engine_link_group_cluster (gamgi_group *group, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->group_end);
cluster->group_end = dlist;
if (dlist->before == NULL) cluster->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = group;
group->object.dlist = dlist;
group->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_group_cell (gamgi_group *group, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->group_end);
cell->group_end = dlist;
if (dlist->before == NULL) cell->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = group;
group->object.dlist = dlist;
group->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_group_assembly (gamgi_group *group, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->group_end);
assembly->group_end = dlist;
if (dlist->before == NULL) assembly->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = group;
group->object.dlist = dlist;
group->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_group_layer (gamgi_group *group, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->group_end);
layer->group_end = dlist;
if (dlist->before == NULL) layer->group_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = group;
group->object.dlist = dlist;
group->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_molecule_cluster (gamgi_molecule *molecule, 
gamgi_cluster *cluster)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cluster->molecule_end);
cluster->molecule_end = dlist;
if (dlist->before == NULL) cluster->molecule_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = molecule;
molecule->object.dlist = dlist;
molecule->object.object = GAMGI_CAST_OBJECT cluster;
}

void gamgi_engine_link_molecule_cell (gamgi_molecule *molecule, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->molecule_end);
cell->molecule_end = dlist;
if (dlist->before == NULL) cell->molecule_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = molecule;
molecule->object.dlist = dlist;
molecule->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_molecule_assembly (gamgi_molecule *molecule, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->molecule_end);
assembly->molecule_end = dlist;
if (dlist->before == NULL) assembly->molecule_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = molecule;
molecule->object.dlist = dlist;
molecule->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_molecule_layer (gamgi_molecule *molecule, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->molecule_end);
layer->molecule_end = dlist;
if (dlist->before == NULL) layer->molecule_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = molecule;
molecule->object.dlist = dlist;
molecule->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_cluster_cluster (gamgi_cluster *child, 
gamgi_cluster *parent)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (parent->cluster_end);
parent->cluster_end = dlist;
if (dlist->before == NULL) parent->cluster_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = child;
child->object.dlist = dlist;
child->object.object = GAMGI_CAST_OBJECT parent;
}

void gamgi_engine_link_cluster_cell (gamgi_cluster *cluster, 
gamgi_cell *cell)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (cell->cluster_end);
cell->cluster_end = dlist;
if (dlist->before == NULL) cell->cluster_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = cluster;
cluster->object.dlist = dlist;
cluster->object.object = GAMGI_CAST_OBJECT cell;
}

void gamgi_engine_link_cluster_assembly (gamgi_cluster *cluster, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->cluster_end);
assembly->cluster_end = dlist;
if (dlist->before == NULL) assembly->cluster_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = cluster;
cluster->object.dlist = dlist;
cluster->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_cluster_layer (gamgi_cluster *cluster, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->cluster_end);
layer->cluster_end = dlist;
if (dlist->before == NULL) layer->cluster_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = cluster;
cluster->object.dlist = dlist;
cluster->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_cell_assembly (gamgi_cell *cell, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->cell_end);
assembly->cell_end = dlist;
if (dlist->before == NULL) assembly->cell_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = cell;
cell->object.dlist = dlist;
cell->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_cell_layer (gamgi_cell *cell, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->cell_end);
layer->cell_end = dlist;
if (dlist->before == NULL) layer->cell_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = cell;
cell->object.dlist = dlist;
cell->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_arrow_assembly (gamgi_arrow *arrow, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->arrow_end);
assembly->arrow_end = dlist;
if (dlist->before == NULL) assembly->arrow_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = arrow;
arrow->object.dlist = dlist;
arrow->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_arrow_layer (gamgi_arrow *arrow, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->arrow_end);
layer->arrow_end = dlist;
if (dlist->before == NULL) layer->arrow_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = arrow;
arrow->object.dlist = dlist;
arrow->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_shape_assembly (gamgi_shape *shape, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->shape_end);
assembly->shape_end = dlist;
if (dlist->before == NULL) assembly->shape_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = shape;
shape->object.dlist = dlist;
shape->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_shape_layer (gamgi_shape *shape, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->shape_end);
layer->shape_end = dlist;
if (dlist->before == NULL) layer->shape_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = shape;
shape->object.dlist = dlist;
shape->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_graph_assembly (gamgi_graph *graph, 
gamgi_assembly *assembly)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (assembly->graph_end);
assembly->graph_end = dlist;
if (dlist->before == NULL) assembly->graph_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = graph;
graph->object.dlist = dlist;
graph->object.object = GAMGI_CAST_OBJECT assembly;
}

void gamgi_engine_link_graph_layer (gamgi_graph *graph, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->graph_end);
layer->graph_end = dlist;
if (dlist->before == NULL) layer->graph_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = graph;
graph->object.dlist = dlist;
graph->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_assembly_assembly (gamgi_assembly *child, 
gamgi_assembly *parent)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (parent->assembly_end);
parent->assembly_end = dlist;
if (dlist->before == NULL) parent->assembly_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = child;
child->object.dlist = dlist;
child->object.object = GAMGI_CAST_OBJECT parent;
}

void gamgi_engine_link_assembly_layer (gamgi_assembly *assembly, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->assembly_end);
layer->assembly_end = dlist;
if (dlist->before == NULL) layer->assembly_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = assembly;
assembly->object.dlist = dlist;
assembly->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_light_layer (gamgi_light *light, 
gamgi_layer *layer)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (layer->light_end);
layer->light_end = dlist;
if (dlist->before == NULL) layer->light_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = light;
light->object.dlist = dlist;
light->object.object = GAMGI_CAST_OBJECT layer;
}

void gamgi_engine_link_layer_window (gamgi_layer *layer, 
gamgi_window *window)
{
gamgi_dlist *dlist;

/*********************************
 * add new member to parent list *
 *********************************/

dlist = gamgi_engine_dlist_add_end (window->layer_end);
window->layer_end = dlist;
if (dlist->before == NULL) window->layer_start = dlist;

/**********************************************
 * cross information between parent and child *
 **********************************************/

dlist->data = layer;
layer->object.dlist = dlist;
layer->object.object = GAMGI_CAST_OBJECT window;
}

void gamgi_engine_link_object_bond (gamgi_object *object, gamgi_bond *bond)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_bond (GAMGI_CAST_TEXT object, bond);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_bond (GAMGI_CAST_ORBITAL object, bond);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_atom (gamgi_object *object, gamgi_atom *atom)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_atom (GAMGI_CAST_TEXT object, atom);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_atom (GAMGI_CAST_ORBITAL object, atom);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_engine_link_bond_atom (GAMGI_CAST_BOND object, atom);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_direction (gamgi_object *object, gamgi_direction *direction)
{
/***********************************
 * bonds have no direction parents *
 ***********************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_direction (GAMGI_CAST_TEXT object, direction);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_direction (GAMGI_CAST_ORBITAL object, direction);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_direction (GAMGI_CAST_ATOM object, direction);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_plane (gamgi_object *object, gamgi_plane *plane)
{
/*******************************
 * bonds have no plane parents *
 *******************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_plane (GAMGI_CAST_TEXT object, plane);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_plane (GAMGI_CAST_ORBITAL object, plane);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_plane (GAMGI_CAST_ATOM object, plane);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_plane (GAMGI_CAST_DIRECTION object, plane);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_group (gamgi_object *object, gamgi_group *group)
{
/*******************************
 * bonds have no group parents *
 *******************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_group (GAMGI_CAST_TEXT object, group);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_group (GAMGI_CAST_ORBITAL object, group);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_group (GAMGI_CAST_ATOM object, group);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_group (GAMGI_CAST_DIRECTION object, group);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_group (GAMGI_CAST_PLANE object, group);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_group (GAMGI_CAST_GROUP object, group);
  break;

  default:
  break;
  }

}
  
void gamgi_engine_link_object_molecule (gamgi_object *object, gamgi_molecule *molecule)
{
/**********************************
 * bonds have no molecule parents *
 **********************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_molecule (GAMGI_CAST_TEXT object, molecule);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_molecule (GAMGI_CAST_ORBITAL object, molecule);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_molecule (GAMGI_CAST_ATOM object, molecule);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_molecule (GAMGI_CAST_DIRECTION object, molecule);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_molecule (GAMGI_CAST_PLANE object, molecule);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_molecule (GAMGI_CAST_GROUP object, molecule);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_cluster (gamgi_object *object, gamgi_cluster *cluster)
{
/*********************************
 * bonds have no cluster parents *
 *********************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_cluster (GAMGI_CAST_TEXT object, cluster);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_cluster (GAMGI_CAST_ORBITAL object, cluster);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_cluster (GAMGI_CAST_ATOM object, cluster);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_cluster (GAMGI_CAST_DIRECTION object, cluster);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_cluster (GAMGI_CAST_PLANE object, cluster);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_cluster (GAMGI_CAST_GROUP object, cluster);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_link_molecule_cluster (GAMGI_CAST_MOLECULE object, cluster);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_link_cluster_cluster (GAMGI_CAST_CLUSTER object, cluster);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_cell (gamgi_object *object, gamgi_cell *cell)
{
/******************************
 * bonds have no cell parents *
 ******************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_cell (GAMGI_CAST_TEXT object, cell);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_cell (GAMGI_CAST_ORBITAL object, cell);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_cell (GAMGI_CAST_ATOM object, cell);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_cell (GAMGI_CAST_DIRECTION object, cell);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_cell (GAMGI_CAST_PLANE object, cell);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_cell (GAMGI_CAST_GROUP object, cell);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_link_molecule_cell (GAMGI_CAST_MOLECULE object, cell);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_link_cluster_cell (GAMGI_CAST_CLUSTER object, cell);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_assembly (gamgi_object *object, gamgi_assembly *assembly)
{
/**********************************
 * bonds have no assembly parents *
 **********************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_assembly (GAMGI_CAST_TEXT object, assembly);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_assembly (GAMGI_CAST_ORBITAL object, assembly);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_assembly (GAMGI_CAST_ATOM object, assembly);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_assembly (GAMGI_CAST_DIRECTION object, assembly);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_assembly (GAMGI_CAST_PLANE object, assembly);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_assembly (GAMGI_CAST_GROUP object, assembly);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_link_molecule_assembly (GAMGI_CAST_MOLECULE object, assembly);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_link_cluster_assembly (GAMGI_CAST_CLUSTER object, assembly);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_link_cell_assembly (GAMGI_CAST_CELL object, assembly);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_link_arrow_assembly (GAMGI_CAST_ARROW object, assembly);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_link_shape_assembly (GAMGI_CAST_SHAPE object, assembly);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_link_graph_assembly (GAMGI_CAST_GRAPH object, assembly);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_link_assembly_assembly (GAMGI_CAST_ASSEMBLY object, assembly);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_layer (gamgi_object *object, gamgi_layer *layer)
{
/*******************************
 * bonds have no layer parents *
 *******************************/

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_layer (GAMGI_CAST_TEXT object, layer);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_orbital_layer (GAMGI_CAST_ORBITAL object, layer);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_atom_layer (GAMGI_CAST_ATOM object, layer);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_direction_layer (GAMGI_CAST_DIRECTION object, layer);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_plane_layer (GAMGI_CAST_PLANE object, layer);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_group_layer (GAMGI_CAST_GROUP object, layer);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_link_molecule_layer (GAMGI_CAST_MOLECULE object, layer);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_link_cluster_layer (GAMGI_CAST_CLUSTER object, layer);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_link_cell_layer (GAMGI_CAST_CELL object, layer);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_link_arrow_layer (GAMGI_CAST_ARROW object, layer);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_link_shape_layer (GAMGI_CAST_SHAPE object, layer);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_link_graph_layer (GAMGI_CAST_GRAPH object, layer);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_link_assembly_layer (GAMGI_CAST_ASSEMBLY object, layer);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_engine_link_light_layer (GAMGI_CAST_LIGHT object, layer);
  break;

  default:
  break;
  }

}

void gamgi_engine_link_object_object (gamgi_object *child, gamgi_object *parent)
{
/**********************************************************
 * only text objects can have text parents                *
 * only text objects can have orbital parents             *
 * only text objects can have arrow, shape, graph parents *
 * no objects can have light parents                      *
 **********************************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_link_text_text (GAMGI_CAST_TEXT child, GAMGI_CAST_TEXT parent);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_link_text_orbital (GAMGI_CAST_TEXT child, GAMGI_CAST_ORBITAL parent);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_engine_link_object_bond (child, GAMGI_CAST_BOND parent);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_link_object_atom (child, GAMGI_CAST_ATOM parent);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_link_object_direction (child, GAMGI_CAST_DIRECTION parent);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_link_object_plane (child, GAMGI_CAST_PLANE parent);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_link_object_group (child, GAMGI_CAST_GROUP parent);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_link_object_molecule (child, GAMGI_CAST_MOLECULE parent);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_link_object_cluster (child, GAMGI_CAST_CLUSTER parent);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_link_object_cell (child, GAMGI_CAST_CELL parent);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_link_text_arrow (GAMGI_CAST_TEXT child, GAMGI_CAST_ARROW parent);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_link_text_shape (GAMGI_CAST_TEXT child, GAMGI_CAST_SHAPE parent);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_link_text_graph (GAMGI_CAST_TEXT child, GAMGI_CAST_GRAPH parent);
  break;
  
  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_link_object_assembly (child, GAMGI_CAST_ASSEMBLY parent);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_engine_link_object_layer (child, GAMGI_CAST_LAYER parent);
  break;

  case GAMGI_ENGINE_WINDOW:
  gamgi_engine_link_layer_window (GAMGI_CAST_LAYER child, GAMGI_CAST_WINDOW parent);
  break;
  
  default:
  break;
  }

} 
