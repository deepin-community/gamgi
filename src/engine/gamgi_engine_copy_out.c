/*********************************************
 *
 * $GAMGI/src/engine/gamgi_engine_copy_out.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_io.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_mesa_text.h"

static void static_copy (gamgi_object *object, gamgi_object *object_new)
{
strcpy (object_new->name, object->name);
strcpy (object_new->label, object->label);
object_new->class = object->class;
object_new->scale = object->scale;
}

void gamgi_engine_copy_out_text (gamgi_text *text, gamgi_text *text_new)
{
static_copy (GAMGI_CAST_OBJECT text, GAMGI_CAST_OBJECT text_new);
text_new->draw = text->draw;
text_new->mark = text->mark;

/**********************************
 * safe programming: text->string *
 * should never be NULL here      *
 **********************************/

if (text->string != NULL)
  {
  text_new->string = (char *) malloc ((text->length + 1) * sizeof (char));
  strcpy (text_new->string, text->string);
  }
text_new->length = text->length;

text_new->style = text->style;
text_new->font = text->font;
gamgi_mesa_text_font_increase (text_new, NULL);

/****************************
 * position and orientation *
 ****************************/

text_new->origin[0] = text->origin[0];
text_new->origin[1] = text->origin[1];
text_new->origin[2] = text->origin[2];

text_new->center[0] = text->center[0];
text_new->center[1] = text->center[1];
text_new->center[2] = text->center[2];

text_new->quaternion[0] = text->quaternion[0];
text_new->quaternion[1] = text->quaternion[1];
text_new->quaternion[2] = text->quaternion[2];
text_new->quaternion[3] = text->quaternion[3];

/*************
 * view data *
 *************/

text_new->red = text->red;
text_new->green = text->green;
text_new->blue = text->blue;
}

void gamgi_engine_copy_out_orbital (gamgi_orbital *orbital, gamgi_orbital *orbital_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT orbital, GAMGI_CAST_OBJECT orbital_new);
orbital_new->draw = orbital->draw;

orbital_new->n = orbital->n;
orbital_new->l = orbital->l;
orbital_new->m = orbital->m;
orbital_new->charge = orbital->charge;

orbital_new->style = orbital->style;
orbital_new->radius = orbital->radius;
orbital_new->density = orbital->density;
orbital_new->sampling = orbital->sampling;
orbital_new->seed = orbital->seed;

orbital_new->phase = orbital->phase;
orbital_new->frame = orbital->frame;
for (i = 0; i < 8; i++) orbital_new->octants[i] = orbital->octants[i];
orbital_new->axes = orbital->axes;

/******************************
 * copy arrays: points, lines *
 ******************************/

orbital_new->dots[0] = orbital->dots[0];
orbital_new->dots[1] = orbital->dots[1];
orbital_new->dots[2] = orbital->dots[2];

orbital_new->points = NULL;
i = 3 * orbital->n_points;
if (i != 0) orbital_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0) orbital_new->points[i] = orbital->points[i];

orbital_new->lines = NULL;
i = orbital->n_lines;
if (i != 0) orbital_new->lines = (int *) malloc (sizeof (int) * i);
while (i-- > 0) orbital_new->lines[i] = orbital->lines[i];

orbital_new->n_points = orbital->n_points;
orbital_new->n_lines = orbital->n_lines;

/**************************************
 * copy position and orientation data *
 **************************************/

orbital_new->origin[0] = orbital->origin[0];
orbital_new->origin[1] = orbital->origin[1];
orbital_new->origin[2] = orbital->origin[2];

orbital_new->center[0] = orbital->center[0];
orbital_new->center[1] = orbital->center[1];
orbital_new->center[2] = orbital->center[2];

orbital_new->quaternion[0] = orbital->quaternion[0];
orbital_new->quaternion[1] = orbital->quaternion[1];
orbital_new->quaternion[2] = orbital->quaternion[2];
orbital_new->quaternion[3] = orbital->quaternion[3];

/**************************
 * copy color information *
 **************************/

orbital_new->base_r = orbital->base_r;
orbital_new->base_g = orbital->base_g;
orbital_new->base_b = orbital->base_b;

