/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_history.h
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

void static_reset (gamgi_dlist *dlist, int max_levels);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_history_update (gamgi_layer *layer,
gamgi_dlist *dlist, int max_levels);

/**********************
 * internal functions *
 **********************

static void static_label (GtkWidget *label, int number);

static void static_button (GtkWidget *button, int number);

static void static_count (gamgi_dlist *dlist_now, int *backward, int *forward);

static void static_update (gamgi_window *window);

static void static_swap (gamgi_layer *layer_old, gamgi_layer *layer_new);

static void static_backward (GtkWidget *widget, void *data);

static void static_forward (GtkWidget *widget, void *data);

static void static_ok (GtkWidget *widget, void *data);

static void static_cancel (GtkWidget *widget, void *data);

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

static void static_create (gamgi_window *window, gamgi_char *name, int max);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_history_start (gamgi_layer *layer);

void gamgi_gtk_history_layer_reset (gamgi_dlist *dlist);

void gamgi_gtk_history_window_reset (gamgi_dlist *dlist);

void gamgi_gtk_history_gamgi_reset (void);

void gamgi_gtk_history_undo (GtkWidget *widget, void *data);

void gamgi_gtk_history_save (GtkWidget *widget, void *data);


/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
