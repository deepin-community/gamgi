/***********************************************
 *
 * $GAMGI/src/gtk/layer/gamgi_gtk_layer_link.c
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
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

static struct {
gamgi_bool local;
gamgi_object *object;
gamgi_bool above, child;
double origin[3]; } cache;

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
int row;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/********************************************
 * when entries are empty, look for a layer *
 ********************************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_LAYER;

/********************
 * get object class *
 ********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
   if (row == 0) return GAMGI_ENGINE_WINDOW;
  }
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == 0) return GAMGI_ENGINE_LIGHT;
  if (row == 1) return GAMGI_ENGINE_ASSEMBLY;
  if (row == 2) return GAMGI_ENGINE_CELL;
  if (row == 3) return GAMGI_ENGINE_CLUSTER;
  if (row == 4) return GAMGI_ENGINE_MOLECULE;
  if (row == 5) return GAMGI_ENGINE_GROUP;
  if (row == 6) return GAMGI_ENGINE_PLANE;
  if (row == 7) return GAMGI_ENGINE_DIRECTION;
  if (row == 8) return GAMGI_ENGINE_ATOM;
  if (row == 9) return GAMGI_ENGINE_ORBITAL;
  if (row == 10) return GAMGI_ENGINE_TEXT;
  }

return FALSE;
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

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_child");
gtk_button_clicked (GTK_BUTTON (button));
}

static void static_hierarchy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo_above, *combo_below;
GtkWidget *button;

combo_above = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
combo_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /************************************************
   * the default for the combo box above is Layer *
   ************************************************/

  gtk_widget_show (combo_above);
  gtk_widget_hide (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_above), 0);
  }
else
  {
  /***********************************************
   * the default for the combo box below is Atom *
   ***********************************************/

  gtk_widget_hide (combo_above);
  gtk_widget_show (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_below), 8);
  }

static_reference (NULL, window);
}

/******************* internal function ****************
 *                                                    *
 *                      STATIC_RESET                  *
 *                                                    *
 * Reset the layer link dialog, cleaning all entries. *
 *                                                    *
 ******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/********************************
 * reset layer and object names *
 ********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*********************
 * start origin page *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***************************
 * reset hierarchy, origin *
 ***************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_below");
gtk_button_clicked (GTK_BUTTON (button));
static_hierarchy (NULL, window);
}

static void static_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;       
GtkWidget *button;
GtkWidget *entry;                          

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  { static_reset (window); return; }

/***************************
 * keep cell, reset object *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/**************** internal function ************
 *                                             *
 *                   STATIC_LIST               *
 *                                             *
 * Makes the layer entry sensitive when        *
 * the local button is pressed and insensitive *
 * when the global button is pressed.          *
 *                                             *
 ***********************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *hbox;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_layer");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*****************************
   * Choose the layer directly *
   *****************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  static_default_layer (window);
  }
else
  {
  /*****************************************
   * Choose the layers previously selected *
   *****************************************/

  static_reset (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  }

static_reference (NULL, window);
}

