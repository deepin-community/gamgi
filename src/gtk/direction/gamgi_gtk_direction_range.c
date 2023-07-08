/***********************************************************
 *
 * $GAMGI/src/gtk/direction/gamgi_gtk_direction_range.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_direction_create.h"
#include "gamgi_gtk_direction_range.h"
#include "gamgi_math_vector.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"
#include "gamgi_io_token.h"

static struct {

/******************************
 * indicates if the structure *
 * is currently being used    *
 ******************************/

gamgi_bool active;

int hkl1[3], hkl2[3];
int start1, end1;
int start2, end2; } range;

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
gamgi_gtk_direction_range_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_start (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *button;
GtkWidget *entry;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_start1");
if (widget == button)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start1");
else
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start2");

/******************************
 * disable/enable order entry *
 ******************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
else gtk_widget_set_sensitive (entry, TRUE);
}

static void static_end (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *button;
GtkWidget *entry;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_end1");
if (widget == button)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end1");
else
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end2");

/******************************
 * disable/enable order entry *
 ******************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
else gtk_widget_set_sensitive (entry, TRUE);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *combo;
double u[3], v[3], cross[3];
double length;
int uvw[3];
gamgi_enum vectors;
const char *name;

/************************************
 * get cell object (to check if     *
 * the two planes are not colinear) *
 ************************************/

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (window->dialog0,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell object", window);
  return;
  }

/************************************
 * get cell vectors (to check if    *
 * the two planes are not colinear) *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "combo_vectors");
vectors = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/****************************************
 * get direction indices uvw (to check  *
 * if the plane indices obey Weiss law) *
 ****************************************/
 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_u");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &uvw[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_v");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &uvw[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "entry_w");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &uvw[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction indices", window);
  return;
  }

/***********************
 * get plane 1 indices *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl1[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl1[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl1[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane indices", window);
  return;
  }

if (gamgi_phys_direction_zero (range.hkl1) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane indices", window);
  return;
  }

/*************************************************************
 * Laue planes are rejected because there are no real planes *
 * equivalent to them. Multiple planes are accepted because  *
 * they correspond always to real planes of multiple order.  *
 * For example, in a BCC lattice, planes (400) between order *
 * 0 and 1 are rejected, but planes (100) between order 0    *
 * and 1 are accepted, because they correspond to real       *
 * planes with Miller indices (200) between order 0 and 2.   *
 *************************************************************/

if (gamgi_phys_plane_laue (cell->lattice, vectors, range.hkl1) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid first plane indices1", window);
  return;
  }

if (gamgi_phys_direction_weiss (uvw, range.hkl1) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "First plane is not in the direction zone", window);
  return;
  }

/******************************************************
 * get plane 1 range: return -INT_MAX, INT_MAX        *
 * when buttons were pressed and entries are disabled *
 ******************************************************/

range.start1 = -INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &range.start1, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane order", window);
  }

range.end1 = INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &range.end1, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane order", window);
  }

if (range.start1 > range.end1)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid first plane order", window);
  }

/***********************
 * get plane 2 indices *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl2[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane indices", window);
  return; 
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl2[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane indices", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &range.hkl2[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane indices", window);
  return;
  }

if (gamgi_phys_direction_zero (range.hkl2) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane indices", window);
  return;
  }

/*************************************************************
 * Laue planes are rejected because there are no real planes *
 * equivalent to them. Multiple planes are accepted because  *
 * they correspond always to real planes of multiple order.  *
 * For example, in a BCC lattice, planes (400) between order *
 * 0 and 1 are rejected, but planes (100) between order 0    *
 * and 1 are accepted, because they correspond to real       *
 * planes with Miller indices (200) between order 0 and 2.   *
 *************************************************************/

if (gamgi_phys_plane_laue (cell->lattice, vectors, range.hkl2) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid second plane indices2", window);
  return;
  }

if (gamgi_phys_direction_weiss (uvw, range.hkl2) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", 
  "Second plane is not in the direction zone", window);
  return;
  }

/******************************************************
 * get plane 2 range: return -INT_MAX, INT_MAX        *
 * when buttons were pressed and entries are disabled *
 ******************************************************/

range.start2 = -INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && 
gamgi_io_token_int_scan (name, &range.start2, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane order", window);
  return;
  }

range.end2 = INT_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_int_scan (name, &range.end2, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane order", window);
  return;
  }

if (range.start2 > range.end2)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid second plane order", window);
  return;
  }

/************************************************************
 * Check that planes hkl1 and hkl2 are not parallel to      *
 * each other, by calculating the cross product of their    *
 * reciprocal vectors. This is slightly more reliable than  *
 * just checking that planes hkl1 and hkl2 are not parallel *
 ************************************************************/

gamgi_phys_plane_vector (cell, range.hkl1, vectors, u);
gamgi_phys_plane_vector (cell, range.hkl2, vectors, v);
gamgi_math_vector_cross (u, v, cross);
length = gamgi_math_vector_length (cross);
if (length < GAMGI_MATH_DIRECTION_RANGE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid parallel planes", window);
  return;
  }

/*****************************************
 * 1) signal that data has been entered  *
 * 2) disable node in first level dialog *
 * 3) close second level dialog          *
 *****************************************/

range.active = TRUE;
gamgi_gtk_direction_create_node (FALSE, window);
gamgi_gtk_dialog_task1_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *button;
char string[2 * GAMGI_ENGINE_TOKEN];

