/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_start.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_number.h"
#include "gamgi_global_start.h"

/**** internal function ******
 *                           *
 *       STATIC_START        *
 *                           *
 * Initialise GAMGI objects. *
 *                           *
 *****************************/

void gamgi_engine_start_object (gamgi_object *object, int class)
{
strcpy (object->name, "");
strcpy (object->label, "");
object->class = class;
object->scale = GAMGI_MESA_SCALE;
object->task = 0;
object->selection = NULL;
object->dlist = NULL;
object->object = NULL;
}

/**** external function *****
 *                          *
 * GAMGI_ENGINE_START_TEXT  *
 *                          *
 * Initialise text objects. *
 *                          *
 ****************************/

void gamgi_engine_start_text (gamgi_text *text)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT text, GAMGI_ENGINE_TEXT);
text->object.number = gamgi_engine_number_text_get ();

/*******************
 * initialise text *
 *******************/

text->draw = NULL;
text->mark = 0;

text->origin[0] = 0.0;
text->origin[1] = 0.0;
text->origin[2] = 0.0;

text->center[0] = 0.0;
text->center[1] = 0.0;
text->center[2] = 0.0;

text->quaternion[0] = 0.0;
text->quaternion[1] = 0.0;
text->quaternion[2] = 0.0;
text->quaternion[3] = 1.0;

text->string = NULL;
text->length = 0;

text->style = FALSE;
text->font = FALSE;
}

/******* external function *****
 *                             *
 * GAMGI_ENGINE_START_ORBITAL  *
 *                             *
 * Initialise orbital objects. *
 *                             *
 *******************************/

void gamgi_engine_start_orbital (gamgi_orbital *orbital)
{
int i;

/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT orbital, GAMGI_ENGINE_ORBITAL);
orbital->object.number = gamgi_engine_number_orbital_get ();

/**********************
 * initialise orbital *
 **********************/

orbital->draw = NULL;

orbital->n = 0;
orbital->l = 0;
orbital->m = 0;
orbital->charge = 0.0;

orbital->style = FALSE;
orbital->radius = 0.0;
orbital->density = 0.0;
orbital->sampling = 0;
orbital->seed = 0;

orbital->phase = FALSE;
orbital->frame = FALSE;
for (i = 0; i < 8; i++) orbital->octants[i] = 0;
orbital->axes = FALSE;

orbital->dots[0] = 0;
orbital->dots[1] = 0;
orbital->dots[2] = 0;

orbital->points = NULL;
orbital->lines= NULL;
orbital->n_points = 0;
orbital->n_lines = 0;

orbital->origin[0] = 0.0;
orbital->origin[1] = 0.0;
orbital->origin[2] = 0.0;

orbital->center[0] = 0.0;
orbital->center[1] = 0.0;
orbital->center[2] = 0.0;

orbital->quaternion[0] = 0.0;
orbital->quaternion[1] = 0.0;
orbital->quaternion[2] = 0.0;
orbital->quaternion[3] = 1.0;
}

/**** external function *****
 *                          *
 * GAMGI_ENGINE_START_BOND  *
 *                          *
 * Initialise bond objects. *
 *                          *
 ****************************/

void gamgi_engine_start_bond (gamgi_bond *bond)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT bond, GAMGI_ENGINE_BOND);
bond->object.number = gamgi_engine_number_bond_get ();

/*******************
 * initialise bond *
 *******************/

bond->draw = NULL;
bond->covalency = -1.0;
bond->order = 1.0;
bond->size = 1.0;
bond->color = FALSE;

bond->first = TRUE;
bond->copy = NULL;
bond->atom1 = NULL;
bond->atom2 = NULL;
bond->dlist1 = NULL;
bond->dlist2 = NULL;
}

/**** external function *****
 *                          *
 * GAMGI_ENGINE_START_ATOM  *
 *                          *
 * Initialise atom objects. *
 *                          *
 ****************************/

void gamgi_engine_start_atom (gamgi_atom *atom)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT atom, GAMGI_ENGINE_ATOM);
atom->object.number = gamgi_engine_number_atom_get ();

/*******************
 * initialise atom *
 *******************/

atom->draw = NULL;
atom->mark = 0;
atom->size = 1.0;
atom->variancy = 0.0;

atom->mass = 0.0;
atom->radius = 0.0;
atom->charge = 0.0;
atom->temperature = 0.0;
atom->occupancy = 1.0;
}

/******** external function ******
 *                               *
 * GAMGI_ENGINE_START_DIRECTION  *
 *                               *
 * Initialise direction objects. *
 *                               *
 *********************************/

