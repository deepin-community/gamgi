/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_GTK
#define GAMGI_GTK

/********** types *********
 **************************/

/*******************************************
 * These types define pointers to callback *
 * functions connected to these signals:   *
 * 1) "clicked", "activated"...            *
 * 2) "delete_event"                       *
 *******************************************/

typedef void (* gamgi_callback2) (GtkWidget *widget, void *data);
typedef void (* gamgi_callback3) (GtkWidget *widget,
GdkEventButton *event, void *data);

/********** enumerations **********
 **********************************/

/**************************************************************
 * All external enumerations start at 1 and share FALSE for 0 *
 **************************************************************/

enum { GAMGI_GTK_SCALE = 1, GAMGI_GTK_MOVE, GAMGI_GTK_ROTATE };

enum { GAMGI_GTK_YES = 1, GAMGI_GTK_NO };

enum { GAMGI_GTK_ALL = 1, GAMGI_GTK_PARTIAL, GAMGI_GTK_NONE };

enum { GAMGI_GTK_CELL = 1, GAMGI_GTK_ATOMS };

enum { GAMGI_GTK_ORDER = 1, GAMGI_GTK_NODE };

/*******************************
 * All tasks with a dialog of  *
 * level 0 are enumerated here *
 *******************************/

enum { GAMGI_GTK_HELP_TOPIC = 1, GAMGI_GTK_HELP_SEARCH, 
GAMGI_GTK_HELP_TUTORIAL, GAMGI_GTK_HELP_CONFIG,

GAMGI_GTK_TEXT_CREATE, GAMGI_GTK_TEXT_COPY,
GAMGI_GTK_TEXT_SELECT, GAMGI_GTK_TEXT_LINK,    
GAMGI_GTK_TEXT_MODIFY, GAMGI_GTK_TEXT_REMOVE,
GAMGI_GTK_TEXT_CONFIG,

GAMGI_GTK_ORBITAL_CREATE, GAMGI_GTK_ORBITAL_COPY, 
GAMGI_GTK_ORBITAL_SELECT, GAMGI_GTK_ORBITAL_LINK, 
GAMGI_GTK_ORBITAL_MODIFY, GAMGI_GTK_ORBITAL_REMOVE,
GAMGI_GTK_ORBITAL_CONFIG,

GAMGI_GTK_BOND_CREATE, GAMGI_GTK_BOND_COPY, 
GAMGI_GTK_BOND_SELECT, GAMGI_GTK_BOND_LINK, 
GAMGI_GTK_BOND_MODIFY, GAMGI_GTK_BOND_REMOVE,
GAMGI_GTK_BOND_CONFIG, 

GAMGI_GTK_ATOM_CREATE, GAMGI_GTK_ATOM_COPY, 
GAMGI_GTK_ATOM_SELECT, GAMGI_GTK_ATOM_LINK, 
GAMGI_GTK_ATOM_MODIFY, GAMGI_GTK_ATOM_MEASURE,
GAMGI_GTK_ATOM_REMOVE, GAMGI_GTK_ATOM_CONFIG,

GAMGI_GTK_DIRECTION_CREATE, GAMGI_GTK_DIRECTION_COPY, 
GAMGI_GTK_DIRECTION_SELECT, GAMGI_GTK_DIRECTION_LINK, 
GAMGI_GTK_DIRECTION_MODIFY, GAMGI_GTK_DIRECTION_MEASURE,
GAMGI_GTK_DIRECTION_REMOVE, GAMGI_GTK_DIRECTION_CONFIG,

GAMGI_GTK_PLANE_CREATE, GAMGI_GTK_PLANE_COPY, 
GAMGI_GTK_PLANE_SELECT, GAMGI_GTK_PLANE_LINK, 
GAMGI_GTK_PLANE_MODIFY, GAMGI_GTK_PLANE_MEASURE,
GAMGI_GTK_PLANE_REMOVE, GAMGI_GTK_PLANE_CONFIG,

GAMGI_GTK_GROUP_CREATE, GAMGI_GTK_GROUP_CREATE_POLYTOPE,
GAMGI_GTK_GROUP_COPY, GAMGI_GTK_GROUP_SELECT,
GAMGI_GTK_GROUP_LINK, GAMGI_GTK_GROUP_MODIFY,
GAMGI_GTK_GROUP_MODIFY_POLYTOPE, GAMGI_GTK_GROUP_MEASURE,
GAMGI_GTK_GROUP_MEASURE_GEOMETRY, GAMGI_GTK_GROUP_MEASURE_SYMMETRY,
GAMGI_GTK_GROUP_MEASURE_VORONOI, GAMGI_GTK_GROUP_REMOVE, 
GAMGI_GTK_GROUP_CONFIG,

GAMGI_GTK_MOLECULE_CREATE, GAMGI_GTK_MOLECULE_COPY, 
GAMGI_GTK_MOLECULE_SELECT, GAMGI_GTK_MOLECULE_LINK, 
GAMGI_GTK_MOLECULE_MODIFY, GAMGI_GTK_MOLECULE_MEASURE, 
GAMGI_GTK_MOLECULE_MEASURE_GEOMETRY, GAMGI_GTK_MOLECULE_MEASURE_SYMMETRY,
GAMGI_GTK_MOLECULE_MEASURE_VORONOI, GAMGI_GTK_MOLECULE_ANIMATE, 
GAMGI_GTK_MOLECULE_REMOVE, GAMGI_GTK_MOLECULE_CONFIG,

GAMGI_GTK_CLUSTER_CREATE, GAMGI_GTK_CLUSTER_COPY, 
GAMGI_GTK_CLUSTER_SELECT, GAMGI_GTK_CLUSTER_LINK, 
GAMGI_GTK_CLUSTER_MODIFY, GAMGI_GTK_CLUSTER_MEASURE, 
GAMGI_GTK_CLUSTER_MEASURE_GEOMETRY, GAMGI_GTK_CLUSTER_MEASURE_SYMMETRY,
GAMGI_GTK_CLUSTER_MEASURE_VORONOI, GAMGI_GTK_CLUSTER_ANIMATE, 
GAMGI_GTK_CLUSTER_REMOVE, GAMGI_GTK_CLUSTER_CONFIG,

GAMGI_GTK_CELL_CREATE, GAMGI_GTK_CELL_COPY, 
GAMGI_GTK_CELL_SELECT, GAMGI_GTK_CELL_LINK, 
GAMGI_GTK_CELL_LINK_RCP, GAMGI_GTK_CELL_LINK_POLYTOPE,
GAMGI_GTK_CELL_MODIFY, GAMGI_GTK_CELL_MEASURE,
GAMGI_GTK_CELL_MEASURE_GEOMETRY, GAMGI_GTK_CELL_MEASURE_RECIPROCAL,
GAMGI_GTK_CELL_MEASURE_VORONOI, GAMGI_GTK_CELL_ANIMATE,
GAMGI_GTK_CELL_REMOVE, GAMGI_GTK_CELL_CONFIG,

GAMGI_GTK_ARROW_CREATE, GAMGI_GTK_ARROW_COPY, 
GAMGI_GTK_ARROW_SELECT, GAMGI_GTK_ARROW_LINK, 
GAMGI_GTK_ARROW_MODIFY, GAMGI_GTK_ARROW_REMOVE,
GAMGI_GTK_ARROW_CONFIG,

GAMGI_GTK_SHAPE_CREATE, GAMGI_GTK_SHAPE_COPY, 
GAMGI_GTK_SHAPE_SELECT, GAMGI_GTK_SHAPE_LINK, 
GAMGI_GTK_SHAPE_MODIFY, GAMGI_GTK_SHAPE_REMOVE,
GAMGI_GTK_SHAPE_CONFIG,

GAMGI_GTK_GRAPH_CREATE, GAMGI_GTK_GRAPH_COPY, 
GAMGI_GTK_GRAPH_SELECT, GAMGI_GTK_GRAPH_LINK, 
GAMGI_GTK_GRAPH_MODIFY, GAMGI_GTK_GRAPH_MEASURE, 
GAMGI_GTK_GRAPH_REMOVE, GAMGI_GTK_GRAPH_CONFIG,

GAMGI_GTK_ASSEMBLY_CREATE, GAMGI_GTK_ASSEMBLY_COPY, 
GAMGI_GTK_ASSEMBLY_SELECT, GAMGI_GTK_ASSEMBLY_LINK, 
GAMGI_GTK_ASSEMBLY_MODIFY, GAMGI_GTK_ASSEMBLY_REMOVE,
GAMGI_GTK_ASSEMBLY_CONFIG,

GAMGI_GTK_LIGHT_CREATE, GAMGI_GTK_LIGHT_COPY, 
GAMGI_GTK_LIGHT_SELECT, GAMGI_GTK_LIGHT_LINK, 
GAMGI_GTK_LIGHT_MODIFY, GAMGI_GTK_LIGHT_REMOVE,
GAMGI_GTK_LIGHT_CONFIG,

GAMGI_GTK_LAYER_CREATE, GAMGI_GTK_LAYER_COPY, 
GAMGI_GTK_LAYER_SELECT, GAMGI_GTK_LAYER_LINK, 
GAMGI_GTK_LAYER_MODIFY, GAMGI_GTK_LAYER_REMOVE,
GAMGI_GTK_LAYER_CONFIG,

GAMGI_GTK_WINDOW_CREATE, GAMGI_GTK_WINDOW_COPY, 
GAMGI_GTK_WINDOW_SELECT, GAMGI_GTK_WINDOW_LINK, 
GAMGI_GTK_WINDOW_MODIFY, GAMGI_GTK_WINDOW_REMOVE,
GAMGI_GTK_WINDOW_CONFIG,

GAMGI_GTK_FILE_IMPORT, GAMGI_GTK_FILE_EXPORT, 
GAMGI_GTK_FILE_CONFIG,

GAMGI_GTK_GAMGI_ABOUT, GAMGI_GTK_GAMGI_RESET, 
GAMGI_GTK_GAMGI_EXIT, GAMGI_GTK_GAMGI_CONFIG };

