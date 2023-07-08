/*************************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_align.c
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
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_mesa_select.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_cell.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"
#include "gamgi_io_token.h"

static void static_rotation_hkl (GtkWidget *widget, void *data);
static void static_rotation_uvw (GtkWidget *widget, void *data);

static struct {
double cell_xyz[3];
gamgi_enum cell_vectors;

gamgi_object *base;
double base_xyz[3];
gamgi_enum base_vectors; } translation;

static struct {
int cell_hkl[3];
int cell_uvw[3];
gamgi_enum cell_vectors;

gamgi_object *base;
int base_hkl[3];
int base_uvw[3];
gamgi_enum base_vectors;
gamgi_enum plane; } rotation;

static void static_translation_remove (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/********************************************
 * this function presses the level 0 button *
 * Align, that closes the level 1 dialog    *
 ********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation");
gtk_button_clicked (GTK_BUTTON (button));
}

static void static_translation_enable (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/*************************************************
 * This function enables again level 1 widgets   *
 * that may have been previously disabled or not *
 *************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_widget_set_sensitive (entry, TRUE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_widget_set_sensitive (entry, TRUE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_widget_set_sensitive (entry, TRUE);

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_x");
gtk_widget_set_sensitive (label, TRUE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_y");
gtk_widget_set_sensitive (label, TRUE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_z");
gtk_widget_set_sensitive (label, TRUE);

/*******************************************************************
 * When these entries were previously disabled, in Create dialogs, *
 * we need to supply proper default data, when enavling them again *
 *******************************************************************/

if (window->action == GAMGI_GTK_CELL_CREATE && translation.base != NULL)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_X);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Y);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Z);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

/****************************************
 * enable the Absolute/Relative buttons *
 * that only exist in the Modify dialog *
 ****************************************/

if (window->action == GAMGI_GTK_CELL_MODIFY)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_absolute");
  gtk_widget_set_sensitive (button, TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_relative");
  gtk_widget_set_sensitive (button, TRUE);
  }
}

static void static_translation_disable (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *button;

/*******************************************
 * This function disaables level 1 widgets *
 * when pressing Ok to enter level 2 data  *
 *******************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_x");
gtk_widget_set_sensitive (label, FALSE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_y");
gtk_widget_set_sensitive (label, FALSE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_z");
gtk_widget_set_sensitive (label, FALSE);

/*****************************************
 * disable the Absolute/Relative buttons *
 * that only exist in the Modify dialog  *
 *****************************************/

if (window->action == GAMGI_GTK_CELL_MODIFY)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_absolute");
  gtk_button_clicked (GTK_BUTTON (button));
  gtk_widget_set_sensitive (button, FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_relative");
  gtk_widget_set_sensitive (button, FALSE);
  }
}

static void static_translation_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

static_translation_enable (window);
static_translation_remove (window);

/*********************************************************
 * disable translation alignment: translation.base is    *
 * used to decide whether to initialize level 0 entries, *
 * so it must be done AFTER static_translation_enable    *
 *********************************************************/

translation.base = NULL;
}

static int static_translation_delete (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
static_translation_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_translation_layer (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *combo;
gamgi_bool *sensitive;

/************************************************
 * This function initializes Layer base widgets *
 ************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_base");
sensitive[GAMGI_PHYS_PRIMITIVE - 1] = FALSE;
}

static void static_translation_cell (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
gamgi_bool *sensitive;

/***********************************************
 * This function initializes Cell base widgets *
 ***********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_widget_set_sensitive (entry, TRUE);

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_base");
sensitive[GAMGI_PHYS_PRIMITIVE - 1] = TRUE;
}

static void static_translation_clicked (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_translation_cell (window);
else
  static_translation_layer (window);
}

static void static_translation_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;

/*******************
 * get base object *
 *******************/

translation.base = GAMGI_CAST_OBJECT window->layer;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
  "entry_cell", GAMGI_ENGINE_CELL, window);
  if (cell == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Object", window);
    return;
    }

  translation.base = GAMGI_CAST_OBJECT cell;
  }

/************************
 * get cell coordinates *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.cell_xyz[0], 
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.cell_xyz[1],
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.cell_xyz[2],
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

/********************
 * get cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
translation.cell_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/************************
 * get base coordinates *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.base_xyz[0], 
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.base_xyz[1],
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &translation.base_xyz[2],
-DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

/********************
 * get base vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
translation.base_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

static_translation_disable (window);
static_translation_remove (window);
}

static void static_translation_init (gamgi_window *window)
{
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];

/*************************************
 * start Layer and both Vector menus *
 *************************************/

