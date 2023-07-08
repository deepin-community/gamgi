/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_config.c
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
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_text_class *text_class, *text_class_old;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
int row;

text_class = gamgi_global_copy_text (gamgi->text);
 
/**************************
 * get default font style *
 **************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
text_class->style = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/**************************
 * get default wired font *
 **************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_wired");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
text_class->wired = row + 1;

/**************************
 * get default solid font *
 **************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_solid");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
text_class->solid = row + 1 + GAMGI_IO_FONTS_WIRED;

/*************
 * get color *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text_class->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_text (text_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text_class->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_text (text_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text_class->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  gamgi_global_remove_text (text_class);
  return;
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

text_class_old = gamgi->text;
gamgi->text = text_class;
gamgi_global_remove_text (text_class_old);

/******************************************
 * Remove dialog and redraw gl_area image *
 ******************************************/

gamgi_gtk_dialog_task0_remove (NULL, window);
gtk_widget_queue_draw (window->area);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo_wired, *combo_solid, *combo;
GtkRequisition size;
char token[GAMGI_ENGINE_TOKEN];
int width, height;
int row;

/**********************
 * default text style *
 **********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), gamgi->text->style - 1);

/**********************
 * default wired font *
 **********************/

row = gamgi->text->wired - 1;
combo_wired = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_wired");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo_wired), row);

gtk_widget_size_request (GTK_WIDGET (combo_wired), &size);
width = size.width; height = size.height;

/**********************
 * default solid font *
 **********************/

row = gamgi->text->solid - GAMGI_IO_FONTS_WIRED - 1;
combo_solid = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_solid");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo_solid), row);

gtk_widget_size_request (GTK_WIDGET (combo_solid), &size);
if (size.width > width) width = size.width;
if (size.height > height) height = size.height;

/*******************************************************************
 * combo boxes need different sizes: use the largest size for both *
 *******************************************************************/

gtk_widget_set_size_request (combo_wired, width, height);
gtk_widget_set_size_request (combo_solid, width, height);

/**********************
 * default text color *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

GtkListStore *gamgi_gtk_text_config_wired (GtkWidget *combo)
{
GtkListStore *store;
GtkTreeIter iter;

store = gtk_list_store_new (1, G_TYPE_STRING);

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Roman", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Roman_Mono", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Script", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Script_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_English", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_German", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_Italian", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Sans", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Serif", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Serif_Bold", -1);

gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

return store;
}

GtkListStore *gamgi_gtk_text_config_solid (GtkWidget *combo)
{
GtkListStore *store;
GtkTreeIter iter;

store = gtk_list_store_new (1, G_TYPE_STRING);

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Bold_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Bold_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Bold_Italic", -1);

gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

return store;
}

GtkListStore *gamgi_gtk_text_config_all (GtkWidget *combo)
{
GtkListStore *store;
GtkTreeIter iter;

store = gtk_list_store_new (1, G_TYPE_STRING);

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Roman", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Roman_Mono", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sans_Bold_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Serif_Bold_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Script", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Script_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_English", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_German", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Gothic_Italian", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Sans", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Serif", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Symbol_Serif_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Bold", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Italic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Mono_Bold_Italic", -1);

gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

return store;
}

/**************** external function ******************
 *                                                   *
 *              GAMGI_GTK_TEXT_CONFIG                *
 *                                                   *
 *  Creates the dialog window used to config Texts.  *
 *                                                   *
 *****************************************************/

void gamgi_gtk_text_config (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Text Config", window);
window->action = GAMGI_GTK_TEXT_CONFIG;
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

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * Font *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********
 * Style *
 *********/

table = gtk_table_new (2, 1, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Style");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
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

/********
 * Font *
 ********/

table = gtk_table_new (3, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Font");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

/*********
 * Wired *
 *********/

label = gtk_label_new ("Wired");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "combo_wired", combo);
gtk_widget_show (combo);

gamgi_gtk_text_config_wired (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********
 * Solid *
 *********/

label = gtk_label_new ("Solid");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 5);
g_object_set_data (G_OBJECT (dialog), "combo_solid", combo);
gtk_widget_show (combo);

gamgi_gtk_text_config_solid (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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
