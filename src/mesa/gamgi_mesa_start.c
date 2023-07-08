/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_start.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_mesa_bond.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_phys_projection.h"

/***** external function *****
 *                           *
 *   GAMGI_MESA_START_TEXT   *
 *                           *
 * Default for text objects. *
 *                           *
 *****************************/

void gamgi_mesa_start_text (gamgi_text *text, gamgi_text_class *text_class)
{
strcpy (text->object.name, "Text");

/*********
 * style *
 *********/

text->style = text_class->style;

/*********
 * color *
 *********/

text->red = text_class->red;
text->green = text_class->green;
text->blue = text_class->blue;
}

/******* external function ******
 *                              *
 *    GAMGI_MESA_START_ORBITAL  *
 *                              *
 * Default for orbital objects. *
 *                              *
 ********************************/

void gamgi_mesa_start_orbital (gamgi_orbital *orbital, gamgi_orbital_class *orbital_class)
{
int i;

orbital->charge = GAMGI_CHEM_ORBITAL_CHARGE;
orbital->density = GAMGI_CHEM_ORBITAL_DENSITY;
orbital->style = GAMGI_MESA_ORBITAL_STYLE;
orbital->seed = GAMGI_MATH_SEED;

orbital->phase = TRUE;
orbital->frame = TRUE;
orbital->axes = GAMGI_CHEM_NONE;
for (i = 0; i < 8; i++)
  orbital->octants[i] = TRUE;

orbital->base_r = GAMGI_MESA_ORBITAL_BASE_R;
orbital->base_g = GAMGI_MESA_ORBITAL_BASE_G;
orbital->base_b = GAMGI_MESA_ORBITAL_BASE_B;
orbital->phase_r = GAMGI_MESA_ORBITAL_PHASE_R;
orbital->phase_g = GAMGI_MESA_ORBITAL_PHASE_G;
orbital->phase_b = GAMGI_MESA_ORBITAL_PHASE_B;
orbital->frame_r = GAMGI_MESA_ORBITAL_FRAME_R;
orbital->frame_g = GAMGI_MESA_ORBITAL_FRAME_G;
orbital->frame_b = GAMGI_MESA_ORBITAL_FRAME_B;
}

/**** external function ******
 *                           *
 *   GAMGI_MESA_START_BOND   *
 *                           *
 * Default for bond objects. *
 *                           *
 *****************************/

void gamgi_mesa_start_bond (gamgi_bond *bond, gamgi_bond_class *bond_class)
{
bond->draw = bond_class->draw;
bond->size = bond_class->size;

bond->color = bond_class->color;
bond->red = bond_class->red;
bond->green = bond_class->green;
bond->blue = bond_class->blue;
}

/**** external function ******
 *                           *
 *   GAMGI_MESA_START_ATOM   *
 *                           *
 * Default for atom objects. *
 *                           *
 *****************************/

void gamgi_mesa_start_atom (gamgi_atom *atom, gamgi_atom_class *atom_class)
{
atom->draw = atom_class->draw;
atom->size = atom_class->size;
atom->variancy = atom_class->variancy;

atom->temperature = GAMGI_CHEM_ATOM_TEMPERATURE;
atom->occupancy = 1.0;
}

/******** external function *******
 *                                *
 *   GAMGI_MESA_START_DIRECTION   *
 *                                *
 * Default for direction objects. *
 *                                *
 **********************************/

void gamgi_mesa_start_direction (gamgi_direction *direction, gamgi_direction_class *direction_class)
{
direction->vectors = GAMGI_PHYS_DIRECTION_VECTORS;
direction->reference = GAMGI_ENGINE_CELL;

/*********
 * color *
 *********/

direction->red = direction_class->red;
direction->green = direction_class->green;
direction->blue = direction_class->blue;
}

/***** external function ******
 *                            *
 *   GAMGI_MESA_START_PLANE   *
 *                            *
 * Default for plane objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_plane (gamgi_plane *plane, gamgi_plane_class *plane_class)
{
plane->vectors = GAMGI_PHYS_PLANE_VECTORS;
plane->reference = GAMGI_ENGINE_CELL;

/*********
 * color *
 *********/

plane->red = plane_class->red;
plane->green = plane_class->green;
plane->blue = plane_class->blue;
}


/***** external function ******
 *                            *
 *   GAMGI_MESA_START_GROUP   *
 *                            *
 * Default for group objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_group (gamgi_group *group, gamgi_group_class *group_class)
{
strcpy (group->object.name, "Group");
group->reference = GAMGI_CHEM_CONTAINER;

group->red = GAMGI_MESA_GROUP_R;
group->green = GAMGI_MESA_GROUP_G;
group->blue = GAMGI_MESA_GROUP_B;
}

/******** external function ******
 *                               *
 *   GAMGI_MESA_START_MOLECULE   *
 *                               *
 * Default for molecule objects. *
 *                               *
 ********************************/

