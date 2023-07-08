/***********************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_modify.c
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

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_atom_table.h"
#include "gamgi_gtk_atom_property.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_chem_atom.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_modify.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

static void static_element (GtkWidget *widget, void *data);
static void static_number (GtkWidget *widget, void *data);

static struct {
gamgi_bool local;
gamgi_bool new[18];
char name[GAMGI_ENGINE_TOKEN];
char label[GAMGI_ENGINE_TOKEN];
int element;
double mass;
double radius;
double charge;
double temperature;
double occupancy;
gamgi_bool absolute;
double translation[3];
void (*draw) (gamgi_atom *);
float size;
float variancy;
float red, green, blue;
double scale; } cache;

enum { STATIC_NAME = 0, STATIC_LABEL, STATIC_ELEMENT, STATIC_MASS,
STATIC_RADIUS, STATIC_CHARGE, STATIC_TEMPERATURE, STATIC_OCCUPANCY,
STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y, STATIC_TRANSLATION_Z,
STATIC_STYLE, STATIC_SIZE, STATIC_VARIANCY, STATIC_RED, STATIC_GREEN,
STATIC_BLUE, STATIC_SCALE };

/*****************************
 * data functions start here *
 *****************************/

static void static_name (gamgi_window *window, char *name)
{
gamgi_atom *atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char string[GAMGI_ENGINE_STRING];
int active;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");

atom = GAMGI_CAST_ATOM gamgi_gtk_object_number (dialog,
"entry_atom", GAMGI_ENGINE_ATOM, window, string);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

if (active == TRUE || atom == NULL)
  {
  if (name == NULL)
    sprintf (string, "%s", "");
  else
    sprintf (string, "%s", name);
  }
else
  {
  if (name == NULL)
    sprintf (string, "%s %d", atom->object.name, atom->object.number);
  else
    sprintf (string, "%s %d", name, atom->object.number);
  }

gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/******************** internal function ****************
 *                                                     *
 *                     STATIC_ABSOLUTE                 *
 *                                                     *
 * Set the absolute radio button to its toggled state. *
 *******************************************************/

static void static_absolute (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_absolute");
gtk_button_clicked (GTK_BUTTON (button));
}

/******************** internal function ****************
 *                                                     *
 *                     STATIC_RELATIVE                 *
 *                                                     *
 * Ret the relative radio button to its toggled state. *
 *******************************************************/

static void static_relative (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_relative");
gtk_button_clicked (GTK_BUTTON (button));
}

/************ internal function **************
 *                                           *
 *              STATIC_ORIGIN                *
 *                                           *
 * Reset the x,y,z entries everytime the     *
 * absolute or relative buttons are toggled. *
 *                                           *
 *********************************************/

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
else
  {
  gtk_widget_set_sensitive (combo_size, TRUE);
  gtk_widget_set_sensitive (label_size, TRUE);

  gtk_widget_set_sensitive (combo_variancy, TRUE);
  gtk_widget_set_sensitive (label_variancy, TRUE);
  }

if (row == GAMGI_MESA_SOLID - 1)
  {
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, gamgi->atom->size);
  entry = gtk_bin_get_child (GTK_BIN (combo_size));
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_VARIANCY, gamgi->atom->variancy);
  entry = gtk_bin_get_child (GTK_BIN (combo_variancy));
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else
  {
  entry = gtk_bin_get_child (GTK_BIN (combo_size));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  entry = gtk_bin_get_child (GTK_BIN (combo_variancy));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
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
GtkWidget *entry_label;
GtkWidget *entry_mass, *entry_radius, *entry_charge;
GtkWidget *entry_red, *entry_green, *entry_blue;
char token[GAMGI_ENGINE_TOKEN];

entry_label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_label");

entry_mass = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
entry_radius = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
entry_charge = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");

if (number >= 0)
  {
  /***************************************************************
   * A valid element was previously found: output default values *
   ***************************************************************/

  /***********************************************
   *                   Atom name                 *
   *                                             *
   * Atom label (the element name in lower case) *
   ***********************************************/

  static_name (window, name);
  gamgi_io_token_lower (name, token);
  gtk_entry_set_text (GTK_ENTRY (entry_label), token);

  /************************
   * Mass, Radius, Charge *
   ************************/

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

  static_name (window, NULL);
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

/**********************************
 * Periodic Table: create, remove *
 **********************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  gamgi_gtk_atom_table (widget, window);
else
  gamgi_gtk_dialog_task1_remove (widget, window);
}

static void static_button (GtkWidget *dialog, char *string)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), string);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

/******************** internal function ****************
 *                                                     *
 *                    STATIC_RESET_DATA                *
 *                                                     *
 * Reset the atom modify dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset_data (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
gtk_entry_set_text (GTK_ENTRY (entry), "");
static_number (entry, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_occupancy");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
static_style (NULL, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/******************** internal function ****************
 *                                                     *
 *                    STATIC_SWITCH_DATA               *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_switch_data (GtkNotebook *notebook,
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

static void static_press_data (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_atom *atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry, *entry_element, *entry_number;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];
 
atom = GAMGI_CAST_ATOM object;

/***************************************************************
 * Toggles the absolute coordinates radio buttons, because     *
 * atom coordinates are absolute. This in turn triggers the    *
 * coordinate cleaning functions. Therefore static_absolute () *
 * must be called before setting the atom coordinates.         *
 ***************************************************************/

static_reset_data (window);
static_absolute (dialog);

/************************************************************
 * show atom name, element name, element number, atom label *
 *                                                          *
 * block,unblock changed signal in element name,number      *
 * entries, to avoid changes back in the initial entry!     *
 ************************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
sprintf (string, "%s %d", atom->object.name, atom->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry_number = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
entry_element = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element");

g_signal_handlers_block_by_func (entry_number, static_number, window);
g_signal_handlers_block_by_func (entry_element, static_element, window);

sprintf (string, "%d", atom->element);
gtk_entry_set_text (GTK_ENTRY (entry_number), string);

gamgi_chem_atom_name (atom->element, string);
gtk_entry_set_text (GTK_ENTRY (entry_element), string);

g_signal_handlers_unblock_by_func (entry_number, static_number, window);
g_signal_handlers_unblock_by_func (entry_element, static_element, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_label");
gtk_entry_set_text (GTK_ENTRY (entry), atom->object.label);

/******************************************************
 * show radius and charge only if they contain real   *
 * values (not all atomic elements have known radius) *
 ******************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
sprintf (string, "%.*f", gamgi->gamgi->mass, atom->mass);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
sprintf (string, "%.*f", gamgi->gamgi->length, atom->radius);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
sprintf (string, "%.*f", gamgi->gamgi->charge, atom->charge);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*******************************
 * show temperature,occcupancy *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_OCCUPANCY, atom->temperature);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_occupancy");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_OCCUPANCY, atom->occupancy);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/****************
 * get position *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (string, "%.*f", gamgi->gamgi->length, atom->position[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (string, "%.*f", gamgi->gamgi->length, atom->position[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (string, "%.*f", gamgi->gamgi->length, atom->position[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**********************
 * get red,green,blue *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, atom->red);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, atom->green);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, atom->blue);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*************
 * get style *
 *************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (atom->draw == gamgi_mesa_atom_draw_cross)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (atom->draw == gamgi_mesa_atom_draw_sphere)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);
static_style (NULL, window);

/**********************
 * get size, variancy *
 **********************/

if (atom->draw == gamgi_mesa_atom_draw_sphere)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SIZE, atom->size);
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_VARIANCY, atom->variancy);
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/*************
 * get scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, atom->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*********************************
 * geometry functions start here *
 *********************************/

