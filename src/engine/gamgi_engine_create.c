/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_create.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU 
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_number.h"
#include "gamgi_mesa_text.h"
#include "gamgi_chem_atom.h"
#include "gamgi_global_create.h"

/***************** external function ********************
 *                                                      *
 *             GAMGI_ENGINE_TEXT_CREATE                 *
 *                                                      *
 * Creates an object describing a 2D or 3D text string. *
 *                                                      *
 ********************************************************/

gamgi_text *gamgi_engine_create_text (void)
{
gamgi_text *old_text = gamgi->text_end, *text;

/*******************
 * allocate memory *
 *******************/

text = GAMGI_CAST_TEXT malloc (sizeof (gamgi_text));

/*************************
 * initialise down links *
 *************************/

text->text_start = NULL;
text->text_end = NULL;

/********************
 * update text list *
 ********************/

gamgi->text_end = text;
text->before = old_text;
text->next = NULL;
if (old_text != NULL) old_text->next = text;
else gamgi->text_start = text;

return text;
}

/***************** external function *******************
 *                                                     *
 *            GAMGI_ENGINE_ORBITAL_CREATE              *
 *                                                     *
 * Creates an object describing a 3D chemical orbital. *
 *                                                     *
 *******************************************************/

gamgi_orbital *gamgi_engine_create_orbital (void)
{
gamgi_orbital *old_orbital = gamgi->orbital_end, *orbital;

/*******************
 * allocate memory *
 *******************/

orbital = GAMGI_CAST_ORBITAL malloc (sizeof (gamgi_orbital));

/*************************
 * initialise down links *
 *************************/

orbital->text_start = NULL;
orbital->text_end = NULL;

/***********************
 * update orbital list *
 ***********************/

gamgi->orbital_end = orbital;
orbital->before = old_orbital;
orbital->next = NULL;
if (old_orbital != NULL) old_orbital->next = orbital;
else gamgi->orbital_start = orbital;

return orbital;
}

/**************** external function *************
 *                                              *
 *            GAMGI_ENGINE_BOND_CREATE          *
 *                                              *
 * Creates an object describing a chemical bond *
 *                                              *
 ************************************************/

gamgi_bond *gamgi_engine_create_bond (void)
{
gamgi_bond *old_bond = gamgi->bond_end, *bond;

/*******************
 * allocate memory *
 *******************/

bond = GAMGI_CAST_BOND malloc (sizeof (gamgi_bond));

/*************************
 * initialise down links *
 *************************/

bond->text_start = NULL;
bond->text_end = NULL;
bond->orbital_start = NULL;
bond->orbital_end = NULL;

/********************
 * update bond list *
 ********************/

gamgi->bond_end = bond;
bond->before = old_bond;
bond->next = NULL;
if (old_bond != NULL) old_bond->next = bond;
else gamgi->bond_start = bond;

return bond;
}

/*********** external function ***********
 *                                       *
 *       GAMGI_ENGINE_ATOM_CREATE        *
 *                                       *
 * Creates an object describing an atom. *
 *                                       *
 *****************************************/

gamgi_atom *gamgi_engine_create_atom (void)
{
gamgi_atom *old_atom = gamgi->atom_end, *atom;

/*******************
 * allocate memory *
 *******************/

atom = GAMGI_CAST_ATOM malloc (sizeof (gamgi_atom));

/*************************
 * initialise down links *
 *************************/

atom->text_start = NULL;
atom->text_end = NULL; 
atom->orbital_start = NULL;
atom->orbital_end = NULL;
atom->bond_start = NULL;
atom->bond_end = NULL;

/********************
 * update atom list *
 ********************/

gamgi->atom_end = atom;
atom->before = old_atom;
atom->next = NULL;
if (old_atom != NULL) old_atom->next = atom;
else gamgi->atom_start = atom;

return atom;
}

