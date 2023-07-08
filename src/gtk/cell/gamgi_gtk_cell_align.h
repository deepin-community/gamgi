/*************************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_align.h
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

static void static_translation_remove (gamgi_window *window);

static void static_translation_enable (gamgi_window *window);

static void static_translation_disable (gamgi_window *window);

static void static_translation_cancel (GtkWidget *widget, void *data);

static int static_translation_delete (GtkWidget *widget, 
GdkEventButton *event, void *data);

static void static_translation_layer (gamgi_window *window);

static void static_translation_cell (gamgi_window *window);

static void static_translation_clicked (GtkWidget *widget, void *data);

static void static_translation_ok (GtkWidget *widget, void *data);

static void static_translation_init (gamgi_window *window);

static void static_translation (gamgi_window *window);

static void static_rotation_remove (gamgi_window *window);

static void static_rotation_enable (gamgi_window *window);

static void static_rotation_disable (gamgi_window *window);

static void static_rotation_cancel (GtkWidget *widget, void *data);

static int static_rotation_delete (GtkWidget *widget, 
GdkEventButton *event, void *data);

static void static_rotation_label (gamgi_window *window);

static void static_rotation_hkl (GtkWidget *widget, void *data);

static void static_rotation_uvw (GtkWidget *widget, void *data);

static void static_rotation_layer (gamgi_window *window);

static void static_rotation_cell (gamgi_window *window);

static void static_rotation_clicked (GtkWidget *widget, void *data);

static void static_rotation_ok (GtkWidget *widget, void *data);

static void static_rotation_init (gamgi_window *window);

static void static_rotation (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_cell_align_press (gamgi_window *window, int x, int y);

void gamgi_gtk_cell_align_start (void);

void gamgi_gtk_cell_align_translation_get (gamgi_cell *cell, double *origin, 
gamgi_window *window);

void gamgi_gtk_cell_align_rotation_get (gamgi_cell *cell, double *angle, 
gamgi_window *window);

void gamgi_gtk_cell_align_translation (GtkWidget *widget, void *data);

void gamgi_gtk_cell_align_rotation (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