static GtkWidget *static_status (gamgi_window *window, int *page, int *quantity)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *vbox;
GtkWidget *combo;

/****************
 * get notebook *
 ****************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
*page = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));

/*****************************************
 * get quantity or return NULL when this *
 * function is called out of context     *
 *****************************************/

switch (*page)
  {
  case 5: case 6:
  vbox = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), *page);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "combo_quantity");
  *quantity = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  return vbox;
  }
  
return NULL;
}

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget* entry;
GtkWidget* vbox;
GtkWidget* button;
int page, property;
const char *name;

/***************************
 * property mode: get atom *
 ***************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_data");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  return GAMGI_ENGINE_ATOM;

/***********************************
 * geometry mode level 0: get atom *
 ***********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE)
  return GAMGI_ENGINE_ATOM;

/********************************************
 * select object class from notebook status *
 ********************************************/

vbox = static_status (window, &page, &property);

/***********
 * level 1 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 3 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_DIRECTION;
  if (property == 4 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_PLANE;
  break;

  case 6:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

/***********
 * level 2 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;

  case 6:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

/***********
 * level 3 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;

  case 6:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

return GAMGI_ENGINE_ATOM;
}

static GtkWidget *static_entry (GtkWidget *vbox, 
int page, int property, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

/***********
 * level 0 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return entry;

/***********
 * level 1 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 3 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 4 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 6:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

/***********
 * level 2 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 6:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

/***********
 * level 3 *
 ***********/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 5:
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 6:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

