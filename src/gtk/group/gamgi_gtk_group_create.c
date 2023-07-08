/*************************************************
 *
 * $GAMGI/src/gtk/group/gamgi_gtk_group_create.c
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_filter.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_group_link.h"
#include "gamgi_mesa_select.h"
#include "gamgi_chem_polytope.h"
#include "gamgi_chem_atom.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"

enum { STATIC_STYLE_WIRED = 0, STATIC_STYLE_SOLID, STATIC_STYLE_LOCAL };

static void static_color (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label_red, *label_green, *label_blue;
char token[GAMGI_ENGINE_TOKEN];

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_blue");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_GROUP_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_GROUP_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_GROUP_B);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  }
}

static void static_number (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_start_container (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;

/**********************
 * start central Atom *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/************************************
 * start central atom in Local mode *
 ************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_local");
gtk_button_clicked (GTK_BUTTON (button));
}

static void static_start_polytope (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/**********************
 * start central Atom *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/************************************
 * start central atom in Local mode *
 ************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_local");
gtk_button_clicked (GTK_BUTTON (button));

/******************************
 * structure, filter policies *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_STRUCTURE - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_FILTER - 1);

/*******************************************
 * start face tolerance: Visible, Coplanar *
 *******************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_visible");
sprintf (token, "%.*E", 2, GAMGI_CHEM_POLYTOPE_VISIBLE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_coplanar");
sprintf (token, "%.*E", 2, GAMGI_CHEM_POLYTOPE_COPLANAR);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/******************************
 * start Number of neighbours *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_number");
gtk_button_clicked (GTK_BUTTON (button));

/*****************************
 * start Style, Color, Scale *
 *****************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
static_color (button, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox_scope, *vbox_view;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;

/************
 * get data *
 ************/

vbox_scope = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
vbox_view = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_view");

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_polytope");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /*****************
   * polytope mode *
   *****************/

  gtk_widget_show (vbox_scope);
  gtk_widget_show (vbox_view);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_central");
  gtk_widget_set_sensitive (label, TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_global");
  gtk_widget_set_sensitive (button, TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_local");
  gtk_widget_set_sensitive (button, TRUE);
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  gtk_widget_set_sensitive (entry, TRUE);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom");
  gtk_widget_set_sensitive (label, TRUE);
  static_start_polytope (window);
  }
else
  {
  /******************
   * container mode *
   ******************/

  gtk_widget_hide (vbox_scope);
  gtk_widget_hide (vbox_view);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_central");
  gtk_widget_set_sensitive (label, FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_global");
  gtk_widget_set_sensitive (button, FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_local");
  gtk_widget_set_sensitive (button, FALSE);
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  gtk_widget_set_sensitive (entry, FALSE);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom");
  gtk_widget_set_sensitive (label, FALSE);
  static_start_container (window);
  }

}

static void static_ok_container (gamgi_group *group, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

group->reference = GAMGI_CHEM_CONTAINER;

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_group",
"Group", group->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Group Name", window);
  gamgi_engine_remove_group (group);
  return;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*************************************
 *  launch a Group Link dialog       *
 *  containing already the name and  *
 *  number of the new group.         *
 *                                   *
 *   The old dialog is removed       *
 *   before creating the new one     *
 *                                   *
 * no need to redraw gl_area because *
 * an empty group is invisible       *
 *                                   *
 * Insert group's name and number    *
 *  in new dialog's group entry      *
 *************************************/

gamgi_engine_link_group_layer (group, window->layer);

gamgi_gtk_group_link (NULL, window);

dialog = window->dialog0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
sprintf (string, "%s %d", group->object.name, group->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_ok_polytope (gamgi_group *group, gamgi_window *window)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
GtkWidget *dialog = window->dialog0;
GtkWidget *text;
GtkWidget *entry;
GtkWidget *combo;
GtkWidget *button;
gamgi_bool global;
gamgi_enum structure, filter;
const char *name;
char token[GAMGI_ENGINE_TOKEN];
char string[GAMGI_ENGINE_STRING];
char line[GAMGI_ENGINE_LINE] = "";
double visible, coplanar, radius;
int out[4];
int number, element;

group->reference = GAMGI_CHEM_POLYTOPE;

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_group",
"Polytope", group->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Group Name", window);
  gamgi_engine_remove_group (group);
  return;
  }

/********************
 * Get central atom *
 ********************/

atom = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
"entry_atom", GAMGI_ENGINE_ATOM, NULL);
if (atom == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid central atom", window);
  gamgi_engine_remove_group (group);
  return;
  }

/************************************************************
 * global (All) or local (only the chosen one) central atom *
 ************************************************************/

global = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_global");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  global = TRUE;

/**********************************
 * get structure, filter policies *
 **********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
structure = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
filter = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/****************************************************
 *     number of neighbours or maximum radius       *
 *                                                  *
 * when no number or distances are set, assume that *
 * all distances and number of neighbours are valid *
 ****************************************************/

number = 0;
radius = DBL_MAX;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE)
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_number");
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
    {
    /******************************
     * fixed number of neighbours *
     ******************************/

    if (gamgi_io_token_int_scan (name, &number, 2, INT_MAX) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", 
      "Invalid neighbours number", window);
      gamgi_engine_remove_group (group);
      return;
      }

    radius = 0.0;
    }
  else
    {
    /*************************************
     * fixed maximum distance (distances *
     * are squared to increase speed)    *
     *************************************/

    if (gamgi_io_token_double_scan (name, &radius, 
    GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", 
      "Invalid neighbours maximum radius", window);
      gamgi_engine_remove_group (group);
      return;
      }

    number = 0;
    radius = radius * radius;
    }
  }