/********************* external function **********************
 *                                                            *
 *              GAMGI_ENGINE_DIRECTION_CREATE                 *
 *                                                            *
 * Creates an object describing a crystallographic direction. *
 *                                                            *
 **************************************************************/

gamgi_direction *gamgi_engine_create_direction (void) 
{
gamgi_direction *old_direction = gamgi->direction_end, *direction;

/*******************
 * allocate memory *
 *******************/

direction = GAMGI_CAST_DIRECTION malloc (sizeof (gamgi_direction));

/*************************
 * initialise down links *
 *************************/

direction->text_start = NULL;
direction->text_end = NULL;
direction->orbital_start = NULL;
direction->orbital_end = NULL;
direction->bond_start = NULL;
direction->bond_end = NULL;
direction->atom_start = NULL;
direction->atom_end = NULL;

/*************************
 * update direction list *
 *************************/

gamgi->direction_end = direction;
direction->before = old_direction;
direction->next = NULL;
if (old_direction != NULL) old_direction->next = direction;
else gamgi->direction_start = direction;

return direction;
}

/********************* external function ******************
 *                                                        *
 *                 GAMGI_ENGINE_PLANE_CREATE              *
 *                                                        *
 * Creates an object describing a crystallographic plane. *
 *                                                        *
 **********************************************************/

gamgi_plane *gamgi_engine_create_plane (void)
{
gamgi_plane *old_plane = gamgi->plane_end, *plane;

/*******************
 * allocate memory *
 *******************/

plane = GAMGI_CAST_PLANE malloc (sizeof (gamgi_plane));

/*************************
 * initialise down links *
 *************************/

plane->text_start = NULL;
plane->text_end = NULL;
plane->orbital_start = NULL;
plane->orbital_end = NULL;
plane->atom_start = NULL;
plane->atom_end = NULL;
plane->bond_start = NULL;
plane->bond_end = NULL;
plane->direction_start = NULL;
plane->direction_end = NULL;

/*********************
 * update plane list *
 *********************/

gamgi->plane_end = plane;
plane->before = old_plane;
plane->next = NULL;
if (old_plane != NULL) old_plane->next = plane;
else gamgi->plane_start = plane;

return plane;
}

/***************** external function **************
 *                                                *
 *             GAMGI_ENGINE_GROUP_CREATE          *
 *                                                *
 * Creates an object describing a group of atoms. *
 *                                                *
 **************************************************/

gamgi_group *gamgi_engine_create_group (void)
{
gamgi_group *old_group = gamgi->group_end, *group;

/*******************
 * allocate memory *
 *******************/

group = GAMGI_CAST_GROUP malloc (sizeof (gamgi_group));

/*************************
 * initialise down links *
 *************************/

group->text_start = NULL;
group->text_end = NULL;
group->orbital_start = NULL;
group->orbital_end = NULL;
group->bond_start = NULL;
group->bond_end = NULL;
group->atom_start = NULL;
group->atom_end = NULL;
group->direction_start = NULL;
group->direction_end = NULL;
group->plane_start = NULL;
group->plane_end = NULL;
group->group_start = NULL;
group->group_end = NULL;

/*********************
 * update group list *
 *********************/

gamgi->group_end = group;
group->before = old_group;
group->next = NULL;
if (old_group != NULL) old_group->next = group;
else gamgi->group_start = group;

return group;
}

/************* external function ************
 *                                          *
 *        GAMGI_ENGINE_MOLECULE_CREATE      *
 *                                          *
 * Creates an object describing a molecule. *
 *                                          *
 ********************************************/

