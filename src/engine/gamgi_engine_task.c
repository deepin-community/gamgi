/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_task.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

static unsigned int task;

unsigned int gamgi_engine_task_get (void)
{
return task;
}

void gamgi_engine_task_start (void)
{
gamgi_text *text;
gamgi_orbital *orbital;
gamgi_bond *bond;
gamgi_atom *atom;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_arrow *arrow;
gamgi_shape *shape;
gamgi_graph *graph;
gamgi_assembly *assembly;
gamgi_light *light;
gamgi_layer *layer;
gamgi_window *window;

task = 0;

for (text = gamgi->text_start; text != NULL; text = text->next)
  (GAMGI_CAST_OBJECT text)->task = 0;

for (orbital = gamgi->orbital_start; orbital != NULL; orbital = orbital->next)
  (GAMGI_CAST_OBJECT orbital)->task = 0;

for (bond = gamgi->bond_start; bond != NULL; bond = bond->next)
  (GAMGI_CAST_OBJECT bond)->task = 0;

for (atom = gamgi->atom_start; atom != NULL; atom = atom->next)
  (GAMGI_CAST_OBJECT atom)->task = 0;

for (direction = gamgi->direction_start; direction != NULL; direction = direction->next)
  (GAMGI_CAST_OBJECT direction)->task = 0;

for (plane = gamgi->plane_start; plane != NULL; plane = plane->next)
  (GAMGI_CAST_OBJECT plane)->task = 0;

for (group = gamgi->group_start; group != NULL; group = group->next)
  (GAMGI_CAST_OBJECT group)->task = 0;

for (molecule = gamgi->molecule_start; molecule != NULL; molecule = molecule->next)
  (GAMGI_CAST_OBJECT molecule)->task = 0;

for (cluster = gamgi->cluster_start; cluster != NULL; cluster = cluster->next)
  (GAMGI_CAST_OBJECT cluster)->task = 0;

for (cell = gamgi->cell_start; cell != NULL; cell = cell->next)
  (GAMGI_CAST_OBJECT cell)->task = 0;

for (shape = gamgi->shape_start; shape != NULL; shape = shape->next)
  (GAMGI_CAST_OBJECT shape)->task = 0;

for (arrow = gamgi->arrow_start; arrow != NULL; arrow = arrow->next)
  (GAMGI_CAST_OBJECT arrow)->task = 0;

for (graph = gamgi->graph_start; graph != NULL; graph = graph->next)
  (GAMGI_CAST_OBJECT graph)->task = 0;

for (assembly = gamgi->assembly_start; assembly != NULL; assembly = assembly->next)
  (GAMGI_CAST_OBJECT assembly)->task = 0;

for (light = gamgi->light_start; light != NULL; light = light->next)
  (GAMGI_CAST_OBJECT light)->task = 0;

for (layer = gamgi->layer_start; layer != NULL; layer = layer->next)
  (GAMGI_CAST_OBJECT layer)->task = 0;

for (window = gamgi->window_start; window != NULL; window = window->next)
  (GAMGI_CAST_OBJECT window)->task = 0;
}

unsigned int gamgi_engine_task_new (void)
{
if (task == UINT_MAX) gamgi_engine_task_start ();

return ++task;
}
