/*********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_link.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_math.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_phys_cell_link.h"
#include "gamgi_phys_space.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_rcp.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_OBJECT, STATIC_CRYSTAL, STATIC_RANDOM };

enum { STATIC_NODE_P, STATIC_NODE_I, STATIC_NODE_A,
STATIC_NODE_B, STATIC_NODE_C, STATIC_NODE_R };

enum { STATIC_RULE_A, STATIC_RULE_B, STATIC_RULE_C };

static struct {
gamgi_bool local;
gamgi_object *object;
gamgi_enum method;

/***************
 * object data *
 ***************/

gamgi_bool above, child;
double origin[3];

/****************
 * crystal data *
 ****************/

const char **wyckoff;
const int *driver;
int site;

gamgi_enum position_vectors;
double position[3];

double angle;
gamgi_enum axis;
gamgi_enum rotation_vectors;
int uvw[3];
gamgi_enum translation_vectors;
double translation[3];

gamgi_enum cutoff;
gamgi_bool nodes[6];
char rules[3][GAMGI_ENGINE_STRING];

/***************
 * random data *
 ***************/

double time;
int atoms, seed; } cache;

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
int row;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/*******************************************
 * when entries are empty, look for a cell *
 *******************************************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_CELL;

/********************
 * get object class *
 ********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == 0) return GAMGI_ENGINE_LAYER;
  if (row == 1) return GAMGI_ENGINE_ASSEMBLY;
  }
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == 0) return GAMGI_ENGINE_CLUSTER;
  if (row == 1) return GAMGI_ENGINE_MOLECULE;
  if (row == 2) return GAMGI_ENGINE_GROUP;
  if (row == 3) return GAMGI_ENGINE_PLANE;
  if (row == 4) return GAMGI_ENGINE_DIRECTION;
  if (row == 5) return GAMGI_ENGINE_ATOM;
  if (row == 6) return GAMGI_ENGINE_ORBITAL;
  if (row == 7) return GAMGI_ENGINE_TEXT;
  }

return FALSE;
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button, *button_list, *button_below;

button_list = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
button_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_below");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_list)) == TRUE
&& gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_below)) == TRUE)
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_parent");
else
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_child");

gtk_button_clicked (GTK_BUTTON (button));
}

static void static_number (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_number");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (entry, TRUE);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
}

static void static_xyz_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_xyz_start (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_NODE_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_NODE_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_NODE_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_wyckoff_change (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
const char **wyckoff;
const int *driver;
gamgi_bool used[3];
int site;

/***********************************
 * wyckoff data: positions, driver *
 ***********************************/

wyckoff = (const char **) g_object_get_data (G_OBJECT (dialog), "wyckoff");
driver = (const int *) g_object_get_data (G_OBJECT (dialog), "driver");

/**************************
 * determine wyckoff site *
 **************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_wyckoff");
site = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/******************************************************************
 * determine which x,y,z entries are needed for this wyckoff site *
 *                                                                *
 * defensive programming: if wyckoff is NULL then driver should   *
 * be also NULL and according to the code above, site will be 0   *
 ******************************************************************/

if (wyckoff == NULL || driver == NULL || site == 0)
  { used[0] = TRUE; used[1] = TRUE; used[2] = TRUE; }
else
  gamgi_phys_space_used (wyckoff, driver, site, used);

/************************
 * update x,y,z entries *
 ************************/

static_xyz_clean (window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
if (used[0] == TRUE) gtk_widget_set_sensitive (entry, TRUE);
else gtk_widget_set_sensitive (entry, FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
if (used[1] == TRUE) gtk_widget_set_sensitive (entry, TRUE);
else gtk_widget_set_sensitive (entry, FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
if (used[2] == TRUE) gtk_widget_set_sensitive (entry, TRUE);
else gtk_widget_set_sensitive (entry, FALSE);
}

static void static_wyckoff_start (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkListStore *store;
GtkTreeIter iter;
const int *driver;
int site, n_sites, n_positions;
int group;
char token[GAMGI_ENGINE_TOKEN];
const char **wyckoff;
const char **symmetry;
char letter;

/******************************
 * get crystallographic group *
 ******************************/

if (cell == NULL) group = 0;
else group = cell->group;

/*****************************************************
 * create combo box list: the first option,          *
 * the basis, is always avalable. To guarantee that  *
 * the combo box is not resized, the Basis option    *
 * should be at least as long as the longest option, *
 * apparently group 221, with positions 4/mm.m       *
 *****************************************************/

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, " 1  Basis  1     ", -1);

/*********************************************
 * List all the wyckoff positions, according *
 * to  the group information available.      *
 *********************************************/

if (group > 0)
  {
  gamgi_phys_space_get (group, &wyckoff, &symmetry, &driver);
  g_object_set_data (G_OBJECT (dialog), "wyckoff", (void *) wyckoff);
  g_object_set_data (G_OBJECT (dialog), "driver", (void *) driver);

  n_sites = driver[0];
  for (site = 1; site <= n_sites; site++)
    {
    n_positions = gamgi_phys_space_multiplicity (driver, site);
    letter = gamgi_phys_space_letter (n_sites, site);
    sprintf (token, "%2d  %c  %s", n_positions, letter, symmetry[site - 1]);

    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, 0, token, -1);
    }
  }
else
  {
  g_object_set_data (G_OBJECT (dialog), "wyckoff", NULL);
  g_object_set_data (G_OBJECT (dialog), "driver", NULL);
  }

/*********************************************************
 * set model (and automatically remove the previous one) *
 *********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_wyckoff");
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

/**********************************************************
 * When group information is available, the combo default *
 * is the group general position, otherwise is the basis  *
 **********************************************************/

if (group == 0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);

static_wyckoff_change (NULL, window);
static_xyz_start (window);
}

