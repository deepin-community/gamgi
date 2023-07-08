/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_modify.c
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
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_text_config.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_text.h"
#include "gamgi_mesa_center.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_NAME = 0, STATIC_CONTENTS,
STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y, STATIC_TRANSLATION_Z,
STATIC_ROTATION_E1, STATIC_ROTATION_E2, STATIC_ROTATION_E3, STATIC_STYLE,
STATIC_FONT, STATIC_RED, STATIC_GREEN, STATIC_BLUE, STATIC_SCALE };

static struct {
gamgi_bool local;
gamgi_bool new[STATIC_SCALE + 1];
char name[GAMGI_ENGINE_TOKEN];
char *contents;
gamgi_enum style, font;
gamgi_bool translation_absolute, rotation_absolute;
double translation[3], rotation[3];
float red, green, blue;
double scale; } cache;

static gamgi_enum static_font (gamgi_enum style, int offset)
{
/************************************************************
 * some font names (offset = 2,3,6,7) exist for both styles *
 ************************************************************/

switch (offset)
  {
  case 0: return GAMGI_IO_SUN_ROMAN;
  case 1: return GAMGI_IO_SUN_ROMAN_MONO;

  case 2:
  if (style == GAMGI_MESA_WIRED) return GAMGI_IO_HERSHEY_SANS;
  if (style == GAMGI_MESA_SOLID) return GAMGI_IO_DEJAVU_SANS;

  case 3:
  if (style == GAMGI_MESA_WIRED) return GAMGI_IO_HERSHEY_SANS_BOLD;
  if (style == GAMGI_MESA_SOLID) return GAMGI_IO_DEJAVU_SANS_BOLD;

  case 4: return GAMGI_IO_DEJAVU_SANS_ITALIC;
  case 5: return GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC;

  case 6:
  if (style == GAMGI_MESA_WIRED) return GAMGI_IO_HERSHEY_SERIF;
  if (style == GAMGI_MESA_SOLID) return GAMGI_IO_DEJAVU_SERIF;

  case 7:
  if (style == GAMGI_MESA_WIRED) return GAMGI_IO_HERSHEY_SERIF_BOLD;
  if (style == GAMGI_MESA_SOLID) return GAMGI_IO_DEJAVU_SERIF_BOLD;

  case 8: return GAMGI_IO_DEJAVU_SERIF_ITALIC;
  case 9: return GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC;

  case 10: return GAMGI_IO_HERSHEY_SCRIPT;
  case 11: return GAMGI_IO_HERSHEY_SCRIPT_BOLD;
  case 12: return GAMGI_IO_HERSHEY_GOTHIC_ENGLISH;
  case 13: return GAMGI_IO_HERSHEY_GOTHIC_GERMAN;
  case 14: return GAMGI_IO_HERSHEY_GOTHIC_ITALIAN;
  case 15: return GAMGI_IO_HERSHEY_SYMBOL_SANS;
  case 16: return GAMGI_IO_HERSHEY_SYMBOL_SERIF;
  case 17: return GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD;
  case 18: return GAMGI_IO_DEJAVU_MONO;
  case 19: return GAMGI_IO_DEJAVU_MONO_BOLD;
  case 20: return GAMGI_IO_DEJAVU_MONO_ITALIC;
  case 21: return GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC;
  }

/**********************************************
 * control should never reach this point: all *
 * options for Local style are handled above  *
 **********************************************/

return 0;
}

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkListStore *store;
GtkTreeIter iter;
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

if (row_style == GAMGI_MESA_WIRED - 1)
  {
  /****************************************************************
   * Wired Style: initialize with current default for Wired fonts *
   ****************************************************************/

  store = gamgi_gtk_text_config_wired (combo);
  row_font = gamgi->text->wired - 1;
  }
else if (row_style == GAMGI_MESA_SOLID - 1)
  {
  /****************************************************************
   * Solid Style: initialize with current default for Solid fonts *
   ****************************************************************/

  store = gamgi_gtk_text_config_solid (combo);
  row_font = gamgi->text->solid - GAMGI_IO_FONTS_WIRED - 1;
  }
else
  {
  /*******************************************
   * Local Style: initialize with Local font *
   *******************************************/

  store = gamgi_gtk_text_config_all (combo);
  row_font = GAMGI_IO_FONTS_TOTAL;
  }

/*************************************************
 * add Local row: the index given to this row    *
 * is always bigger than the indices given to    *
 * the specific fonts and is equal to all styles *
 *************************************************/

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), row_font);
}

