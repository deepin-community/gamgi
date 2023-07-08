/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_object.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_start.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_io_token.h"
#include "gamgi_global_selection.h"

gamgi_object *static_address (gamgi_enum class, 
gamgi_window *window, long number)
{
gamgi_layer *layer;
gamgi_light *light;
gamgi_assembly *assembly;
gamgi_arrow *arrow;
gamgi_shape *shape;
gamgi_cell *cell;
gamgi_cluster *cluster;
gamgi_molecule *molecule;
gamgi_group *group;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_orbital *orbital;
gamgi_text *text;
gamgi_object *object;

switch (class)
  {
  case GAMGI_ENGINE_WINDOW:
  window = gamgi_engine_find_window_gamgi (number);
  if (window != NULL) return GAMGI_CAST_OBJECT window;
  break;
  
  case GAMGI_ENGINE_LAYER:
  if (window == NULL)
    layer = gamgi_engine_find_layer_gamgi (number);
  else
    layer = gamgi_engine_find_layer_window (number, window);
  if (layer != NULL) return GAMGI_CAST_OBJECT layer;
  break;

  case GAMGI_ENGINE_LIGHT:
  if (window == NULL)
    light = gamgi_engine_find_light_gamgi (number);
  else
    light = gamgi_engine_find_light_layer (number, window->layer);
  if (light != NULL) return object = GAMGI_CAST_OBJECT light;
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  if (window == NULL)
    assembly = gamgi_engine_find_assembly_gamgi (number);
  else
    assembly = gamgi_engine_find_assembly_layer (number, window->layer);
  if (assembly != NULL) return object = GAMGI_CAST_OBJECT assembly;
  break;

  case GAMGI_ENGINE_ARROW:
  if (window == NULL)
    arrow = gamgi_engine_find_arrow_gamgi (number);
  else
    arrow = gamgi_engine_find_arrow_layer (number, window->layer);
  if (arrow != NULL) return object = GAMGI_CAST_OBJECT arrow;
  break;

  case GAMGI_ENGINE_SHAPE:
  if (window == NULL)
    shape = gamgi_engine_find_shape_gamgi (number);
  else
    shape = gamgi_engine_find_shape_layer (number, window->layer);
  if (shape != NULL) return object = GAMGI_CAST_OBJECT shape;
  break;

  case GAMGI_ENGINE_CELL:
  if (window == NULL)
    cell = gamgi_engine_find_cell_gamgi (number);
  else
    cell = gamgi_engine_find_cell_layer (number, window->layer);
  if (cell != NULL) return object = GAMGI_CAST_OBJECT cell;
  break;

  case GAMGI_ENGINE_CLUSTER:
  if (window == NULL)
    cluster = gamgi_engine_find_cluster_gamgi (number);
  else
    cluster = gamgi_engine_find_cluster_layer (number, window->layer);
  if (cluster != NULL) return object = GAMGI_CAST_OBJECT cluster;
  break;

  case GAMGI_ENGINE_MOLECULE:
  if (window == NULL)
    molecule = gamgi_engine_find_molecule_gamgi (number);
  else
    molecule = gamgi_engine_find_molecule_layer (number, window->layer);
  if (molecule != NULL) return object = GAMGI_CAST_OBJECT molecule;
  break;

  case GAMGI_ENGINE_GROUP:
  if (window == NULL)
    group = gamgi_engine_find_group_gamgi (number);
  else
    group = gamgi_engine_find_group_layer (number, window->layer);
  if (group != NULL) return object = GAMGI_CAST_OBJECT group;
  break;

  case GAMGI_ENGINE_PLANE: 
  if (window == NULL)
    plane = gamgi_engine_find_plane_gamgi (number);
  else
    plane = gamgi_engine_find_plane_layer (number, window->layer);
  if (plane != NULL) return object = GAMGI_CAST_OBJECT plane;
  break;

  case GAMGI_ENGINE_DIRECTION:
  if (window == NULL)
    direction = gamgi_engine_find_direction_gamgi (number);
  else
    direction = gamgi_engine_find_direction_layer (number, window->layer);
  if (direction != NULL) return object = GAMGI_CAST_OBJECT direction;
  break;

  case GAMGI_ENGINE_ATOM:
  if (window == NULL)
    atom = gamgi_engine_find_atom_gamgi (number);
  else
    atom = gamgi_engine_find_atom_layer (number, window->layer);
  if (atom != NULL) return object = GAMGI_CAST_OBJECT atom;
  break;

  case GAMGI_ENGINE_BOND:
  if (window == NULL)
    bond = gamgi_engine_find_bond_gamgi (number);
  else
    bond = gamgi_engine_find_bond_layer (number, window->layer);
  if (bond != NULL) return object = GAMGI_CAST_OBJECT bond;
  break;

  case GAMGI_ENGINE_ORBITAL:
  if (window == NULL)
    orbital = gamgi_engine_find_orbital_gamgi (number);
  else
    orbital = gamgi_engine_find_orbital_layer (number, window->layer);
  if (orbital != NULL) return object = GAMGI_CAST_OBJECT orbital;
  break;

  case GAMGI_ENGINE_TEXT:
  if (window == NULL)
    text = gamgi_engine_find_text_gamgi (number);
  else
    text = gamgi_engine_find_text_layer (number, window->layer);
  if (text != NULL) return object = GAMGI_CAST_OBJECT text;
  break;
  }

return NULL;
}