static void static_nodes_start (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button_p, *button_i, *button_a, *button_b, *button_c, *button_r;
gamgi_enum lattice, model;

/******************************
 * get cell lattice and model *
 ******************************/

if (cell == NULL)
  { lattice = FALSE; model = FALSE; }
else
  { lattice = cell->lattice; model = cell->model; }

/***********************************
 * get buttons for occupancy nodes *
 ***********************************/

button_p = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_p");
button_i = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_i");
button_a = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_a");
button_b = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_b");
button_c = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_c");
button_r = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_r");

/******************************
 * set P,I,A,B,C,R buttons on *
 ******************************/

gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_p), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_i), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_a), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_b), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_c), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_r), TRUE);

/**********************************
 * set P,I,A,B,C,R buttons active *
 **********************************/

gtk_widget_set_sensitive (button_p, TRUE);
gtk_widget_set_sensitive (button_i, TRUE);
gtk_widget_set_sensitive (button_a, TRUE);
gtk_widget_set_sensitive (button_b, TRUE);
gtk_widget_set_sensitive (button_c, TRUE);
gtk_widget_set_sensitive (button_r, TRUE);

/***************************************************************
 * when the Bravais lattice is unknown, all nodes are possible *
 ***************************************************************/

if (lattice == FALSE) return;

/********************************************************
 * set P,I,A,B,C,R buttons according to Bravais lattice *
 ********************************************************/

gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_p), TRUE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_i), FALSE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_a), FALSE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_b), FALSE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_c), FALSE);
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_r), FALSE);

/********************************************************
 * set P,I,A,B,C,R buttons according to Bravais lattice *
 ********************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I: 
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_i), TRUE);
  break;
 
  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_a), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_b), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_c), TRUE);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_c), TRUE);
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_r), TRUE);
  break;
  
  default:
  break;
  }

/************************************************************
 * buttons for nodes that users cannot select (because they *
 * do not exist for this cell lattice) or unselect (because *
 * this cell type really requires them) are set insensitive *
 ************************************************************/

if (model != GAMGI_PHYS_CONVENTIONAL)
  {
  gtk_widget_set_sensitive (button_p, FALSE);
  gtk_widget_set_sensitive (button_i, FALSE);
  gtk_widget_set_sensitive (button_a, FALSE);
  gtk_widget_set_sensitive (button_b, FALSE);
  gtk_widget_set_sensitive (button_c, FALSE);
  gtk_widget_set_sensitive (button_r, FALSE);
  }
else
  {
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_i)) == FALSE)
    gtk_widget_set_sensitive (button_i, FALSE);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_a)) == FALSE)
    gtk_widget_set_sensitive (button_a, FALSE);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_b)) == FALSE)
    gtk_widget_set_sensitive (button_b, FALSE);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_c)) == FALSE)
    gtk_widget_set_sensitive (button_c, FALSE);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_r)) == FALSE)
    gtk_widget_set_sensitive (button_r, FALSE);
  }

}

static void static_axis (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label_u, *label_v, *label_w;
GtkWidget *combo;
int row;

/*************************************************************
 * this function updates the axis labels: plane or direction *
 *************************************************************/

label_u = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_u");
label_v = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v");
label_w = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_w");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axis");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == 0)
  {
  gtk_label_set_text (GTK_LABEL (label_u), "H");
  gtk_label_set_text (GTK_LABEL (label_v), "K");
  gtk_label_set_text (GTK_LABEL (label_w), "L");
  }
else
  {
  gtk_label_set_text (GTK_LABEL (label_u), "U");
  gtk_label_set_text (GTK_LABEL (label_v), "V");
  gtk_label_set_text (GTK_LABEL (label_w), "W");
  }
}

static void static_cell (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
int row;

/*****************
 * get link mode *
 *****************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == STATIC_CRYSTAL)
  {
  /*****************************************************
   * crystallographic link mode: show wyckoff,node     *
   * data (when cell is NULL, reset wyckoff,node data) *
   *****************************************************/

  cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
  "entry_cell", GAMGI_ENGINE_CELL, window);

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_position_vectors");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

  static_wyckoff_start (cell, window);
  static_nodes_start (cell, window);
  }
}

static void static_start_shift (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/*********************
 * start origin page *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_GTK_LINK_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************
 * reset origin *
 ****************/

static_reference (NULL, window);
}

