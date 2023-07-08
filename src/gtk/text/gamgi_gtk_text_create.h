/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_create.h
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

static void static_style (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);
  
static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_text_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_text_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