orbital_new->phase_r = orbital->phase_r;
orbital_new->phase_g = orbital->phase_g;
orbital_new->phase_b = orbital->phase_b;

orbital_new->frame_r = orbital->frame_r;
orbital_new->frame_g = orbital->frame_g;
orbital_new->frame_b = orbital->frame_b;
}

void gamgi_engine_copy_out_bond (gamgi_bond *bond, gamgi_bond *bond_new)
{
/*************
 * copy data *
 *************/

static_copy (GAMGI_CAST_OBJECT bond, GAMGI_CAST_OBJECT bond_new);

bond_new->draw = bond->draw;
bond_new->size = bond->size;

bond_new->color = bond->color;
bond_new->red = bond->red;
bond_new->green = bond->green;
bond_new->blue = bond->blue;

bond_new->order = bond->order;
bond_new->covalency = bond->covalency;

/**************
 * start data *
 **************/

bond_new->first = TRUE;
bond_new->copy = NULL;
bond_new->atom1 = NULL;
bond_new->atom2 = NULL;
bond_new->dlist1 = NULL;
bond_new->dlist2 = NULL;
}

void gamgi_engine_copy_out_atom (gamgi_atom *atom, gamgi_atom *atom_new)
{
static_copy (GAMGI_CAST_OBJECT atom, GAMGI_CAST_OBJECT atom_new);
atom_new->draw = atom->draw;
atom_new->mark = atom->mark;

atom_new->element = atom->element;
atom_new->mass = atom->mass;
atom_new->radius = atom->radius;
atom_new->charge = atom->charge;
atom_new->temperature = atom->temperature;
atom_new->occupancy = atom->occupancy;

atom_new->position[0] = atom->position[0];
atom_new->position[1] = atom->position[1];
atom_new->position[2] = atom->position[2];

/*************
 * view data *
 *************/

atom_new->size = atom->size;
atom_new->variancy = atom->variancy;

atom_new->red = atom->red;
atom_new->green = atom->green;
atom_new->blue = atom->blue;
}

void gamgi_engine_copy_out_direction (gamgi_direction *direction, 
gamgi_direction *direction_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT direction, GAMGI_CAST_OBJECT direction_new);
direction_new->net = direction->net;
direction_new->draw = direction->draw;
direction_new->reference = direction->reference;
direction_new->model = direction->model;

/*****************************
 * copy description: indices *
 *****************************/

direction_new->uvw[0] = direction->uvw[0];
direction_new->uvw[1] = direction->uvw[1];
direction_new->uvw[2] = direction->uvw[2];
direction_new->vectors = direction->vectors;

direction_new->node[0] = direction->node[0];
direction_new->node[1] = direction->node[1];
direction_new->node[2] = direction->node[2];
direction_new->node[3] = direction->node[3];

/**************************************
 * copy position and orientation data *
 **************************************/

direction_new->origin[0] = direction->origin[0];
direction_new->origin[1] = direction->origin[1];
direction_new->origin[2] = direction->origin[2];

direction_new->center[0] = direction->center[0];
direction_new->center[1] = direction->center[1];
direction_new->center[2] = direction->center[2];

direction_new->quaternion[0] = direction->quaternion[0];
direction_new->quaternion[1] = direction->quaternion[1];
direction_new->quaternion[2] = direction->quaternion[2];
direction_new->quaternion[3] = direction->quaternion[3];

/******************************
 * copy arrays: points, loops *
 ******************************/

direction_new->points = NULL;
i = 3 * direction->n_points;
if (i != 0)
  direction_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0)
  direction_new->points[i] = direction->points[i];

direction_new->loops = NULL;
i = direction->n_loops;
if (i != 0)
  direction_new->loops = (int *) malloc (sizeof (int) * i);
while (i-- > 0)
  direction_new->loops[i] = direction->loops[i];

direction_new->n_points = direction->n_points;
direction_new->n_loops = direction->n_loops;

/*****************************
 * copy visualization: color *
 *****************************/

