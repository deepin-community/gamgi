/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_gamgi_exit.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_mesa_lists.h"
#include "gamgi_global_selection.h"

void gamgi_gtk_gamgi_exit_ok (GtkWidget *widget, void *data)
{ 
gamgi_window *window;

/***************************************
 * Remove resources: 1) mesa lists     *
 * 2) global timers (to show the time) *
 ***************************************/

gamgi_mesa_lists_remove ();
g_source_remove (gamgi->timer_time);

/*************************
 * remove global dialogs *
 *************************/

gamgi_gtk_dialog_global_remove ();

/*********************************
 * remove object selection lists *
 *********************************/

gamgi_global_selection_remove_all ();

/***********************************************************
 * remove dialogs, timers, windows: dialogs may address    *
 * other windows, so it is important to remove them first! *
 ***********************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_window_remove_dialogs (window);

for (window = gamgi->window_start; window != NULL; window = window->next)
  gamgi_gtk_window_remove_window (window);

/***********************************************
 * quit the gtk main loop: execution continues *
 * immediately after gtk_main (), on main ().  *
 ***********************************************/

gtk_main_quit ();
}

void gamgi_gtk_gamgi_exit (GtkWidget *widget, void *data)
{
gamgi_gtk_dialog_question_create ("Exit", "Exit the program?", 
gamgi_gtk_gamgi_exit_ok, data);
}
