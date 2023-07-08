/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_copy.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_copy_out.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_number.h"

static void static_start (gamgi_object *object)
{
object->task = 0;
object->selection = NULL;
object->object = NULL;
object->dlist = NULL;
}

gamgi_text *gamgi_engine_copy_text (gamgi_text *text)
{
gamgi_dlist *dlist;
gamgi_text *text_new, *text_child;

text_new = gamgi_engine_create_text ();
static_start (GAMGI_CAST_OBJECT text_new);
gamgi_engine_copy_out_text (text, text_new);
text_new->object.number = gamgi_engine_number_text_get ();

dlist = text->text_start;
while (dlist != NULL)
  { text_child = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_text (text_child, text_new);
    dlist = dlist->next; }

return text_new;
}

gamgi_orbital *gamgi_engine_copy_orbital (gamgi_orbital *orbital)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;

orbital_new = gamgi_engine_create_orbital ();
static_start (GAMGI_CAST_OBJECT orbital_new);
gamgi_engine_copy_out_orbital (orbital, orbital_new);
orbital_new->object.number = gamgi_engine_number_orbital_get ();

dlist = orbital->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_orbital (text_new, orbital_new);
    dlist = dlist->next; }

return orbital_new;
}

gamgi_bond *gamgi_engine_copy_bond (gamgi_bond *bond)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_bond *bond_new;

bond_new = gamgi_engine_create_bond ();
static_start (GAMGI_CAST_OBJECT bond_new);
gamgi_engine_copy_out_bond (bond, bond_new);
bond_new->object.number = gamgi_engine_number_bond_get ();

dlist = bond->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_bond (orbital_new, bond_new);
    dlist = dlist->next; }

dlist = bond->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_bond (text_new, bond_new);
    dlist = dlist->next; }

return bond_new;
}

gamgi_atom *gamgi_engine_copy_atom (gamgi_atom *atom)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_bond *bond, *bond_new;
gamgi_atom *atom_new;

atom_new = gamgi_engine_create_atom ();
static_start (GAMGI_CAST_OBJECT atom_new);
gamgi_engine_copy_out_atom (atom, atom_new);
atom_new->object.number = gamgi_engine_number_atom_get ();

dlist = atom->bond_start;
while (dlist != NULL)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  dlist = dlist->next;

  /**************************************
   * in case second link does not exist *
   *   this case should never occur!    *
   **************************************/

  if (bond->atom2 == NULL) continue;
 
  if (bond->copy == NULL) bond->copy = atom_new;
  else
    {
    bond_new = gamgi_engine_copy_bond (bond);
    gamgi_engine_link_bond_atom (bond_new, bond->copy);
    gamgi_engine_link_bond_atom (bond_new, atom_new);
    bond->copy = NULL;
    }
  }

dlist = atom->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_atom (orbital_new, atom_new);
    dlist = dlist->next; }

dlist = atom->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_atom (text_new, atom_new);
    dlist = dlist->next; }

return atom_new;
}

gamgi_direction *gamgi_engine_copy_direction (gamgi_direction *direction)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;

direction_new = gamgi_engine_create_direction ();
static_start (GAMGI_CAST_OBJECT direction_new);
gamgi_engine_copy_out_direction (direction, direction_new);
direction_new->object.number = gamgi_engine_number_direction_get ();

dlist = direction->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_direction (atom_new, direction_new);
    dlist = dlist->next; }

dlist = direction->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_direction (orbital_new, direction_new);
    dlist = dlist->next; }

dlist = direction->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_direction (text_new, direction_new);
    dlist = dlist->next; }

return direction_new;
}

gamgi_plane *gamgi_engine_copy_plane (gamgi_plane *plane)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;

plane_new = gamgi_engine_create_plane ();
static_start (GAMGI_CAST_OBJECT plane_new);
gamgi_engine_copy_out_plane (plane, plane_new);
plane_new->object.number = gamgi_engine_number_plane_get ();

dlist = plane->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_plane (direction_new, plane_new);
    dlist = dlist->next; }

dlist = plane->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_plane (atom_new, plane_new);
    dlist = dlist->next; }

dlist = plane->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_plane (orbital_new, plane_new);
    dlist = dlist->next; }