static_translation_layer (window);
if (translation.base == NULL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);
  return;
  }

/******************
 * show cell data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_x");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.cell_xyz[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_y");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.cell_xyz[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_z");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.cell_xyz[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), translation.cell_vectors - 1);

/******************
 * show base data *
 ******************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
if (translation.base->class == GAMGI_ENGINE_CELL)
  {
  gtk_button_clicked (GTK_BUTTON (button));
  cell = GAMGI_CAST_CELL translation.base;
  sprintf (string, "%s %d", cell->object.name, cell->object.number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_x");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.base_xyz[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_y");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.base_xyz[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_z");
sprintf (string, "%.*f", gamgi->gamgi->length, translation.base_xyz[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), translation.base_vectors - 1);
}

static void static_translation (gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *vbox_dialog, *vbox_frame;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *frame;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

dialog = gamgi_gtk_dialog_task1_create ("Translation Align", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_translation_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/********
 * Cell *
 ********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Cell");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/********************
 * Cell coordinates *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_x", entry);
gtk_widget_show (entry);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_y", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_z", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/****************
 * Cell vectors *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_cell", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/********
 * Base *
 ********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Reference");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/***************
 * Base object *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Layer");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_translation_clicked), window);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Cell");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_translation_clicked), window);
g_object_set_data (G_OBJECT (dialog), "button_cell", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
gtk_widget_show (entry);

/********************
 * Base coordinates *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_x", entry);
gtk_widget_show (entry);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_y", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_z", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/****************
 * Base vectors *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_base", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_base", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
g_signal_connect (button, "clicked",
G_CALLBACK (static_translation_ok), window);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_translation_cancel), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
static_translation_init (window);
}

static void static_rotation_remove (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/********************************************
 * this function presses the level 0 button *
 * Align, that closes the level 1 dialog    *
 ********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation");
gtk_button_clicked (GTK_BUTTON (button));
}

static void static_rotation_enable (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/*************************************************
 * This function enables again level 1 widgets   *
 * that may have been previously disabled or not *
 *************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_widget_set_sensitive (entry, TRUE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_widget_set_sensitive (entry, TRUE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_widget_set_sensitive (entry, TRUE);

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e1");
gtk_widget_set_sensitive (label, TRUE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e2");
gtk_widget_set_sensitive (label, TRUE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e3");
gtk_widget_set_sensitive (label, TRUE);

/*******************************************************************
 * When these entries were previously disabled, in Create dialogs, *
 * we need to supply proper default data, when enavling them again *
 *******************************************************************/

if (window->action == GAMGI_GTK_CELL_CREATE && rotation.base != NULL)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E1);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E2);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E3);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

/****************************************
 * enable the Absolute/Relative buttons *
 * that only exist in the Modify dialog *
 ****************************************/

if (window->action == GAMGI_GTK_CELL_MODIFY)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_absolute");
  gtk_widget_set_sensitive (button, TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_relative");
  gtk_widget_set_sensitive (button, TRUE);
  }
}

static void static_rotation_disable (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *button;

/*******************************************
 * This function disaables level 1 widgets *
 * when pressing Ok to enter level 2 data  *
 *******************************************/

dialog = window->dialog0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e1");
gtk_widget_set_sensitive (label, FALSE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e2");
gtk_widget_set_sensitive (label, FALSE);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_e3");
gtk_widget_set_sensitive (label, FALSE);

/*****************************************
 * disable the Absolute/Relative buttons *
 * that only exist in the Modify dialog  *
 *****************************************/

if (window->action == GAMGI_GTK_CELL_MODIFY)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_absolute");
  gtk_button_clicked (GTK_BUTTON (button));
  gtk_widget_set_sensitive (button, FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_relative");
  gtk_widget_set_sensitive (button, FALSE);
  }
}

static void static_rotation_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

static_rotation_enable (window);
static_rotation_remove (window);

/*****************************************************
 * disable rotation alignment: rotation.base is used *
 * to decide whether to initialize level 0 entries,  *
 * so it must be done AFTER static_rotation_enable   *
 *****************************************************/

rotation.base = NULL;
}

