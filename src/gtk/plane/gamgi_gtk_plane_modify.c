/*************************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_modify.c
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
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_reparent.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_gtk_plane_node.h"
#include "gamgi_gtk_plane_modify.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_NAME, STATIC_REFERENCE, STATIC_CELL, STATIC_ATOMS,
STATIC_H, STATIC_K, STATIC_L, STATIC_VECTORS, STATIC_STRUCTURE,
STATIC_FILTER, STATIC_THICKNESS, STATIC_ORDER, STATIC_NODES, STATIC_NET,
STATIC_MODEL, STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y,
STATIC_TRANSLATION_Z, STATIC_ROTATION_E1, STATIC_ROTATION_E2,
STATIC_ROTATION_E3, STATIC_RED, STATIC_GREEN, STATIC_BLUE, STATIC_SCALE };

enum { STATIC_REFERENCE_CELL, STATIC_REFERENCE_ATOMS, STATIC_REFERENCE_LOCAL };
enum { STATIC_VECTORS_CONVENTIONAL, STATIC_VECTORS_PRIMITIVE, STATIC_VECTORS_LOCAL };
enum { STATIC_STRUCTURE_COPY, STATIC_STRUCTURE_LINK, STATIC_STRUCTURE_NONE };
enum { STATIC_FILTER_CLASS, STATIC_FILTER_CHILD, STATIC_FILTER_IDENTICAL,
STATIC_FILTER_NONE };
enum { STATIC_INSTANCE_ORDER, STATIC_INSTANCE_NODE };
enum { STATIC_NET_WULFF, STATIC_NET_SCHMIDT, STATIC_NET_LOCAL };
enum { STATIC_MODEL_POLE, STATIC_MODEL_TRACE, STATIC_MODEL_LOCAL };

static struct {
gamgi_bool local;
gamgi_bool new[STATIC_SCALE + 1];
char name[GAMGI_ENGINE_TOKEN];
gamgi_enum reference;
gamgi_cell *cell;
gamgi_atom *atom1, *atom2, *atom3;
int hkl[3];
gamgi_enum vectors;
gamgi_enum structure, filter;
double thickness;
int order;
int node[4];
gamgi_net net;
gamgi_enum model;
double translation[3], rotation[3];
float red, green, blue;
double scale; } cache;

static void static_name (gamgi_window *window, char *name)
{
gamgi_plane *plane;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char string[GAMGI_ENGINE_STRING];
int active;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");

plane = GAMGI_CAST_PLANE gamgi_gtk_object_number (dialog,
"entry_plane", GAMGI_ENGINE_PLANE, window, string);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

if (active == TRUE || plane == NULL)
  {
  sprintf (string, "%s", name);
  }
else
  {
  if (strcmp (name, "") == 0)
    sprintf (string, "%s %d", plane->object.name, plane->object.number);
  else
    sprintf (string, "%s %d", name, plane->object.number);
  }

gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_indices (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN] = "";
const char *name;
gamgi_bool out;
int h, k, l;

/*********************
 * Get plane indices *
 *********************/

out = TRUE;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
out = out && gamgi_io_token_int_scan (name, &h, -INT_MAX, INT_MAX);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
out = out && gamgi_io_token_int_scan (name, &k, -INT_MAX, INT_MAX);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
out = out && gamgi_io_token_int_scan (name, &l, -INT_MAX, INT_MAX);

out = out && (h != 0 || k != 0 || l != 0);

/*********************
 * update plane name *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");
if (out == TRUE) gamgi_phys_plane_name (h, k, l, token);
static_name (window, token);
}

static void static_atom_remove (gamgi_plane *plane, gamgi_bool *link)
{
if (cache.structure != GAMGI_CHEM_STRUCTURE_LINK)
  {
  if (link[0] == TRUE)
    { gamgi_engine_unlink_atom (cache.atom1); gamgi_engine_remove_atom (cache.atom1); }
  if (link[1] == TRUE)
    { gamgi_engine_unlink_atom (cache.atom2); gamgi_engine_remove_atom (cache.atom2); }
  if (link[2] == TRUE)
    { gamgi_engine_unlink_atom (cache.atom3); gamgi_engine_remove_atom (cache.atom3); }
  }
}

static gamgi_bool static_atom_parent (gamgi_plane *plane, gamgi_atom *atom)
{
if (gamgi_engine_find_atom_plane (atom->object.number, plane) != NULL)
  {
  gamgi_engine_unlink_atom (atom);
  gamgi_engine_link_object_object (GAMGI_CAST_OBJECT atom, plane->object.object);
  return TRUE;
  }

return FALSE;
}

static void static_plane_remove (gamgi_plane *plane, gamgi_bool *link)
{
gamgi_dlist *dlist;

/***************************************************
 * this code is just to svoid a compilaton warning *
 ***************************************************/