dlist = plane->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_plane (text_new, plane_new);
    dlist = dlist->next; }

return plane_new;
}

gamgi_group *gamgi_engine_copy_group (gamgi_group *group)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new, *group_child;

group_new = gamgi_engine_create_group (); 
static_start (GAMGI_CAST_OBJECT group_new);
gamgi_engine_copy_out_group (group, group_new);
group_new->object.number = gamgi_engine_number_group_get ();

dlist = group->group_start;
while (dlist != NULL)
  { group_child = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_group (group_child, group_new);
    dlist = dlist->next; }

dlist = group->plane_start;
while (dlist != NULL) 
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_group (plane_new, group_new);
    dlist = dlist->next; }

dlist = group->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_group (direction_new, group_new);
    dlist = dlist->next; }

dlist = group->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_group (atom_new, group_new);
    dlist = dlist->next; }

dlist = group->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_group (orbital_new, group_new);
    dlist = dlist->next; }

dlist = group->text_start;
while (dlist != NULL) 
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_group (text_new, group_new);
    dlist = dlist->next; }

return group_new;
}

gamgi_molecule *gamgi_engine_copy_molecule (gamgi_molecule *molecule)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new;
gamgi_molecule *molecule_new;

molecule_new = gamgi_engine_create_molecule (); 
static_start (GAMGI_CAST_OBJECT molecule_new);
gamgi_engine_copy_out_molecule (molecule, molecule_new);
molecule_new->object.number = gamgi_engine_number_molecule_get ();

dlist = molecule->group_start;
while (dlist != NULL)
  { group_new = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_molecule (group_new, molecule_new);
    dlist = dlist->next; }

dlist = molecule->plane_start;
while (dlist != NULL) 
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_molecule (plane_new, molecule_new);
    dlist = dlist->next; } 

dlist = molecule->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_molecule (direction_new, molecule_new);
    dlist = dlist->next; }

dlist = molecule->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_molecule (atom_new, molecule_new);
    dlist = dlist->next; }

dlist = molecule->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_molecule (orbital_new, molecule_new);
    dlist = dlist->next; }

dlist = molecule->text_start;
while (dlist != NULL) 
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_molecule (text_new, molecule_new);
    dlist = dlist->next; } 

return molecule_new;
}

gamgi_cluster *gamgi_engine_copy_cluster (gamgi_cluster *cluster)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new;
gamgi_molecule *molecule_new;
gamgi_cluster *cluster_new, *cluster_child;

cluster_new = gamgi_engine_create_cluster (); 
static_start (GAMGI_CAST_OBJECT cluster_new);
gamgi_engine_copy_out_cluster (cluster, cluster_new);
cluster_new->object.number = gamgi_engine_number_cluster_get ();

dlist = cluster->cluster_start;
while (dlist != NULL)
  { cluster_child = gamgi_engine_copy_cluster (GAMGI_CAST_CLUSTER dlist->data);
    gamgi_engine_link_cluster_cluster (cluster_child, cluster_new);
    dlist = dlist->next; }

