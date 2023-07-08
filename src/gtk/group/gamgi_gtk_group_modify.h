/*************************************************
 *
 * $GAMGI/src/gtk/group/gamgi_gtk_group_modify.h
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

static void static_text (gamgi_text *text);

static void static_orbital (gamgi_orbital *orbital);

static void static_bond (gamgi_bond *bond);

static void static_atom (gamgi_atom *atom);

static void static_direction (gamgi_direction *direction);

static void static_plane (gamgi_plane *plane);

static void static_group (gamgi_group *group);

static void static_remove (gamgi_group *group);

static gamgi_enum static_class (gamgi_window *window);

static void static_number (GtkWidget *widget, void *data);

static void static_absolute (GtkWidget *dialog);

static void static_relative (GtkWidget *dialog);

static void static_color (GtkWidget *widget, void *data);

static void static_start_polytope (gamgi_window *window);

static void static_start_container (gamgi_window *window);

static void static_start_position (gamgi_window *window);

static void static_start_view (gamgi_window *window);

static void static_reference (GtkWidget *widget, void *data);

static void static_translation (GtkWidget *widget, void *data);

static void static_rotation (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply (gamgi_cell *cell, gamgi_window *window);

static void static_ok_global (gamgi_window *window);

static void static_ok_local (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_group_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_group_modify (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
