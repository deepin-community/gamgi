/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_statusbar.h
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

static void static_show (GtkWidget *statusbar, int context, char *text);

static void static_align (GtkWidget *statusbar, int context, char *text);

static void static_timer (int *timer);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_statusbar_total (gamgi_window *window);

void gamgi_gtk_statusbar_color (GtkWidget *label, 
char *color_name, gamgi_window *window);

gamgi_bool gamgi_gtk_statusbar_message_timeout (void *data);

gamgi_bool gamgi_gtk_statusbar_focus_timeout (void *data);

gamgi_bool gamgi_gtk_statusbar_layer_timeout (void *data);

gamgi_bool gamgi_gtk_statusbar_time_timeout (void *data);

void gamgi_gtk_statusbar_message_show (gamgi_window *window, 
char *text, char *color, int time);

void gamgi_gtk_statusbar_focus_show (gamgi_window *window);

void gamgi_gtk_statusbar_layer_show (gamgi_window *window);

gamgi_bool gamgi_gtk_statusbar_progress (void *data);

void gamgi_gtk_statusbar_mode (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented, cleaned.

 *************
 * Rationale *
 *************/
