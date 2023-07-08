/*******************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_list.h
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static gamgi_bool static_range_increase (int *new, int *range);

static gamgi_bool static_range_decrease (int value, int *range_new);

static void static_allocate_increase (int *range);

static void static_allocate_decrease (int *range);

static void static_add (int *range_new, gamgi_bool value);

static gamgi_bool static_get (int *new, gamgi_window *window);

static gamgi_bool static_update (int value, gamgi_window *window);

static void static_row (GtkListStore *store, int u, int v, int w);

static void static_show (gamgi_window *window);

static void static_hide (gamgi_window *window);

static void static_apply (int value, gamgi_window *window);

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

static void static_include (GtkWidget *widget, void *data);

static void static_exclude (GtkWidget *widget, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_create (gamgi_window *window);

static void static_direction (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum structure, gamgi_enum filter,
double thickness, gamgi_dlist *dlist, gamgi_enum pole, gamgi_enum trace);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_direction_list_create (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum structure, gamgi_enum filter, double thickness,
gamgi_dlist *dlist, gamgi_enum pole, gamgi_enum trace);

void gamgi_gtk_direction_list_start (gamgi_window *window);

void gamgi_gtk_direction_list_cancel (GtkWidget *widget, void *data);

void gamgi_gtk_direction_list_destroy (GtkWidget *widget, void *data);

void gamgi_gtk_direction_list (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Last update: January 2012

 *************
 * Rationale *
 *************/