link[0] = link[1] = link[2] = FALSE;

/*******************************************************
 * save reference atoms before removing plane contents *
 *******************************************************/

if (plane->reference == GAMGI_ENGINE_ATOM)
  {
  link[0] = static_atom_parent (plane, cache.atom1);
  link[1] = static_atom_parent (plane, cache.atom2);
  link[2] = static_atom_parent (plane, cache.atom3);
  }

/*************************
 * remove plane contents *
 *************************/

dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
plane->direction_start = NULL;
plane->direction_end = NULL;

dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
plane->atom_start = NULL;
plane->atom_end = NULL;

dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
plane->orbital_start = NULL;
plane->orbital_end = NULL;

dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
plane->text_start = NULL;
plane->text_end = NULL;
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

static void static_end_atom (GtkWidget *dialog)
{
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
int row;

/***************************************************
 * when the plane entry is empty, look for a plane *
 ***************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_PLANE;

/*******************************************
 * otherwise select which object to select *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_CELL) return GAMGI_ENGINE_CELL;
if (row == STATIC_REFERENCE_ATOMS) return GAMGI_ENGINE_ATOM;
if (row == STATIC_REFERENCE_LOCAL) return GAMGI_ENGINE_PLANE;

/****************************************************
 * this case can only occur when Reference is Local *
 ****************************************************/

return FALSE;
}

static void static_start_cell (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
int row;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_CELL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PLANE_VECTORS - 1);
  }
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_VECTORS_LOCAL);
  }

}

static void static_start_atom (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_instance (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *hbox_order, *hbox_node;
GtkWidget *entry;
int row;

/*********************************************************
 * initialize order,node combo box (before disabling it) *
 *********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 7);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_order");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***********************************
 * set order,node hbox sensitivity *
 ***********************************/

hbox_order = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_order");
hbox_node = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_node");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_ATOMS)
  {
  gtk_widget_set_sensitive (hbox_order, FALSE);
  gtk_widget_set_sensitive (hbox_node, FALSE);
  return;
  }

gtk_widget_set_sensitive (hbox_order, TRUE);

/******************************
 * set order,node sensitivity *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_instance");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == STATIC_INSTANCE_ORDER)
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (hbox_node, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (hbox_node, TRUE);
  }

}

static void static_start_scope (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
int row;

/********************************************************************
 * initialize with the same defaults used in gamgi_gtk_plane_create *
 ********************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STRUCTURE_COPY);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_FILTER_CLASS);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_thickness");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_PLANE_THICKNESS);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***************
 * start order *
 ***************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_instance");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_INSTANCE_ORDER);
static_instance (NULL, window);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_CELL)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_order");
  sprintf (token, "%d", GAMGI_PHYS_PLANE_RANGE_START);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_start_position (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/*******************************
 * reset translation, rotation *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_start_projection (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int row;

/*******************************************************
 * default for projection and model combo boxes: Local *
 *******************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_CELL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_net");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_NET_WULFF);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_MODEL_POLE);
  }
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_net");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_NET_LOCAL);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_MODEL_LOCAL);
  }
}

static void static_nodes (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int lattice, vectors;
int row;

/******************************
 * initialize node combo box  *
 * (before desabling it)      *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 7);

/********************
 * get cell lattice *
 ********************/

if (cell == NULL) lattice = 0;
else lattice = cell->lattice;

/********************
 * get cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == STATIC_VECTORS_PRIMITIVE) vectors = GAMGI_PHYS_PRIMITIVE;
else vectors = GAMGI_PHYS_CONVENTIONAL;

/****************
 * handle nodes *
 ****************/

gamgi_gtk_cell_symmetry_nodes (lattice, vectors, dialog);
}

static void static_vectors (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell != NULL) static_nodes (cell, window);
}

static void static_page_show (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_scope, *vbox_position, *vbox_projection;

vbox_scope = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
vbox_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
vbox_projection = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_projection");

gtk_widget_show (vbox_scope);
gtk_widget_show (vbox_position);
gtk_widget_show (vbox_projection);

static_start_scope (window);
static_start_position (window);
static_start_projection (window);
}

static void static_page_hide (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_position;
GtkWidget *combo;
int row;

vbox_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row != STATIC_REFERENCE_LOCAL) gtk_widget_hide (vbox_position);
}

static void static_page_handle (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_cell, *vbox_atom, *vbox_projection;
GtkWidget *combo;
int row;

static_page_show (window);

vbox_projection = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_projection");
vbox_cell = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cell");
vbox_atom = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_atom");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_ATOMS)
  {
  gtk_widget_hide (vbox_cell);
  gtk_widget_show (vbox_atom);
  static_start_atom (window);

  gtk_widget_hide (vbox_projection);
  }
else
  {
  gtk_widget_hide (vbox_atom);
  gtk_widget_show (vbox_cell);
  static_start_cell (window);
  }
}