return NULL;
}

static gamgi_plane *static_plane (GtkWidget *widget,
char *id, gamgi_window *window)
{
gamgi_plane *plane;

/*************
 * Get Plane *
 *************/

plane = GAMGI_CAST_PLANE gamgi_gtk_object_name_number (widget,
id, GAMGI_ENGINE_PLANE, window);
if (plane == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid plane object", window);
  return NULL;
  }

return plane;
}

static gamgi_direction *static_direction (GtkWidget *widget,
char *id, gamgi_window *window)
{
gamgi_direction *direction;

/*****************
 * Get Direction *
 *****************/

direction = GAMGI_CAST_DIRECTION gamgi_gtk_object_name_number (widget,
id, GAMGI_ENGINE_DIRECTION, window);
if (direction == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid direction object", window);
  return NULL;
  }

return direction;
}

static gamgi_atom *static_atom (GtkWidget *widget,
char *id, gamgi_window *window)
{
gamgi_atom *atom;

/************
 * Get Atom *
 ************/

atom = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (widget,
id, GAMGI_ENGINE_ATOM, window);
if (atom == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom object", window);
  return NULL;
  }

return atom;
}

/******************** internal function ****************
 *                                                     *
 *                   STATIC_RESET_GEOMETRY             *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_reset_geometry (GtkWidget *vbox,
int page, int property, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/**************
 * clean page *
 **************/

switch (page)
  {
  case 5: case 6:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  gtk_entry_set_text (GTK_ENTRY (entry), "");

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  }

}

static void static_quantity (GtkWidget *vbox,
int page, int property, gamgi_window *window)
{
GtkWidget *label1, *label2, *label3;
GtkWidget *entry1, *entry2, *entry3;

/****************
 * clean page *
 ****************/

static_reset_geometry (vbox, page, property, window);
entry1 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_value");
gtk_entry_set_text (GTK_ENTRY (entry1), "");

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

label1 = label2 = label3 = NULL;
entry1 = entry2 = entry3 = NULL;

/***************************************
 * get widgets for all levels except 0 *
 ***************************************/

switch (page)
  {
  case 5: case 6:
  entry1 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  label1 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object1");
  entry2 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  label2 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object2");
  entry3 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  label3 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object3");
  }

/**************************************
 * set labels for all levels except 0 *
 **************************************/

switch (page)
  {
  case 5:
  switch (property)
    {
    case 0:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;
 
    case 1:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 2:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_label_set_text (GTK_LABEL (label3), "Atom");
    break;

    case 3:
    gtk_label_set_text (GTK_LABEL (label1), "Direction");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 4:
    gtk_label_set_text (GTK_LABEL (label1), "Plane");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;
    }
  break;

  case 6:
  switch (property)
    {
    case 0:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 1:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_label_set_text (GTK_LABEL (label3), "Atom");
    break;
    }
  break;
  }

/***************************************
 * set widgets for all levels except 0 *
 ***************************************/

switch (page)
  {
  case 5:
  switch (property)
    {
    case 0:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 1:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 2:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, TRUE);
    gtk_widget_set_sensitive (label3, TRUE);
    break;

    case 3:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 4:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;
    }
  break;

  case 6:
  switch (property)
    {
    case 0:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 1:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, TRUE);
    gtk_widget_set_sensitive (label3, TRUE);
    break;
    }
  break;
  }

}

