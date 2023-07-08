/*********************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_create.c
 *
 * Copyright (C) 2006 Carlos Pereira
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_filter.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_link.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_gtk_direction_create.h"
#include "gamgi_gtk_direction_range.h"
#include "gamgi_gtk_direction_list.h"
#include "gamgi_mesa_select.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_position.h"
#include "gamgi_math_node.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_direction.h"
#include "gamgi_io_token.h"

static void static_instance (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/*********************************************************************
 * disable data in second level dialogs everytime Cell, Direction or *
 * Vectors information is changed, because data might be incorrect:  *
 * 1) in Direction->Create, changing the direction indices, the      *
 * second level plane indices might not obey the Weiss law; 2) in    *
 * Plane->Create, changing the Cell or the Vectors, the second       *
 * level o4 combo box might give nodes that are not valid anymore.   *
 *********************************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_node");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  gtk_button_clicked (GTK_BUTTON (button));
}

static void static_nodes (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int lattice, vectors;

/********************
 * get cell lattice *
 ********************/

if (cell == NULL) lattice = 0;
else lattice = cell->lattice;

/********************
 * get cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/****************
 * handle nodes *
 ****************/

gamgi_gtk_cell_symmetry_nodes (lattice, vectors, dialog);

/***************************
 * reset second level data *
 ***************************/

static_instance (NULL, window);
}

static void static_start_scope (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button_cell;
GtkWidget *button_range;
GtkWidget *button_node;
char token[GAMGI_ENGINE_TOKEN];
gamgi_bool active;

/*****************************************
 * structure, filter policies, thickness *
 *****************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_STRUCTURE - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_FILTER - 1);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_thickness");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_DIRECTION_THICKNESS);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************************************
 * range: 1) initially the button node *
 * is active, even if node is disabled *
 * 2) enable, disable radio buttons    *
 * 3) initialize node coordinates      *
 ***************************************/

button_cell = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
button_range = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_range");
button_node = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_node");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_node)) == FALSE)
  gtk_button_clicked (GTK_BUTTON (button_node));

active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_cell));
if (active == TRUE)
  {
  gtk_widget_set_sensitive (button_range, TRUE);
  gtk_widget_set_sensitive (button_node, TRUE);
  }
else
  {
  gtk_widget_set_sensitive (button_range, FALSE);
  gtk_widget_set_sensitive (button_node, FALSE);
  }

gamgi_gtk_direction_create_node (active, window);

/****************
 * second level *
 ****************/

static_instance (NULL, window);
}

static void static_start_projection (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/*********************************
 * Wulff net, Pole yes, Trace no *
 *********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_wulff");
gtk_button_clicked (GTK_BUTTON (button));

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_pole");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_trace");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

static void static_end_cell (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_DIRECTION_VECTORS - 1);
}

static void static_end_atom (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_cell (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_scope, *vbox_projection;

vbox_scope = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
vbox_projection = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_projection");

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell != NULL && cell->model == GAMGI_PHYS_PROJECTION)
  {
  gtk_widget_hide (vbox_scope);
  gtk_widget_show (vbox_projection);
  static_start_projection (window);
  }
else
  {
  gtk_widget_hide (vbox_projection);
  gtk_widget_show (vbox_scope);
  static_start_scope (window);
  static_nodes (cell, window);
  }

}

static GtkWidget *static_atom (GtkWidget *dialog)
{
GtkWidget *entry;
const char *name;

/**********
 * atom 1 *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return entry;

/**********
 * atom 2 *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return entry;

return NULL;
}

static void static_vectors (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell != NULL && cell->model != GAMGI_PHYS_PROJECTION)
  static_nodes (cell, window);
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *vbox_cell, *vbox_atom;

vbox_cell = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cell");
vbox_atom = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_atom");

/****************************************************
 * clean contents before hiding, not before showing *
 ****************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  static_end_atom (window);
  gtk_widget_hide (vbox_atom);
  gtk_widget_show (vbox_cell);
  }
else
  {
  static_end_cell (window);
  gtk_widget_hide (vbox_cell);
  gtk_widget_show (vbox_atom);
  }

static_cell (NULL, window);
}

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/***************************
 * reset direction indices *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/**************************
 * reset atom identifiers *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_SWITCH                 *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

/*********************************************************
 * close second level dialogs when notebook page changes *
 *********************************************************/

if (window->dialog1 != NULL)
  gamgi_gtk_direction_range_cancel (NULL, window);
}