static void static_start_change (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/***************
 * translation *
 ***************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_translation_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_SHIFT_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_SHIFT_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_SHIFT_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/************
 * rotation *
 ************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
sprintf (token, "%.*f", gamgi->gamgi->angle, 0.0);
gtk_entry_set_text (GTK_ENTRY (entry), token);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_rotation_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axis");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 1);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
sprintf (token, "%d", GAMGI_PHYS_SHIFT_U);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
sprintf (token, "%d", GAMGI_PHYS_SHIFT_V);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
sprintf (token, "%d", GAMGI_PHYS_SHIFT_W);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_start_occupancy (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cutoff");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
gtk_entry_set_text (GTK_ENTRY (entry), "*");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
gtk_entry_set_text (GTK_ENTRY (entry), "*");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
gtk_entry_set_text (GTK_ENTRY (entry), "*");
}

static void static_start_packing (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_radius");
gtk_button_clicked (GTK_BUTTON (button));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_time");
sprintf (token, "%.*f", 1, GAMGI_PHYS_RCP_TIME);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
sprintf (token, "%d", GAMGI_MATH_SEED);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_method (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *vbox_shift;
GtkWidget *vbox_position, *vbox_change, *vbox_occupancy;
GtkWidget *vbox_packing;
int row;

/**********************
 * get link combo box *
 **********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/************
 * get data *
 ************/

vbox_shift = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_shift");
vbox_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
vbox_change = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_change");
vbox_occupancy = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_occupancy");
vbox_packing = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_packing");

/*******************************
 * Object: link object to cell *
 *******************************/

if (row == STATIC_OBJECT)
  {
  gtk_widget_hide (vbox_position);
  gtk_widget_hide (vbox_change);
  gtk_widget_hide (vbox_occupancy);
  gtk_widget_hide (vbox_packing);
  gtk_widget_show (vbox_shift);

  static_start_shift (window);
  }

/************************************************
 * Crystallographic: use crystallographic       *
 * information to link object to lattice nodes  *
 ************************************************/

if (row == STATIC_CRYSTAL)
  {
  gtk_widget_hide (vbox_shift);
  gtk_widget_hide (vbox_packing);
  gtk_widget_show (vbox_position);
  gtk_widget_show (vbox_change);
  gtk_widget_show (vbox_occupancy);

  static_cell (NULL, window);
  static_start_change (window);
  static_start_occupancy (window);
  }

/********************************************
 * Random Close Packing: use RCP algorithm  *
 * to build compact random sphere structure *
 ********************************************/

if (row == STATIC_RANDOM)
  {
  gtk_widget_hide (vbox_shift);
  gtk_widget_hide (vbox_position);
  gtk_widget_hide (vbox_change);
  gtk_widget_hide (vbox_occupancy);
  gtk_widget_show (vbox_packing);

  static_start_packing (window);
  }

}

static void static_hierarchy (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo_below, *combo_above, *combo_method;
GtkWidget *button;
GtkWidget *entry;
gamgi_bool *sensitive;

combo_above = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
combo_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");

combo_method = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_link");

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /***************************************************
   * the default for the object class above is Layer *
   ***************************************************/

  gtk_widget_show (combo_above);
  gtk_widget_hide (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_above), 0);

  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_method), 0);
  sensitive[STATIC_CRYSTAL] = FALSE;
  sensitive[STATIC_RANDOM] = FALSE;
  }
else
  {
  /***********************************************
   * the default for the combo box below is Atom *
   ***********************************************/

  gtk_widget_hide (combo_above);
  gtk_widget_show (combo_below);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_below), 5);

  sensitive[STATIC_CRYSTAL] = TRUE;
  sensitive[STATIC_RANDOM] = TRUE;
  }

/**********************
 * reset object entry *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/********************
 * update mode data *
 ********************/

static_method (NULL, window);
}

/****************** internal function ****************
 *                                                   *
 *                     STATIC_RESET                  *
 *                                                   *
 * Reset the cell link dialog, cleaning all entries. *
 *                                                   *
 *****************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/*******************************
 * reset cell and object names *
 *******************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***************
 * reset pages *
 ***************/

static_start_shift (window);
static_cell (NULL, window);
static_start_change (window);
static_start_occupancy (window);
static_start_packing (window);
}

static void static_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *entry;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  { static_reset (window); return; }

/***************************
 * keep cell, reset object *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/************ internal function ************
 *                                         *
 *                STATIC_LIST              *
 *                                         *
 * Makes the cell entry sensitive when the *
 * local button is pressed and insensitive *
 * when the global button is pressed.      *
 *                                         *
 *******************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *hbox;

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_cell");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /****************************
   * Choose the cell directly *
   ****************************/

  gtk_widget_set_sensitive (hbox, TRUE);
  }
else
  {
  /****************************************
   * Choose the cells previously selected *
   ****************************************/

  static_reset (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  }

static_reference (NULL, window);
}

static void static_origin (gamgi_object *child, gamgi_object *parent)
{
gamgi_text *text;
gamgi_orbital *orbital;
gamgi_atom *atom;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;
double origin[3];

/*****************************************************
 * translation is always applied to the object below *
 *****************************************************/

if (cache.child == TRUE)
  gamgi_mesa_translate_object (child, cache.origin);
else
  {
  /**********************************************************************
   * get reference origin from parent, apply translated origin to child *
   **********************************************************************/

  switch (parent->class)
    {
    case GAMGI_ENGINE_CELL:
    cell = GAMGI_CAST_CELL parent;
    gamgi_math_vector_copy (cell->origin, origin);
    break;

    case GAMGI_ENGINE_ASSEMBLY:
    assembly = GAMGI_CAST_ASSEMBLY parent;
    gamgi_math_vector_copy (assembly->origin, origin);
    break;

    case GAMGI_ENGINE_LAYER:
    layer = GAMGI_CAST_LAYER parent;
    gamgi_math_vector_copy (layer->center, origin);
    break;
    }

  /**************************************
   * 1) get new origin for child        *
   * 2) get translate vector for child  *
   * 3) apply translate vector to child *
   **************************************/

  gamgi_math_vector_add (origin, cache.origin, origin);

  switch (child->class)
    {
    case GAMGI_ENGINE_TEXT:
    text = GAMGI_CAST_TEXT child;
    gamgi_math_vector_sub (origin, text->origin, origin);
    break;

    case GAMGI_ENGINE_ORBITAL:
    orbital = GAMGI_CAST_ORBITAL child;
    gamgi_math_vector_sub (origin, orbital->origin, origin);
    break;

    case GAMGI_ENGINE_ATOM:
    atom = GAMGI_CAST_ATOM child;
    gamgi_math_vector_sub (origin, atom->position, origin);
    break;

    case GAMGI_ENGINE_DIRECTION:
    direction = GAMGI_CAST_DIRECTION parent;
    gamgi_math_vector_sub (origin, direction->origin, origin);
    break;

    case GAMGI_ENGINE_PLANE:
    plane = GAMGI_CAST_PLANE parent;
    gamgi_math_vector_sub (origin, plane->origin, origin);
    break;

    case GAMGI_ENGINE_GROUP:
    group = GAMGI_CAST_GROUP parent;
    gamgi_math_vector_sub (origin, group->origin, origin);
    break;

    case GAMGI_ENGINE_MOLECULE:
    molecule = GAMGI_CAST_MOLECULE parent;
    gamgi_math_vector_sub (origin, molecule->origin, origin);
    break;

    case GAMGI_ENGINE_CLUSTER:
    cluster = GAMGI_CAST_CLUSTER parent;
    gamgi_math_vector_sub (origin, cluster->origin, origin);
    break;

    case GAMGI_ENGINE_CELL:
    cell = GAMGI_CAST_CELL parent;
    gamgi_math_vector_sub (origin, cell->origin, origin);
    break;
    }

  gamgi_mesa_translate_object (child, origin); 
  }
 
}