/*************** internal function *****************
 *                                                 *
 *                STATIC_ABSOLUTE                  *
 *                                                 *
 * Set origin and angle reading modes to absolute. *
 ***************************************************/

static void static_absolute (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_absolute");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_absolute");
gtk_button_clicked (GTK_BUTTON (button));
}

/***************** internal function ***************
 *                                                 *
 *                  STATIC_RELATIVE                *
 *                                                 *
 * Set origin and angle reading modes to relative. *
 ***************************************************/

static void static_relative (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_relative");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_relative");
gtk_button_clicked (GTK_BUTTON (button));
}

/***************** internal function ***************
 *                                                 *
 *                   STATIC_ORIGIN                 *
 *                                                 *
 * Reset the origin entries everytime the absolute *
 * or relative origin buttons are toggled.         *
 *                                                 *
 ***************************************************/

static void static_origin (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (entry, "");
}

/***************** internal function **************
 *                                                *
 *                    STATIC_ANGLE                *
 *                                                *
 * Reset the angle entries everytime the absolute *
 * or relative angle buttons are toggled.         *
 *                                                *
 **************************************************/

static void static_angle (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (entry, "");
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_RESET                  *
 *                                                     *
 * Reset the text modify dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
GtkWidget *text;
GtkTextBuffer *buffer;

/*******************
 * clean text name *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***********************
 * clean text contents *
 ***********************/

text = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "text_contents");
buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_set_text (buffer, "", -1);

/************************************
 * reset style and font combo boxes * 
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_style (NULL, window);

/*****************
 * clean position *
 *****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*********************
 * clean orientation *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***************
 * clean color *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***************
 * clean scale *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/****************** internal function **************
 *                                                 *
 *                     STATIC_LIST                 *
 *                                                 *
 ***************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_text");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * modify a single text directly *
   *********************************/

  gtk_label_set_text (GTK_LABEL (label), "Text");
  static_absolute (dialog);
  }
else
  {
  /*********************************************
   * modify multiple texts previously selected *
   *********************************************/

  gtk_label_set_text (GTK_LABEL (label), "Name");
  static_reset (window);
  static_relative (dialog);
  }
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
GtkWidget *text_view;
GtkTextBuffer *buffer;
GtkTextIter start;
GtkTextIter end;
const char *name;
int row;

/*******************************************
 * style, font: when style is unknown,     *
 * the new font depends of the text object *
 *******************************************/

cache.new[STATIC_STYLE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2) { cache.new[STATIC_STYLE] = TRUE; cache.style = row + 1; }

cache.new[STATIC_FONT] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (cache.style == GAMGI_MESA_WIRED)
  {
  if (row < GAMGI_IO_FONTS_WIRED)
    { cache.new[STATIC_FONT] = TRUE; cache.font = 1 + row; }
  }
else if (cache.style == GAMGI_MESA_SOLID)
  {
  if (row < GAMGI_IO_FONTS_SOLID)
    { cache.new[STATIC_FONT] = TRUE; cache.font = 1 + row + GAMGI_IO_FONTS_WIRED; }
  }
else
  {
  if (row < GAMGI_IO_FONTS_TOTAL)
    { cache.new[STATIC_FONT] = TRUE; cache.font = row; }
  }

/******************************
 * translation: type, x, y, z *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_absolute");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.translation_absolute = TRUE;
else
  cache.translation_absolute = FALSE;

cache.new[STATIC_TRANSLATION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text translation", window);
    return FALSE;
    }
  }

cache.new[STATIC_TRANSLATION_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_Y] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text translation", window);
    return FALSE;
    }
  }

cache.new[STATIC_TRANSLATION_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_Z] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text translation", window);
    return FALSE;
    }
  }

/******************************
 * rotation: type, e1, e2, e3 *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation_absolute");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.rotation_absolute = TRUE;
else
  cache.rotation_absolute = FALSE;

cache.new[STATIC_ROTATION_E1] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E1] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[0], 0.0, 180.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text rotation", window);
    return FALSE;
    }
  }

cache.new[STATIC_ROTATION_E2] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E2] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[1], 0.0, 360.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text rotation", window);
    return FALSE;
    }
  }

cache.new[STATIC_ROTATION_E3] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E3] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[2], 0.0, 360.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text rotation", window);
    return FALSE;
    }
  }

/***************************
 * red, green, blue, scale *
 ***************************/
  
cache.new[STATIC_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_GREEN] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_GREEN] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.green, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_BLUE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BLUE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.blue, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_SCALE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SCALE] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.scale, GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid scale", window);
    return FALSE;
    }
  }