/************* macros ************
 *********************************/

/***********************************
 * Active color: red (used in the  *
 * rotate,move,scale tool buttons) *
 ***********************************/

#define GAMGI_GTK_ACTIVE_R 0.039
#define GAMGI_GTK_ACTIVE_G 0.609
#define GAMGI_GTK_ACTIVE_B 0.0

/****************************
 * Stop, Move colors: used  *
 * in the Undo/Save dialogs *
 ****************************/

/*******************
 * Stop color: red *
 *******************/

#define GAMGI_GTK_STOP_R 1.0
#define GAMGI_GTK_STOP_G 0.0
#define GAMGI_GTK_STOP_B 0.0

/********************************
 * Move color: green (10,156,0) *
 ********************************/

#define GAMGI_GTK_MOVE_R 0.039
#define GAMGI_GTK_MOVE_G 0.609
#define GAMGI_GTK_MOVE_B 0.0

/*******************************
 * Dark, Light colors: used in *
 * the Chemical Periodic Table *
 *******************************/

/*********************
 * Dark color: black *
 *********************/

#define GAMGI_GTK_DARK_R 0.0
#define GAMGI_GTK_DARK_G 0.0
#define GAMGI_GTK_DARK_B 0.0

/**********************
 * Light color: white *
 **********************/

