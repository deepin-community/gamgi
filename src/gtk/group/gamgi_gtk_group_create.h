/*************************************************
 *
 * $GAMGI/src/gtk/group/gamgi_gtk_group_create.h
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

static void static_color (GtkWidget *widget, void *data);

static void static_number (GtkWidget *widget, void *data);

static void static_start_container (gamgi_window *window);

static void static_start_polytope (gamgi_window *window);

static void static_reference (GtkWidget *widget, void *data);

static void static_ok_container (gamgi_group *group, gamgi_window *window);

static void static_ok_polytope (gamgi_group *group, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);
  
 **********************
 * external functions *
 **********************/

void gamgi_gtk_group_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_group_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
