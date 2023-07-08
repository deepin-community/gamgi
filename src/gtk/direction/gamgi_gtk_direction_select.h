/*********************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_select.h
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

static gamgi_bool static_node (gamgi_direction *direction, int node, int value);

static void static_method_reference (char *label, gamgi_window *window);

static void static_method_model (char *label, gamgi_window *window);

static void static_method_net (char *label, gamgi_window *window);

static void static_method_o4 (char *label, gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window);

static gamgi_dlist *static_ok_reference (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_u (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_w (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_net (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o4 (gamgi_dlist *dlist_old,
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

void gamgi_gtk_direction_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_direction_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Jan 2010.

 *************
 * Rationale *
 *************/
