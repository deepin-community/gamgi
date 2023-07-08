/***********************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_link.h
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static gamgi_enum static_class (gamgi_window *window);

static void static_reference (GtkWidget *widget, void *data);

static void static_hierarchy (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_clean (gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static void static_origin (gamgi_object *child, gamgi_object *parent);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply (gamgi_object *child, gamgi_object *parent,
gamgi_layer *layer_child, gamgi_layer *layer_parent, gamgi_window *window);

static void static_ok_global (gamgi_window *window_dialog);

static void static_ok_local (gamgi_window *window_dialog);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_plane_link_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_plane_link (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
