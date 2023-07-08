/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_gamgi_config.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

/************ internal function **********
 *                                       *
 *               STATIC_OK               *
 *                                       *
 *  This is the function that creates a  *
 *  new layer, when the user presses Ok. *
 *                                       *
 *****************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_gamgi_class *gamgi_class, *gamgi_class_old;
GtkWidget *button;
GtkWidget *entry;
const char *name;

gamgi_class = gamgi_global_copy_gamgi (gamgi->gamgi);

/***************************
 * get hardware parameters *
 ***************************/

/********
 * Beep *
 ********/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_beep");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  gamgi_class->beep = TRUE;
else
  gamgi_class->beep = FALSE;

/***********
 * button1 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &gamgi_class->button1, 1, 3) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Button Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }
else gamgi_class->button1 = gamgi_gtk_dialog_button_mask (gamgi_class->button1);

/***********
 * button2 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &gamgi_class->button2, 1, 3) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Button Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }
else gamgi_class->button2 = gamgi_gtk_dialog_button_mask (gamgi_class->button2);

/***********
 * button3 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &gamgi_class->button3, 1, 3) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Button Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }
else gamgi_class->button3 = gamgi_gtk_dialog_button_mask (gamgi_class->button3);

/******************
 * Pick tolerance *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tolerance");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name,
&gamgi_class->tolerance, 0, GAMGI_MESA_PICK_TOLERANCE_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Tolerance Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/****************************
 * get precision parameters *
 ****************************/

/**********
 * length *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_length");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, 
&gamgi_class->length, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Precision Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }


/*********
 * angle *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, 
&gamgi_class->angle, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Precision Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/********
 * mass *
 ********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, 
&gamgi_class->mass, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Precision Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/**********
 * charge *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, 
&gamgi_class->charge, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Precision Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/************************
 * get color parameters *
 ************************/

/**************
 * background *
 **************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, 
&gamgi_class->background[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name,
&gamgi_class->background[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name,
&gamgi_class->background[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/**************
 * foreground *
 **************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name,
&gamgi_class->foreground[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name,
&gamgi_class->foreground[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name,
&gamgi_class->foreground[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/*********
 * title *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->title[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->title[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->title[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/********
 * bold *
 ********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->bold[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->bold[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->bold[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/********
 * link *
 ********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->link[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->link[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &gamgi_class->link[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_gamgi (gamgi_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

gamgi_class_old = gamgi->gamgi;
gamgi->gamgi = gamgi_class;
gamgi_global_remove_gamgi (gamgi_class_old);

/*****************
 * Remove dialog *
 *****************/

gamgi_gtk_dialog_task0_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
gamgi_gamgi_class *gamgi_class = gamgi->gamgi;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/**********************************
 * initialise mouse buttons 1,2,3 *
 **********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button1");
sprintf (token, "%d", gamgi_gtk_dialog_button_unmask (gamgi_class->button1));
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button2");
sprintf (token, "%d", gamgi_gtk_dialog_button_unmask (gamgi_class->button2));
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_button3");
sprintf (token, "%d", gamgi_gtk_dialog_button_unmask (gamgi_class->button3));
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***********************************
 * initialise mouse pick tolerance *
 ***********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_tolerance");
sprintf (token, "%d", gamgi_class->tolerance);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*************************
 * initialise beep sound *
 *************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_beep");
if (gamgi_class->beep == TRUE)
  gtk_button_clicked (GTK_BUTTON (button));

/****************************************
 * initialise decimal precision figures *
 ****************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_length");
sprintf (token, "%d", gamgi_class->length);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
sprintf (token, "%d", gamgi_class->angle);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
sprintf (token, "%d", gamgi_class->mass);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
sprintf (token, "%d", gamgi_class->charge);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/**************************
 * initialise title color *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************************
 * initialise foreground color *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/**************************
 * initialise title color *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_title_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*************************
 * initialise bold color *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bold_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*************************
 * initialise link color *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_link_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_LAYER_CREATE               *
 *                                                  *
 * Creates the dialog window used to create layers. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_gamgi_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Gamgi Config", window);
window->action = GAMGI_GTK_GAMGI_CONFIG;
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

/******************
 * Usability page *
 ******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Usability");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***********************
 * mouse: button order *
 ***********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

table = gtk_table_new (2, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Buttons");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("2");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("3");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_button1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_button2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_button3", entry);
gtk_widget_show (entry);

/*************************
 * mouse: pick tolerance *
 *************************/

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Tolerance");
gtk_table_attach (GTK_TABLE (table), label, 0, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Pixels");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_tolerance", entry);
gtk_widget_show (entry);

/********
 * Beep *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gtk_toggle_button_new_with_label (" Sound ");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_beep", button);
gtk_widget_show (button);

/************************************
 * Precision page (decimal figures) *
 ************************************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Precision"); 
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (3, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Digits");
gtk_table_attach (GTK_TABLE (table), label, 0, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Length");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Angle");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Mass");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Charge");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_length", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_angle", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_mass", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_charge", entry);
gtk_widget_show (entry);

/**************
 * Color page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Color ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/******************
 * GtkText colors *
 ******************/

table = gtk_table_new (4, 6, FALSE);
gtk_box_pack_start (GTK_BOX (vbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Base");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Text");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Title");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Bold");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Link");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

/**************
 * background *
 **************/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_b", entry);
gtk_widget_show (entry);

/**************
 * foreground *
 **************/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_text_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_text_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_text_b", entry);
gtk_widget_show (entry);

/*********
 * title *
 *********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_title_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_title_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_title_b", entry);
gtk_widget_show (entry);
  
/********
 * bold *
 ********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_bold_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_bold_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_bold_b", entry);
gtk_widget_show (entry);

/********
 * link *
 ********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_link_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_link_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_link_b", entry);
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