static void static_cell (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_scope, *vbox_position, *vbox_projection;
GtkWidget *combo;

vbox_scope = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
vbox_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
vbox_projection = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_projection");

/****************************
 * start all possible pages *
 ****************************/

static_page_show (window);

/*************************************
 * hide pages not used for this cell *
 *************************************/

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell != NULL)
  {
  if (cell->model == GAMGI_PHYS_PROJECTION)
    {
    gtk_widget_hide (vbox_scope);
    gtk_widget_hide (vbox_position);
    }
  else if (cell->reciprocal != 0.0)
    {
    gtk_widget_hide (vbox_scope);
    gtk_widget_hide (vbox_position);
    gtk_widget_hide (vbox_projection);
    }
  else
    {
    gtk_widget_hide (vbox_projection);
    static_nodes (cell, window);
    }
  }

if (cell == NULL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_net");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_NET_LOCAL);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_MODEL_LOCAL);
  }
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

static_page_handle (window);
static_page_hide (window);
}

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

/********************
 * clean plane name *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/********************
 * reset plane data *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_LOCAL);
static_reference (NULL, window);

/***********************
 * reset Position data *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*******************
 * reset View data *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/************* internal function ************
 *                                          *
 *                STATIC_LIST               *
 *                                          *
 *     Makes the plane entry insensitive    *
 *     when the list button is pressed.     *
 *                                          *
 ********************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;
gamgi_bool *sensitive;

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_reference");

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_plane");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /**********************************
   * modify a single plane directly *
   **********************************/

  gtk_label_set_text (GTK_LABEL (label), "Plane");
  sensitive[STATIC_REFERENCE_CELL] = TRUE;
  sensitive[STATIC_REFERENCE_ATOMS] = TRUE;
  }
else
  {
  /**********************************************
   * modify multiple planes previously selected *
   **********************************************/

  gtk_label_set_text (GTK_LABEL (label), "Name");
  sensitive[STATIC_REFERENCE_CELL] = FALSE;
  sensitive[STATIC_REFERENCE_ATOMS] = FALSE;
  static_reset (window);
  }

}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *vbox;
GtkWidget *entry;
gamgi_bool visible;
const char *name;
int row;

/*****************************************************
 * Cell, Atom1,Atom2,Atom3 objects are required when *
 * Reference is explicitly set to Cell or Atoms      *
 *****************************************************/

cache.new[STATIC_REFERENCE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < STATIC_REFERENCE_LOCAL)
  {
  cache.new[STATIC_REFERENCE] = TRUE;
  if (row == STATIC_REFERENCE_CELL) cache.reference = GAMGI_ENGINE_CELL;
  if (row == STATIC_REFERENCE_ATOMS) cache.reference = GAMGI_ENGINE_ATOM;
  }

/*********************************************************************
 * atoms: coupled parameters, must be far enough to define a polygon *
 *********************************************************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_atom");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_ATOMS] = 0;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ATOMS] += 1;
  cache.atom1 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom1", GAMGI_ENGINE_ATOM, window);
  if (cache.atom1 == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom name", window);
    return FALSE;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ATOMS] += 1;
  cache.atom2 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom2", GAMGI_ENGINE_ATOM, window);
  if (cache.atom2 == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom name", window);
    return FALSE;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ATOMS] += 1;
  cache.atom3 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom3", GAMGI_ENGINE_ATOM, window);
  if (cache.atom3 == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom name", window);
    return FALSE;
    }
  }

if (cache.new[STATIC_ATOMS] == 3)
  {
  if (gamgi_phys_plane_polygon (cache.atom1, cache.atom2, cache.atom3) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid plane atoms", window);
    return FALSE;
    }
  }
else if (cache.new[STATIC_ATOMS] != 0)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane atoms", window);
  return FALSE;
  }

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_cell");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_CELL] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CELL] = TRUE;
  cache.cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
  "entry_cell", GAMGI_ENGINE_CELL, window);
  if (cache.cell == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell name", window);
    return FALSE;
    }
  }

cache.new[STATIC_H] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_H] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.hkl[0], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
    return FALSE;
    }
  }

cache.new[STATIC_K] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_K] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.hkl[1], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
    return FALSE;
    }
  }

cache.new[STATIC_L] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_L] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.hkl[2], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
    return FALSE;
    }
  }

cache.new[STATIC_VECTORS] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE && row < STATIC_VECTORS_LOCAL)
  {
  cache.new[STATIC_VECTORS] = TRUE;
  cache.vectors = row + 1;
  }

/****************************************************
 * scope: structure, filter, thickness, order, node *
 ****************************************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_STRUCTURE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE)
  {
  cache.new[STATIC_STRUCTURE] = TRUE;
  cache.structure = 1 + row;
  }

cache.new[STATIC_FILTER] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE)
  {
  cache.new[STATIC_FILTER] = TRUE;
  cache.filter = 1 + row;
  }

cache.new[STATIC_THICKNESS] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_thickness");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_THICKNESS] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.thickness, GAMGI_CHEM_PLANE_THICKNESS, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid plane thickness", window);
    return FALSE;
    }
  }

cache.new[STATIC_ORDER] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_order");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ORDER] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.order, -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid plane order", window);
    return FALSE;
    }
  }

/***********************************
 * o1,o2,o3,o4: coupled parameters *
 ***********************************/

