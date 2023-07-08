/***********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_create.h
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

static void static_mass (GtkWidget *widget, void *data);

static void static_radius (GtkWidget *widget, void *data);

static void static_charge (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data);

static void static_default (int number, char *name, gamgi_window *window);

static void static_element (GtkWidget *widget, void *data);

static void static_number (GtkWidget *widget, void *data);

static void static_table (GtkWidget *widget, void *data);

static void static_button (GtkWidget *dialog, char *string);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data);

static void static_reset (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);
  
static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_atom_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_atom_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
