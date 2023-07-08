/*****************************************************
 *
 * $GAMGI/src/gtk/cluster/gamgi_gtk_cluster_create.c
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_cluster_link.h"
#include "gamgi_io_token.h"

/******************* internal function ****************
 *                                                    *
 *                        STATIC_OK                   *
 *                                                    *
 * This is the function that actually creates a new   *
 * cluster. It is triggered when the user presses Ok. *
 *                                                    *
 ******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_cluster *cluster;
GtkEntry *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

cluster = gamgi_engine_create_cluster ();
gamgi_engine_start_cluster (cluster);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_cluster",
"Cluster", cluster->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cluster Name", window);
  gamgi_engine_remove_cluster (cluster);
  return;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_cluster_layer (cluster, window->layer);

/*************************************
 *  launch a Cluster Link dialog     *
 *  containing already the name and  *
 *  number of the new cluster.       *
 *                                   *
 *   The old dialog is removed       *
 *   before creating the new one     *
 *                                   *
 * no need to redraw gl_area because *
 * an empty cluster is invisible     *
 *************************************/

gamgi_gtk_cluster_link (NULL, window);

/************************************
 * Insert cluster's name and number *
 *  in new dialog's cluster entry   *
 ************************************/

dialog = window->dialog0;
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_cluster");
sprintf (string, "%s %d", cluster->object.name, cluster->object.number);
gtk_entry_set_text (entry, string);
}


/******************** external function *****************
 *                                                      *
 *                 GAMGI_GTK_CLUSTER_CREATE             *
 *                                                      *
 * Creates the dialog window used to create assemblies. *
 *                                                      *
 ********************************************************/

void gamgi_gtk_cluster_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox, *hbox1;
GtkWidget *vbox;

/******************
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Cluster Create", window);
window->action = GAMGI_GTK_CLUSTER_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

/****************
 * Cluster name *
 ****************/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

hbox1 = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox), hbox1, FALSE, FALSE, 0);
gtk_widget_show (hbox1);

label = gtk_label_new ("Name");
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox1), entry, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "entry_cluster", entry);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_widget_show (entry);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}
