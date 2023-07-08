/***********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_create.c
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
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_link.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_atom_table.h"
#include "gamgi_gtk_atom_property.h"
#include "gamgi_mesa_area.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_chem_atom.h"
#include "gamgi_io_token.h"

static void static_element (GtkWidget *widget, void *data);
static void static_number (GtkWidget *widget, void *data);

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label_size, *label_variancy;
GtkWidget *combo_style, *combo_size, *combo_variancy;
char token[GAMGI_ENGINE_TOKEN];
int row;

/****************
 * update style *
 ****************/

combo_style = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_style));

combo_size = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
label_size = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_size");

combo_variancy = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
label_variancy = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_variancy");

if (row == GAMGI_MESA_WIRED - 1)
  {
  entry = gtk_bin_get_child (GTK_BIN (combo_size));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (combo_size, FALSE);
  gtk_widget_set_sensitive (label_size, FALSE);

  entry = gtk_bin_get_child (GTK_BIN (combo_variancy));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (combo_variancy, FALSE);
  gtk_widget_set_sensitive (label_variancy, FALSE);
  }

if (row == GAMGI_MESA_SOLID - 1)
  {
  gtk_widget_set_sensitive (combo_size, TRUE);
  gtk_widget_set_sensitive (label_size, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, gamgi->atom->size);
  entry = gtk_bin_get_child (GTK_BIN (combo_size));
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  gtk_widget_set_sensitive (combo_variancy, TRUE);
  gtk_widget_set_sensitive (label_variancy, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_VARIANCY, gamgi->atom->variancy);
  entry = gtk_bin_get_child (GTK_BIN (combo_variancy));
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

}

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
GtkWidget *entry_atom, *entry_label;
GtkWidget *entry_mass, *entry_radius, *entry_charge;
GtkWidget *entry_red, *entry_green, *entry_blue;
char token[GAMGI_ENGINE_TOKEN];

entry_atom = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
entry_label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_label");

entry_mass = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
entry_radius = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
entry_charge = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");

if (number >= 0)
  {
  /****************************************************
   * A valid element was found: output default values *
   ****************************************************/

  /***********************************************
   *                   Atom name                 *
   *                                             *
   * Atom label (the element name in lower case) *
   ***********************************************/

  gtk_entry_set_text (GTK_ENTRY (entry_atom), name);

  gamgi_io_token_lower (name, token);
  gtk_entry_set_text (GTK_ENTRY (entry_label), token);

  /***********************
   *          Mass       *
   *                     *
   * Radius (if defined) *
   ***********************/

  sprintf (token, "%.*f", gamgi->gamgi->mass, gamgi->atom->mass[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_mass), token);

  sprintf (token, "%.*f", gamgi->gamgi->length, gamgi->atom->radius[number]);
  gtk_entry_set_text (GTK_ENTRY (entry_radius), token);

  sprintf (token, "%.*f", gamgi->gamgi->charge, 0.0);
  gtk_entry_set_text (GTK_ENTRY (entry_charge), token);
  
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

  gtk_entry_set_text (GTK_ENTRY (entry_atom), "");
  gtk_entry_set_text (GTK_ENTRY (entry_label), "");

  gtk_entry_set_text (GTK_ENTRY (entry_mass), "");
  gtk_entry_set_text (GTK_ENTRY (entry_radius), "");
  gtk_entry_set_text (GTK_ENTRY (entry_charge), "");

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
  static_button (dialog, "button_charge");
  }

}

/**************** internal function *************
 *                                              *
 *                   STATIC_RESET               *
 *                                              *
 * Reset the atom create dialog, cleaning only  *
 * the xyz entries, because the user might wish *
 * to create more atoms of the same element.    *
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
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** internal function ****************
 *                                                *
 *                    STATIC_OK                   *
 *                                                *
 *  This is the function that actually creates    *
 *  a new atom. It is triggered when the user     *
 * presses Ok or when she/he cliks on the screen, *
 * to indicate the position of the new atom.      *
 *                                                *
 **************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_atom *atom;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
const char *name_red, *name_green, *name_blue;
const char *name;
int row;

atom = gamgi_engine_create_atom ();
gamgi_engine_start_atom (atom);

/**********************
 * Get element number *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &atom->element, 0, GAMGI_CHEM_ATOM_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid element", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/*****************
 * Get atom name *
 *****************/

