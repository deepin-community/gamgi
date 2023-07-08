/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_select.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_mesa_select.h"
#include "gamgi_io_token.h"

void gamgi_gtk_arrow_select (GtkWidget *widget, void *data)
{
printf ("handle arrow select\n");
}

void gamgi_gtk_shape_select (GtkWidget *widget, void *data)
{
printf ("handle shape select\n");
}

void gamgi_gtk_graph_select (GtkWidget *widget, void *data)
{
printf ("handle graph select\n");
}

void gamgi_gtk_select_mode (char *name, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkTreeSelection *selection;

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));

if (strcmp (name, "Pick") == 0)
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);
else
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);
}

void gamgi_gtk_select_method_direct (char *label, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *widget;
GtkWidget *hbox;
GtkWidget *entry;

widget = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), label);
hbox = gtk_widget_get_parent (widget);
gtk_widget_destroy (widget);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), label, entry);
gtk_widget_show (entry);

gtk_widget_set_sensitive (entry, FALSE);
}

void gamgi_gtk_select_method_bool (char *label, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *widget;
GtkWidget *hbox;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

widget = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), label);
hbox = gtk_widget_get_parent (widget);
gtk_widget_destroy (widget);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), label, combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, " Yes ", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, " No ", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

void gamgi_gtk_select_method_default (char *label, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *widget;
GtkWidget *hbox;
GtkWidget *entry;

widget = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), label);
hbox = gtk_widget_get_parent (widget);
gtk_widget_destroy (widget);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), label, entry);
gtk_widget_show (entry);
}

gamgi_dlist *gamgi_gtk_select_ok_pick (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_object *object;
gamgi_dlist *dlist, *dlist_new;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
int number;

/*********************
 * get treeview data *
 *********************/

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));

/***************************************
 * when no objects have been selected, *
 * just include or exclude everything  *
 ***************************************/

dlist_new = NULL;

number = gtk_tree_selection_count_selected_rows (selection);
if (number == 0)
  {
  if (action == FALSE)
    {
    for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
      {
      dlist_new = gamgi_engine_dlist_add_start (dlist_new);
      dlist_new->data = dlist->data;
      }
    }
  return dlist_new;
  }

/*******************************************
 * otherwise traverse first level of model *
 * to build new list with selected objects *
 *******************************************/

gtk_tree_model_get_iter_first (model, &iter);
do
  {
  if (gtk_tree_selection_iter_is_selected (selection, &iter) != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    gtk_tree_model_get (model, &iter, 1, &object, -1);
    dlist_new->data = object;
    }
  }
while (gtk_tree_model_iter_next (model, &iter) == TRUE);

return dlist_new;
}

gamgi_dlist *gamgi_gtk_select_ok_region (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_object *object, *parent;
gamgi_dlist *dlist, *dlist_new;
gamgi_slist *slist, *slist_region;
gamgi_bool condition;
int class;

/********************
 * get object class *
 ********************/

if (dlist_old == NULL) return NULL;
object = GAMGI_CAST_OBJECT dlist_old->data;
class = object->class;

/***************************************
 * when no objects have been selected, *
 * just include or exclude everything  *
 ***************************************/

dlist_new = NULL;

if (gamgi->state == GAMGI_MESA_SELECT_READY)
  {
  if (action == FALSE)
    {
    for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
      {
      dlist_new = gamgi_engine_dlist_add_start (dlist_new);
      dlist_new->data = dlist->data;
      }
    }
  return dlist_new;
  }

/****************************************************************
 * mark all objects in scope: after this, if object->selection  *
 * remains NULL then object is not in scope and will be ignored *
 ****************************************************************/

for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  (GAMGI_CAST_OBJECT dlist->data)->selection = dlist;

/*****************************************************************
 * find all objects inside selection rectangle that are in scope *
 *****************************************************************/

slist_region = gamgi_mesa_select_region (gamgi->box,
class, gamgi->window_mouse, window);
  
/************************************************************
 * 1) mark all objects in scope found inside the rectangle  *
 ************************************************************/

for (slist = slist_region; slist != NULL; slist = slist->next)
  (GAMGI_CAST_OBJECT slist->data)->selection = NULL;

/******************************************************
 * recursive objects: all parents with the same class *
 * are necessarily in scope and must be selected also *
 ******************************************************/

if (class == GAMGI_ENGINE_ASSEMBLY || class == GAMGI_ENGINE_CLUSTER ||
class == GAMGI_ENGINE_GROUP || class == GAMGI_ENGINE_TEXT)
  {
  for (slist = slist_region; slist != NULL; slist = slist->next)
    {
    object = GAMGI_CAST_OBJECT slist->data;
    parent = object->object;
    while (parent->class == object->class)
      {
      parent->selection = NULL;
      object = parent;
      parent = object->object;
      }
    }
  }

/*******************************************
 * remove list of objects inside rectangle *
 *******************************************/

gamgi_engine_slist_remove_all (slist_region);

/**********************************
 * build list of selected objects *
 **********************************/

for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  object = GAMGI_CAST_OBJECT dlist->data;
  if (object->selection == NULL) condition = TRUE;
  else condition = FALSE;

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = object;
    }
  }

