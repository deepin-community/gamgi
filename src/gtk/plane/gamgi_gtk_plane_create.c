/*************************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_create.c
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
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_plane_create.h"
#include "gamgi_gtk_plane_node.h"
#include "gamgi_gtk_plane_list.h"
#include "gamgi_mesa_select.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_position.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"
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
 * level o4 menu might give nodes that are not valid anymore.        *
 *********************************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_range");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  gtk_button_clicked (GTK_BUTTON (button));
}

static void static_order_start (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/******************************
 * disable/enable order entry *
 ******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  sprintf (token, "%d", GAMGI_PHYS_PLANE_RANGE_START);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_order_end (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/******************************
 * disable/enable order entry *
 ******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  sprintf (token, "%d", GAMGI_PHYS_PLANE_RANGE_END);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

}

static void static_start_scope (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button_cell;
GtkWidget *button_node;
GtkWidget *button_range;
GtkWidget *combo;
GtkWidget *entry;
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
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_PLANE_THICKNESS);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************************************
 * range: 1) initially the button range *
 * is active, even if range is disabled *
 * 2) enable, disable radio buttons     *
 * 3) initialize range limits           *
 ****************************************/

button_cell = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
button_node = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_node");
button_range = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_range");

gtk_button_clicked (GTK_BUTTON (button_range));
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
gamgi_gtk_plane_create_range (active, window);

/************************
 * disable second level *
 ************************/

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

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PLANE_VECTORS - 1);
}

