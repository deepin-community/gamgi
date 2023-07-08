/*******************************************************
 *
 * $GAMGI/src/gtk/molecule/gamgi_gtk_molecule_create.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_molecule_link.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_graphene.h"
#include "gamgi_chem_graphene_cone.h"
#include "gamgi_chem_graphene_tube.h"
#include "gamgi_io_token.h"

enum { STATIC_OBJECT, STATIC_TUBE, STATIC_CONE };

static void static_length (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
const char *name1, *name2;
double length;
int element1, element2;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
name1 = gtk_entry_get_text (GTK_ENTRY (entry));
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
name2 = gtk_entry_get_text (GTK_ENTRY (entry));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_length");

if (gamgi_io_token_alpha_scan (name1, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == TRUE &&
gamgi_chem_atom_number (token, &element1) == TRUE &&
gamgi_io_token_alpha_scan (name2, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == TRUE &&
gamgi_chem_atom_number (name2, &element2) == TRUE)
  {
  length = gamgi->atom->radius[element1] + gamgi->atom->radius[element2];
  sprintf (token, "%.*f", gamgi->gamgi->length, length);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_height (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *entry;
GtkWidget *label;
char token[GAMGI_ENGINE_TOKEN];

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_sheet_height");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_height");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  gtk_widget_set_sensitive (label, TRUE);
  gtk_widget_set_sensitive (entry, TRUE);
  sprintf (token, "%d", GAMGI_CHEM_GRAPHENE_RINGS);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
}

static void static_angle (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkListStore *store;
GtkTreeIter iter;
int row;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_angle");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/****************************************************************
 * create combo box list: the first option, a single rotation   *
   around the origin, is always avalable. As the combo box is   *
   not resized, the Origin option should be the longest option. *
 ****************************************************************/

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Origin", -1);

if (row == 60/60)
  {
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Double", -1);
  }
if (row == 120/60)
  {
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Edge", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Hexagon", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Single", -1);
  }
if (row == 180/60)
  {
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Double", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Single", -1);
  }
if (row == 240/60)
  {
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Edge", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Hexagon", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Plane", -1);
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Axis", -1);
  }
if (row == 300/60)
  {
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, 0, "Edge", -1);
  }

/*********************************************************
 * set model (and automatically remove the previous one) *
 *********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_isomer");
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

/************************************
 * set Origin isomer as the default *
 ************************************/

gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_start_atoms (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
gamgi_chem_atom_name (GAMGI_CHEM_GRAPHENE_ELEMENT_A, token);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
gamgi_chem_atom_name (GAMGI_CHEM_GRAPHENE_ELEMENT_B, token);
gtk_entry_set_text (GTK_ENTRY (entry), token);

static_length (NULL, window);
}

static void static_start_sheet (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *entry;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
gtk_button_clicked (GTK_BUTTON (button));
static_height (NULL, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_m");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_n");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_start_tube (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_m");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_n");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_height");
sprintf (token, "%d", GAMGI_CHEM_GRAPHENE_RINGS);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_start_cone (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_angle");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_GRAPHENE_ANGLE / 60);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_in");
sprintf (token, "%d", GAMGI_CHEM_GRAPHENE_RINGS_IN);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_out");
sprintf (token, "%d", GAMGI_CHEM_GRAPHENE_RINGS_OUT);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_start_cage (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cage");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cage_height");
gtk_entry_set_text (GTK_ENTRY (entry), "0");
}

static void static_start_cap (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cap");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);
}

