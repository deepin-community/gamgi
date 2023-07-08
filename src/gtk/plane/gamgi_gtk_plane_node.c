/***************************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_node.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_gtk_plane_create.h"
#include "gamgi_gtk_plane_node.h"
#include "gamgi_math_node.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_io_token.h"

static struct {

/******************************
 * indicates if the structure *
 * is currently being used    *
 ******************************/

gamgi_bool active;

int o1, o2, o3;
gamgi_enum o4; } node;

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
gamgi_gtk_plane_node_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
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

gamgi_gtk_cell_symmetry_nodes (lattice, vectors, window->dialog1);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *combo;
int hkl[3], hkl_min[3], hkl_miller[3];
gamgi_enum vectors;
const char *name;

/***********************************
 * get cell object (to check if    *
 * the node coordinates are valid) *
 ***********************************/

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (window->dialog0,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell object", window);
  return;
  }

/***********************************
 * get cell vectors (to check if   *
 * the node coordinates are valid) *
 ***********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "combo_vectors");
vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/**************************************
 * get plane indices hkl (to check    *
 * if the node coordinates are valid) *
 **************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &hkl[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &hkl[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &hkl[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane indices", window);
  return;
  }

/*********************************
 * get o1,o2,o3 node coordinates *
 *********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &node.o1, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &node.o2, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &node.o3, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

/**********************************************************
 * get o4 node coordinate (unless the lattice is centered *
 * and the vectors are conventional, o4 is always 000)    *
 **********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
node.o4 = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (gamgi_math_node_check (cell->lattice, node.o4) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

/********************************************************
 * Reject Laue planes, as (400) in cI lattices.         *
 *                                                      *
 * Reject multiple planes, as (100) in cI lattices,     *
 * for centered nodes, as (1/2 1/2 1/2) in cI lattices. *
 ********************************************************/

gamgi_phys_direction_decrease (hkl, hkl_min);
gamgi_phys_plane_increase (cell->lattice, hkl_min, vectors, hkl_miller);

if (gamgi_phys_direction_higher (hkl, hkl_miller) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

if (gamgi_phys_direction_higher (hkl_miller, hkl) == TRUE && node.o4 != FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid node coordinates", window);
  return;
  }

/******************************************
 * 1) signal that data has been entered   *
 * 2) disable order in first level dialog *
   3) close second level dialog           *
 ******************************************/

node.active = TRUE;
gamgi_gtk_plane_create_range (FALSE, window);
gamgi_gtk_dialog_task1_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
gamgi_cell *cell;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (window->dialog0,
"entry_cell", GAMGI_ENGINE_CELL, window);
static_nodes (cell, window);

if (node.active == FALSE)
  {
  /****************************
   * show default coordinates *
   ****************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
  sprintf (token, "%d", GAMGI_PHYS_NODE_O1);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
  sprintf (token, "%d", GAMGI_PHYS_NODE_O2);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
  sprintf (token, "%d", GAMGI_PHYS_NODE_O3);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);
  }
else
  {
  /*************************
   * show node coordinates *
   *************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
  sprintf (token, "%d", node.o1);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
  sprintf (token, "%d", node.o2);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
  sprintf (token, "%d", node.o3);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), node.o4);
  }
}

static void static_create (gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *vbox_dialog, *vbox_frame;
GtkWidget *hbox_center;
GtkWidget *frame;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

dialog = gamgi_gtk_dialog_task1_create ("Plane Node", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/********
 * Node *
 ********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Node");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/********************
 * Cell coordinates *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 8, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("O1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("O2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("O3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
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
gtk_table_attach (GTK_TABLE (table), label, 9, 10, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

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
G_CALLBACK (gamgi_gtk_plane_node_cancel), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}

void gamgi_gtk_plane_node_start (void *data)
{
node.active = FALSE;
}

void gamgi_gtk_plane_node_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

gamgi_gtk_dialog_task1_remove (widget, window);

/***********************************
 * if no data has been submited,   *
 * the default is the range button *
 ***********************************/

if (node.active == FALSE)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_range");
  gtk_button_clicked (GTK_BUTTON (button));
  }
}

gamgi_bool gamgi_gtk_plane_node_get (gamgi_cell *cell, 
int *o1, int *o2, int *o3, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_enum vectors;

/**************************************************
 * return FALSE if node information is not active *
 **************************************************/

if (node.active == FALSE) return FALSE;

/********************
 * export node data *
 ********************/

/*****************************************************************
 * the type of axes vectors (primitive or conventional) used to  *
 * define the plane indices are the same used to define the node *
 *****************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_vectors");
vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/*************************************
 * get node in primitive coordinates *
 *************************************/

if (vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice, 
  node.o1, node.o2, node.o3, node.o4, o1, o2, o3);
else
  { *o1 = node.o1; *o2 = node.o2; *o3 = node.o3; }

return TRUE;
}

void gamgi_gtk_plane_node (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_node");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /******************************
   * create second level dialog *
   ******************************/

  static_create (window);
  }
else
  {
  /***********************************
   *   remove second level dialog    *
   *                                 *
   * restart Order data, if disabled *
   ***********************************/

  if (window->dialog1 != NULL)
    gamgi_gtk_dialog_task1_remove (widget, window);

  if (node.active == TRUE)
    { 
    node.active = FALSE;
    gamgi_gtk_plane_create_range (TRUE, window);
    }
  }
}