static void static_activate (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *vbox;
int page, property;

/***********************
 * get notebook status *
 ***********************/

vbox = static_status (window, &page, &property);
if (vbox == NULL) return;

/*******************
 * update property *
 *******************/

static_quantity (vbox, page, property, window);
}

/******************** internal function ****************
 *                                                     *
 *                   STATIC_SWITCH_GEOMETRY            *
 *                                                     *
 *                                                     *
 *******************************************************/
  
static void static_switch_geometry (GtkNotebook *notebook, 
GtkNotebookPage *notebook_page, int page, void *data)
{ 
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *combo;
GtkWidget* vbox;
  
/***********************************************
 * reset menu, object widgets, object contents *
 ***********************************************/

vbox = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), page);
combo = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "combo_quantity");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
static_quantity (vbox, page, 0, window);
}

/******************** internal function ****************
 *                                                     *
 *                   STATIC_OK_GEOMETRY                *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_ok_geometry (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_atom *atom0;
gamgi_atom *atom1;
gamgi_atom *atom2;
gamgi_atom *atom3;
gamgi_plane *plane1;
gamgi_direction *direction1;
GtkWidget* vbox;
GtkWidget* entry;
const char *name;
double translate[3], origin[3], rotate[9];
double value;
int page, property;

atom0 = static_atom (dialog, "entry_atom", window);
if (atom0 == NULL) return;

/**************
 * get status *
 **************/

vbox = static_status (window, &page, &property);

/***************
 * get objects *
 ***************/

atom1 = NULL; atom2 = NULL; atom3 = NULL;
plane1 = NULL; direction1 = NULL;

switch (page)
  {
  case 5:
  switch (property)
    {
    case 0:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    break;

    case 1:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    break;

    case 2:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    atom3 = static_atom (vbox, "entry_object3", window);
    if (atom3 == NULL) return;
    break;

    case 3:
    direction1 = static_direction (vbox, "entry_object1", window);
    if (direction1 == NULL) return;
    break;

    case 4:
    plane1 = static_plane (vbox, "entry_object1", window);
    if (plane1 == NULL) return;
    break;
    }
  break;

  case 6:
  switch (property)
    {
    case 0:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    break;

    case 1:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    atom3 = static_atom (vbox, "entry_object3", window);
    if (atom3 == NULL) return;
    break;
    }
  break;
  }

/*****************************
 * objects must be different *
 *****************************/