direction_new->red = direction->red;
direction_new->green = direction->green;
direction_new->blue = direction->blue;
}

void gamgi_engine_copy_out_plane (gamgi_plane *plane, gamgi_plane *plane_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT plane, GAMGI_CAST_OBJECT plane_new);
plane_new->net = plane->net;
plane_new->draw = plane->draw;
plane_new->reference = plane->reference;
plane_new->model = plane->model;

/*****************************
 * copy description: indices *
 *****************************/

plane_new->hkl[0] = plane->hkl[0];
plane_new->hkl[1] = plane->hkl[1];
plane_new->hkl[2] = plane->hkl[2];
plane_new->vectors = plane->vectors;
plane_new->order = plane->order;

/**************************************
 * copy position and orientation data *
 **************************************/

plane_new->origin[0] = plane->origin[0];
plane_new->origin[1] = plane->origin[1];
plane_new->origin[2] = plane->origin[2];

plane_new->center[0] = plane->center[0];
plane_new->center[1] = plane->center[1];
plane_new->center[2] = plane->center[2];

plane_new->quaternion[0] = plane->quaternion[0];
plane_new->quaternion[1] = plane->quaternion[1];
plane_new->quaternion[2] = plane->quaternion[2];
plane_new->quaternion[3] = plane->quaternion[3];

/********************************************
 * copy visualization arrays: points, loops *
 ********************************************/

plane_new->points = NULL;
i = 3 * plane->n_points;
if (i != 0)
  plane_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0)
  plane_new->points[i] = plane->points[i];

plane_new->loops = NULL;
i = plane->n_loops;
if (i != 0)
  plane_new->loops = (int *) malloc (sizeof (int) * i);
while (i-- > 0)
  plane_new->loops[i] = plane->loops[i];

plane_new->n_points = plane->n_points;
plane_new->n_loops = plane->n_loops;

/**************
 * copy color *
 **************/

plane_new->red = plane->red;
plane_new->green = plane->green;
plane_new->blue = plane->blue;
}

void gamgi_engine_copy_out_group (gamgi_group *group, gamgi_group *group_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT group, GAMGI_CAST_OBJECT group_new);
group_new->draw = group->draw;
group_new->reference = group->reference;
group_new->mark = group->mark;

/**************************************
 * copy position and orientation data *
 **************************************/

group_new->origin[0] = group->origin[0];
group_new->origin[1] = group->origin[1];
group_new->origin[2] = group->origin[2];

group_new->center[0] = group->center[0];
group_new->center[1] = group->center[1];
group_new->center[2] = group->center[2];

group_new->quaternion[0] = group->quaternion[0];
group_new->quaternion[1] = group->quaternion[1];
group_new->quaternion[2] = group->quaternion[2];
group_new->quaternion[3] = group->quaternion[3];

/******************************
 * copy arrays: points, loops *
 ******************************/

group_new->points = NULL;
i = 3 * group->n_points;
if (i > 0) group_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0) group_new->points[i] = group->points[i];

group_new->loops = NULL;
i = group->n_loops;
if (i > 0) group_new->loops = (int *) malloc (sizeof (int) * i);
while (i-- > 0) group_new->loops[i] = group->loops[i];

/********************************
 * copy number of points, loops *
 ********************************/

group_new->n_points = group->n_points;
group_new->n_loops = group->n_loops;

/******************
 * copy view data *
 ******************/

group_new->faces = group->faces;
group_new->red = group->red;
group_new->green = group->green;
group_new->blue = group->blue;
}

void gamgi_engine_copy_out_molecule (gamgi_molecule *molecule, gamgi_molecule *molecule_new)
{
static_copy (GAMGI_CAST_OBJECT molecule, GAMGI_CAST_OBJECT molecule_new);
molecule_new->draw = molecule->draw;

/**************************************
 * copy position and orientation data *
 **************************************/

molecule_new->origin[0] = molecule->origin[0];
molecule_new->origin[1] = molecule->origin[1];
molecule_new->origin[2] = molecule->origin[2];

molecule_new->center[0] = molecule->center[0];
molecule_new->center[1] = molecule->center[1];
molecule_new->center[2] = molecule->center[2];

molecule_new->quaternion[0] = molecule->quaternion[0];
molecule_new->quaternion[1] = molecule->quaternion[1];
molecule_new->quaternion[2] = molecule->quaternion[2];
molecule_new->quaternion[3] = molecule->quaternion[3];
}

