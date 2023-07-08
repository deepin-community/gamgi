/***********************************************
 *
 * $GAMGI/src/gtk/bond/gamgi_gtk_bond_config.c
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
#include "gamgi_mesa_bond.h"
#include "gamgi_math_sort.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

static void static_limit (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry, *entry_min, *entry_max;
char token[GAMGI_ENGINE_TOKEN];
const char *name1, *name2;
double min, max;
int z1, z2;

entry_min = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_min");
entry_max = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_max");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
name1 = gtk_entry_get_text (GTK_ENTRY (entry));
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
name2 = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_alpha_scan (name1, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == TRUE &&
gamgi_chem_atom_number (token, &z1) == TRUE &&
gamgi_io_token_alpha_scan (name2, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == TRUE &&
gamgi_chem_atom_number (name2, &z2) == TRUE)
  {
  gamgi_chem_bond_range (gamgi->bond, z1, z2,
  gamgi->atom->radius[z1], gamgi->atom->radius[z2], &min, &max);
  if (min > 0 && max > 0)
    {
    sprintf (token, "%.*f", gamgi->gamgi->length, min);
    gtk_entry_set_text (entry_min, token);

    sprintf (token, "%.*f", gamgi->gamgi->length, max);
    gtk_entry_set_text (entry_max, token);
    }
  }
else
  {
  gtk_entry_set_text (entry_min, "");
  gtk_entry_set_text (entry_max, "");
  }
}

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
gamgi_bond_class *bond_class, *bond_class_old;
GtkWidget *button, *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
double min, max;
int element1, element2;
int n, min_max;
int row;

bond_class = gamgi_global_copy_bond (gamgi->bond);

/********************************************
 * get lower,upper global length parameters *
 ********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lower");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &bond_class->lower, 0, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_upper");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &bond_class->upper, 0, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

if (bond_class->lower > bond_class->upper)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

/*********************************************************
 * get element1,element2,min,max local length parameters *
 *********************************************************/

min_max = 0;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  min_max++;
  if (gamgi_io_token_alpha_scan (name, token, 
  GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE || 
  gamgi_chem_atom_number (token, &element1) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Element Data", window);
    gamgi_global_remove_bond (bond_class);
    return;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  min_max++;
  if (gamgi_io_token_alpha_scan (name, token, 
  GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE ||
  gamgi_chem_atom_number (token, &element2) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Element Data", window);
    gamgi_global_remove_bond (bond_class);
    return;
    }
  }

min = 0.0;
max = 0.0;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  min_max++;
  if (gamgi_io_token_double_scan (name, &min, 0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
    gamgi_global_remove_bond (bond_class);
    return;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_max");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  min_max++;
  if (gamgi_io_token_double_scan (name, &max, 0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
    gamgi_global_remove_bond (bond_class);
    return;
    }
  }

if (min > max)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

if (min_max == 1 || min_max == 2 || min_max == 3)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

if (min_max == 4)
  {
  gamgi_math_sort_int2 (element1, element2, &element1, &element2);
  n = element2 * (element2 + 1)/2 - element1;
  bond_class->min[n] = min;
  bond_class->max[n] = max;
  }

/*************************************************************
 * get view parameters: style, size, color, red, green, blue *
 *************************************************************/

/*********
 * style *
 *********/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == GAMGI_MESA_WIRED - 1)
  bond_class->draw = gamgi_mesa_bond_draw_line;
if (row == GAMGI_MESA_SOLID - 1)
  bond_class->draw = gamgi_mesa_bond_draw_cylinder;

/********
 * size *
 ********/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &bond_class->size, 
GAMGI_MATH_TOLERANCE, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Size Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

/*********
 * color *
 *********/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  bond_class->color = TRUE;
else
  bond_class->color = FALSE;

/******************
 * red,green,blue *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &bond_class->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &bond_class->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &bond_class->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_global_remove_bond (bond_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

bond_class_old = gamgi->bond;
gamgi->bond = bond_class;
gamgi_global_remove_bond (bond_class_old);

/*****************
 * Remove dialog *
 *****************/

gamgi_gtk_dialog_task0_remove (widget, window);
}

static void static_init (gamgi_window *window)
{
gamgi_bond_class *bond_class = gamgi->bond;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/*****************************************
 * initialise lower,upper length factors *
 *****************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lower");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, bond_class->lower);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_upper");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, bond_class->upper);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************************
 * initialise style option menu *
 ********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (bond_class->draw == gamgi_mesa_bond_draw_line)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (bond_class->draw == gamgi_mesa_bond_draw_cylinder)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);

/*****************************
 * initialise size combo box *
 *****************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, bond_class->size);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***************************
 * initialise color button *
 ***************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
if (bond_class->color == TRUE)
  gtk_button_clicked (GTK_BUTTON (button));

/****************************
 * initialise color entries *
 ****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond_class->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond_class->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond_class->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

void gamgi_gtk_bond_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *notebook;
GtkWidget *dialog;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
char token[GAMGI_ENGINE_TOKEN];

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Bond Config", window);
window->action = GAMGI_GTK_BOND_CONFIG;
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
 * Length page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Length");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************************
 * Global scaling factors *
 **************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Scaling");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_lower", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Lower");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_upper", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Upper");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/*****************************
 * Element Element distances *
 *****************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_limit), window);
g_object_set_data (G_OBJECT (dialog), "entry_element1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_limit), window);
g_object_set_data (G_OBJECT (dialog), "entry_element2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Min");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Max");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_min", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_max", entry);
gtk_widget_show (entry);

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

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * View *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********************
 * style option menu *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Style");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_style", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Wired", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Solid", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/******************
 * Size combo box *
 ******************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Size");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_entry_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "combo_size", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.3);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.4);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.5);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

gtk_combo_box_entry_set_text_column (GTK_COMBO_BOX_ENTRY (combo), 0);

/**************
 * Bond color *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label ("Color");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "button_color", button);
gtk_widget_show (button);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_blue", label);
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
