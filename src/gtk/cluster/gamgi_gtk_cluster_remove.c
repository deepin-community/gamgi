/*****************************************************
 *
 * $GAMGI/src/gtk/cluster/gamgi_gtk_cluster_remove.c
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

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_reparent.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_io_token.h"
#include "gamgi_global_selection.h"

static void static_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/***********************
 * clean cluster name *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cluster");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/***************** internal function ******************
 *                                                    *
 *                    STATIC_RESET                    *
 *                                                    *
 * Reset the dialog: clean the cluster name and scope *
 *                                                    *
 ******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

/***********************
 * reset cluster name *
 ***********************/

static_clean (window);

/*************************
 * reset scope combo box *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

/************************ internal function ***************
 *                                                        *
 *                         STATIC_LIST                    *
 *                                                        *
 * When the List button is on, disable the cluster entry  *
 * and reset dialog, otherwise, enable the cluster entry. *
 **********************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *hbox;
GtkWidget *button;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_cluster");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*******************************
   * Choose the cluster directly *
   *******************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  }
else
  {
  /*******************************************
   * Choose the clusters previously selected *
   *******************************************/

  static_reset (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  }
}

static void static_apply (gamgi_cluster *cluster, int scope,
gamgi_window *window)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_object *parent;
gamgi_dlist *dlist, *dlist_b;

/*****************************************************
 * Remove cluster data according to Scope combo box: *
 * 1) All: remove cluster and all its contents;      *
 * 2) Container: link all child objects in           *
 * cluster to cluster parent and remove cluster;     *
 * 3) Contents: remove all child objects in cluster; *
 * 4) Child: Remove the child list of a given class; *
 *****************************************************/

/********************************************
 * All: remove cluster and all its contents *
 *******************************************/

if (scope == 0)
  {
  gamgi_engine_unlink_cluster (cluster);
  gamgi_engine_remove_cluster (cluster);
  }

/*****************************************************************
 * Container: link contents to cluster parent and remove cluster *
 *****************************************************************/

if (scope == 1)
  {
  parent = cluster->object.object;
  gamgi_engine_reparent_cluster (cluster->cluster_start, parent);
  gamgi_engine_reparent_molecule (cluster->molecule_start, parent);
  gamgi_engine_reparent_group (cluster->group_start, parent);
  gamgi_engine_reparent_plane (cluster->plane_start, parent);
  gamgi_engine_reparent_direction (cluster->direction_start, parent);
  gamgi_engine_reparent_atom (cluster->atom_start, parent);
  gamgi_engine_reparent_orbital (cluster->orbital_start, parent);
  gamgi_engine_reparent_text (cluster->text_start, parent);
  gamgi_engine_unlink_cluster (cluster);
  gamgi_engine_remove_cluster (cluster);
  }

/*************************************************
 * Contents: remove all child objects in cluster *
 *************************************************/

