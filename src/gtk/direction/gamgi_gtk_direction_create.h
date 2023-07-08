/*********************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_create.h
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

static void static_instance (GtkWidget *widget, void *data);

static void static_nodes (gamgi_cell *cell, gamgi_window *window);

static void static_start_scope (gamgi_window *window);

static void static_start_projection (gamgi_window *window);

static void static_end_cell (gamgi_window *window);

static void static_end_atom (gamgi_window *window);

static void static_cell (GtkWidget *widget, void *data);

static GtkWidget *static_atom (GtkWidget *dialog);

static void static_vectors (GtkWidget *widget, void *data);

static void static_reference (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_bool static_ok_cell (gamgi_direction *direction,
gamgi_cell **cell, gamgi_window *window);

static gamgi_bool static_ok_atoms (gamgi_direction *direction,
gamgi_atom **atom1, gamgi_atom **atom2, gamgi_window *window);

static gamgi_bool static_ok_scope (gamgi_direction *direction,
gamgi_cell *cell, double *thickness, gamgi_window *window);

static gamgi_bool static_ok_projection (gamgi_direction *direction,
gamgi_bool *pole, gamgi_bool *trace, gamgi_window *window);

static gamgi_bool static_ok_view (gamgi_direction *direction, 
gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_direction_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_direction_create_node (gamgi_bool state, gamgi_window *window);

void gamgi_gtk_direction_create_uvw (gamgi_bool active, gamgi_window *window);

void gamgi_gtk_direction_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