static int static_rotation_delete (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
static_rotation_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_rotation_label (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *label1, *label2, *label3;
GtkWidget *combo;
int row;

/***********************************************************
 * this function updates the labels for the base objects:  *
 * direction/direction, plane/direction or direction/plane *
 ***********************************************************/

/*****************
 * combo box hkl *
 *****************/

label1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_h");
label2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_k");
label3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_l");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == 0)
  {
  gtk_label_set_text (GTK_LABEL (label1), "H");
  gtk_label_set_text (GTK_LABEL (label2), "K");
  gtk_label_set_text (GTK_LABEL (label3), "L");
  }
else
  {
  gtk_label_set_text (GTK_LABEL (label1), "U");
  gtk_label_set_text (GTK_LABEL (label2), "V");
  gtk_label_set_text (GTK_LABEL (label3), "W");
  }

/*******************
 * Option Menu uvw *
 *******************/
 
label1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_u");
label2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_v");
label3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_base_w");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == 0)
  {
  gtk_label_set_text (GTK_LABEL (label1), "H");
  gtk_label_set_text (GTK_LABEL (label2), "K");
  gtk_label_set_text (GTK_LABEL (label3), "L");
  }
else
  {
  gtk_label_set_text (GTK_LABEL (label1), "U");
  gtk_label_set_text (GTK_LABEL (label2), "V");
  gtk_label_set_text (GTK_LABEL (label3), "W");
  }

}

static void static_rotation_hkl (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *combo;
GtkWidget *button;
int row;

/*********************************************************
 * this function updates the second (uvw) combo box when *
 * the first (hkl) combo box changes, as in Cell mode    *
 * the base objects must be one plane and one direction  *
 *********************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE) return;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");
row = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");

/*********************************************
 * block changed signal for uvw combo box as *
 * changes done here to uvw combo box should *
 * not be used to change hkl combo box again *
 *********************************************/

g_signal_handlers_block_by_func (combo, static_rotation_uvw, window);

if (row == 0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

/***************************************************
 * unblock changed signal for uvw combo box, so it *
 * handles changes done directly to uvw combo box  *
 ***************************************************/

g_signal_handlers_unblock_by_func (combo, static_rotation_uvw, window);

static_rotation_label (window);
}

static void static_rotation_uvw (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *combo;
GtkWidget *button;
int row;

/********************************************************
 * this function updates the first (hkl) combo box when *
 * the second (uvw) combo box changes, as in Cell mode  *
 * the base objects must be one plane and one direction *
 ********************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE) return;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");
row = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");

/*********************************************
 * block changed signal for hkl combo box as *
 * changes done here to hkl combo box should *
 * not be used to change uvw combo box again *
 *********************************************/

g_signal_handlers_block_by_func (combo, static_rotation_hkl, window);

if (row == 0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

/***************************************************
 * unblock changed signal for hkl combo box, so it *
 * handles changes done directly to hkl combo box  *
 ***************************************************/

g_signal_handlers_unblock_by_func (combo, static_rotation_hkl, window);

static_rotation_label (window);
}

static void static_rotation_layer (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
gamgi_bool *sensitive_hkl, *sensitive_uvw, *sensitive_base;

/************************************************
 * This function initializes Layer base widgets *
 ************************************************/

/******************
 * handle entries *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");
gtk_widget_set_sensitive (entry, FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_h");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_H);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_k");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_K);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_l");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_L);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_u");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_U);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_v");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_V);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_w");
sprintf (token, "%d", GAMGI_PHYS_ALIGN_W);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/**********************
 * handle combo boxes *
 **********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);

sensitive_hkl = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_hkl");
sensitive_hkl[0] = FALSE;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);

sensitive_uvw = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_uvw");
sensitive_uvw[0] = FALSE;

static_rotation_label (window);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

sensitive_base = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_base");
sensitive_base[GAMGI_PHYS_PRIMITIVE - 1] = FALSE;
}

static void static_rotation_cell (gamgi_window *window)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *combo;
gamgi_bool *sensitive_base, *sensitive_hkl, *sensitive_uvw;

/***********************************************
 * This function initializes Cell base widgets *
 ***********************************************/

/******************
 * handle entries *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_widget_set_sensitive (entry, TRUE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_h");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_k");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_l");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_u");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_v");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_w");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/**********************
 * handle combo boxes *
 **********************/

sensitive_hkl = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_hkl");
sensitive_hkl[0] = TRUE;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

sensitive_uvw = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_uvw");
sensitive_uvw[0] = TRUE;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);

static_rotation_label (window);

sensitive_base = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_base");
sensitive_base[GAMGI_PHYS_PRIMITIVE - 1] = TRUE;
}

static void static_rotation_clicked (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog1;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_rotation_cell (window);
else
  static_rotation_layer (window);
}

static void static_rotation_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;

/************************
 * get Reference object *
 ************************/
 
rotation.base = GAMGI_CAST_OBJECT window->layer;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
  "entry_cell", GAMGI_ENGINE_CELL, window);
  if (cell == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Object", window);
    return;
    }

  rotation.base = GAMGI_CAST_OBJECT cell;
  }

