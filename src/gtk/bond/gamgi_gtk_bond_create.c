/***********************************************
 *
 * $GAMGI/src/gtk/bond/gamgi_gtk_bond_create.c
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
#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_filter.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_bond_create.h"
#include "gamgi_io_token.h"

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
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->red);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->green);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->bond->blue);
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

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
int row;

/****************
 * update style *
 ****************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_size");

if (row == GAMGI_MESA_WIRED - 1)
  {
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (combo, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
if (row == GAMGI_MESA_SOLID - 1)
  {
  gtk_widget_set_sensitive (combo, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SIZE, gamgi->bond->size);
  entry = gtk_bin_get_child (GTK_BIN (combo));
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_limit (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry, *entry_minimum, *entry_maximum;
char token[GAMGI_ENGINE_TOKEN];
const char *name1, *name2;
double min, max;
int z1, z2;

entry_minimum = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_minimum");
entry_maximum = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_maximum");

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
    gtk_entry_set_text (entry_minimum, token);

    sprintf (token, "%.*f", gamgi->gamgi->length, max);
    gtk_entry_set_text (entry_maximum, token);
    }
  }
else
  {
  gtk_entry_set_text (entry_minimum, "");
  gtk_entry_set_text (entry_maximum, "");
  }
}

static void static_length (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_minimum");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_maximum");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_model (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox;
GtkWidget *button;
GtkWidget *entry_atom1, *entry_atom2;
GtkWidget *label_atom1, *label_atom2;

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_length");

entry_atom1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
entry_atom2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
label_atom1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom1");
label_atom2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom2");

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_atoms");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_hide (vbox);
  gtk_widget_set_sensitive (entry_atom1, TRUE);
  gtk_widget_set_sensitive (entry_atom2, TRUE);
  gtk_widget_set_sensitive (label_atom1, TRUE);
  gtk_widget_set_sensitive (label_atom2, TRUE);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_length");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_show (vbox);
  gtk_entry_set_text (GTK_ENTRY (entry_atom1), "");
  gtk_entry_set_text (GTK_ENTRY (entry_atom2), "");
  gtk_widget_set_sensitive (entry_atom1, FALSE);
  gtk_widget_set_sensitive (entry_atom2, FALSE);
  gtk_widget_set_sensitive (label_atom1, FALSE);
  gtk_widget_set_sensitive (label_atom2, FALSE);
  static_length (window);
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_voronoi");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_show (vbox);
  gtk_entry_set_text (GTK_ENTRY (entry_atom1), "");
  gtk_entry_set_text (GTK_ENTRY (entry_atom2), "");
  gtk_widget_set_sensitive (entry_atom1, FALSE);
  gtk_widget_set_sensitive (entry_atom2, FALSE);
  gtk_widget_set_sensitive (label_atom1, FALSE);
  gtk_widget_set_sensitive (label_atom2, FALSE);
  static_length (window);
  }
}

/**************** internal function ************
 *                                             *
 *                   STATIC_RESET              *
 *                                             *
 * Reset the bond create dialog, cleaning all  *
 * atom, bond and length entries, even if they *
 * are not active. Although this is redundant, *
 * it's simpler than actually trying to know   *
 * what is the state of these entries.         *
 *                                             *
 ***********************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
gtk_entry_set_text (entry, "");

static_length (window);
}

/**************** internal function ************
 *                                             *
 *                    STATIC_OK                *
 *                                             *
 ***********************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_bond *bond;
gamgi_atom *atom1, *atom2;
gamgi_dlist *dlist;
gamgi_dlist *atom_list;
gamgi_dlist *bond_list;
GtkWidget *button, *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
double min, max;
int element1, element2;
int row;

bond = gamgi_engine_create_bond ();
gamgi_engine_start_bond (bond);

/*************************************************************
 * get name (the default is empty because default bond names *
   can only be attributed later, based in the atom names     *
 *************************************************************/

if (gamgi_gtk_object_name (dialog, "entry_bond", "", bond->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Bond Name", window);
  gamgi_engine_remove_bond (bond);
  return;
  }

/************************************
 * get properties: covalency, order *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_covalency");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && 
gamgi_io_token_float_scan (name, &bond->covalency, 0, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Covalency Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_order");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &bond->order, 0, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Order Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

/*******************************************************
 * get view: style, scale, size, color, red,green,blue *
 *******************************************************/

/*********
 * style *
 *********/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == GAMGI_MESA_WIRED - 1) bond->draw = gamgi_mesa_bond_draw_line;
if (row == GAMGI_MESA_SOLID - 1) bond->draw = gamgi_mesa_bond_draw_cylinder;

/*********
 * scale *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &bond->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

/********
 * size *
 ********/

