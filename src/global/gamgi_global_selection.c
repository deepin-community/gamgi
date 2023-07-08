/**********************************************
 *
 * $GAMGI/src/global/gamgi_global_selection.c
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"

void gamgi_global_selection_text (gamgi_text *parent, int mark)
{
gamgi_text *child;
gamgi_dlist *dlist;

for (dlist = parent->text_start; dlist != NULL; dlist = dlist->next)
  {
  child = GAMGI_CAST_TEXT dlist->data;
  child->mark = mark;
  gamgi_global_selection_text (child, mark);
  }
}

void gamgi_global_selection_group (gamgi_group *parent, int mark)
{
gamgi_group *child;
gamgi_dlist *dlist;

for (dlist = parent->group_start; dlist != NULL; dlist = dlist->next)
  {
  child = GAMGI_CAST_GROUP dlist->data;
  child->mark = mark;
  gamgi_global_selection_group (child, mark);
  }
}

void gamgi_global_selection_cluster (gamgi_cluster *parent, int mark)
{
gamgi_cluster *child;
gamgi_dlist *dlist;

for (dlist = parent->cluster_start; dlist != NULL; dlist = dlist->next)
  {
  child = GAMGI_CAST_CLUSTER dlist->data;
  child->mark = mark;
  gamgi_global_selection_cluster (child, mark);
  }
}

void gamgi_global_selection_assembly (gamgi_assembly *parent, int mark)
{
gamgi_assembly *child;
gamgi_dlist *dlist;

for (dlist = parent->assembly_start; dlist != NULL; dlist = dlist->next)
  {
  child = GAMGI_CAST_ASSEMBLY dlist->data;
  child->mark = mark;
  gamgi_global_selection_assembly (child, mark);
  }
}

gamgi_dlist *gamgi_global_selection_list (gamgi_enum class)
{
switch (class)
  {
  case GAMGI_ENGINE_TEXT:
  return gamgi->texts;

  case GAMGI_ENGINE_ORBITAL:
  return gamgi->orbitals;

  case GAMGI_ENGINE_BOND:
  return gamgi->bonds;

  case GAMGI_ENGINE_ATOM:
  return gamgi->atoms;

  case GAMGI_ENGINE_DIRECTION:
  return gamgi->directions;

  case GAMGI_ENGINE_PLANE:
  return gamgi->planes;

  case GAMGI_ENGINE_GROUP:
  return gamgi->groups;

  case GAMGI_ENGINE_MOLECULE:
  return gamgi->molecules;

  case GAMGI_ENGINE_CLUSTER:
  return gamgi->clusters;

  case GAMGI_ENGINE_CELL:
  return gamgi->cells;

  case GAMGI_ENGINE_ARROW:
  return gamgi->arrows;

  case GAMGI_ENGINE_SHAPE:
  return gamgi->shapes;

  case GAMGI_ENGINE_GRAPH:
  return gamgi->graphs;

  case GAMGI_ENGINE_ASSEMBLY:
  return gamgi->assemblys;

  case GAMGI_ENGINE_LIGHT:
  return gamgi->lights;

  case GAMGI_ENGINE_LAYER:
  return gamgi->layers;

  case GAMGI_ENGINE_WINDOW:
  return gamgi->windows;

  case GAMGI_ENGINE_GAMGI:
  return GAMGI_CAST_DLIST gamgi;
  }

return NULL;
}

void gamgi_global_selection_layer (void)
{
gamgi_window *window;

/****************************************************
 * for all windows: set last layer as current layer *
 ****************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_object_layer_local (GAMGI_CAST_LAYER window->layer_end->data, window);
}

void gamgi_global_selection_focus (void)
{
gamgi_window *window;

/********************************************************
 * for all windows: set current layer as current object *
 ********************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);
}

void gamgi_global_selection_undo (void)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_dlist *dlist;

/***************************************************
 * for all windows: save all layers as undo layers *
 ***************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
    {
    layer = GAMGI_CAST_LAYER dlist->data;
    gamgi_gtk_history_update (layer, layer->undo, GAMGI_ENGINE_UNDO);
    }
  }
}

void gamgi_global_selection_hide (char *name, gamgi_window *window)
{
GtkWidget *button;
GtkWidget *label;

/*************************************************************
 * change only gamgi windows with a gtk window: for example, *
 * gtk windows are removed before window child objects       *
 *************************************************************/

if (window->window == NULL) return;

/*************************************************
 * set normal color for object label in top menu *
 *************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), name);
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_dialog_color_default (label, window);
}

void gamgi_global_selection_show (char *name, gamgi_window *window)
{
GtkWidget *button;
GtkWidget *label;
GdkColor color;

/*************************************************************
 * change only gamgi windows with a gtk window: for example, *
 * gtk windows are removed before window child objects       *
 *************************************************************/

if (window->window == NULL) return;

/***************************************************
 * set positive color for object label in top menu *
 ***************************************************/

gamgi_gtk_dialog_color (&color,
GAMGI_GTK_MOVE_R, GAMGI_GTK_MOVE_G, GAMGI_GTK_MOVE_B);

button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), name);
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_dialog_color_fg (label, color);
}

