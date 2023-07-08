/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_create.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_conventional (GtkWidget *widget, void *data);

static void static_vectors (GtkWidget *widget, void *data);

static void static_faces (GtkWidget *widget, void *data);

static void static_model (GtkWidget *widget, void *data);

static void static_button (GtkWidget *dialog, char *string);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, unsigned int tag, void *data);

static void static_reset (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_cell_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_cell_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned. Trigonal, Monoclinic and Triclinic Wigner-Seitz 
cells not implemented yet.

 *************
 * Rationale *
 *************/