cache.new[STATIC_NODES] = 0;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NODES] += 1;
  if (gamgi_io_token_int_scan (name,
  &cache.node[0], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell node", window);
    return FALSE;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NODES] += 1;
  if (gamgi_io_token_int_scan (name,
  &cache.node[1], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell node", window);
    return FALSE;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NODES] += 1;
  if (gamgi_io_token_int_scan (name,
  &cache.node[2], -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell node", window);
    return FALSE;
    }
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE && row < 7)
  {
  cache.new[STATIC_NODES] += 1;
  cache.node[3] = row;
  }

if (cache.new[STATIC_NODES] != 0 && cache.new[STATIC_NODES] != 4)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell node", window);
  return FALSE;
  }

/**************************
 * projection: net, model *
 **************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_projection");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_NET] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_net");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE && row < STATIC_NET_LOCAL)
  {
  cache.new[STATIC_NET] = TRUE;
  cache.net = NULL;
  if (row == STATIC_NET_WULFF) cache.net = gamgi_phys_projection_wulff;
  if (row == STATIC_NET_SCHMIDT) cache.net = gamgi_phys_projection_schmidt;
  }

cache.new[STATIC_MODEL] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE && row < STATIC_MODEL_LOCAL)
  {
  cache.new[STATIC_MODEL] = TRUE;
  cache.model = FALSE;
  if (row == STATIC_MODEL_POLE) cache.model = GAMGI_PHYS_POLE;
  if (row == STATIC_MODEL_TRACE) cache.model = GAMGI_PHYS_TRACE;
  }

/*********************************
 * position: x, y, z, e1, e2, e3 *
 *********************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_TRANSLATION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
    return FALSE;
    }
  }

cache.new[STATIC_TRANSLATION_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_Y] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
    return FALSE;
    }
  }

cache.new[STATIC_TRANSLATION_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_Z] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
    return FALSE;
    }
  }

cache.new[STATIC_ROTATION_E1] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E1] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[0], 0.0, 180.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
    return FALSE;
    }
  }

cache.new[STATIC_ROTATION_E2] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E2] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[1], 0.0, 360.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
    return FALSE;
    }
  }

cache.new[STATIC_ROTATION_E3] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E3] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[2], 0.0, 360.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
    return FALSE;
    }
  }

/***************************
 * red, green, blue, scale *
 ***************************/

cache.new[STATIC_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_GREEN] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_GREEN] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.green, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_BLUE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BLUE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.blue, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_SCALE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SCALE] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.scale, GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid scale", window);
    return FALSE;
    }
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_plane *plane, gamgi_window *window)
{
gamgi_cell *cell;
gamgi_object *parent;
gamgi_dlist *dlist;
char name[GAMGI_ENGINE_TOKEN];
gamgi_net net;
gamgi_enum reference, vectors, model;
gamgi_bool link[3];
gamgi_bool new;
double zero[3], euler[3], origin[3];
double scale;
float red, green, blue;
int hkl[3];
int order, o1, o2, o3;
int pole, trace;

/**************************
 * import data from plane *
 **************************/

strcpy (name, plane->object.name);

reference = plane->reference;
if (plane->reference == GAMGI_ENGINE_CELL && 
(plane->object.object)->class == GAMGI_ENGINE_CELL)
  cell = GAMGI_CAST_CELL plane->object.object;
else cell = NULL;

hkl[0] = plane->hkl[0];
hkl[1] = plane->hkl[1];
hkl[2] = plane->hkl[2];
vectors = plane->vectors;

net = plane->net;
model = plane->model;
order = plane->order;

gamgi_math_position_plane_rotation_get (plane, euler);
gamgi_math_position_plane_translation_get (plane, origin);

red = plane->red;
green = plane->green;
blue = plane->blue;
scale = plane->object.scale;

/*********************************************
 * apply new data: atoms,nodes handled later *
 *********************************************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_REFERENCE] == TRUE) reference = cache.reference;
if (cache.new[STATIC_CELL] == TRUE) cell = cache.cell;
if (cache.new[STATIC_H] == TRUE) hkl[0] = cache.hkl[0];
if (cache.new[STATIC_K] == TRUE) hkl[1] = cache.hkl[1];
if (cache.new[STATIC_L] == TRUE) hkl[2] = cache.hkl[2];
if (cache.new[STATIC_VECTORS] == TRUE) vectors = cache.vectors;

if (cache.new[STATIC_NET] == TRUE) net = cache.net;
if (cache.new[STATIC_MODEL] == TRUE) model = cache.model;
if (cache.new[STATIC_ORDER] == TRUE) order = cache.order;

if (cache.new[STATIC_TRANSLATION_X] == TRUE) origin[0] = cache.translation[0];
if (cache.new[STATIC_TRANSLATION_Y] == TRUE) origin[1] = cache.translation[1];
if (cache.new[STATIC_TRANSLATION_Z] == TRUE) origin[2] = cache.translation[2];

if (cache.new[STATIC_ROTATION_E1] == TRUE) euler[0] = cache.rotation[0];
if (cache.new[STATIC_ROTATION_E2] == TRUE) euler[1] = cache.rotation[1];
if (cache.new[STATIC_ROTATION_E3] == TRUE) euler[2] = cache.rotation[2];

if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/***************************************************
 * set default data, when changing plane reference *
 ***************************************************/

if (cache.new[STATIC_REFERENCE] == TRUE)
  {
  if (reference == GAMGI_ENGINE_ATOM)
    {
    cell = NULL;
    hkl[0] = 0; hkl[1] = 0; hkl[2] = 0;
    vectors = GAMGI_PHYS_PLANE_VECTORS;
    order = 1;
    model = GAMGI_PHYS_POLYGON;
    net = NULL;
    }

  if (reference == GAMGI_ENGINE_CELL)
    {
    if (cell->model == GAMGI_PHYS_PROJECTION) order = 1;
    else { model = GAMGI_PHYS_POLYGON; net = NULL; }
    }
  }

/****************************
 * check data compatibility *
 ****************************/

if (reference == GAMGI_ENGINE_CELL)
  {
  if (hkl[0] == 0 && hkl[1] == 0 && hkl[2] == 0)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane indices", window);
    return FALSE;
    }

  if ((model == GAMGI_PHYS_POLE || model == GAMGI_PHYS_TRACE) &&
  (net == NULL || cell == NULL || cell->model != GAMGI_PHYS_PROJECTION))
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (model != GAMGI_PHYS_POLE && model != GAMGI_PHYS_TRACE &&
  net != NULL)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (model == GAMGI_PHYS_VECTOR &&
  (cache.new[STATIC_TRANSLATION_X] == TRUE ||
  cache.new[STATIC_TRANSLATION_Y] == TRUE ||
  cache.new[STATIC_TRANSLATION_Z] == TRUE ||
  cache.new[STATIC_ROTATION_E1] == TRUE ||
  cache.new[STATIC_ROTATION_E2] == TRUE ||
  cache.new[STATIC_ROTATION_E3] == TRUE))
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid direction parameters", window);
    return FALSE;
    }

  }