if (range.active == FALSE) return;

/************************
 * show plane 1 indices *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h1");
sprintf (string, "%d", range.hkl1[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k1");
sprintf (string, "%d", range.hkl1[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l1");
sprintf (string, "%d", range.hkl1[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**********************
 * show plane 1 range *
 **********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_start1");
if (range.start1 == -INT_MAX) gtk_button_clicked (GTK_BUTTON (button));
else
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start1");
  sprintf (string, "%d", range.start1);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_end1");
if (range.end1 == INT_MAX) gtk_button_clicked (GTK_BUTTON (button));
else
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end1");
  sprintf (string, "%d", range.end1);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/************************
 * show plane 2 indices *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_h2");
sprintf (string, "%d", range.hkl2[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_k2");
sprintf (string, "%d", range.hkl2[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_l2");
sprintf (string, "%d", range.hkl2[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**********************
 * show plane 2 range *
 **********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_start2");
if (range.start2 == -INT_MAX) gtk_button_clicked (GTK_BUTTON (button));
else
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_start2");
  sprintf (string, "%d", range.start2);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_end2");
if (range.end2 == INT_MAX) gtk_button_clicked (GTK_BUTTON (button));
else
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_end2");
  sprintf (string, "%d", range.end2);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

}

static void static_create (gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *vbox_dialog, *vbox_frame;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *frame;

dialog = gamgi_gtk_dialog_task1_create ("Direction Range", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/***********
 * Plane 1 *
 ***********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Family");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 10);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/********************
 * Indices 1: h,k,l *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Plane");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("H");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_h1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_k1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_l1", entry);
gtk_widget_show (entry);

/***********************
 * Range 1: Start, End *
 ***********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Order");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_start1", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Start");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_start), window);
g_object_set_data (G_OBJECT (dialog), "button_start1", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_end1", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("End");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_end), window);
g_object_set_data (G_OBJECT (dialog), "button_end1", button);
gtk_widget_show (button);

/***********
 * Plane 2 *
 ***********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Family");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 10);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/********************
 * Indices 2: h,k,l *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Plane");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("H");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_h2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_k2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_l2", entry);
gtk_widget_show (entry);

/***********************
 * Range 2: Start, End *
 ***********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Order");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_start2", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Start");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_start), window);
g_object_set_data (G_OBJECT (dialog), "button_start2", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_end2", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("End");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_end), window);
g_object_set_data (G_OBJECT (dialog), "button_end2", button);
gtk_widget_show (button);

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
G_CALLBACK (gamgi_gtk_direction_range_cancel), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}

void gamgi_gtk_direction_range_start (void *data)
{
range.active = FALSE;
}

void gamgi_gtk_direction_range_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

gamgi_gtk_dialog_task1_remove (widget, window);

/**********************************
 * if no data has been submited,  *
 * the default is the node button *
 **********************************/

if (range.active == FALSE)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_node");
  gtk_button_clicked (GTK_BUTTON (button));
  }
}

gamgi_bool gamgi_gtk_direction_range_get (gamgi_enum lattice,
gamgi_enum vectors, int *hkl1, int *hkl2, int *start1, int *end1,
int *start2, int *end2)
{
int hkl_min[3];
int factor;

/***************************************************
 * return FALSE if range information is not active *
 ***************************************************/

if (range.active == FALSE) return FALSE;

/*********************
 * export range data *
 *********************/

/*****************************************************
 * the range of directions is defined by the Miller  * 
 * indices, start and end order, for two plane       *
 * families belonging to the direction zone, thus    *
 * defining a closed region around the direction     *
 *                                                   *
 * multiple planes are accepted, so for example, in  *
 * a BCC lattice, a (100) plane is accepted, after   *
 * being converted to (200) (the correct indices).   *
 * The order range must also be scaled by the same   *
 * factor, so in the same example, a order range     *
 * 0-1 will be converted to 0-2, thus defining twice *
 * the planes at half the distance from each other.  *
 *****************************************************/

*start1 = range.start1;
*end1 = range.end1;
*start2 = range.start2;
*end2 = range.end2;

gamgi_phys_direction_decrease (range.hkl1, hkl_min);
factor = gamgi_phys_plane_increase (lattice, hkl_min, vectors, hkl1);
if (gamgi_phys_direction_higher (hkl1, range.hkl1) == TRUE)
  {
  if (*start1 != -INT_MAX) *start1 *= factor;
  if (*end1 != INT_MAX) *end1 *= factor;
  }

gamgi_phys_direction_decrease (range.hkl2, hkl_min);
factor = gamgi_phys_plane_increase (lattice, hkl_min, vectors, hkl2);
if (gamgi_phys_direction_higher (hkl2, range.hkl2) == TRUE)
  {
  if (*start2 != -INT_MAX) *start2 *= factor;
  if (*end2 != INT_MAX) *end2 *= factor;
  }

return TRUE;
}

void gamgi_gtk_direction_range (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_range");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /******************************
   * create second level dialog *
   ******************************/

  static_create (window);
  }
else
  {
  /**********************************
   *   remove second level dialog   *
   *                                *
   * restart Node data, if disabled *
   **********************************/

  if (window->dialog1 != NULL)
    gamgi_gtk_dialog_task1_remove (widget, window);

  if (range.active == TRUE)
    gamgi_gtk_direction_create_node (TRUE, window);
  range.active = FALSE;
  }
}
