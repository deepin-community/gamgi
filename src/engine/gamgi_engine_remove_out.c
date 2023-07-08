/***********************************************
 *
 * $GAMGI/src/engine/gamgi_engine_remove_out.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

void gamgi_engine_remove_out_text (gamgi_text *text)
{
/* bypass text list and free text */
if (text->before) text->before->next = text->next;
if (text->next) text->next->before = text->before;
free(text);
}

void gamgi_engine_remove_out_orbital (gamgi_orbital *orbital)
{
/* bypass orbital list and free orbital */
if (orbital->before) orbital->before->next = orbital->next;
if (orbital->next) orbital->next->before = orbital->before;
free(orbital);
}

void gamgi_engine_remove_out_bond (gamgi_bond *bond)
{
/* bypass bond list and free bond */
if (bond->before) bond->before->next = bond->next;
if (bond->next) bond->next->before = bond->before;
free(bond);
}

void gamgi_engine_remove_out_atom (gamgi_atom *atom)
{
/* bypass atom list and free atom */
if (atom->before) atom->before->next = atom->next;
if (atom->next) atom->next->before = atom->before;
free(atom);
}

void gamgi_engine_remove_out_direction (gamgi_direction *direction)
{
/* bypass direction list and free direction */
if (direction->before) direction->before->next = direction->next;
if (direction->next) direction->next->before = direction->before;
free(direction);
}

void gamgi_engine_remove_out_plane (gamgi_plane *plane)
{
/* bypass plane list and free plane */
if (plane->before) plane->before->next = plane->next;
if (plane->next) plane->next->before = plane->before;
free(plane);
}

void gamgi_engine_remove_out_group (gamgi_group *group)
{
/* bypass group list and free group */
if (group->before) group->before->next = group->next;
if (group->next) group->next->before = group->before;
free(group);
}

void gamgi_engine_remove_out_molecule (gamgi_molecule *molecule)
{
/* bypass molecule list and free molecule */
if (molecule->before) molecule->before->next = molecule->next;
if (molecule->next) molecule->next->before = molecule->before;
free(molecule);
}

void gamgi_engine_remove_out_cluster (gamgi_cluster *cluster)
{
/* bypass cluster list and free cluster */
if (cluster->before) cluster->before->next = cluster->next;
if (cluster->next) cluster->next->before = cluster->before;
free(cluster);
}

void gamgi_engine_remove_out_cell (gamgi_cell *cell)
{
/* bypass cell list and free cell */
if (cell->before) cell->before->next = cell->next;
if (cell->next) cell->next->before = cell->before;
free(cell);
}

void gamgi_engine_remove_out_arrow (gamgi_arrow *arrow)
{
/* bypass arrow list and free arrow */
if (arrow->before) arrow->before->next = arrow->next;
if (arrow->next) arrow->next->before = arrow->before;
free(arrow);
}

void gamgi_engine_remove_out_shape (gamgi_shape *shape)
{
/* bypass shape list and free shape */
if (shape->before) shape->before->next = shape->next;
if (shape->next) shape->next->before = shape->before;
free(shape);
}

void gamgi_engine_remove_out_graph (gamgi_graph *graph)
{
/* bypass graph list and free graph */
if (graph->before) graph->before->next = graph->next;
if (graph->next) graph->next->before = graph->before;
free(graph);
}

void gamgi_engine_remove_out_assembly (gamgi_assembly *assembly)
{
/* bypass assembly list and free assembly */
if (assembly->before) assembly->before->next = assembly->next;
if (assembly->next) assembly->next->before = assembly->before;
free(assembly);
}

void gamgi_engine_remove_out_light (gamgi_light *light)
{
/* bypass light list and free light */
if (light->before) light->before->next = light->next;
if (light->next) light->next->before = light->before;
free(light);
}

void gamgi_engine_remove_out_layer (gamgi_layer *layer)
{
/* bypass layer list and free layer */
if (layer->before) layer->before->next = layer->next;
if (layer->next) layer->next->before = layer->before;
free(layer);
}

void gamgi_engine_remove_out_window (gamgi_window *window)
{
/* bypass window list and free window */
if (window->before) window->before->next = window->next;
if (window->next) window->next->before = window->before;
free(window);
}