void gamgi_engine_start_direction (gamgi_direction *direction)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT direction, GAMGI_ENGINE_DIRECTION);
direction->object.number = gamgi_engine_number_direction_get ();

/************************
 * initialise direction *
 ************************/

direction->reference = FALSE;

direction->uvw[0] = 0;
direction->uvw[1] = 0;
direction->uvw[2] = 0;
direction->vectors = FALSE;

direction->node[0] = 0;
direction->node[1] = 0;
direction->node[2] = 0;
direction->node[3] = 0;

direction->origin[0] = 0.0;
direction->origin[1] = 0.0;
direction->origin[2] = 0.0;

direction->center[0] = 0.0;
direction->center[1] = 0.0;
direction->center[2] = 0.0;

direction->quaternion[0] = 0.0;
direction->quaternion[1] = 0.0;
direction->quaternion[2] = 0.0;
direction->quaternion[3] = 1.0;

direction->draw = NULL;
direction->net = NULL;
direction->model = FALSE;

direction->points = NULL;
direction->loops = NULL;
direction->n_points = 0;
direction->n_loops = 0;
}

/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_PLANE  *
 *                           *
 * Initialise plane objects. *
 *                           *
 *****************************/

void gamgi_engine_start_plane (gamgi_plane *plane)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT plane, GAMGI_ENGINE_PLANE);
plane->object.number = gamgi_engine_number_plane_get ();

/********************
 * initialise plane *
 ********************/

plane->reference = FALSE;

plane->hkl[0] = 0;
plane->hkl[1] = 0;
plane->hkl[2] = 0;
plane->vectors = FALSE;
plane->order = 1;

plane->origin[0] = 0.0;
plane->origin[1] = 0.0;
plane->origin[2] = 0.0;

plane->center[0] = 0.0;
plane->center[1] = 0.0;
plane->center[2] = 0.0;

plane->quaternion[0] = 0.0;
plane->quaternion[1] = 0.0;
plane->quaternion[2] = 0.0;
plane->quaternion[3] = 1.0;

plane->draw = NULL;
plane->net = NULL;
plane->model = FALSE;

plane->points = NULL;
plane->loops = NULL;
plane->n_points = 0;
plane->n_loops = 0;
}

/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_GROUP  *
 *                           *
 * Initialise group objects. *
 *                           *
 *****************************/

void gamgi_engine_start_group (gamgi_group *group)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT group, GAMGI_ENGINE_GROUP);
group->object.number = gamgi_engine_number_group_get ();

/********************
 * initialise group *
 ********************/

group->draw = NULL;
group->reference = FALSE;
group->mark = 0;

group->origin[0] = 0.0;
group->origin[1] = 0.0;
group->origin[2] = 0.0;

group->center[0] = 0.0;
group->center[1] = 0.0;
group->center[2] = 0.0;

group->quaternion[0] = 0.0;
group->quaternion[1] = 0.0;
group->quaternion[2] = 0.0;
group->quaternion[3] = 1.0;

group->points = NULL;
group->loops = NULL;
group->n_points = 0;
group->n_loops = 0;

group->faces = FALSE;
}

/******* external function ******
 *                              *
 * GAMGI_ENGINE_START_MOLECULE  *
 *                              *
 * Initialise molecule objects. *
 *                              *
 ********************************/

void gamgi_engine_start_molecule (gamgi_molecule *molecule)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT molecule, GAMGI_ENGINE_MOLECULE);
molecule->object.number = gamgi_engine_number_molecule_get ();

/***********************
 * initialise molecule *
 ***********************/

molecule->origin[0] = 0.0;
molecule->origin[1] = 0.0;
molecule->origin[2] = 0.0;

molecule->center[0] = 0.0;
molecule->center[1] = 0.0;
molecule->center[2] = 0.0;

molecule->quaternion[0] = 0.0;
molecule->quaternion[1] = 0.0;
molecule->quaternion[2] = 0.0;
molecule->quaternion[3] = 1.0;

molecule->draw = NULL;
}


/****** external function ******
 *                             *
 * GAMGI_ENGINE_START_CLUSTER  *
 *                             *
 * Initialise cluster objects. *
 *                             *
 *******************************/

void gamgi_engine_start_cluster (gamgi_cluster *cluster)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT cluster, GAMGI_ENGINE_CLUSTER);
cluster->object.number = gamgi_engine_number_cluster_get ();

/**********************
 * initialise cluster *
 **********************/

cluster->reference = FALSE;
cluster->mark = 0;

cluster->origin[0] = 0.0;
cluster->origin[1] = 0.0;
cluster->origin[2] = 0.0;

