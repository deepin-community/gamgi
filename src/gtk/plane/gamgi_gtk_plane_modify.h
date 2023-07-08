/*************************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_modify.h
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

static void static_name (gamgi_window *window, char *name);

static void static_indices (GtkWidget *widget, void *data);

static void static_atom_remove (gamgi_plane *plane, gamgi_bool *link);

static gamgi_bool static_atom_parent (gamgi_plane *plane, gamgi_atom *atom);

static void static_plane_remove (gamgi_plane *plane, gamgi_bool *link);

static GtkWidget *static_atom (GtkWidget *dialog);

static void static_end_atom (GtkWidget *dialog);

static gamgi_enum static_class (gamgi_window *window);

static void static_start_cell (gamgi_window *window);

static void static_start_atom (gamgi_window *window);

static void static_instance (GtkWidget *widget, void *data);

static void static_start_scope (gamgi_window *window);

static void static_start_projection (gamgi_window *window);

static void static_nodes (GtkWidget *widget, void *data);

static void static_vectors (GtkWidget *widget, void *data);

static void static_page_show (gamgi_window *window);

static void static_page_hide (gamgi_window *window);

static void static_page_handle (gamgi_window *window);

static void static_cell (GtkWidget *widget, void *data);

static void static_reference (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply (gamgi_plane *plane, gamgi_window *window);

static void static_ok_global (gamgi_window *window);

static void static_ok_local (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_plane_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_plane_modify (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
