/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_select.h
 *
 * Copyright (C) 2004, 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_destroy (GtkWidget *widget, void *data);

static void static_model (gamgi_dlist *dlist, gamgi_window *window);

static gamgi_dlist *static_dlist (int scope, gamgi_window *window);

static void static_scope (GtkWidget *widget, void *data);

static int static_font (gamgi_enum font);

static void static_method_style (char *label, gamgi_window *window);

static void static_method_font (char *label, gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window);

static gamgi_dlist *static_ok_contents (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e1 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e2 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_e3 (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_style (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_font (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_red (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_green (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_blue (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_scale (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_text_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_text_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Nov 2009.

 *************
 * Rationale *
 *************/