/************
 * contents *
 ************/

cache.new[STATIC_CONTENTS] = FALSE;
text_view = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "text_contents");
buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
gtk_text_buffer_get_start_iter (buffer, &start);
gtk_text_buffer_get_end_iter (buffer, &end);
cache.contents = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
if (gamgi_io_token_check (cache.contents) == TRUE)
  {
  cache.new[STATIC_CONTENTS] = TRUE;
  if (gamgi_io_token_alpha_check (cache.contents,
  GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text contents", window);
    return FALSE;
    }
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_text *text, gamgi_window *window)
{
char name[GAMGI_ENGINE_TOKEN];
char line[GAMGI_ENGINE_LINE];
double origin[3], origin_old[3];
double euler[3], euler_old[3];
double zero[3], center[3];
double scale;
float red, green, blue;
gamgi_enum style, font, cache_font;

/*************************
 * import data from text *
 *************************/

strcpy (name, text->object.name);
style = text->style;
font = text->font;

gamgi_math_position_text_translation_get (text, origin_old);
if (cache.translation_absolute == TRUE)
  gamgi_math_vector_copy (origin_old, origin);
else
  gamgi_math_vector_zero (origin);

gamgi_math_position_text_rotation_get (text, euler_old);
if (cache.rotation_absolute == TRUE)
  gamgi_math_vector_copy (euler_old, euler);
else
  gamgi_math_vector_zero (euler);

red = text->red;
green = text->green;
blue = text->blue;
scale = text->object.scale;

/*******************************************
 * apply new data: when style is unknown,  *
 * the new font depends of the text object *
 *******************************************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_STYLE] == TRUE) style = cache.style;
if (cache.new[STATIC_STYLE] == FALSE && cache.new[STATIC_FONT] == TRUE)
  cache_font = static_font (text->style, cache.font);
else cache_font = cache.font;
if (cache.new[STATIC_FONT] == TRUE) font = cache_font;

if (cache.new[STATIC_TRANSLATION_X] == TRUE) origin[0] = cache.translation[0];
if (cache.new[STATIC_TRANSLATION_Y] == TRUE) origin[1] = cache.translation[1];
if (cache.new[STATIC_TRANSLATION_Z] == TRUE) origin[2] = cache.translation[2];
if (cache.new[STATIC_ROTATION_E1] == TRUE) euler[0] = cache.rotation[0];
if (cache.new[STATIC_ROTATION_E2] == TRUE) euler[1] = cache.rotation[1];
if (cache.new[STATIC_ROTATION_E3] == TRUE) euler[2] = cache.rotation[2];

if (cache.translation_absolute == FALSE)
  gamgi_math_vector_add (origin, origin_old, origin);
if (cache.rotation_absolute == FALSE)
  gamgi_math_vector_add (euler, euler_old, euler);

if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

if (style == GAMGI_MESA_WIRED && font > GAMGI_IO_FONTS_WIRED)
  {
  /*******************************
   * wired style with solid font *
   *******************************/

  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid font data", window);
  return FALSE;
  }

if (style == GAMGI_MESA_SOLID && font <= GAMGI_IO_FONTS_WIRED)
  {
  /*******************************
   * solid style with wired font *
   *******************************/

  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid font data", window);
  return FALSE;
  }

if (cache.new[STATIC_CONTENTS] == TRUE)
  {
  if (gamgi_mesa_text_center (font,
  cache.contents, strlen (cache.contents), center, line) == FALSE)
    {
    /***********************************
     * new center cannot be calculated *
     ***********************************/

    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error", line, window);
    return FALSE;
    }
  }
else
  {
  if (gamgi_mesa_text_center (font,
  text->string, text->length, center, line) == FALSE)
    {
    /***********************************
     * old center cannot be calculated *
     ***********************************/

    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error", line, window);
    return FALSE;
    }
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/***********************************************
 * export data to text: reset position; update *
 * glyph arrays when contents or font changed  *
 ***********************************************/
 
strcpy (text->object.name, name);
text->style = style;

gamgi_math_vector_zero (zero);
gamgi_math_position_text_rotation_set (text, zero);
gamgi_math_position_text_translation_set (text, zero);

if (cache.new[STATIC_CONTENTS] == TRUE)
  {
  gamgi_mesa_text_font_decrease (text);
  gamgi_mesa_text_remove (text);
  gamgi_mesa_text_create (text, cache.contents);
  text->font = font;
  gamgi_mesa_text_font_increase (text, NULL);
  }
else if (font != text->font)
  {
  gamgi_mesa_text_font_decrease (text);
  text->font = font;
  gamgi_mesa_text_font_increase (text, NULL);
  }

gamgi_math_vector_copy (center, text->center);
gamgi_math_position_text_rotation_set (text, euler);
gamgi_math_position_text_translation_set (text, origin);

text->red = red;
text->green = green;
text->blue = blue;
text->object.scale = scale;

return TRUE;
}

static void static_ok_global (gamgi_window *window)
{
gamgi_dlist *dlist;
gamgi_slist *start;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

cache.local = FALSE;

if (gamgi->texts == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of text objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid text identity", window);
    return;
    }
  }

if (static_grab (window) == FALSE) return;

/************************************************
 * For all windows: save layers as undo layers  *
 * and set current layer as new current object  *
 *                                              *
 * Modify listed objects and show error message *
 * with objects that could not be modified      *
 *                                              *
 * For all windows: redraw gl_area image        *
 ************************************************/

gamgi_global_selection_undo ();
gamgi_global_selection_focus ();

start = NULL;
for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_TEXT dlist->data, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window);

gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window);
}

static void static_ok_local (gamgi_window *window)
{
gamgi_text *text;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/*****************************************
 * pick text and get name (possibly new) *
 *                                       *
 * check if focus object belongs to text *
 *****************************************/

cache.new[STATIC_NAME] = TRUE;
text = GAMGI_CAST_TEXT gamgi_gtk_object_number (dialog,
"entry_text", GAMGI_ENGINE_TEXT, window, cache.name);
if (text == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid text identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT text) == TRUE) focus = TRUE;

/********************************************
 * collect data from dialog and modify text *
 ********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (text, window) == FALSE) return;

/*************************************************************
 * when focus object is affected, change it to current layer *
 *************************************************************/

if (focus == TRUE)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);

/*****************************************
 * redraw gl_area image and reset dialog *
 *****************************************/

gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

/*********************************************
 * modify data: execute global or local task *
 *********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_global (window);
else
  static_ok_local (window);

/***************
 * free memory *
 ***************/

free (cache.contents);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkRequisition size;

/******************************
 * set combo box maximum size *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
gtk_widget_size_request (GTK_WIDGET (combo), &size);
gtk_widget_set_size_request (combo, size.width, size.height);

/*****************************************
 * initialize style and font combo boxes *
 *****************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_style (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_text *text;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *contents;
GtkWidget *entry;
GtkTextBuffer *buffer;
char string[2 * GAMGI_ENGINE_TOKEN];
double rotate[9], euler[3];
int row;

text = GAMGI_CAST_TEXT object;

static_absolute (dialog);

/***************************
 * set text identification *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_text");
sprintf (string, "%s %d", text->object.name, text->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*********************
 * set text contents *
 *********************/

contents = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "text_contents");
buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (contents));
gtk_text_buffer_set_text (buffer, text->string, text->length);

/****************************
 * set font style combo box *
 ****************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), text->style - 1);
static_style (NULL, window);

/***************************
 * set font name combo box *
 ***************************/

row = text->font - 1;
if (text->style == GAMGI_MESA_SOLID) row -= GAMGI_IO_FONTS_WIRED;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_font");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), row);

/*********************
 * set position data *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (string, "%.*f", gamgi->gamgi->length, text->origin[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (string, "%.*f", gamgi->gamgi->length, text->origin[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (string, "%.*f", gamgi->gamgi->length, text->origin[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/************************
 * set orientation data *
 ************************/

gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/******************
 * get color data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, text->red);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, text->green);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, text->blue);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/******************
 * Get Scale Data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, text->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_TEXT_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_text_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;

/******************************
 * local mouse selection only *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_TEXT, FALSE, static_press);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_TEXT_MODIFY                *
 *                                                  *
 *  Creates the dialog window used to modify texts. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_text_modify (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *frame;
GtkWidget *scrolled_window;
GtkWidget *text;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *label;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/****************** 
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Text Modify", window);
window->action = GAMGI_GTK_TEXT_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Text objects *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********
 * Local *
 *********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Text");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_text", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_text", entry);
gtk_widget_show (entry);

/********
 * List *
 ********/

button = gtk_toggle_button_new_with_label (" List ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_list), window);
g_object_set_data (G_OBJECT (dialog), "button_list", button);
gtk_widget_show (button);

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

/**********
 * Origin *
 **********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_relative", button);
gtk_widget_show (button);

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

/*********
 * Angle *
 *********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("    Rotation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_relative", button);
gtk_widget_show (button);

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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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
