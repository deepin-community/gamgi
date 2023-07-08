/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_modify.h
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

static void static_absolute (GtkWidget *dialog);

static void static_relative (GtkWidget *dialog);

static void static_translation_clean (GtkWidget *widget, void *data);

static void static_rotation_clean (GtkWidget *widget, void *data);

static void static_conventional (GtkWidget *widget, void *data);

static void static_vectors (GtkWidget *widget, void *data);

static void static_faces (GtkWidget *widget, void *data);

static void static_model (GtkWidget *widget, void *data);

static void static_button (GtkWidget *dialog, char *string);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, unsigned int tag, void *data);

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

void gamgi_gtk_cell_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_cell_modify (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned. Global mode not implemented yet.
Trigonal, Monoclinic and Triclinic Wigner-Seitz
cells not implemented yet.

 *************
 * Rationale *
 *************/