gamgi_molecule *gamgi_engine_create_molecule (void)
{
gamgi_molecule *old_molecule = gamgi->molecule_end, *molecule;

/*******************
 * allocate memory *
 *******************/

molecule = GAMGI_CAST_MOLECULE malloc (sizeof (gamgi_molecule));

/*************************
 * initialise down links *
 *************************/

molecule->text_start = NULL;
molecule->text_end = NULL;
molecule->orbital_start = NULL;
molecule->orbital_end = NULL;
molecule->bond_start = NULL;
molecule->bond_end = NULL;
molecule->atom_start = NULL;
molecule->atom_end = NULL;
molecule->direction_start = NULL;
molecule->direction_end = NULL;
molecule->plane_start = NULL;
molecule->plane_end = NULL;
molecule->group_start = NULL;
molecule->group_end = NULL;

/************************
 * update molecule list *
 ************************/

gamgi->molecule_end = molecule;
molecule->before = old_molecule;
molecule->next = NULL;
if (old_molecule != NULL) old_molecule->next = molecule;
else gamgi->molecule_start = molecule;

return molecule;
}

/******************* external function ******************
 *                                                      *
 *             GAMGI_ENGINE_CLUSTER_CREATE              *
 *                                                      *
 * Creates an object describing a cluster of molecules. *
 *                                                      *
 ********************************************************/

gamgi_cluster *gamgi_engine_create_cluster (void)
{
gamgi_cluster *old_cluster = gamgi->cluster_end, *cluster;

/*******************
 * allocate memory *
 *******************/

cluster = GAMGI_CAST_CLUSTER malloc (sizeof (gamgi_cluster));

/*************************
 * initialise down links *
 *************************/

cluster->text_start = NULL;
cluster->text_end = NULL;
cluster->orbital_start = NULL;
cluster->orbital_end = NULL;
cluster->bond_start = NULL;
cluster->bond_end = NULL;
cluster->atom_start = NULL;
cluster->atom_end = NULL;
cluster->direction_start = NULL;
cluster->direction_end = NULL;
cluster->plane_start = NULL;
cluster->plane_end = NULL;
cluster->group_start = NULL;
cluster->group_end = NULL;
cluster->molecule_start = NULL;
cluster->molecule_end = NULL;
cluster->cluster_start = NULL;
cluster->cluster_end = NULL;

/***********************
 * update cluster list *
 ***********************/

gamgi->cluster_end = cluster;
cluster->before = old_cluster;
cluster->next = NULL;
if (old_cluster != NULL) old_cluster->next = cluster;
else gamgi->cluster_start = cluster;

return cluster;
}

/***************** external function *************
 *                                               *
 *            GAMGI_ENGINE_CELL_CREATE           *
 *                                               *
 * Creates an object describing a periodic cell. *
 *                                               *
 *************************************************/

gamgi_cell *gamgi_engine_create_cell (void)
{
gamgi_cell *old_cell = gamgi->cell_end, *cell;

/*******************
 * allocate memory *
 *******************/

cell = GAMGI_CAST_CELL malloc (sizeof (gamgi_cell));

/*************************
 * initialise down links *
 *************************/

cell->text_start = NULL;
cell->text_end = NULL;
cell->orbital_start = NULL;
cell->orbital_end = NULL;
cell->bond_start = NULL;
cell->bond_end = NULL;
cell->atom_start = NULL;
cell->atom_end = NULL;
cell->direction_start = NULL;
cell->direction_end = NULL;
cell->plane_start = NULL;
cell->plane_end = NULL;
cell->group_start = NULL;
cell->group_end = NULL;
cell->molecule_start = NULL;
cell->molecule_end = NULL;
cell->cluster_start = NULL;
cell->cluster_end = NULL;

/********************
 * update cell list *
 ********************/

gamgi->cell_end = cell;
cell->before = old_cell;
cell->next = NULL;
if (old_cell != NULL) old_cell->next = cell;
else gamgi->cell_start = cell;

return cell;
}

/******************** external function *******************
 *                                                        *
 *                GAMGI_ENGINE_ARROW_CREATE               *
 *                                                        *
 * Creates an object describing a single or double arrow. *
 *                                                        *
 **********************************************************/