void gamgi_engine_copy_out_cluster (gamgi_cluster *cluster, gamgi_cluster *cluster_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT cluster, GAMGI_CAST_OBJECT cluster_new);
cluster_new->reference = cluster->reference;
cluster_new->mark = cluster->mark;

/**************************************
 * copy position and orientation data *
 **************************************/

cluster_new->origin[0] = cluster->origin[0];
cluster_new->origin[1] = cluster->origin[1];
cluster_new->origin[2] = cluster->origin[2];

cluster_new->center[0] = cluster->center[0];
cluster_new->center[1] = cluster->center[1];
cluster_new->center[2] = cluster->center[2];

cluster_new->quaternion[0] = cluster->quaternion[0];
cluster_new->quaternion[1] = cluster->quaternion[1];
cluster_new->quaternion[2] = cluster->quaternion[2];
cluster_new->quaternion[3] = cluster->quaternion[3];

/**********************************************
 * copy arrays: points, loops, colors, paints *
 **********************************************/

cluster_new->points = NULL;
i = 3 * cluster->n_points;
if (i > 0) cluster_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0) cluster_new->points[i] = cluster->points[i];

cluster_new->loops = NULL;
i = cluster->n_loops;
if (i > 0) cluster_new->loops = (int *) malloc (sizeof (int) * i);
while (i-- > 0) cluster_new->loops[i] = cluster->loops[i];

cluster_new->colors = NULL;
i = 3 * cluster->n_colors;
if (i > 0) cluster_new->colors = (float *) malloc (sizeof (float) * i);
while (i-- > 0) cluster_new->colors[i] = cluster->colors[i];

cluster_new->paints = NULL;
i = cluster->n_paints;
if (i > 0) cluster_new->paints = (int *) malloc (sizeof (int) * i);
while (i-- > 0) cluster_new->paints[i] = cluster->paints[i];

/**********************************************************
 * copy number of points, loops, colors, paints and seeds *
 **********************************************************/

cluster_new->n_points = cluster->n_points;
cluster_new->n_loops = cluster->n_loops;
cluster_new->n_colors = cluster->n_colors;
cluster_new->n_paints = cluster->n_paints;
cluster_new->atoms = cluster->atoms;

/*************
 * View data *
 *************/

cluster_new->faces = cluster->faces;
}

