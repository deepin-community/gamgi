/*******************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_link.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_copy.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_math_vector.h"
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
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button;
const char *name;
int row;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/************************************************
 * when entries are empty, look for a direction *
 ************************************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_DIRECTION;

/********************
 * get object class *
 ********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == 0) return GAMGI_ENGINE_LAYER;
  if (row == 1) return GAMGI_ENGINE_ASSEMBLY;
  if (row == 2) return GAMGI_ENGINE_CELL;
  if (row == 3) return GAMGI_ENGINE_CLUSTER;
  if (row == 4) return GAMGI_ENGINE_MOLECULE;
  if (row == 5) return GAMGI_ENGINE_GROUP;
  if (row == 6) return GAMGI_ENGINE_PLANE;
  }
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == 0) return GAMGI_ENGINE_ATOM;
  if (row == 1) return GAMGI_ENGINE_ORBITAL;
  if (row == 2) return GAMGI_ENGINE_TEXT;
  }

return FALSE;
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button, *button_list, *button_below;

button_list = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
button_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_below");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_list)) == TRUE
&& gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_below)) == TRUE)
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_parent");
else
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
  /***************************************************
   * the default for the object class above is Layer *
   ***************************************************/

  gtk_widget_show (combo_above);
  gtk_widget_hide (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_above), 0);
  }
else
  {
  /**************************************************
   * the default for the object class below is Atom *
   **************************************************/

  gtk_widget_hide (combo_above);
  gtk_widget_show (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_below), 0);
  }

static_reference (NULL, window);
}

/********************* internal function ******************
 *                                                        *
 *                        STATIC_RESET                    *
 *                                                        *
 * Reset the direction link dialog, cleaning all entries. *
 *                                                        *
 **********************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/************************************
 * reset direction and object names *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction");
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

/********************************
 * keep direction, reset object *
 ********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/************** internal function ***********
 *                                          * 
 *                 STATIC_LIST              *
 *                                          *
 * Makes the direction entry sensitive when *
 * the global button is up and insensitive  *
 * when the global button is down.          *
 *                                          *
 ********************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *hbox;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_direction");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * Choose the direction directly *
   *********************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  }
else
  {
  /*********************************************
   * Choose the directions previously selected *
   *********************************************/

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
    case GAMGI_ENGINE_DIRECTION:
    direction = GAMGI_CAST_DIRECTION parent;
    gamgi_math_vector_copy (direction->origin, origin);
    break;
  
    case GAMGI_ENGINE_PLANE:
    plane = GAMGI_CAST_PLANE parent;
    gamgi_math_vector_copy (plane->origin, origin);
    break;

    case GAMGI_ENGINE_GROUP:
    group = GAMGI_CAST_GROUP parent;
    gamgi_math_vector_copy (group->origin, origin);
    break;

    case GAMGI_ENGINE_MOLECULE:
    molecule = GAMGI_CAST_MOLECULE parent;
    gamgi_math_vector_copy (molecule->origin, origin);
    break;

    case GAMGI_ENGINE_CLUSTER:
    cluster = GAMGI_CAST_CLUSTER parent;
    gamgi_math_vector_copy (cluster->origin, origin); 
    break;

    case GAMGI_ENGINE_CELL:
    cell = GAMGI_CAST_CELL parent;
    gamgi_math_vector_copy (cell->origin, origin);
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    assembly = GAMGI_CAST_ASSEMBLY parent;
    gamgi_math_vector_copy (assembly->origin, origin);
    break;

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
    direction = GAMGI_CAST_DIRECTION child;
    gamgi_math_vector_sub (origin, direction->origin, origin);
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
 * get global object *
 *********************/

cache.object = gamgi_gtk_object_name_number (dialog,
"entry_object", static_class (window), NULL);
if (cache.object == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid object", window);
  return FALSE;
  }

/****************************************
 * get reference (child|parent) and     *
 * position (absolute|relative) options *
 ****************************************/

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

static gamgi_bool static_apply (gamgi_object *child, gamgi_object *parent,
gamgi_layer *layer_child, gamgi_layer *layer_parent, gamgi_window *window)
{
if (child->class == GAMGI_ENGINE_DIRECTION &&
(GAMGI_CAST_DIRECTION child)->model != GAMGI_PHYS_LINE)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Child must be linked to parent", window);
  return FALSE;
  }

/********************************************
 * check if child already belongs to parent *
 ********************************************/

if (child->object == parent)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Child already linked to parent", window);
  return FALSE;
  }

/**********************************************
 * save layers before linking child to parent *
 **********************************************/

if (cache.local == TRUE)
  {
  gamgi_gtk_history_update (layer_parent, layer_parent->undo, GAMGI_ENGINE_UNDO);
  if (layer_child != layer_parent)
    gamgi_gtk_history_update (layer_child, layer_child->undo, GAMGI_ENGINE_UNDO);
  }