if (atom3 != NULL && (atom3 == atom2 || atom3 == atom1 || atom3 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom data", window);
  return;
  }

if (atom2 != NULL && (atom2 == atom1 || atom2 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom data", window);
  return;
  }

if (atom1 != NULL && (atom1 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom data", window);
  return;
  }

/******************************
 * get length or angle to set *
 ******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_value");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &value, -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid data value", window);
  return;
  }

/***************************************************
 * get transformation (translate or origin,rotate) *
 ***************************************************/

switch (page)
  {
  case 5:
  switch (property)
    {
    case 0:
    if (gamgi_math_modify_get_length_2atom (atom0, 
    atom1, value, translate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;

    case 1:
    if (gamgi_math_modify_get_length_atom_2atom (atom0, 
    atom1, atom2, value, translate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;

    case 2:
    if (gamgi_math_modify_get_length_atom_3atom (atom0,
    atom1, atom2, atom3, value, translate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;
    
    case 3:
    if (gamgi_math_modify_get_length_atom_direction (atom0, 
    direction1, value, translate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;

    case 4:
    if (gamgi_math_modify_get_length_atom_plane (atom0, 
    plane1, value, translate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;
    }
  break;

  case 6:
  switch (property)
    {
    case 0:
    if (gamgi_math_modify_get_angle_3atom (atom0, 
    atom1, atom2, value, origin, rotate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;

    case 1:
    if (gamgi_math_modify_get_angle_4atom (atom0, 
    atom1, atom2, atom3, value, origin, rotate, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid geometry data", window);
      return;
      }
    break;
    }
  break;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/************************
 * apply transformation *
 ************************/

switch (page)
  {
  case 5:
  switch (property)
    {
    case 0:
    gamgi_math_modify_set_length_2atom (atom0, atom1, translate);
    break;

    case 1:
    gamgi_math_modify_set_length_atom_2atom (atom0, atom1, atom2, translate);
    break;

    case 2:
    gamgi_math_modify_set_length_atom_3atom (atom0, atom1, atom2, atom3, translate);
    break;

    case 3:
    gamgi_math_modify_set_length_atom_direction (atom0, direction1, translate);
    break;

    case 4:
    gamgi_math_modify_set_length_atom_plane (atom0, plane1, translate);
    break;
    }
  break;

  case 6:
  switch (property)
    {
    case 0:
    gamgi_math_modify_set_angle_3atom (atom0, atom1, atom2, origin, rotate);
    break;

    case 1:
    gamgi_math_modify_set_angle_4atom (atom0, atom1, atom2, atom3, origin, rotate);
    break;
    }
  break;
  }

/*****************************************
 * reset dialog and redraw gl_area image *
 *****************************************/

static_reset_geometry (vbox, page, property, window);

gtk_widget_queue_draw (window->area);
}

static void static_press_geometry (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget* vbox;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];
int page, property;

vbox = static_status (window, &page, &property);

/****************
 * enter object *
 ****************/

entry = static_entry (vbox, page, property, window);
if (entry == NULL)
  {
  static_reset_geometry (vbox, page, property, window);
  entry = static_entry (vbox, page, property, window); 
  }

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/****************** internal function **************
 *                                                 *
 *                      STATIC_LIST                *
 *                                                 *
 ***************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * modify a single atom directly *
   *********************************/

  gtk_label_set_text (GTK_LABEL (label), "Atom");
  static_absolute (dialog);
  }
else
  {
  /*********************************************
   * modify multiple atoms previously selected *
   *********************************************/

  gtk_label_set_text (GTK_LABEL (label), "Name");
  static_reset_data (window);
  static_relative (dialog);
  }
}

static void static_mode (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *vbox_element, *vbox_data, *vbox_analysis, *vbox_position, *vbox_view;
GtkWidget *vbox_length, *vbox_angle;
GtkWidget *button_list, *button_data;

/************
 * get data *
 ************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
button_list = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");

vbox_element = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_element");
vbox_data = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_data");
vbox_analysis = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_analysis");
vbox_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
vbox_view = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_view");
vbox_length = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_length");
vbox_angle = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_angle");

button_data = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_data");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_data)) == TRUE)
  {
  gtk_widget_set_sensitive (button_list, TRUE);

  gtk_widget_hide (vbox_length);
  gtk_widget_hide (vbox_angle);

  gtk_widget_show (vbox_element);
  gtk_widget_show (vbox_data);
  gtk_widget_show (vbox_analysis);
  gtk_widget_show (vbox_position);
  gtk_widget_show (vbox_view);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 0);

  static_reset_data (window);
  static_absolute (dialog);
  }
else
  {
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_list), FALSE);
  gtk_widget_set_sensitive (button_list, FALSE);

  gtk_widget_hide (vbox_element);
  gtk_widget_hide (vbox_data);
  gtk_widget_hide (vbox_analysis);
  gtk_widget_hide (vbox_position);
  gtk_widget_hide (vbox_view);

  gtk_widget_show (vbox_length);
  gtk_widget_show (vbox_angle);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 5);
  }
}

static void static_switch (GtkNotebook *notebook,
GtkNotebookPage *page, int tag, void *data)
{
switch (tag)
  {
  case 0: case 1: case 2: case 3: case 4:
  static_switch_data (notebook, page, tag, data);
  break;

  case 5: case 6:
  static_switch_geometry (notebook, page, tag, data);
  break;
  }
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
int row;

/************************
 * atomic number, label *
 ************************/

cache.new[STATIC_ELEMENT] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ELEMENT] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.element, 0, GAMGI_CHEM_ATOM_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid element", window);
    return FALSE;
    }
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && cache.new[STATIC_ELEMENT] == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid element", window);
  return FALSE;
  }

cache.new[STATIC_LABEL] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_label");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_LABEL] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.label, GAMGI_IO_TEXT, GAMGI_CHEM_ATOM_LABEL + 1) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid label", window);
    return  FALSE;
    }
  }

/************************
 * mass, radius, charge *
 ************************/

cache.new[STATIC_MASS] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_mass");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_MASS] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.mass, GAMGI_MATH_TOLERANCE_MASS, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid mass", window);
    return FALSE;
    }
  }

cache.new[STATIC_RADIUS] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RADIUS] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid radius", window);
    return FALSE;
    }
  }

cache.new[STATIC_CHARGE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CHARGE] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.charge, -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid charge", window);
    return FALSE;
    }
  }

/**************************
 * temperature, occupancy *
 **************************/

cache.new[STATIC_TEMPERATURE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_temperature");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TEMPERATURE] = TRUE;
  if (gamgi_io_token_double_scan (name, &cache.temperature,
  GAMGI_CHEM_ATOM_TEMPERATURE_MIN, GAMGI_CHEM_ATOM_TEMPERATURE_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid temperature", window);
    return FALSE;
    }
  }

cache.new[STATIC_OCCUPANCY] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_occupancy");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_OCCUPANCY] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.occupancy, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid occupancy", window);
    return FALSE;
    }
  }