#define GAMGI_GTK_LIGHT_R 1.0
#define GAMGI_GTK_LIGHT_G 1.0
#define GAMGI_GTK_LIGHT_B 1.0

/****************************************
 * Background, Foreground, Title, Bold, *
 * Link: used to show Help information  *
 ****************************************/

/********************************************
 * Background color: olive green (47,79,79) *
 ********************************************/

#define GAMGI_GTK_BACKGROUND_R 0.184
#define GAMGI_GTK_BACKGROUND_G 0.309
#define GAMGI_GTK_BACKGROUND_B 0.309

/************************************************
 * Foreground color: light yellow (245,222,179) *
 ************************************************/

#define GAMGI_GTK_FOREGROUND_R 0.957
#define GAMGI_GTK_FOREGROUND_G 0.867
#define GAMGI_GTK_FOREGROUND_B 0.699

/**********************************
 * Title color: very light yellow *
 **********************************/

#define GAMGI_GTK_TITLE_R 1.0
#define GAMGI_GTK_TITLE_G 1.0
#define GAMGI_GTK_TITLE_B 0.0

/*********************************
 * Bold color: very light yellow *
 *********************************/

#define GAMGI_GTK_BOLD_R 1.0
#define GAMGI_GTK_BOLD_G 1.0
#define GAMGI_GTK_BOLD_B 0.0

/**********************
 * Text size increase *
 **********************/

#define GAMGI_GTK_TEXT_NORMAL PANGO_SCALE_MEDIUM
#define GAMGI_GTK_TEXT_LARGE  PANGO_SCALE_LARGE

/*******************************
 * Link color: very light blue *
 *******************************/

#define GAMGI_GTK_LINK_R 0.8
#define GAMGI_GTK_LINK_G 1.0
#define GAMGI_GTK_LINK_B 1.0

/*******************************************
 * x,y origin of a new top window. Setting *
 * these values to 0,0, the window starts  *
 * at the top left corner of the screen.   *
 *******************************************/

#define GAMGI_GTK_WINDOW_ORIGIN_X 10
#define GAMGI_GTK_WINDOW_ORIGIN_Y 10

/**************************************
 * default dimensions for top windows *
 *         (golden rule=1.62)         *
 * minimum dimensions for top windows * 
 **************************************/

#define GAMGI_GTK_WINDOW_WIDTH  950
#define GAMGI_GTK_WINDOW_HEIGHT 650

#define GAMGI_GTK_WINDOW_WIDTH_MIN  700
#define GAMGI_GTK_WINDOW_HEIGHT_MIN 500

/*************************************************
 * default and minimum dimensions for windows    *
 * with GtkTextView widgets, used to show large  *
 * volumes of information: Help, Analysis, etc.  *
 *************************************************/