/**************************
 * get Cell plane indices *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_hkl[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_hkl[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_hkl[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

if (gamgi_phys_direction_zero (rotation.cell_hkl) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

/******************************
 * get Cell direction indices *
 ******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_u");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_uvw[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_v");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_uvw[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_w");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.cell_uvw[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

if (gamgi_phys_direction_zero (rotation.cell_uvw) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

/********************
 * get Cell vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
rotation.cell_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/************************************
 * get Reference vector (given by   *
 * Plane or Direction) aligned with *
 * Cell vector (given by Plane)     *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_h");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_hkl[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_k");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_hkl[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_l");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_hkl[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

if (gamgi_phys_direction_zero (rotation.base_hkl) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

/************************************
 * get Reference vector (given by   *
 * Plane or Direction) aligned with *
 * Cell vector (given by Direction) *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_u");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_uvw[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_v");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_uvw[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_w");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &rotation.base_uvw[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

if (gamgi_phys_direction_zero (rotation.base_uvw) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

/************************************************
 * Cell direction must belong to Cell plane     *
 * and Reference direction must belong to       *
 * Reference plane, so the two Cell vectors     *
 * and the two Reference vectors must be normal *
 *                                              *
 * Wen the Reference is Layer, two Directions   *
 * are used instead, one describing a virtual   *
 * Plane, as there are no Planes in Layers      *
 ************************************************/

if (gamgi_phys_direction_weiss (rotation.cell_hkl, rotation.cell_uvw) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data", window);
  return;
  }

if (gamgi_phys_direction_weiss (rotation.base_hkl, rotation.base_uvw) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Reference Data", window);
  return;
  }

/********************
 * get base vectors *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
rotation.base_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**********************************************
 * get information on which indices represent *
 * a plane, and which represent a direction   *
 **********************************************/

rotation.plane = -1;
if (rotation.base->class == GAMGI_ENGINE_CELL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
  rotation.plane = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  }

static_rotation_disable (window);
static_rotation_remove (window);
}

static void static_rotation_init (gamgi_window *window)
{
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];

/*************************************
 * start Layer and both Vector menus *
 *************************************/

static_rotation_layer (window);
if (rotation.base == NULL)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);
  return;
  }

/******************
 * show cell data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_h");
sprintf (string, "%d", rotation.cell_hkl[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_k");
sprintf (string, "%d", rotation.cell_hkl[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_l");
sprintf (string, "%d", rotation.cell_hkl[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_u");
sprintf (string, "%d", rotation.cell_uvw[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);
 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_v");
sprintf (string, "%d", rotation.cell_uvw[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell_w");
sprintf (string, "%d", rotation.cell_uvw[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cell");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), rotation.cell_vectors - 1);

/******************
 * show base data *
 ******************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
if (rotation.base->class == GAMGI_ENGINE_CELL)
  {
  gtk_button_clicked (GTK_BUTTON (button));
  cell = GAMGI_CAST_CELL rotation.base;
  sprintf (string, "%s %d", cell->object.name, cell->object.number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_h");
sprintf (string, "%d", rotation.base_hkl[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_k");
sprintf (string, "%d", rotation.base_hkl[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_l");
sprintf (string, "%d", rotation.base_hkl[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_u");
sprintf (string, "%d", rotation.base_uvw[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_v");
sprintf (string, "%d", rotation.base_uvw[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_w");
sprintf (string, "%d", rotation.base_uvw[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_hkl");
if (rotation.plane != 0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_uvw");
if (rotation.plane != 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_base");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), rotation.base_vectors - 1);
}

static void static_rotation (gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *vbox_dialog, *vbox_frame;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *frame;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

dialog = gamgi_gtk_dialog_task1_create ("Rotation Align", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_rotation_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/********
 * Cell *
 ********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Cell");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

/**************
 * Cell Plane *
 **************/

