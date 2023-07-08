/*******************************************
 *
 * $GAMGI/src/global/gamgi_global_create.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU 
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"
#include "gamgi_io.h"

#include "gamgi_engine_start.h"

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_HELP              *
 *                                                         *
 * Create and initialise a (singleton) help class object.  *
 *                                                         *
 ***********************************************************/

gamgi_help_class *gamgi_global_create_help (void)
{
gamgi_help_class *help_class;

/*****************
 * create object *
 *****************/

help_class = (gamgi_help_class *) malloc (sizeof (gamgi_help_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT help_class, GAMGI_ENGINE_HELP);
help_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return help_class;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_CREATE_TEXT             *
 *                                                        *
 * Create and initialise a (singleton) text class object. *
 *                                                        *
 **********************************************************/

gamgi_text_class *gamgi_global_create_text (void)
{
gamgi_text_class *text_class;

/*****************
 * create object *
 *****************/

text_class = (gamgi_text_class *) malloc (sizeof (gamgi_text_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT text_class, GAMGI_ENGINE_TEXT);
text_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return text_class;
}

/************************ external function ******************
 *                                                           *
 *                   GAMGI_GLOBAL_CREATE_ORBITAL             *
 *                                                           *
 * Create and initialise a (singleton) orbital class object. *
 *                                                           *
 *************************************************************/

gamgi_orbital_class *gamgi_global_create_orbital (void)
{
gamgi_orbital_class *orbital_class;

/*****************
 * create object *
 *****************/

orbital_class = (gamgi_orbital_class *) malloc (sizeof (gamgi_orbital_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT orbital_class, GAMGI_ENGINE_ORBITAL);
orbital_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return orbital_class;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_CREATE_BOND             *
 *                                                        *
 * Create and initialise a (singleton) text class object. *
 *                                                        *
 **********************************************************/

gamgi_bond_class *gamgi_global_create_bond (void)
{
gamgi_bond_class *bond_class;
int bonds;

/*****************
 * create object *
 *****************/

bond_class = (gamgi_bond_class *) malloc (sizeof (gamgi_bond_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT bond_class, GAMGI_ENGINE_BOND);
bond_class->object.number = 0;

/*****************
 * create arrays *
 *****************/

bonds = GAMGI_CHEM_ATOM_MAX * (GAMGI_CHEM_ATOM_MAX + 1)/2;
bond_class->min = (double *) malloc (bonds * sizeof (double));
bond_class->max = (double *) malloc (bonds * sizeof (double));

/*******************
 * initialise data *
 *******************/

return bond_class;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_CREATE_ATOM             *
 *                                                        *
 * Create and initialise a (singleton) atom class object. *
 *                                                        *
 **********************************************************/

gamgi_atom_class *gamgi_global_create_atom (void)
{
gamgi_atom_class *atom_class;
int atoms;

/*****************
 * create object *
 *****************/

atom_class = (gamgi_atom_class *) malloc (sizeof (gamgi_atom_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT atom_class, GAMGI_ENGINE_ATOM);
atom_class->object.number = 0;

/*****************
 * create arrays *
 *****************/

atoms = GAMGI_CHEM_ATOM_MAX + 1;
atom_class->mass = (double *) malloc (atoms * sizeof (double));
atom_class->radius = (double *) malloc (atoms * sizeof (double));
atom_class->red = (float *) malloc (atoms * sizeof (float));
atom_class->green = (float *) malloc (atoms * sizeof (float));
atom_class->blue = (float *) malloc (atoms * sizeof (float));

return atom_class;
}

/************************** external function ******************
 *                                                             *
 *                   GAMGI_GLOBAL_CREATE_DIRECTION             *
 *                                                             *
 * Create and initialise a (singleton) direction class object. *
 *                                                             *
 ***************************************************************/

gamgi_direction_class *gamgi_global_create_direction (void)
{
gamgi_direction_class *direction_class;

/*****************
 * create object *
 *****************/

direction_class = (gamgi_direction_class *) malloc (sizeof (gamgi_direction_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT direction_class, GAMGI_ENGINE_DIRECTION);
direction_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return direction_class;
}

/*********************** external function *****************
 *                                                         *
 *                    GAMGI_GLOBAL_CREATE_PLANE            *
 *                                                         *
 * Create and initialise a (singleton) plane class object. *
 *                                                         *
 ***********************************************************/

gamgi_plane_class *gamgi_global_create_plane (void)
{
gamgi_plane_class *plane_class;

/*****************
 * create object *
 *****************/

plane_class = (gamgi_plane_class *) malloc (sizeof (gamgi_plane_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT plane_class, GAMGI_ENGINE_PLANE);
plane_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return plane_class;
}

/*********************** external function *****************
 *                                                         *
 *                    GAMGI_GLOBAL_CREATE_GROUP            *
 *                                                         *
 * Create and initialise a (singleton) group class object. *
 *                                                         *
 ***********************************************************/

gamgi_group_class *gamgi_global_create_group (void)
{
gamgi_group_class *group_class;

/*****************
 * create object *
 *****************/

group_class = (gamgi_group_class *) malloc (sizeof (gamgi_group_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT group_class, GAMGI_ENGINE_GROUP);
group_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return group_class;
}

/************************** external function *****************
 *                                                            *
 *                   GAMGI_GLOBAL_CREATE_MOLECULE             *
 *                                                            *
 * Create and initialise a (singleton) molecule class object. *
 *                                                            *
 **************************************************************/

gamgi_molecule_class *gamgi_global_create_molecule (void)
{
gamgi_molecule_class *molecule_class;

/*****************
 * create object *
 *****************/

molecule_class = (gamgi_molecule_class *) malloc (sizeof (gamgi_molecule_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT molecule_class, GAMGI_ENGINE_MOLECULE);
molecule_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return molecule_class;
}

/************************* external function *****************
 *                                                           *
 *                   GAMGI_GLOBAL_CREATE_CLUSTER             *
 *                                                           *
 * Create and initialise a (singleton) cluster class object. *
 *                                                           *
 *************************************************************/

gamgi_cluster_class *gamgi_global_create_cluster (void)
{
gamgi_cluster_class *cluster_class;

/*****************
 * create object *
 *****************/

cluster_class = (gamgi_cluster_class *) malloc (sizeof (gamgi_cluster_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT cluster_class, GAMGI_ENGINE_CLUSTER);
cluster_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return cluster_class;
}

/********************** external function *****************
 *                                                        *
 *                   GAMGI_GLOBAL_CREATE_CELL             *
 *                                                        *
 * Create and initialise a (singleton) cell class object. *
 *                                                        *
 **********************************************************/

gamgi_cell_class *gamgi_global_create_cell (void)
{
gamgi_cell_class *cell_class;

/*****************
 * create object *
 *****************/

cell_class = (gamgi_cell_class *) malloc (sizeof (gamgi_cell_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT cell_class, GAMGI_ENGINE_CELL);
cell_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return cell_class;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_ARROW             *
 *                                                         *
 * Create and initialise a (singleton) arrow class object. *
 *                                                         *
 ***********************************************************/

gamgi_arrow_class *gamgi_global_create_arrow (void)
{
gamgi_arrow_class *arrow_class;

/*****************
 * create object *
 *****************/

arrow_class = (gamgi_arrow_class *) malloc (sizeof (gamgi_arrow_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT arrow_class, GAMGI_ENGINE_ARROW);
arrow_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return arrow_class;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_SHAPE             *
 *                                                         *
 * Create and initialise a (singleton) shape class object. *
 *                                                         *
 ***********************************************************/

gamgi_shape_class *gamgi_global_create_shape (void)
{
gamgi_shape_class *shape_class;

/*****************
 * create object *
 *****************/

shape_class = (gamgi_shape_class *) malloc (sizeof (gamgi_shape_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT shape_class, GAMGI_ENGINE_SHAPE);
shape_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return shape_class;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_GRAPH             *
 *                                                         *
 * Create and initialise a (singleton) graph class object. *
 *                                                         *
 **********************************************************/

gamgi_graph_class *gamgi_global_create_graph (void)
{
gamgi_graph_class *graph_class;

/*****************
 * create object *
 *****************/

graph_class = (gamgi_graph_class *) malloc (sizeof (gamgi_graph_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT graph_class, GAMGI_ENGINE_GRAPH);
graph_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return graph_class;
}

/************************* external function ******************
 *                                                            *
 *                   GAMGI_GLOBAL_CREATE_ASSEMBLY             *
 *                                                            *
 * Create and initialise a (singleton) assembly class object. *
 *                                                            *
 **************************************************************/

gamgi_assembly_class *gamgi_global_create_assembly (void)
{
gamgi_assembly_class *assembly_class;

/*****************
 * create object *
 *****************/

assembly_class = (gamgi_assembly_class *) malloc (sizeof (gamgi_assembly_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT assembly_class, GAMGI_ENGINE_ASSEMBLY);
assembly_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return assembly_class;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_LIGHT             *
 *                                                         *
 * Create and initialise a (singleton) light class object. *
 *                                                         *
 ***********************************************************/

gamgi_light_class *gamgi_global_create_light (void)
{
gamgi_light_class *light_class;

/*****************
 * create object *
 *****************/

light_class = (gamgi_light_class *) malloc (sizeof (gamgi_light_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT light_class, GAMGI_ENGINE_LIGHT);
light_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return light_class;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_LAYER             *
 *                                                         *
 * Create and initialise a (singleton) layer class object. *
 *                                                         *
 ***********************************************************/

gamgi_layer_class *gamgi_global_create_layer (void)
{
gamgi_layer_class *layer_class;

/*****************
 * create object *
 *****************/

layer_class = (gamgi_layer_class *) malloc (sizeof (gamgi_layer_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT layer_class, GAMGI_ENGINE_LAYER);
layer_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return layer_class;
}

/************************ external function *****************
 *                                                          *
 *                  GAMGI_GLOBAL_CREATE_WINDOW              *
 *                                                          *
 * Create and initialise a (singleton) window class object. *
 *                                                          *
 ************************************************************/

gamgi_window_class *gamgi_global_create_window (void)
{
gamgi_window_class *window_class;

/*****************
 * create object *
 *****************/

window_class = (gamgi_window_class *) malloc (sizeof (gamgi_window_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT window_class, GAMGI_ENGINE_WINDOW);
window_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return window_class;
}

/********************** external function ******************
 *                                                         *
 *                   GAMGI_GLOBAL_CREATE_GAMGI             *
 *                                                         *
 * Create and initialise a (singleton) gamgi class object. *
 *                                                         *
 ***********************************************************/

gamgi_gamgi_class *gamgi_global_create_gamgi (void)
{
gamgi_gamgi_class *gamgi_class;

/*****************
 * create object *
 *****************/

gamgi_class = (gamgi_gamgi_class *) malloc (sizeof (gamgi_gamgi_class));
gamgi_engine_start_object (GAMGI_CAST_OBJECT gamgi_class, GAMGI_ENGINE_GAMGI);
gamgi_class->object.number = 0;

/*******************
 * initialise data *
 *******************/

return gamgi_class;
}