#define GAMGI_GTK_TEXT_WIDTH      650
#define GAMGI_GTK_TEXT_HEIGHT     550

#define GAMGI_GTK_TEXT_WIDTH_MIN  250
#define GAMGI_GTK_TEXT_HEIGHT_MIN 250

/*****************************************
 * dimensions for Object->Select windows *
 *****************************************/

#define GAMGI_GTK_SELECT_WIDTH      300
#define GAMGI_GTK_SELECT_HEIGHT     400

#define GAMGI_GTK_SELECT_LIST_WIDTH      250
#define GAMGI_GTK_SELECT_LIST_HEIGHT     400

/************************************************
 * height for Help Topic, Help Tutorial windows *
 ************************************************/

#define GAMGI_GTK_HELP_TOPIC_WIDTH     300
#define GAMGI_GTK_HELP_TOPIC_HEIGHT    400

#define GAMGI_GTK_HELP_TUTORIAL_HEIGHT 400

/***********************************
 * min,max dimensions for Cell     *
 * Systems,Lattice,Groups windows. *
 ***********************************/

#define GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN 200
#define GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MAX 400

/******************************
 * width of Ok/Cancel buttons *
 * used in dialog windows     *
 ******************************/

#define GAMGI_GTK_BUTTON_WIDTH 85

/*****************************************************
 * width in char and allowed char for dialog entries *
 *****************************************************/

#define GAMGI_GTK_CHAR_50 50
#define GAMGI_GTK_CHAR_40 40
#define GAMGI_GTK_CHAR_20 20
#define GAMGI_GTK_CHAR_10 10
#define GAMGI_GTK_CHAR_7  7
#define GAMGI_GTK_CHAR_5  5
#define GAMGI_GTK_CHAR_3  3
#define GAMGI_GTK_CHAR_2  2

/*******************************************
 * width in char used in Method ComboBoxes *
 * with submenus in Object->Select dialogs *
 *******************************************/

#define GAMGI_GTK_CHAR_MENU 13

/***********************************************************
 * max number of chars used to create file filter patterns *
 ***********************************************************/

#define GAMGI_GTK_FILE_PATTERNS GAMGI_GTK_CHAR_40

/**************************
 * default for beep sound *
 **************************/

#define GAMGI_GTK_BEEP TRUE

/***************************************
 * ruler amplitude                     *
 *                                     *
 * ruler maximum rotate: 30 degrees    *
 * ruler maximum move:   10.0 Angstrom *
 * ruler maximum scale:  2.0           *
 *                                     *
 * ruler rotate step: 0 (continous)    *
 * ruler move step: 0 (continous)      *
 * ruler scale step: 0 (continous)     *
 ***************************************/

#define GAMGI_GTK_RULER_MAX 10.0

#define GAMGI_GTK_ROTATE_MAX 30.0
#define GAMGI_GTK_MOVE_MAX 10.0
#define GAMGI_GTK_SCALE_MAX 2.0

#define GAMGI_GTK_MOVE_STEP 0.0
#define GAMGI_GTK_ROTATE_STEP 0.0
#define GAMGI_GTK_SCALE_STEP 0.0

/****************************************************************
 *              For each top window statusbar                   *
 *                                                              *
 * TIME_FOCUS, TIME_LAYER: time delay before color changes      *
 *                                                              *
 * TIME_TIME: time to update time (every 60 seconds)            *
 *                                                              *
 * PROGRESS_TIME, PROGRESS_FRACTION:                            *
 * time and fraction steps, to update progress bar              *
 ****************************************************************/

#define GAMGI_GTK_STATUSBAR_TIME_FOCUS   2000
#define GAMGI_GTK_STATUSBAR_TIME_LAYER   2000

#define GAMGI_GTK_STATUSBAR_TIME_TIME   60000

#define GAMGI_GTK_STATUSBAR_PROGRESS_TIME      200
#define GAMGI_GTK_STATUSBAR_PROGRESS_FRACTION 0.05

/***********************************
 * default order for mouse buttons *
 ***********************************/

#define GAMGI_GTK_BUTTON_1 1
#define GAMGI_GTK_BUTTON_2 2
#define GAMGI_GTK_BUTTON_3 3

/*****************************
 * defaults for copy dialogs *
 *****************************/

#define GAMGI_GTK_COPY_NUMBER 1
#define GAMGI_GTK_COPY_X    0.0
#define GAMGI_GTK_COPY_Y    0.0
#define GAMGI_GTK_COPY_Z    0.0

/*****************************
 * defaults for link dialogs *
 *****************************/

#define GAMGI_GTK_LINK_X    0.0
#define GAMGI_GTK_LINK_Y    0.0
#define GAMGI_GTK_LINK_Z    0.0

#endif
