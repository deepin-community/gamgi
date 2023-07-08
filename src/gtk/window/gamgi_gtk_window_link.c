/*************************************************
 *
 * $GAMGI/src/gtk/window/gamgi_gtk_window_link.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_empty.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_OBJECT, STATIC_ALL };

static struct {
gamgi_bool local;
gamgi_enum method;
gamgi_object *object;
gamgi_bool above;
double origin[3]; } cache;

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
int row;

/**********************************************************
 * All mode: look for a window                            *
 * Object mode: look first for a window, then for a layer *
 **********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_ALL) return GAMGI_ENGINE_WINDOW;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_WINDOW;

return GAMGI_ENGINE_LAYER;
}

static void static_default_window (gamgi_window *window)
{
gamgi_object *object;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[GAMGI_ENGINE_STRING];

object = GAMGI_CAST_OBJECT window;
sprintf (string, "%s %d", object->name, object->number);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_method (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox;
GtkWidget *entry;
GtkWidget *hbox;
GtkWidget *combo;
int row;

/************
 * get data *
 ************/

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_object");
vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_shift");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

switch (row)
  {
  /***************************************************
   * The selected window collects all the non-empty  *
   * layers. By default, the selected window current *
   * layer will remain the current layer in the end. *
   ***************************************************/

  case STATIC_ALL:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (hbox, FALSE);
  gtk_widget_hide (vbox);
  break;

  case STATIC_OBJECT: default:
  gtk_widget_set_sensitive (hbox, TRUE);
  gtk_widget_show (vbox);
  break;
  }

}

/******************* internal function *****************
 *                                                     *
 *                      STATIC_RESET                   *
 *                                                     *
 * Reset the window link dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

/**************************
 * reset window and layer *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_OBJECT);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%f", GAMGI_GTK_LINK_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%f", GAMGI_GTK_LINK_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%f", GAMGI_GTK_LINK_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;       
GtkWidget *entry;                          

/*****************************
 * keep window, reset object *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/**************** internal function ************
 *                                             *
 *                   STATIC_LIST               *
 *                                             *
 * Makes the window entry sensitive when       *
 * the local button is pressed and insensitive *
 * when the global button is pressed.          *
 *                                             *
 ***********************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *hbox, *button;
gamgi_bool *sensitive;

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_method");
hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_window");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /******************************
   * Choose the window directly *
   ******************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  sensitive[STATIC_ALL] = TRUE;
  static_default_window (window);
  }
else
  {
  /******************************************
   * Choose the windows previously selected *
   ******************************************/

  static_reset (window);
  sensitive[STATIC_ALL] = FALSE;
  gtk_widget_set_sensitive (hbox, FALSE);
  }
}

static void static_origin (gamgi_object *child)
{
/*****************************************************
 * translation is always applied to the object below *
 *****************************************************/

gamgi_mesa_translate_object (child, cache.origin);
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;

/************************
 * get link option menu *
 ************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
cache.method = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (cache.method == STATIC_ALL) return TRUE;

/********************
 * get global layer *
 ********************/

cache.object = gamgi_gtk_object_name_number (dialog,
"entry_object", static_class (window), NULL);
if (cache.object == NULL )
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid layer object", window);
  return FALSE;
  }

/*********************
 * x,y,z coordinates *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_object *child,
gamgi_object *parent, gamgi_window *window_dialog)
{
/********************************************
 * check if child already belongs to parent *
 ********************************************/

if (child->object == parent)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Child already linked to parent", window_dialog);
  return FALSE;
  }

if (cache.local == FALSE)
  {
  /***************************************************************
   * get new equivalent layer and start undo and save mechanisms *
   ***************************************************************/

  child = gamgi_engine_copy_object (child);
  gamgi_gtk_history_start (GAMGI_CAST_LAYER child);
  }
else
  gamgi_engine_unlink_object (child);

gamgi_engine_link_object_object (child, parent);

/**************************
 * translate child object *
 **************************/

static_origin (child);

return TRUE;
}

static void static_ok_all (gamgi_window *window_mouse,
gamgi_window *window_dialog)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_dlist *dlist;

/***************************************************************
 * window_mouse is the window where the user presses the mouse *
 * window_dialog is the window where the dialog window is open *
 ***************************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  if (window == window_mouse) continue;

  /**************************************
   * each window must have at least one *
   * layer, so dlist cannot be NULL     *
   **************************************/

  dlist = window->layer_start;
  do
    {
    layer = GAMGI_CAST_LAYER dlist->data;
    dlist = dlist->next;

    /*************************************
     * If layer is current layer, then a *
     *  new current layer must be found. *
     *************************************/

    if (gamgi_engine_empty_layer (layer) == TRUE) continue;

    if (layer == window->layer)
      gamgi_gtk_object_layer_current (layer, window);

    gamgi_engine_unlink_layer (layer);
    gamgi_engine_link_layer_window (layer, window_mouse);
    window_mouse->layer = layer;
    }
    while (dlist != NULL);
  }

/********************************
 * remove all the other windows *
 ********************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  if (window != window_mouse) gamgi_gtk_window_remove_apply (window);

/********************************************************
 * the last layer linked (or the old one, when no layer *
 * was linked) becomes the new current layer and object *
 ********************************************************/

gamgi_gtk_object_layer_local (window_mouse->layer, window_mouse);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window_mouse->layer, window_mouse);

