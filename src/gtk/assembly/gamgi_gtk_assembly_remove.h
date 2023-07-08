/*******************************************************
 *
 * $GAMGI/src/gtk/assembly/gamgi_gtk_assembly_remove.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_clean (gamgi_window *window);

static void static_reset (gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static void static_apply (gamgi_assembly *assembly, int scope,
gamgi_window *window);

static void static_ok_global (int scope, gamgi_window *window);

static void static_ok_local (int scope, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_assembly_remove_press (gamgi_window *window_mouse, 
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_assembly_remove (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned. Global mode not implemented yet.

 *************
 * Rationale *
 *************/
