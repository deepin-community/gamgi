/*********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_link.h
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

static gamgi_enum static_class (gamgi_window *window);

static void static_reference (GtkWidget *widget, void *data);

static void static_number (GtkWidget *widget, void *data);

static void static_xyz_clean (gamgi_window *window);

static void static_xyz_start (gamgi_window *window);

static void static_wyckoff_change (GtkWidget *widget, void *data);

static void static_wyckoff_start (gamgi_cell *cell, gamgi_window *window);

static void static_nodes_start (gamgi_cell *cell, gamgi_window *window);

static void static_start_shift (gamgi_window *window);

static void static_start_change (gamgi_window *window);

static void static_start_occupancy (gamgi_window *window);

static void static_start_packing (gamgi_window *window);

static void static_cell (GtkWidget *widget, void *data);

static void static_method (GtkWidget *widget, void *data);

static void static_hierarchy (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_clean (gamgi_cell *cell, gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static void static_origin (gamgi_object *child, gamgi_object *parent);

static void static_nodes (gamgi_cell *cell, gamgi_window *window);

static void static_rotation (gamgi_cell *cell, double angle, gamgi_enum axis,
gamgi_enum vectors, int *uvw, double *rotation, gamgi_window *window);

static void static_translation (gamgi_cell *cell, double *shift,
gamgi_enum vectors, double *translation, gamgi_window *window);

static gamgi_bool static_grab_object (gamgi_window *window);

static gamgi_bool static_grab_crystal (gamgi_window *window);

static gamgi_bool static_grab_glass (gamgi_window *window);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply_object (gamgi_object *child, gamgi_object *parent,
gamgi_layer *layer_child, gamgi_layer *layer_parent, gamgi_window *window_child,
gamgi_window *window_parent, gamgi_window *window_dialog);

static gamgi_bool static_apply_crystal (gamgi_cell *cell, gamgi_window *window);

static gamgi_bool static_apply_glass (gamgi_cell *cell, gamgi_window *window);

static gamgi_slist *static_ok_global_object (gamgi_window *window);

static gamgi_slist *static_ok_global_crystal (gamgi_window *window);

static gamgi_slist *static_ok_global_glass (gamgi_window *window);

static void static_ok_local_object (gamgi_cell *cell, gamgi_window *window);

static gamgi_bool static_ok_local_crystal (gamgi_cell *cell, gamgi_window *window);

static gamgi_bool static_ok_local_glass (gamgi_cell *cell, gamgi_window *window);

static void static_ok_global (gamgi_window *window_dialog);

static void static_ok_local (gamgi_window *window_dialog);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_cell_link_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_cell_link (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
