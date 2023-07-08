/************************************************
 *
 * $GAMGI/src/gtk/file/gamgi_gtk_file_chooser.h
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static int static_dir (char *fullname);

static gamgi_bool static_timer (void *data);

static gamgi_bool static_hide_folders (char *folder, gamgi_window *window);

static gamgi_bool static_show_folders (char *folder, gamgi_window *window);

static gamgi_bool static_hide_files (char *file, gamgi_window *window);

static gamgi_bool static_show_files (char *file, gamgi_window *window);

static void static_list_folders (const char *path, void *data);

static void static_list_files (const char *path, void *data);

static void static_update (char *path, gamgi_window *window);

static void static_select_folders (GtkWidget *widget, void *data);

static void static_select_files (GtkWidget *widget, void *data);

static void static_places (GtkWidget *widget, void *data);

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_gtk_file_chooser_ok (gamgi_window *window, char *fullname);

void gamgi_gtk_file_chooser_init (gamgi_window *window);

void gamgi_gtk_file_chooser_location (GtkWidget *vbox_page, gamgi_window *window);

void gamgi_gtk_file_chooser_filters (GtkWidget *vbox_page, gamgi_window *window);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
