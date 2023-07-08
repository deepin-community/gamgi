/***********************************************
 *
 * $GAMGI/src/gtk/bond/gamgi_gtk_bond_modify.c
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

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

static struct {
gamgi_bool local;
gamgi_bool new[10];
char name[GAMGI_ENGINE_TOKEN];
float order;
float covalency;
void (*draw) (gamgi_bond *);
float size;
gamgi_bool color;
float red, green, blue;
double scale; } cache;

enum { STATIC_NAME = 0, STATIC_ORDER, STATIC_COVALENCY, 
STATIC_STYLE, STATIC_SIZE, STATIC_COLOR, STATIC_RED, STATIC_GREEN,
STATIC_BLUE, STATIC_SCALE };

static void static_color (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label_red, *label_green, *label_blue;
char token[GAMGI_ENGINE_TOKEN];
int row;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_blue");

if (row == GAMGI_GTK_NO - 1)
  {
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  }

if (row == GAMGI_GTK_YES - 1)
  {
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->red);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->green);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->blue);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  }
}

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *label;
char token[GAMGI_ENGINE_TOKEN];
int row;

/*****************
 * get new style *
 *****************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_size");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");

if (row == GAMGI_MESA_WIRED - 1)
  {
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (combo, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (combo, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

if (row == GAMGI_MESA_SOLID - 1)
  {
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, gamgi->bond->size);
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else
  {
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  }
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_RESET                  *
 *                                                     *
 * Reset the bond modify dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bond");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_covalency");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_order");
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_style (NULL, window);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_color (NULL, window);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_bond");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * modify a single bond directly *
   *********************************/

  gtk_label_set_text (GTK_LABEL (label), "Bond");
  }
else
  {
  /*********************************************
   * modify multiple bonds previously selected *
   *********************************************/

  gtk_label_set_text (GTK_LABEL (label), "Name");
  static_reset (window);
  }
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
int row;

/********************
 * order, covalency *
 ********************/

cache.new[STATIC_ORDER] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_order");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ORDER] = TRUE;
  if (gamgi_io_token_float_scan (name, &cache.order, 0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid order data", window);
    return FALSE;
    }
  }

cache.new[STATIC_COVALENCY] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_covalency");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_COVALENCY] = TRUE;
  if (gamgi_io_token_float_scan (name, &cache.covalency, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid covalency data", window);
    return FALSE;
    }
  }

/***************
 * style, size *
 ***************/

cache.new[STATIC_STYLE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_STYLE] = TRUE;
  if (row == GAMGI_MESA_WIRED - 1) cache.draw = gamgi_mesa_bond_draw_line;
  if (row == GAMGI_MESA_SOLID - 1) cache.draw = gamgi_mesa_bond_draw_cylinder;
  }

cache.new[STATIC_SIZE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SIZE] = TRUE;
  if (gamgi_io_token_float_scan (name, &cache.size, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid size data", window);
    return FALSE;
    }
  }

/**********************************
 * color, red, green, blue, scale *
 **********************************/
   
cache.new[STATIC_COLOR] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_COLOR] = TRUE;
  if (row == GAMGI_GTK_YES - 1) cache.color = TRUE;
  if (row == GAMGI_GTK_NO - 1) cache.color = FALSE;
  }

cache.new[STATIC_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
    return FALSE;
    }
  }

cache.new[STATIC_SCALE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SCALE] = TRUE;
  if (gamgi_io_token_double_scan (name, &cache.scale,
  GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid scale data", window);
    return FALSE;
    }
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_bond *bond, gamgi_window *window)
{
void (*draw) (gamgi_bond *);
char name[GAMGI_ENGINE_TOKEN];
double order, covalency, scale;
float size, red, green, blue;
gamgi_enum color;

/*************************
 * import data from bond *
 *************************/

strcpy (name, bond->object.name);

covalency = bond->covalency;
order = bond->order;

draw = bond->draw;
size = bond->size;
color = bond->color;
red = bond->red;
green = bond->green;
blue = bond->blue;
scale = bond->object.scale;

/**********************************************
 * set default: size,variancy when style      *
 * changes; red,green,blue when color changes *
 **********************************************/

if (cache.new[STATIC_STYLE] == TRUE && cache.draw != bond->draw)
  if (cache.new[STATIC_SIZE] == FALSE) size = gamgi->bond->size;

if (cache.new[STATIC_COLOR] == TRUE && cache.color != bond->color)
  {
  if (cache.new[STATIC_RED] == FALSE) red = gamgi->bond->red;
  if (cache.new[STATIC_GREEN] == FALSE) green = gamgi->bond->green;
  if (cache.new[STATIC_BLUE] == FALSE) blue = gamgi->bond->blue;
  }

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_ORDER] == TRUE) order = cache.order;
if (cache.new[STATIC_COVALENCY] == TRUE) covalency = cache.covalency;