static void static_nodes (gamgi_cell *cell, gamgi_bool *nodes)
{
/***************************************************
 * copy nodes from global cache, then disable non  *
 * existent I,A,B,C,R nodes for this specific cell *
 ***************************************************/

nodes[STATIC_NODE_P] = cache.nodes[STATIC_NODE_P];
nodes[STATIC_NODE_I] = cache.nodes[STATIC_NODE_I];
nodes[STATIC_NODE_A] = cache.nodes[STATIC_NODE_A];
nodes[STATIC_NODE_B] = cache.nodes[STATIC_NODE_B];
nodes[STATIC_NODE_C] = cache.nodes[STATIC_NODE_C];
nodes[STATIC_NODE_R] = cache.nodes[STATIC_NODE_R];

switch (cell->lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  nodes[STATIC_NODE_A] = FALSE;
  nodes[STATIC_NODE_B] = FALSE;
  nodes[STATIC_NODE_C] = FALSE;
  nodes[STATIC_NODE_R] = FALSE;
  break;

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  nodes[STATIC_NODE_I] = FALSE;
  nodes[STATIC_NODE_R] = FALSE;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  nodes[STATIC_NODE_I] = FALSE;
  nodes[STATIC_NODE_A] = FALSE;
  nodes[STATIC_NODE_B] = FALSE;
  nodes[STATIC_NODE_R] = FALSE;
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  nodes[STATIC_NODE_I] = FALSE;
  nodes[STATIC_NODE_A] = FALSE;
  nodes[STATIC_NODE_B] = FALSE;
  nodes[STATIC_NODE_C] = FALSE;
  break;

  default:
  nodes[STATIC_NODE_I] = FALSE;
  nodes[STATIC_NODE_A] = FALSE;
  nodes[STATIC_NODE_B] = FALSE;
  nodes[STATIC_NODE_C] = FALSE;
  nodes[STATIC_NODE_R] = FALSE;
  break;
  }

}

static void static_rotation (gamgi_cell *cell, double angle, gamgi_enum axis,
gamgi_enum vectors, int *uvw, double *rotation, gamgi_window *window)
{
double quaternion[4];
double vector[3];

/***********************
 * get rotation matrix *
 ***********************/

if (axis == FALSE)
  gamgi_phys_plane_vector (cell, uvw, vectors, vector);
else
  gamgi_phys_direction_vector (cell, uvw, vectors, vector);

angle = GAMGI_MATH_DEG_RAD * angle;
gamgi_math_quaternion_from_axis (angle, vector, quaternion);
gamgi_math_quaternion_to_matrix (quaternion, rotation);
}

static void static_translation (gamgi_cell *cell, double *shift,
gamgi_enum vectors, double *translation, gamgi_window *window)
{
double basis[9], inverse[9];
double position[3];

/***************************************************************
 * convert coordinates in primitive vectors ([-1,+1]) or       *
 * absolute vectors (any) to conventional vectors ([-1,+1]):   *
 *                                                             *
 * [conv. vectors][conv. coord] = [prim. vectors][prim. coord] *
 * [conv. vectors][conv. coord] = [axes coord]                 *
 *                                                             *
 * this conversion must be done before generating positions    *
 * with space symmetry information, as wyckoff positions       *
 * are always given as [-1,+1] conventional coordinates        *
 ***************************************************************/

gamgi_math_vector_copy (shift, translation);
if (vectors != GAMGI_PHYS_CONVENTIONAL)
  {
  gamgi_math_matrix_absolute (basis,
  cell->a1[0], cell->a2[0], cell->a3[0],
  cell->a1[1], cell->a2[1], cell->a3[1],
  cell->a1[2], cell->a2[2], cell->a3[2]);
  gamgi_math_matrix_inverse (basis, inverse);

  if (vectors == GAMGI_PHYS_PRIMITIVE)
    {
    gamgi_math_matrix_absolute (basis,
    cell->p1[0], cell->p2[0], cell->p3[0],
    cell->p1[1], cell->p2[1], cell->p3[1],
    cell->p1[2], cell->p2[2], cell->p3[2]);
    }
  else gamgi_math_matrix_unit (basis);

  /***********************************
   * convert to conventional vectors *
   ***********************************/

  gamgi_math_matrix_vector (basis, translation, position);
  gamgi_math_matrix_vector (inverse, position, translation);

  /*****************************************
   * use coordinates in the range ]-1, +1[ *
   *****************************************/

  gamgi_math_vector_fmod (translation, translation);
  }
}

