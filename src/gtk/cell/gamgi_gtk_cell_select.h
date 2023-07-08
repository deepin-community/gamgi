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

static int static_system (char *system, gamgi_window *window);

static int static_lattice (char *lattice, gamgi_window *window);

static int static_cell_system (gamgi_enum bravais);

static int static_cell_lattice (gamgi_enum bravais);

static void static_method_system (char *label, gamgi_window *window);

static void static_method_lattice (char *label, gamgi_window *window);

static void static_method_model (char *label, gamgi_window *window);

static void static_method_o4 (char *label, gamgi_window *window);

static void static_method_borders (char *label, gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window);

static gamgi_dlist *static_ok_system (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_lattice (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_group (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_a (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_b (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_c (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_bc (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_ac (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_ab (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_model (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_n1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_n2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_n3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v23 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v13 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_v12 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_o4 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_axes (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_borders (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_faces (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_nodes (gamgi_dlist *dlist_old,
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

void gamgi_gtk_cell_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_cell_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Nov 2009.

 *************
 * Rationale *
 *************/
