/***************************************************
 *
 * $GAMGI/src/gtk/plane/gamgi_gtk_plane_node.h
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

static void static_nodes (gamgi_cell *cell, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_create (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_plane_node_start (void *data);

void gamgi_gtk_plane_node_cancel (GtkWidget *widget, void *data);

gamgi_bool gamgi_gtk_plane_node_get (gamgi_cell *cell, 
int *o1, int *o2, int *o3, gamgi_window *window);

void gamgi_gtk_plane_node (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Last update: 06-09-2006

 *************
 * Rationale *
 *************/