/***********************************
 * get element for neighbour atoms *
 ***********************************/

element = -1;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name) == TRUE &&
(gamgi_io_token_alpha_scan (name, token, GAMGI_IO_TEXT,
GAMGI_ENGINE_TOKEN) == FALSE || gamgi_chem_atom_number (token,
&element) == FALSE))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid neighbours element", window);
  gamgi_engine_remove_group (group);
  return;
  }

/*************************************
 * face Tolerance: Coplanar, Visible *
 *************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_visible");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &visible,
0.0, GAMGI_CHEM_POLYTOPE_VISIBLE_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid visible face tolerance", window);
  gamgi_engine_remove_group (group);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_coplanar");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &coplanar,
0.0, GAMGI_CHEM_POLYTOPE_COPLANAR_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid coplanar face tolerance", window);
  gamgi_engine_remove_group (group);
  return;
  }

/*************
 * get Style *
 *************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
group->faces = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*************
 * get Color *
 *************/

group->red = -1.0;
group->green = -1.0;
group->blue = -1.0;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &group->red, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_group (group);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &group->green, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_group (group);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &group->blue, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    gamgi_engine_remove_group (group);
    return;
    }

  }
else
  {
  /****************************************************************
   * Default color: use default color for neighbour element. When *
   * this is undefined, use current color for each central atom   *
   ****************************************************************/

  if (element >= 0)
    {
    group->red = gamgi->atom->red[element];
    group->green = gamgi->atom->green[element];
    group->blue = gamgi->atom->blue[element];
    }
  }

/*************
 * get Scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &group->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid scale data", window);
  gamgi_engine_remove_group (group);
  return;
  }

/****************
 * global tests *
 ****************/

if (global == TRUE && radius == DBL_MAX)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid number selection", window);
  gamgi_engine_remove_group (group);
  return;
  }

/**********************
 * check central atom *
 **********************/

if (gamgi_engine_find_object (GAMGI_CAST_OBJECT atom, window->focus) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid searching scope for central atom", window);
  gamgi_engine_remove_group (group);
  return;
  }

/**********************************
 * check central atom, neighbours *
 **********************************/

if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT window->layer) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid searching scope for neighbour atoms", window);
  gamgi_engine_remove_group (group);
  return;
  }

/**********************************
 * check central atom, neighbours *
 **********************************/

dlist = gamgi_engine_dlist_atom_object (window->focus, NULL);
dlist = gamgi_engine_filter_atom (dlist, 0, filter,
GAMGI_ENGINE_GROUP, window->focus);
if (dlist == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid searching scope for neighbour atoms", window);
  gamgi_engine_remove_group (group);
  return;
  }

/**********************************
 * input data is ok: save History *
 **********************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/***********************************
 * create and initialize text      *
 * widget, to show Polytope report *
 ***********************************/

text = gamgi_gtk_dialog_text_create ();

/****************************
 * apply Polytope algorithm *
 ****************************/

if (gamgi_chem_polytope (dlist, group, atom, global, structure,
visible, coplanar, element, number, radius, out, text) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error",
  "Insufficient number of neighbours", window);
  gtk_widget_destroy (text);
  return;
  }

if (out[GAMGI_MATH_POLYTOPE_4] > 0)
  {
  dialog = gamgi_gtk_dialog_report_create (text,
  "Group Create Polytope", GAMGI_GTK_GROUP_CREATE_POLYTOPE, window);
  gtk_widget_show (dialog);
  }
else
  {
  gtk_widget_destroy (text);
  if (global == FALSE) gamgi_engine_remove_group (group);
  }

