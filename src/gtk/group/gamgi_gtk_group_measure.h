/**************************************************
 *
 * $GAMGI/src/gtk/group/gamgi_gtk_group_measure.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_method (GtkWidget *widget, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_group_measure_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_group_measure (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

In development.

 *************
 * Rationale *
 *************/