static gamgi_bool static_ok_cell (gamgi_direction *direction, 
gamgi_cell **cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
char token[GAMGI_ENGINE_TOKEN] = "";

direction->reference = GAMGI_ENGINE_CELL;

/**********************
 * get reference cell *
 **********************/

*cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (*cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Object", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/*************************
 * Get direction indices *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->uvw[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->uvw[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->uvw[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_phys_direction_zero (direction->uvw) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/**********************
 * get direction name *
 **********************/

if (gtk_widget_is_sensitive (entry) == TRUE)
  sprintf (token, "[%d.%d.%d]", direction->uvw[0], direction->uvw[1], direction->uvw[2]);
if (gamgi_gtk_object_name (dialog, "entry_direction",
token, direction->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction name", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/********************
 * get cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
direction->vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

return TRUE;
}

static gamgi_bool static_ok_atoms (gamgi_direction *direction, 
gamgi_atom **atom1, gamgi_atom **atom2, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;

direction->reference = GAMGI_ENGINE_ATOM;

/**********************
 * get direction name *
 **********************/

if (gamgi_gtk_object_name (dialog, "entry_direction",
"Direction", direction->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction name", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/***********************
 * get reference atoms *
 ***********************/

*atom1 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom1", GAMGI_ENGINE_ATOM, window);
if (*atom1 == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

*atom2 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom2", GAMGI_ENGINE_ATOM, window);
if (*atom2 == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/**************************************
 * atoms must be far enough from each *
 * other to define a proper line      *
 **************************************/

if (gamgi_phys_direction_length ((*atom1)->position, (*atom2)->position) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom positions", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_ok_scope (gamgi_direction *direction,
gamgi_cell *cell, double *thickness, gamgi_enum *structure,
gamgi_enum *filter, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;

/**********************************
 * get structure, filter policies *
 **********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
*structure = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
*filter = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**************************
 * get scanning thickness *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_thickness");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, thickness, 
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid thickness data", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/********************************************
 * return control for atomic directions,    *
 * continue for crystallographic directions *
 ********************************************/

if (cell == NULL) return TRUE;

/**************************************
 * get o1,o2,o3 node coordinates      *
 * (for crystallographic planes only) *
 **************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->node[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction node", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->node[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction node", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &direction->node[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction node", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/**********************************************************
 * get o4 node coordinate (unless the lattice is centered *
 * and the vectors are conventional, o4 is always 000)    *
 **********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
direction->node[3] = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/****************************************************
 * each crystallographic direction is characterized *
 * by [uvw] indices plus a node where it passes,    *
 * to distinguish directions of the same family     *
 *                                                  *
 * when range data is used, node data is written    *
 * later, after determining a node in the           *
 * intersection of the two defining planes          *
 ****************************************************/

return TRUE;
}

static gamgi_bool static_ok_projection (gamgi_direction *direction, 
gamgi_bool *pole, gamgi_bool *trace, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/****************************************
 * get projection net: Wulff or Schmidt *
 ****************************************/

direction->net = gamgi_phys_projection_wulff;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_schmidt");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  direction->net = gamgi_phys_projection_schmidt;

/************************************
 * get direction model: pole, trace *
 ************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_pole");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  *pole = TRUE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_trace");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  *trace = TRUE;

/**********************************************
 * Something in the direction must be visible *
 **********************************************/

if (*pole == FALSE && *trace == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Data", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_ok_view (gamgi_direction *direction, 
gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

/***********************
 * get direction color *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &direction->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &direction->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &direction->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

/***********************
 * get direction scale *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &direction->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_direction (direction);
  return FALSE;
  }

return TRUE;
}

/*************** internal function *****************
 *                                                 *
 *                    STATIC_OK                    *
 *                                                 *
 * This is the function that actually creates      *
 * new directions. It is triggered when the user   *
 * presses Ok or when she/he cliks on the screen,  *
 * to indicate the position of the new directions. *
 *                                                 *
 ***************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_cell *cell;
gamgi_direction *direction;
gamgi_atom *atom1, *atom2;
gamgi_object *parent;
gamgi_dlist *dlist;
GtkWidget *button;
double zero[3];
double thickness;
int hkl1[3], hkl2[3];
int start1, end1;
int start2, end2;
gamgi_enum pole, trace;
gamgi_enum structure, filter;

direction = gamgi_engine_create_direction ();
gamgi_engine_start_direction (direction);

/******************************************
 * get name (set empty default, to handle *
 * it later, when u,v,w indices are known *
 ******************************************/

if (gamgi_gtk_object_name (dialog, "entry_direction",
"", direction->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Direction name", window);
  gamgi_engine_remove_direction (direction);
  return;
  }

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

atom1 = atom2 = NULL; cell = NULL;
pole = trace = FALSE;

/********************************
 * get reference: cell or atoms *
 ********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /****************************************
   * this is a crystallographic direction *
   ****************************************/

  if (static_ok_cell (direction, &cell, window) == FALSE) return;
  if (cell->model == GAMGI_PHYS_PROJECTION)
    {
    if (static_ok_projection (direction, &pole, &trace, window) == FALSE) return;
    }
  else
    {
    if (static_ok_scope (direction, cell, &thickness,
    &structure, &filter, window) == FALSE) return;
    }
  }
else
  {
  /*******************************
   * this is an atomic direction *
   *******************************/

  if (static_ok_atoms (direction, &atom1, &atom2, window) == FALSE) return;
  if (static_ok_scope (direction, NULL, &thickness,
  &structure, &filter, window) == FALSE) return;
  }
if (static_ok_view (direction, window) == FALSE) return;

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*****************************************
 * create and link the direction objects *
 *****************************************/

if (direction->reference == GAMGI_ENGINE_CELL)
  {
  /****************************************
   * this is a crystallographic direction *
   ****************************************/

  dlist = NULL;
  if (cell->model != GAMGI_PHYS_PROJECTION)
    {
    dlist = gamgi_engine_dlist_atom_object (GAMGI_CAST_OBJECT cell, NULL);
    dlist = gamgi_engine_filter_atom (dlist, 0, filter,
    GAMGI_ENGINE_DIRECTION, GAMGI_CAST_OBJECT cell);
    }

  if (gamgi_gtk_direction_range_get (cell->lattice, direction->vectors,
  hkl1, hkl2, &start1, &end1, &start2, &end2) == TRUE)
    gamgi_phys_direction_range (direction, cell, structure,
    filter, thickness, dlist, hkl1, start1, end1, hkl2, start2, end2);
  else
    gamgi_gtk_direction_list_create (direction, cell,
    structure, filter, thickness, dlist, pole, trace);

  gamgi_gtk_direction_list_cancel (NULL, window);
  while (dlist != NULL)
    dlist = gamgi_engine_dlist_remove_start (dlist);
  gamgi_engine_remove_direction (direction);
  }

if (direction->reference == GAMGI_ENGINE_ATOM)
  {
  /*************************************************************
   * this is an atomic direction: 1) get list of parent atoms  *
   * 2) reorder list so the two defining atoms come first      *
   *                                                           *
   * the direction type is needed because of the rotation next *
   *************************************************************/

  if (strcmp (direction->object.name, "") == 0)
    strcpy (direction->object.name, "Direction");
  direction->model = GAMGI_PHYS_LINE;
  direction->vectors = GAMGI_PHYS_DIRECTION_VECTORS;

  parent = gamgi_phys_direction_parent (atom1, atom2, window);
  dlist = gamgi_engine_dlist_atom_object (parent, NULL);
  dlist = gamgi_phys_direction_reorder (dlist, atom1, atom2);
  dlist = gamgi_engine_filter_atom (dlist, 2, filter, GAMGI_ENGINE_DIRECTION, parent);
  if (parent->class <= GAMGI_ENGINE_DIRECTION) parent = GAMGI_CAST_OBJECT window->layer;

  /**********************************************************
   * reset and build direction line AFTER linking to parent *
   **********************************************************/

  gamgi_engine_link_object_object (GAMGI_CAST_OBJECT direction, parent);

  gamgi_math_vector_zero (zero);
  gamgi_math_position_direction_rotation_set (direction, zero);
  gamgi_math_position_direction_translation_set (direction, zero);

  gamgi_phys_direction_atoms (direction, atom1,
  atom2, dlist, structure, thickness, TRUE);
  }

/************************   
 * Redraw gl_area image *
 ************************/
 
gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
GtkRequisition size;

/***************************************************
 * Set notebook current size as its minimum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
gtk_widget_set_size_request (notebook, size.width, size.height);

/*********************************************
 * initialize vectors, switch off projection *
 *********************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_DIRECTION_VECTORS - 1);
static_cell (NULL, window);

/*****************************
 * initialize red,green,blue *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->direction->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->direction->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->direction->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************************************
 * initialize data in second level dialogs *
 *******************************************/

gamgi_gtk_direction_range_start (window);
gamgi_gtk_direction_list_start (window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char string[2 * GAMGI_ENGINE_TOKEN];

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
else
  {
  entry = static_atom (dialog);
  if (entry == NULL)
    {
    static_end_atom (window);
    entry = static_atom (dialog);
    }

  }

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

void gamgi_gtk_direction_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;

/******************************
 * local mouse selection only *
 ******************************/

if (window_dialog != window_mouse) return;

/**********************************
 * press reference: cell or atoms *
 **********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  gamgi_mesa_select_object (window_mouse, x, y, 
  GAMGI_ENGINE_CELL, FALSE, static_press);
else
  gamgi_mesa_select_object (window_mouse, x, y, 
  GAMGI_ENGINE_ATOM, FALSE, static_press);
}

void gamgi_gtk_direction_create_node (gamgi_bool node, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_o1, *entry_o2, *entry_o3;
GtkWidget *label_o1, *label_o2, *label_o3, *label_o4;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

entry_o1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
entry_o2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
entry_o3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");

label_o1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_o1");
label_o2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_o2");
label_o3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_o3");
label_o4 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_o4");

/*******************************************
 * the o4 combo box should be 000 when     *
 * Node is started and when Node is closed *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

/****************************************
 * enable when Cell and Node are chosen *
 ****************************************/

if (node == TRUE)
  {
  gtk_widget_set_sensitive (combo, TRUE);

  gtk_widget_set_sensitive (entry_o1, TRUE);
  gtk_widget_set_sensitive (entry_o2, TRUE);
  gtk_widget_set_sensitive (entry_o3, TRUE);

  gtk_widget_set_sensitive (label_o1, TRUE);
  gtk_widget_set_sensitive (label_o2, TRUE);
  gtk_widget_set_sensitive (label_o3, TRUE);
  gtk_widget_set_sensitive (label_o4, TRUE);

  sprintf (token, "%d", GAMGI_PHYS_NODE_O1);
  gtk_entry_set_text (GTK_ENTRY (entry_o1), token);
  sprintf (token, "%d", GAMGI_PHYS_NODE_O2);
  gtk_entry_set_text (GTK_ENTRY (entry_o2), token);
  sprintf (token, "%d", GAMGI_PHYS_NODE_O3);
  gtk_entry_set_text (GTK_ENTRY (entry_o3), token);
  }
else
  {
  gtk_widget_set_sensitive (combo, FALSE);

  gtk_entry_set_text (GTK_ENTRY (entry_o1), "");
  gtk_entry_set_text (GTK_ENTRY (entry_o2), "");
  gtk_entry_set_text (GTK_ENTRY (entry_o3), "");

  gtk_widget_set_sensitive (entry_o1, FALSE);
  gtk_widget_set_sensitive (entry_o2, FALSE);
  gtk_widget_set_sensitive (entry_o3, FALSE);

  gtk_widget_set_sensitive (label_o1, FALSE);
  gtk_widget_set_sensitive (label_o2, FALSE);
  gtk_widget_set_sensitive (label_o3, FALSE);
  gtk_widget_set_sensitive (label_o4, FALSE);
  }

}

void gamgi_gtk_direction_create_uvw (gamgi_bool active, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_u, *entry_v, *entry_w;
GtkWidget *label_u, *label_v, *label_w;

entry_u = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
entry_v = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
entry_w = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
label_u = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_u");
label_v = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v");
label_w = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_w");

/**************************************************************
 * enable when one uvw diretcion family is directly specified *
 **************************************************************/

if (active == TRUE)
  {
  gtk_widget_set_sensitive (entry_u, TRUE);
  gtk_widget_set_sensitive (entry_v, TRUE);
  gtk_widget_set_sensitive (entry_w, TRUE);
  gtk_widget_set_sensitive (label_u, TRUE);
  gtk_widget_set_sensitive (label_v, TRUE);
  gtk_widget_set_sensitive (label_w, TRUE);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_u), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v), "");
  gtk_entry_set_text (GTK_ENTRY (entry_w), "");

  gtk_widget_set_sensitive (entry_u, FALSE);
  gtk_widget_set_sensitive (entry_v, FALSE);
  gtk_widget_set_sensitive (entry_w, FALSE);
  gtk_widget_set_sensitive (label_u, FALSE);
  gtk_widget_set_sensitive (label_v, FALSE);
  gtk_widget_set_sensitive (label_w, FALSE);
  }

}

/******************** external function *****************
 *                                                      *
 *              GAMGI_GTK_DIRECTION_CREATE              *
 *                                                      *
 * Creates the dialog window used to create directions. *
 *                                                      *
 ********************************************************/

void gamgi_gtk_direction_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *button;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Direction Create", window);
window->action = GAMGI_GTK_DIRECTION_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "destroy",
G_CALLBACK (gamgi_gtk_direction_list_destroy), window);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/******************
 * Direction name *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Name");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_direction", entry);
gtk_widget_show (entry);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_signal_connect (notebook, "switch_page",
G_CALLBACK (static_switch), window);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*************
 * Type page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Type ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * Type *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Reference");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Cell");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_reference), window);
g_object_set_data (G_OBJECT (dialog), "button_cell", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Atoms");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_reference), window);
gtk_widget_show (button);

/********
 * Cell *
 ********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_cell", vbox_top_top);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 10);
gtk_widget_show (table);

label = gtk_label_new ("Cell");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_signal_connect (entry, "changed",
G_CALLBACK (static_cell), window);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
gtk_widget_show (entry);

/*********************
 * Direction indices *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Indices");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("U");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_u", label);
gtk_widget_show (label);

label = gtk_label_new ("V");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_v", label);
gtk_widget_show (label);

label = gtk_label_new ("W");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_w", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_u", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_v", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_w", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label (" List ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_direction_list), window);
g_object_set_data (G_OBJECT (dialog), "button_list", button);
gtk_widget_show (button);

/*********************
 * Vectors combo box *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_vectors), window);
g_object_set_data (G_OBJECT (dialog), "combo_vectors", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********************
 * Atom-based method *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_atom", vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom2", entry);
gtk_widget_show (entry);

/*******************
 * Projection page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Projection");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_projection", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * Projection *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Net");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Wulff");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_wulff", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Schmidt");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_schmidt", button);
gtk_widget_show (button);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_toggle_button_new_with_label (" Pole ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_pole", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label (" Trace ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_trace", button);
gtk_widget_show (button);

/**************
 * Scope page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Scope ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_scope", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*************
 * Structure *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Structure");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_structure", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Copy", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Link", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * Filter *
 **********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Filter");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_filter", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Class", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Parent", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Identical", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*************
 * Thickness *
 *************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Thickness");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_thickness", entry);
gtk_widget_show (entry);

/**************
 * Directions *
 **************/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Node"); 
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_direction_range), window);
g_object_set_data (G_OBJECT (dialog), "button_node", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Range");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_direction_range), window);
g_object_set_data (G_OBJECT (dialog), "button_range", button);
gtk_widget_show (button);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 8, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("O1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_o1", label);
gtk_widget_show (label);

label = gtk_label_new ("O2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_o2", label);
gtk_widget_show (label);

label = gtk_label_new ("O3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_o3", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o3", entry);
gtk_widget_show (entry);

/********************************************************
 * o4 combo box, with code to change sensitive property *
 ********************************************************/

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "combo_o4", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "000", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "011", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "101", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "110", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "111", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "122", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "211", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (7);
g_object_set_data (G_OBJECT (dialog), "sensitive_o4", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

label = gtk_label_new ("O4");
gtk_table_attach (GTK_TABLE (table), label, 8, 9, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_o4", label);
gtk_widget_show (label);

/*************
 * View page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********
 * Color *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Color");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_SHRINK, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_blue", entry);
gtk_widget_show (entry);

/*********
 * Scale *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_scale", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_scale", entry);
gtk_widget_show (entry);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
