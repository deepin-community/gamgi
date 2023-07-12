/***********************************************
 *
 * $GAMGI/src/gtk/file/gamgi_gtk_file_export.c
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_file_chooser.h"
#include "gamgi_io_token.h"
#include "gamgi_io_file.h"

static void static_write (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
char fullname[GAMGI_IO_FULLNAME];

if (gamgi->question != NULL)
  gamgi_gtk_dialog_remove (gamgi->question, &gamgi->question);

/***************************************
 * export data: when errors are found, *
 * show error messages and keep dialog *
 ***************************************/

if (gamgi_gtk_file_chooser_ok (window, fullname) == FALSE) return;
if (gamgi_io_file_write (fullname, window) == FALSE) return;

/*****************
 * Remove dialog *
 *****************/

gamgi_gtk_dialog_task0_remove (NULL, window);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
char fullname[GAMGI_IO_FULLNAME];

gamgi_gtk_file_chooser_ok (window, fullname);

gamgi_io_file_overwrite (fullname, static_write, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

gamgi_gtk_file_chooser_init (window);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_http");
gtk_widget_set_sensitive (button, FALSE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_ftp");
gtk_widget_set_sensitive (button, FALSE);
}

void gamgi_gtk_file_export (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *hbox_center;
GtkWidget *vbox_dialog;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("File Export", window);
window->action = GAMGI_GTK_FILE_EXPORT;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*****************
 * Location page *
 *****************/

gamgi_gtk_file_chooser_location (notebook, window);

/****************
 * Filters page *
 ****************/

gamgi_gtk_file_chooser_filters (notebook, window);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