if (reference == GAMGI_ENGINE_ATOM)
  {
  if (cell != NULL)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parent", window);
    return FALSE;
    }

  if (hkl[0] != 0 || hkl[1] != 0 || hkl[2] != 0)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (vectors != GAMGI_PHYS_PLANE_VECTORS)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (net != NULL)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (model == GAMGI_PHYS_POLE || model == GAMGI_PHYS_TRACE ||
  model == GAMGI_PHYS_VECTOR)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (cache.new[STATIC_H] == TRUE || cache.new[STATIC_K] == TRUE ||
  cache.new[STATIC_L] == TRUE || cache.new[STATIC_VECTORS] == TRUE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }

  if (cache.new[STATIC_ORDER] == TRUE || cache.new[STATIC_NODES] > 0)
      {
      if (cache.local == TRUE)
        gamgi_gtk_dialog_message_create ("Error",
        "Invalid plane parameters", window);
      return FALSE;
      }

  if (cache.new[STATIC_NET] == TRUE || cache.new[STATIC_MODEL] == TRUE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane parameters", window);
    return FALSE;
    }
  }

/***************
 * handle node *
 ***************/

if (cache.new[STATIC_NODES] > 0)
  {
  if (cell == NULL || gamgi_math_node_check (cell->lattice, cache.node[3]) == FALSE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid plane node", window);
    return FALSE;
    }
 
  if (vectors == GAMGI_PHYS_CONVENTIONAL)
    gamgi_math_node_primitive (cell->lattice, cache.node[0],
    cache.node[1], cache.node[2], cache.node[3], &o1, &o2, &o3);
  else
    { o1 = cache.node[0]; o2 = cache.node[1]; o3 = cache.node[2]; }

  order = gamgi_phys_plane_order (cell, hkl, vectors, o1, o2, o3);
  }