static gamgi_object *static_object (GtkWidget *host, char *tag,
gamgi_enum class, gamgi_window *window, char *name)
{
GtkWidget *entry;
const char *input;
long number;

/******************
 * get input data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (host), tag);
input = gtk_entry_get_text (GTK_ENTRY (entry));

/***************************
 * get object name, number *
 ***************************/

if (gamgi_io_token_alpha_long_scan (input, name, &number,
GAMGI_ENGINE_TOKEN, 1, LONG_MAX) == FALSE) return NULL;

/**************
 * get object *
 **************/

return static_address (class, window, number);
}

gamgi_bool gamgi_gtk_object_name (GtkWidget *host,
char *tag, char *label, char *name)
{
GtkWidget *entry;
const char *input;

/******************
 * get input data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (host), tag);
input = gtk_entry_get_text (GTK_ENTRY (entry));

/********************
 * use default name *
 ********************/

if (gamgi_io_token_check (input) == FALSE)
  { strcpy (name, label); return TRUE; }

/************
 * get name *
 ************/

if (gamgi_io_token_alpha_scan (input, name,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return FALSE;

return TRUE;
}

gamgi_object *gamgi_gtk_object_number (GtkWidget *host, 
char *tag, gamgi_enum class, gamgi_window *window, char *name)
{
/**************
 * get object *
 **************/

return static_object (host, tag, class, window, name);
}

gamgi_object *gamgi_gtk_object_name_number (GtkWidget *host,
char *tag, gamgi_enum class, gamgi_window *window)
{
gamgi_object *object;
char name[GAMGI_ENGINE_TOKEN];

/**************
 * get object *
 **************/

object = static_object (host, tag, class, window, name);
if (object == NULL) return NULL;

/****************
 * confirm name *
 ****************/

if (strcmp (name, object->name) != 0) return NULL;
return object;
}

void gamgi_gtk_object_layer_local (gamgi_layer *layer, gamgi_window *window)
{
window->layer = layer;
gamgi_gtk_statusbar_layer_show (window);
}

void gamgi_gtk_object_focus_local (gamgi_object *object,
gamgi_window *window_dialog)
{
gamgi_window *window;

/***********************************************************
 * if the current object is Gamgi, the first window in the *
 * list must know, because it existed before (the current  *
 * window might be a new window), unless this is the first *
 * window created, in which case the focus cannot be Gamgi *
 ***********************************************************/

if (gamgi->focus != FALSE)
  {
  gamgi->focus = FALSE;
  for (window = gamgi->window_start; 
  window != NULL; window = window->next)
    {
    /****************************************************************
     * the test for NULL is needed because when importing windows   *
     * from files, at this stage some might be engine-created but   *
     * not gtk-created, so window exists but window->window not yet *
     ****************************************************************/

    if (window != window_dialog && window->window != NULL)
      {
      window->focus = GAMGI_CAST_OBJECT window->layer;
      gamgi_gtk_statusbar_focus_show (window);
      }
    }
  }

window_dialog->focus = object;
gamgi_gtk_statusbar_focus_show (window_dialog);
}

void gamgi_gtk_object_focus_global (gamgi_window *window_dialog)
{
gamgi_window *window;

if (gamgi_global_selection_list (window_dialog->class) == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Empty list of selected objects", window_dialog);
  return;
  }

gamgi->focus = window_dialog->class;
for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_statusbar_focus_show (window);
}

gamgi_layer *gamgi_gtk_object_layer_create (gamgi_window *window)
{
gamgi_layer *layer;

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, gamgi->layer);

/**********************************
 * start undo and save mechanisms *
 **********************************/

gamgi_gtk_history_start (layer);

/************************
 * link layer to window *
 ************************/

gamgi_engine_link_layer_window (layer, window);

return layer;
}

gamgi_layer *gamgi_gtk_object_layer_current (gamgi_layer *layer, 
gamgi_window *window)
{
gamgi_layer *layer_new;
gamgi_dlist *dlist;

dlist = GAMGI_CAST_DLIST layer->object.dlist;

/*************************
 * Try the layer before. *
 *************************/

if (dlist->before != NULL)
  layer_new = GAMGI_CAST_LAYER (dlist->before)->data;

/***********************
 * Try the layer next. *
 ***********************/

else if (dlist->next != NULL)
  layer_new = GAMGI_CAST_LAYER (dlist->next)->data;

/*************************************
 * create a new layer, to be current * 
 *************************************/

else layer_new = gamgi_gtk_object_layer_create (window);

/**********************************************************
 * layer_new becomes new current layer and current object *
 **********************************************************/

gamgi_gtk_object_layer_local (layer_new, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer_new, window);

return layer_new;
}

void gamgi_gtk_object_raise (gamgi_layer *layer, gamgi_window *window)
{
GdkWindow *drawable;

drawable = gtk_widget_get_window (window->window);
gdk_window_raise (drawable);
if (layer != window->layer)
  {
  gamgi_gtk_object_layer_local (layer, window);
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer, window);
  }
}

