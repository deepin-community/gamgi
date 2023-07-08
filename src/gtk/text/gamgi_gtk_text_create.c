/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_create.c
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_text_config.h"
#include "gamgi_mesa_area.h"
#include "gamgi_mesa_text.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"

#define GTK_ENABLE_BROKEN

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int row_style, row_font;

/*****************
 * get new style *
 *****************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row_style = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*********************************
 * replace old by new TreeMoldel *
 *********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
row_font = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

row_font = FALSE;
if (row_style == GAMGI_MESA_WIRED - 1)
  {
  gamgi_gtk_text_config_wired (combo);
  row_font = gamgi->text->wired - 1;
  }
if (row_style == GAMGI_MESA_SOLID - 1)
  {
  gamgi_gtk_text_config_solid (combo);
  row_font = gamgi->text->solid - GAMGI_IO_FONTS_WIRED - 1;
  }

gtk_combo_box_set_active (GTK_COMBO_BOX (combo), row_font);
}

/**************** internal function *************
 *                                              *
 *                   STATIC_RESET               *
 *                                              *
 * Reset the text create dialog, cleaning only  *
 * the x,y,z and e1,e2,e3 entries, as the user  *
 * might wish to create more text objects with  *
 * the same style, font, color and scale.       *
 *                                              *
 ************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/****************
 * reset origin *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*********************
 * reset orientation *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E1);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E2);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E3);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** internal function ****************
 *                                                *
 *                    STATIC_OK                   *
 *                                                *
 *   This is the function that actually creates   *
 *   a new text. It is triggered when the user    *
 * presses Ok or when she/he cliks on the screen, *
 * to indicate the position of the new text.      *
 *                                                *
 **************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_text *text;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *text_view;
GtkTextBuffer *buffer;
GtkTextIter start;
GtkTextIter end;
char line[GAMGI_ENGINE_LINE];
char *contents = NULL;
const char *name;
double origin[3], euler[3];

text = gamgi_engine_create_text ();
gamgi_engine_start_text (text);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_text",
"Text", text->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Text Name", window);
  gamgi_engine_remove_text (text);
  return;
  }

/*******************************************************************
 * text objects cannot be longer than GAMGI_ENGINE_LINE (200) char *
 *                                                                 *
 * get contents and free it as soon as possible. text->string is   *
 * free inside gamgi_engine_remove_text (in case an error occurs)  *
 *******************************************************************/

text_view = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "text_contents");
buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
gtk_text_buffer_get_start_iter (buffer, &start);
gtk_text_buffer_get_end_iter (buffer, &end);
contents = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

if (gamgi_io_token_check (contents) == FALSE ||
gamgi_io_token_alpha_check (contents, GAMGI_IO_FONT_ASCII,
GAMGI_ENGINE_LINE) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Text Contents", window);
  gamgi_engine_remove_text (text);
  free (contents);
  return;
  }

gamgi_mesa_text_create (text, contents);
free (contents);

/*************
 * get style *
 *************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
text->style = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/***********************************************************************
 * get font: the global font enumeration lists first the wired fonts,  *
 * then the solid fonts, so to get the correct number for solid fonts, *
 * we must add an offset equal to the total number of wired fonts      *
 ***********************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
text->font = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (text->style == GAMGI_MESA_SOLID) text->font += GAMGI_IO_FONTS_WIRED;

/************************************************************
 * As soon as text->font becomes available, the font must   *
 * be referenced immediately, as gamgi_engine_remove_text   *
 * will unreference it, if later an error occurs.           *
 *                                                          *
 * Allocate font, increase glyph count, compile new glyphs, *
 * cancel everything when the font filename is not found    *
 ************************************************************/

if (gamgi_mesa_text_center (text->font,
text->string, text->length, text->center, line) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", line, window);
  text->font = FALSE;
  gamgi_engine_remove_text (text);
  return;
  }
gamgi_mesa_text_font_increase (text, NULL);