static gamgi_bool static_grab_object (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
const char *name;

/****************************************
 * get reference (child|parent) and     *
 * position (absolute|relative) options *
 ***************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_above");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.above = TRUE;
else
  cache.above = FALSE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_child");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.child = TRUE;
else
  cache.child = FALSE;

/*********************
 * x,y,z coordinates *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_shift_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.origin[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_grab_crystal (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;

if (cache.object->class != GAMGI_ENGINE_ATOM)
  {
  gamgi_gtk_dialog_message_create ("Error", "Sorry, not supported yet", window);
  return FALSE;
  }

/*****************
 * Position page *
 *****************/

/*******************************************
 * get Wyckoff data from combo box, arrays *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_wyckoff");
cache.site = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

cache.wyckoff = (const char **) g_object_get_data (G_OBJECT (dialog), "wyckoff");
cache.driver = (const int *) g_object_get_data (G_OBJECT (dialog), "driver");

/***************************************************************
 * get node position vectors, coordinates: coordinates for     *
 * absolute vectors must be real, coordinates for conventional *
 * and primitive vectors must be in the range ]-1, +1[         *
 ***************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_position_vectors");
cache.position_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

cache.position[0] = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && 
gamgi_io_token_double_scan (name, &cache.position[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Vector Coordinates", window);
  return FALSE;
  }

cache.position[1] = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && 
gamgi_io_token_double_scan (name, &cache.position[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Vector Coordinates", window);
  return FALSE;
  }

cache.position[2] = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &cache.position[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Vector Coordinates", window);
  return FALSE;
  }

if ((cache.position_vectors == GAMGI_PHYS_CONVENTIONAL ||
cache.position_vectors == GAMGI_PHYS_PRIMITIVE) && (abs (cache.position[0]) >= 1.0 ||
abs (cache.position[1]) >= 1.0 || abs (cache.position[2]) >= 1.0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid position vector coordinates", window);
  return FALSE;
  }
 
/***************
 * Change page *
 ***************/

/***************************************************************
 * get translation vectors, coordinates: coordinates for       *
 * absolute vectors must be real, coordinates for conventional *
 * and primitive vectors must be in the range ]-1, +1[         *
 ***************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_translation_vectors");
cache.translation_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.translation[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid shift coordinates", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.translation[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid change coordinates", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_translation_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.translation[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid change coordinates", window);
  return FALSE;
  }

if ((cache.translation_vectors == GAMGI_PHYS_CONVENTIONAL ||
cache.translation_vectors == GAMGI_PHYS_PRIMITIVE) &&
(abs (cache.translation[0]) >= 1.0 || abs (cache.translation[1]) >= 1.0 ||
abs (cache.translation[2]) >= 1.0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid change coordinates", window);
  return FALSE;
  }

/*********************************************************
 * get rotation angle, axis (direction | plane), vectors *
 * (conventional | primitive), axis coordinates          *
 *********************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.angle, -360.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid rotation angle", window);
  return FALSE;
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axis");
cache.axis = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_rotation_vectors");
cache.rotation_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_u");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cache.uvw[0], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid rotation axis", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cache.uvw[1], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid rotation axis", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_w");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cache.uvw[2], -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid rotation axis", window);
  return FALSE;
  }

if (gamgi_phys_direction_zero (cache.uvw) == TRUE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid rotation axis", window);
  return FALSE;
  }

/******************
 * Occupancy page *
 ******************/

/*********************************************
 * get cutoff, active nodes, occupancy rules *
 *********************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_cutoff");
cache.cutoff = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_p");
cache.nodes[STATIC_NODE_P] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_i");
cache.nodes[STATIC_NODE_I] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_a");
cache.nodes[STATIC_NODE_A] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_b");
cache.nodes[STATIC_NODE_B] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_c");
cache.nodes[STATIC_NODE_C] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "check_button_r");
cache.nodes[STATIC_NODE_R] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, cache.rules[STATIC_RULE_A], 
GAMGI_IO_RULES, GAMGI_ENGINE_STRING) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid occupancy rule", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, cache.rules[STATIC_RULE_B], 
GAMGI_IO_RULES, GAMGI_ENGINE_STRING) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid occupancy rule", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_alpha_scan (name, cache.rules[STATIC_RULE_C], 
GAMGI_IO_RULES, GAMGI_ENGINE_STRING) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid occupancy rule", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_grab_glass (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
const char *name;

if (cache.object->class != GAMGI_ENGINE_ATOM)
  {
  gamgi_gtk_dialog_message_create ("Error", "Sorry, not supported yet", window);
  return FALSE;
  }

/***************************************************************
 * calculate number of atoms that will have approximately the  *
 * same radius as the template or get explicit number of atoms *
 ***************************************************************/

cache.atoms = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_int_scan (name, 
&cache.atoms, GAMGI_PHYS_RCP_ATOMS, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid number of atoms", window);
  return FALSE;
  }
  
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_time");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cache.time, 
GAMGI_PHYS_RCP_TIME_MIN, GAMGI_PHYS_RCP_TIME_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid relaxation data", window);
  return FALSE;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &cache.seed, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid seed data", window);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_grab (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;

/*********************************************************
 * get global object: currently only atoms are supported *
 *********************************************************/

cache.object = gamgi_gtk_object_name_number (dialog,
"entry_object", static_class (window), NULL);
if (cache.object == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid object", window);
  return FALSE;
  }

/*******************
 * get link method *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
cache.method = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (cache.method == STATIC_OBJECT) return static_grab_object (window);
if (cache.method == STATIC_CRYSTAL) return static_grab_crystal (window);
if (cache.method == STATIC_RANDOM) return static_grab_glass (window);

return FALSE;
}

static gamgi_bool static_apply_object (gamgi_object *child, gamgi_object *parent,
gamgi_layer *layer_child, gamgi_layer *layer_parent, gamgi_window *window_dialog)
{
/********************************************
 * check if child already belongs to parent *
 ********************************************/

if (child->object == parent)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Child already linked to parent", window_dialog);
  return FALSE;
  }

