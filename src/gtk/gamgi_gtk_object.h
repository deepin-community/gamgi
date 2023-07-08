/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_object.h
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

gamgi_object *static_address (gamgi_enum class, 
gamgi_window *window, long number);

gamgi_object *static_object (GtkWidget *host, char *tag,
gamgi_enum class, gamgi_window *window, char *name);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_gtk_object_name (GtkWidget *host,
char *tag, char *label, char *name);

gamgi_object *gamgi_gtk_object_number (GtkWidget *host,
char *tag, gamgi_enum class, gamgi_window *window, char *name);

gamgi_object *gamgi_gtk_object_name_number (GtkWidget *host,
char *tag, gamgi_enum class, gamgi_window *window);

void gamgi_gtk_object_layer_local (gamgi_layer *layer, gamgi_window *window);

void gamgi_gtk_object_focus_local (gamgi_object *object,
gamgi_window *window_dialog);

void gamgi_gtk_object_focus_global (gamgi_window *window_dialog);

gamgi_layer *gamgi_gtk_object_layer_create (gamgi_window *window);

gamgi_layer *gamgi_gtk_object_layer_current (gamgi_layer *layer,
gamgi_window *window);

void gamgi_gtk_object_raise (gamgi_layer *layer, gamgi_window *window);

void gamgi_gtk_object_link (gamgi_object *child,
gamgi_object *parent, gamgi_window *window);

void gamgi_gtk_object_copy (gamgi_object *child,
gamgi_object *parent, gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