gamgi_chem_atom_name (atom->element, token);
if (gamgi_gtk_object_name (dialog, "entry_atom",
token, atom->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom identity", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/*************
 * Get label *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_label");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, atom->object.label,
GAMGI_IO_TEXT, GAMGI_CHEM_ATOM_LABEL + 1) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid label", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/************
 * Get mass *
 ************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->mass, 
GAMGI_MATH_TOLERANCE_MASS, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid mass", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/**************
 * Get radius *
 **************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->radius, 
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid radius", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/**************
 * Get charge *
 **************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->charge, -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid charge", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/***************
 * temperature *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->temperature,
GAMGI_CHEM_ATOM_TEMPERATURE_MIN, GAMGI_CHEM_ATOM_TEMPERATURE_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid temperature", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/*************
 * occupancy *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_occupancy");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->occupancy, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid temperature", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/********************
 * Get x coordinate *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name,
&atom->position[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/********************
 * Get y coordinate *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name,
&atom->position[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/********************
 * Get z coordinate *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name,
&atom->position[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/*************
 * Get color *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name_red = gtk_entry_get_text (GTK_ENTRY (entry));
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name_green = gtk_entry_get_text (GTK_ENTRY (entry));
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name_blue = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name_red) == FALSE 
&& gamgi_io_token_check (name_green) == FALSE
&& gamgi_io_token_check (name_blue) == FALSE)
  {
  atom->red = gamgi->atom->red[atom->element];
  atom->green = gamgi->atom->green[atom->element];
  atom->blue = gamgi->atom->blue[atom->element];
  }
else
  {
  /***************
   * Get r color *
   ***************/

  if (gamgi_io_token_float_scan (name_red, &atom->red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_atom (atom);
    return;
    }

  /***************
   * Get g color *
   ***************/

  if (gamgi_io_token_float_scan (name_green, &atom->green, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_atom (atom);
    return;
    }

  /***************
   * Get b color *
   ***************/

  if (gamgi_io_token_float_scan (name_blue, &atom->blue, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_atom (atom);
    return;
    }
  }

/*******************************************
 * get style: atom->draw sets a pointer to *
 * the function which will draw this atom. *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == GAMGI_MESA_WIRED - 1) atom->draw = gamgi_mesa_atom_draw_cross;
if (row == GAMGI_MESA_SOLID - 1) atom->draw = gamgi_mesa_atom_draw_sphere;

/********
 * size *
 ********/

atom->size = gamgi->atom->size;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (atom->draw == gamgi_mesa_atom_draw_sphere &&
gamgi_io_token_float_scan (name, &atom->size, GAMGI_MATH_TOLERANCE, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid size", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/************
 * variancy *
 ************/
 
atom->variancy = gamgi->atom->variancy;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (atom->draw == gamgi_mesa_atom_draw_sphere &&
gamgi_io_token_float_scan (name, &atom->variancy, 0.0, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid variancy", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/*************
 * Get scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &atom->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid scale", window);
  gamgi_engine_remove_atom (atom);
  return;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_atom_layer (atom, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the atom create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_init (gamgi_window *window)
{
gamgi_atom_class *atom_class = gamgi->atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

/******************************
 * initialize style combo box *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (atom_class->draw == gamgi_mesa_atom_draw_cross)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (atom_class->draw == gamgi_mesa_atom_draw_sphere)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);

/************************************
 * initialize temperature,occupancy *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_OCCUPANCY, atom_class->temperature);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_occupancy");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_OCCUPANCY, 1.0);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***********************
 * start position page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ATOM_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * initialize scale *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/***********************************************
 * initialize everything that depends of style *
 ***********************************************/

static_style (NULL, window);
}



/*************** external function *************
 *                                             *
 *          GAMGI_GTK_ATOM_CREATE_PRESS        *
 *                                             *
 * Receives the 2D window x,y coordinates,     *
 * determines the 3D x,y,z object coordinates, *
 * prints them in the x, y, z entries and      *
 * runs static_ok (), as if the user had       *
 * pressed the Ok button.                      *
 *                                             *
 ***********************************************/

void gamgi_gtk_atom_create_press (gamgi_window *window_mouse,
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
 *             GAMGI_GTK_ATOM_CREATE                *
 *                                                  *
 *  Creates the dialog window used to create atoms. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_atom_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
char token[GAMGI_ENGINE_TOKEN];

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Atom Create", window);
window->action = GAMGI_GTK_ATOM_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*************
 * Atom name *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
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
g_object_set_data (G_OBJECT (dialog), "entry_atom", entry);
gtk_widget_show (entry);

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

/*******************
 * Element, Number *
 *******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 25);
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

/********************************
 * Label, Periodic Table Button *
 ********************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Label");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_CHEM_ATOM_LABEL);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_CHEM_ATOM_LABEL);
g_object_set_data (G_OBJECT (dialog), "entry_label", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Table");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_table), window);
g_object_set_data (G_OBJECT (dialog), "button_table", button);
gtk_widget_show (button);

/*****************
 * Property page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Property");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/************************
 * mass, radius, charge *
 ************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * mass *
 ********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_atom_property_mass), window);
g_object_set_data (G_OBJECT (dialog), "button_mass", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_mass", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Mass");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/**********
 * radius *
 **********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_atom_property_radius), window);
g_object_set_data (G_OBJECT (dialog), "button_radius", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_radius", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Radius");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/**********
 * charge *
 **********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_atom_property_charge), window);
g_object_set_data (G_OBJECT (dialog), "button_charge", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_charge", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Charge");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

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

/*************************
 * temperature,occupancy *
 *************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_temperature", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Temperature");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_occupancy", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Occupancy");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

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

/**********
 * Origin *
 **********/

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

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*******************
 * Style, Variancy *
 *******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

vbox_top_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_style), window);
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
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
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
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_variancy", label);
gtk_widget_show (label);

/***************
 * Size, Scale *
 ***************/

vbox_top_top = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
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
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_size", label);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_scale", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Scale");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

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
