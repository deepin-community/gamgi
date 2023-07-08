/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_gamgi_about.c
 *
 * Copyright (C) 2004 Carlos Pereira
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

/************** external function ****************
 *                                               *
 *            GAMGI_GTK_GAMGI_ABOUT              *
 *                                               *
 * Shows Gamgi version, authors, and legalities. *
 *                                               *
 *************************************************/

void gamgi_gtk_gamgi_about (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *label;
char string[GAMGI_ENGINE_LINE];

/******************
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Gamgi About", window);
window->action = GAMGI_GTK_GAMGI_ABOUT;

vbox = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
gtk_widget_show (vbox);

sprintf (string, "GAMGI: General Atomistic Modelling Graphic Interface");
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

sprintf (string, "Website: %s", GAMGI_IO_WEBSITE);
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

sprintf (string, "License: %s", GAMGI_IO_LICENSE);
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

sprintf (string, "Version: %s", GAMGI_IO_VERSION);
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

sprintf (string, "Date: %s", GAMGI_IO_DATE);
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

sprintf (string, "Authors: %s", GAMGI_IO_AUTHORS);
label = gtk_label_new (string);
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}
