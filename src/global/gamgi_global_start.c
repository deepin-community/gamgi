/******************************************
 *
 * $GAMGI/src/global/gamgi_global_start.c
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
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_property.h"
#include "gamgi_io_token.h"

/********************** external function *****************
 *                                                        *
 *                  GAMGI_GLOBAL_START_HELP               *
 *                                                        *
 *                                                        *
 **********************************************************/

void gamgi_global_start_help (gamgi_help_class *help_class)
{
/***********************************************
 * source and agent to show help documentation *
 ***********************************************/

help_class->source_in = TRUE;
help_class->agent_in = TRUE;

/**************************************************************
 * initialise paths: GAMGI and BROWSER environment variables  *
 * are obtained only after this stage and if defined they are *
 * then used to set gamgi->help->local, gamgi->help->browser  *
 **************************************************************/

help_class->local = NULL;
help_class->remote = NULL;
help_class->browser = NULL;
}

/********************** external function *****************
 *                                                        *
 *                  GAMGI_GLOBAL_START_TEXT               *
 *                                                        *
 *                                                        *
 **********************************************************/

void gamgi_global_start_text (gamgi_text_class *text_class)
{
text_class->style =  GAMGI_MESA_TEXT_STYLE;
text_class->wired =  GAMGI_IO_TEXT_WIRED;
text_class->solid = GAMGI_IO_TEXT_SOLID;

text_class->red = GAMGI_MESA_TEXT_R;
text_class->green = GAMGI_MESA_TEXT_G;
text_class->blue = GAMGI_MESA_TEXT_B;

gamgi_io_token_create (GAMGI_IO_PATH_TRUETYPE, &text_class->path);
}

/************************ external function ******************
 *                                                           *
 *                  GAMGI_GLOBAL_START_ORBITAL               *
 *                                                           *
 *                                                           *
 *************************************************************/

void gamgi_global_start_orbital (gamgi_orbital_class *orbital_class)
{
}

/********************** external function *****************
 *                                                        *
 *                  GAMGI_GLOBAL_START_BOND               *
 *                                                        *
 *                                                        *
 **********************************************************/

void gamgi_global_start_bond (gamgi_bond_class *bond_class)
{
/***********************************
 * drawing properties: style, size *
 ***********************************/

bond_class->draw = GAMGI_MESA_BOND_DRAW;
bond_class->size = GAMGI_CHEM_BOND_SIZE;

bond_class->color = GAMGI_MESA_BOND_COLOR;
bond_class->red = GAMGI_MESA_BOND_R;
bond_class->green = GAMGI_MESA_BOND_G;
bond_class->blue = GAMGI_MESA_BOND_B;

/*******************
 * bond properties *
 *******************/

bond_class->lower = GAMGI_CHEM_BOND_LOWER;
bond_class->upper = GAMGI_CHEM_BOND_UPPER;

/***********************************
 * drawing properties: style, size *
 ***********************************/

bond_class->draw = GAMGI_MESA_BOND_DRAW;
bond_class->size = GAMGI_CHEM_BOND_SIZE;

gamgi_chem_bond_length (bond_class);
}

/********************** external function *****************
 *                                                        *
 *                  GAMGI_GLOBAL_START_ATOM               *
 *                                                        *
 *                                                        *
 **********************************************************/

void gamgi_global_start_atom (gamgi_atom_class *atom_class)
{
/********************************************
 * drawing properties: style, quality, size *
 ********************************************/

atom_class->draw = GAMGI_MESA_ATOM_DRAW;
atom_class->variancy = GAMGI_CHEM_ATOM_VARIANCY;
atom_class->size = GAMGI_CHEM_ATOM_SIZE;

atom_class->slices = GAMGI_MESA_ATOM_SLICES;
atom_class->width = GAMGI_MESA_ATOM_WIDTH;
atom_class->min = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_DEFAULT];

atom_class->temperature = GAMGI_CHEM_ATOM_TEMPERATURE;

/************************************
 * fill arrays with atom properties *
 ************************************/

gamgi_chem_atom_mass (atom_class);
gamgi_chem_atom_radius (atom_class);
gamgi_chem_atom_color (atom_class);
}

/************************** external function ******************
 *                                                             *
 *                  GAMGI_GLOBAL_START_DIRECTION               *
 *                                                             *
 *                                                             *
 ***************************************************************/