static void static_end_atom (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
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

/**********
 * atom 3 *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return entry;

return NULL;
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

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/**************************
 * reset atom identifiers *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
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
  gamgi_gtk_plane_node_cancel (NULL, window);
}

static gamgi_bool static_ok_cell (gamgi_plane *plane, gamgi_cell **cell, 
gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
char token[GAMGI_ENGINE_TOKEN] = "";

plane->reference = GAMGI_ENGINE_CELL;

/**********************
 * get reference cell *
 **********************/

*cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (*cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Object", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/*********************
 * Get plane indices *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &plane->hkl[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &plane->hkl[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &plane->hkl[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_phys_direction_zero (plane->hkl) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/*****************************************
 * get plane name (automatic hkl indices *
 * are handled later, when generated)    *
 *****************************************/

if (gtk_widget_is_sensitive (entry) == TRUE)
  sprintf (token, "(%d.%d.%d)", plane->hkl[0], plane->hkl[1], plane->hkl[2]);
if (gamgi_gtk_object_name (dialog, "entry_plane",
token, plane->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane name", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/********************
 * get cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
plane->vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

return TRUE;
}

static gamgi_bool static_ok_atoms (gamgi_plane *plane, 
gamgi_atom **atom1, gamgi_atom **atom2, gamgi_atom **atom3, 
gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;

plane->reference = GAMGI_ENGINE_ATOM;

/***********************
 * get reference atoms *
 ***********************/

*atom1 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom1", GAMGI_ENGINE_ATOM, window);
if (*atom1 == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

*atom2 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom2", GAMGI_ENGINE_ATOM, window);
if (*atom2 == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

*atom3 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom3", GAMGI_ENGINE_ATOM, window);
if (*atom3 == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/**************************************
 * atoms must be far enough from each *
 * other to define a proper polygon   *
 **************************************/

if (gamgi_phys_plane_polygon (*atom1, *atom2, *atom3) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom positions", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_ok_scope (gamgi_plane *plane, gamgi_cell *cell,
double *thickness, gamgi_enum *structure, gamgi_enum *filter, int *start,
int *end, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
int o1, o2, o3;

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
  gamgi_gtk_dialog_message_create ("Error", "Invalid Thickness Data", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/****************************************
 * return control for atomic planes,    *
 * continue for crystallographic planes *
 ****************************************/

if (cell == NULL) return TRUE;

/*********************************************************
 * get order range: when both buttons were pressed and   *
 * entries are disabled, start = -INT_MAX, end = INT_MAX *
 *********************************************************/

*start = -INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, start, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Order", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

*end = INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, end, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Order", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/***********************************************************
 * get order range: when a node was specified, start = end *
 ***********************************************************/

if (gamgi_gtk_plane_node_get (cell, &o1, &o2, &o3, window) == TRUE)
  {
  *start = *end = gamgi_phys_plane_order (cell,
  plane->hkl, plane->vectors, o1, o2, o3);
  }

if (*start > *end)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Order", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_ok_projection (gamgi_plane *plane, gamgi_bool *pole,
gamgi_bool *trace, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/************************************
 * get projection: Wulff or Schmidt *
 ************************************/

plane->net = gamgi_phys_projection_wulff;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_schmidt");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  plane->net = gamgi_phys_projection_schmidt;

/********************************
 * get plane model: pole, trace *
 ********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_pole");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  *pole = TRUE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_trace");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  *trace = TRUE;

/******************************************
 * Something in the plane must be visible *
 ******************************************/

if (*pole == FALSE && *trace == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Data", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_ok_view (gamgi_plane *plane, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

/*******************
 * get plane color *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &plane->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &plane->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &plane->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Color", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

/*******************
 * get plane scale *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &plane->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_plane (plane);
  return FALSE;
  }

return TRUE;
}

/*************** internal function *******************
 *                                                   *
 *                    STATIC_OK                      *
 *                                                   *
 * This is the function that actually creates new    *
 * planes. It is triggered when the user presses Ok. *
 *                                                   *
 *****************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_cell *cell;
gamgi_plane *plane;
gamgi_atom *atom1, *atom2, *atom3;
gamgi_object *parent;
gamgi_dlist *dlist;
GtkWidget *button;
double zero[3];
double thickness;
int start, end;
gamgi_enum pole, trace;
gamgi_enum structure, filter;

plane = gamgi_engine_create_plane ();
gamgi_engine_start_plane (plane);

/******************************************
 * get name (set empty default, to handle *
 * it later, when h,k,l indices are known *
 ******************************************/

if (gamgi_gtk_object_name (dialog, "entry_plane",
"", plane->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane name", window);
  gamgi_engine_remove_plane (plane);
  return;
  }

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

atom1 = atom2 = atom3 = NULL; cell = NULL;
pole = trace = FALSE;

/********************************
 * get reference: cell or atoms *
 ********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /************************************
   * this is a crystallographic plane *
   ************************************/

  if (static_ok_cell (plane, &cell, window) == FALSE) return;
  if (cell->model == GAMGI_PHYS_PROJECTION)
    {
    if (static_ok_projection (plane, &pole, &trace, window) == FALSE) return;
    }
  else
    {
    if (static_ok_scope (plane, cell, &thickness, &structure,
    &filter, &start, &end, window) == FALSE) return;
    }
  }
else
  {
  /***************************
   * this is an atomic plane *
   ***************************/

  if (static_ok_atoms (plane, &atom1, &atom2,
  &atom3, window) == FALSE) return;
  if (static_ok_scope (plane, NULL, &thickness, &structure,
  &filter, &start, &end, window) == FALSE) return;
  }
if (static_ok_view (plane, window) == FALSE) return;

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*************************************
 * create and link the plane objects *
 *************************************/

if (plane->reference == GAMGI_ENGINE_CELL)
  {
  /************************************
   * this is a crystallographic plane *
   ************************************/

  dlist = NULL;
  if (cell->model != GAMGI_PHYS_PROJECTION)
    {
    dlist = gamgi_engine_dlist_atom_object (GAMGI_CAST_OBJECT cell, NULL);
    dlist = gamgi_engine_filter_atom (dlist, 0, filter,
    GAMGI_ENGINE_PLANE, GAMGI_CAST_OBJECT cell);
    }

  gamgi_gtk_plane_list_create (plane, cell, structure,
  filter, thickness, dlist, start, end, pole, trace);

  gamgi_gtk_plane_list_cancel (NULL, window);
  while (dlist != NULL)
    dlist = gamgi_engine_dlist_remove_start (dlist);
  gamgi_engine_remove_plane (plane);
  }

if (plane->reference == GAMGI_ENGINE_ATOM)
  {
  /**********************************************************
   * this is an atomic plane: 1) get list of parent atoms   *
   * 2) reorder list so the three defining atoms come first *
   *                                                        *
   * the plane type is needed because of the rotation next  *
   **********************************************************/

  if (strcmp (plane->object.name, "") == 0)
    strcpy (plane->object.name, "Plane");
  plane->model = GAMGI_PHYS_POLYGON;
  plane->vectors = GAMGI_PHYS_PLANE_VECTORS;

  parent = gamgi_phys_plane_parent (atom1, atom2, atom3, window);
  dlist = gamgi_engine_dlist_atom_object (parent, NULL);
  dlist = gamgi_phys_plane_reorder (dlist, atom1, atom2, atom3);
  dlist = gamgi_engine_filter_atom (dlist, filter, 3, GAMGI_ENGINE_PLANE, parent);
  if (parent->class <= GAMGI_ENGINE_PLANE) parent = GAMGI_CAST_OBJECT window->layer;

  /*********************************************************
   * reset and build plane polygon AFTER linking to parent *
   *********************************************************/

  gamgi_engine_link_object_object (GAMGI_CAST_OBJECT plane, parent);

  gamgi_math_vector_zero (zero);
  gamgi_math_position_plane_rotation_set (plane, zero);
  gamgi_math_position_plane_translation_set (plane, zero);

  gamgi_phys_plane_atoms (plane, atom1,
  atom2, atom3, dlist, structure, thickness);
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
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PLANE_VECTORS - 1);
static_cell (NULL, window);

/************************************
 * initialize red,green,blue, scale *
 ************************************/
 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->plane->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->plane->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->plane->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************************************
 * initialize data in second level dialogs *
 *******************************************/

gamgi_gtk_plane_node_start (window);
gamgi_gtk_plane_list_start (window);
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
  /***********************
   * get next atom entry *
   ***********************/

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

void gamgi_gtk_plane_create_press (gamgi_window *window_mouse,
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
  gamgi_mesa_select_object (window_dialog, x, y, 
  GAMGI_ENGINE_CELL, FALSE, static_press);
else
  gamgi_mesa_select_object (window_dialog, x, y, 
  GAMGI_ENGINE_ATOM, FALSE, static_press);
}

void gamgi_gtk_plane_create_range (gamgi_bool active, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button_start, *button_end;
GtkWidget *entry_start, *entry_end;
GtkWidget *label;
char token[GAMGI_ENGINE_TOKEN];

button_start = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_start");
button_end = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_end");
entry_start = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start");
entry_end = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_order");

/********************************
 * buttons should be up when    *
 * range is enabled or disabled *
 ********************************/

gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_start), FALSE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_end), FALSE);

/*****************************************
 * enable when Cell and Range are chosen *
 *****************************************/

if (active == TRUE)
  {
  gtk_widget_set_sensitive (button_start, TRUE);
  gtk_widget_set_sensitive (button_end, TRUE);
  gtk_widget_set_sensitive (entry_start, TRUE);
  gtk_widget_set_sensitive (entry_end, TRUE);
  gtk_widget_set_sensitive (label, TRUE);

  sprintf (token, "%d", GAMGI_PHYS_PLANE_RANGE_START);
  gtk_entry_set_text (GTK_ENTRY (entry_start), token);
  sprintf (token, "%d", GAMGI_PHYS_PLANE_RANGE_END);
  gtk_entry_set_text (GTK_ENTRY (entry_end), token);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_start), "");
  gtk_entry_set_text (GTK_ENTRY (entry_end), "");

  gtk_widget_set_sensitive (button_start, FALSE);
  gtk_widget_set_sensitive (button_end, FALSE);
  gtk_widget_set_sensitive (entry_start, FALSE);
  gtk_widget_set_sensitive (entry_end, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
}

void gamgi_gtk_plane_create_hkl (gamgi_bool active, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_h, *entry_k, *entry_l;
GtkWidget *label_h, *label_k, *label_l;

entry_h = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
entry_k = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
entry_l = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
label_h = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_h");
label_k = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_k");
label_l = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_l");

/**********************************************************
 * enable when one hkl plane family is directly specified *
 **********************************************************/

if (active == TRUE)
  {
  gtk_widget_set_sensitive (entry_h, TRUE);
  gtk_widget_set_sensitive (entry_k, TRUE);
  gtk_widget_set_sensitive (entry_l, TRUE);
  gtk_widget_set_sensitive (label_h, TRUE);
  gtk_widget_set_sensitive (label_k, TRUE);
  gtk_widget_set_sensitive (label_l, TRUE);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_h), "");
  gtk_entry_set_text (GTK_ENTRY (entry_k), "");
  gtk_entry_set_text (GTK_ENTRY (entry_l), "");

  gtk_widget_set_sensitive (entry_h, FALSE);
  gtk_widget_set_sensitive (entry_k, FALSE);
  gtk_widget_set_sensitive (entry_l, FALSE);
  gtk_widget_set_sensitive (label_h, FALSE);
  gtk_widget_set_sensitive (label_k, FALSE);
  gtk_widget_set_sensitive (label_l, FALSE);
  }
  
}

/*************** external function ******************
 *                                                  *
 *            GAMGI_GTK_PLANE_CREATE                *
 *                                                  *
 * Creates the dialog window used to create planes. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_plane_create (GtkWidget *widget, void *data)
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

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Plane Create", window);
window->action = GAMGI_GTK_PLANE_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "destroy",
G_CALLBACK (gamgi_gtk_plane_list_destroy), window);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/**************
 * Plane name *
 **************/

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
g_object_set_data (G_OBJECT (dialog), "entry_plane", entry);
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

/*************
 * Reference *
 *************/

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

/*****************
 * Plane indices *
 *****************/

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

label = gtk_label_new ("H");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_h", label);
gtk_widget_show (label);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_k", label);
gtk_widget_show (label);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_l", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_h", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_k", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "entry_l", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label (" List ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_plane_list), window);
g_object_set_data (G_OBJECT (dialog), "button_list", button);
gtk_widget_show (button);

/***********************
 * Vectors option menu *
 ***********************/

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
G_CALLBACK (static_instance), window);
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

table = gtk_table_new (3, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
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

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom3", entry);
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

/***************
 * Node, Order *
 ***************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
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
G_CALLBACK (gamgi_gtk_plane_node), window);
g_object_set_data (G_OBJECT (dialog), "button_node", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Range");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_plane_node), window);
g_object_set_data (G_OBJECT (dialog), "button_range", button);
gtk_widget_show (button);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Order");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_order", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_start", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Start");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_order_start), window);
g_object_set_data (G_OBJECT (dialog), "button_start", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_end", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("End");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_order_end), window);
g_object_set_data (G_OBJECT (dialog), "button_end", button);
gtk_widget_show (button);

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