void gamgi_engine_copy_out_cell (gamgi_cell *cell, gamgi_cell *cell_new)
{
int i;

static_copy (GAMGI_CAST_OBJECT cell, GAMGI_CAST_OBJECT cell_new);

/******************************
 * copy crystallographic data *
 ******************************/

cell_new->lattice = cell->lattice;
cell_new->group = cell->group;
cell_new->reciprocal = cell->reciprocal;

cell_new->a = cell->a;
cell_new->b = cell->b;
cell_new->c = cell->c;
cell_new->ab = cell->ab;
cell_new->ac = cell->ac;
cell_new->bc = cell->bc;

/*******************************************
 * copy conventional and primitive vectors *
 *******************************************/

gamgi_math_vector_copy (cell->a1, cell_new->a1);
gamgi_math_vector_copy (cell->a2, cell_new->a2);
gamgi_math_vector_copy (cell->a3, cell_new->a3);

gamgi_math_vector_copy (cell->p1, cell_new->p1);
gamgi_math_vector_copy (cell->p2, cell_new->p2);
gamgi_math_vector_copy (cell->p3, cell_new->p3);

/**************************************
 * copy position and orientation data *
 **************************************/

cell_new->origin[0] = cell->origin[0];
cell_new->origin[1] = cell->origin[1];
cell_new->origin[2] = cell->origin[2];

cell_new->center[0] = cell->center[0];
cell_new->center[1] = cell->center[1];
cell_new->center[2] = cell->center[2];

cell_new->quaternion[0] = cell->quaternion[0];
cell_new->quaternion[1] = cell->quaternion[1];
cell_new->quaternion[2] = cell->quaternion[2];
cell_new->quaternion[3] = cell->quaternion[3];

/*************************************
 * copy arrays: points, loops, lines *
 *************************************/

cell_new->points = NULL;
i = 3 * cell->n_points;
if (i != 0) cell_new->points = (double *) malloc (sizeof (double) * i);
while (i-- > 0) cell_new->points[i] = cell->points[i];

cell_new->loops = NULL;
i = cell->n_loops;
if (i != 0) cell_new->loops = (int *) malloc (sizeof (int) * i);
while (i-- > 0) cell_new->loops[i] = cell->loops[i];

cell_new->lines = NULL;
i = cell->n_lines;
if (i != 0) cell_new->lines = (int *) malloc (sizeof (int) * i);
while (i-- > 0) cell_new->lines[i] = cell->lines[i];

/*************************************************
 * copy number of points, loops, lines and nodes *
 *************************************************/

cell_new->n_points = cell->n_points;
cell_new->n_loops = cell->n_loops;
cell_new->n_lines = cell->n_lines;
cell_new->n_nodes = cell->n_nodes;

/********************************************************
 * copy volume data: number of cells, volume parameters *
 ********************************************************/

cell_new->model = cell->model;

cell_new->n1 = cell->n1;
cell_new->n2 = cell->n2;
cell_new->n3 = cell->n3;

cell_new->v1 = cell->v1;
cell_new->v2 = cell->v2;
cell_new->v3 = cell->v3;
cell_new->v12 = cell->v12;
cell_new->v13 = cell->v13;
cell_new->v23 = cell->v23;

/********************
 * copy origin data *
 ********************/

cell_new->o1 = cell->o1;
cell_new->o2 = cell->o2;
cell_new->o3 = cell->o3;
cell_new->origin_vectors = cell->origin_vectors;

/******************
 * copy axes data *
 ******************/

cell_new->axes = cell->axes;
cell_new->axes_vectors = cell->axes_vectors;

/************************************************
 * copy enumerations and booleans, regarding    *
 * the type of lattice, group, volume, borders, *
 * and whether faces and nodes are visible.     *
 ************************************************/

cell_new->borders = cell->borders;
cell_new->faces = cell->faces;
cell_new->nodes = cell->nodes;

/**************************
 * copy color information *
 **************************/

cell_new->red = cell->red;
cell_new->green = cell->green;
cell_new->blue = cell->blue;
}

void gamgi_engine_copy_out_arrow (gamgi_arrow *arrow, gamgi_arrow *arrow_new)
{
static_copy (GAMGI_CAST_OBJECT arrow, GAMGI_CAST_OBJECT arrow_new);
arrow_new->draw = arrow->draw;
}

void gamgi_engine_copy_out_shape (gamgi_shape *shape, gamgi_shape *shape_new)
{
static_copy (GAMGI_CAST_OBJECT shape, GAMGI_CAST_OBJECT shape_new);
shape_new->draw = shape->draw;
}

void gamgi_engine_copy_out_graph (gamgi_graph *graph, gamgi_graph *graph_new)
{
static_copy (GAMGI_CAST_OBJECT graph, GAMGI_CAST_OBJECT graph_new);
graph_new->draw = graph->draw;
}

void gamgi_engine_copy_out_assembly (gamgi_assembly *assembly, gamgi_assembly *assembly_new)
{
static_copy (GAMGI_CAST_OBJECT assembly, GAMGI_CAST_OBJECT assembly_new);
assembly_new->draw = assembly->draw;
assembly_new->mark = assembly->mark;

/**************************************
 * copy position and orientation data *
 **************************************/

assembly_new->origin[0] = assembly->origin[0];
assembly_new->origin[1] = assembly->origin[1];
assembly_new->origin[2] = assembly->origin[2];

assembly_new->center[0] = assembly->center[0];
assembly_new->center[1] = assembly->center[1];
assembly_new->center[2] = assembly->center[2];

assembly_new->quaternion[0] = assembly->quaternion[0];
assembly_new->quaternion[1] = assembly->quaternion[1];
assembly_new->quaternion[2] = assembly->quaternion[2];
assembly_new->quaternion[3] = assembly->quaternion[3];
}

