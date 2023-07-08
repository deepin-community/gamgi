/*************************************************
 *
 * $GAMGI/src/gtk/layer/gamgi_gtk_layer_create.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_io_token.h"

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
gamgi_layer *layer;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkEntry *entry;
double x[3], z[3], up_z[3];
double length;
const char *name;

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_layer",
"Layer", layer->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Layer Name", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/**********************************************
 * Get projection: perspective or orthogonal. *
 **********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_perspective");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->perspective = TRUE;
else
  layer->perspective = FALSE;

/************************************
 * get projection volume parameters *
 ************************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_top");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->top, 
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Volume Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_near");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->near, 
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Volume Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_far");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->far, 
layer->near + GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Volume Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/***********************
 * get view parameters *
 ***********************/

/*******
 * eye *
 *******/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->eye[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->eye[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->eye[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/**********
 * center *
 **********/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->center[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->center[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->center[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/******
 * up *
 ******/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->up[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->up[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->up[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/************************************************************
 * check view parameters:                                   *
 * 1) the distance eye-center must be in the near-far range *
 * 2) the up component normal to eye-center cannot be zero  *
 ************************************************************/

gamgi_math_vector_sub (layer->eye, layer->center, z);
length = gamgi_math_vector_length (z);
if (length < layer->near || length > layer->far)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }
gamgi_math_vector_normal (z);

length = gamgi_math_vector_dot (layer->up, z);
gamgi_math_vector_scale (z, up_z, length);
gamgi_math_vector_sub (layer->up, up_z, layer->up);
length = gamgi_math_vector_length (layer->up);
if (length < GAMGI_MATH_TOLERANCE_LENGTH)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Parameters", window);
  gamgi_engine_remove_layer (layer);
  return;
  }
gamgi_math_vector_normal (layer->up);

gamgi_math_vector_cross (layer->up, z, x);
gamgi_math_matrix_absolute (layer->referential,
x[0], layer->up[0], z[0], x[1], layer->up[1], z[1], x[2], layer->up[2], z[2]);

/*****************************************
 * Get visibility in: all, partial, none *
 *****************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_in_all");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_in = GAMGI_GTK_ALL;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_in_partial");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_in = GAMGI_GTK_PARTIAL;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_in_none");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_in = GAMGI_GTK_NONE;

/******************************************
 * Get visibility out: all, partial, none *
 ******************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_out_all");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_out = GAMGI_GTK_ALL;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_out_partial");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_out = GAMGI_GTK_PARTIAL;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_out_none");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->visibility_out = GAMGI_GTK_NONE;

/************************
 * Get background color *
 ************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &layer->color[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Layer Color", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &layer->color[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Layer Color", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &layer->color[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Layer Color", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/*************
 * Get scale *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &layer->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_layer (layer);
  return;
  }

/************************
 * show axes: yes or no *
 ************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_yes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  layer->axes = TRUE;
else
  layer->axes = FALSE;

/****************************************************
 * 1) link layer to window                          *
 * 2) set layer as current layer and current object *
 * 3) start undo and save mechanisms                *
 ****************************************************/

gamgi_engine_link_layer_window (layer, window);
gamgi_gtk_object_layer_local (layer, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer, window);
gamgi_gtk_history_start (layer);

gamgi_gtk_dialog_task0_remove (widget, window);
gtk_widget_queue_draw (window->area);
}

static void static_init (gamgi_window *window)
{
gamgi_layer_class *layer_class = gamgi->layer;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/*************************
 * initialize projection *
 *************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_orthographic");
if (gamgi->layer->perspective == FALSE)
  gtk_button_clicked (GTK_BUTTON (button));

/***************************
 * initialize top,near,far *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_top");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->top);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_near");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->near);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_far");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->far);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************************
 * initialize Eye,Center,Up *
 ****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_x");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->eye[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_y");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->eye[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_z");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->eye[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_x");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->center[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_y");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->center[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_z");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->center[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_x");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->up[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_y");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->up[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_z");
sprintf (token, "%.*f", gamgi->gamgi->length, layer_class->up[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*****************************
 * initialize Red,Green,Blue *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_LAYER_R);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_LAYER_G);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_LAYER_B);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************
 * initialize Axes *
 *******************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_no");
gtk_button_clicked (GTK_BUTTON (button));

/********************
 * initialize Scale *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_LAYER_CREATE               *
 *                                                  *
 * Creates the dialog window used to create layers. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_layer_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Layer Create", window);
window->action = GAMGI_GTK_LAYER_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/**************
 * Layer name *
 **************/

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Name");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "entry_layer", entry);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_widget_show (entry);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*******************
 * Projection page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Projection");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * projection *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Type");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Perspective");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_perspective", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Orthographic");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_orthographic", button);
gtk_widget_show (button);

/**********
 * volume *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Top");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Near");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Far");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_top", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_near", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_far", entry);
gtk_widget_show (entry);

/*****************
 * Position page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Position");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

/*******************
 * Eye, Center, Up *
 *******************/

table = gtk_table_new (3, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Eye");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Center");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Up");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_z", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_z", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_z", entry);
gtk_widget_show (entry);
  
/*******************
 * Visibility page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Visibility");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

table = gtk_table_new (2, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Inside");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "All");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
g_object_set_data (G_OBJECT (dialog), "button_in_all", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Partial");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "button_in_partial", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "None");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
g_object_set_data (G_OBJECT (dialog), "button_in_none", button);
gtk_widget_show (button);

label = gtk_label_new ("Outside");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "All");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
g_object_set_data (G_OBJECT (dialog), "button_out_all", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Partial");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "button_out_partial", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "None");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
g_object_set_data (G_OBJECT (dialog), "button_out_none", button);
gtk_widget_show (button);

/*************
 * View page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********
 * Color *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Color");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_blue", entry);
gtk_widget_show (entry);

/***********
 * 2nd row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/********
 * Axes *
 ********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Axes");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Yes");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_yes", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "No");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_no", button);
gtk_widget_show (button);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_scale", entry);
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
