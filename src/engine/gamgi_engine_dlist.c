/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_dlist.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"

gamgi_dlist *gamgi_engine_dlist_text_text (gamgi_text *text,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = text->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT text;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_orbital (gamgi_orbital *orbital,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = orbital->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_bond (gamgi_bond *bond,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = bond->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = bond->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_atom (gamgi_atom *atom,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = atom->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = atom->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = atom->bond_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_bond (GAMGI_CAST_BOND
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_direction (gamgi_direction *direction,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = direction->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = direction->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = direction->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = plane->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_arrow (gamgi_arrow *arrow,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = arrow->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_shape (gamgi_shape *shape,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = shape->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_graph (gamgi_graph *graph,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = graph->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->arrow_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_arrow (GAMGI_CAST_ARROW
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->shape_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_shape (GAMGI_CAST_SHAPE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->graph_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_graph (GAMGI_CAST_GRAPH
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->text_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_text (GAMGI_CAST_TEXT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->arrow_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_arrow (GAMGI_CAST_ARROW
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->shape_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_shape (GAMGI_CAST_SHAPE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->graph_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_graph (GAMGI_CAST_GRAPH
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_text_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_text_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_text_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_orbital (gamgi_orbital *orbital,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT orbital;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_bond (gamgi_bond *bond,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = bond->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_atom (gamgi_atom *atom,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = atom->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = atom->bond_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_bond (GAMGI_CAST_BOND
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_direction (gamgi_direction *direction,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = direction->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = direction->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = plane->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_orbital_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->orbital_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_orbital (GAMGI_CAST_ORBITAL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_orbital_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_orbital_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_orbital_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_orbital_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_bond (gamgi_bond *bond,
gamgi_dlist *dlist_start)
{
/***********************************************
 * Bonds are owned by two parent atoms, so     *
 * they should appear twice, when searching    *
 * for atoms child. Bonds are stack listed     *
 * immediately when an atom references them.   *
 * As last bonds come first in the list and    *
 * atoms are also generated from end to start, *
 * the overall results are:                    *
 *                                             *
 * a bond appears in the list when the second  *
 * atom appears (ordered from start to end).   *
 *                                             *
 * half bonds, that are be referenced only     *
 * by one atom, will always be listed, making  *
 * it easy to reset the bond->first flag,      *
 * that would be corrupted otherwise. This     *
 * is safe programming only: half bonds        *
 * should not really exist in GAMGI.           *
 ***********************************************/
   
if (bond->first == TRUE)
  {
  bond->first = FALSE;
  dlist_start = gamgi_engine_dlist_add_start (dlist_start);
  dlist_start->data = GAMGI_CAST_OBJECT bond;
  }
else bond->first = TRUE;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_atom (gamgi_atom *atom,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = atom->bond_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_bond (GAMGI_CAST_BOND
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_direction (gamgi_direction *direction,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = direction->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = plane->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_bond_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_bond_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_bond_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_bond_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_bond_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_atom (gamgi_atom *atom,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT atom;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_direction (gamgi_direction *direction,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = direction->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = plane->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = plane->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_atom_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->atom_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_atom_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_atom_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_atom_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_direction (gamgi_direction *direction,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT direction;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = plane->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_direction_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->direction_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_direction (GAMGI_CAST_DIRECTION
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_direction_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_direction_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_direction_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_direction_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_plane (gamgi_plane *plane,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT plane;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_plane_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->plane_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_plane (GAMGI_CAST_PLANE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_plane_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_plane_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_plane_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_plane_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_group (gamgi_group *group,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = group->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT group;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = molecule->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_group_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->group_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_group (GAMGI_CAST_GROUP
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_group_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_group_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_group_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_group_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_molecule (gamgi_molecule *molecule,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT molecule;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_molecule_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->molecule_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_molecule (GAMGI_CAST_MOLECULE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_molecule_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_molecule_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_molecule_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_molecule_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_cluster (gamgi_cluster *cluster,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cluster->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT cluster;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = cell->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cluster_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->cluster_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_cluster (GAMGI_CAST_CLUSTER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cluster_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cluster_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cluster_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_cluster_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cell_cell (gamgi_cell *cell,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT cell;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cell_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cell_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = assembly->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cell_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cell_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->cell_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cell_cell (GAMGI_CAST_CELL
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_cell_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cell_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_cell_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_cell_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_cell_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_arrow_arrow (gamgi_arrow *arrow,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT arrow;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_arrow_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->arrow_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_arrow_arrow (GAMGI_CAST_ARROW
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_arrow_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->arrow_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_arrow_arrow (GAMGI_CAST_ARROW
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_arrow_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_arrow_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_arrow_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_arrow_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_arrow_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_shape_shape (gamgi_shape *shape,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT shape;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_shape_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->shape_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_shape_shape (GAMGI_CAST_SHAPE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_shape_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->shape_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_shape_shape (GAMGI_CAST_SHAPE
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_shape_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_shape_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_shape_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_shape_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_shape_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_graph_graph (gamgi_graph *graph,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT graph;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_graph_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->graph_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_graph_graph (GAMGI_CAST_GRAPH
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_graph_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->graph_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_graph_graph (GAMGI_CAST_GRAPH
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

dlist = layer->assembly_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_graph_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_graph_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_graph_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_graph_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_graph_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_assembly_assembly (gamgi_assembly *assembly,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = assembly->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_assembly_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }
  
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT assembly;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_assembly_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->assembly_end;
while (dlist != NULL) 
  {
  dlist_start = gamgi_engine_dlist_assembly_assembly (GAMGI_CAST_ASSEMBLY
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_assembly_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_assembly_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_assembly_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_assembly_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_light_light (gamgi_light *light,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT light;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_light_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = layer->light_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_light_light (GAMGI_CAST_LIGHT
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_light_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_light_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_light_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_light_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_layer_layer (gamgi_layer *layer,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT layer;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_layer_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;

dlist = window->layer_end;
while (dlist != NULL)
  {
  dlist_start = gamgi_engine_dlist_layer_layer (GAMGI_CAST_LAYER
  dlist->data, dlist_start);
  dlist = dlist->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_layer_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_layer_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_window_window (gamgi_window *window,
gamgi_dlist *dlist_start)
{
dlist_start = gamgi_engine_dlist_add_start (dlist_start);
dlist_start->data = GAMGI_CAST_OBJECT window;

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_window_gamgi (gamgi_dlist *dlist_start)
{
gamgi_window *window;

window = gamgi->window_end;
while (window != NULL)
  {
  dlist_start = gamgi_engine_dlist_window_window (window, dlist_start);
  window = window->before;
  }

return dlist_start;
}

gamgi_dlist *gamgi_engine_dlist_atom_object (gamgi_object *object, gamgi_dlist *dlist_start)
{
switch (object->class)
  {
  case GAMGI_ENGINE_ATOM:
  dlist_start = gamgi_engine_dlist_atom_atom (GAMGI_CAST_ATOM object, dlist_start);
  break;

  case GAMGI_ENGINE_DIRECTION:
  dlist_start = gamgi_engine_dlist_atom_direction (GAMGI_CAST_DIRECTION object, dlist_start);
  break;

  case GAMGI_ENGINE_PLANE:
  dlist_start = gamgi_engine_dlist_atom_plane (GAMGI_CAST_PLANE object, dlist_start);
  break;

  case GAMGI_ENGINE_GROUP:
  dlist_start = gamgi_engine_dlist_atom_group (GAMGI_CAST_GROUP object, dlist_start);
  break;

  case GAMGI_ENGINE_MOLECULE:
  dlist_start = gamgi_engine_dlist_atom_molecule (GAMGI_CAST_MOLECULE object, dlist_start);
  break;

  case GAMGI_ENGINE_CLUSTER:
  dlist_start = gamgi_engine_dlist_atom_cluster (GAMGI_CAST_CLUSTER object, dlist_start);
  break;

  case GAMGI_ENGINE_CELL:
  dlist_start = gamgi_engine_dlist_atom_cell (GAMGI_CAST_CELL object, dlist_start);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  dlist_start = gamgi_engine_dlist_atom_assembly (GAMGI_CAST_ASSEMBLY object, dlist_start);
  break;

  case GAMGI_ENGINE_LAYER:
  dlist_start = gamgi_engine_dlist_atom_layer (GAMGI_CAST_LAYER object, dlist_start);
  break;

  case GAMGI_ENGINE_WINDOW:
  dlist_start = gamgi_engine_dlist_atom_window (GAMGI_CAST_WINDOW object, dlist_start);
  break;

  case GAMGI_ENGINE_GAMGI:
  dlist_start = gamgi_engine_dlist_atom_gamgi (dlist_start);
  break;

  default:
  break;
  }

return dlist_start;
}
