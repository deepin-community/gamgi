/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_config.c
 *
 * Copyright (C) 2017 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_chem.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_mesa_lists.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

/************ internal function **********
 *                                       *
 *               STATIC_OK               *
 *                                       *
 *                                       *
 *****************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_cell_class *cell_class, *cell_class_old;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;

cell_class = gamgi_global_copy_cell (gamgi->cell);

/*************************
 * Volume: default model *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
cell_class->model = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/************************
 * Origin: default axes *
 ************************/

cell_class->axes = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_yes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell_class->axes = TRUE;

/**************************************************************
 * View: default borders, faces, nodes, red,green,blue colors *
 **************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
cell_class->borders = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

cell_class->faces = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_faces");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell_class->faces = TRUE;

cell_class->nodes = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_nodes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell_class->nodes = TRUE;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell_class->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell_class->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell_class->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

/*************************************************
 * Global: axes size, borders width, nodes width *
 *************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_size");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell_class->axes_size,
GAMGI_MESA_AXES_MIN, GAMGI_MESA_AXES_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid axes size", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_borders");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cell_class->borders_width,
GAMGI_MESA_CELL_LINE_MIN, GAMGI_MESA_CELL_LINE_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid borders width", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_nodes");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cell_class->nodes_width,
GAMGI_MESA_CELL_POINT_MIN, GAMGI_MESA_CELL_POINT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid nodes width", window);
  gamgi_global_remove_cell (cell_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

cell_class_old = gamgi->cell;
gamgi->cell = cell_class;
gamgi_global_remove_cell (cell_class_old);

/******************************************************
 *    Remove dialog and redraw ALL top windows        *
 * (because of global parameters: size, border, nodes *
 ******************************************************/

gamgi_gtk_dialog_task0_remove (widget, window);
gamgi_mesa_draw_gamgi ();
}

static void static_init (gamgi_window *window)
{
gamgi_cell_class *cell_class = gamgi->cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

/*********************************
 * initialise Volume page: model *
 *********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell_class->model - 1);

/********************************
 * initialise Origin page: axes *
 ********************************/

if (cell_class->axes == TRUE)
  { button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_yes");
    gtk_button_clicked (GTK_BUTTON (button)); }
else
  { button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_no");
    gtk_button_clicked (GTK_BUTTON (button)); }

/******************************************************
 * initialise View page: borders, faces, nodes, color *
 ******************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell_class->borders - 1);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_faces");
if (cell_class->faces == TRUE)
  gtk_button_clicked (GTK_BUTTON (button));

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_nodes");
if (cell_class->nodes == TRUE)
  gtk_button_clicked (GTK_BUTTON (button));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell_class->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell_class->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell_class->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************************************************************
 * initialize Global page: axes size, borders width, node width *
 ****************************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_size");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, cell_class->axes_size);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_borders");
sprintf (token, "%d", cell_class->borders_width);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_nodes");
sprintf (token, "%d", cell_class->nodes_width);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

void gamgi_gtk_cell_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Cell Config", window);
window->action = GAMGI_GTK_CELL_CONFIG;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/**********************
 * Volume page: Model *
 **********************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Volume");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*******************
 * Model combo box *
 *******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_model", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Wigner-Seitz", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Parallelepiped", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sphere", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Projection", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));

g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/***************
 * Origin page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Origin");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * axes *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Axes");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

button = gtk_radio_button_new_with_label (NULL, "Yes");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_yes", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "No");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_no", button);
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

vbox_top = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, TRUE, TRUE, 0);
gtk_widget_show (vbox_top);

/*************************
 * Borders, Faces, Nodes *
 *************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Borders");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_borders", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Faces", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Edges", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

button = gtk_toggle_button_new_with_label ("Faces");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_faces", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("Nodes");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_nodes", button);
gtk_widget_show (button);

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
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
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

/***************
 * Global page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Global");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*************************
 * Width: Borders, Nodes *
 *************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Axes");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Size");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_size", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 15, 0);
gtk_widget_show (label);

label = gtk_label_new ("Borders");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Nodes");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_borders", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_nodes", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Width");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