gamgi_arrow *gamgi_engine_create_arrow (void)
{
gamgi_arrow *old_arrow = gamgi->arrow_end, *arrow;

/*******************
 * allocate memory *
 *******************/

arrow = GAMGI_CAST_ARROW malloc (sizeof (gamgi_arrow));

/*************************
 * initialise down links *
 *************************/

arrow->text_start = NULL;
arrow->text_end = NULL;

/*********************
 * update arrow list *
 *********************/

gamgi->arrow_end = arrow;
arrow->before = old_arrow;
arrow->next = NULL;
if (old_arrow != NULL) old_arrow->next = arrow;
else gamgi->arrow_start = arrow;

return arrow;
}

/******************** external function ********************
 *                                                         *
 *                GAMGI_ENGINE_SHAPE_CREATE                *
 *                                                         *
 * Creates an object describing a contoured or full shape. *
 *                                                         *
 ***********************************************************/

gamgi_shape *gamgi_engine_create_shape (void)
{
gamgi_shape *old_shape = gamgi->shape_end, *shape;

/*******************
 * allocate memory *
 *******************/

shape = GAMGI_CAST_SHAPE malloc (sizeof (gamgi_shape));

/*************************
 * initialise down links *
 *************************/

shape->text_start = NULL;
shape->text_end = NULL;

/*********************
 * update shape list *
 *********************/

gamgi->shape_end = shape;
shape->before = old_shape;
shape->next = NULL;
if (old_shape != NULL) old_shape->next = shape;
else gamgi->shape_start = shape;

return shape;
}

/****************** external function *************
 *                                                *
 *             GAMGI_ENGINE_GRAPH_CREATE          *
 *                                                *
 * Creates an object describing a 2D or 3D graph. *
 *                                                *
 **************************************************/

gamgi_graph *gamgi_engine_create_graph (void)
{
gamgi_graph *old_graph = gamgi->graph_end, *graph;

/*******************
 * allocate memory *
 *******************/

graph = GAMGI_CAST_GRAPH malloc (sizeof (gamgi_graph));

/*************************
 * initialise down links *
 *************************/

graph->text_start = NULL;
graph->text_end = NULL;

/*********************
 * update graph list *
 *********************/

gamgi->graph_end = graph;
graph->before = old_graph;
graph->next = NULL;
if (old_graph != NULL) old_graph->next = graph;
else gamgi->graph_start = graph;

return graph;
}

/******************** external function *****************
 *                                                      *
 *              GAMGI_ENGINE_ASSEMBLY_CREATE            *
 *                                                      *
 * Creates an object describing an assembly of objects. *
 *                                                      *
 ********************************************************/

gamgi_assembly *gamgi_engine_create_assembly (void)
{
gamgi_assembly *old_assembly = gamgi->assembly_end, *assembly;

/*******************
 * allocate memory *
 *******************/

assembly = GAMGI_CAST_ASSEMBLY malloc (sizeof (gamgi_assembly));

/*************************
 * initialise down links *
 *************************/

assembly->text_start = NULL;
assembly->text_end = NULL;
assembly->orbital_start = NULL;
assembly->orbital_end = NULL;
assembly->bond_start = NULL;
assembly->bond_end = NULL;
assembly->atom_start = NULL;
assembly->atom_end = NULL;
assembly->direction_start = NULL;
assembly->direction_end = NULL;
assembly->plane_start = NULL;
assembly->plane_end = NULL;
assembly->group_start = NULL;
assembly->group_end = NULL;
assembly->molecule_start = NULL;
assembly->molecule_end = NULL;
assembly->cluster_start = NULL;
assembly->cluster_end = NULL;
assembly->cell_start = NULL;
assembly->cell_end = NULL;
assembly->arrow_start = NULL;
assembly->arrow_end = NULL;
assembly->shape_start = NULL;
assembly->shape_end = NULL;
assembly->graph_start = NULL;
assembly->graph_end = NULL;
assembly->assembly_start = NULL;
assembly->assembly_end = NULL;

/************************
 * update assembly list *
 ************************/

gamgi->assembly_end = assembly;
assembly->before = old_assembly;
assembly->next = NULL;
if (old_assembly != NULL) old_assembly->next = assembly;
else gamgi->assembly_start = assembly;

return assembly;
}

