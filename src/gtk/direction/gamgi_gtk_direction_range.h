/***********************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_range.h
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

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

static void static_start (GtkWidget *widget, void *data);

static void static_end (GtkWidget *widget, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_create (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_direction_range_start (void *data);

void gamgi_gtk_direction_range_cancel (GtkWidget *widget, void *data);

gamgi_bool gamgi_gtk_direction_range_get (gamgi_enum lattice,
gamgi_enum vectors, int *hkl1, int *hkl2, int *start1, int *end1,
int *start2, int *end2);

void gamgi_gtk_direction_range (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