if (order != 1 &&
(reference == GAMGI_ENGINE_ATOM || model == GAMGI_PHYS_POLE ||
model == GAMGI_PHYS_TRACE || model == GAMGI_PHYS_VECTOR))
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid plane order", window);
  return FALSE;
  }

/********************************************
 * check if array data must be recalculated *
 ********************************************/

new = FALSE;
if (cache.new[STATIC_REFERENCE] == TRUE) new = TRUE;
if (cache.new[STATIC_ATOMS] > 0) new = TRUE;
if (cache.new[STATIC_CELL] == TRUE) new = TRUE;

if (plane->hkl[0] != hkl[0]) new = TRUE;
if (plane->hkl[1] != hkl[1]) new = TRUE;
if (plane->hkl[2] != hkl[2]) new = TRUE;
if (plane->vectors != vectors) new = TRUE;

if (plane->order != order) new = TRUE;
if (plane->net != net) new = TRUE;
if (plane->model != model) new = TRUE;

/*************************************
 * reciprocal lattica cannot be new  *
 * and polygon planes need thickness *
 *************************************/

if (new == TRUE && plane->model == GAMGI_PHYS_VECTOR)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid plane parameters", window);
  return FALSE;
  }

if (new == TRUE && model == GAMGI_PHYS_POLYGON &&
cache.new[STATIC_THICKNESS] == FALSE)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid plane parameters", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*******************
 * build new plane *
 *******************/

strcpy (plane->object.name, name);
plane->reference = reference;
plane->hkl[0] = hkl[0];
plane->hkl[1] = hkl[1];
plane->hkl[2] = hkl[2];
plane->vectors = vectors;

plane->order = order;
plane->net = net;
plane->model = model;

plane->red = red;
plane->green = green;
plane->blue = blue;
plane->object.scale = scale;

if (new == TRUE)
  {
  /**************************************************************
   * rebuild arrays: unlink plane AFTER removing contents, as   *
   * reference atoms might need to be saved in the parent above *
   **************************************************************/

  static_plane_remove (plane, link);
  gamgi_engine_unlink_plane (plane);
  free (plane->points);
  plane->points = NULL;
  free (plane->loops);
  plane->loops = NULL;
  plane->n_points = 0;
  plane->n_loops = 0;

  if (plane->reference == GAMGI_ENGINE_CELL)
    {
    /**************************************************************
     * Plane Create: plane is just a template, removed in the     *
     * end, used to copy data to plane_new, the real plane        *
     *                                                            *
     * Plane Modify: plane is the real plane, the plane to modify *
     *                                                            *
     * plane->object.object is used here as a flag to distinguish *
     * between the two cases. plane->object.object can be used    *
     * because plane is always unlinked at this stage             *
     **************************************************************/

    plane->object.object = GAMGI_CAST_OBJECT cell;

    /************************************
     * this is a crystallographic plane *
     ************************************/

    if (cell->model == GAMGI_PHYS_PROJECTION)
      {
      if (plane->model == GAMGI_PHYS_POLE) pole = TRUE; else pole = FALSE;
      if (plane->model == GAMGI_PHYS_TRACE) trace = TRUE; else trace = FALSE;
      gamgi_phys_plane_projection (plane, cell, pole, trace);
      }
    else
      {
      dlist = gamgi_engine_dlist_atom_object (GAMGI_CAST_OBJECT cell, NULL);
      dlist = gamgi_engine_filter_atom (dlist, 0, cache.filter,
      GAMGI_ENGINE_PLANE, GAMGI_CAST_OBJECT cell);

      gamgi_phys_plane_range (plane, cell, cache.structure,
      cache.filter, cache.thickness, dlist, order, order);

      while (dlist != NULL)
        dlist = gamgi_engine_dlist_remove_start (dlist);
      }
    }

  if (plane->reference == GAMGI_ENGINE_ATOM)
    {
    /**********************************************************
     * this is an atomic plane: 1) get list of parent atoms   *
     * 2) reorder list so the three defining atoms come first *
     *                                                        *
     * the plane model is needed because of the rotation next *
     **********************************************************/

    plane->model = GAMGI_PHYS_POLYGON;
    plane->vectors = GAMGI_PHYS_PLANE_VECTORS;

    parent = gamgi_phys_plane_parent (cache.atom1, cache.atom2, cache.atom3, window);
    dlist = gamgi_engine_dlist_atom_object (parent, NULL);
    dlist = gamgi_phys_plane_reorder (dlist, cache.atom1, cache.atom2, cache.atom3);
    dlist = gamgi_engine_filter_atom (dlist, 3, cache.filter, GAMGI_ENGINE_PLANE, parent);
    if (parent->class <= GAMGI_ENGINE_PLANE) parent = GAMGI_CAST_OBJECT window->layer;

    /*********************************************************
     * reset and build plane polygon AFTER linking to parent *
     *********************************************************/

    gamgi_engine_link_object_object (GAMGI_CAST_OBJECT plane, parent);

    gamgi_math_vector_zero (zero);
    gamgi_math_position_plane_rotation_set (plane, zero);
    gamgi_math_position_plane_translation_set (plane, zero);

    gamgi_phys_plane_atoms (plane, cache.atom1, cache.atom2,
    cache.atom3, dlist, cache.structure, cache.thickness);
    static_atom_remove (plane, link);
    }
  }