if (scope == 2)
  {
  dlist = cluster->cluster_start;
  while (dlist != NULL)
    { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->cluster_start = NULL;
  cluster->cluster_end = NULL;

  dlist = cluster->molecule_start;
  while (dlist != NULL)
    { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->molecule_start = NULL;
  cluster->molecule_end = NULL;

  dlist = cluster->group_start;
  while (dlist != NULL)
    { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->group_start = NULL;
  cluster->group_end = NULL;

  dlist = cluster->plane_start;
  while (dlist != NULL)
    { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->plane_start = NULL;
  cluster->plane_end = NULL;

  dlist = cluster->direction_start;
  while (dlist != NULL)
    { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->direction_start = NULL;
  cluster->direction_end = NULL;

  dlist = cluster->atom_start;
  while (dlist != NULL)
    { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->atom_start = NULL;
  cluster->atom_end = NULL;

  dlist = cluster->orbital_start;
  while (dlist != NULL)
    { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->orbital_start = NULL;
  cluster->orbital_end = NULL;

  dlist = cluster->text_start;
  while (dlist != NULL)
    { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->text_start = NULL;
  cluster->text_end = NULL;
  }

/*******************************************
 * Cluster: remove cluster list in cluster *
 *******************************************/

if (scope == 3)
  {
  dlist = cluster->cluster_start;
  while (dlist != NULL)
    { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->cluster_start = NULL;
  cluster->cluster_end = NULL;
  }

/*********************************************
 * Molecule: remove molecule list in cluster *
 *********************************************/

if (scope == 4)
  {
  dlist = cluster->molecule_start;
  while (dlist != NULL)
    { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->molecule_start = NULL;
  cluster->molecule_end = NULL;
  }

/***************************************
 * Group: remove group list in cluster *
 ***************************************/

if (scope == 5)
  {
  dlist = cluster->group_start;
  while (dlist != NULL)
    { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->group_start = NULL;
  cluster->group_end = NULL;
  }

/***************************************
 * Plane: remove plane list in cluster *
 ***************************************/

if (scope == 6)
  {
  dlist = cluster->plane_start;
  while (dlist != NULL)
    { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->plane_start = NULL;
  cluster->plane_end = NULL;
  }

/***********************************************
 * Direction: remove direction list in cluster *
 ***********************************************/

if (scope == 7)
  {
  dlist = cluster->direction_start;
  while (dlist != NULL)
    { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->direction_start = NULL;
  cluster->direction_end = NULL;
  }

/*************************************
 * Atom: remove atom list in cluster *
 *************************************/

if (scope == 8)
  {
  dlist = cluster->atom_start;
  while (dlist != NULL)
    { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->atom_start = NULL;
  cluster->atom_end = NULL;
  }

/**************************************************
 * Bond: remove bond list in atom list in cluster *
 **************************************************/

if (scope == 9)
  {
  dlist = cluster->atom_start;
  while (dlist != NULL)
    {
    atom = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;

    dlist_b = atom->bond_start;
    while (dlist_b != NULL)
       {
       bond = GAMGI_CAST_BOND dlist_b->data;
       dlist_b = dlist_b->next;
       gamgi_engine_unlink_bond (bond);
       gamgi_engine_remove_bond (bond);
       }
     }
  }

/*******************************************
 * Orbital: remove orbital list in cluster *
 *******************************************/

if (scope == 10)
  {
  dlist = cluster->orbital_start;
  while (dlist != NULL)
    { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->orbital_start = NULL;
  cluster->orbital_end = NULL;
  }

/*************************************
 * Text: remove text list in cluster *
 *************************************/

if (scope == 11)
  {
  dlist = cluster->text_start;
  while (dlist != NULL)
    { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  cluster->text_start = NULL;
  cluster->text_end = NULL;
  }

}

static void static_ok_global (int scope, gamgi_window *window)
{
gamgi_dlist *dlist, *clusters;

if (gamgi->clusters == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of cluster objects", window); return; }

/************************************************
 * for all windows: save layers as undo layers, *
 * set current layer as new current object      *
 ************************************************/

gamgi_global_selection_undo ();
gamgi_global_selection_focus ();

/*********************************************************
 * disable automatic mechanism to remove selection list, *
 * so we can scan the list and remove it only in the end *
 *********************************************************/

clusters = gamgi->clusters;
gamgi->clusters = NULL;

/**********************************************************
 *  remove cluster data as specified in scope combo box   *
 *                                                        *
 * clusters are recursive objects: a cluster may          *
 * recursively remove a child of the same class that      *
 * appears later in the list: listed objects that were    *
 * already removed will have a list node pointing to NULL *
 **********************************************************/

for (dlist = clusters; dlist != NULL; dlist = dlist->next)
  if (dlist->data != NULL)
    static_apply (GAMGI_CAST_CLUSTER dlist->data, scope, window);

/******************************************************
 * remove the selection list of a class of objects    *
 * only when objects of that class are really removed *
 ******************************************************/

gamgi->clusters = clusters;
if (scope < 4)
  gamgi_global_selection_remove (&gamgi->clusters, "button_cluster");

/*****************************************
 * For all windows: redraw gl_area image *
 *****************************************/

gamgi_mesa_draw_gamgi ();
}

static void static_ok_local (int scope, gamgi_window *window)
{
gamgi_cluster *cluster;
GtkWidget *dialog = window->dialog0;

/***************
 * Pick object *
 ***************/

cluster = GAMGI_CAST_CLUSTER gamgi_gtk_object_name_number (dialog,
"entry_cluster", GAMGI_ENGINE_CLUSTER, window);
if (cluster == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cluster object", window);
  return;
  }

/*************************************************
 * Before removing the cluster, check if the     *
 * current object is affected. In this case, the *
 * current layer becomes the new current object. *
 *************************************************/

if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT cluster) == TRUE)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/****************************************************
 * remove cluster data as chosen in scope combo box *
 ****************************************************/

static_apply (cluster, scope, window);

/************************************************
 * Redraw gl_area image and reset cluster entry *
 ************************************************/

gtk_widget_queue_draw (window->area);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *combo;
int scope;

/*********************************************************
 * get dialog data common to both local and global tasks *
 *********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
scope = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/********************************
 * execute global or local task *
 ********************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_global (scope, window);
else
  static_ok_local (scope, window);

/***********************************************************
 * clean dialog data common to both local and global tasks *
 ***********************************************************/

static_clean (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

/******************************************************
 * by default remove the cluster and all its contents *
 ******************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cluster");
sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

static_ok (NULL, window);
}

/***************** external function ****************
 *                                                  *
 *           GAMGI_GTK_CLUSTER_REMOVE_PRESS         *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cluster_remove_press (gamgi_window *window_mouse, 
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

gamgi_mesa_select_object (window_mouse, x, y, GAMGI_ENGINE_CLUSTER, FALSE, static_press);
}

/**************** external function *****************
 *                                                  *
 *             GAMGI_GTK_CLUSTER_REMOVE             *
 *                                                  *
 *   Creates a dialog to remove cluster objects.    *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cluster_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Cluster Remove", window);
window->action = GAMGI_GTK_CLUSTER_REMOVE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*******************
 * Cluster objects *
 *******************/

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
g_object_set_data (G_OBJECT (dialog), "hbox_cluster", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Cluster");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_cluster", entry);
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

/***************
 * Object page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Object");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**********
 * object *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/***********************************************
 * combo box describing what should be removed *
 ***********************************************/

label = gtk_label_new ("Scope");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_scope", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Container", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Contents", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Cluster", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Molecule", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Group", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Plane", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Direction", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Bond", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Orbital", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Text", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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
