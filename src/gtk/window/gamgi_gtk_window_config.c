/***************************************************
 *
 * $GAMGI/src/gtk/window/gamgi_gtk_window_config.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_math_vector.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

/************ internal function **********
 *                                       *
 *               STATIC_OK               *
 *                                       *
 *                                       *
 *****************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_window_class *window_class, *window_class_old;
GtkWidget *entry;
const char *name;

window_class = gamgi_global_copy_window (gamgi->window);

/************************
 * get ruler parameters *
 ************************/

/**********
 * rotate *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_rotate_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->rotate_max, 
GAMGI_MATH_TOLERANCE_ANGLE, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Rotate Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_rotate_step");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->rotate_step, 
0.0, window_class->rotate_max) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Rotate Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

/*********
 * move *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_move_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->move_max,
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Move Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_move_step");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->move_step,
0.0, window_class->move_max) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Move Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

/*********
 * scale *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->scale_max,
1.0 + GAMGI_MATH_TOLERANCE, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale_step");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &window_class->scale_step, 
0.0, window_class->scale_max - 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Parameters", window);
  gamgi_global_remove_window (window_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

window_class_old = gamgi->window;
gamgi->window = window_class;
gamgi_global_remove_window (window_class_old);

/*****************
 * Remove dialog *
 *****************/

gamgi_gtk_dialog_task0_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
gamgi_window_class *window_class = gamgi->window;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/*******************************
 * initialise ruler parameters *
 *******************************/
 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_rotate_max");
sprintf (token, "%.*f", gamgi->gamgi->angle, window_class->rotate_max);
gtk_entry_set_text (GTK_ENTRY (entry), token);
 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_rotate_step");
sprintf (token, "%.*f", gamgi->gamgi->angle, window_class->rotate_step);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_move_max");
sprintf (token, "%.*f", gamgi->gamgi->length, window_class->move_max);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_move_step");
sprintf (token, "%.*f", gamgi->gamgi->length, window_class->move_step);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale_max");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, window_class->scale_max);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale_step");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, window_class->scale_step);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_LAYER_CREATE               *
 *                                                  *
 * Creates the dialog window used to create layers. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_window_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Window Config", window);
window->action = GAMGI_GTK_WINDOW_CONFIG;
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

/**************
 * Ruler page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Ruler ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/***************************
 * Ruler amplitudes, steps *
 ***************************/

table = gtk_table_new (3, 4, FALSE);
gtk_box_pack_start (GTK_BOX (vbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Rotate");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("Move");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("Scale");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("Max");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Step");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_rotate_max", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_rotate_step", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_move_max", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_move_step", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_scale_max", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_scale_step", entry);
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
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
