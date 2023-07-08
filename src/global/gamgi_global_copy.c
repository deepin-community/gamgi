/******************************************
 *
 * $GAMGI/src/global/gamgi_global_copy.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU 
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_mesa_text.h"
#include "gamgi_math_vector.h"
#include "gamgi_io_token.h"
#include "gamgi_global_create.h"

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_COPY_HELP               *
 *                                                        *
 *                                                        *
 **********************************************************/

gamgi_help_class *gamgi_global_copy_help (gamgi_help_class *help_class)
{
gamgi_help_class *help_class_new;

/***********************
 * create class object *
 ***********************/

help_class_new = gamgi_global_create_help ();

/**********************
 * copy configuration *
 **********************/

help_class_new->source_in = help_class->source_in;
help_class_new->agent_in = help_class->agent_in;

/*************************************
 * Copy path for local documentation *
 *************************************/

gamgi_io_token_create (help_class->local, &help_class_new->local);

/**************************************
 * Copy path for remote documentation *
 **************************************/

gamgi_io_token_create (help_class->remote, &help_class_new->remote);

/**********************************
 * Copy path for prefered browser *
 **********************************/

gamgi_io_token_create (help_class->browser, &help_class_new->browser);

return help_class_new;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_COPY_TEXT               *
 *                                                        *
 *                                                        *
 **********************************************************/

gamgi_text_class *gamgi_global_copy_text (gamgi_text_class *text_class)
{
gamgi_text_class *text_class_new;

/***********************
 * create class object *
 ***********************/

text_class_new = gamgi_global_create_text ();

/**********************
 * copy configuration *
 **********************/

text_class_new->style = text_class->style;
text_class_new->wired = text_class->wired;
text_class_new->solid = text_class->solid;

text_class_new->red = text_class->red;
text_class_new->green = text_class->green;
text_class_new->blue = text_class->blue;

/*****************************
 * Copy path for local fonts *
 *****************************/

gamgi_io_token_create (text_class->path, &text_class_new->path);

return text_class_new;
}

/************************ external function ******************
 *                                                           *
 *                   GAMGI_GLOBAL_COPY_ORBITAL               *
 *                                                           *
 *                                                           *
 *************************************************************/

gamgi_orbital_class *gamgi_global_copy_orbital (gamgi_orbital_class *orbital_class)
{
gamgi_orbital_class *orbital_class_new;

/***********************
 * create class object *
 ***********************/

orbital_class_new = gamgi_global_create_orbital ();

/**********************
 * copy configuration *
 **********************/

return orbital_class_new;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_COPY_BOND               *
 *                                                        *
 *                                                        *
 **********************************************************/

gamgi_bond_class *gamgi_global_copy_bond (gamgi_bond_class *bond_class)
{
gamgi_bond_class *bond_class_new;
int bond;

/***********************
 * create class object *
 ***********************/

bond_class_new = gamgi_global_create_bond ();

/**********************
 * copy configuration *
 **********************/

bond_class_new->draw = bond_class->draw;
bond_class_new->size = bond_class->size;

bond_class_new->color = bond_class->color;
bond_class_new->red = bond_class->red;
bond_class_new->green = bond_class->green;
bond_class_new->blue = bond_class->blue;

bond_class_new->lower = bond_class->lower;
bond_class_new->upper = bond_class->upper;

/***********************************************************
 * copy arrays with element-element specific configuration *
 ***********************************************************/

bond = GAMGI_CHEM_ATOM_MAX * (GAMGI_CHEM_ATOM_MAX + 1)/2;
while (bond-- > 0)
  {
  bond_class_new->min[bond] = bond_class->min[bond];
  bond_class_new->max[bond] = bond_class->max[bond];
  }

return bond_class_new;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_COPY_ATOM               *
 *                                                        *
 *                                                        *
 **********************************************************/

gamgi_atom_class *gamgi_global_copy_atom (gamgi_atom_class *atom_class)
{
gamgi_atom_class *atom_class_new;
int atom;

/***********************
 * create class object *
 ***********************/

atom_class_new = gamgi_global_create_atom ();

/**********************
 * copy configuration *
 **********************/

atom_class_new->temperature = atom_class->temperature;

atom_class_new->draw = atom_class->draw;
atom_class_new->variancy = atom_class->variancy;
atom_class_new->size = atom_class->size;
atom_class_new->min = atom_class->min;

/***************************************************
 * copy arrays with element specific configuration *
 *                                                 *
 *    array[0] is used to configure dummy atoms    *
 ***************************************************/

atom = GAMGI_CHEM_ATOM_MAX + 1;
while (atom-- > 0)
  {
  atom_class_new->mass[atom] = atom_class->mass[atom];
  atom_class_new->radius[atom] = atom_class->radius[atom];
  atom_class_new->red[atom] = atom_class->red[atom];
  atom_class_new->green[atom] = atom_class->green[atom];
  atom_class_new->blue[atom] = atom_class->blue[atom];
  }

/********************
 * copy global data *
 ********************/

atom_class_new->slices = atom_class->slices;
atom_class_new->width = atom_class->width;

return atom_class_new;
}

/************************** external function ******************
 *                                                             *
 *                   GAMGI_GLOBAL_COPY_DIRECTION               *
 *                                                             *
 *                                                             *
 ***************************************************************/

gamgi_direction_class *gamgi_global_copy_direction (gamgi_direction_class *direction_class)
{
gamgi_direction_class *direction_class_new;

/***********************
 * create class object *
 ***********************/

direction_class_new = gamgi_global_create_direction ();

/**********************
 * copy configuration *
 **********************/

return direction_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                    GAMGI_GLOBAL_COPY_PLANE              *
 *                                                         *
 *                                                         *
 ***********************************************************/

gamgi_plane_class *gamgi_global_copy_plane (gamgi_plane_class *plane_class)
{
gamgi_plane_class *plane_class_new;

/***********************
 * create class object *
 ***********************/

plane_class_new = gamgi_global_create_plane ();

/**********************
 * copy configuration *
 **********************/

return plane_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                    GAMGI_GLOBAL_COPY_GROUP              *
 *                                                         *
 *                                                         *
 ***********************************************************/

gamgi_group_class *gamgi_global_copy_group (gamgi_group_class *group_class)
{
gamgi_group_class *group_class_new;

/***********************
 * create class object *
 ***********************/

group_class_new = gamgi_global_create_group ();

/**********************
 * copy configuration *
 **********************/

return group_class_new;
}

/************************** external function *****************
 *                                                            *
 *                   GAMGI_GLOBAL_COPY_MOLECULE               *
 *                                                            *
 *                                                            *
 **************************************************************/

gamgi_molecule_class *gamgi_global_copy_molecule (gamgi_molecule_class *molecule_class)
{
gamgi_molecule_class *molecule_class_new;

/***********************
 * create class object *
 ***********************/

molecule_class_new = gamgi_global_create_molecule ();

/**********************
 * copy configuration *
 **********************/

return molecule_class_new;
}

/************************* external function *****************
 *                                                           *
 *                   GAMGI_GLOBAL_COPY_CLUSTER               *
 *                                                           *
 *                                                           *
 *************************************************************/

gamgi_cluster_class *gamgi_global_copy_cluster (gamgi_cluster_class *cluster_class)
{
gamgi_cluster_class *cluster_class_new;

/***********************
 * create class object *
 ***********************/

cluster_class_new = gamgi_global_create_cluster ();

/**********************
 * copy configuration *
 **********************/

return cluster_class_new;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_COPY_CELL               *
 *                                                        *
 *                                                        *
 **********************************************************/

gamgi_cell_class *gamgi_global_copy_cell (gamgi_cell_class *cell_class)
{
gamgi_cell_class *cell_class_new;

/***********************
 * create class object *
 ***********************/

cell_class_new = gamgi_global_create_cell ();

/**********************
 * copy configuration *
 **********************/

cell_class_new->model = cell_class->model;
cell_class_new->axes = cell_class->axes;

cell_class_new->borders = cell_class->borders;
cell_class_new->faces = cell_class->faces;
cell_class_new->nodes = cell_class->nodes;

cell_class_new->red = cell_class->red;
cell_class_new->green = cell_class->green;
cell_class_new->blue = cell_class->blue;

/********************
 * copy global data *
 ********************/

cell_class_new->axes_size = cell_class->axes_size;
cell_class_new->borders_width = cell_class->borders_width;
cell_class_new->nodes_width = cell_class->nodes_width;

return cell_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_ARROW               *
 *                                                         *
 ***********************************************************/

gamgi_arrow_class *gamgi_global_copy_arrow (gamgi_arrow_class *arrow_class)
{
gamgi_arrow_class *arrow_class_new;

/***********************
 * create class object *
 ***********************/

arrow_class_new = gamgi_global_create_arrow ();

/**********************
 * copy configuration *
 **********************/

return arrow_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_SHAPE               *
 *                                                         *
 * Create and initialise a (singleton) shape class object. *
 *                                                         *
 ***********************************************************/

gamgi_shape_class *gamgi_global_copy_shape (gamgi_shape_class *shape_class)
{
gamgi_shape_class *shape_class_new;

/***********************
 * create class object *
 ***********************/

shape_class_new = gamgi_global_create_shape ();

/**********************
 * copy configuration *
 **********************/

return shape_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_GRAPH               *
 *                                                         *
 * Create and initialise a (singleton) graph class object. *
 *                                                         *
 **********************************************************/

gamgi_graph_class *gamgi_global_copy_graph (gamgi_graph_class *graph_class)
{
gamgi_graph_class *graph_class_new;

/***********************
 * create class object *
 ***********************/

graph_class_new = gamgi_global_create_graph ();

/**********************
 * copy configuration *
 **********************/

return graph_class_new;
}

/************************* external function ******************
 *                                                            *
 *                   GAMGI_GLOBAL_COPY_ASSEMBLY               *
 *                                                            *
 *                                                            *
 **************************************************************/

gamgi_assembly_class *gamgi_global_copy_assembly (gamgi_assembly_class *assembly_class)
{
gamgi_assembly_class *assembly_class_new;

/***********************
 * create class object *
 ***********************/

assembly_class_new = gamgi_global_create_assembly ();

/**********************
 * copy configuration *
 **********************/

return assembly_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_LIGHT               *
 *                                                         *
 *                                                         *
 ***********************************************************/

gamgi_light_class *gamgi_global_copy_light (gamgi_light_class *light_class)
{
gamgi_light_class *light_class_new;

/***********************
 * create class object *
 ***********************/

light_class_new = gamgi_global_create_light ();

/**********************
 * copy configuration *
 **********************/

light_class_new->ambient[0] = light_class->ambient[0];
light_class_new->ambient[1] = light_class->ambient[1];
light_class_new->ambient[2] = light_class->ambient[2];

light_class_new->diffuse[0] = light_class->diffuse[0];
light_class_new->diffuse[1] = light_class->diffuse[1];
light_class_new->diffuse[2] = light_class->diffuse[2];

light_class_new->specular[0] = light_class->specular[0];
light_class_new->specular[1] = light_class->specular[1];
light_class_new->specular[2] = light_class->specular[2];

/********************
 * copy global data *
 ********************/

light_class_new->shininess = light_class->shininess;

return light_class_new;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_LAYER               *
 *                                                         *
 *                                                         *
 ***********************************************************/

gamgi_layer_class *gamgi_global_copy_layer (gamgi_layer_class *layer_class)
{
gamgi_layer_class *layer_class_new;

/***********************
 * create class object *
 ***********************/

layer_class_new = gamgi_global_create_layer ();

/**********************
 * copy configuration *
 **********************/

layer_class_new->undo = layer_class->undo;
layer_class_new->save = layer_class->save;

layer_class_new->perspective = layer_class->perspective;

layer_class_new->top = layer_class->top;
layer_class_new->near = layer_class->near;
layer_class_new->far = layer_class->far;

gamgi_math_vector_copy (layer_class->eye, layer_class_new->eye);
gamgi_math_vector_copy (layer_class->center, layer_class_new->center);
gamgi_math_vector_copy (layer_class->up, layer_class_new->up);

return layer_class_new;
}

/************************ external function *****************
 *                                                          *
 *                  GAMGI_GLOBAL_COPY_WINDOW                *
 *                                                          *
 * Create and initialise a (singleton) window class object. *
 *                                                          *
 ************************************************************/

gamgi_window_class *gamgi_global_copy_window (gamgi_window_class *window_class)
{
gamgi_window_class *window_class_new;

/***********************
 * create class object *
 ***********************/

window_class_new = gamgi_global_create_window ();

/**********************
 * copy configuration *
 **********************/

window_class_new->rotate_max = window_class->rotate_max;
window_class_new->rotate_step = window_class->rotate_step;

window_class_new->move_max = window_class->move_max;
window_class_new->move_step = window_class->move_step;

window_class_new->scale_max = window_class->scale_max;
window_class_new->scale_step = window_class->scale_step;

return window_class_new;
}

/********************** external function ******************
 *                                                         *
 *                   GAMGI_GLOBAL_COPY_GAMGI               *
 *                                                         *
 *                                                         *
 ***********************************************************/

gamgi_gamgi_class *gamgi_global_copy_gamgi (gamgi_gamgi_class *gamgi_class)
{
gamgi_gamgi_class *gamgi_class_new;

/***********************
 * create class object *
 ***********************/

gamgi_class_new = gamgi_global_create_gamgi ();

/**********************
 * copy configuration *
 **********************/

/*********
 * sound *
 *********/

gamgi_class_new->beep = gamgi_class->beep;

/******************
 * pick tolerance *
 ******************/

gamgi_class_new->tolerance = gamgi_class->tolerance;

/****************
 * button order *
 ****************/

gamgi_class_new->button1 = gamgi_class->button1;
gamgi_class_new->button2 = gamgi_class->button2;
gamgi_class_new->button3 = gamgi_class->button3;

/*****************************
 * number of decimal figures *
 *****************************/

gamgi_class_new->length = gamgi_class->length;
gamgi_class_new->angle = gamgi_class->angle;
gamgi_class_new->charge = gamgi_class->charge;
gamgi_class_new->mass = gamgi_class->mass;

/**********************
 * colors in gtk_text *
 **********************/

gamgi_class_new->background[0] = gamgi_class->background[0];
gamgi_class_new->background[1] = gamgi_class->background[1];
gamgi_class_new->background[2] = gamgi_class->background[2];

gamgi_class_new->foreground[0] = gamgi_class->foreground[0];
gamgi_class_new->foreground[1] = gamgi_class->foreground[1];
gamgi_class_new->foreground[2] = gamgi_class->foreground[2];

gamgi_class_new->title[0] = gamgi_class->title[0];
gamgi_class_new->title[1] = gamgi_class->title[1];
gamgi_class_new->title[2] = gamgi_class->title[2];

gamgi_class_new->bold[0] = gamgi_class->bold[0];
gamgi_class_new->bold[1] = gamgi_class->bold[1];
gamgi_class_new->bold[2] = gamgi_class->bold[2];

gamgi_class_new->link[0] = gamgi_class->link[0];
gamgi_class_new->link[1] = gamgi_class->link[1];
gamgi_class_new->link[2] = gamgi_class->link[2];

return gamgi_class_new;
}