void gamgi_global_start_direction (gamgi_direction_class *direction_class)
{
direction_class->red = GAMGI_MESA_DIRECTION_R;
direction_class->green = GAMGI_MESA_DIRECTION_G;
direction_class->blue = GAMGI_MESA_DIRECTION_B;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_START_PLANE              *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_plane (gamgi_plane_class *plane_class)
{
plane_class->red = GAMGI_MESA_PLANE_R;
plane_class->green = GAMGI_MESA_PLANE_G;
plane_class->blue = GAMGI_MESA_PLANE_B;
}

/*********************** external function *****************
 *                                                         *
 *                   GAMGI_GLOBAL_START_GROUP              *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_group (gamgi_group_class *group_class)
{
}

/************************** external function *****************
 *                                                            *
 *                  GAMGI_GLOBAL_START_MOLECULE               *
 *                                                            *
 *                                                            *
 **************************************************************/

void gamgi_global_start_molecule (gamgi_molecule_class *molecule_class)
{
}

/************************* external function *****************
 *                                                           *
 *                  GAMGI_GLOBAL_START_CLUSTER               *
 *                                                           *
 *                                                           *
 *************************************************************/

void gamgi_global_start_cluster (gamgi_cluster_class *cluster_class)
{
}

/********************** external function *****************
 *                                                        *
 *                  GAMGI_GLOBAL_START_CELL               *
 *                                                        *
 *                                                        *
 **********************************************************/

void gamgi_global_start_cell (gamgi_cell_class *cell_class)
{
/**********************
 * default parameters *
 **********************/

cell_class->model = GAMGI_PHYS_CELL_MODEL;
cell_class->axes = GAMGI_PHYS_CELL_AXES;
cell_class->borders = GAMGI_PHYS_CELL_BORDERS;
cell_class->faces = GAMGI_PHYS_CELL_FACES;
cell_class->nodes = GAMGI_PHYS_CELL_NODES;
cell_class->red = GAMGI_MESA_CELL_R;
cell_class->green = GAMGI_MESA_CELL_G;
cell_class->blue = GAMGI_MESA_CELL_B;

/*********************
 * global parameters *
 *********************/

cell_class->axes_size = GAMGI_MESA_AXES_SIZE;
cell_class->borders_width = GAMGI_MESA_CELL_LINE;
cell_class->nodes_width = GAMGI_MESA_CELL_POINT;
}

/*********************** external function *****************
 *                                                         *
 *                  GAMGI_GLOBAL_START_ARROW               *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_arrow (gamgi_arrow_class *arrow_class)
{
}

/*********************** external function *****************
 *                                                         *
 *                  GAMGI_GLOBAL_START_SHAPE               *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_shape (gamgi_shape_class *shape_class)
{
}

/*********************** external function *****************
 *                                                         *
 *                  GAMGI_GLOBAL_START_GRAPH               *
 *                                                         *
 *                                                         *
 **********************************************************/

void gamgi_global_start_graph (gamgi_graph_class *graph_class)
{
}

/************************* external function ******************
 *                                                            *
 *                  GAMGI_GLOBAL_START_ASSEMBLY               *
 *                                                            *
 *                                                            *
 **************************************************************/

void gamgi_global_start_assembly (gamgi_assembly_class *assembly_class)
{
}

/*********************** external function *****************
 *                                                         *
 *                  GAMGI_GLOBAL_START_LIGHT               *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_light (gamgi_light_class *light_class)
{
light_class->shininess = GAMGI_MESA_LIGHT_SHININESS;

light_class->ambient[0] = GAMGI_MESA_LIGHT_AMBIENT_R;
light_class->ambient[1] = GAMGI_MESA_LIGHT_AMBIENT_G;
light_class->ambient[2] = GAMGI_MESA_LIGHT_AMBIENT_B;

light_class->diffuse[0] = GAMGI_MESA_LIGHT_DIFFUSE_R;
light_class->diffuse[1] = GAMGI_MESA_LIGHT_DIFFUSE_G;
light_class->diffuse[2] = GAMGI_MESA_LIGHT_DIFFUSE_B;

light_class->specular[0] = GAMGI_MESA_LIGHT_SPECULAR_R;
light_class->specular[1] = GAMGI_MESA_LIGHT_SPECULAR_G;
light_class->specular[2] = GAMGI_MESA_LIGHT_SPECULAR_B;
}

/*********************** external function *****************
 *                                                         *
 *                  GAMGI_GLOBAL_START_LAYER               *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_layer (gamgi_layer_class *layer_class)
{
/***********
 * history *
 ***********/

layer_class->undo = GAMGI_ENGINE_UNDO;
layer_class->save = GAMGI_ENGINE_SAVE;

/**************
 * projection *
 **************/

layer_class->perspective = GAMGI_MESA_LAYER_PERSPECTIVE;
layer_class->top = GAMGI_MESA_LAYER_TOP;
layer_class->near = GAMGI_MESA_LAYER_NEAR;
layer_class->far = GAMGI_MESA_LAYER_FAR;

/*************
 * modelview *
 *************/

layer_class->eye[0] = GAMGI_MESA_LAYER_EYEX;
layer_class->eye[1] = GAMGI_MESA_LAYER_EYEY;
layer_class->eye[2] = GAMGI_MESA_LAYER_EYEZ;
layer_class->center[0] = GAMGI_MESA_LAYER_CENTERX;
layer_class->center[1] = GAMGI_MESA_LAYER_CENTERY;
layer_class->center[2] = GAMGI_MESA_LAYER_CENTERZ;
layer_class->up[0] = GAMGI_MESA_LAYER_UPX;
layer_class->up[1] = GAMGI_MESA_LAYER_UPY;
layer_class->up[2] = GAMGI_MESA_LAYER_UPZ;
}