label = gtk_label_new ("Plane");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_h", entry);
gtk_widget_show (entry);

label = gtk_label_new ("H");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_k", entry);
gtk_widget_show (entry);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_l", entry);
gtk_widget_show (entry);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/******************
 * Cell Direction *
 ******************/

label = gtk_label_new ("Direction");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_u", entry);
gtk_widget_show (entry);

label = gtk_label_new ("U");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_v", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_cell_w", entry);
gtk_widget_show (entry);

label = gtk_label_new ("W");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/****************
 * Cell vectors *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_cell", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/********
 * Base *
 ********/

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), frame, TRUE, TRUE, 0);
gtk_frame_set_label (GTK_FRAME (frame), "Reference");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

vbox_frame = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (frame), vbox_frame);
gtk_container_set_border_width (GTK_CONTAINER (vbox_frame), 5);
gtk_widget_show (vbox_frame);

/***************
 * Base Object *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Layer");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_rotation_clicked), window);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Cell");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_rotation_clicked), window);
g_object_set_data (G_OBJECT (dialog), "button_cell", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
gtk_widget_show (entry);

/****************************************
 * Base hkl vector (Plane or Direction) *
 ****************************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_rotation_hkl), window);
g_object_set_data (G_OBJECT (dialog), "combo_hkl", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Plane", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Direction", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_hkl", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_h", entry);
gtk_widget_show (entry);

label = gtk_label_new ("H");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_h", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_k", entry);
gtk_widget_show (entry);

label = gtk_label_new ("K");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_k", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_l", entry);
gtk_widget_show (entry);

label = gtk_label_new ("L");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_l", label);
gtk_widget_show (label);

/****************************************
 * Base uvw vector (Plane or Direction) *
 ****************************************/

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_rotation_uvw), window);
g_object_set_data (G_OBJECT (dialog), "combo_uvw", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Plane", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Direction", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_uvw", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_u", entry);
gtk_widget_show (entry);

label = gtk_label_new ("U");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_u", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_v", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_v", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_base_w", entry);
gtk_widget_show (entry);

label = gtk_label_new ("W");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_base_w", label);
gtk_widget_show (label);

/****************
 * Base vectors *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_frame), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_base", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_base", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

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
G_CALLBACK (static_rotation_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_rotation_cancel), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
static_rotation_init (window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog1;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

cell = GAMGI_CAST_CELL object;

/***************************
 * set Cell identification *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
sprintf (string, "%s %d", cell->object.name, cell->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

void gamgi_gtk_cell_align_press (gamgi_window *window, int x, int y)
{
GtkWidget *dialog = window->dialog1;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_cell");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE) return;

gamgi_mesa_select_object (window, x, y, GAMGI_ENGINE_CELL, FALSE, static_press);
}

void gamgi_gtk_cell_align_start (void)
{
/******************************************
 * disable translation,rotation alignment *
 ******************************************/

translation.base = NULL;
rotation.base = NULL;
}