/******************************************
 * unmark all objects in scope, including *
 * selected parents of recursive objects  *
 ******************************************/

for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  (GAMGI_CAST_OBJECT dlist->data)->selection = NULL;

return dlist_new;
}

gamgi_bool gamgi_gtk_select_entry_alpha (char *valid,
int length, char *min, char *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

/********************
 * get range limits *
 ********************/

*min = '\0';
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
min, valid, length) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = '\0';
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
max, valid, length) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_entry_double (double lower,
double upper, double *min, double *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
double swap;

/********************
 * get range limits *
 ********************/

*min = lower;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name,
min, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = upper;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name,
max, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_entry_float (float lower,
float upper, float *min, float *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
float swap;

/********************
 * get range limits *
 ********************/

*min = lower;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_float_scan (name,
min, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = upper;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_float_scan (name,
max, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_entry_int (int lower,
int upper, int *min, int *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
int swap;

/********************
 * get range limits *
 ********************/

*min = lower;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_int_scan (name,
min, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = upper;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_int_scan (name,
max, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_combo_entry_alpha (char *valid,
int length, char *min, char *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char *name;

/********************
 * get range limits *
 ********************/

*min = '\0';
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
min, valid, length) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = '\0';
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
max, valid, length) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_combo_entry_float (float lower,
float upper, float *min, float *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char *name;
float swap;

/********************
 * get range limits *
 ********************/

*min = lower;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_float_scan (name,
min, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = upper;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_float_scan (name,
max, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

return TRUE;
}

gamgi_bool gamgi_gtk_select_combo_entry_int (int lower,
int upper, int *min, int *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char *name;
int swap;

/********************
 * get range limits *
 ********************/

*min = lower;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_int_scan (name,
min, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

*max = upper;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_int_scan (name,
max, lower, upper) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return FALSE;
  }

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

return TRUE;
}

void gamgi_gtk_select_combo_int (int *min, int *max, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int swap;

/********************
 * get range limits *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
*min = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
*max = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (*min > *max)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  swap = *min;
  *min = *max;
  *max = swap;
  }

}

void gamgi_gtk_select_merge (gamgi_dlist **current, gamgi_dlist *start)
{
gamgi_dlist *before, *old, *new, *dlist;

/********************************************
 * get list Before, initialize list Current *
 ********************************************/

before = *current;
*current = dlist = NULL;

/************************************
 * copy list Before to list Current *
 ************************************/

for (old = before; old != NULL; old = old->next)
  {
  if (dlist == NULL)
    *current = dlist = gamgi_engine_dlist_add_end (NULL);
  else
    dlist = gamgi_engine_dlist_add_end (dlist);
  dlist->data = old->data;
  }

/***********************************
 * append new list to list Current *
 ***********************************/

for (new = start; new != NULL; new = new->next)
  {
  for (old = *current; old != NULL; old = old->next)
    if (new->data == old->data) break;

  if (old == NULL)
    {
    if (dlist == NULL)
      *current = dlist = gamgi_engine_dlist_add_end (NULL);
    else
      dlist = gamgi_engine_dlist_add_end (dlist);
    dlist->data = new->data;
    }
  }

/********************************************
 * remove new list: objects not in previous *
 * Current list have already been appended  *
 ********************************************/

gamgi_engine_dlist_remove_all (start);
}

GtkTreeIter gamgi_gtk_select_row (GtkTreeStore *store, 
GtkTreeIter *parent, gamgi_object *object)
{
GtkTreeIter iter;
char string[GAMGI_ENGINE_STRING];

sprintf (string, "%s %d", object->name, object->number);

gtk_tree_store_append (store, &iter, parent);
gtk_tree_store_set (store, &iter, 0, string, 1, object, -1);

return iter;
}

GtkWidget *gamgi_gtk_select_dialog (gamgi_window *window, 
gamgi_callback2 function_ok)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *scrolled_window;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;

gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC); 
gtk_widget_show (scrolled_window);
  
treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
gtk_widget_set_size_request (treeview,
GAMGI_GTK_SELECT_WIDTH, GAMGI_GTK_SELECT_HEIGHT);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_object_set_data (G_OBJECT (dialog), "treeview", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Object", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (function_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

return treeview;
}

void gamgi_gtk_select_text (GtkTreeStore *store, 
GtkTreeIter *iter_parent, gamgi_text *text)
{
gamgi_dlist *dlist;
gamgi_text *child;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT text);

dlist = text->text_start;
while (dlist != NULL)
  {
  child = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, child);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_orbital (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_orbital *orbital)
{
gamgi_dlist *dlist;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT orbital);

dlist = orbital->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }
}

void gamgi_gtk_select_bond (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_bond *bond)
{
gamgi_dlist *dlist;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT bond);

dlist = bond->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = bond->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_atom (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_atom *atom)
{
gamgi_dlist *dlist;
gamgi_bond *bond;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT atom);

dlist = atom->bond_start;
while (dlist != NULL)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  gamgi_gtk_select_bond (store, &iter, bond);
  dlist = dlist->next;
  }

dlist = atom->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = atom->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_direction (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_direction *direction)
{
gamgi_dlist *dlist;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT direction);

dlist = direction->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = direction->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = direction->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_plane (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_plane *plane)
{
gamgi_dlist *dlist;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT plane);

dlist = plane->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = plane->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = plane->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = plane->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_group (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_group *group)
{
gamgi_dlist *dlist;
gamgi_group *child;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT group);

dlist = group->group_start;
while (dlist != NULL)
  {
  child = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, child);
  dlist = dlist->next;
  }

dlist = group->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = group->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = group->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = group->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = group->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_molecule (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_molecule *molecule)
{
gamgi_dlist *dlist;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT molecule);

dlist = molecule->group_start;
while (dlist != NULL)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, group);
  dlist = dlist->next;
  }

dlist = molecule->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = molecule->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = molecule->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = molecule->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = molecule->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_cluster (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_cluster *cluster)
{
gamgi_dlist *dlist;
gamgi_cluster *child;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT cluster);

dlist = cluster->cluster_start;
while (dlist != NULL)
  {
  child = GAMGI_CAST_CLUSTER dlist->data;
  gamgi_gtk_select_cluster (store, &iter, child);
  dlist = dlist->next;
  }

dlist = cluster->molecule_start;
while (dlist != NULL)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;
  gamgi_gtk_select_molecule (store, &iter, molecule);
  dlist = dlist->next;
  }