void gamgi_engine_copy_out_light (gamgi_light *light, gamgi_light *light_new)
{
static_copy (GAMGI_CAST_OBJECT light, GAMGI_CAST_OBJECT light_new);

light_new->draw = light->draw;

light_new->ambient [0] = light->ambient [0];
light_new->ambient [1] = light->ambient [1];
light_new->ambient [2] = light->ambient [2];
light_new->ambient [3] = light->ambient [3];

light_new->diffuse [0] = light->diffuse [0];
light_new->diffuse [1] = light->diffuse [1];
light_new->diffuse [2] = light->diffuse [2];
light_new->diffuse [3] = light->diffuse [3];

light_new->specular [0] = light->specular [0];
light_new->specular [1] = light->specular [1];
light_new->specular [2] = light->specular [2];
light_new->specular [3] = light->specular [3];

light_new->position[0] = light->position[0];
light_new->position[1] = light->position[1];
light_new->position[2] = light->position[2];
light_new->position[3] = light->position[3];

light_new->direction[0] = light->direction[0];
light_new->direction[1] = light->direction[1];
light_new->direction[2] = light->direction[2];

light_new->constant = light->constant;
light_new->linear = light->linear;
light_new->quadratic = light->quadratic;
light_new->angle = light->angle;
light_new->radial = light->radial;
}

void gamgi_engine_copy_out_layer (gamgi_layer *layer, gamgi_layer *layer_new)
{
static_copy (GAMGI_CAST_OBJECT layer, GAMGI_CAST_OBJECT layer_new);

layer_new->draw = layer->draw;

layer_new->perspective = layer->perspective;
layer_new->top = layer->top;
layer_new->near = layer->near;
layer_new->far = layer->far;

gamgi_math_vector_copy (layer->eye, layer_new->eye);
gamgi_math_vector_copy (layer->center, layer_new->center);
gamgi_math_vector_copy (layer->up, layer_new->up);

gamgi_math_matrix_copy (layer->referential, layer_new->referential);

layer_new->visibility_in = layer->visibility_in;
layer_new->visibility_out = layer->visibility_out;

layer_new->axes = layer->axes;

layer_new->light = layer->light;
layer_new->color[0] = layer->color[0];
layer_new->color[1] = layer->color[1];
layer_new->color[2] = layer->color[2];
layer_new->color[3] = layer->color[3];
}

void gamgi_engine_copy_out_window (gamgi_window *window, gamgi_window *window_new)
{
/*************
 * copy data *
 *************/

static_copy (GAMGI_CAST_OBJECT window, GAMGI_CAST_OBJECT window_new);

window_new->draw = window->draw;

window_new->origin_x = window->origin_x;
window_new->origin_y = window->origin_y;
window_new->width = window->width;
window_new->height = window->height;

window_new->top = window->top;
window_new->medium = window->medium;
window_new->bottom = window->bottom;
window_new->top_flag = window->top_flag;
window_new->medium_flag = window->medium_flag;
window_new->bottom_flag = window->bottom_flag;

/**************
 * start data *
 **************/

window_new->window = NULL;
window_new->dialog0 = NULL;
window_new->dialog1 = NULL;
window_new->help = NULL;

window_new->area = NULL;
window_new->history = NULL;
window_new->layer = NULL;
window_new->focus = NULL;

window_new->action = FALSE;
window_new->mode = FALSE;
window_new->axes = FALSE;
window_new->class = FALSE;

window_new->top_flag = TRUE;
window_new->medium_flag = TRUE;
window_new->bottom_flag = TRUE;

window_new->timer_focus = 0;
window_new->timer_layer = 0;
window_new->timer_message = 0;
window_new->timer_progress = 0;

window_new->axis = FALSE;
window_new->total = 0.0;

window_new->demo_flag = 0;
}