dlist = cluster->molecule_start;
while (dlist != NULL)
  { molecule_new = gamgi_engine_copy_molecule (GAMGI_CAST_MOLECULE dlist->data);
    gamgi_engine_link_molecule_cluster (molecule_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->group_start;
while (dlist != NULL)
  { group_new = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_cluster (group_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->plane_start;
while (dlist != NULL) 
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_cluster (plane_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_cluster (direction_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_cluster (atom_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_cluster (orbital_new, cluster_new);
    dlist = dlist->next; }

dlist = cluster->text_start;
while (dlist != NULL) 
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_cluster (text_new, cluster_new);
    dlist = dlist->next; }

return cluster_new;
}

gamgi_cell *gamgi_engine_copy_cell (gamgi_cell *cell)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new;
gamgi_molecule *molecule_new;
gamgi_cluster *cluster_new;
gamgi_cell *cell_new;

cell_new = gamgi_engine_create_cell (); 
static_start (GAMGI_CAST_OBJECT cell_new);
gamgi_engine_copy_out_cell (cell, cell_new);
cell_new->object.number = gamgi_engine_number_cell_get ();

dlist = cell->cluster_start;
while (dlist != NULL)
  { cluster_new = gamgi_engine_copy_cluster (GAMGI_CAST_CLUSTER dlist->data);
    gamgi_engine_link_cluster_cell (cluster_new, cell_new);
    dlist = dlist->next; }

dlist = cell->molecule_start;
while (dlist != NULL)
  { molecule_new = gamgi_engine_copy_molecule (GAMGI_CAST_MOLECULE dlist->data);
    gamgi_engine_link_molecule_cell (molecule_new, cell_new);
    dlist = dlist->next; }

dlist = cell->group_start;
while (dlist != NULL)
  { group_new = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_cell (group_new, cell_new);
    dlist = dlist->next; }

dlist = cell->plane_start;
while (dlist != NULL)
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_cell (plane_new, cell_new);
    dlist = dlist->next; }

dlist = cell->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_cell (direction_new, cell_new);
    dlist = dlist->next; }

dlist = cell->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_cell (atom_new, cell_new);
    dlist = dlist->next; }

dlist = cell->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_cell (orbital_new, cell_new);
    dlist = dlist->next; }

dlist = cell->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_cell (text_new, cell_new);
    dlist = dlist->next; }

return cell_new;
}

gamgi_arrow *gamgi_engine_copy_arrow (gamgi_arrow *arrow)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_arrow *arrow_new;

arrow_new = gamgi_engine_create_arrow ();
static_start (GAMGI_CAST_OBJECT arrow_new);
gamgi_engine_copy_out_arrow (arrow, arrow_new);
arrow_new->object.number = gamgi_engine_number_arrow_get ();

dlist = arrow->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_arrow (text_new, arrow_new);
    dlist = dlist->next; }

return arrow_new;
}

gamgi_shape *gamgi_engine_copy_shape (gamgi_shape *shape)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_shape *shape_new;

shape_new = gamgi_engine_create_shape ();
static_start (GAMGI_CAST_OBJECT shape_new);
gamgi_engine_copy_out_shape (shape, shape_new);
shape_new->object.number = gamgi_engine_number_shape_get ();

dlist = shape->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_shape (text_new, shape_new);
    dlist = dlist->next; }

return shape_new;
}

gamgi_graph *gamgi_engine_copy_graph (gamgi_graph *graph)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_graph *graph_new;

graph_new = gamgi_engine_create_graph ();
static_start (GAMGI_CAST_OBJECT graph_new);
gamgi_engine_copy_out_graph (graph, graph_new);
graph_new->object.number = gamgi_engine_number_graph_get ();

dlist = graph->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_graph (text_new, graph_new);
    dlist = dlist->next; }

return graph_new;
}

gamgi_assembly *gamgi_engine_copy_assembly (gamgi_assembly *assembly)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new;
gamgi_molecule *molecule_new;
gamgi_cluster *cluster_new;
gamgi_cell *cell_new;
gamgi_arrow *arrow_new;
gamgi_shape *shape_new;
gamgi_graph *graph_new;
gamgi_assembly *assembly_new, *assembly_child;

assembly_new = gamgi_engine_create_assembly ();
static_start (GAMGI_CAST_OBJECT assembly_new);
gamgi_engine_copy_out_assembly (assembly, assembly_new);
assembly_new->object.number = gamgi_engine_number_assembly_get ();

dlist = assembly->assembly_start;
while (dlist != NULL)
  { assembly_child = gamgi_engine_copy_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    gamgi_engine_link_assembly_assembly (assembly_child, assembly_new);
    dlist = dlist->next; }