void gamgi_gtk_cell_align_translation_get (gamgi_cell *cell, 
double *origin, gamgi_window *window)
{
gamgi_cell *base;
double rotate[9];
double x, y, z;
double offset[3], position[3];
double b[3], c[3];

/************************************************
 * check if align information has been provided *
 ************************************************/

if (translation.base == NULL) return;

/************************
 * get cell position: c *
 ************************/

x = translation.cell_xyz[0];
y = translation.cell_xyz[1];
z = translation.cell_xyz[2];

if (translation.cell_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_vector_translation (x, y, z, cell->a1, cell->a2, cell->a3, position);
if (translation.cell_vectors == GAMGI_PHYS_PRIMITIVE)
  gamgi_math_vector_translation (x, y, z, cell->p1, cell->p2, cell->p3, position);

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_matrix_vector (rotate, position, c);

/************************
 * get base position: b *
 ************************/

x = translation.base_xyz[0];
y = translation.base_xyz[1];
z = translation.base_xyz[2];

if (translation.base->class == GAMGI_ENGINE_CELL)
  {
  base = GAMGI_CAST_CELL translation.base;

  if (translation.base_vectors == GAMGI_PHYS_CONVENTIONAL)
    gamgi_math_vector_translation (x, y, z, base->a1, base->a2, base->a3, position);
  if (translation.base_vectors == GAMGI_PHYS_PRIMITIVE)
    gamgi_math_vector_translation (x, y, z, base->p1, base->p2, base->p3, position);

  gamgi_math_cell_offset (base, offset);
  gamgi_math_vector_add (offset, position, position);

  gamgi_math_quaternion_to_matrix (base->quaternion, rotate);
  gamgi_math_matrix_vector (rotate, position, b);

  gamgi_math_vector_add (base->origin, b, b);
  }
else gamgi_math_vector_absolute (b, x, y, z); 

/*******************************
 * get cell origin such that   *
 * points b and c are the same *
 *******************************/

gamgi_math_vector_sub (b, c, origin);
}

void gamgi_gtk_cell_align_rotation_get (gamgi_cell *cell, 
double *angle, gamgi_window *window)
{
gamgi_cell *base;
double matrix[9], rotate[9], inverse[9];
double a[3], b[3], c[3];
double pole[3];

/************************************************
 * check if align information has been provided *
 ************************************************/

if (rotation.base == NULL) return;

/*****************
 * get cell axes *
 *****************/

gamgi_phys_plane_vector (cell, rotation.cell_hkl, rotation.cell_vectors, a);
gamgi_phys_direction_vector (cell, rotation.cell_uvw, rotation.cell_vectors, b);

gamgi_math_vector_normal (a);
gamgi_math_vector_normal (b);
gamgi_math_vector_cross (a, b, c);

gamgi_math_matrix_absolute (matrix, 
a[0], b[0], c[0], a[1], b[1], c[1], a[2], b[2], c[2]);
gamgi_math_matrix_inverse (matrix, inverse);

/**********************
 * get reference axes *
 **********************/

/*********************************************
 * -1: (no plane) Layer, direction direction *
 *  0: (plane first) Cell, plane direction   *
 * -1: (plane last) Cell, direction plane    *
 *********************************************/

if (rotation.plane == -1)
  {
  /****************************************
   * rotated, translated layers work fine *
   ****************************************/
  gamgi_math_vector_absolute (a, 
  rotation.base_hkl[0], rotation.base_hkl[1], rotation.base_hkl[2]);
  gamgi_math_vector_absolute (b, 
  rotation.base_uvw[0], rotation.base_uvw[1], rotation.base_uvw[2]);
  }

if (rotation.plane == 0)
  {
  base = GAMGI_CAST_CELL rotation.base;
  gamgi_math_quaternion_to_matrix (base->quaternion, rotate);

  gamgi_phys_plane_vector (base, rotation.base_hkl, rotation.base_vectors, pole);
  gamgi_math_matrix_vector (rotate, pole, a);
  gamgi_phys_direction_vector (base, rotation.base_uvw, rotation.base_vectors, pole);
  gamgi_math_matrix_vector (rotate, pole, b);
  }
if (rotation.plane == 1)
  {
  base = GAMGI_CAST_CELL rotation.base;
  gamgi_math_quaternion_to_matrix (base->quaternion, rotate);

  gamgi_phys_direction_vector (base, rotation.base_hkl, rotation.base_vectors, pole);
  gamgi_math_matrix_vector (rotate, pole, a);
  gamgi_phys_plane_vector (base, rotation.base_uvw, rotation.base_vectors, pole);
  gamgi_math_matrix_vector (rotate, pole, a);
  }

gamgi_math_vector_normal (a);
gamgi_math_vector_normal (b);
gamgi_math_vector_cross (a, b, c);
gamgi_math_matrix_absolute (matrix, a[0], b[0], c[0], a[1], b[1], c[1], a[2], b[2], c[2]);

/**************************************************
 * determine Eular angles for the rotation matrix *
 * that converts cell axes into reference axes    *
 **************************************************/

gamgi_math_matrix_matrix (matrix, inverse, rotate);
gamgi_math_euler_from_matrix (rotate, angle);
}

void gamgi_gtk_cell_align_translation (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*********************
   * reset Cell dialog *
   *********************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  /***********************
   * create Align dialog *
   ***********************/

  static_translation (window);
  }

/***********************
 * remove Align dialog *
 ***********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

void gamgi_gtk_cell_align_rotation (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*********************
   * reset Cell dialog *
   *********************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  /***********************
   * Create Align Dialog *
   ***********************/

  static_rotation (window);
  }

/***********************
 * remove Align dialog *
 ***********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}