/******************************
 * translation: type, x, y, z *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_absolute");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.absolute = TRUE;
else
  cache.absolute = FALSE;

cache.new[STATIC_TRANSLATION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TRANSLATION_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.translation[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom translation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom translation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom translation", window);
    return FALSE;
    }
  }

/*************************
 * style, size, variancy *
 *************************/

cache.new[STATIC_STYLE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_STYLE] = TRUE;
  if (row == GAMGI_MESA_WIRED - 1) cache.draw = gamgi_mesa_atom_draw_cross;
  if (row == GAMGI_MESA_SOLID - 1) cache.draw = gamgi_mesa_atom_draw_sphere;
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid size", window);
    return FALSE;
    }
  }

cache.new[STATIC_VARIANCY] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_variancy");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_VARIANCY] = TRUE;
  if (gamgi_io_token_float_scan (name, &cache.variancy, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid variancy", window);
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

return TRUE;
}

static gamgi_bool static_apply (gamgi_atom *atom, gamgi_window *window)
{
void (*draw) (gamgi_atom *);
char name[GAMGI_ENGINE_TOKEN];
char label[GAMGI_ENGINE_TOKEN];
double translation[3];
double mass, radius, charge, temperature, occupancy, scale;
float size, variancy, red, green, blue;
int element;

/*************************
 * import data from atom *
 *************************/

element = atom->element;
strcpy (name, atom->object.name);
strcpy (label, atom->object.label);

mass = atom->mass;
radius = atom->radius;
charge = atom->charge;

temperature = atom->temperature;
occupancy = atom->occupancy;

if (cache.absolute == TRUE)
  gamgi_math_vector_copy (atom->position, translation);
else gamgi_math_vector_zero (translation);

draw = atom->draw;
size = atom->size;
variancy = atom->variancy;
red = atom->red;
green = atom->green;
blue = atom->blue;
scale = atom->object.scale;

/*************************************************
 * set default: size,variancy when style changes *
 *************************************************/

if (cache.new[STATIC_STYLE] == TRUE && cache.draw != atom->draw)
  {
  if (cache.new[STATIC_SIZE] == FALSE) size = gamgi->atom->size;
  if (cache.new[STATIC_VARIANCY] == FALSE) variancy = gamgi->atom->variancy;
  }

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);
if (cache.new[STATIC_LABEL] == TRUE) strcpy (label, cache.label);
if (cache.new[STATIC_ELEMENT] == TRUE) element = cache.element;

if (cache.new[STATIC_MASS] == TRUE) mass = cache.mass;
if (cache.new[STATIC_RADIUS] == TRUE) radius = cache.radius;
if (cache.new[STATIC_CHARGE] == TRUE) charge = cache.charge;

if (cache.new[STATIC_TEMPERATURE] == TRUE) temperature = cache.temperature;
if (cache.new[STATIC_OCCUPANCY] == TRUE) occupancy = cache.occupancy;

if (cache.new[STATIC_TRANSLATION_X] == TRUE) translation[0] = cache.translation[0];
if (cache.new[STATIC_TRANSLATION_Y] == TRUE) translation[1] = cache.translation[1];
if (cache.new[STATIC_TRANSLATION_Z] == TRUE) translation[2] = cache.translation[2];

if (cache.new[STATIC_STYLE] == TRUE) draw = cache.draw;
if (cache.new[STATIC_SIZE] == TRUE) size = cache.size;
if (cache.new[STATIC_VARIANCY] == TRUE) variancy = cache.variancy;
  
if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

if (draw == gamgi_mesa_atom_draw_cross)
  {
  /*************************************************
   * size,variancy are not available in wired mode *
   *************************************************/

  if (cache.new[STATIC_SIZE] == TRUE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error", "Invalid size", window);
    return FALSE;
    }

  if (cache.new[STATIC_VARIANCY] == TRUE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Error", "Invalid variancy", window);
    return FALSE;
    }
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 * *************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/***********************
 * export data to atom *
 ***********************/

strcpy (atom->object.name, name);
strcpy (atom->object.label, label);
atom->element = element;

atom->mass = mass;
atom->radius = radius;
atom->charge = charge;

atom->temperature = temperature;
atom->occupancy = occupancy;

if (cache.absolute == FALSE)
  gamgi_math_vector_add (atom->position, translation, translation);
gamgi_math_vector_copy (translation, atom->position);

atom->draw = draw;
atom->size = size;
atom->variancy = variancy;
atom->red = red;
atom->green = green;
atom->blue = blue;
atom->object.scale = scale;

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

if (gamgi->atoms == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of atom objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atom identity", window);
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
for (dlist = gamgi->atoms; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_ATOM dlist->data, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window);

gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset_data (window);
}

static void static_ok_local (gamgi_window *window)
{
gamgi_atom *atom;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/*****************************************
 * pick atom and get name (possibly new) *
 *                                       *
 * check if focus object belongs to atom *
 *****************************************/

cache.new[STATIC_NAME] = TRUE;
atom = GAMGI_CAST_ATOM gamgi_gtk_object_number (dialog,
"entry_atom", GAMGI_ENGINE_ATOM, window, cache.name);
if (atom == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid atom identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT atom) == TRUE) focus = TRUE;

/********************************************
 * collect data from dialog and modify atom *
 ********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (atom, window) == FALSE) return;

/*************************************************************
 * when focus object is affected, change it to current layer *
 *************************************************************/

if (focus == TRUE)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);

/*****************************************
 * redraw gl_area image and reset dialog *
 *****************************************/

gtk_widget_queue_draw (window->area);
static_reset_data (window);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_data");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /*********************************************
   * modify data: execute global or local task *
   *********************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
    static_ok_global (window);
  else
    static_ok_local (window);
  }