static void static_method (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *vbox_sheet, *vbox_tube, *vbox_cone, *vbox_cage, *vbox_cap;
GtkWidget *vbox_atoms;
GtkTreeModel *model;
GtkTreeIter iter;
char *name;

vbox_sheet = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_sheet");
vbox_tube = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_tube");
vbox_cone = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cone");
vbox_cage = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cage");
vbox_cap = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cap");
vbox_atoms = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_atoms");

/*******************
 * get Method name *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter);
gtk_tree_model_get (model, &iter, 0, &name, -1);

if (strcmp (name, "Object") == 0)
  {
  gtk_widget_hide (vbox_sheet);
  gtk_widget_hide (vbox_tube);
  gtk_widget_hide (vbox_cone);
  gtk_widget_hide (vbox_cage);
  gtk_widget_hide (vbox_cap);
  gtk_widget_hide (vbox_atoms);
  }
else if (strcmp (name, "Sheet") == 0)
  {
  gtk_widget_hide (vbox_tube);
  gtk_widget_hide (vbox_cone);
  gtk_widget_hide (vbox_cage);
  gtk_widget_hide (vbox_cap);
  gtk_widget_show (vbox_sheet);
  static_start_sheet (window);
  gtk_widget_show (vbox_atoms);
  static_start_atoms (window);
  }
else if (strcmp (name, "Tube") == 0)
  {
  gtk_widget_hide (vbox_sheet);
  gtk_widget_hide (vbox_cone);
  gtk_widget_hide (vbox_cage);
  gtk_widget_hide (vbox_cap);
  gtk_widget_show (vbox_tube);
  static_start_tube (window);
  gtk_widget_show (vbox_atoms);
  static_start_atoms (window);
  }
else if (strcmp (name, "Cone") == 0)
  {
  gtk_widget_hide (vbox_sheet);
  gtk_widget_hide (vbox_tube);
  gtk_widget_hide (vbox_cage);
  gtk_widget_hide (vbox_cap);
  gtk_widget_show (vbox_cone);
  static_start_cone (window);
  gtk_widget_show (vbox_atoms);
  static_start_atoms (window);
  }
else if (strcmp (name, "Cage") == 0)
  {
  gtk_widget_hide (vbox_sheet);
  gtk_widget_hide (vbox_tube);
  gtk_widget_hide (vbox_cone);
  gtk_widget_hide (vbox_cap);
  gtk_widget_show (vbox_cage);
  static_start_cage (window);
  gtk_widget_show (vbox_atoms);
  static_start_atoms (window);
  }
else if (strcmp (name, "Cap") == 0)
  {
  gtk_widget_hide (vbox_sheet);
  gtk_widget_hide (vbox_tube);
  gtk_widget_hide (vbox_cone);
  gtk_widget_hide (vbox_cage);
  gtk_widget_show (vbox_cap);
  static_start_cap (window);
  gtk_widget_show (vbox_atoms);
  static_start_atoms (window);
  }
}

static gamgi_bool static_ok_atoms (gamgi_molecule *molecule,
gamgi_window *window, int *element1, int *element2, double *bond)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
double length;

/*******************************************
 * get element1, element2, bond parameters *
 *******************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE ||
gamgi_chem_atom_number (token, element1) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid Atom Element Data", window);
  gamgi_engine_remove_molecule (molecule);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE ||
gamgi_chem_atom_number (token, element2) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid Atom Element Data", window);
  gamgi_engine_remove_molecule (molecule);
  return FALSE;
  }

length = gamgi->atom->radius[*element1] + gamgi->atom->radius[*element2];
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_length");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name,
bond, length / 2, length * 2) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid Bond Size Data", window);
  gamgi_engine_remove_molecule (molecule);
  return FALSE;
  }

return TRUE;
}

static void static_ok_object (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  strcpy (molecule->object.name, "Molecule");

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*************************************
 *  launch a Molecule Link dialog    *
 *  containing already the name and  *
 *  number of the new molecule.      *
 *                                   *
 *   The old dialog is removed       *
 *   before creating the new one     *
 *                                   *  
 * no need to redraw gl_area because *
 * an empty molecule is invisible    *
 *************************************/

gamgi_gtk_molecule_link (NULL, window);

/***********************************
 * Insert molecule name and number *
 * in new dialog molecule entry    *
 ***********************************/

dialog = window->dialog0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_molecule");
sprintf (string, "%s %d", molecule->object.name, molecule->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_ok_sheet (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
const char *name;
double bond;
int m, n, height, element1, element2;

/*******************************
 * get m, n, height parameters *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_m");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &m, 0, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid sheet coordinate data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_n");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &n, 0, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid sheet coordinate data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

if (m == 0 && n == 0)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid sheet coordinate data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

height = -1;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sheet_height");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_int_scan (name, &height, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid sheet height data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

/**************************************************
 * get element1, element2, bond length parameters *
 **************************************************/

if (static_ok_atoms (molecule, window,
&element1, &element2, &bond) == FALSE) return;

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  strcpy (molecule->object.name, "Graphene");

/*************************
 * create graphene sheet *
 *************************/

gamgi_chem_graphene_sheet (molecule, m, n, height, element1, element2, bond);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
gamgi_gtk_dialog_task0_remove (NULL, window);
}

static void static_ok_tube (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
double bond;
int m, n, height, element1, element2;

/*******************************
 * get m, n, height parameters *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_m");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &m, 0, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid tube coordinate data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_n");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &n, 0, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid tube coordinate data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

if (m == 0 && n == 0)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid tube coordinate Data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tube_height");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &height, 1, INT_MAX) == FALSE)
  { 
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid tube height data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

/**************************************************
 * get element1, element2, bond length parameters *
 **************************************************/

if (static_ok_atoms (molecule, window,
&element1, &element2, &bond) == FALSE) return;

/*******************
 * create nanotube *
 *******************/

gamgi_chem_graphene_tube (molecule, m, n, height, element1, element2, bond);

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  {
  sprintf (token, "Tube_%d_%d", m, n);
  strcpy (molecule->object.name, token);
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
gamgi_gtk_dialog_task0_remove (NULL, window);
}

static void static_ok_cone (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
double bond;
int mode, in, out, element1, element2;
int angle, isomer;

/*********************
 * get nanocone mode *
 *********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_angle");
angle = 60 * gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_isomer");
isomer = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (angle == 0) mode = GAMGI_CHEM_CONE_0;

if (angle == 60 && isomer == 0) mode = GAMGI_CHEM_CONE_60_O;
if (angle == 60 && isomer == 1) mode = GAMGI_CHEM_CONE_60_D;

if (angle == 120 && isomer == 0) mode = GAMGI_CHEM_CONE_120_O;
if (angle == 120 && isomer == 1) mode = GAMGI_CHEM_CONE_120_E;
if (angle == 120 && isomer == 2) mode = GAMGI_CHEM_CONE_120_H;
if (angle == 120 && isomer == 3) mode = GAMGI_CHEM_CONE_120_S;

if (angle == 180 && isomer == 0) mode = GAMGI_CHEM_CONE_180_O;
if (angle == 180 && isomer == 1) mode = GAMGI_CHEM_CONE_180_D;
if (angle == 180 && isomer == 2) mode = GAMGI_CHEM_CONE_180_S;

if (angle == 240 && isomer == 0) mode = GAMGI_CHEM_CONE_240_O;
if (angle == 240 && isomer == 1) mode = GAMGI_CHEM_CONE_240_E;
if (angle == 240 && isomer == 2) mode = GAMGI_CHEM_CONE_240_H;
if (angle == 240 && isomer == 3) mode = GAMGI_CHEM_CONE_240_P;
if (angle == 240 && isomer == 4) mode = GAMGI_CHEM_CONE_240_A;

if (angle == 300 && isomer == 0) mode = GAMGI_CHEM_CONE_300_O;
if (angle == 300 && isomer == 1) mode = GAMGI_CHEM_CONE_300_E;

/*********************************
 * get in, out height parameters *
 *********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_out");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &out, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid Cone Size Data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_in");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &in, 0, out - 1) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid Cone Size Data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

/*******************************************
 * get element1, element2, bond parameters *
 *******************************************/

if (static_ok_atoms (molecule, window,
&element1, &element2, &bond) == FALSE) return;

/*******************
 * create nanotube *
 *******************/

gamgi_chem_graphene_cone (molecule, mode, in, out, element1, element2, bond);

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  {
  sprintf (token, "Cone_%d_%d", angle, isomer + 1);
  strcpy (molecule->object.name, token);
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
gamgi_gtk_dialog_task0_remove (NULL, window);
}

static void static_ok_cage (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
double bond;
int mode, height, element1, element2, atoms;

/********************
 * get mode, height *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cage");
mode = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cage_height");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &height, 0, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid cage height data", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

/**************************************************
 * get element1, element2, bond length parameters *
 **************************************************/

if (static_ok_atoms (molecule, window,
&element1, &element2, &bond) == FALSE) return;

/***************
 * create cage *
 ***************/

gamgi_chem_graphene_cage (molecule, mode, height, element1, element2, bond);

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  {
  atoms = 0;
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_5_0) atoms = 10 * (height + 2);
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_5_5) atoms = 10 * (height + 6);
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_6_6_S) atoms = 12 * (height + 6);
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_6_6_C) atoms = 12 * (height + 6);
  sprintf (token, "C_%d", atoms);
  strcpy (molecule->object.name, token);
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
gamgi_gtk_dialog_task0_remove (NULL, window);
}

static void static_ok_cap (gamgi_molecule *molecule, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
char word[GAMGI_ENGINE_TOKEN];
double bond;
int mode, element1, element2;

/************
 * get mode *
 ************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cap");
mode = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**************************************************
 * get element1, element2, bond length parameters *
 **************************************************/

if (static_ok_atoms (molecule, window,
&element1, &element2, &bond) == FALSE) return;

/**************
 * create cap *
 **************/

gamgi_chem_graphene_cap (molecule, mode, element1, element2, bond);

/*****************************************
 * if molecule name is empty set default *
 *****************************************/

if (gamgi_io_token_check (molecule->object.name) == FALSE)
  {
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_5_0) strcpy (word, "1P_5P");
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_5_5) strcpy (word, "1P_5H_5PH");
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_6_6_S) strcpy (word, "1H_6H_6PH_S");
  if (mode == GAMGI_CHEM_GRAPHENE_CAP_6_6_C) strcpy (word, "1H_6H_6PH_C");
  sprintf (token, "Cap_%s", word);
  strcpy (molecule->object.name, token);
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_molecule_layer (molecule, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
gamgi_gtk_dialog_task0_remove (NULL, window);
}