static void static_origin (gamgi_object *child, gamgi_object *parent)
{
gamgi_text *text;
gamgi_orbital *orbital;
gamgi_atom *atom;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;
double origin[3];

/*****************************************************
 * translation is always applied to the object below *
 *****************************************************/

if (cache.child == TRUE)
  gamgi_mesa_translate_object (child, cache.origin);
else
  {
  /**********************************************************************
   * get reference origin from parent, apply translated origin to child *
   **********************************************************************/

  switch (parent->class)
    {
    case GAMGI_ENGINE_LAYER:
    layer = GAMGI_CAST_LAYER parent;
    gamgi_math_vector_copy (layer->center, origin);
    break;
    }

  /**************************************
   * 1) get new origin for child        *
   * 2) get translate vector for child  *
   * 3) apply translate vector to child *
   **************************************/

  gamgi_math_vector_add (origin, cache.origin, origin);

  switch (child->class)
    {
    case GAMGI_ENGINE_TEXT:
    text = GAMGI_CAST_TEXT child;
    gamgi_math_vector_sub (origin, text->origin, origin);
    break;

    case GAMGI_ENGINE_ORBITAL:
    orbital = GAMGI_CAST_ORBITAL child;
    gamgi_math_vector_sub (origin, orbital->origin, origin);
    break;

    case GAMGI_ENGINE_ATOM:
    atom = GAMGI_CAST_ATOM child;
    gamgi_math_vector_sub (origin, atom->position, origin);
    break;

    case GAMGI_ENGINE_DIRECTION:
    direction = GAMGI_CAST_DIRECTION parent;
    gamgi_math_vector_sub (origin, direction->origin, origin);
    break;
  
    case GAMGI_ENGINE_PLANE:
    plane = GAMGI_CAST_PLANE parent;
    gamgi_math_vector_sub (origin, plane->origin, origin);
    break;

    case GAMGI_ENGINE_GROUP:
    group = GAMGI_CAST_GROUP parent;
    gamgi_math_vector_sub (origin, group->origin, origin);
    break;

    case GAMGI_ENGINE_MOLECULE:
    molecule = GAMGI_CAST_MOLECULE parent;
    gamgi_math_vector_sub (origin, molecule->origin, origin);
    break;

    case GAMGI_ENGINE_CLUSTER:
    cluster = GAMGI_CAST_CLUSTER parent;
    gamgi_math_vector_sub (origin, cluster->origin, origin); 
    break;

    case GAMGI_ENGINE_CELL:
    cell = GAMGI_CAST_CELL parent;
    gamgi_math_vector_sub (origin, cell->origin, origin);
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    assembly = GAMGI_CAST_ASSEMBLY parent;
    gamgi_math_vector_sub (origin, assembly->origin, origin);
    break;

    case GAMGI_ENGINE_LAYER:
    layer = GAMGI_CAST_LAYER parent;
    gamgi_math_vector_sub (origin, layer->center, origin);
    break;
    }

  gamgi_mesa_translate_object (child, origin); 
  }
 
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
const char *name;

/*********************
 *  * get global object *
 *   *********************/

cache.object = gamgi_gtk_object_name_number (dialog,
"entry_object", static_class (window), NULL);
if (cache.object == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Object", window);
  return FALSE;
  }

/****************************************
 *  * get reference (child|parent) and     *
 *   * position (absolute|relative) options *
 *    ****************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.above = TRUE;
else
  cache.above = FALSE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_child");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.child = TRUE;
else
  cache.child = FALSE;

/*********************
 *  * x,y,z coordinates *
 *   *********************/

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
gamgi_window *window_parent, *window_child;
gamgi_layer *layer_parent, *layer_child;

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

if (cache.above == TRUE)
  {
  /****************************************
   * Link layer or layers to window above *
   ****************************************/

  layer_child = GAMGI_CAST_LAYER child;
  window_child = GAMGI_CAST_WINDOW child->object;
  window_parent = GAMGI_CAST_WINDOW parent;

  /******************************************
   * get new current layer for child window *
   ******************************************/

  if (layer_child == window_child->layer)
    gamgi_gtk_object_layer_current (layer_child, window_child);

  gamgi_engine_unlink_object (child);

  if (cache.local == FALSE)
    {
    window_parent->layer = layer_child;
    window_parent->focus = child;
    }
  }

if (cache.above == FALSE)
  {
  /****************************************
   * Link layer or layers to object below *
   ****************************************/

  layer_parent = GAMGI_CAST_LAYER parent;
  layer_child = gamgi_engine_find_layer (child);

  if (cache.local == TRUE)
    {
    /**********************************************
     * save layers before linking child to parent *
     **********************************************/

    gamgi_gtk_history_update (layer_parent, layer_parent->undo, GAMGI_ENGINE_UNDO);
    if (layer_child != layer_parent)
      gamgi_gtk_history_update (layer_child, layer_child->undo, GAMGI_ENGINE_UNDO);

    /************************************
     * remove all bonds that are not    *
     * entirely inside the child object *
     ************************************/

    if (layer_child != layer_parent)
      gamgi_chem_bond_half_remove (child);

    gamgi_engine_unlink_object (child);
    }
  else
    {
    /******************************************************
     * List mode: copy template child to new child object *
     * (bonds to objects outside child are not copied)    *
     ******************************************************/

    child = gamgi_engine_copy_object (child);
    }
  }

gamgi_engine_link_object_object (child, parent);

/**************************
 * translate child object *
 **************************/

static_origin (child, parent);

return TRUE;
}