/**************** external function **************
 *                                               *
 *            GAMGI_ENGINE_LIGHT_CREATE          *
 *                                               *
 * Creates an object describing an OpenGL light. *
 *                                               *
 *************************************************/

gamgi_light *gamgi_engine_create_light (void)
{
gamgi_light *old_light = gamgi->light_end, *light;

/*******************
 * allocate memory *
 *******************/

light = GAMGI_CAST_LIGHT malloc (sizeof (gamgi_light));

/*********************
 * update light list *
 *********************/

gamgi->light_end = light;
light->before = old_light;
light->next = NULL;
if (old_light != NULL) old_light->next = light;
else gamgi->light_start = light;

return light;
}

/*********************** external function *********************
 *                                                             *
 *                   GAMGI_ENGINE_CREATE_LAYER                 *
 *                                                             *
 * Creates an object describing a transparent or opaque layer. *
 *                                                             *
 ***************************************************************/

gamgi_layer *gamgi_engine_create_layer (void)
{
gamgi_layer *old_layer = gamgi->layer_end, *layer;

/*******************
 * allocate memory *
 *******************/

layer = GAMGI_CAST_LAYER malloc (sizeof (gamgi_layer));

/*************************
 * initialise down links *
 *************************/

layer->text_start = NULL;
layer->text_end = NULL;
layer->orbital_start = NULL;
layer->orbital_end = NULL;
layer->bond_start = NULL;
layer->bond_end = NULL;
layer->atom_start = NULL;
layer->atom_end = NULL;
layer->direction_start = NULL;
layer->direction_end = NULL;
layer->plane_start = NULL;
layer->plane_end = NULL;
layer->group_start = NULL;
layer->group_end = NULL;
layer->molecule_start = NULL;
layer->molecule_end = NULL;
layer->cluster_start = NULL;
layer->cluster_end = NULL;
layer->cell_start = NULL; 
layer->cell_end = NULL; 
layer->arrow_start = NULL;
layer->arrow_end = NULL;
layer->shape_start = NULL;
layer->shape_end = NULL;
layer->graph_start = NULL;
layer->graph_end = NULL;
layer->assembly_start = NULL;
layer->assembly_end = NULL;
layer->light_start = NULL;
layer->light_end = NULL;

/*********************
 * update layer list *
 *********************/

gamgi->layer_end = layer;
layer->before = old_layer;
layer->next = NULL;
if (old_layer != NULL) old_layer->next = layer;
else gamgi->layer_start = layer;

return layer;
}

/**************** external function ***************
 *                                                *
 *            GAMGI_ENGINE_WINDOW_CREATE          *
 *                                                *
 * Creates an object describing a GTK top window. *
 *                                                *
 **************************************************/

gamgi_window *gamgi_engine_create_window (void)
{
gamgi_window *old_window = gamgi->window_end, *window;

/*******************
 * allocate memory *
 *******************/

window = GAMGI_CAST_WINDOW malloc (sizeof (gamgi_window));

/*************************
 * initialise down links *
 *************************/

window->layer_start = NULL;
window->layer_end = NULL;

/**********************
 * update window list *
 **********************/

gamgi->window_end = window;
window->before = old_window;
window->next = NULL;
if (old_window != NULL) old_window->next = window;
else gamgi->window_start = window;

return window;
}

/*********************** external function *******************
 *                                                           *
 *                  GAMGI_ENGINE_CREATE_GAMGI                *
 *                                                           *
 * Creates an object to allocate the GAMGI system resources. *
 *                                                           *
 *************************************************************/