/******************* internal function *****************
 *                                                     *
 *                        STATIC_OK                    *
 *                                                     *
 * This is the function that actually creates a new    *
 * molecule. It is triggered when the user presses Ok. *
 *                                                     *
 *******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_molecule *molecule;
GtkWidget *combo;
GtkTreeModel *model;
GtkTreeIter iter;
char *name;

molecule = gamgi_engine_create_molecule ();
gamgi_engine_start_molecule (molecule);

/**************************************************
 * get name (the default is empty because default *
 * molecule names depend of the applied methods   *
 **************************************************/

if (gamgi_gtk_object_name (dialog, "entry_molecule",
"", molecule->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Molecule Name", window);
  gamgi_engine_remove_molecule (molecule);
  return;
  }

/**************
 * get method *
 **************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter);
gtk_tree_model_get (model, &iter, 0, &name, -1);

if (strcmp (name, "Object") == 0) static_ok_object (molecule, window);
if (strcmp (name, "Sheet") == 0) static_ok_sheet (molecule, window);
if (strcmp (name, "Tube") == 0) static_ok_tube (molecule, window);
if (strcmp (name, "Cone") == 0) static_ok_cone (molecule, window);
if (strcmp (name, "Cage") == 0) static_ok_cage (molecule, window);
if (strcmp (name, "Cap") == 0) static_ok_cap (molecule, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *combo;
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

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_OBJECT);
}

/******************* external function ****************
 *                                                    *
 *               GAMGI_GTK_MOLECULE_CREATE            *
 *                                                    *
 * Create the dialog window used to create molecules. *
 *                                                    *
 ******************************************************/

void gamgi_gtk_molecule_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *combo;
GtkListStore *store;
GtkTreeStore *tree_store;
GtkCellRenderer *renderer;
GtkTreeIter iter, child;

/******************
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Molecule Create", window);
window->action = GAMGI_GTK_MOLECULE_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Molecule name *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
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
g_object_set_data (G_OBJECT (dialog), "entry_molecule", entry);
gtk_widget_show (entry);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
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

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**********************
 * Method option menu *
 **********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Method");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_method), window);
g_object_set_data (G_OBJECT (dialog), "combo_method", combo);
gtk_widget_show (combo);

tree_store = gtk_tree_store_new (1, G_TYPE_STRING);
gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Object", -1);
gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Graphene", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Sheet", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Tube", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Cone", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Cage", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Cap", -1);

gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (tree_store));
g_object_unref (tree_store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);
g_object_set (renderer, "width-chars", GAMGI_GTK_CHAR_MENU, NULL);

gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_titles_scan, NULL, NULL);

/*******************
 * Graphene Sheets *
 *******************/

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_sheet", vbox_top_top);

/*************
 * first row *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Lattice");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_lattice", button);
g_signal_connect (button, "clicked", G_CALLBACK (static_height), window);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Tube");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_height), window);
gtk_widget_show (button);

/**************
 * second row *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("M");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_sheet_m", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("N");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_sheet_n", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Height");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_sheet_height", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_sheet_height", entry);
gtk_widget_show (entry);

/******************
 * Graphene Tubes *
 ******************/

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 15);
g_object_set_data (G_OBJECT (dialog), "vbox_tube", vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("M");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_tube_m", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("N");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_tube_n", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Height");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_tube_height", entry);
gtk_widget_show (entry);

