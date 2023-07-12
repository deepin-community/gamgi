/***************************************************
 *
 * $GAMGI/src/gtk/window/gamgi_gtk_window_select.h
 *
 * Copyright (C) 2010 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_destroy (GtkWidget *widget, void *data);

static void static_model (gamgi_dlist *dlist, gamgi_window *window);

static gamgi_dlist *static_dlist (int scope, gamgi_window *window);

static void static_scope (GtkWidget *widget, void *data);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_window_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_window_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Jan 2009.

 *************
 * Rationale *
 *************/