void gamgi_global_selection_update (gamgi_dlist **start,
char *label, gamgi_window *window)
{
if (*start == NULL)
  gamgi_global_selection_hide (label, window);
else
  gamgi_global_selection_show (label, window);
}

void gamgi_global_selection_update_objects (gamgi_window *window)
{
/*******************************************
 * update all object labels for one window *
 *******************************************/

gamgi_global_selection_update (&gamgi->texts, "button_text", window);
gamgi_global_selection_update (&gamgi->orbitals, "button_orbital", window);
gamgi_global_selection_update (&gamgi->bonds, "button_bond", window);
gamgi_global_selection_update (&gamgi->atoms, "button_atom", window);
gamgi_global_selection_update (&gamgi->directions, "button_direction", window);
gamgi_global_selection_update (&gamgi->planes, "button_plane", window);
gamgi_global_selection_update (&gamgi->groups, "button_group", window);
gamgi_global_selection_update (&gamgi->molecules, "button_molecule", window);
gamgi_global_selection_update (&gamgi->clusters, "button_cluster", window);
gamgi_global_selection_update (&gamgi->cells, "button_cell", window);
gamgi_global_selection_update (&gamgi->arrows, "button_arrow", window);
gamgi_global_selection_update (&gamgi->shapes, "button_shape", window);
gamgi_global_selection_update (&gamgi->graphs, "button_graph", window);
gamgi_global_selection_update (&gamgi->assemblys, "button_assembly", window);
gamgi_global_selection_update (&gamgi->lights, "button_light", window);
gamgi_global_selection_update (&gamgi->layers, "button_layer", window);
gamgi_global_selection_update (&gamgi->windows, "button_window", window);
}

void gamgi_global_selection_update_windows (gamgi_dlist **start, char *label)
{
gamgi_window *window;

/*******************************************
 * update all windows for one object label *
 *******************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_global_selection_update (start, label, window);
}

void gamgi_global_selection_link (gamgi_dlist *start)
{
gamgi_dlist *dlist;

for (dlist = start; dlist != NULL; dlist = dlist->next)
  (GAMGI_CAST_OBJECT dlist->data)->selection = dlist;
}

void gamgi_global_selection_unlink (gamgi_dlist *start)
{
gamgi_dlist *dlist;

/********************************************************************
 * when the list is removed in the end, the objects no longer exist *
 ********************************************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  if (dlist->data != NULL)
    (GAMGI_CAST_OBJECT dlist->data)->selection = NULL;
}

void gamgi_global_selection_remove (gamgi_dlist **start, char *label)
{
if (*start == NULL) return;

gamgi_global_selection_unlink (*start);
gamgi_engine_dlist_remove_all (*start);
*start = NULL;
gamgi_global_selection_update_windows (start, label);
}

void gamgi_global_selection_remove_all (void)
{
gamgi_global_selection_remove (&gamgi->texts, "button_text");
gamgi_global_selection_remove (&gamgi->orbitals, "button_orbital");
gamgi_global_selection_remove (&gamgi->bonds, "button_bond");
gamgi_global_selection_remove (&gamgi->atoms, "button_atom");
gamgi_global_selection_remove (&gamgi->directions, "button_direction");
gamgi_global_selection_remove (&gamgi->planes, "button_plane");
gamgi_global_selection_remove (&gamgi->groups, "button_group");
gamgi_global_selection_remove (&gamgi->molecules, "button_molecule");
gamgi_global_selection_remove (&gamgi->clusters, "button_cluster");
gamgi_global_selection_remove (&gamgi->cells, "button_cell");
gamgi_global_selection_remove (&gamgi->arrows, "button_arrow");
gamgi_global_selection_remove (&gamgi->shapes, "button_shape");
gamgi_global_selection_remove (&gamgi->graphs, "button_graph");
gamgi_global_selection_remove (&gamgi->assemblys, "button_assembly");
gamgi_global_selection_remove (&gamgi->lights, "button_light");
gamgi_global_selection_remove (&gamgi->layers, "button_layer");
gamgi_global_selection_remove (&gamgi->windows, "button_window");
}

void gamgi_global_selection_clean (gamgi_enum class,
gamgi_dlist **start, char *label, gamgi_dlist *selection)
{
/****************************************************************
 * before removing this object, check if it is in the selection *
 * list, in which case the selection list should be removed.    *
 ****************************************************************/

if (selection != NULL)
  {
  /***********************************************************************
   * set selection->data to NULL, to signal that this object has been    *
   * removed: this is needed because: 1) recursive objects might be      *
   * removed recursively and show up later in the list; 2) when removing *
   * a list of objects, is better to remove the list only in the end.    *
   ***********************************************************************/

  selection->data = NULL;

  /*******************************************************
   * When start is not NULL, the selection list still    *
   * exists, is available, should be removed immediately *
   *******************************************************/

  if (*start != NULL)
    {
    /*************************************
     * class must describe a true object *
     * class, not a global Gamgi object  *
     *************************************/

    gamgi_global_selection_remove (start, label);
    if (gamgi->focus == class) gamgi_global_selection_focus ();
    }
  }
}