/**********************
 * Graphene Nanocones *
 **********************/

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_cone", vbox_top_top);
gtk_widget_show (vbox_top_top);

/*************
 * first row *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Angle");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "combo_angle", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "0", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "60", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "120", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "180", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "240", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "300", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Isomer");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
gtk_widget_set_size_request (combo, 100, -1);
g_object_set_data (G_OBJECT (dialog), "combo_isomer", combo);
gtk_widget_show (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**************
 * second row *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Radius");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_in", entry);
gtk_widget_show (entry);

label = gtk_label_new ("In");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_out", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Out");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/******************
 * Graphene Cages *
 ******************/

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 15);
g_object_set_data (G_OBJECT (dialog), "vbox_cage", vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_cage", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Tube 5 0", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Tube 5 5", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Tube 6 6 S", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Tube 6 6 C", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Height");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_cage_height", entry);
gtk_widget_show (entry);

/*****************
 * Graphene Caps *
 *****************/

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 15);
g_object_set_data (G_OBJECT (dialog), "vbox_cap", vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Rings");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "combo_cap", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1P 5P", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1P 5H 5PH", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1H 6H 6PH S", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1H 6H 6PH C", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**************
 * Atoms page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Atoms");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_atoms", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**************************
 * first column: elements *
 **************************/

vbox_top = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top, TRUE, TRUE, 0);
gtk_widget_show (vbox_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Element");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed", G_CALLBACK (static_length), window);
g_object_set_data (G_OBJECT (dialog), "entry_element1", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Element");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed", G_CALLBACK (static_length), window);
g_object_set_data (G_OBJECT (dialog), "entry_element2", entry);
gtk_widget_show (entry);

/*************************
 * second column: length *
 *************************/

vbox_top = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top, TRUE, TRUE, 0);
gtk_widget_show (vbox_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Length");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_length", entry);
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