else
  /***************************************
   * modify geometry: execute local task *
   ***************************************/

  static_ok_geometry (widget, data);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkRequisition size;
int width, height;

/**************************************************************************
 * get height showing tabs, get width hiding tabs, then fix notebook size *
 **************************************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
height = size.height;

gtk_notebook_set_show_tabs  (GTK_NOTEBOOK (notebook), FALSE);
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
width = size.width;

gtk_widget_set_size_request (notebook, width, height);
gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), TRUE);

/********************
 * set default mode *
 ********************/

static_mode (NULL, window);

g_signal_connect (notebook, "switch_page",
G_CALLBACK (static_switch), window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_data");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_press_data (object, data);
else
  static_press_geometry (object, data);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_ATOM_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_atom_modify_press (gamgi_window *window_mouse,
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
static_class (window_dialog), FALSE, static_press);
}

/*************** external function ******************
 *                                                  *
 *             GAMGI_GTK_ATOM_MODIFY                *
 *                                                  *
 *  Creates the dialog window used to modify atoms. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_atom_modify (GtkWidget *widget, void *data)
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
 * Dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Atom Modify", window);
window->action = GAMGI_GTK_ATOM_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Atom objects *
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

label = gtk_label_new ("Atom");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_atom", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom", entry);
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

/********
 * Mode *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 5);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Data");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_mode), window);
g_object_set_data (G_OBJECT (dialog), "button_data", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Geometry");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_mode), window);
g_object_set_data (G_OBJECT (dialog), "button_geometry", button);
gtk_widget_show (button);

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/*************
 * Type page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Type ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_element", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
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
g_object_set_data (G_OBJECT (dialog), "vbox_data", vbox_page);
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

vbox_top = gtk_vbox_new (FALSE, 15);
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
g_object_set_data (G_OBJECT (dialog), "vbox_analysis", vbox_page);
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

vbox_top = gtk_vbox_new (FALSE, 15);
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
g_object_set_data (G_OBJECT (dialog), "vbox_position", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***************
 * Translation *
 ***************/

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
g_signal_connect (button, "toggled",
G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_relative", button);
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

/*************
 * View page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_view", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
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

vbox_top_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
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

vbox_top_top = gtk_vbox_new (FALSE, 15);
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

/***************
 * Length page *
 ***************/

label = gtk_label_new ("Length");
vbox_page = gtk_vbox_new (FALSE, 0);
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_length", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * Quantity menu *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Quantity");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_activate), window);
g_object_set_data (G_OBJECT (vbox_page), "combo_quantity", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "2-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom 2-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom 3-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom Direction Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom Plane Distance", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/************
 * Object 1 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object1", label);
gtk_widget_show (label);

/************
 * Object 2 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object2", label);
gtk_widget_show (label);

/************
 * Object 3 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object3", label);
gtk_widget_show (label);

/*********
 * Value *
 *********/

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (vbox_page), "entry_value", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Value");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

/**************
 * Angle page *
 **************/

label = gtk_label_new ("Angle");
vbox_page = gtk_vbox_new (FALSE, 0);
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_angle", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * Quantity menu *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Quantity");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_activate), window);
g_object_set_data (G_OBJECT (vbox_page), "combo_quantity", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "3-Atom Angle", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "4-Atom Torsion", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/************
 * Object 1 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object1", label);
gtk_widget_show (label);

/************
 * Object 2 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object2", label);
gtk_widget_show (label);

/************
 * Object 3 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object3", label);
gtk_widget_show (label);

/*********
 * Value *
 *********/

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (vbox_page), "entry_value", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Value");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

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
