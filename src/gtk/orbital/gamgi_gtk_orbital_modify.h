/*****************************************************
 *
 * $GAMGI/src/gtk/orbital/gamgi_gtk_orbital_modify.h
 *
 * Copyright (C) 2013 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_name (gamgi_window *window, char *name);

static void static_axes (GtkWidget *widget, void *data);

static void static_phase (GtkWidget *widget, void *data);

static void static_frame (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data);

static void static_m (GtkWidget *widget, void *data);

static void static_l (GtkWidget *widget, void *data);

static void static_n (GtkWidget *widget, void *data);

static void static_type_clean (gamgi_window *window);

static void static_scope_clean (gamgi_window *window);

static void static_numbers (GtkWidget *widget, void *data);

static void static_octants (GtkWidget *widget, void *data);

static void static_absolute (GtkWidget *dialog);

static void static_relative (GtkWidget *dialog);

static void static_origin (GtkWidget *widget, void *data);

static void static_angle (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_list (GtkWidget *widget, void *data);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply (gamgi_text *text, gamgi_window *window);

static void static_ok_global (gamgi_window *window);

static void static_ok_local (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_orbital_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_orbital_modify (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

In development: August 2013.

 *************
 * Rationale *
 *************/
