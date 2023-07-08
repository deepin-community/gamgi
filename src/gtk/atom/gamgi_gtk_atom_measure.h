/*************************************************
 *
 * $GAMGI/src/gtk/atome/gamgi_gtk_atom_measure.h
 *
 * Copyright (C) Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static GtkWidget *static_status (gamgi_window *window, int *page, int *property);

static gamgi_enum static_class (gamgi_window *window);

static GtkWidget *static_entry (GtkWidget *vbox, int page, int property);

static gamgi_plane *static_plane (GtkWidget *vbox,
char *id, gamgi_window *window);

static gamgi_direction *static_direction (GtkWidget *vbox,
char *id, gamgi_window *window);

static gamgi_atom *static_atom (GtkWidget *vbox,
char *id, gamgi_window *window);

static void static_reset (GtkWidget *vbox, int page);

static void static_property (GtkWidget *vbox,
int page, int property, gamgi_window *window);

static void static_activate (GtkWidget *widget, void *data);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *notebook_page, int page, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_atom_measure_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_atom_measure (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Started.

 *************
 * Rationale *
 *************/