bond->size = gamgi->bond->size;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_size");
entry = gtk_bin_get_child (GTK_BIN (combo));
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (bond->draw == gamgi_mesa_bond_draw_cylinder &&
gamgi_io_token_float_scan (name, &bond->size, 0.0, FLT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Size Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

/*********
 * color *
 *********/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  bond->color = TRUE;
else
  bond->color = FALSE;

/******************
 * red,green,blue *
 ******************/

bond->red = gamgi->bond->red;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (bond->color == TRUE && 
gamgi_io_token_float_scan (name, &bond->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

bond->green = gamgi->bond->green;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (bond->color == TRUE && 
gamgi_io_token_float_scan (name, &bond->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

bond->blue = gamgi->bond->blue;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (bond->color == TRUE && 
gamgi_io_token_float_scan (name, &bond->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_bond (bond); 
  return;
  }

/************************
 * determine the model  *
 * used to create bonds *
 ************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_atoms");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /**************
   * pick model *
   **************/

  /**************
   * get atom 1 *
   **************/

  atom1 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom1", GAMGI_ENGINE_ATOM, window);
  if (atom1 == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /**************
   * get atom 2 *
   **************/

  atom2 = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom2", GAMGI_ENGINE_ATOM, window);
  if (atom2 == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /**************
   * check bond *
   **************/

  if (atom1 == atom2)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  if (gamgi_chem_bond_get (atom1, atom2) != NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

   /***************************************
   * Save current layer as undo buffer   *
   * layer so it can be recovered later. *
   ***************************************/

   gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

  /********************************************************
   * link bond and set bond name automatically (if empty) *
   ********************************************************/

  gamgi_engine_link_bond_atom (bond, atom1);
  gamgi_engine_link_bond_atom (bond, atom2);
  if (gamgi_io_token_check (bond->object.name) == FALSE)
    gamgi_chem_bond_name (bond, bond->object.name);
  }
else
  {
  /****************************************************
   * get element1,element2 coupled info: data must be *
   * indicated for both element1,element2 or for none *
   ****************************************************/

  element1 = -1;
  element2 = -1;

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element1");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_check (name) == TRUE &&
  (gamgi_io_token_alpha_scan (name, token, GAMGI_IO_TEXT, 
  GAMGI_ENGINE_TOKEN) == FALSE || gamgi_chem_atom_number (token, &element1) == FALSE))
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Element Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element2");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_check (name) == TRUE &&
  (gamgi_io_token_alpha_scan (name, token, GAMGI_IO_TEXT,
  GAMGI_ENGINE_TOKEN) == FALSE || gamgi_chem_atom_number (token, &element2) == FALSE))
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Element Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /******************************************************************
   * Automatic bonds between dymmy atoms (element = 0) are allowed, *
   * as long as bond distance information is specifically entered.  *
     As dummy atoms have no radius, bonds are not created in length *
     mode, unless specific min, max information is specified.       *
   ******************************************************************/

  if ((element1 < 0 && element2 >= 0) || (element2 < 0 && element1 >= 0))
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Element Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /*********************************************
   * get length range coupled info: data must  *
   * be indicated for both min,max or for none *
   *********************************************/

  min = -1.0;
  max = -1.0;

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_minimum");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_check (name) == TRUE && 
  gamgi_io_token_double_scan (name, &min, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_maximum");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_check (name) == TRUE && 
  gamgi_io_token_double_scan (name, &max, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /**********************************
   * the second test also checks if *
   * max is -1.0 and min is defined *
   **********************************/

  if ((min < 0.0 && max >= 0.0) || min > max)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Length Data", window);
    gamgi_engine_remove_bond (bond);
    return;
    }

  /***************************************
   * Save current layer as undo buffer   *
   * layer so it can be recovered later. *
   ***************************************/

  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

  /*******************************************
   * generate list with atoms in the current *
   * object, for element and voronoi models  *
   *******************************************/

  atom_list = gamgi_engine_dlist_atom_object (window->focus, NULL);
  bond_list = NULL;

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_length");
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
    {
    /**************************************
     * remove fom the list all atoms that *
     * are not element1 or element2       *
     **************************************/

    if (element1 >= 0 && element2 >= 0)
      {
      atom_list = gamgi_engine_filter_atom_element (atom_list, 
      element1, element2);
      }

    /****************************************
     * generate list of bonds from list of  *
     * atoms, according to length criterium *
     ****************************************/

    bond_list = gamgi_chem_bond_create_length (window, atom_list, min, max);
    }

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_voronoi");
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
    {
    /*****************************************
     * generate list of bonds from list of   *
     * atoms, according to voronoi criterium *
     *****************************************/

    bond_list = gamgi_chem_bond_create_voronoi (window, atom_list);
    if (min >= 0 && max >= 0)
      {
      bond_list = gamgi_engine_filter_bond_length (bond_list, 
      min, max);
      }
    }

  /***************************************************
   * remove from list all bonds that are not between * 
   * element1 and element2 (element1-element1 and    *
   * element2-element2 bonds are also removed).      *
   ***************************************************/

  if (element1 >= 0 && element2 >= 0)
   {
   bond_list = gamgi_engine_filter_bond_element (bond_list, 
   element1, element2);
   }

  /*********************************************
   * copy data from bond template to bond list *
   *********************************************/

  for (dlist = bond_list; dlist != NULL; dlist = dlist->next)
    gamgi_chem_bond_copy (bond, GAMGI_CAST_BOND dlist->data);

  /***************************
   * 1) free atom,bond lists *
   * 2) free bond template   *
   ***************************/

  gamgi_engine_dlist_remove_all (atom_list);
  gamgi_engine_dlist_remove_all (bond_list);
  gamgi_engine_remove_bond (bond);
  }

gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_init (gamgi_window *window)
{
gamgi_bond_class *bond_class = gamgi->bond;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *button;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/******************************
 * set single bond as default *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_order");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

/*************************************************************
 * initialize independent parameters: style, color and scale *
 *************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
if (bond_class->draw == gamgi_mesa_bond_draw_line)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);
if (bond_class->draw == gamgi_mesa_bond_draw_cylinder)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SOLID - 1);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_color");
if (bond_class->color == TRUE)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
else
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * hide Length page *
 ********************/

static_model (NULL, window);

/************************************************************
 * initialize dependent parameters: size and red,green,blue *
 ************************************************************/

static_style (NULL, window);
static_color (button, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_atom *atom;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry1, *entry2;
char string[2 * GAMGI_ENGINE_TOKEN];
const char *name1, *name2;

atom = GAMGI_CAST_ATOM object;

entry1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
name1 = gtk_entry_get_text (GTK_ENTRY (entry1));
entry2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
name2 = gtk_entry_get_text (GTK_ENTRY (entry2));

if (gamgi_io_token_check (name1) == FALSE)
  {
  sprintf (string, "%s %d", atom->object.name, atom->object.number);
  gtk_entry_set_text (GTK_ENTRY (entry1), string);
  }
else
  {
  sprintf (string, "%s %d", atom->object.name, atom->object.number);
  gtk_entry_set_text (GTK_ENTRY (entry2), string);
  }

/********************************
 * both tests are required,     *
 * because users may delete     *
 * entry1 after entering entry2 *
 ********************************/

name1 = gtk_entry_get_text (GTK_ENTRY (entry1));
if (gamgi_io_token_check (name1) == FALSE) return;

name2 = gtk_entry_get_text (GTK_ENTRY (entry2));
if (gamgi_io_token_check (name2) == FALSE) return;

static_ok (NULL, window);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_BOND_CREATE_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_bond_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
GtkWidget *entry;
const char *name1, *name2;

/******************************
 * local mouse selection only *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_atoms");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE ||
window_dialog != window_mouse) return;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom1");
name1 = gtk_entry_get_text (GTK_ENTRY (entry));
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom2");
name2 = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_check (name1) == TRUE
&& gamgi_io_token_check (name2) == TRUE) return;

gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_ATOM, FALSE, static_press);
}

/*************** external function *************
 *                                             *
 *             GAMGI_GTK_BOND_CREATE           *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_bond_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
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

dialog = gamgi_gtk_dialog_task0_create ("Bond Create", window);
window->action = GAMGI_GTK_BOND_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*************
 * Bond name *
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
g_object_set_data (G_OBJECT (dialog), "entry_bond", entry);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
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
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********
 * Type *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Atoms");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_model), window);
g_object_set_data (G_OBJECT (dialog), "button_atoms", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Length");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_model), window);
g_object_set_data (G_OBJECT (dialog), "button_length", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Delaunay");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_model), window);
g_object_set_data (G_OBJECT (dialog), "button_voronoi", button);
gtk_widget_show (button);

/*********
 * Atoms *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_atom1", label);
gtk_widget_show (label);

label = gtk_label_new ("Atom");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_atom2", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom2", entry);
gtk_widget_show (entry);

/***************
 * Length page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Length");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_length", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/**********
 * Length *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/************
 * Elements *
 ************/

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_limit), window);
g_object_set_data (G_OBJECT (dialog), "entry_element1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_limit), window);
g_object_set_data (G_OBJECT (dialog), "entry_element2", entry);
gtk_widget_show (entry);

/*******************
 * Minimum,Maximum *
 *******************/

table = gtk_table_new (2, 2, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Minimum");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Maximum");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_minimum", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_maximum", entry);
gtk_widget_show (entry);

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

vbox_top = gtk_vbox_new (FALSE, 10);
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

/**************
 * Bond color *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label (" Color ");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_color), window);
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
