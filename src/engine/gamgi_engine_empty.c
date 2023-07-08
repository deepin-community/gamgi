/******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_empty.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

gamgi_bool gamgi_engine_empty_text (gamgi_text *text)
{
if (text->text_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_orbital (gamgi_orbital *orbital)
{
if (orbital->text_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_bond (gamgi_bond *bond)
{
if (bond->text_start != NULL) return FALSE;
if (bond->orbital_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_atom (gamgi_atom *atom)
{
if (atom->text_start != NULL) return FALSE;
if (atom->orbital_start != NULL) return FALSE;
if (atom->bond_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_direction (gamgi_direction *direction)
{
if (direction->text_start != NULL) return FALSE;
if (direction->orbital_start != NULL) return FALSE;
if (direction->atom_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_plane (gamgi_plane *plane)
{
if (plane->text_start != NULL) return FALSE;
if (plane->orbital_start != NULL) return FALSE;
if (plane->atom_start != NULL) return FALSE;
if (plane->direction_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_group (gamgi_group *group)
{
if (group->text_start != NULL) return FALSE;
if (group->orbital_start != NULL) return FALSE;
if (group->atom_start != NULL) return FALSE;
if (group->direction_start != NULL) return FALSE;
if (group->plane_start != NULL) return FALSE;
if (group->group_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_molecule (gamgi_molecule *molecule)
{
if (molecule->text_start != NULL) return FALSE;
if (molecule->orbital_start != NULL) return FALSE;
if (molecule->atom_start != NULL) return FALSE;
if (molecule->direction_start != NULL) return FALSE;
if (molecule->plane_start != NULL) return FALSE;
if (molecule->group_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_cluster (gamgi_cluster *cluster)
{
if (cluster->text_start != NULL) return FALSE;
if (cluster->orbital_start != NULL) return FALSE;
if (cluster->atom_start != NULL) return FALSE;
if (cluster->direction_start != NULL) return FALSE;
if (cluster->plane_start != NULL) return FALSE;
if (cluster->group_start != NULL) return FALSE;
if (cluster->molecule_start != NULL) return FALSE;
if (cluster->cluster_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_cell (gamgi_cell *cell)
{
if (cell->text_start != NULL) return FALSE;
if (cell->orbital_start != NULL) return FALSE;
if (cell->atom_start != NULL) return FALSE;
if (cell->direction_start != NULL) return FALSE;
if (cell->plane_start != NULL) return FALSE;
if (cell->group_start != NULL) return FALSE;
if (cell->molecule_start != NULL) return FALSE;
if (cell->cluster_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_arrow (gamgi_arrow *arrow)
{
if (arrow->text_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_shape (gamgi_shape *shape)
{
if (shape->text_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_graph (gamgi_graph *graph)
{
if (graph->text_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_assembly (gamgi_assembly *assembly)
{
if (assembly->text_start != NULL) return FALSE;
if (assembly->orbital_start != NULL) return FALSE;
if (assembly->atom_start != NULL) return FALSE;
if (assembly->direction_start != NULL) return FALSE;
if (assembly->plane_start != NULL) return FALSE;
if (assembly->group_start != NULL) return FALSE;
if (assembly->molecule_start != NULL) return FALSE;
if (assembly->cluster_start != NULL) return FALSE;
if (assembly->cell_start != NULL) return FALSE;
if (assembly->arrow_start != NULL) return FALSE;
if (assembly->shape_start != NULL) return FALSE;
if (assembly->graph_start != NULL) return FALSE;
if (assembly->assembly_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_light (gamgi_light *light)
{
return TRUE;
}

gamgi_bool gamgi_engine_empty_layer (gamgi_layer *layer)
{
if (layer->text_start != NULL) return FALSE;
if (layer->orbital_start != NULL) return FALSE;
if (layer->atom_start != NULL) return FALSE;
if (layer->direction_start != NULL) return FALSE;
if (layer->plane_start != NULL) return FALSE;
if (layer->group_start != NULL) return FALSE;
if (layer->molecule_start != NULL) return FALSE;
if (layer->cluster_start != NULL) return FALSE;
if (layer->cell_start != NULL) return FALSE;
if (layer->arrow_start != NULL) return FALSE;
if (layer->shape_start != NULL) return FALSE;
if (layer->graph_start != NULL) return FALSE;
if (layer->assembly_start != NULL) return FALSE;
if (layer->light_start != NULL) return FALSE;

return TRUE;
}

gamgi_bool gamgi_engine_empty_window (gamgi_window *window)
{
if (window->layer_start != NULL) return FALSE;

return TRUE;
}
