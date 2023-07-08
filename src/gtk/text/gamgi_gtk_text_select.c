/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_select.c
 *
 * Copyright (C) 2004, 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_math.h"
#include "gamgi_mesa.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_gtk_select.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_text_config.h"
#include "gamgi_mesa_select.h"
#include "gamgi_io_token.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_global_selection.h"

/*************************************************
 * local enumerations in GAMGI always start at 0 *
 *************************************************/

enum { STATIC_GAMGI, STATIC_WINDOW, STATIC_LAYER,
STATIC_BEFORE, STATIC_CURRENT };

/***********************************************************
 * GAMGI provides an Undo mechanism allowing users to      *
 * recover the list of objects selected before the current *
 * one. static_before points to this list, which is        *
 * available only while the Object->Select dialog is open. *
 ***********************************************************/

static gamgi_dlist *static_before;

static void static_destroy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

/**************************************************************
 * everytime the dialog is removed: 1) free list Before; 2)   *
 * link objects in list Current; 3) reset selection by region *
 **************************************************************/

gamgi_engine_dlist_remove_all (static_before);
gamgi_global_selection_link (gamgi->texts);
gamgi_mesa_select_reset (window);
}

static void static_model (gamgi_dlist *dlist, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkTreeStore *store;

store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
while (dlist != NULL)
  {
  gamgi_gtk_select_text (store, NULL, GAMGI_CAST_TEXT dlist->data);
  dlist = dlist->next;
  }

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);
}

static gamgi_dlist *static_dlist (int scope, gamgi_window *window)
{
gamgi_dlist *dlist = NULL;

switch (scope)
  {
  case STATIC_GAMGI:
  dlist = gamgi_engine_dlist_text_gamgi (NULL);
  break;

  case STATIC_WINDOW:
  dlist = gamgi_engine_dlist_text_window (window, NULL);
  break;

  case STATIC_LAYER:
  dlist = gamgi_engine_dlist_text_layer (window->layer, NULL);
  break;

  case STATIC_BEFORE:
  dlist = static_before;
  break;

  case STATIC_CURRENT:
  dlist = gamgi->texts;
  break;
  }

return dlist;
}

static void static_scope (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_dlist *dlist;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int scope;

/***********************
 * get scope combo box *
 ***********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
scope = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*********************************************
 * update model and remove unnecessary lists *
 *********************************************/

dlist = static_dlist (scope, window);
static_model (dlist, window);
if (scope != STATIC_BEFORE && scope != STATIC_CURRENT)
  gamgi_engine_dlist_remove_all (dlist);
}

static int static_font (gamgi_enum font)
{
switch (font)
  {
  case GAMGI_IO_SUN_ROMAN: return 0;
  case GAMGI_IO_SUN_ROMAN_MONO: return 1;

  case GAMGI_IO_HERSHEY_SANS:
  case GAMGI_IO_DEJAVU_SANS:
  return 2;

  case GAMGI_IO_HERSHEY_SANS_BOLD:
  case GAMGI_IO_DEJAVU_SANS_BOLD:
  return 3;

  case GAMGI_IO_DEJAVU_SANS_ITALIC: return 4;
  case GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC: return 5;

  case GAMGI_IO_HERSHEY_SERIF:
  case GAMGI_IO_DEJAVU_SERIF:
  return 6;

  case GAMGI_IO_HERSHEY_SERIF_BOLD:
  case GAMGI_IO_DEJAVU_SERIF_BOLD:
  return 7;

  case GAMGI_IO_DEJAVU_SERIF_ITALIC: return 8;
  case GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC: return 9;

  case GAMGI_IO_HERSHEY_SCRIPT: return 10;
  case GAMGI_IO_HERSHEY_SCRIPT_BOLD: return 11;
  case GAMGI_IO_HERSHEY_GOTHIC_ENGLISH: return 12;
  case GAMGI_IO_HERSHEY_GOTHIC_GERMAN: return 13;
  case GAMGI_IO_HERSHEY_GOTHIC_ITALIAN: return 14;
  case GAMGI_IO_HERSHEY_SYMBOL_SANS: return 15;
  case GAMGI_IO_HERSHEY_SYMBOL_SERIF: return 16;
  case GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD: return 17;
  case GAMGI_IO_DEJAVU_MONO: return 18;
  case GAMGI_IO_DEJAVU_MONO_BOLD: return 19;
  case GAMGI_IO_DEJAVU_MONO_ITALIC: return 20;
  case GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC: return 21;
  }

/*******************
 * impossible case *
 *******************/

return -1;
}