dlist = cluster->group_start;
while (dlist != NULL)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, group);
  dlist = dlist->next;
  }

dlist = cluster->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = cluster->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = cluster->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = cluster->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = cluster->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_cell (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_cell *cell)
{
gamgi_dlist *dlist;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT cell);

dlist = cell->cluster_start;
while (dlist != NULL)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  gamgi_gtk_select_cluster (store, &iter, cluster);
  dlist = dlist->next;
  }

dlist = cell->molecule_start;
while (dlist != NULL)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;
  gamgi_gtk_select_molecule (store, &iter, molecule);
  dlist = dlist->next;
  }

dlist = cell->group_start;
while (dlist != NULL)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, group);
  dlist = dlist->next;
  }

dlist = cell->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = cell->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = cell->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = cell->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = cell->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_arrow (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_arrow *arrow)
{
gamgi_dlist *dlist;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT arrow);

dlist = arrow->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }
}

void gamgi_gtk_select_shape (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_shape *shape)
{
gamgi_dlist *dlist;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT shape);

dlist = shape->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_graph (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_graph *graph)
{
gamgi_dlist *dlist;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT graph);

dlist = graph->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }
}

void gamgi_gtk_select_assembly (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_assembly *assembly)
{
gamgi_dlist *dlist;
gamgi_assembly *child;
gamgi_graph *graph;
gamgi_shape *shape;
gamgi_arrow *arrow;
gamgi_cell *cell;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT assembly);

dlist = assembly->assembly_start;
while (dlist != NULL)
  {
  child = GAMGI_CAST_ASSEMBLY dlist->data;
  gamgi_gtk_select_assembly (store, &iter, child);
  dlist = dlist->next;
  }

dlist = assembly->graph_start;
while (dlist != NULL)
  {
  graph = GAMGI_CAST_GRAPH dlist->data;
  gamgi_gtk_select_graph (store, &iter, graph);
  dlist = dlist->next;
  }

dlist = assembly->shape_start;
while (dlist != NULL)
  {
  shape = GAMGI_CAST_SHAPE dlist->data;
  gamgi_gtk_select_shape (store, &iter, shape);
  dlist = dlist->next;
  }

dlist = assembly->arrow_start;
while (dlist != NULL)
  {
  arrow = GAMGI_CAST_ARROW dlist->data;
  gamgi_gtk_select_arrow (store, &iter, arrow);
  dlist = dlist->next;
  }