/**********************************************
 * save layers before linking child to parent *
 **********************************************/

if (cache.local == TRUE)
  {
  gamgi_gtk_history_update (layer_parent, layer_parent->undo, GAMGI_ENGINE_UNDO);
  if (layer_child != layer_parent)
    gamgi_gtk_history_update (layer_child, layer_child->undo, GAMGI_ENGINE_UNDO);
  }

if (cache.local == FALSE && cache.above == FALSE)
  {
  /*******************************************
   * as child object is a new instance,      *
   * there is no need to check for recursive *
   * objects or to check for half bonds      *
   *******************************************/

  child = gamgi_engine_copy_object (child);
  }
else
  {
  /************************************
   * remove all bonds that are not    *
   * entirely inside the child object *
   ************************************/

  if (layer_parent != layer_child) gamgi_chem_bond_half_remove (child);

  gamgi_engine_unlink_object (child);
  }

gamgi_engine_link_object_object (child, parent);

/**************************
 * translate child object *
 **************************/

static_origin (child, parent);

return TRUE;
}

static gamgi_bool static_apply_crystal (gamgi_cell *cell, gamgi_window *window)
{
double rotation[9];
double translation[3];
gamgi_bool nodes[6];

if (cell->model == GAMGI_PHYS_PROJECTION) 
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell type", window);
  return FALSE;
  }

/*************************************************************************
 * First, disable nodes that do not exist for this cell. This is needed, *
 * because in List mode, all nodes must available, as cells are not      *
 * known in advance. Then, check that some selected nodes still exist.   *
 ************************************************************************/

static_nodes (cell, nodes);
if (nodes[STATIC_NODE_P] == FALSE && nodes[STATIC_NODE_I] == FALSE
&& nodes[STATIC_NODE_A] == FALSE && nodes[STATIC_NODE_B] == FALSE
&& nodes[STATIC_NODE_C] == FALSE && nodes[STATIC_NODE_R] == FALSE)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid node occupancy", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/****************************************
 * 1) get translation and rotation data *
 * 2) link objects to cell and reset    *
 ****************************************/

static_rotation (cell, cache.angle, cache.axis,
cache.rotation_vectors, cache.uvw, rotation, window);

static_translation (cell, cache.translation,
cache.translation_vectors, translation, window);

gamgi_phys_cell_link (cell, GAMGI_CAST_ATOM cache.object, cache.position_vectors,
cache.wyckoff, cache.driver, cache.site, cache.position, translation, rotation,
cache.cutoff, nodes[STATIC_NODE_P], nodes[STATIC_NODE_I], nodes[STATIC_NODE_A],
nodes[STATIC_NODE_B], nodes[STATIC_NODE_C], nodes[STATIC_NODE_R],
cache.rules[STATIC_RULE_A], cache.rules[STATIC_RULE_B],
cache.rules[STATIC_RULE_C], window);

return TRUE;
}

static gamgi_bool static_apply_glass (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *text, *dialog;

if (cell->model == GAMGI_PHYS_PROJECTION)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell type", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

text = NULL;
if (cache.local == TRUE)
  text = gamgi_gtk_dialog_text_create ();

/*****************************************************
 * 1) create and initialize text widget, to show RCP *
 * report; 2) apply RCP algorithm; 3) show report    *
 *****************************************************/

if (gamgi_phys_rcp (cell, GAMGI_CAST_ATOM cache.object,
cache.atoms, cache.seed, cache.time, text) == FALSE)
  {
  if (cache.local == TRUE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid packing data", window);
    gtk_widget_destroy (text);
    }
  return FALSE;
  }

if (cache.local == TRUE)
  {
  dialog = gamgi_gtk_dialog_report_create (text,
  "Random Close Packing", GAMGI_GTK_CELL_LINK_RCP, window);
  gtk_widget_show (dialog);
  }

return TRUE;
}

