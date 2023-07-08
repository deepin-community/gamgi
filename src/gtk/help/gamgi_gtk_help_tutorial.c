/*************************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_tutorial.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_help_dialog.h"

static void static_tutorial_cscl (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: CsCl cristalline structure",
"tutorials/cscl", data, 1, 1, "page1");
}

static void static_tutorial_nacl (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: NaCl cristalline structure",
"tutorials/nacl", data, 1, 1, "page1");
}

static void static_tutorial_calcite (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Calcite cristalline structure",
"tutorials/calcite", data, 1, 1, "page1");
}

static void static_tutorial_polyethylene (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Orthorhombic Polyethylene",
"tutorials/polyethylene", data, 1, 1, "page1");
}

static void static_tutorial_interstices_bcc (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Interstices in the BCC structure",
"tutorials/interstices_bcc", data, 2, 1, "page1", "page2");
}

static void static_tutorial_interstices_cfc (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Interstices in the CFC structure",
"tutorials/interstices_cfc", data, 2, 1, "page1", "page2");
}

static void static_tutorial_interstices_hcp (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Interstices in the HCP structure",
"tutorials/interstices_hcp", data, 2, 1, "page1", "page2");
}

static void static_tutorial_symmetry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Point Symmetry analysis",
"tutorials/symmetry", data, 2, 1, "page1", "page2");
}

static void static_tutorial_voronoi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Voronoi tesselation",
"tutorials/voronoi", data, 2, 1, "page1", "page2");
}

static void static_tutorial_occupancy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Cell occupancy rules",
"tutorials/occupancy", data, 2, 1, "page1", "page2");
}

static void static_tutorial_nanostructure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Multilayer nanostructure",
"tutorials/nanostructure", data, 1, 1, "page1");
}

static void static_tutorial_rcp (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Random Close Packing structure",
"tutorials/rcp", data, 1, 1, "page1");
}

static void static_tutorial_lights (void *data)
{
gamgi_gtk_help_dialog_import ("Help Tutorial: Directional and positional lights",
"tutorials/lights", data, 2, 1, "page1", "page2");
}

static void static_activated (GtkTreeView *treeview,
GtkTreePath *path, GtkTreeViewColumn *col, void *data)
{
GtkTreeIter iter;
GtkTreeModel *model;
gamgi_function1 function;

/*********************************
 * a row has been double-clicked *
 *********************************/

model = gtk_tree_view_get_model (treeview);
if (gtk_tree_model_get_iter (model, &iter, path) == FALSE) return;

gtk_tree_model_get (model, &iter, 2, &function, -1);
(* function) (data);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
gamgi_function1 function;

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Help tutorial not selected", window);
  return;
  }

gtk_tree_model_get (model, &iter, 2, &function, -1);
(* function) (data);
}

static void static_row (GtkListStore *store,
char *label0, char *label1, gamgi_function1 function)
{
GtkTreeIter iter;

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, label0, 1, label1, 2, function, -1);
}

static void static_contents (GtkWidget *treeview)
{
GtkListStore *store;

store = gtk_list_store_new (3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

static_row (store, "CsCl crystalline structure", "1", static_tutorial_cscl);
static_row (store, "NaCl crystalline structure", "1", static_tutorial_nacl);
static_row (store, "Calcite CaCO3 crystalline structure", "1", static_tutorial_calcite);
static_row (store, "Orthorhombic Polyethylene", "1", static_tutorial_polyethylene);
static_row (store, "Interstices in the BCC structure", "2", static_tutorial_interstices_bcc);
static_row (store, "Interstices in the CFC structure", "2", static_tutorial_interstices_cfc);
static_row (store, "Interstices in the HCP structure", "2", static_tutorial_interstices_hcp);
static_row (store, "Point Symmetry analysis", "2", static_tutorial_symmetry);
static_row (store, "Voronoi tesselation", "2", static_tutorial_voronoi);
static_row (store, "Cell occupancy rules", "2", static_tutorial_occupancy);
static_row (store, "Multilayer nanotructure", "1", static_tutorial_nanostructure);
static_row (store, "Random Close Packing structure", "1", static_tutorial_rcp);
static_row (store, "Directional and positional lights", "2", static_tutorial_lights);

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);
}

/***************** external function **********************
 *                                                        *
 *               GAMGI_GTK_HELP_TUTORIAL                  *
 *                                                        *
 * Provides a list of tutorials showing how to use Gamgi. *
 *                                                        *
 **********************************************************/

void gamgi_gtk_help_tutorial (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *scrolled_window;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkWidget *button;

dialog = gamgi_gtk_dialog_task0_create ("Help Tutorial", window);
window->action = GAMGI_GTK_HELP_TUTORIAL;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

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
gtk_widget_set_size_request (treeview, -1, GAMGI_GTK_HELP_TUTORIAL_HEIGHT);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "row-activated",
G_CALLBACK (static_activated), window);
g_object_set_data (G_OBJECT (dialog), "treeview", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Subject", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Pages   ", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
g_object_set (G_OBJECT (renderer), "align-set", TRUE, "xalign", 0.5, NULL);
gtk_tree_view_column_set_alignment (column, 0.5);

static_contents (treeview);

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

gtk_widget_show (dialog);
}