void gamgi_engine_create_gamgi ()
{
/**************************************************
 * gamgi is the only global variable defined in   *
 * GAMGI. Its purpose is to contain the global    *
 * resources used by the program. Every data      *
 * available in GAMGI can be obtained from gamgi. *
 **************************************************/

gamgi = GAMGI_CAST_GAMGI malloc (sizeof (gamgi_gamgi));

/*******************************
 * create hash table to speed  *
 * up atom element recognition *
 *******************************/

gamgi->element = gamgi_chem_atom_element ();

/****************************
 * create font glyph arrays *
 ****************************/

gamgi_mesa_text_font_start ();

/**********************************
 * initialize pointers to windows *
 **********************************/

gamgi->message = NULL;
gamgi->question = NULL;

/**************************************
 * create configuration class objects *
 **************************************/

gamgi->help = gamgi_global_create_help ();
gamgi->text = gamgi_global_create_text ();
gamgi->orbital = gamgi_global_create_orbital ();
gamgi->bond = gamgi_global_create_bond ();
gamgi->atom = gamgi_global_create_atom ();
gamgi->direction = gamgi_global_create_direction ();
gamgi->plane = gamgi_global_create_plane ();
gamgi->group = gamgi_global_create_group ();
gamgi->molecule = gamgi_global_create_molecule ();
gamgi->cluster = gamgi_global_create_cluster ();
gamgi->cell = gamgi_global_create_cell ();
gamgi->arrow = gamgi_global_create_arrow ();
gamgi->shape = gamgi_global_create_shape ();
gamgi->graph = gamgi_global_create_graph ();
gamgi->assembly = gamgi_global_create_assembly ();
gamgi->light = gamgi_global_create_light ();
gamgi->layer = gamgi_global_create_layer ();
gamgi->window = gamgi_global_create_window ();
gamgi->gamgi = gamgi_global_create_gamgi ();

/***************************************************
 * initialize object selection tools, lists, focus *
 ***************************************************/

gamgi->function = NULL;
gamgi->window_dialog = NULL;
gamgi->window_mouse = NULL;
gamgi->state = FALSE;

gamgi->texts = NULL;
gamgi->orbitals = NULL;
gamgi->bonds = NULL;
gamgi->atoms = NULL;
gamgi->directions = NULL;
gamgi->planes = NULL;
gamgi->groups = NULL;
gamgi->molecules = NULL;
gamgi->clusters = NULL;
gamgi->cells = NULL;
gamgi->arrows = NULL;
gamgi->shapes = NULL;
gamgi->graphs = NULL;
gamgi->assemblys = NULL;
gamgi->lights = NULL;
gamgi->layers = NULL;
gamgi->windows = NULL;

gamgi->focus = FALSE;

/**********************************
 * initialise pointers to objects *
 **********************************/

gamgi->text_start = NULL;
gamgi->text_end = NULL;
gamgi->orbital_start = NULL;
gamgi->orbital_end = NULL;
gamgi->bond_start = NULL;
gamgi->bond_end = NULL;
gamgi->atom_start = NULL;
gamgi->atom_end = NULL;
gamgi->direction_start = NULL;
gamgi->direction_end = NULL;
gamgi->plane_start = NULL;
gamgi->plane_end = NULL;
gamgi->group_start = NULL;
gamgi->group_end = NULL;
gamgi->molecule_start = NULL;
gamgi->molecule_end = NULL;
gamgi->cluster_start = NULL;
gamgi->cluster_end = NULL;
gamgi->cell_start = NULL; 
gamgi->cell_end = NULL;
gamgi->arrow_start = NULL;
gamgi->arrow_end = NULL;
gamgi->shape_start = NULL;
gamgi->shape_end = NULL;
gamgi->graph_start = NULL;
gamgi->graph_end = NULL;
gamgi->assembly_start = NULL;
gamgi->assembly_end = NULL;
gamgi->light_start = NULL;
gamgi->light_end = NULL;
gamgi->layer_start = NULL;
gamgi->layer_end = NULL;
gamgi->window_start = NULL;
gamgi->window_end = NULL;
}
