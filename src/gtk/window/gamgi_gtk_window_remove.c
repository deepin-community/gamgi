/***************************************************
 *
 * $GAMGI/src/gtk/window/gamgi_gtk_window_remove.c
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
#include "gamgi_gtk_window_create.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_gtk_gamgi_exit.h"
#include "gamgi_mesa_select.h"
#include "gamgi_io_token.h"
#include "gamgi_global_selection.h"

static void static_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/*********************
 * clean window name *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/****************** internal function ****************
 *                                                   *
 *                     STATIC_RESET                  *
 *                                                   *
 * Reset the dialog: clean the window name and scope *
 *                                                   *
 *****************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

/*********************
 * reset window name *
 *********************/

static_clean (window);

/*************************
 * reset scope combo box *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

/*********************** internal function ***************
 *                                                       *
 *                         STATIC_LIST                   *
 *                                                       *
 * When the List button is on, disable the window entry  *
 * and reset dialog, otherwise, enable the window entry. *
 *********************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *hbox;
GtkWidget *button;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_window");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /******************************
   * Choose the window directly *
   ******************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  }
else
  {
  /******************************************
   * Choose the windows previously selected *
   ******************************************/

  static_reset (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  }
}

static void static_apply (gamgi_window *window_mouse, 
int scope, gamgi_window *window_dialog)
{
gamgi_dlist *dlist;

/******************************************************
 * window_mouse is the selected window, window_dialog *
 * is the window where the dialog was opened          *
 ******************************************************/

/****************************************************
 * Remove window data according to Scope menu:      *
 * 1) All: remove window and all its contents;      *
 * 2) Contents: remove all child objects in window; *
 ****************************************************/

/*******************************************
 * All: remove window and all its contents *
 *******************************************/

if (scope == 0)
  {
  if (gamgi->window_start == gamgi->window_end)
    gamgi_gtk_gamgi_exit_ok (NULL, window_mouse);
  else
    gamgi_gtk_window_remove_ok (NULL, window_mouse);
  }

/************************************************
 * Contents: remove all child objects in window *
 ************************************************/

if (scope == 1)
  {
  dlist = window_mouse->layer_start;
  while (dlist != NULL)
    { gamgi_engine_remove_layer (GAMGI_CAST_LAYER dlist->data);
      dlist = gamgi_engine_dlist_remove_start (dlist); }
  window_mouse->layer_start = NULL;
  window_mouse->layer_end = NULL;

  gamgi_gtk_window_create_layer (window_mouse);
  gtk_widget_queue_draw (window_mouse->area);
  }

}

static void static_ok_global (int scope, gamgi_window *window_dialog)
{
gamgi_dlist *dlist, *windows;

if (gamgi->windows == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of window objects", window_dialog); return; }

/*********************************************************
 * disable automatic mechanism to remove selection list, *
 * so we can scan the list and remove it only in the end *
 *********************************************************/

windows = gamgi->windows;
gamgi->windows = NULL;

/******************************************************
 * remove window data as specified in scope combo box *
 ******************************************************/

for (dlist = windows; dlist != NULL; dlist = dlist->next)
  static_apply (GAMGI_CAST_WINDOW dlist->data, scope, window_dialog);

/*****************************************************************
 *       remove the selection list of a class of objects         *
 *       only when objects of that class are really removed      *
 *                                                               *
 * remove list and change button color for remaining windows     *
 * (when all windows are removed, all gtk widgets have already   *
 * been removed at this stage, but the last window still exists) *
 *****************************************************************/

gamgi->windows = windows;
if (scope < 1)
  {
  gamgi_engine_dlist_remove_all (gamgi->windows);
  gamgi->windows = NULL;

  if (gamgi->window_start->window != NULL)
    gamgi_global_selection_update_windows (&gamgi->windows, "button_window");
  }
}

static void static_ok_local (int scope, gamgi_window *window_dialog)
{
gamgi_window *window_mouse;
GtkWidget *dialog = window_dialog->dialog0;

/***************************************************************
 * window_mouse is the window where the user presses the mouse *
 * window_dialog is the window where the dialog window is open *
 ***************************************************************/

/***************
 * Pick method *
 ***************/

window_mouse = GAMGI_CAST_WINDOW gamgi_gtk_object_name_number (dialog,
"entry_window", GAMGI_ENGINE_WINDOW, window_dialog);
if (window_mouse == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", 
  "Invalid window object", window_dialog);
  return;
  }

/***************************************************
 * remove window data as chosen in scope combo box *
 ***************************************************/

static_apply (window_mouse, scope, window_dialog);
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
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

/*****************************
 * start with current window *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
sprintf (string, "%s %d", window->object.name, window->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*****************************************************
 * by default remove the window and all its contents *
 *****************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

void gamgi_gtk_window_remove_dialogs (gamgi_window *window)
{
/************************************************
 * remove local dialogs (including for current) *
 ************************************************/

if (window->dialog0 != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->dialog0);
if (window->dialog1 != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->dialog1);
if (window->help != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->help);
}

void gamgi_gtk_window_remove_timers (gamgi_window *window)
{
/********************************************
 * remove local timers (except for current) *
 ********************************************/

if (window->timer_focus != 0)
  g_source_remove (window->timer_focus);
if (window->timer_layer != 0)
  g_source_remove (window->timer_layer);
if (window->timer_message != 0)
  g_source_remove (window->timer_message);
if (window->timer_progress != 0)
  g_source_remove (window->timer_progress);
}

void gamgi_gtk_window_remove_window (gamgi_window *window)
{
/****************************
 * 1) remove timers         *
 * 2) remove Gtk window     *
 ****************************/

gamgi_gtk_window_remove_timers (window);
gtk_widget_destroy (GTK_WIDGET (window->window));
window->window = NULL;
}

void gamgi_gtk_window_remove_apply (gamgi_window *window)
{
/*****************************
 * 1) remove all Gtk dialogs *
 * 2) remove Gtk window      *
 * 3) remove Gamgi object    *
 *****************************/

gamgi_gtk_window_remove_dialogs (window);
gamgi_gtk_window_remove_window (window);
gamgi_engine_remove_window (window);
}

void gamgi_gtk_window_remove_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

/*************************
 * remove global dialogs *
 *                       *
 *     remove window     *
 *************************/

gamgi_gtk_dialog_global_remove ();

gamgi_gtk_window_remove_apply (window);
}

void gamgi_gtk_window_remove_confirm (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

if (gamgi->window_start == gamgi->window_end)
  gamgi_gtk_gamgi_exit (widget, window);
else
  gamgi_gtk_dialog_question_create ("Close",
  "Close the window?", gamgi_gtk_window_remove_ok, data);
}

/***************** external function ****************
 *                                                  *
 *          GAMGI_GTK_WINDOW_REMOVE_PRESS           *
 *                                                  *
 ****************************************************/

void gamgi_gtk_window_remove_press (gamgi_window *window_mouse, 
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

gamgi_mesa_select_object (window_mouse, x, y, GAMGI_ENGINE_WINDOW, FALSE, static_press);
}

/**************** external function *****************
 *                                                  *
 *             GAMGI_GTK_WINDOW_REMOVE              *
 *                                                  *
 * Creates the dialog window used to link windows.  *
 *                                                  *
 ****************************************************/

void gamgi_gtk_window_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Window Remove", window);
window->action = GAMGI_GTK_WINDOW_REMOVE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Window objects *
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
g_object_set_data (G_OBJECT (dialog), "hbox_window", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Window");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_window", entry);
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

/**********
 * object *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
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
gtk_list_store_set (store, &iter, 0, "Contents", -1);
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