if (cache.new[STATIC_STYLE] == TRUE) draw = cache.draw;
if (cache.new[STATIC_SIZE] == TRUE) size = cache.size;
if (cache.new[STATIC_COLOR] == TRUE) color = cache.color;
if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

if (draw == gamgi_mesa_bond_draw_line && cache.new[STATIC_SIZE] == TRUE)
  {
  /*************************************
   * size is not defined in wired mode *
   *************************************/

  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid size", window);
  return FALSE;
  }

if (color == FALSE && (cache.new[STATIC_RED] == TRUE ||
cache.new[STATIC_GREEN] == TRUE || cache.new[STATIC_BLUE] == TRUE))
  {
  /****************************************************
   * red,green,blue are not defined when color is off *
   ****************************************************/

  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid color data", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*************************
 * export data from bond *
 *************************/

strcpy (bond->object.name, name);

bond->covalency = covalency;
bond->order = order;

bond->draw = draw;
bond->size = size;
bond->color = color;
bond->red = red;
bond->green = green;
bond->blue = blue;
bond->object.scale = scale;

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

if (gamgi->bonds == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of bond objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bond");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid bond identity", window);
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
for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_BOND dlist->data, window) == FALSE)
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
gamgi_bond *bond;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/*****************************************
 * pick bond and get name (possibly new) *
 *                                       *
 * check if focus object belongs to bond *
 *****************************************/

cache.new[STATIC_NAME] = TRUE;
bond = GAMGI_CAST_BOND gamgi_gtk_object_number (dialog,
"entry_bond", GAMGI_ENGINE_BOND, window, cache.name);
if (bond == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid bond identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT bond) == TRUE) focus = TRUE;

/********************************************
 * collect data from dialog and modify bond *
 ********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (bond, window) == FALSE) return;

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
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

/************************************
 * initialize style combo box and   *
 * everything that depends of style *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_style (NULL, window);

/************************************
 * initialize color combo box and   *
 * everything that depends of color *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_color (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
gamgi_bond *bond;
char string[2 * GAMGI_ENGINE_TOKEN];

bond = GAMGI_CAST_BOND object;

static_reset (window);

/***********************
 * Get name and number *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bond");
sprintf (string, "%s %d", bond->object.name, bond->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/************************************
 * Get properties: covalency, order *
 ************************************/

if (bond->covalency + GAMGI_MATH_TOLERANCE >= 0)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_covalency");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_ORDER, bond->covalency);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_order");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_ORDER, bond->order);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*************
 * Get style *
 *************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (bond->draw == gamgi_mesa_bond_draw_line) 
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (bond->draw == gamgi_mesa_bond_draw_cylinder)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);
static_style (NULL, window);

/************
 * Get size *
 ************/

if (bond->draw == gamgi_mesa_bond_draw_cylinder)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SIZE, bond->size);
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/*************
 * Get color *
 *************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
if (bond->color == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
if (bond->color == FALSE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);
static_color (NULL, window);

/**********************
 * get red,green,blue *
 **********************/

if (bond->color == TRUE)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond->red);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond->green);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, bond->blue);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/******************
 * Get scale data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, bond->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_BOND_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_bond_modify_press (gamgi_window *window_mouse,
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
GAMGI_ENGINE_BOND, FALSE, static_press);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_BOND_MODIFY              *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_bond_modify (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
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

dialog = gamgi_gtk_dialog_task0_create ("Bond Modify", window);
window->action = GAMGI_GTK_BOND_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Bond objects *
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

label = gtk_label_new ("Bond");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_bond", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_bond", entry);
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
 * property page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Property");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

/*********
 * Order *
 *********/

label = gtk_label_new ("Order");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

combo = gtk_combo_box_entry_new ();
gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 30, 5);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "combo_order", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ORDER, 1.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ORDER, 1.5);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ORDER, 2.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ORDER, 3.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

gtk_combo_box_entry_set_text_column (GTK_COMBO_BOX_ENTRY (combo), 0);

/*************
 * Covalency *
 *************/

label = gtk_label_new ("Covalency");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 30, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_covalency", entry);
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

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********************
 * Style option menu *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Style");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
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

/******************
 * Size combo box *
 ******************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Size");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_size", label);
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

/*********
 * Scale *
 *********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_scale", entry);
gtk_widget_show (entry);

/******************************
 * Color first row: combo box *
 ******************************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Color");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_color", label);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_color), window);
g_object_set_data (G_OBJECT (dialog), "combo_color", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Yes", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "No", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/***************************
 * Color second row: R G B *
 ***************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_blue", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
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