cluster->center[0] = 0.0;
cluster->center[1] = 0.0;
cluster->center[2] = 0.0;

cluster->quaternion[0] = 0.0;
cluster->quaternion[1] = 0.0;
cluster->quaternion[2] = 0.0;
cluster->quaternion[3] = 1.0;

cluster->points = NULL;
cluster->loops = NULL;
cluster->colors = NULL;
cluster->paints = NULL;

cluster->n_points = 0;
cluster->n_loops = 0;
cluster->n_colors = 0;
cluster->n_paints = 0;
cluster->atoms = 0;

cluster->faces = FALSE;
}

/***** external function ****
 *                          *
 * GAMGI_ENGINE_START_CELL  *
 *                          *
 * Initialise cell objects. *
 *                          *
 ****************************/

void gamgi_engine_start_cell (gamgi_cell *cell)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT cell, GAMGI_ENGINE_CELL);
cell->object.number = gamgi_engine_number_cell_get ();

/********************
 * initialise cell *
 ********************/

cell->lattice = FALSE;
cell->group = 0;
cell->reciprocal = 0.0;

cell->a = 0.0;
cell->b = 0.0;
cell->c = 0.0;
cell->ab = 0.0;
cell->ac = 0.0;
cell->bc = 0.0;

cell->origin[0] = 0.0;
cell->origin[1] = 0.0;
cell->origin[2] = 0.0;

cell->center[0] = 0.0;
cell->center[1] = 0.0;
cell->center[2] = 0.0;

cell->quaternion[0] = 0.0;
cell->quaternion[1] = 0.0;
cell->quaternion[2] = 0.0;
cell->quaternion[3] = 1.0;

cell->model = FALSE;

cell->n1 = 1;
cell->n2 = 1;
cell->n3 = 1;

cell->v1 = 0.0;
cell->v2 = 0.0;
cell->v3 = 0.0;
cell->v12 = 0.0;
cell->v13 = 0.0;
cell->v23 = 0.0;

cell->points = NULL;
cell->loops = NULL;
cell->lines= NULL;

cell->n_points = 0;
cell->n_loops = 0;
cell->n_lines = 0;
cell->n_nodes = 0;

cell->o1 = 0; cell->o2 = 0; cell->o3 = 0;
cell->origin_vectors = FALSE;
cell->axes = FALSE;
cell->axes_vectors = FALSE;

cell->faces = FALSE;
cell->nodes = FALSE;
cell->borders = GAMGI_PHYS_NONE;
}

/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_ARROW  *
 *                           *
 * Initialise arrow objects. *
 *                           *
 *****************************/

void gamgi_engine_start_arrow (gamgi_arrow *arrow)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT arrow, GAMGI_ENGINE_ARROW);
arrow->object.number = gamgi_engine_number_arrow_get ();

/********************
 * initialise arrow *
 ********************/

arrow->draw = NULL;
}


/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_SHAPE  *
 *                           *
 * Initialise shape objects. *
 *                           *
 *****************************/

void gamgi_engine_start_shape (gamgi_shape *shape)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT shape, GAMGI_ENGINE_SHAPE);
shape->object.number = gamgi_engine_number_shape_get ();

/********************
 * initialise shape *
 ********************/

shape->draw = NULL;
}


/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_GRAPH  *
 *                           *
 * Initialise graph objects. *
 *                           *
 *****************************/

void gamgi_engine_start_graph (gamgi_graph *graph)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT graph, GAMGI_ENGINE_GRAPH);
graph->object.number = gamgi_engine_number_graph_get ();

/********************
 * initialise graph *
 ********************/

graph->draw = NULL;
}

/******* external function ******
 *                              *
 * GAMGI_ENGINE_START_ASSEMBLY  *
 *                              *
 * Initialise assembly objects. *
 *                              *
 ********************************/

void gamgi_engine_start_assembly (gamgi_assembly *assembly)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT assembly, GAMGI_ENGINE_ASSEMBLY);
assembly->object.number = gamgi_engine_number_assembly_get ();

/***********************
 * initialise assembly *
 ***********************/

assembly->draw = NULL;
assembly->mark = 0;

assembly->origin[0] = 0.0;
assembly->origin[1] = 0.0;
assembly->origin[2] = 0.0;

assembly->center[0] = 0.0;
assembly->center[1] = 0.0;
assembly->center[2] = 0.0;

assembly->quaternion[0] = 0.0;
assembly->quaternion[1] = 0.0;
assembly->quaternion[2] = 0.0;
assembly->quaternion[3] = 1.0;
}