if (out[GAMGI_MATH_POLYTOPE_1] > 0)
  {
  sprintf (string, "Ignored Groups (%d), ", out[GAMGI_MATH_POLYTOPE_1]);
  strcat (line, string);
  }
if (out[GAMGI_MATH_POLYTOPE_2] > 0)
  {
  sprintf (string, "Created Directions (%d), ", out[GAMGI_MATH_POLYTOPE_2]);
  strcat (line, string);
  }
if (out[GAMGI_MATH_POLYTOPE_3] > 0)
  {
  sprintf (string, "Created Planes (%d), ", out[GAMGI_MATH_POLYTOPE_3]);
  strcat (line, string);
  }
if (line[0] != '\0') strcat (line, "due to improper atom coordinates");

if (out[GAMGI_MATH_POLYTOPE_1] + out[GAMGI_MATH_POLYTOPE_2] +
out[GAMGI_MATH_POLYTOPE_3] > 0)
  gamgi_gtk_dialog_message_create ("Warning", line, window);

/************************
 * Redraw gl_area image *
 ************************/

if (out[GAMGI_MATH_POLYTOPE_2] + out[GAMGI_MATH_POLYTOPE_3] +
out[GAMGI_MATH_POLYTOPE_4] > 0) gtk_widget_queue_draw (window->area);
}

/******************* internal function *****************
 *                                                     *
 *                        STATIC_OK                    *
 *                                                     *
 * This is the function that actually creates a new    *
 * group. It is triggered when the user presses Ok.    *
 *                                                     *
 *******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_group *group;
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

group = gamgi_engine_create_group ();
gamgi_engine_start_group (group);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_polytope");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_polytope (group, window);
else
  static_ok_container (group, window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *button;
GtkRequisition size;

/***************************************************
 * Set notebook current size as its maximum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
gtk_widget_set_size_request (notebook, size.width, size.height);

/********************************************
 * hide pages AFTER measuring notebook size *
 ********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_container");
gtk_button_clicked (GTK_BUTTON (button));
static_reference (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_atom *atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];

atom = GAMGI_CAST_ATOM object;

/***************************
 * set Atom identification *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
sprintf (string, "%s %d", atom->object.name, atom->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *         GAMGI_GTK_GROUP_CREATE_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_group_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;

/******************************
 * local mouse selection only *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_polytope");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE ||
window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y,
GAMGI_ENGINE_ATOM, FALSE, static_press);
}

/******************* external function **************
 *                                                  *
 *                GAMGI_GTK_GROUP_CREATE            *
 *                                                  *
 * Creates the dialog window used to create groups. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_group_create (GtkWidget *widget, void *data)
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

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Group Create", window);
window->action = GAMGI_GTK_GROUP_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/**************
 * Group name *
 **************/

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
g_object_set_data (G_OBJECT (dialog), "entry_group", entry);
gtk_widget_show (entry);

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
g_object_set_data (G_OBJECT (dialog), "vbox_type", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*************
 * Reference *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Reference");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Container");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_reference), window);
g_object_set_data (G_OBJECT (dialog), "button_container", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Polytope");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_reference), window);
g_object_set_data (G_OBJECT (dialog), "button_polytope", button);
gtk_widget_show (button);

/************
 * Polytope *
 ************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

/****************
 * Central Atom *
 ****************/

label = gtk_label_new ("Central");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_central", label);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Atom");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_atom", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom", entry);
gtk_widget_show (entry);

/******************************
 * Local, Global central atom *
 ******************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_end (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Local");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_local", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Global");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_global", button);
gtk_widget_show (button);

/************************
 * Polytope: Scope page *
 ************************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Scope ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_scope", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********************
 * Structure, Filter *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Structure");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_structure", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Copy", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Link", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Filter");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_filter", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Class", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Child", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Identical", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********************
 * Visible, Coplanar *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_end (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Visible");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "entry_visible", entry);
gtk_widget_show (entry);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Coplanar");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "entry_coplanar", entry);
gtk_widget_show (entry);

/**************
 * Neighbours *
 **************/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Neighbours");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_SHRINK, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_object_set_data (G_OBJECT (dialog), "entry_element", entry);
gtk_widget_show (entry);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_table_attach (GTK_TABLE (table), hbox_left, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Number");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_number), window);
g_object_set_data (G_OBJECT (dialog), "button_number", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Radius");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_number), window);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_SHRINK, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_number", entry);
gtk_widget_show (entry);

/***********************
 * Polytope: View page *
 ***********************/

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

/*********
 * Style *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Style");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
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

/*********
 * Color *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label (" Color ");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_color), window);
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

/*********
 * Scale *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
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
