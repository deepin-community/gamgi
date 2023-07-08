/**********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_table.h
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

static void static_clicked (GtkWidget *widget, void *data);

static void static_button (GtkWidget *table, int element, 
int r, int g, int b, int x0, int  y0, int x1, int y1, gamgi_window *window);

static void static_remove (GtkWidget *widget, void *data);

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_atom_table (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
