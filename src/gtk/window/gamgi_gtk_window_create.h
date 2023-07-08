/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_window.h
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

static void static_menu_position (GtkMenu *menu, 
int *x, int *y, gamgi_bool *push, void *data);

static int static_menu (GtkWidget *button, GdkEventButton *event, void *data);

static GtkWidget *static_item_create (gamgi_char *string_label, 
gamgi_char *string_color);

static GtkWidget *static_item_insert (GtkWidget *menu, gamgi_char *string_item,
gamgi_char *string_color, gamgi_event2 handler_function, gamgi_window *window);

static void static_button_menu (GtkWidget *table, GtkWidget *button,
int x0, int  y0, int  x1, int y1);

static GtkWidget *static_button_tool (GtkWidget *hbox, gamgi_char *name,
gamgi_char *color, gamgi_callback2 handler_function, gamgi_window *window);

static int static_delete (GtkWidget *widget, GdkEventButton *event, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_create (GtkWidget *widget, void *data);

static double static_ruler_position (double max,
double step, gamgi_window *window);

static void static_ruler_start (gamgi_window *window);

static void static_ruler_press (GtkWidget *widget,
GdkEventButton *event, void *data);

static void static_ruler_motion (GtkWidget *widget,
GdkEventMotion *event, void *data);

static void static_ruler_on (gamgi_window *window, gamgi_enum mode);

static void static_ruler_off (gamgi_window *window);

static void static_axes (GtkWidget *widget, void *data);

static void static_rotate (GtkWidget *widget, void *data)

static void static_move (GtkWidget *widget, void *data)

static void static_scale (GtkWidget *widget, void *data)

static void static_console (GtkWidget *widget, void *data)

static void static_shell (GtkWidget *widget, void *data)

 **********************
 * external functions *
 **********************/

void gamgi_gtk_window_create (gamgi_window *window, gamgi_layer *layer);

void gamgi_gtk_window_create_menus (gamgi_window *window);

void gamgi_gtk_window_create_progress (gamgi_window *window);

void gamgi_gtk_window_create_data (gamgi_window *window);

gamgi_layer *gamgi_gtk_window_create_layer (gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented but needs cleaning.

 *************
 * Rationale *
 *************/