static void static_ok_global (gamgi_window *window_dialog)
{
gamgi_window *window_parent;
gamgi_layer *layer_child;
gamgi_object *child, *parent;
gamgi_dlist *dlist;
gamgi_slist *start;

cache.local = FALSE;

/******************
 * get layer list *
 ******************/

if (gamgi->layers == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of layer objects", window_dialog); return; }

if (static_grab (window_dialog) == FALSE) return;

/************************************************
 * For all windows: save layers as undo layers  *
 * and set current layer as new current object  *
 *                                              *
 * Link listed objects and show error message   *
 * with objects that could not be linked        *
 *                                              *
 * For all windows: redraw gl_area image        *
 ************************************************/

gamgi_global_selection_undo ();

if (cache.above == TRUE)
  {
  /*******************************************
   * link above: link each listed layer      *
   * (child) to the selected window (parent) *
   *******************************************/

  parent = cache.object;
  window_parent = GAMGI_CAST_WINDOW parent;
  layer_child = window_parent->layer;

  start = NULL;
  for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
    {
    child = GAMGI_CAST_OBJECT dlist->data;
    layer_child = GAMGI_CAST_LAYER child;

    if (static_apply (child, parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }

  /**************************************
   * raise to top window and last layer *
   **************************************/

  gamgi_gtk_object_raise (layer_child, window_parent);
  } 
else
  {
  /**************************************************
   * link below: link each listed layer (parent) to *
   * a child object copied from the selected object *
   **************************************************/

  child = cache.object;

  start = NULL;
  for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
    {
    parent = GAMGI_CAST_OBJECT dlist->data;
    if (static_apply (child, parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }

  }

gamgi_io_error_list (start, window_dialog);

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
gamgi_window *window_child, *window_parent;
gamgi_layer *layer, *layer_parent;
gamgi_object *child, *parent;
GtkWidget *dialog = window_dialog->dialog0;

cache.local = TRUE;

/*******************************
 * pick layer, get dialog data *
 *******************************/

layer = GAMGI_CAST_LAYER gamgi_gtk_object_name_number (dialog,
"entry_layer", GAMGI_ENGINE_LAYER, window_dialog);
if (layer == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid layer object", window_dialog);
  return;
  }

if (static_grab (window_dialog) == FALSE) return;

if (cache.above == TRUE)
  {
  /***********************************************************************
   * link above: link selected layer (child) to selected window (parent) *
   ***********************************************************************/

  child = GAMGI_CAST_OBJECT layer;
  parent = cache.object;

  window_child = GAMGI_CAST_WINDOW child->object;
  window_parent = GAMGI_CAST_WINDOW parent;
  layer_parent = GAMGI_CAST_LAYER child;
  }
else
  {
  /***********************************************************************
   * link below: link selected layer (parent) to selected object (child) *
   ***********************************************************************/

  child = cache.object;
  parent = GAMGI_CAST_OBJECT layer;

  window_child =  gamgi_engine_find_window (child);
  window_parent =  GAMGI_CAST_WINDOW parent->object;
  layer_parent = layer;

  /***********************************
   * If child is the current object, *
   * then replace by current layer   *
   ***********************************/

  if (child == window_child->focus)
    gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT
    window_child->layer, window_child);
  }

static_apply (child, parent, window_dialog);

/********************************************************
 * raise to top window and layer where parent object is *
 ********************************************************/

gamgi_gtk_object_raise (layer_parent, window_parent);

/***************************************************
 * redraw drawing area for parent and child window *
 ***************************************************/

gtk_widget_queue_draw (window_parent->area);
if (window_child != window_parent)
  gtk_widget_queue_draw (window_child->area);

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
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo_above, *combo_below, *combo;
GtkRequisition size;
char string[2 * GAMGI_ENGINE_TOKEN];
int width, height;

/****************************
 * start with current layer *
 ****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
sprintf (string, "%s %d", window->layer->object.name, window->layer->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/***********************************************************
 * combo boxes above and below need different sizes: use   *
 * the largest size for both, so the size remains constant *
 ***********************************************************/

combo_above = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
gtk_widget_size_request (GTK_WIDGET (combo_above), &size);
width = size.width; height = size.height;

combo_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");
gtk_widget_size_request (GTK_WIDGET (combo_below), &size);
if (size.width > width) width = size.width;
if (size.height > height) height = size.height;

gtk_widget_set_size_request (combo_above, width, height);
gtk_widget_set_size_request (combo_below, width, height);

/*************************************************
 * show combo box BEFORE measuring notebook size *
 *************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_below");
gtk_button_clicked (GTK_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

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

static_reset (window);
static_default_layer (window);
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
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
gamgi_io_token_check (name) == TRUE)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/***************** external function ****************
 *                                                  *
 *             GAMGI_GTK_LAYER_LINK_PRESS           *
 *                                                  *
 ****************************************************/

void gamgi_gtk_layer_link_press (gamgi_window *window_mouse,
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
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
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

/************************************************************************
 * window_dialog is the (local) window where the dialog window was open *
 * window_mouse is the (global) window where the user presses the mouse *
 ************************************************************************/

if (global == FALSE && window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y, 
static_class (window_dialog), global, static_press);
}

/***************** external function ***************
 *                                                 *
 *                GAMGI_GTK_LAYER_LINK             *
 *                                                 *
 *  Creates the dialog window used to link layers. *
 *                                                 *
 ***************************************************/

void gamgi_gtk_layer_link (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Layer Link", window);
window->action = GAMGI_GTK_LAYER_LINK;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Layer objects *
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
g_object_set_data (G_OBJECT (dialog), "hbox_layer", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Layer");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_layer", entry);
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

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

/*************
 * Hierarchy *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Hierarchy");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Above");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0); 
g_signal_connect (button, "clicked", G_CALLBACK (static_hierarchy), window);
g_object_set_data (G_OBJECT (dialog), "button_above", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Below");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_hierarchy), window);
g_object_set_data (G_OBJECT (dialog), "button_below", button);
gtk_widget_show (button);

/**********
 * object *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/***********************************
 * hide combo box for objects above *
 ************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_above", combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Window", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/************************************
 * hide combo box for objects below *
 ************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_below", combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Light", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Assembly", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Cell", -1);
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
gtk_list_store_set (store, &iter, 0, "Orbital", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Text", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

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
g_object_set_data (G_OBJECT (dialog), "combo_method", combo);
gtk_widget_show (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Object", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

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

/****************
 * Local,Parent *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Reference");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Child");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_child", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Parent");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_parent", button);
gtk_widget_show (button);

/***********
 * 2nd row *
 ***********/

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
