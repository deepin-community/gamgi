/*************************************************
 *
 * $GAMGI/src/gtk/window/gamgi_gtk_window_copy.c
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
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_link.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_window_create.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_math_vector.h"
#include "gamgi_chem_bond.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"

enum { STATIC_LAYER, STATIC_LOCAL };

static struct { 
gamgi_bool local;
char name[GAMGI_ENGINE_TOKEN];
int number; } cache;

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

/**************** internal function ************
 *                                             *
 *                   STATIC_RESET              *
 *                                             *
 ***********************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[GAMGI_ENGINE_TOKEN];

/*******************************************************
 * initialize window, name of copies, number of copies *
 *******************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_name");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
sprintf (string, "%d", GAMGI_GTK_COPY_NUMBER);
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
GtkWidget *hbox, *button;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_window");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /******************************
   * Choose the window directly *
   ******************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  static_default_window (window);
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

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

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
  gamgi_gtk_dialog_message_create ("Error", "Invalid number data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_window *window_mouse, gamgi_window *window_dialog)
{
gamgi_window *window_new;
gamgi_layer *layer;
gamgi_dlist *dlist;
int i;

/***********************************
 * copy windows and their contents *
 ***********************************/

for (i = 0; i < cache.number; i++)
  {
  window_new = gamgi_engine_copy_window (window_mouse);
  if (cache.name[0] != '\0')
    strcpy (window_new->object.name, cache.name);

  for (dlist = window_new->layer_start; dlist != NULL; dlist = dlist->next)
    {
    layer = GAMGI_CAST_LAYER dlist->data;

    /**********************************
     * start undo and save mechanisms *
     **********************************/

    layer->undo = gamgi_engine_dlist_add_start (NULL);
    layer->save = gamgi_engine_dlist_add_start (NULL);
    layer->undo->data = layer;
    layer->save->data = layer;
    }

  gamgi_gtk_window_create (window_new, window_new->layer);
  }

return TRUE;
}

static void static_ok_global (gamgi_window *window_dialog)
{
gamgi_dlist *dlist;
gamgi_slist *start;

cache.local = FALSE;

/********************************
 * get object list, dialog data *
 ********************************/

if (gamgi->windows == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of window objects", window_dialog); return; }

if (static_grab (window_dialog) == FALSE) return;

/*****************************************
 * copy windows according to dialog data *
 ******************************************/

start = NULL;
for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_WINDOW dlist->data, window_dialog) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window_dialog);

/*******************************************
 * reset the copy dialog in current window *
 *******************************************/

static_reset (window_dialog);
}

static void static_ok_local (gamgi_window *window_dialog)
{
gamgi_window *window_mouse;
GtkWidget *dialog = window_dialog->dialog0;

cache.local = TRUE;

/********************************
 * pick object, get dialog data *
 ********************************/

window_mouse = GAMGI_CAST_WINDOW gamgi_gtk_object_name_number (dialog,
"entry_window", GAMGI_ENGINE_WINDOW, window_dialog);
if (window_mouse == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid window object", window_dialog);
  return;
  }

if (static_grab (window_dialog) == FALSE) return;

/**********************************************
 * copy window_mouse according to dialog data *
 **********************************************/

static_apply (window_mouse, window_dialog);

/*******************************************
 * reset the copy dialog in current window *
 *******************************************/

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
static_default_window (window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window_dialog = gamgi->window_dialog;
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_window");
sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_WINDOW_COPY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_window_copy_press (gamgi_window *window_mouse,  
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
gamgi_bool global;

/************************************************************
 * global selection can be used to select the second object *
 * or the list of objects, not the first (main) object      *
 ************************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

/***********************************************
 * global selection can be used only to select *
 * the second object, not the main object      *
 ***********************************************/

global = TRUE;
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  global = FALSE;

/************************************************************************
 * window_dialog is the (local) window where the dialog window was open *
 * window_mouse is the (global) window where the user presses the mouse *
 ************************************************************************/

if (global == FALSE && window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_WINDOW, global, static_press);
}

/*************** external function *************
 *                                             *
 *             GAMGI_GTK_WINDOW_COPY           *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_window_copy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Window Copy", window);
window->action = GAMGI_GTK_WINDOW_COPY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Window object *
 *****************/

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
g_object_set_data (G_OBJECT (dialog), "entry_window", entry);
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

/****************
 * Name, Number *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
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