static void static_method_style (char *label, gamgi_window *window)
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
gtk_list_store_set (store, &iter, 0, " Wired ", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, " Solid ", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_method_font (char *label, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *widget;
GtkWidget *hbox;
GtkWidget *combo;
GtkCellRenderer *renderer;

widget = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), label);
hbox = gtk_widget_get_parent (widget);
gtk_widget_destroy (widget);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), label, combo);
gtk_widget_show (combo);

gamgi_gtk_text_config_all (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_method (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkTreeModel *model;
GtkTreeIter old, new;
char *name;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));

/****************************************************
 * GtkComboBox does not support submenus very well: *
 * when users select level 0 items acting as level  *
 * 1 items, just go back to level 0 first item as   *
 * it is a good default and has no level 1 items    *
 ****************************************************/

gtk_tree_model_get_iter_first (model, &old);
gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &new);
if (gtk_tree_model_iter_has_child (model, &new) == TRUE)
  gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &old);

/*****************************************************
 * remove old Min, Max widgets, then build new ones  *
 *                                                   *
 * widgets can be Entry, ComboBox or Combo_Box_Entry *
 *****************************************************/

gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &new);
gtk_tree_model_get (model, &new, 0, &name, -1);

if (strcmp (name, "Pick") == 0 || strcmp (name, "Region") == 0)
  {
  gamgi_gtk_select_method_direct ("widget_min", window);
  gamgi_gtk_select_method_direct ("widget_max", window);
  }
else if (strcmp (name, "Style") == 0)
  {
  static_method_style ("widget_min", window);
  static_method_style ("widget_max", window);
  }

else if (strcmp (name, "Font") == 0)
  {
  static_method_font ("widget_min", window);
  static_method_font ("widget_max", window);
  }
else
  {
  gamgi_gtk_select_method_default ("widget_min", window);
  gamgi_gtk_select_method_default ("widget_max", window);
  }

/***********************************************************
 * GtkTreeView multiple selection is enabled in Pick mode, *
 * in the other modes GtkTreeView selection is disabled    *
 *                                                         *
 *  the multiple, global, Region selection of objects is   *
 *  handled centrally for all classes of visible objects   *
 ***********************************************************/

gamgi_gtk_select_mode (name, window);
gamgi_mesa_select_state (name, window);

free (name);
}

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_CURRENT);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_action");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

/************************************************
 * show current list and reset selection method *
 ************************************************/

static_model (gamgi->texts, window);
static_method (NULL, window);
}

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_atom *atom;
gamgi_dlist *dlist, *dlist_new;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char name_min[GAMGI_ENGINE_TOKEN] = "";
char name_max[GAMGI_ENGINE_TOKEN] = "";
char swap[GAMGI_ENGINE_TOKEN] = "";
const char *name;
gamgi_bool condition;

/********************
 *  * get range limits *
 *   ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
name_min, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return NULL;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "widget_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_alpha_scan (name,
name_max, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid range limits", window);
  return NULL;
  }

if (*name_min != '\0' && *name_max != '\0' && strcmp (name_min, name_max) > 0)
  {
  gamgi_gtk_dialog_message_create ("Warning", "Swapped range limits", window);

  strcpy (swap, name_min);
  strcpy (name_min, name_max);
  strcpy (name_max, swap);
  }

/******************************
 *  * get list of selected atoms *
 *   ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  condition = TRUE;
  if (*name_min != '\0' && strcmp (atom->object.name, name_min) < 0)
    condition = FALSE;
  if (*name_max != '\0' && strcmp (atom->object.name, name_max) > 0)
    condition = FALSE;

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = atom;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window)
{
gamgi_atom *atom;
gamgi_object *parent;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
int min, max;

/********************
 *  * get range limits *
 *   ********************/

if (gamgi_gtk_select_entry_int (1, INT_MAX,
&min, &max, window) == FALSE) return NULL;