/************************ external function *****************
 *                                                          *
 *                 GAMGI_GLOBAL_START_WINDOW                *
 *                                                          *
 *                                                          *
 ************************************************************/

void gamgi_global_start_window (gamgi_window_class *window_class)
{
window_class->rotate_max = GAMGI_GTK_ROTATE_MAX;
window_class->rotate_step = GAMGI_GTK_ROTATE_STEP;

window_class->move_max = GAMGI_GTK_MOVE_MAX;
window_class->move_step = GAMGI_GTK_MOVE_STEP;

window_class->scale_max = GAMGI_GTK_SCALE_MAX;
window_class->scale_step = GAMGI_GTK_SCALE_STEP;
}

/********************** external function ******************
 *                                                         *
 *                  GAMGI_GLOBAL_START_GAMGI               *
 *                                                         *
 *                                                         *
 ***********************************************************/

void gamgi_global_start_gamgi (gamgi_gamgi_class *gamgi_class)
{
/*********
 * sound *
 *********/

gamgi_class->beep = GAMGI_GTK_BEEP;

/******************************************
 * number of pixels from the center that  *
 * are considered when choosing an object *
 ******************************************/

gamgi_class->tolerance = GAMGI_MESA_PICK_TOLERANCE;

/*********************************
 * default mouse button ordering *
 *********************************/

gamgi_class->button1 = GDK_BUTTON1_MASK;
gamgi_class->button2 = GDK_BUTTON2_MASK;
gamgi_class->button3 = GDK_BUTTON3_MASK;

/*******************************************************
 * number of decimal figures used to show real numbers *
 *******************************************************/

gamgi_class->length = GAMGI_MATH_DECIMAL_LENGTH;
gamgi_class->angle = GAMGI_MATH_DECIMAL_ANGLE;
gamgi_class->charge = GAMGI_MATH_DECIMAL_CHARGE;
gamgi_class->mass = GAMGI_MATH_DECIMAL_MASS;

/******************************
 * colors to show information *
 ******************************/

gamgi_class->background[0] = GAMGI_GTK_BACKGROUND_R;
gamgi_class->background[1] = GAMGI_GTK_BACKGROUND_G;
gamgi_class->background[2] = GAMGI_GTK_BACKGROUND_B;

gamgi_class->foreground[0] = GAMGI_GTK_FOREGROUND_R;
gamgi_class->foreground[1] = GAMGI_GTK_FOREGROUND_G;
gamgi_class->foreground[2] = GAMGI_GTK_FOREGROUND_B;

gamgi_class->title[0] = GAMGI_GTK_TITLE_R;
gamgi_class->title[1] = GAMGI_GTK_TITLE_G;
gamgi_class->title[2] = GAMGI_GTK_TITLE_B;

gamgi_class->bold[0] = GAMGI_GTK_BOLD_R;
gamgi_class->bold[1] = GAMGI_GTK_BOLD_G;
gamgi_class->bold[2] = GAMGI_GTK_BOLD_B;

gamgi_class->link[0] = GAMGI_GTK_LINK_R;
gamgi_class->link[1] = GAMGI_GTK_LINK_G;
gamgi_class->link[2] = GAMGI_GTK_LINK_B;
}