/*****************************************************
 * redraw window_mouse drawing area and close dialog *
 *****************************************************/

gtk_widget_queue_draw (window_mouse->area);

gamgi_gtk_dialog_task0_remove (NULL, window_dialog);
}

static void static_ok_global (gamgi_window *window_dialog)
{
gamgi_object *child, *parent;
gamgi_dlist *dlist;
gamgi_slist *start;

cache.local = FALSE;

/*******************
 * get window list *
 *******************/

if (gamgi->windows == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of window objects", window_dialog); return; }

if (static_grab (window_dialog) == FALSE) return;

/**********************************************
 * Link listed objects and show error message *
 * with objects that could not be linked      *
 *                                            *
 *      For all windows: set last layer       *
 *      as new current object and layer       *
 *                                            *
 *   For all windows: redraw gl_area image    *
 **********************************************/

/*****************************************
 * link each window of the list to a new *
 * layer copied from the selected layer  *
 *****************************************/

child = cache.object;

start = NULL;
for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  {
  parent = GAMGI_CAST_OBJECT dlist->data;

  if (static_apply (child, parent, window_dialog) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
  }
gamgi_io_error_list (start, window_dialog);

/************************************************************
 * for all windows: 1) last layer becomes current layer;    *
 * 2) current layer becomes current object; 3) redraw image *
 ************************************************************/

gamgi_global_selection_layer ();
gamgi_global_selection_focus ();
gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window_dialog);
}

static void static_ok_local (gamgi_window *window_dialog)
{
gamgi_window *window_mouse, *window_old;
gamgi_layer *layer;
GtkWidget *dialog = window_dialog->dialog0;

cache.local = TRUE;

/********************************
 * pick window, get dialog data *
 ********************************/

window_mouse = GAMGI_CAST_WINDOW gamgi_gtk_object_name_number (dialog,
"entry_window", GAMGI_ENGINE_WINDOW, window_dialog);
if (window_mouse == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid window object", window_dialog);
  return;
  }

/********************
 * grab dialog data *
 ********************/

if (static_grab (window_dialog) == FALSE) return;

/*************************************************
 * method ALL: move all non-empty layers to      *
 * current window, then remove the other windows *
 *************************************************/

if (cache.method == STATIC_ALL)
  { static_ok_all (window_mouse, window_dialog); return; }

/*******************************************************************
 * method OBJECT: link window to selected layer. If selected layer *
 * was current layer, then a  new current layer must be found.     *
 *******************************************************************/

layer = GAMGI_CAST_LAYER cache.object;
window_old = GAMGI_CAST_WINDOW layer->object.object;
if (layer == window_old->layer)
  gamgi_gtk_object_layer_current (layer, window_old);

static_apply (cache.object, GAMGI_CAST_OBJECT window_mouse, window_dialog);

/********************************************************
 * raise to top window and layer where parent object is *
 ********************************************************/

gamgi_gtk_object_raise (layer, window_mouse);

/***********************************************
 * redraw drawing area for new and old windows *
 ***********************************************/

gtk_widget_queue_draw (window_mouse->area);
gtk_widget_queue_draw (window_old->area);

static_clean (window_dialog);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

/*******************************************
 * link data: execute global or local task *
 *******************************************/

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
 * Set notebook current size as its maximum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
gtk_widget_set_size_request (notebook, size.width, size.height);

/********************************************
 * hide pages AFTER measuring notebook size *
 ********************************************/

static_reset (window);
static_default_window (window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window_dialog = gamgi->window_dialog;
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *entry;
char string[GAMGI_ENGINE_STRING];

if (static_class (window_dialog) == GAMGI_ENGINE_WINDOW)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
else
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/***************** external function ****************
 *                                                  *
 *             GAMGI_GTK_WINDOW_LINK_PRESS          *
 *                                                  *
 ****************************************************/

void gamgi_gtk_window_link_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
GtkWidget *entry;
gamgi_bool global;
const char *name;

/***********************************************
 * global selection can be used only to select *
 * the second object, not the main object(s)   *
 ***********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE && 
window_dialog != window_mouse) return;

/***********************************************
 * global selection can be used only to select *
 * the second object, not the main object      *
 ***********************************************/

global = TRUE;
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) global = FALSE;

/****************************************************************
 * window_dialog is the window where the dialog window was open *
 * window_mouse is the window where the user presses the mouse  *
 ****************************************************************/

if (global == FALSE && window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y, 
static_class (window_dialog), global, static_press);
}

/****************** external function ***************
 *                                                  *
 *                GAMGI_GTK_WINDOW_LINK             *
 *                                                  *
 *  Creates the dialog window used to link windows. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_window_link (GtkWidget *widget, void *data)
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
gamgi_bool *sensitive;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Window Link", window);
window->action = GAMGI_GTK_WINDOW_LINK;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************************
 * Window or list of Windows *
 *****************************/

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
g_signal_connect (button, "toggled", G_CALLBACK (static_list), window);
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

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * Object: Layer *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "hbox_object", hbox_left);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Layer");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0); 
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_object", entry);
gtk_widget_show (entry);

/**********
 * method *
 **********/

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

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Object", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_method", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/**************
 * Shift page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Shift ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_shift", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********************
 * x,y,z coordinates *
 *********************/

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

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
g_signal_connect (button, "clicked", G_CALLBACK (static_ok), window);
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
