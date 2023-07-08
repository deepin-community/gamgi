/*********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_copy.c
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
#include "gamgi_engine_copy.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_LAYER, STATIC_LOCAL };

static struct {
gamgi_bool local;
char name[GAMGI_ENGINE_TOKEN];
int number;
gamgi_layer *layer;
double shift[3]; } cache;

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
const char *name;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/*******************************************
 * when entries are empty, look for a text *
 *******************************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_TEXT;

/******************************
 * otherwise look for a layer *
 ******************************/

return GAMGI_ENGINE_LAYER;
}

static void static_default_layer (gamgi_window *window)
{
gamgi_object *object;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[GAMGI_ENGINE_STRING];

object = GAMGI_CAST_OBJECT window->layer;
sprintf (string, "%s %d", object->name, object->number);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_destination (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
int row;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_destination");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_LAYER)
  {
  gtk_widget_set_sensitive (entry, TRUE);
  static_default_layer (window);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
}

/**************** internal function ************
 *                                             *
 *                   STATIC_RESET              *
 *                                             *
 ***********************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[GAMGI_ENGINE_TOKEN];

/***********************************************
 * initialize text, name of copies, number     *
 * of copies, copies destination, copies shift *
 ***********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_name");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
sprintf (string, "%d", GAMGI_GTK_COPY_NUMBER);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_destination");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_LAYER);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (string, "%f", GAMGI_GTK_COPY_X);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (string, "%f", GAMGI_GTK_COPY_Y);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (string, "%f", GAMGI_GTK_COPY_Z);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/****************** internal function **************
 *                                                 *
 *                     STATIC_LIST                 *
 *                                                 *
 ***************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *combo;
gamgi_bool *sensitive;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_text");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_destination");
sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_destination");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /****************************
   * Choose the text directly *
   ****************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_LAYER);
  sensitive[STATIC_LOCAL] = FALSE;
  }
else
  {
  /****************************************
   * Choose the texts previously selected *
   ****************************************/

  static_reset (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_LAYER);
  sensitive[STATIC_LOCAL] = TRUE;
  }

}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char *name;
int row;

/********
 * name *
 ********/

cache.name[0] = '\0';
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_name");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid copy name", window);
    return FALSE;
    }
  }

/**********
 * number *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cache.number, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid copy number", window);
  return FALSE;
  }

/*********
 * layer *
 *********/

cache.layer = NULL;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_destination");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_LAYER)
  {
  cache.layer = GAMGI_CAST_LAYER gamgi_gtk_object_name_number (dialog,
  "entry_layer", GAMGI_ENGINE_LAYER, NULL);
  if (cache.layer == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid copy destination", window);
    return FALSE;
    }
  }

/***************
 * x,y,z shift *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.shift[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid shift data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.shift[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid shift data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.shift[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid shift data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_text *text, gamgi_window *window)
{
gamgi_layer *layer;
gamgi_text *text_new = NULL;
double translate[3];
int i;

/*******************
 * find host layer *
 *******************/

if (cache.layer == NULL)
  layer = gamgi_engine_find_layer (GAMGI_CAST_OBJECT text);
else
  layer = cache.layer;

if (cache.local == TRUE)
  gamgi_gtk_history_update (layer, layer->undo, GAMGI_ENGINE_UNDO);

gamgi_math_vector_zero (translate);
for (i = 0; i < cache.number; i++)
  {
  text_new = gamgi_engine_copy_text (text);
  if (cache.name[0] != '\0')
    strcpy (text_new->object.name, cache.name);

  gamgi_engine_link_text_layer (text_new, cache.layer);

  gamgi_math_vector_add (translate, cache.shift, translate);
  gamgi_mesa_translate_text (text_new, translate);
  }

return TRUE;
}

static void static_ok_global (gamgi_window *window_dialog)
{
gamgi_window *window_mouse;
gamgi_layer *layer;
gamgi_object *object;
gamgi_dlist *dlist;
gamgi_slist *start;

cache.local = FALSE;

/********************************
 * get object list, dialog data *
 ********************************/

if (gamgi->texts == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of text objects", window_dialog); return; }

if (static_grab (window_dialog) == FALSE) return;

layer = cache.layer;
if (layer == NULL)
  {
  /************************************************
   * For all windows: save layers as undo layers  *
   * and set current layer as new current object  *
   ************************************************/

  gamgi_global_selection_undo ();
  gamgi_global_selection_focus ();
  }
else
  {
  /******************************************
   * Save destiny host layer as undo buffer *
   * layer, so it can be recovered later.   *
   ******************************************/

  gamgi_gtk_history_update (layer, layer->undo, GAMGI_ENGINE_UNDO);
  }

/***************************************
 * copy texts according to dialog data *
 ***************************************/

start = NULL;
for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_TEXT dlist->data, window_dialog) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window_dialog);