else
  {
  gamgi_math_vector_zero (zero);
  gamgi_math_position_plane_rotation_set (plane, zero);
  gamgi_math_position_plane_translation_set (plane, zero);

  gamgi_math_position_plane_rotation_set (plane, euler);
  gamgi_math_position_plane_translation_set (plane, origin);
  }

return TRUE;
}

static void static_ok_global (gamgi_window *window)
{
gamgi_dlist *dlist;
gamgi_slist *start;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

cache.local = FALSE;

if (gamgi->planes == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of plane objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error","Invalid plane identity", window);
    return;
    }
  }

if (static_grab (window) == FALSE) return;

/************************************************
 * For all windows: save layers as undo layers  *
 * and set current layer as new current object  *
 *                                              *
 * Modify listed objects and show error message *
 * with objects that could not be modified      *
 *                                              *
 * For all windows: redraw gl_area image        *
 ************************************************/

gamgi_global_selection_undo ();
gamgi_global_selection_focus ();

start = NULL;
for (dlist = gamgi->planes; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_PLANE dlist->data, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window);

gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window);
}

static void static_ok_local (gamgi_window *window)
{
gamgi_plane *plane;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/******************************************
 * pick plane and get name (possibly new) *
 *                                        *
 * check if focus object belongs to plane *
 ******************************************/

cache.new[STATIC_NAME] = TRUE;
plane = GAMGI_CAST_PLANE gamgi_gtk_object_number (dialog,
"entry_plane", GAMGI_ENGINE_PLANE, window, cache.name);
if (plane == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT plane) == TRUE) focus = TRUE;

/*********************************************
 * collect data from dialog and modify plane *
 *********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (plane, window) == FALSE) return;

/*************************************************************
 * when focus object is affected, change it to current layer *
 *************************************************************/

if (focus == TRUE)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);

/*****************************************
 * redraw gl_area image and reset dialog *
 *****************************************/

gtk_widget_queue_draw (window->area);
static_reset (window);
}