dlist = assembly->cell_start;
while (dlist != NULL)
  {
  cell = GAMGI_CAST_CELL dlist->data;
  gamgi_gtk_select_cell (store, &iter, cell);
  dlist = dlist->next;
  }

dlist = assembly->cluster_start;
while (dlist != NULL)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  gamgi_gtk_select_cluster (store, &iter, cluster);
  dlist = dlist->next;
  }

dlist = assembly->molecule_start;
while (dlist != NULL)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;
  gamgi_gtk_select_molecule (store, &iter, molecule);
  dlist = dlist->next;
  }

dlist = assembly->group_start;
while (dlist != NULL)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, group);
  dlist = dlist->next;
  }

dlist = assembly->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = assembly->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = assembly->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = assembly->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }

dlist = assembly->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_light (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_light *light)
{
gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT light);
}

void gamgi_gtk_select_layer (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_layer *layer)
{
gamgi_dlist *dlist;
gamgi_light *light;
gamgi_assembly *assembly;
gamgi_graph *graph;
gamgi_shape *shape;
gamgi_arrow *arrow;
gamgi_cell *cell;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_orbital *orbital;
gamgi_text *text;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT layer);

dlist = layer->light_start;
while (dlist != NULL)
  {
  light = GAMGI_CAST_LIGHT dlist->data;
  gamgi_gtk_select_light (store, &iter, light);
  dlist = dlist->next;
  }

dlist = layer->assembly_start;
while (dlist != NULL)
  {
  assembly = GAMGI_CAST_ASSEMBLY dlist->data;
  gamgi_gtk_select_assembly (store, &iter, assembly);
  dlist = dlist->next;
  }

dlist = layer->graph_start;
while (dlist != NULL)
  {
  graph = GAMGI_CAST_GRAPH dlist->data;
  gamgi_gtk_select_graph (store, &iter, graph);
  dlist = dlist->next;
  }

dlist = layer->shape_start;
while (dlist != NULL)
  {
  shape = GAMGI_CAST_SHAPE dlist->data;
  gamgi_gtk_select_shape (store, &iter, shape);
  dlist = dlist->next;
  }

dlist = layer->arrow_start;
while (dlist != NULL)
  {
  arrow = GAMGI_CAST_ARROW dlist->data;
  gamgi_gtk_select_arrow (store, &iter, arrow);
  dlist = dlist->next;
  }

dlist = layer->cell_start;
while (dlist != NULL)
  {
  cell = GAMGI_CAST_CELL dlist->data;
  gamgi_gtk_select_cell (store, &iter, cell);
  dlist = dlist->next;
  }

dlist = layer->cluster_start;
while (dlist != NULL)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  gamgi_gtk_select_cluster (store, &iter, cluster);
  dlist = dlist->next;
  }

dlist = layer->molecule_start;
while (dlist != NULL)
  {
  molecule = GAMGI_CAST_MOLECULE dlist->data;
  gamgi_gtk_select_molecule (store, &iter, molecule);
  dlist = dlist->next;
  }

dlist = layer->group_start;
while (dlist != NULL)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  gamgi_gtk_select_group (store, &iter, group);
  dlist = dlist->next;
  }

dlist = layer->plane_start;
while (dlist != NULL)
  {
  plane = GAMGI_CAST_PLANE dlist->data;
  gamgi_gtk_select_plane (store, &iter, plane);
  dlist = dlist->next;
  }

dlist = layer->direction_start;
while (dlist != NULL)
  {
  direction = GAMGI_CAST_DIRECTION dlist->data;
  gamgi_gtk_select_direction (store, &iter, direction);
  dlist = dlist->next;
  }

dlist = layer->atom_start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_gtk_select_atom (store, &iter, atom);
  dlist = dlist->next;
  }

dlist = layer->orbital_start;
while (dlist != NULL)
  {
  orbital = GAMGI_CAST_ORBITAL dlist->data;
  gamgi_gtk_select_orbital (store, &iter, orbital);
  dlist = dlist->next;
  }
 
dlist = layer->text_start;
while (dlist != NULL)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_gtk_select_text (store, &iter, text);
  dlist = dlist->next;
  }

}

void gamgi_gtk_select_window (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_window *window)
{
gamgi_dlist *dlist;
gamgi_layer *layer;
GtkTreeIter iter;

iter = gamgi_gtk_select_row (store, iter_parent, GAMGI_CAST_OBJECT window);

dlist = window->layer_start;
while (dlist != NULL)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_gtk_select_layer (store, &iter, layer);
  dlist = dlist->next;
  }
}
