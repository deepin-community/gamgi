/*************************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_property.c
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

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_chem_atom.h"
#include "gamgi_io_token.h"

static void static_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_mass");
if (button != NULL)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_radius");
if (button != NULL)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_charge");
if (button != NULL)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
static_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_mass_select (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_radius_select (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_charge_select (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_mass_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;
const char *name;
int element;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &element, 0, 
GAMGI_CHEM_ATOM_MAX) == FALSE) element = FALSE;

dialog = gamgi_gtk_dialog_task1_create ("Atomic Mass", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (vbox_dialog), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_mass_select), window);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Mass", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Isotope", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
g_object_set (G_OBJECT (renderer), "align-set", TRUE, "xalign", 0.5, NULL);
gtk_tree_view_column_set_alignment (column, 0.5);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Abundance", renderer, "text", 2, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

column = gtk_tree_view_column_new_with_attributes ("Half-Life", renderer, "text", 3, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (4, 
G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
gamgi_chem_atom_mass_list (store, element);
gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_radius_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;
const char *name;
int element;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &element, 0,
GAMGI_CHEM_ATOM_MAX) == FALSE) element = FALSE;

dialog = gamgi_gtk_dialog_task1_create ("Element Radius", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (vbox_dialog), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_radius_select), window);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Radius", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

column = gtk_tree_view_column_new_with_attributes ("Type", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Valence", renderer, "text", 2, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
g_object_set (G_OBJECT (renderer), "align-set", TRUE, "xalign", 0.5, NULL);
gtk_tree_view_column_set_alignment (column, 0.5);

column = gtk_tree_view_column_new_with_attributes ("Coordination", renderer, "text", 3, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_column_set_alignment (column, 0.5);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (4,
G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
gamgi_chem_atom_radius_list (store, element);
gamgi_chem_atom_ionic_list (store, element);
gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_charge_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *button;
GtkWidget *scrolled_window;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;

dialog = gamgi_gtk_dialog_task1_create ("Atomic Charge", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), scrolled_window, FALSE, FALSE, 0);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_charge_select), window);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Charge", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Configuration", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

void gamgi_gtk_atom_property_mass (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/********************
 * Create Mass List *
 ********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*******************************************
   * remove previous (Radius or Charge) List *
   *******************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_radius");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_charge");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_mass_create (widget, window);
  }

/********************
 * Remove Mass List *
 ********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

void gamgi_gtk_atom_property_radius (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/**********************
 * Create Radius List *
 **********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*****************************************
   * remove previous (Mass or Charge) List *
   *****************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_mass");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_charge");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_radius_create (widget, window);
  }

/**********************
 * Remove Radius List *
 **********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

void gamgi_gtk_atom_property_charge (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/**********************
 * Create Charge List *
 **********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*****************************************
   * remove previous (Mass or Radius) List *
   *****************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_mass");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_radius");
  if (button != NULL)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_charge_create (widget, window);
  }

/**********************
 * Remove Charge List *
 **********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}
