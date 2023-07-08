/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_select.h
 *
 * Copyright (C) 2004, 2009 Carlos Pereira
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

static void static_method_projection (char *label, gamgi_window *window);

static void static_method_visibility (char *label, gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window);

static gamgi_dlist *static_ok_projection (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_top (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_near (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_far (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_eye_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_eye_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_eye_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_center_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_center_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_center_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_inside (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_outside (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_up_axes (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_red (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_green (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_blue (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_scale (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_layer_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_layer_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Nov 2009.

 *************
 * Rationale *
 *************/
