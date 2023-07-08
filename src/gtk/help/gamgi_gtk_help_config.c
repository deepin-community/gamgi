/***********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_config.c
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
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_help_class *help_class, *help_class_old;
GtkWidget *button;
GtkWidget *entry;
char token[GAMGI_IO_PATHNAME];
const char *name;

help_class = gamgi_global_copy_help (gamgi->help);

/************************************
 * source: local(in) or remote(out) *
 ************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_remote");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  help_class->source_in = FALSE;
else
  help_class->source_in = TRUE;

/************************************
 * agent: gamgi(in) or browser(out) *
 ************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_browser");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  help_class->agent_in = FALSE;
else
  help_class->agent_in = TRUE;

/**********************************
 * local,remote,browser pathnames *
 **********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_local");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, token, 
GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Local Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }
else
  {
  gamgi_io_token_remove (&help_class->local);
  gamgi_io_token_create (token, &help_class->local);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_remote");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, token, 
GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Remote Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }
else
  {
  gamgi_io_token_remove (&help_class->remote);
  gamgi_io_token_create (token, &help_class->remote);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_browser");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, token, 
GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Browser Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }
else
  {
  gamgi_io_token_remove (&help_class->browser);
  gamgi_io_token_create (token, &help_class->browser);
  }

/***************************************************
 * Check if local == NULL when source_in is TRUE   *
 * Check if remote == NULL when source_in is FALSE *
 * Check if browser == NULL when agent_in is FALSE *
 ***************************************************/

if (help_class->source_in == TRUE && help_class->local == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Local Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }

if (help_class->source_in == FALSE && help_class->remote == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Remote Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }

if (help_class->agent_in == FALSE && help_class->browser == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Browser Data", window);
  gamgi_global_remove_help (help_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

help_class_old = gamgi->help;
gamgi->help = help_class;
gamgi_global_remove_help (help_class_old);

/*****************
 * Remove dialog *
 *****************/

gamgi_gtk_dialog_task0_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
gamgi_help_class *help_class = gamgi->help;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;

/********************
 * initialize paths *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_local");
if (help_class->local == NULL)
  gtk_entry_set_text (GTK_ENTRY (entry), "");
else
  gtk_entry_set_text (GTK_ENTRY (entry), help_class->local);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_remote");
if (help_class->remote == NULL)
  gtk_entry_set_text (GTK_ENTRY (entry), "");
else
  gtk_entry_set_text (GTK_ENTRY (entry), help_class->remote);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_browser");
if (help_class->browser == NULL)
  gtk_entry_set_text (GTK_ENTRY (entry), "");
else
  gtk_entry_set_text (GTK_ENTRY (entry), help_class->browser);

/***********************************************
 * initialize source: local(in) or remote(out) *
 ***********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_remote");
if (help_class->source_in == FALSE)
  gtk_button_clicked (GTK_BUTTON (button));

/***********************************************
 * initialize agent: gamgi(in) or browser(out) *
 ***********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_browser");
if (help_class->agent_in == FALSE)
  gtk_button_clicked (GTK_BUTTON (button));
}

/***************** external function *******************
 *                                                     *
 *                GAMGI_GTK_HELP_CONFIG                *
 *                                                     *
 *******************************************************/

void gamgi_gtk_help_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *button;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Help Config", window);
window->action = GAMGI_GTK_HELP_CONFIG;
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

/***************
 * Source page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Source");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********
 * Local *
 *********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Local");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_50);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_IO_PATHNAME - 1);
g_object_set_data (G_OBJECT (dialog), "entry_local", entry);
gtk_widget_show (entry);

/**********
 * Remote *
 **********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Remote");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_remote", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_50);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_IO_PATHNAME - 1);
g_object_set_data (G_OBJECT (dialog), "entry_remote", entry);
gtk_widget_show (entry);

/**************
 * Agent page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Agent");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/******************
 * Agent, Browser *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Gamgi");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Browser");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_browser", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_40);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_IO_PATHNAME - 1);
g_object_set_data (G_OBJECT (dialog), "entry_browser", entry);
gtk_widget_show (entry);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);

static_init (window);
gtk_widget_show (dialog);
}
