/************************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_measure.c
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
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_measure.h"
#include "gamgi_mesa_select.h"
#include "gamgi_chem_geometry.h"
#include "gamgi_io_token.h"

static void static_method (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_reciprocal;
GtkWidget *vbox_voronoi1, *vbox_voronoi2, *vbox_voronoi3;
GtkWidget *combo;
int row;

/**********************
 * get notebook pages *
 **********************/

vbox_reciprocal = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_vbox_page");
vbox_voronoi1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_vbox_page1");
vbox_voronoi2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_vbox_page2");
vbox_voronoi3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_vbox_page3");

/**************
 * get method *
 **************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*************************
 * handle notebook pages *
 *************************/

if (row == 0)
  {
  /************
   * geometry *
   ************/

  gtk_widget_hide (vbox_reciprocal);
  gtk_widget_hide (vbox_voronoi1);
  gtk_widget_hide (vbox_voronoi2);
  gtk_widget_hide (vbox_voronoi3);
  }

if (row == 1)
  {
  /**************
   * reciprocal *
   **************/

  gtk_widget_show (vbox_reciprocal);
  gtk_widget_hide (vbox_voronoi1);
  gtk_widget_hide (vbox_voronoi2);
  gtk_widget_hide (vbox_voronoi3);
  gamgi_gtk_measure_reciprocal_init (window);
  }

if (row == 2)
  {
  /***********
   * voronoi *
   ***********/

  gtk_widget_hide (vbox_reciprocal);
  gtk_widget_show (vbox_voronoi1);
  gtk_widget_show (vbox_voronoi2);
  gtk_widget_show (vbox_voronoi3);
  gamgi_gtk_measure_voronoi_init (window);
  }

}

/******************** internal function ****************
 *                                                     *
 *                        STATIC_OK                    *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_object *object;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int row;

/************
 * get cell *
 ************/

object = gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (object == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell object", window);
  return;
  }

/****************************************************************
 * get method: get all the information from the dialog before   *
 * applying the method, after that the dialog no longer exists! *
 ****************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == 0) gamgi_gtk_measure_geometry_ok (object, window);
if (row == 1) gamgi_gtk_measure_reciprocal_ok (object, window);
if (row == 2) gamgi_gtk_measure_voronoi_ok (object, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkRequisition size;
GtkWidget *combo;

/***************************************************
 * Set notebook current size as its maximum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
gtk_widget_set_size_request (notebook, size.width, size.height);

/******************************
 * start with Symmetry method *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
static_method (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

static_ok (NULL, window);
}

/****************** external function ******************
 *                                                     *
 *           GAMGI_GTK_CELL_MEASURE_PRESS              *
 *                                                     *
 *                                                     *
 *******************************************************/

void gamgi_gtk_cell_measure_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
/******************************
 * local mouse selection only *
 ******************************/
 
if (window_dialog != window_mouse) return;
gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_CELL, FALSE, static_press);
}

/****************** external function ******************
 *                                                     *
 *                 GAMGI_GTK_CELL_MEASURE              *
 *                                                     *
 *                                                     *
 *******************************************************/

void gamgi_gtk_cell_measure (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Cell Measure", window);
window->action = GAMGI_GTK_CELL_MEASURE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/***************
 * Cell object *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Cell");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
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

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/**********
 * Method *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Method");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_method), window);
g_object_set_data (G_OBJECT (dialog), "combo_method", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Geometry", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Reciprocal", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Voronoi", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**************************
 * insert Reciprocal page *
 **************************/

gamgi_gtk_measure_reciprocal (notebook, window);

/************************
 * insert Voronoi pages *
 ************************/

gamgi_gtk_measure_voronoi (notebook, window);

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