/****************** internal function *****************
 *                                                    *
 *                     STATIC_OK                      *
 *                                                    *
 * This is the function triggered when Ok is pressed. *
 *                                                    *
 ******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

/*********************************************
 * modify data: execute global or local task *
 *********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_global (window);
else
  static_ok_local (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
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

static_reset (window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
gamgi_plane *plane;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];
double origin[3], euler[3];
gamgi_enum class;

class = static_class (window);
if (class == GAMGI_ENGINE_PLANE)
  {
  plane = GAMGI_CAST_PLANE object;

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
  if (plane->reference == GAMGI_ENGINE_CELL)
    {
    /************************************
     * This is a crystallographic plane *
     ************************************/

    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_CELL);
    static_page_handle (window);

    /***************************
     * set cell identification *
     ***************************/

    cell = GAMGI_CAST_CELL plane->object.object;
    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
    sprintf (string, "%s %d", cell->object.name, cell->object.number);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    /***********************
     * Plane h,k,l indices *
     ***********************/

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h");
    sprintf (string, "%d", plane->hkl[0]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k");
    sprintf (string, "%d", plane->hkl[1]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l");
    sprintf (string, "%d", plane->hkl[2]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    /*****************
     * Plane vectors *
     *****************/

    combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), plane->vectors - 1);

    if (cell->model == GAMGI_PHYS_PROJECTION)
      {
      combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_net");
      if (plane->net == gamgi_phys_projection_wulff)
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_NET_WULFF);
      if (plane->net == gamgi_phys_projection_schmidt)
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_NET_SCHMIDT);

      combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
      if (plane->model == GAMGI_PHYS_POLE)
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_MODEL_POLE);
      if (plane->model == GAMGI_PHYS_TRACE)
        gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_MODEL_TRACE);
      }
    else
      {
      /*******************
       * set Plane order *
       *******************/

      combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_instance");
      gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_INSTANCE_ORDER);
      static_instance (NULL, window);

      entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_order");
      sprintf (string, "%d", plane->order);
      gtk_entry_set_text (GTK_ENTRY (entry), string);
      }
    }
  else if (plane->reference == GAMGI_ENGINE_ATOM)
    {
    /***************************
     * This is an atomic plane *
     ***************************/

    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_ATOMS);
    static_page_handle (window);
    }

  /***************************************
   * restore Reference combo, Cell entry *
   ***************************************/

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
  g_signal_handlers_block_by_func (combo, static_reference, window);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_LOCAL);
  g_signal_handlers_unblock_by_func (combo, static_reference, window);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
  g_signal_handlers_block_by_func (entry, static_cell, window);
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  g_signal_handlers_unblock_by_func (entry, static_cell, window);

  /***********************************
   * set Plane orientation, position *
   ***********************************/

  gamgi_math_position_plane_translation_get (plane, origin);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  gamgi_math_position_plane_rotation_get (plane, euler);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  /**************************
   * set Plane color, scale *
   **************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, plane->red);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, plane->green);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, plane->blue);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, plane->object.scale);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  /**********************************************************************
   * set plane identification at the end, after automatic name setting! *
   **********************************************************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_plane");
  sprintf (string, "%s %d", object->name, object->number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

else if (class == GAMGI_ENGINE_CELL)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
  sprintf (string, "%s %d", object->name, object->number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  static_page_hide (window);
  }
else if (class == GAMGI_ENGINE_ATOM)
  {
  /***********************
   * get next atom entry *
   ***********************/

  entry = static_atom (dialog);
  if (entry == NULL)
    { static_end_atom (dialog); entry = static_atom (dialog); }
  sprintf (string, "%s %d", object->name, object->number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  static_page_hide (window);
  }

}

/*************** external function *************
 *                                             *
 *         GAMGI_GTK_PLANE_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_plane_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;

/******************************
 * local mouse selection only *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y,
static_class (window_dialog), TRUE, static_press);
}

/*************** external function ******************
 *                                                  *
 *            GAMGI_GTK_PLANE_MODIFY                *
 *                                                  *
 * Creates the dialog window used to modify planes. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_plane_modify (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
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

dialog = gamgi_gtk_dialog_task0_create ("Plane Modify", window);
window->action = GAMGI_GTK_PLANE_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Cell objects *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********
 * Local *
 *********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "hbox_plane", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Plane");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_plane", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_plane", entry);
gtk_widget_show (entry);

/********
 * List *
 ********/

button = gtk_toggle_button_new_with_label (" List ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_list), window);
g_object_set_data (G_OBJECT (dialog), "button_list", button);
gtk_widget_show (button);

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
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_reference), window);
g_object_set_data (G_OBJECT (dialog), "combo_reference", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Cell", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atoms", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (3);
g_object_set_data (G_OBJECT (dialog), "sensitive_reference", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

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
gtk_widget_show (label);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_indices), window);
g_object_set_data (G_OBJECT (dialog), "entry_h", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_indices), window);
g_object_set_data (G_OBJECT (dialog), "entry_k", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_signal_connect (entry, "changed",
G_CALLBACK (static_indices), window);
g_object_set_data (G_OBJECT (dialog), "entry_l", entry);
gtk_widget_show (entry);

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
gtk_list_store_append (store, &iter); 
gtk_list_store_set (store, &iter, 0, "Local", -1);
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

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * Projection *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 40);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/****************************
 * wulff, schmidt combo box *
 ****************************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Net");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_net", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Wulff", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Schmidt", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*************************
 * pole, trace combo box *
 *************************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_model", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Pole", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Trace", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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

/*********
 * order *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "hbox_order", hbox_left);
gtk_widget_show (hbox_left);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_instance), window);
g_object_set_data (G_OBJECT (dialog), "combo_instance", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Order", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Node", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_order", entry);
gtk_widget_show (entry);

/********
 * node *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "hbox_node", hbox_center);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (8);
g_object_set_data (G_OBJECT (dialog), "sensitive_o4", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

label = gtk_label_new ("O4");
gtk_table_attach (GTK_TABLE (table), label, 8, 9, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_o4", label);
gtk_widget_show (label);

/*****************
 * Position page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Position");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_position", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/**********
 * Origin *
 **********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_z", entry);
gtk_widget_show (entry);

/*********
 * Angle *
 *********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Rotation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("E1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e3", entry);
gtk_widget_show (entry);

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
