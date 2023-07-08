/*****************************************************
 *
 * $GAMGI/src/gtk/orbital/gamgi_gtk_orbital_create.h
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

static void static_axes (GtkWidget *widget, void *data);

static void static_phase (GtkWidget *widget, void *data);

static void static_frame (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data);

static void static_m (GtkWidget *widget, void *data);

static void static_l (GtkWidget *widget, void *data);

static void static_n (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_orbital_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_orbital_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented: Nov 2012. Last update: May 2013.

 *************
 * Rationale *
 *************/
