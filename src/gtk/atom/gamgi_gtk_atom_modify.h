/***********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_modify.h
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

 ******************
 * data functions *
 ****************** 

static void static_name (gamgi_window *window, char *name);

static void static_absolute (GtkWidget *dialog);

static void static_relative (GtkWidget *dialog);

static void static_origin (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data);

static void static_default (int number, char *name, gamgi_window *window);

static void static_element (GtkWidget *widget, void *data);

static void static_number (GtkWidget *widget, void *data);

static void static_table (GtkWidget *widget, void *data);

static void static_button (GtkWidget *dialog, char *string);

static void static_reset_data (gamgi_window *window);

static void static_switch_data (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data);

static void static_ok_data (GtkWidget *widget, void *data);

static void static_press_data (gamgi_object *object, void *data);

 **********************
 * geometry functions *
 **********************

static GtkWidget *static_status (gamgi_window *window, int *page, int *quantity);

static gamgi_enum static_class (gamgi_window *window);

static GtkWidget *static_entry (GtkWidget *vbox, 
int page, int property, gamgi_window *window);

static gamgi_plane *static_plane (GtkWidget *widget,
char *id, gamgi_window *window);

static gamgi_direction *static_direction (GtkWidget *widget,
char *id, gamgi_window *window);

static gamgi_atom *static_atom (GtkWidget *widget,
char *id, gamgi_window *window);

static void static_reset_geometry (GtkWidget *vbox,
int page, int property, gamgi_window *window);

static void static_quantity (GtkWidget *vbox,
int page, int property, gamgi_window *window);

static void static_activate (GtkWidget *widget, void *data);
  
static void static_switch_geometry (GtkNotebook *notebook, 
GtkNotebookPage *notebook_page, int page, void *data);

static void static_ok_geometry (GtkWidget *widget, void *data);

static void static_press_geometry (gamgi_object *object, void *data);

 ********************
 * common functions *
 ********************

static void static_list (GtkWidget *widget, void *data);

static void static_mode (GtkWidget *widget, void *data);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data);

static gamgi_bool static_grab (gamgi_window *window);

static gamgi_bool static_apply (gamgi_atom *atom, gamgi_window *window);

static void static_ok_global (gamgi_window *window);

static void static_ok_local (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_atom_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_atom_modify (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned. Global mode not implemented yet.

 *************
 * Rationale *
 *************/