dlist = assembly->graph_start;
while (dlist != NULL)
  { graph_new = gamgi_engine_copy_graph (GAMGI_CAST_GRAPH dlist->data);
    gamgi_engine_link_graph_assembly (graph_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->shape_start;
while (dlist != NULL)
  { shape_new = gamgi_engine_copy_shape (GAMGI_CAST_SHAPE dlist->data);
    gamgi_engine_link_shape_assembly (shape_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->arrow_start;
while (dlist != NULL)
  { arrow_new = gamgi_engine_copy_arrow (GAMGI_CAST_ARROW dlist->data);
    gamgi_engine_link_arrow_assembly (arrow_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->cell_start;
while (dlist != NULL)
  { cell_new = gamgi_engine_copy_cell (GAMGI_CAST_CELL dlist->data);
    gamgi_engine_link_cell_assembly (cell_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->cluster_start;
while (dlist != NULL)
  { cluster_new = gamgi_engine_copy_cluster (GAMGI_CAST_CLUSTER dlist->data);
    gamgi_engine_link_cluster_assembly (cluster_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->molecule_start;
while (dlist != NULL)
  { molecule_new = gamgi_engine_copy_molecule (GAMGI_CAST_MOLECULE dlist->data);
    gamgi_engine_link_molecule_assembly (molecule_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->group_start;
while (dlist != NULL)
  { group_new = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_assembly (group_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->plane_start;
while (dlist != NULL)
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_assembly (plane_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_assembly (direction_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_assembly (atom_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_assembly (orbital_new, assembly_new);
    dlist = dlist->next; }

dlist = assembly->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_assembly (text_new, assembly_new);
    dlist = dlist->next; }

return assembly_new;
}

gamgi_light *gamgi_engine_copy_light (gamgi_light *light)
{
gamgi_light *light_new;

light_new = gamgi_engine_create_light ();
static_start (GAMGI_CAST_OBJECT light_new);
gamgi_engine_copy_out_light (light, light_new);
light_new->object.number = gamgi_engine_number_light_get ();

return light_new;
}

gamgi_layer *gamgi_engine_copy_layer (gamgi_layer *layer)
{
gamgi_dlist *dlist;
gamgi_text *text_new;
gamgi_orbital *orbital_new;
gamgi_atom *atom_new;
gamgi_direction *direction_new;
gamgi_plane *plane_new;
gamgi_group *group_new;
gamgi_molecule *molecule_new;
gamgi_cluster *cluster_new;
gamgi_cell *cell_new;
gamgi_arrow *arrow_new;
gamgi_shape *shape_new;
gamgi_graph *graph_new;
gamgi_assembly *assembly_new;
gamgi_light *light_new;
gamgi_layer *layer_new;

layer_new = gamgi_engine_create_layer ();
static_start (GAMGI_CAST_OBJECT layer_new);
gamgi_engine_copy_out_layer (layer, layer_new);
layer_new->object.number = gamgi_engine_number_layer_get ();

dlist = layer->light_start;
while (dlist != NULL)
  { light_new = gamgi_engine_copy_light (GAMGI_CAST_LIGHT dlist->data);
    gamgi_engine_link_light_layer (light_new, layer_new);
    dlist = dlist->next; }

dlist = layer->assembly_start;
while (dlist != NULL)
  { assembly_new = gamgi_engine_copy_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    gamgi_engine_link_assembly_layer (assembly_new, layer_new);
    dlist = dlist->next; }

dlist = layer->graph_start;
while (dlist != NULL)
  { graph_new = gamgi_engine_copy_graph (GAMGI_CAST_GRAPH dlist->data);
    gamgi_engine_link_graph_layer (graph_new, layer_new);
    dlist = dlist->next; }

dlist = layer->shape_start;
while (dlist != NULL)
  { shape_new = gamgi_engine_copy_shape (GAMGI_CAST_SHAPE dlist->data);
    gamgi_engine_link_shape_layer (shape_new, layer_new);
    dlist = dlist->next; }

dlist = layer->arrow_start;
while (dlist != NULL)
  { arrow_new = gamgi_engine_copy_arrow (GAMGI_CAST_ARROW dlist->data);
    gamgi_engine_link_arrow_layer (arrow_new, layer_new);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { cell_new = gamgi_engine_copy_cell (GAMGI_CAST_CELL dlist->data);
    gamgi_engine_link_cell_layer (cell_new, layer_new);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { cluster_new = gamgi_engine_copy_cluster (GAMGI_CAST_CLUSTER dlist->data);
    gamgi_engine_link_cluster_layer (cluster_new, layer_new);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { molecule_new = gamgi_engine_copy_molecule (GAMGI_CAST_MOLECULE dlist->data);
    gamgi_engine_link_molecule_layer (molecule_new, layer_new);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { group_new = gamgi_engine_copy_group (GAMGI_CAST_GROUP dlist->data);
    gamgi_engine_link_group_layer (group_new, layer_new);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { plane_new = gamgi_engine_copy_plane (GAMGI_CAST_PLANE dlist->data);
    gamgi_engine_link_plane_layer (plane_new, layer_new);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { direction_new = gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION dlist->data);
    gamgi_engine_link_direction_layer (direction_new, layer_new);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { atom_new = gamgi_engine_copy_atom (GAMGI_CAST_ATOM dlist->data);
    gamgi_engine_link_atom_layer (atom_new, layer_new);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { orbital_new = gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL dlist->data);
    gamgi_engine_link_orbital_layer (orbital_new, layer_new);
    dlist = dlist->next; }

dlist = layer->text_start;
while (dlist != NULL)
  { text_new = gamgi_engine_copy_text (GAMGI_CAST_TEXT dlist->data);
    gamgi_engine_link_text_layer (text_new, layer_new);
    dlist = dlist->next; }

return layer_new;
}

gamgi_window *gamgi_engine_copy_window (gamgi_window *window)
{
gamgi_dlist *dlist;
gamgi_layer *layer;
gamgi_layer *layer_new;
gamgi_window *window_new;

window_new = gamgi_engine_create_window ();
static_start (GAMGI_CAST_OBJECT window_new);
gamgi_engine_copy_out_window (window, window_new);
window_new->object.number = gamgi_engine_number_window_get ();

dlist = window->layer_start;
while (dlist != NULL)
  { layer = GAMGI_CAST_LAYER dlist->data;
    layer_new = gamgi_engine_copy_layer (layer);
    gamgi_engine_link_layer_window (layer_new, window_new);
    if (window->layer == layer) window_new->layer = layer_new;
    dlist = dlist->next; }

return window_new;
}

/*************** external function ***********
 *                                           *
 *           GAMGI_ENGINE_COPY_OBJECT        *
 *                                           *
 *********************************************/

gamgi_object *gamgi_engine_copy_object (gamgi_object *object)
{
gamgi_object *object_new;

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_text (GAMGI_CAST_TEXT object);
  break;

  case GAMGI_ENGINE_ORBITAL:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_orbital (GAMGI_CAST_ORBITAL object);
  break;

  case GAMGI_ENGINE_BOND:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_bond (GAMGI_CAST_BOND object);
  break;

  case GAMGI_ENGINE_ATOM:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_atom (GAMGI_CAST_ATOM object);
  break;

  case GAMGI_ENGINE_DIRECTION:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_direction (GAMGI_CAST_DIRECTION object);
  break;

  case GAMGI_ENGINE_PLANE:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_plane (GAMGI_CAST_PLANE object);
  break;

  case GAMGI_ENGINE_GROUP:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_group (GAMGI_CAST_GROUP object);
  break;

  case GAMGI_ENGINE_MOLECULE:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_molecule (GAMGI_CAST_MOLECULE object);
  break;

  case GAMGI_ENGINE_CLUSTER:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_cluster (GAMGI_CAST_CLUSTER object);
  break;

  case GAMGI_ENGINE_CELL:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_cell (GAMGI_CAST_CELL object);
  break;

  case GAMGI_ENGINE_ARROW:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_arrow (GAMGI_CAST_ARROW object);
  break;

  case GAMGI_ENGINE_SHAPE:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_shape (GAMGI_CAST_SHAPE object);
  break;

  case GAMGI_ENGINE_GRAPH:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_graph (GAMGI_CAST_GRAPH object);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_assembly (GAMGI_CAST_ASSEMBLY object);
  break;

  case GAMGI_ENGINE_LIGHT:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_light (GAMGI_CAST_LIGHT object);
  break;

  case GAMGI_ENGINE_LAYER:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_layer (GAMGI_CAST_LAYER object);
  break;

  case GAMGI_ENGINE_WINDOW:
  object_new = GAMGI_CAST_OBJECT gamgi_engine_copy_window (GAMGI_CAST_WINDOW object);
  break;

  default:
  object_new = NULL;
  }

return object_new;
}