static gamgi_slist *static_ok_global_object (gamgi_window *window_dialog)
{
gamgi_window *window_parent;
gamgi_layer *layer_child, *layer_parent;
gamgi_object *child, *parent;
gamgi_dlist *dlist;
gamgi_slist *start;

if (cache.above == TRUE)
  {
   /******************************************
   * link above: link each listed cell       *
   * (child) to the selected object (parent) *
   *******************************************/

  parent = cache.object;
  layer_parent = gamgi_engine_find_layer (parent);

  start = NULL;
  for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
    {
    child = GAMGI_CAST_OBJECT dlist->data;
    layer_child = gamgi_engine_find_layer (child);

    if (static_apply_object (child, parent,
    layer_child, layer_parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }

  /********************************************************
   * raise to top window and layer where parent object is *
   ********************************************************/

  window_parent = GAMGI_CAST_WINDOW layer_parent->object.object;
  gamgi_gtk_object_raise (layer_parent, window_parent);
  } 
else
  {
  /**************************************************
   * link below: link each listed cell (parent) to  *
   * a child object copied from the selected object *
   **************************************************/

  child = cache.object;

  start = NULL;
  for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
    {
    parent = GAMGI_CAST_OBJECT dlist->data;
    layer_parent = gamgi_engine_find_layer (parent);

    if (static_apply_object (child, parent,
    layer_parent, layer_parent, window_dialog) == FALSE)
      {
      start = gamgi_engine_slist_add_start (start);
      start->data = dlist->data;
      }
    }
  }

return start;
}

static gamgi_slist *static_ok_global_crystal (gamgi_window *window)
{
gamgi_cell *cell;
gamgi_dlist *dlist;
gamgi_slist *start;

start = NULL;
for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  {
  cell = GAMGI_CAST_CELL dlist->data;
  if (static_apply_crystal (cell, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
  }

return start;
}

static gamgi_slist *static_ok_global_glass (gamgi_window *window)
{
gamgi_cell *cell;
gamgi_dlist *dlist;
gamgi_slist *start;

start = NULL;
for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  {
  cell = GAMGI_CAST_CELL dlist->data;
  if (static_apply_glass (cell, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
  }

return start;
}

static void static_ok_local_object (gamgi_cell *cell, gamgi_window *window_dialog)
{
gamgi_window *window_child, *window_parent;
gamgi_layer *layer_child, *layer_parent;
gamgi_object *child, *parent;

/**********************************************************************
 * link above: link selected cell (child) to selected object (parent) *
 * link below: link selected cell (parent) to selected object (child) *
 **********************************************************************/

if (cache.above == TRUE)
  { child = GAMGI_CAST_OBJECT cell; parent = cache.object; }
else
  { child = cache.object; parent = GAMGI_CAST_OBJECT cell; }

layer_parent = gamgi_engine_find_layer (parent);
window_parent =  GAMGI_CAST_WINDOW layer_parent->object.object;
layer_child = gamgi_engine_find_layer (child);
window_child =  GAMGI_CAST_WINDOW layer_child->object.object;

/***********************************
 * If child is the current object, *
 * then replace by current layer   *
 ***********************************/

if (child == window_child->focus)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT
  window_child->layer, window_child);

static_apply_object (child, parent, layer_child, layer_parent, window_dialog);

/********************************************************
 * raise to top window and layer where parent object is *
 ********************************************************/

gamgi_gtk_object_raise (layer_parent, window_parent);

/***************************************************
 * redraw drawing area for parent and child window *
 ***************************************************/

gtk_widget_queue_draw (window_parent->area);
if (window_child != window_parent)
  gtk_widget_queue_draw (window_child->area);

/****************
 * reset dialog *
 ****************/

static_clean (window_dialog);
}

static gamgi_bool static_ok_local_crystal (gamgi_cell *cell, gamgi_window *window)
{
if (static_apply_crystal (cell, window) == FALSE) return FALSE;

/************************
 * redraw gl_area image *
 ************************/

gtk_widget_queue_draw (window->area);

/**********************************
 * reset dialog, wyckoff position *
 **********************************/

static_clean (window);
static_wyckoff_start (cell, window);

return TRUE;
}

static gamgi_bool static_ok_local_glass (gamgi_cell *cell, gamgi_window *window)
{
/*****************************************************
 * 1) create and initialize text widget, to show RCP *
 * report; 2) apply RCP algorithm; 3) show report    *
 *****************************************************/

if (static_apply_glass (cell, window) == FALSE) return FALSE;

/************************
 * redraw gl_area image *
 ************************/

gtk_widget_queue_draw (window->area);

return TRUE;
}

static void static_ok_global (gamgi_window *window)
{
gamgi_slist *start;

cache.local = FALSE;

/***********************
 * get child cell list *
 ***********************/

if (gamgi->cells == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of cell objects", window); return; }

if (static_grab (window) == FALSE) return;

/************************************************
 * For all windows: save layers as undo layers  *
 * and set current layer as new current object  *
 *                                              *
 * Link listed objects and show error message   *
 * with objects that could not be linked        *
 *                                              *
 * For all windows: redraw gl_area image        *
 ************************************************/

gamgi_global_selection_undo ();
gamgi_global_selection_focus ();

start = NULL;
if (cache.method == STATIC_OBJECT) start = static_ok_global_object (window);
if (cache.method == STATIC_CRYSTAL) start = static_ok_global_crystal (window);
if (cache.method == STATIC_RANDOM) start = static_ok_global_glass (window);
gamgi_io_error_list (start, window);

gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window);
}

static void static_ok_local (gamgi_window *window)
{
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;

cache.local = TRUE;

/********************************
 * pick object, get dialog data *
 ********************************/

cell = GAMGI_CAST_CELL gamgi_gtk_object_name_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window);
if (cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell object", window);
  return;
  }

if (static_grab (window) == FALSE) return;

if (cache.method == STATIC_OBJECT) static_ok_local_object (cell, window);
if (cache.method == STATIC_CRYSTAL) static_ok_local_crystal (cell, window);
if (cache.method == STATIC_RANDOM) static_ok_local_glass (cell, window);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget* button;

/*******************************************
 * link data: execute global or local task *
 *******************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  static_ok_global (window);
else
  static_ok_local (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *combo_above, *combo_below, *combo;
GtkRequisition size;
int width, height;

/***********************************************************
 * combo boxes above and below need different sizes: use   *
 * the largest size for both, so the size remains constant *
 ***********************************************************/

combo_above = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_above");
gtk_widget_size_request (GTK_WIDGET (combo_above), &size);
width = size.width; height = size.height;

combo_below = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_below");
gtk_widget_size_request (GTK_WIDGET (combo_below), &size);
if (size.width > width) width = size.width;
if (size.height > height) height = size.height;

gtk_widget_set_size_request (combo_above, width, height);
gtk_widget_set_size_request (combo_below, width, height);

/*************************************************
 * show combo box BEFORE measuring notebook size *
 *************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_below");
gtk_button_clicked (GTK_BUTTON (button));

/***************************************************
 * Set notebook current size as its maximum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

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

/**********************
 * set default method *
 **********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_method");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_OBJECT);
static_method (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window_dialog = gamgi->window_dialog;
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];
const char *name;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
gamgi_io_token_check (name) == TRUE)
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_object");

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/***************** external function ****************
 *                                                  *
 *              GAMGI_GTK_CELL_LINK_PRESS           *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cell_link_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;
GtkWidget *entry;
gamgi_bool global;
const char *name;

/***********************************************
 * global selection can be used only to select *
 * the second object, not the main object(s)   *
 ***********************************************/
 
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE && 
window_dialog != window_mouse) return;

/***********************************************
 * global selection can be used only to select *
 * the second object, not the main object      *
 ***********************************************/

global = TRUE;
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE &&
gamgi_io_token_check (name) == FALSE) global = FALSE;

/************************************************************************
 * window_dialog is the (local) window where the dialog window was open *
 * window_mouse is the (global) window where the user presses the mouse *
 ************************************************************************/

if (global == FALSE && window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y,
static_class (window_dialog), global, static_press);
}

/*************** external function ******************
 *                                                  *
 *               GAMGI_GTK_CELL_LINK                *
 *                                                  *
 *  Creates the dialog window used to link cells.   *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cell_link (GtkWidget *widget, void *data)
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
gamgi_bool *sensitive;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Cell Link", window);
window->action = GAMGI_GTK_CELL_LINK;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Cell objects *
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
g_object_set_data (G_OBJECT (dialog), "hbox_cell", hbox_left_left);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Cell");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_signal_connect (entry, "changed", G_CALLBACK (static_cell), window);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
gtk_widget_show (entry);

/********
 * List *
 ********/

button = gtk_toggle_button_new_with_label (" List ");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_list), window);
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