void gamgi_gtk_object_link (gamgi_object *child, 
gamgi_object *parent, gamgi_window *window)
{
gamgi_window *window_child, *window_parent;
gamgi_layer *layer_child, *layer_parent;
GdkWindow *drawable;

/********************************************
 * check if child already belongs to parent *
 ********************************************/

if (child->object == parent)
  {
  gamgi_gtk_dialog_message_create ("Error", 
  "Child already linked to parent", window);
  return;
  }

/************************************************************
 * When parent and child are recursive objects (assembly,   *
 * cluster, group or text), check if the parent belongs     *
 * to the child!: in that case, link first the parent to    *
 * the child parent before linking the child to the parent. *
 ************************************************************/

if (child->class == parent->class && 
gamgi_engine_find_object (parent, child) == TRUE)
  {
  gamgi_engine_unlink_object (parent);
  gamgi_engine_link_object_object (parent, child->object);
  }
  
/**********************************
 * get current linking situation  *
 * before linking child to parent *
 **********************************/

if (parent->class == GAMGI_ENGINE_WINDOW)
  {
  /*************************************************
   * link child layer to a different parent window *
   *************************************************/

  window_parent = GAMGI_CAST_WINDOW parent;
  layer_parent =  GAMGI_CAST_LAYER child;
  window_child =  GAMGI_CAST_WINDOW child->object;
  layer_child =  GAMGI_CAST_LAYER window_child->layer;

  /*************************************
   * If layer is current layer, then a *
   *  new current layer must be found. *
   *************************************/

  if (layer_parent == layer_child)
    gamgi_gtk_object_layer_current (layer_child, window_child);
  }
else
  {
  /***************************************
   * link child to parent layer or below *
   ***************************************/

  layer_parent = gamgi_engine_find_layer (parent);
  window_parent =  GAMGI_CAST_WINDOW layer_parent->object.object;
  layer_child = gamgi_engine_find_layer (child);
  window_child =  GAMGI_CAST_WINDOW layer_child->object.object;

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

  if (layer_parent != layer_child) gamgi_chem_bond_half_remove (child);

  /***********************************
   * If child is the current object, *
   * then replace by current layer   *
   ***********************************/

  if (child == window_child->focus)
    gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT 
    window_child->layer, window_child);
  }

gamgi_engine_unlink_object (child);
gamgi_engine_link_object_object (child, parent);

/********************************************************
 * raise to top window and layer where parent object is *
 ********************************************************/

drawable = gtk_widget_get_window (window_parent->window);
gdk_window_raise (drawable);
if (layer_parent != window_parent->layer)
  {
  gamgi_gtk_object_layer_local (layer_parent, window_parent);
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer_parent, window_parent);
  }

/***************************************************
 * redraw drawing area for parent and child window *
 ***************************************************/

gtk_widget_queue_draw (window_parent->area);
if (window_child != window_parent)
  gtk_widget_queue_draw (window_child->area);
}

void gamgi_gtk_object_copy (gamgi_object *child,
gamgi_object *parent, gamgi_window *window)
{
gamgi_window *window_up = NULL;
gamgi_layer *layer_up;
GdkWindow *drawable;

/*****************************************************
 * child: is the last copied object; parent: is the  *
 * host object, directly containg the copied objects *
 *****************************************************/

if (child->class == GAMGI_ENGINE_WINDOW)
  {
  window_up = GAMGI_CAST_WINDOW child;
  }
else if (child->class == GAMGI_ENGINE_LAYER)
  {
  /***************************************************************
   * set the last layer copy as the new current layer and object *
   ***************************************************************/

  window_up = GAMGI_CAST_WINDOW parent;
  gamgi_gtk_object_layer_local (GAMGI_CAST_LAYER child, window_up);
  gamgi_gtk_object_focus_local (child, window_up);
  }
else if (parent->class == GAMGI_ENGINE_LAYER)
  {
  layer_up = GAMGI_CAST_LAYER parent;
  window_up =  GAMGI_CAST_WINDOW layer_up->object.object;

  /***************************************************************
   * when the layer containing the copied objects is not the top *
   * layer, set that layer as the new current layer and object   *
   ***************************************************************/

  if (window_up->layer != layer_up)
    {
    gamgi_gtk_object_layer_local (layer_up, window_up);
    gamgi_gtk_object_focus_local (parent, window_up);
    }
  }

/****************************************************
 * raise window containing the copied object to top *
 ****************************************************/

drawable = gtk_widget_get_window (window_up->window);
gdk_window_raise (drawable);
}
