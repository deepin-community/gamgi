/***********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_dialog.h
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

static gamgi_bool static_browser (char *fullname,
char *shortname, gamgi_window *window);

**********************
 * external functions *
 **********************/

GtkWidget *gamgi_gtk_help_dialog_text (GtkWidget *vbox);

GtkWidget *gamgi_gtk_help_dialog_page (GtkWidget *notebook);

GtkWidget *gamgi_gtk_help_dialog_notebook (GtkWidget *vbox_dialog,
void *data, GtkWidget *dialog);

GtkWidget *gamgi_gtk_help_dialog_window (char *title, gamgi_window *window);

gamgi_bool gamgi_gtk_help_dialog_import (char *title,
char *file, gamgi_window *window, int n_pages, int page, ...);

void gamgi_gtk_help_dialog_tips (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