/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_LIGHT  *
 *                           *
 * Initialise light objects. *
 *                           *
 *****************************/

void gamgi_engine_start_light (gamgi_light *light)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT light, GAMGI_ENGINE_LIGHT);
light->object.number = gamgi_engine_number_light_get ();

/********************
 * initialise light *
 ********************/

light->ambient[0] = 0.0;
light->ambient[1] = 0.0;
light->ambient[2] = 0.0;
light->ambient[3] = 1.0;

light->diffuse[0] = 0.0;
light->diffuse[1] = 0.0;
light->diffuse[2] = 0.0;
light->diffuse[3] = 1.0;

light->specular[0] = 0.0;
light->specular[1] = 0.0;
light->specular[2] = 0.0;
light->specular[3] = 1.0;

light->direction[0] = GAMGI_MESA_LIGHT_DIRECTION_X;
light->direction[1] = GAMGI_MESA_LIGHT_DIRECTION_Y;
light->direction[2] = GAMGI_MESA_LIGHT_DIRECTION_Z;

light->angle = 180.0;

light->constant = GAMGI_MESA_LIGHT_CONSTANT;
light->linear = GAMGI_MESA_LIGHT_LINEAR;
light->quadratic = GAMGI_MESA_LIGHT_QUADRATIC;
light->radial = GAMGI_MESA_LIGHT_RADIAL;

light->draw = NULL;
}

/***** external function *****
 *                           *
 * GAMGI_ENGINE_START_LAYER  *
 *                           *
 * Initialise layer objects. *
 *                           *
 *****************************/

void gamgi_engine_start_layer (gamgi_layer *layer)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT layer, GAMGI_ENGINE_LAYER);
layer->object.number = gamgi_engine_number_layer_get ();

/********************
 * initialise layer *
 ********************/

layer->draw = NULL;
layer->undo = NULL;
layer->save = NULL;

/********
 * axis *
 ********/

layer->axes = FALSE;

/*******************************
 * initialise background color *
 *******************************/

layer->color[0] = 0.0;
layer->color[1] = 0.0;
layer->color[2] = 0.0;
layer->color[3] = 1.0;
}

/**** external function *******
 *                            *
 * GAMGI_ENGINE_START_WINDOW  *
 *                            *
 * Initialise window objects. *
 *                            *
 ******************************/

void gamgi_engine_start_window (gamgi_window *window)
{
/*********************
 * initialise object *
 *********************/

gamgi_engine_start_object (GAMGI_CAST_OBJECT window, GAMGI_ENGINE_WINDOW);
window->object.number = gamgi_engine_number_window_get ();

/*********************
 * initialise window *
 *********************/

window->draw = NULL;

window->window = NULL;
window->dialog0 = NULL;
window->dialog1 = NULL;
window->help = NULL;

window->area = NULL;
window->history = NULL;
window->layer = NULL;
window->focus = NULL;

window->action = FALSE;
window->mode = FALSE;
window->axes = FALSE;
window->class = FALSE;

window->top_flag = TRUE;
window->medium_flag = TRUE;
window->bottom_flag = TRUE;

window->timer_focus = 0;
window->timer_layer = 0;
window->timer_message = 0;
window->timer_progress = 0;

window->axis = FALSE;
window->total = 0.0;

window->demo_flag = 0;
}

/**** external function *******
 *                            *
 *  GAMGI_ENGINE_START_GAMGI  *
 *                            *
 * Initialise gamgi object.   *
 *                            *
 ******************************/

void gamgi_engine_start_gamgi (void)
{
/***************************************
 * Initialise GAMGI configuration data *
 ***************************************/

gamgi_global_start_gamgi (gamgi->gamgi);
gamgi_global_start_window (gamgi->window);
gamgi_global_start_layer (gamgi->layer);
gamgi_global_start_light (gamgi->light);
gamgi_global_start_assembly (gamgi->assembly);
gamgi_global_start_graph (gamgi->graph);
gamgi_global_start_shape (gamgi->shape);
gamgi_global_start_arrow (gamgi->arrow);
gamgi_global_start_cell (gamgi->cell);
gamgi_global_start_cluster (gamgi->cluster);
gamgi_global_start_molecule (gamgi->molecule);
gamgi_global_start_group (gamgi->group);
gamgi_global_start_plane (gamgi->plane);
gamgi_global_start_direction (gamgi->direction);
gamgi_global_start_atom (gamgi->atom);
gamgi_global_start_bond (gamgi->bond);
gamgi_global_start_orbital (gamgi->orbital);
gamgi_global_start_text (gamgi->text);
gamgi_global_start_help (gamgi->help);
}