/*************************
 * get x,y,z coordinates *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

/********************
 * get Euler angles *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[0], 0.0, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[1], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[2], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

/********************
 * get R,G,B colors *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &text->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

/*************
 * Get scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &text->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_text (text);
  return;
  }

gamgi_math_position_text_rotation_set (text, euler);
gamgi_math_position_text_translation_set (text, origin);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_text_layer (text, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the text create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
GtkRequisition size;
char token[GAMGI_ENGINE_TOKEN];

/*********************************
 * set combo box to maximum size *
 *********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
gtk_widget_size_request (GTK_WIDGET (combo), &size);
gtk_widget_set_size_request (combo, size.width, size.height);

/*****************************************
 * initialize style and font combo boxes *
 *****************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), gamgi->text->style - 1);
static_style (NULL, window);

/*********************
 * initialize origin *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_TEXT_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/**************************
 * initialize orientation *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E1);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E2);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_MESA_TEXT_E3);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*****************************
 * initialize red,green,blue *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->text->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * initialize scale *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_TEXT_CREATE_PRESS        *
 *                                             *
 * Receives the 2D window x,y coordinates,     *
 * determines the 3D x,y,z object coordinates, *
 * prints them in the x, y, z entries and      *
 * runs static_ok (), as if the user had       *
 * pressed the Ok button.                      *
 *                                             *
 ***********************************************/

void gamgi_gtk_text_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *entry;
double x_3d, y_3d, z_3d;
char string[GAMGI_ENGINE_TOKEN];

/******************************
 * local mouse selection only *
 ******************************/

if (window_dialog != window_mouse) return;
gamgi_mesa_area_unproject (window_mouse, x, y, &x_3d, &y_3d, &z_3d);

sprintf (string, "%.*f", gamgi->gamgi->length, x_3d);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), string);

sprintf (string, "%.*f", gamgi->gamgi->length, y_3d);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), string);

sprintf (string, "%.*f", gamgi->gamgi->length, z_3d);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), string);

static_ok (NULL, window_dialog);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_TEXT_CREATE                *
 *                                                  *
 *  Creates the dialog window used to create texts. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_text_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *frame;
GtkWidget *scrolled_window;
GtkWidget *text;
GtkWidget *table;
GtkWidget *label;
GtkWidget *button;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Text Create", window);
window->action = GAMGI_GTK_TEXT_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*************
 * Text name *
 *************/

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
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_text", entry);
gtk_widget_show (entry);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*****************
 * Contents page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Contents");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

frame = gtk_frame_new (NULL);
gtk_box_pack_start (GTK_BOX (vbox_center), frame, TRUE, TRUE, 0);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_widget_show (frame);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_container_add (GTK_CONTAINER (frame), scrolled_window);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

text = gtk_text_view_new ();
gtk_text_view_set_editable (GTK_TEXT_VIEW (text), TRUE);
gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), TRUE);
gtk_text_view_set_justification (GTK_TEXT_VIEW (text), GTK_JUSTIFY_LEFT);
gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_WORD);

gtk_container_add (GTK_CONTAINER (scrolled_window), text);
g_object_set_data (G_OBJECT (dialog), "text_contents", text);
gtk_widget_show (text);

/*****************
 * Position page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Position");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/***************
 * Translation *
 ***************/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_z", entry);
gtk_widget_show (entry);

/************
 * Rotation *
 ************/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Rotation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("E1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_e3", entry);
gtk_widget_show (entry);

/*************
 * View page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/*****************************
 * 1st row: font Style,Faces *
 *****************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********
 * Style *
 *********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Style");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_style), window);
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

/**************************************************************
 * Font: add all font names, to get maximum size, the correct *
 * font names are set later, when defaults are initialized    *
 **************************************************************/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Font");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_font", combo);
gtk_widget_show (combo);

gamgi_gtk_text_config_all (combo);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/******************
 * 2nd row: Color *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
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

/*********
 * Scale *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
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
