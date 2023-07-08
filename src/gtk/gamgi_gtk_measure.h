/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_measure.h
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

static void static_reciprocal_bravais (GtkWidget *widget, void *data);

static void static_reciprocal_start (gamgi_layer *layer_old, 
gamgi_layer *layer);

static gamgi_layer *static_reciprocal_create (gamgi_layer *layer_old,
gamgi_window *window);

static void static_voronoi_color (GtkWidget *widget, void *data);

static void static_voronoi_faces (GtkWidget *widget, void *data);

static void static_voronoi_layer (gamgi_cluster *cluster, gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_measure_geometry_ok (gamgi_object *object, gamgi_window *window);

void gamgi_gtk_measure_symmetry_ok (gamgi_object *object, gamgi_window *window);

void gamgi_gtk_measure_symmetry_init (gamgi_window *window);

void gamgi_gtk_measure_symmetry (GtkWidget *notebook, gamgi_window *window);

void gamgi_gtk_measure_reciprocal_ok (gamgi_object *object, gamgi_window *window);

void gamgi_gtk_measure_reciprocal_init (gamgi_window *window);

void gamgi_gtk_measure_reciprocal (GtkWidget *notebook, gamgi_window *window);

void gamgi_gtk_measure_voronoi_ok (gamgi_object *object, gamgi_window *window);

void gamgi_gtk_measure_voronoi_init (gamgi_window *window);

void gamgi_gtk_measure_voronoi (GtkWidget *notebook, gamgi_window *window);

/*************
 * Rationale *
 *************

In development.

 *************
 * Rationale *
 *************/
