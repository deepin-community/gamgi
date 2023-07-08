/***********************************************
 *
 * $GAMGI/src/gtk/bond/gamgi_gtk_bond_create.h
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

static void static_color (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data);

static void static_limit (GtkWidget *widget, void *data);

static void static_length (gamgi_window *window);

static void static_model (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_bond_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_bond_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
