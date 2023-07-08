/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_gamgi_reset.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_number.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_window_create.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_mesa_start.h"
#include "gamgi_global_selection.h"

static void static_window (gamgi_window *window)
{
/**********************************
 * 1) restart window top menus    *
 * 1) restart window progress bar *
 * 1) restart window information  *
 * 2) create and restart layer    *
 * 3) redraw layer                *
 **********************************/

gamgi_gtk_window_create_menus (window);
gamgi_gtk_window_create_progress (window);
gamgi_gtk_window_create_data (window);
gamgi_gtk_window_create_layer (window);
gtk_widget_queue_draw (window->area);
}

static void static_unlinked (void)
{
/*****************************************************
 * remove all unlinked objects (actually this should *
 * never occur as objects should always be linked!)  *
 *****************************************************/

while (gamgi->layer_end != NULL)
  gamgi_engine_remove_layer (gamgi->layer_end);

while (gamgi->light_end != NULL)
  gamgi_engine_remove_light (gamgi->light_end);

while (gamgi->assembly_end != NULL)
  gamgi_engine_remove_assembly (gamgi->assembly_end);

while (gamgi->graph_end != NULL)
  gamgi_engine_remove_graph (gamgi->graph_end);

while (gamgi->shape_end != NULL)
  gamgi_engine_remove_shape (gamgi->shape_end);

while (gamgi->arrow_end != NULL)
  gamgi_engine_remove_arrow (gamgi->arrow_end);

while (gamgi->cell_end != NULL)
  gamgi_engine_remove_cell (gamgi->cell_end);

while (gamgi->cluster_end != NULL)
  gamgi_engine_remove_cluster (gamgi->cluster_end);

while (gamgi->molecule_end != NULL)
  gamgi_engine_remove_molecule (gamgi->molecule_end);

while (gamgi->group_end != NULL)
  gamgi_engine_remove_group (gamgi->group_end);

while (gamgi->plane_end != NULL)
  gamgi_engine_remove_plane (gamgi->plane_end);

while (gamgi->direction_end != NULL)
  gamgi_engine_remove_direction (gamgi->direction_end);

while (gamgi->atom_end != NULL)
  gamgi_engine_remove_atom (gamgi->atom_end);

while (gamgi->bond_end != NULL)
  gamgi_engine_remove_bond (gamgi->bond_end);

while (gamgi->orbital_end != NULL)
  gamgi_engine_remove_orbital (gamgi->orbital_end);

while (gamgi->text_end != NULL)
  gamgi_engine_remove_text (gamgi->text_end);

}

static void static_pointers (gamgi_window *window)
{
/************************
 * restart object lists *
 ************************/

gamgi->text_start = NULL;
gamgi->text_end = NULL;
gamgi->orbital_start = NULL;
gamgi->orbital_end = NULL;
gamgi->bond_start = NULL;
gamgi->bond_end = NULL;
gamgi->atom_start = NULL;
gamgi->atom_end = NULL;
gamgi->direction_start = NULL;
gamgi->direction_end = NULL;
gamgi->plane_start = NULL;
gamgi->plane_end = NULL;
gamgi->group_start = NULL;
gamgi->group_end = NULL;
gamgi->molecule_start = NULL;
gamgi->molecule_end = NULL;
gamgi->cluster_start = NULL;
gamgi->cluster_end = NULL;
gamgi->cell_start = NULL;
gamgi->cell_end = NULL;
gamgi->arrow_start = NULL;
gamgi->arrow_end = NULL;
gamgi->shape_start = NULL;
gamgi->shape_end = NULL;
gamgi->graph_start = NULL;
gamgi->graph_end = NULL;
gamgi->assembly_start = NULL;
gamgi->assembly_end = NULL;
gamgi->light_start = NULL;
gamgi->light_end = NULL;
gamgi->layer_start = NULL;
gamgi->layer_end = NULL;
gamgi->window_start = window;
gamgi->window_end = window;
}

static void static_numbers (void)
{
/**************************
 * restart object numbers *
 **************************/

gamgi_engine_number_text_reset ();
gamgi_engine_number_orbital_reset ();
gamgi_engine_number_bond_reset ();
gamgi_engine_number_atom_reset ();
gamgi_engine_number_direction_reset ();
gamgi_engine_number_plane_reset ();
gamgi_engine_number_group_reset ();
gamgi_engine_number_molecule_reset ();
gamgi_engine_number_cluster_reset ();
gamgi_engine_number_cell_reset ();
gamgi_engine_number_arrow_reset ();
gamgi_engine_number_shape_reset ();
gamgi_engine_number_graph_reset ();
gamgi_engine_number_assembly_reset ();
gamgi_engine_number_light_reset ();
gamgi_engine_number_layer_reset ();
gamgi_engine_number_window_reset ();
}

/***************** internal function ******************
 *                                                    *
 *                     STATIC_OK                      *
 *                                                    *
 * Reset GAMGI: 1) remove all windows but the current *
 * one; 2) remove all layers in the current window;   *
 * 3) restart the overall GAMGI state; recreate the   *
 * current layer.                                     *
 *                                                    *
 ******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{  
gamgi_window *window_dialog = GAMGI_CAST_WINDOW data;
gamgi_window *window;
gamgi_dlist *dlist;

/*************************
 * remove global dialogs *
 *************************/

gamgi_gtk_dialog_global_remove ();

/*********************************
 * remove object selection lists *
 *********************************/

gamgi_global_selection_remove_all ();

/*******************************************************************
 * remove dialogs, timers, windows except current: dialogs may     *
 * address other windows, so it is important to remove them first! *
 *******************************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_window_remove_dialogs (window);

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  if (window == window_dialog) continue;
  gamgi_gtk_window_remove_window (window);
  gamgi_engine_remove_window (window);
  }

/**********************************************
 * remove all layers linked to current window *
 **********************************************/

dlist = window_dialog->layer_start;
while (dlist != NULL)
  { gamgi_engine_remove_layer (GAMGI_CAST_LAYER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
window_dialog->layer_start = NULL;
window_dialog->layer_end = NULL;

/*********************************
 * remove object selection lists *
 *********************************/

gamgi_global_selection_remove_all ();

/***********************
 * restart global data *
 ***********************/

static_unlinked ();
static_pointers (window_dialog);
static_numbers ();

/******************
 * restart window *
 ******************/

static_window (window_dialog);
}

/**************** external function **************
 *                                               *
 *              GAMGI_GTK_GAMGI_RESET            *
 *                                               *
 * Set reset parameters for the question dialog. *
 *                                               *
 *************************************************/

void gamgi_gtk_gamgi_reset (GtkWidget *widget, void *data)
{
gamgi_gtk_dialog_question_create ("Reset", "Reset the program?", static_ok, data);
}