if (layer == NULL)
  {
  /*****************************************
   * For all windows: redraw gl_area image *
   ****************************************/

  gamgi_mesa_draw_gamgi ();
  }
else
  {
  /*****************************************
   * raise window containing layer to the  *
   * top and if layer is not on top, set   *
   * layer as the current layer and object *
   *                                       *
   *    any text object can be used here   *
   *****************************************/

  object = GAMGI_CAST_OBJECT gamgi->texts->data;
  gamgi_gtk_object_copy (object, GAMGI_CAST_OBJECT layer, window_dialog);

  /********************************************
   * Redraw gl_area image in window_mouse and *
   * reset the copy dialog in window_dialog   *
   ********************************************/

  window_mouse =  GAMGI_CAST_WINDOW layer->object.object;
  gtk_widget_queue_draw (window_mouse->area);
  }

static_reset (window_dialog);
}

static void static_ok_local (gamgi_window *window_dialog)
{
gamgi_window *window_mouse;
gamgi_layer *layer;
gamgi_text *text;
GtkWidget *dialog = window_dialog->dialog0;

cache.local = TRUE;

/********************************
 * pick object, get dialog data *
 ********************************/

text = GAMGI_CAST_TEXT gamgi_gtk_object_name_number (dialog,
"entry_text", GAMGI_ENGINE_TEXT, window_dialog);
if (text == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid text object", window_dialog);
  return;
  }

if (static_grab (window_dialog) == FALSE) return;

/**************************************
 * copy text according to dialog data *
 **************************************/

static_apply (text, window_dialog);

/******************************************
 * get and save host layer as undo buffer *
 * layer, so it can be recovered later.   *
 ******************************************/

if (cache.layer == NULL)
  layer = gamgi_engine_find_layer (GAMGI_CAST_OBJECT text);
else
  layer = cache.layer;

/*****************************************
 * raise window containing layer to the  *
 * top and if layer is not on top, set   *
 * layer as the current layer and object *
 *****************************************/

gamgi_gtk_object_copy (GAMGI_CAST_OBJECT text,
GAMGI_CAST_OBJECT layer, window_dialog);

/********************************************
 * Redraw gl_area image in window_mouse and *
 * reset the copy dialog in window_dialog   *
 ********************************************/

window_mouse =  GAMGI_CAST_WINDOW layer->object.object;
gtk_widget_queue_draw (window_mouse->area);

static_reset (window_dialog);
}

/**************** internal function ************
 *                                             *
 *                    STATIC_OK                *
 *                                             *
 ***********************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

/*******************************************
 * copy data: execute global or local task *
 *******************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_global (window);
else
  static_ok_local (window);
}

static void static_init (gamgi_window *window)
{
static_reset (window);
static_list (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window_dialog = gamgi->window_dialog;
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];
const char *name;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
gamgi_io_token_check (name) == TRUE)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *           GAMGI_GTK_TEXT_COPY_PRESS         *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_text_copy_press (gamgi_window *window_mouse,  
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *combo;
GtkWidget *button;
GtkWidget *entry;
gamgi_bool global;
const char *name;
int row;

/**************************************************
 * local destination is active only in List mode, *
 * so objects and locations are already known     *
 *                                                *
 * global selection can be used only to select    *
 * the destination, the object to copy is local   *
 **************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_destination");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_LOCAL) return;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/************************************************************
 * global selection can be used to select the second object *
 * or the list of objects, not the first (main) object      *
 ************************************************************/

global = TRUE;
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) global = FALSE;

/************************************************************************
 * window_dialog is the (local) window where the dialog window was open *
 * window_mouse is the (global) window where the user presses the mouse *
 ************************************************************************/

if (global == FALSE && window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y,
static_class (window_dialog), global, static_press);
}

/*************** external function *************
 *                                             *
 *              GAMGI_GTK_TEXT_COPY            *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_text_copy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Text Copy", window);
window->action = GAMGI_GTK_TEXT_COPY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/***************
 * Text object *
 ***************/

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
g_object_set_data (G_OBJECT (dialog), "hbox_text", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Text");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "entry_text", entry);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
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

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/****************
 * Name, Number *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Name");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_name", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Number");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_number", entry);
gtk_widget_show (entry);

/***************
 * Destination *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Destination");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_destination), window);
g_object_set_data (G_OBJECT (dialog), "combo_destination", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Layer", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_destination", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_layer", entry);
gtk_widget_show (entry);

/**************
 * Shift page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Shift ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***************
 * Translation *
 ***************/

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