/***************
 * Object page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Object ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

/*************
 * Hierarchy *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Hierarchy");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Above");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0); 
g_signal_connect (button, "clicked", G_CALLBACK (static_hierarchy), window);
g_object_set_data (G_OBJECT (dialog), "button_above", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Below");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_hierarchy), window);
g_object_set_data (G_OBJECT (dialog), "button_below", button);
gtk_widget_show (button);

/**********
 * object *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/************************************
 * hide combo box for objects above *
 ************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_above", combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Layer", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Assembly", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/************************************
 * hide combo box for objects below *
 ************************************/

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_below", combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Cluster", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Molecule", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Group", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Plane", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Direction", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Orbital", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Text", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0); 
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_object", entry);
gtk_widget_show (entry);

/**********
 * method *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Method");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_method), window);
g_object_set_data (G_OBJECT (dialog), "combo_method", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Object", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Crystal", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Random", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (3);
g_object_set_data (G_OBJECT (dialog), "sensitive_link", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/**************
 * Shift page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Shift ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_shift", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/****************
 * Local,Parent *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Reference");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Child");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_child", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Parent");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_parent", button);
gtk_widget_show (button);

/***********
 * 2nd row *
 ***********/

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
g_object_set_data (G_OBJECT (dialog), "entry_shift_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_shift_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_shift_z", entry);
gtk_widget_show (entry);

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
 * Vectors combo box *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_position_vectors", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Absolute", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********************
 * Wyckoff combo box *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Wyckoff");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_wyckoff_change), window);
g_object_set_data (G_OBJECT (dialog), "combo_wyckoff", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, " 1  Basis  1     ", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/***************
 * Coordinates *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_x", entry);
gtk_widget_show (entry);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_y", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_z", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/***************
 * Change page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Change ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_change", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***************
 * Translation *
 **************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_translation_vectors", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Absolute", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_translation_x", entry);
gtk_widget_show (entry);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_translation_y", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_translation_z", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/************
 * Rotation *
 ************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Angle");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_angle", entry);
gtk_widget_show (entry);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Axis");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_axis), window);
g_object_set_data (G_OBJECT (dialog), "combo_axis", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Plane", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Direction", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Vectors");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_rotation_vectors", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_u", entry);
gtk_widget_show (entry);

label = gtk_label_new ("U");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_u", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_v", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_w", entry);
gtk_widget_show (entry);

label = gtk_label_new ("W");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_w", label);
gtk_widget_show (label);

/******************
 * Occupancy page *
 ******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Occupancy");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_occupancy", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/********************
 * Cutoff combo box *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Cutoff");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_cutoff", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Objects", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Nodes", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********
 * Nodes *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Nodes");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_check_button_new_with_label ("P");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_p", button);
gtk_widget_show (button);

button = gtk_check_button_new_with_label ("I");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_i", button);
gtk_widget_show (button);

button = gtk_check_button_new_with_label ("A");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_a", button);
gtk_widget_show (button);

button = gtk_check_button_new_with_label ("B");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_b", button);
gtk_widget_show (button);

button = gtk_check_button_new_with_label ("C");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_c", button);
gtk_widget_show (button);

button = gtk_check_button_new_with_label ("R");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "check_button_r", button);
gtk_widget_show (button);

/*******************
 * Occupancy rules *
 *******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Rules");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (vbox_top_top), table, FALSE, FALSE, 0);
gtk_widget_show (table);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_ENGINE_STRING);
g_object_set_data (G_OBJECT (dialog), "entry_a", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_ENGINE_STRING);
g_object_set_data (G_OBJECT (dialog), "entry_b", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_ENGINE_STRING);
g_object_set_data (G_OBJECT (dialog), "entry_c", entry);
gtk_widget_show (entry);

label = gtk_label_new ("A");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("C");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

/****************
 * Packing page *
 ****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Packing");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_packing", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/******************
 * Radius, Number *
 ******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Atom");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Radius");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_number), window);
g_object_set_data (G_OBJECT (dialog), "button_radius", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Number");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_number), window);
g_object_set_data (G_OBJECT (dialog), "button_number", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_number", entry);
gtk_widget_show (entry);

/********************
 * Relaxation, Seed *
 ********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Relaxation");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_time", entry);
gtk_widget_show (entry);

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Seed");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_seed", entry);
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
g_signal_connect (button, "clicked", G_CALLBACK (static_ok), window);
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