/******************************
 *  * get list of selected atoms *
 *   ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  parent = atom->object.object;
  condition = (parent->class == class &&
  parent->number >= min && parent->number <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = atom;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_contents (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
char min[GAMGI_ENGINE_TOKEN];
char max[GAMGI_ENGINE_TOKEN];
char *name;
int length_max, length_string;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_alpha (GAMGI_IO_FONT_ASCII,
GAMGI_ENGINE_TOKEN, min, max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  
  /********************************************************* 
   *  min must be at the beginning, max must be at the end *
   *********************************************************/

  condition = TRUE;
  if (strcmp (min, "") != 0)
    {
    name = strstr (text->string, min);
    if (name != text->string) condition = FALSE;
    }
  if (strcmp (max, "") != 0)
    {
    length_max = strlen (max);
    length_string = strlen (text->string);
    if (length_string < length_max) condition = FALSE;
    else
      {
      name = strstr (text->string + length_string - length_max, max);
      if (name == NULL) condition = FALSE;
      }
    }

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->origin[0] >= min && text->origin[0] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 *****************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->origin[1] >= min && text->origin[1] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->origin[2] >= min && text->origin[2] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_e1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double rotate[9], euler[3];
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
  gamgi_math_euler_from_matrix (rotate, euler);
  condition = (euler[0] >= min && euler[0] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_e2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double rotate[9], euler[3];
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
  gamgi_math_euler_from_matrix (rotate, euler);
  condition = (euler[1] >= min && euler[1] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_e3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double rotate[9], euler[3];
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
  gamgi_math_euler_from_matrix (rotate, euler);
  condition = (euler[2] >= min && euler[2] <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_style (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
int min, max;
int style;

/********************
 * get range limits *
 ********************/

gamgi_gtk_select_combo_int (&min, &max, window);

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;

  style = 0;
  if (text->style == GAMGI_MESA_SOLID) style = 1;
  condition = (style >= min && style <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_font (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
int min, max;
int font;

/********************
 * get range limits *
 ********************/

gamgi_gtk_select_combo_int (&min, &max, window);

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  font = static_font (text->font);
  condition = (font >= min && font <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_red (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
float min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_float (-FLT_MAX,
FLT_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->red >= min && text->red <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_green (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
float min, max;

/********************
 * get range limits * 
 ********************/

if (gamgi_gtk_select_entry_float (-FLT_MAX,
FLT_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->green >= min && text->green <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_blue (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_text *text;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
float min, max;

/********************
 * get range limits * 
 ********************/

if (gamgi_gtk_select_entry_float (-FLT_MAX,
FLT_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  condition = (text->blue >= min && text->blue <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = text;
    }
  }

return dlist_new;
}

static gamgi_dlist *static_ok_scale (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window)
{
gamgi_object *object;
gamgi_dlist *dlist, *dlist_new;
gamgi_bool condition;
double min, max;

/********************
 * get range limits *
 ********************/

if (gamgi_gtk_select_entry_double (-DBL_MAX,
DBL_MAX, &min, &max, window) == FALSE) return NULL;

/******************************
 * get list of selected texts *
 ******************************/

dlist_new = NULL;
for (dlist = dlist_old; dlist != NULL; dlist = dlist->next)
  {
  object = GAMGI_CAST_OBJECT dlist->data;
  condition = (object->scale >= min && object->scale <= max);

  if (condition != action)
    {
    dlist_new = gamgi_engine_dlist_add_start (dlist_new);
    dlist_new->data = object;
    }
  }

return dlist_new;
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_dlist *dlist_new, *dlist_old;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkTreeModel *model;
GtkTreeIter iter;
char *name;
int scope, action;

/*******************************************
 * action: FALSE = include, TRUE = exclude *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
scope = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_action");
action = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
gtk_combo_box_get_active_iter (GTK_COMBO_BOX (combo), &iter);
gtk_tree_model_get (model, &iter, 0, &name, -1);

/**************************************************************
 * build list of candidate objects, according to chosen scope *
 *                                                            *
 * build list of selected objects, according to chosen method *
 **************************************************************/

dlist_old = static_dlist (scope, window);

/********************************************************
 * read Min, Max widgets, then apply selection criteria *
 ********************************************************/

if (strcmp (name, "Pick") == 0)
  dlist_new = gamgi_gtk_select_ok_pick (dlist_old, action, window);

else if (strcmp (name, "Region") == 0)
  dlist_new = gamgi_gtk_select_ok_region (dlist_old, action, window);

else if (strcmp (name, "Name") == 0)
  dlist_new = static_ok_name (dlist_old, action, window);

else if (strcmp (name, "Layer") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_LAYER, window);
else if (strcmp (name, "Assembly") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_ASSEMBLY, window);
else if (strcmp (name, "Cell") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_CELL, window);
else if (strcmp (name, "Cluster") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_CLUSTER, window);
else if (strcmp (name, "Molecule") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_MOLECULE, window);
else if (strcmp (name, "Group") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_GROUP, window);
else if (strcmp (name, "Plane") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_PLANE, window);
else if (strcmp (name, "Direction") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_DIRECTION, window);
else if (strcmp (name, "Atom") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_ATOM, window);
else if (strcmp (name, "Bond") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_BOND, window);
else if (strcmp (name, "Orbital") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_ORBITAL, window);
else if (strcmp (name, "Text") == 0)
  dlist_new = static_ok_parent (dlist_old,
  action, GAMGI_ENGINE_TEXT, window);

else if (strcmp (name, "Contents") == 0)
  dlist_new = static_ok_contents (dlist_old, action, window);

else if (strcmp (name, "X") == 0)
  dlist_new = static_ok_x (dlist_old, action, window);
else if (strcmp (name, "Y") == 0)
  dlist_new = static_ok_y (dlist_old, action, window);
else if (strcmp (name, "Z") == 0)
  dlist_new = static_ok_z (dlist_old, action, window);
else if (strcmp (name, "E1") == 0)
  dlist_new = static_ok_e1 (dlist_old, action, window);
else if (strcmp (name, "E2") == 0)
  dlist_new = static_ok_e2 (dlist_old, action, window);
else if (strcmp (name, "E3") == 0)
  dlist_new = static_ok_e3 (dlist_old, action, window);

else if (strcmp (name, "Style") == 0)
  dlist_new = static_ok_style (dlist_old, action, window);
else if (strcmp (name, "Font") == 0)
  dlist_new = static_ok_font (dlist_old, action, window);
else if (strcmp (name, "R") == 0)
  dlist_new = static_ok_red (dlist_old, action, window);
else if (strcmp (name, "G") == 0)
  dlist_new = static_ok_green (dlist_old, action, window);
else if (strcmp (name, "B") == 0)
  dlist_new = static_ok_blue (dlist_old, action, window);
else if (strcmp (name, "Scale") == 0)
  dlist_new = static_ok_scale (dlist_old, action, window);

else dlist_new = NULL;
free (name);

/****************************************
 * invert list: first node becomes last *
 ****************************************/

dlist_new = gamgi_engine_dlist_invert (dlist_new);

/********************************************
 * remove dlist_old, except for list Before *
 * as this is deleted after, and list       *
 * Current as this will become list Before  *
 *                                          *
 * remove list Before, the list Current     *
 * becomes the new list Before              *
 *                                          *
 * The selected list replaces list Current  *
 * when Scope is Current and merges with    *
 * list Current when Scope is different     *
 ********************************************/

if (dlist_old != static_before && dlist_old != gamgi->texts)
  gamgi_engine_dlist_remove_all (dlist_old);

gamgi_engine_dlist_remove_all (static_before);
static_before = gamgi->texts;

if (scope == STATIC_CURRENT)
  gamgi->texts = dlist_new;
else
  gamgi_gtk_select_merge (&gamgi->texts, dlist_new);

/*****************************************
 * Change color for Text label to signal *
 * that a list of selected texts exists  *
 *                                       *
 * reset dialog: the Scope list is set   *
 * to Current and the model is updated   *
 *****************************************/

gamgi_global_selection_update_windows (&gamgi->texts, "button_text");

static_reset (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox;
GtkWidget *combo;
GtkRequisition size;

/***************************************************
 * Set vbox_top current size as its maximum size:  *
 * this is needed because different methods need   *
 * different widths, changing the dialog width.    *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_top");
gtk_widget_size_request (GTK_WIDGET (vbox), &size);
gtk_widget_set_size_request (vbox, size.width, size.height);

/**************************************************
 * everytime the dialog is created: initialize    *
 * list Before and unlink objects in list Current *
 **************************************************/

static_before = NULL;
gamgi_global_selection_unlink (gamgi->texts);

/************************************
 * initialize Scope, Action, Method *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_scope");
if (gamgi->texts == NULL)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_LAYER);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_CURRENT);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_action");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window_dialog = gamgi->window_dialog;
gamgi_object *object_tree;
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *treeview;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;

/*********************
 * get treeview data *
 *********************/

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));

/***************************************************************
 * a valid object has been found, check if the object is in    *
 * the GtkTreeView Current list: 1) yes, set the object as     *
 * selected in the list; 2) no, ignore the press button event. *
 ***************************************************************/

if (gtk_tree_model_get_iter_first (model, &iter) == FALSE) return;
do
  {
  gtk_tree_model_get (model, &iter, 1, &object_tree, -1);
  if (object == object_tree)
    { gtk_tree_selection_select_iter (selection, &iter); return; }
  }
  while (gtk_tree_model_iter_next (model, &iter) == TRUE);

}

void gamgi_gtk_text_select_press (gamgi_window *window_mouse,    
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
/**************************************************
 * Ctrl key has not been pressed: this is a local *
 * object selection, to set new object with focus *
 **************************************************/

if ((event->state & GDK_CONTROL_MASK) == 0)
  {
  if (window_dialog == window_mouse)
    gamgi_mesa_select_object (window_mouse,
    x, y, GAMGI_ENGINE_TEXT, FALSE, NULL);
  }

/*********************************************************
 * Ctrl key has been pressed: this is a global object    *
 * selection, to find an object belonging to GtkTreeView *
 * Current list, to mark the object as selected          *
 *********************************************************/

else
  gamgi_mesa_select_object (window_mouse,
  x, y, GAMGI_ENGINE_TEXT, TRUE, static_press);
}

void gamgi_gtk_text_select (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *scrolled_window;
GtkWidget *frame;
GtkWidget *treeview;
GtkWidget *combo;
GtkWidget *button;
GtkWidget *label;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_top, *vbox_top_top;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *list_store;
GtkTreeStore *tree_store;
GtkTreeIter iter, child;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Text Select", window);
window->action = GAMGI_GTK_TEXT_SELECT;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "destroy", G_CALLBACK (static_destroy), window);

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

/****************
 * Objects page *
 ****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" List ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_page), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*************
 * left side *
 *************/

/*******************
 * Object TreeView *
 *******************/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (hbox_left), frame, FALSE, FALSE, 0);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_container_add (GTK_CONTAINER (frame), scrolled_window);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
gtk_widget_set_size_request (treeview,
GAMGI_GTK_SELECT_LIST_WIDTH, GAMGI_GTK_SELECT_LIST_HEIGHT);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);
g_object_set_data (G_OBJECT (dialog), "treeview", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("List", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);

/**************
 * right side *
 **************/

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_top", vbox_top);
gtk_widget_show (vbox_top);

/*********
 * Scope *
 *********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Scope");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_center), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_scope), window);
g_object_set_data (G_OBJECT (dialog), "combo_scope", combo);
gtk_widget_show (combo);

list_store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Gamgi", -1);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Window", -1);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Layer", -1);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Before", -1);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Current", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (list_store));
g_object_unref (list_store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * Action *
 **********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Action");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_center), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_action", combo);
gtk_widget_show (combo);

list_store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Include", -1);
gtk_list_store_append (list_store, &iter);
gtk_list_store_set (list_store, &iter, 0, "Exclude", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (list_store));
g_object_unref (list_store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * Method *
 **********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Method");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_center), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_method), window);
g_object_set_data (G_OBJECT (dialog), "combo_method", combo);
gtk_widget_show (combo);

tree_store = gtk_tree_store_new (1, G_TYPE_STRING);
gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Pick", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Region", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Name", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Parent", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Layer", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Assembly", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Cell", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Cluster", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Molecule", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Group", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Plane", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Direction", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Atom", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Bond", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Orbital", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Text", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Contents", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "Position", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "X", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Y", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Z", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "E1", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "E2", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "E3", -1);

gtk_tree_store_append (tree_store, &iter, NULL);
gtk_tree_store_set (tree_store, &iter, 0, "View", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Style", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Font", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "R", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "G", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "B", -1);
gtk_tree_store_append (tree_store, &child, &iter);
gtk_tree_store_set (tree_store, &child, 0, "Scale", -1);

gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (tree_store));
g_object_unref (tree_store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);
g_object_set (renderer, "width-chars", GAMGI_GTK_CHAR_MENU, NULL);

gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_titles_scan, NULL, NULL);

/***************************
 * 4th, 5th rows: Min, Max *
 ***************************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

/*******
 * Min *
 *******/

label = gtk_label_new ("Min");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********************************************
 * this widget will be removed in static_init *
 **********************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "widget_min", combo);

/*******
 * Max *
 *******/

label = gtk_label_new ("Max");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********************************************
 * this widget will be removed in static_init *
 **********************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "widget_max", combo);

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
