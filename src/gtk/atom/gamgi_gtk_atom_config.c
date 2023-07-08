/***********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_config.c
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
#include "gamgi_chem.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_atom_table.h"
#include "gamgi_gtk_atom_property.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_lists.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_chem_atom.h"
#include "gamgi_io_token.h"
#include "gamgi_global_copy.h"
#include "gamgi_global_remove.h"

static void static_element (GtkWidget *widget, void *data);
static void static_number (GtkWidget *widget, void *data);

/************************ internal function *************************
 *                                                                  *
 *                          STATIC_DEFAULT                          *
 *                                                                  *
 * This call is executed everytime element name or number entries   *
 * change. If the element name or number is recognized, the default *
 * values are shown. Otherwise, these entries are cleaned up.       *
 *                                                                  *
 ********************************************************************/

static void static_default (int number, char *name, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_mass, *entry_radius;
GtkWidget *entry_red, *entry_green, *entry_blue;
char token[GAMGI_ENGINE_TOKEN];
double radius;

entry_mass = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
entry_radius = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");

if (number >= 0)
  {
  /****************************************************
   * A valid element was found: output default values *
   ****************************************************/

  /***********************
   *          Mass       *
   *                     *
   * Radius (if defined) *
   ***********************/

  sprintf (token, "%.*f", gamgi->gamgi->mass, gamgi->atom->mass[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_mass), token);

  radius = gamgi->atom->radius[number];
  if (radius > 0)
    {
    sprintf (token, "%.*f", gamgi->gamgi->length, radius);
    gtk_entry_set_text (GTK_ENTRY (entry_radius), token);
    }
  else gtk_entry_set_text (GTK_ENTRY (entry_radius), "");

  /***************
   * R,G,B color *
   ***************/

  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->atom->red[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->atom->green[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->atom->blue[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  /****************************************
   * The element is not defined: clean    *
   * everything related with the element. *
   ****************************************/

  gtk_entry_set_text (GTK_ENTRY (entry_mass), "");
  gtk_entry_set_text (GTK_ENTRY (entry_radius), "");

  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  }

}

static void static_element (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
int number;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");

/**************************************************
 * block changed signal for number entry as       *
 * changes done in this function to that entry    *
 * should not be used to change name entry again! *
 **************************************************/

g_signal_handlers_block_by_func (entry, static_number, window);

/************************************
 * get name, get number, set number *
 ************************************/

name = gtk_entry_get_text (GTK_ENTRY (widget));
if (gamgi_io_token_alpha_scan (name, token,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == TRUE &&
gamgi_chem_atom_number (token, &number) == TRUE)
  {
  static_default (number, token, window);
  sprintf (token, "%d", number);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else
  {
  static_default (-1, NULL, window);
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  }

/**************************************************
 * unblock changed signal for number entry, so it *
 * accepts changes done directly in that entry    *
 **************************************************/

g_signal_handlers_unblock_by_func (entry, static_number, window);
}

static void static_number (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
int number;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element");

/********************************************
 * block changed signal for name entry so   *
 * changes done here to that entry will not *
 * be used to change number entry again!    *
 ********************************************/

g_signal_handlers_block_by_func (entry, static_element, window);

/**********************************
 * get number, get name, set name *
 **********************************/

name = gtk_entry_get_text (GTK_ENTRY (widget));
if (gamgi_io_token_int_scan (name, &number,
0, GAMGI_CHEM_ATOM_MAX) == TRUE)
  {
  gamgi_chem_atom_name (number, token);
  static_default (number, token, window);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else
  {
  static_default (-1, NULL, window);
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  }

/************************************************
 * unblock changed signal for name entry, so it *
 * accepts changes done directly in that entry  *
 ************************************************/

g_signal_handlers_unblock_by_func (entry, static_element, window);
}

static void static_button (GtkWidget *dialog, char *string)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), string);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_SWITCH                 *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;

if (window->dialog1 != NULL)
  {
  /*********************************************************
   * close second level dialogs when notebook page changes *
   *********************************************************/

  static_button (dialog, "button_table");
  static_button (dialog, "button_mass");
  static_button (dialog, "button_radius");
  }

}

/**************** internal function ***************
 *                                                *
 *                   STATIC_TABLE                 *
 *                                                *
 * Launch or remove a second dialog containing    *
 * a Periodic Table and a Cancel button. This     *
 * call back function is executed when the user   *
 * presses either the Table button in the Atom    *
 * Create dialog (level one) or the Cancel button *
 * in the Periodic Table dialog (level two).      *
 *                                                *
 **************************************************/

static void static_table (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_table");

/**************************
 * Create Periodic Table. *
 **************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  gamgi_gtk_atom_table (widget, window);

/**************************
 * Remove Periodic Table. *
 **************************/

else gamgi_gtk_dialog_task1_remove (widget, window);
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
gamgi_atom_class *atom_class, *atom_class_old;
GtkWidget *combo;
GtkWidget *entry;
double mass, radius;
float red, green, blue;
int element, color = 0;
const char *name;
int row;

atom_class = gamgi_global_copy_atom (gamgi->atom);

/**********************************************************************
 * get Element default parameters: element,mass,radius,red,green,blue *
 **********************************************************************/

/***********
 * element *
 ***********/

element = -1;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_int_scan (name, &element, 0, GAMGI_CHEM_ATOM_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid element", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/***************
 * mass,radius *
 ***************/

mass = -1.0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_double_scan (name, &mass, 
GAMGI_MATH_TOLERANCE_MASS, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid mass", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

radius = -1.0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_double_scan (name, &radius, 
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid radius", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

 /******************
 * red,green,blue *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  color++;
  if (gamgi_io_token_float_scan (name, &red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_global_remove_atom (atom_class);
    return;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  color++;
  if (gamgi_io_token_float_scan (name, &green, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_global_remove_atom (atom_class);
    return;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  color++;
  if (gamgi_io_token_float_scan (name, &blue, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_global_remove_atom (atom_class);
    return;
    }
  }
 
/************************************************
 * get Analysis default parameters: temperature *
 ************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom_class->temperature, 0.0, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid temperature", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/********************************************************
 * get View default parameters: style,size,variancy,min *
 ********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == GAMGI_MESA_WIRED - 1) atom_class->draw = gamgi_mesa_atom_draw_cross;
if (row == GAMGI_MESA_SOLID - 1) atom_class->draw = gamgi_mesa_atom_draw_sphere;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &atom_class->size,
GAMGI_MATH_TOLERANCE, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid size", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &atom_class->variancy,
0.0, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid variancy", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_min");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom_class->min,
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid minimum radius", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/******************************************************************************
 * get Global parameters: slices (for solid spheres), width (for wired lines) *
 ******************************************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_slices");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &atom_class->slices,
GAMGI_MESA_ATOM_SLICES_MIN, GAMGI_MESA_ATOM_SLICES_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid slices", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_width");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &atom_class->width,
GAMGI_MESA_ATOM_WIDTH_MIN, GAMGI_MESA_ATOM_WIDTH_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid line width", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/*************************************************************
 * red,green,blue are coupled parameters: define all or none *
 *************************************************************/

if (color == 1 || color == 2)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/******************************
 * properties without element *
 ******************************/

if (element == -1 && (mass != -1.0 || radius != -1.0 || color != 0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid element data", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/******************************
 * element without properties *
 ******************************/

if (element != -1 && (mass == -1.0 && radius == -1.0 && color == 0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid element data", window);
  gamgi_global_remove_atom (atom_class);
  return;
  }

/************************************************
 * set element properties, after validity tests *
 ************************************************/

if (mass != -1.0) atom_class->mass[element] = mass;
if (radius != -1.0) atom_class->radius[element] = radius;
if (color == 3)
  {
  atom_class->red[element] = red;
  atom_class->green[element] = green;
  atom_class->blue[element] = blue;
  }
  
/******************************************
 * recreate OpenGL pre-compiled shared    *
 * lists with the new width/slices values *
 ******************************************/

if (atom_class->width != gamgi->atom->width)
  {
  gamgi_mesa_lists_cross (atom_class->width);
  gamgi_mesa_lists_line (atom_class->width);
  }

if (atom_class->slices != gamgi->atom->slices)
  {
  gamgi_mesa_lists_sphere (atom_class->slices, atom_class->slices);
  gamgi_mesa_lists_cylinder_1 (atom_class->slices, 1);
  gamgi_mesa_lists_cylinder_3 (atom_class->slices, 1);
  gamgi_mesa_lists_cylinder_5 (atom_class->slices, 1);
  }

/*******************************************************
 * unsetting the old class object before removing it   *
 * is marginally better because this way the official  *
 * object address is always occupied by a valid object *
 *******************************************************/

atom_class_old = gamgi->atom;
gamgi->atom = atom_class;
gamgi_global_remove_atom (atom_class_old);

/*****************************************************
 *    Remove dialog and redraw ALL top windows       *
 * (because of global parameters: slices,size,width) *
 *****************************************************/

gamgi_gtk_dialog_task0_remove (widget, window);
gamgi_mesa_draw_gamgi ();
}

static void static_init (gamgi_window *window)
{
gamgi_atom_class *atom_class = gamgi->atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/**************************
 * initialize temperature *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_OCCUPANCY, atom_class->temperature);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/************************************
 * initialize style, size, variancy *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (atom_class->draw == gamgi_mesa_atom_draw_cross)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (atom_class->draw == gamgi_mesa_atom_draw_sphere)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, atom_class->size);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), token);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_VARIANCY, atom_class->variancy);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************************************
 * initialise minimum radius, slices, width *
 ********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_min");
sprintf (token, "%.*f", gamgi->gamgi->length, atom_class->min);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_slices");
sprintf (token, "%d", atom_class->slices);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_width");
sprintf (token, "%d", atom_class->width);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

void gamgi_gtk_atom_config (GtkWidget *widget, void *data)
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
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
char token[GAMGI_ENGINE_TOKEN];

/******************
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Atom Config", window);
window->action = GAMGI_GTK_ATOM_CONFIG;
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
g_signal_connect (notebook, "switch_page",
G_CALLBACK (static_switch), window);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*************
 * Type page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Type ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************************
 * element, number, table *
 **************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Element");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_element), window);
g_object_set_data (G_OBJECT (dialog), "entry_element", entry);
gtk_widget_show (entry);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Number");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_number), window);
g_object_set_data (G_OBJECT (dialog), "entry_number", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Table");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_table), window);
g_object_set_data (G_OBJECT (dialog), "button_table", button);
gtk_widget_show (button);

/****************
 * mass, radius *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Mass");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_mass", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_atom_property_mass), window);
g_object_set_data (G_OBJECT (dialog), "button_mass", button);
gtk_widget_show (button);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Radius");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_radius", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_atom_property_radius), window);
g_object_set_data (G_OBJECT (dialog), "button_radius", button);
gtk_widget_show (button);

/*********
 * color *
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
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_EXPAND, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_blue", entry);
gtk_widget_show (entry);
 
/*****************
 * Analysis page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Analysis");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/***************
 * temperature *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Temperature");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_temperature", entry);
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

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

/***************
 * left column *
 ***************/

vbox_top = gtk_vbox_new (TRUE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********
 * style *
 *********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
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

label = gtk_label_new ("Style");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/************
 * variancy *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

combo = gtk_combo_box_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "combo_variancy", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 1.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

gtk_combo_box_entry_set_text_column (GTK_COMBO_BOX_ENTRY (combo), 0);

label = gtk_label_new ("Variancy");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_variancy", label);
gtk_widget_show (label);

/****************
 * right column *
 ****************/

vbox_top = gtk_vbox_new (TRUE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * size *
 ********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

combo = gtk_combo_box_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
entry = gtk_bin_get_child (GTK_BIN (combo));
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "combo_size", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.5);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 0.8);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_list_store_append (store, &iter);
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, 1.0);
gtk_list_store_set (store, &iter, 0, token, -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

gtk_combo_box_entry_set_text_column (GTK_COMBO_BOX_ENTRY (combo), 0);

label = gtk_label_new ("Size");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_size", label);
gtk_widget_show (label);

/**********************************
 * minimum radius (H, by default) *
 **********************************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_min", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Min");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/***************
 * Global page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Global");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***************************
 * variancy, slices, width *
 ***************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/******************************
 * spherical/cylindric Slices *
 ******************************/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Slices");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_object_set_data (G_OBJECT (dialog), "entry_slices", entry);
gtk_widget_show (entry);

/**************
 * line width *
 **************/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Width");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_2);
g_object_set_data (G_OBJECT (dialog), "entry_width", entry);
gtk_widget_show (entry);

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
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