void gamgi_mesa_start_molecule (gamgi_molecule *molecule, gamgi_molecule_class *molecule_class)
{
strcpy (molecule->object.name, "Molecule");
}


/******* external function ******
 *                              *
 *  GAMGI_MESA_START_CLUSTER    *
 *                              *
 * Default for cluster objects. *
 *                              *
 ********************************/

void gamgi_mesa_start_cluster (gamgi_cluster *cluster, gamgi_cluster_class *cluster_class)
{
strcpy (cluster->object.name, "Cluster");
cluster->reference = GAMGI_CHEM_CONTAINER;
}

/***** external function *****
 *                           *
 *   GAMGI_MESA_START_CELL   *
 *                           *
 * Default for cell objects. *
 *                           *
 *****************************/

void gamgi_mesa_start_cell (gamgi_cell *cell, gamgi_cell_class *cell_class)
{
strcpy (cell->object.name, "Cell");

cell->model = cell_class->model;
cell->axes = cell_class->axes;

cell->borders = cell_class->borders;
cell->faces = cell_class->faces;
cell->nodes = cell_class->nodes;

cell->red = cell_class->red;
cell->green = cell_class->green;
cell->blue = cell_class->blue;
}

/****** external function *****
 *                            *
 *   GAMGI_MESA_START_ARROW   *
 *                            *
 * Default for arrow objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_arrow (gamgi_arrow *arrow, gamgi_arrow_class *arrow_class)
{
strcpy (arrow->object.name, "Arrow");
}

/****** external function *****
 *                            *
 *   GAMGI_MESA_START_SHAPE   *
 *                            *
 * Default for shape objects. *
 *                            *
 ******************* **********/

void gamgi_mesa_start_shape (gamgi_shape *shape, gamgi_shape_class *shape_class)
{
strcpy (shape->object.name, "Shape");
}

/****** external function *****
 *                            *
 *   GAMGI_MESA_START_GRAPH   *
 *                            *
 * Default for graph objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_graph (gamgi_graph *graph, gamgi_graph_class *graph_class)
{
strcpy (graph->object.name, "Graph");
}

/******* external function *******
 *                               *
 *   GAMGI_MESA_START_ASSEMBLY   *
 *                               *
 * Default for assembly objects. *
 *                               *
 *********************************/

void gamgi_mesa_start_assembly (gamgi_assembly *assembly, gamgi_assembly_class *assembly_class)
{
strcpy (assembly->object.name, "Assembly");
}

/****** external function *****
 *                            *
 *   GAMGI_MESA_START_LIGHT   *
 *                            *
 * Default for light objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_light (gamgi_light *light, gamgi_light_class *light_class)
{
strcpy (light->object.name, "Light");
}

/***** external function ******
 *                            *
 *   GAMGI_MESA_START_LAYER   *
 *                            *
 * Default for layer objects. *
 *                            *
 ******************************/

void gamgi_mesa_start_layer (gamgi_layer *layer, gamgi_layer_class *layer_class)
{
strcpy (layer->object.name, "Layer");

/**************
 * projection *
 **************/

layer->perspective = layer_class->perspective;
layer->top = layer_class->top;
layer->near = layer_class->near;
layer->far = layer_class->far;

/*************
 * modelview *
 *************/

gamgi_math_vector_copy (layer_class->eye, layer->eye);
gamgi_math_vector_copy (layer_class->center, layer->center);
gamgi_math_vector_copy (layer_class->up, layer->up);

gamgi_math_matrix_unit (layer->referential);

/**************
 * visibility *
 **************/

layer->visibility_in = GAMGI_GTK_ALL;
layer->visibility_out = GAMGI_GTK_ALL;

/**************
 * background *
 **************/

layer->color[0] = GAMGI_MESA_LAYER_R;
layer->color[1] = GAMGI_MESA_LAYER_G;
layer->color[2] = GAMGI_MESA_LAYER_B;
}

/***** external function *******
 *                             *
 *   GAMGI_MESA_START_WINDOW   *
 *                             *
 * Default for window objects. *
 *                             *
 ******************* ***********/

void gamgi_mesa_start_window (gamgi_window *window, gamgi_window_class *window_class)
{
strcpy (window->object.name, "Window");

window->origin_x = GAMGI_GTK_WINDOW_ORIGIN_X;
window->origin_y = GAMGI_GTK_WINDOW_ORIGIN_Y;

window->width = GAMGI_GTK_WINDOW_WIDTH;
window->height = GAMGI_GTK_WINDOW_HEIGHT;
}