if (cache.local == FALSE && cache.above == FALSE)
  {
  /*******************************************
   * as child object is a new instance,      *
   * there is no need to check for recursive *
   * objects or to check for half bonds      *
   *******************************************/

  child = gamgi_engine_copy_object (child);
  }
else
  {
  /************************************
   * remove all bonds that are not    *
   * entirely inside the child object *
   ************************************/

  if (layer_parent != layer_child) gamgi_chem_bond_half_remove (child);

  gamgi_engine_unlink_object (child);
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
gamgi_layer *layer_child, *layer_parent;
gamgi_object *child, *parent;
gamgi_dlist *dlist;
gamgi_slist *start;

cache.local = FALSE;

/**********************
 * get direction list *
 *********************/

if (gamgi->directions == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of direction objects", window_dialog); return; }

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
gamgi_global_selection_focus ();

if (cache.above == TRUE)
  {
  /*******************************************
   * link above: link each listed direction  *
   * (child) to the selected object (parent) *
   *******************************************/

  parent = cache.object;
  layer_parent = gamgi_engine_find_layer (parent);

  start = NULL;
  for (dlist = gamgi->directions; dlist != NULL; dlist = dlist->next)
    {
    child = GAMGI_CAST_OBJECT dlist->data;
    layer_child = gamgi_engine_find_layer (child);

    if (static_apply (child, parent,
    layer_child, layer_parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }

  /********************************************************
   * raise to top window and layer where parent object is *
   ********************************************************/

  window_parent = GAMGI_CAST_WINDOW layer_parent->object.object;
  gamgi_gtk_object_raise (layer_parent, window_parent);
  } 
else
  {
  /******************************************************
   * link below: link each listed direction (parent) to *
   * a child object copied from the selected object     *
   ******************************************************/

  child = cache.object;

  start = NULL;
  for (dlist = gamgi->directions; dlist != NULL; dlist = dlist->next)
    {
    parent = GAMGI_CAST_OBJECT dlist->data;
    layer_parent = gamgi_engine_find_layer (parent);

    if (static_apply (child, parent,
    layer_parent, layer_parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }
  }

gamgi_io_error_list (start, window_dialog);

gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window_dialog);
}

static void static_ok_local (gamgi_window *window_dialog)
{
gamgi_window *window_child, *window_parent;
gamgi_layer *layer_child, *layer_parent;
gamgi_direction *direction;
gamgi_object *child, *parent;
GtkWidget *dialog = window_dialog->dialog0;

cache.local = TRUE;

/***********************************
 * pick direction, get dialog data *
 ***********************************/

direction = GAMGI_CAST_DIRECTION gamgi_gtk_object_name_number (dialog,
"entry_direction", GAMGI_ENGINE_DIRECTION, window_dialog);
if (direction == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid direction object", window_dialog);
  return;
  }

if (static_grab (window_dialog) == FALSE) return;

/***************************************************************************
 * link above: link selected direction (child) to selected object (parent) *
 * link below: link selected direction (parent) to selected object (child) *
 ***************************************************************************/

if (cache.above == TRUE)
  { child = GAMGI_CAST_OBJECT direction; parent = cache.object; }
else
  { child = cache.object; parent = GAMGI_CAST_OBJECT direction; }

layer_parent = gamgi_engine_find_layer (parent);
window_parent =  GAMGI_CAST_WINDOW layer_parent->object.object;
layer_child = gamgi_engine_find_layer (child);
window_child =  GAMGI_CAST_WINDOW layer_child->object.object;

/***********************************
 * If child is the current object, *
 * then replace by current layer   *
 ***********************************/

if (child == window_child->focus)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT
  window_child->layer, window_child);

static_apply (child, parent, layer_child, layer_parent, window_dialog);

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
GtkWidget *button;
GtkWidget *combo_above, *combo_below, *combo;
GtkRequisition size;
int width, height;

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
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
gamgi_io_token_check (name) == TRUE)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/**************** external function ****************
 *                                                 *
 *         GAMGI_GTK_DIRECTION_LINK_PRESS          *
 *                                                 *
 ***************************************************/

void gamgi_gtk_direction_link_press (gamgi_window *window_mouse,
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
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction");
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

/********************* external function ****************
 *                                                      *
 *                  GAMGI_GTK_DIRECTION_LINK            *
 *                                                      *
 * Creates the dialog window used to create directions. *
 *                                                      *
 ********************************************************/

void gamgi_gtk_direction_link (GtkWidget *widget, void *data)
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

dialog = gamgi_gtk_dialog_task0_create ("Direction Link", window);
window->action = GAMGI_GTK_DIRECTION_LINK;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*********************
 * Direction objects *
 *********************/

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
g_object_set_data (G_OBJECT (dialog), "hbox_direction", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Direction");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_direction", entry);
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

/************************************
 * hide combo box for objects above *
 ************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_above", combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Layer", -1);
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

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Orbital", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Text", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Object", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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
