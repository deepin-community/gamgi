/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_create.c
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
#include "gamgi_phys.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_gtk_cell_align.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_area.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_phys_cell.h"
#include "gamgi_io_token.h"

static void static_vectors (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_enum lattice, vectors;
int row;

/***************************************************
 * this is not necessary but prevents a O3 warning *
 ***************************************************/

vectors = FALSE;
lattice = FALSE;

/************************************************************
 *                          update o4                       *
 *                                                          *
 * Conventional vectors: get lattice, update accordingly    *
 * Primitive vectors: disable centered items, set o4 to 000 *
 * Local vectors: enable centered items, set o4 to Local    *
 ************************************************************/

row = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

if (row == GAMGI_PHYS_CONVENTIONAL - 1)
  {
  vectors = GAMGI_PHYS_CONVENTIONAL;
  lattice = gamgi_gtk_cell_symmetry_lattice (dialog);
  }

if (row == GAMGI_PHYS_PRIMITIVE - 1)
  {
  vectors = GAMGI_PHYS_PRIMITIVE;
  lattice = GAMGI_PHYS_CUBIC_P;
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
  }

gamgi_gtk_cell_symmetry_nodes (lattice, vectors, dialog);
}

static void static_faces (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_bool *sensitive;
int row;

/*************************
 * get borders combo box *
 *************************/

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_borders");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*************************************************
   * if the borders option is all, change to faces *
   *************************************************/

  if (row == GAMGI_PHYS_ALL - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_FACES - 1);
  sensitive[GAMGI_PHYS_ALL - 1] = FALSE;
  }
else sensitive[GAMGI_PHYS_ALL - 1] = TRUE;
}

static void static_model (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_n1, *entry_n2, *entry_n3;
GtkWidget *label_n1, *label_n2, *label_n3;
GtkWidget *entry_v1, *entry_v2, *entry_v3;
GtkWidget *label_v1, *label_v2, *label_v3;
GtkWidget *entry_v12, *entry_v13, *entry_v23;
GtkWidget *label_v12, *label_v13, *label_v23;
GtkWidget *button_faces, *button_nodes;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
gamgi_bool *sensitive;
int model, borders;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
model = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*******************************************
 * set N1, N2, N3 entries and Faces button *
 *******************************************/

entry_n1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
entry_n2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
entry_n3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");

label_n1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n1");
label_n2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n2");
label_n3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n3");

button_faces = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_faces");

if (model != GAMGI_PHYS_PARALLELEPIPED - 1 && 
model != GAMGI_PHYS_SPHERE - 1 && model != GAMGI_PHYS_PROJECTION - 1)
  {
  /******************
   * enable widgets *
   ******************/

  gtk_widget_set_sensitive (label_n1, TRUE);
  gtk_widget_set_sensitive (label_n2, TRUE);
  gtk_widget_set_sensitive (label_n3, TRUE);

  gtk_widget_set_sensitive (entry_n1, TRUE);
  gtk_widget_set_sensitive (entry_n2, TRUE);
  gtk_widget_set_sensitive (entry_n3, TRUE);

  gtk_widget_set_sensitive (button_faces, TRUE);

  /**********************
   * initialize widgets *
   **********************/

  name = gtk_entry_get_text (GTK_ENTRY (entry_n1));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N1);
    gtk_entry_set_text (GTK_ENTRY(entry_n1), token);
    }
  name = gtk_entry_get_text (GTK_ENTRY (entry_n2));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N2);
    gtk_entry_set_text (GTK_ENTRY(entry_n2), token);
    }
  name = gtk_entry_get_text (GTK_ENTRY (entry_n3));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N3);
    gtk_entry_set_text (GTK_ENTRY(entry_n3), token);
    }
  }
else
  {
  /*******************
   * disable widgets *
   *******************/

  gtk_entry_set_text (GTK_ENTRY (entry_n1), "");
  gtk_entry_set_text (GTK_ENTRY (entry_n2), "");
  gtk_entry_set_text (GTK_ENTRY (entry_n3), "");

  gtk_widget_set_sensitive (entry_n1, FALSE);
  gtk_widget_set_sensitive (entry_n2, FALSE);
  gtk_widget_set_sensitive (entry_n3, FALSE);

  gtk_widget_set_sensitive (label_n1, FALSE);
  gtk_widget_set_sensitive (label_n2, FALSE);
  gtk_widget_set_sensitive (label_n3, FALSE);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_faces), FALSE);
  gtk_widget_set_sensitive (button_faces, FALSE);
  }

/*****************************************
 * set V1, V2, V3, V12, V13, V23 entries *
 *****************************************/

entry_v1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v1");
entry_v2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v2");
entry_v3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v3");
entry_v12 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v12");
entry_v13 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v13");
entry_v23 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v23");

label_v1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v1");
label_v2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v2");
label_v3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v3");
label_v12 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v12");
label_v13 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v13");
label_v23 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_v23");

if (model == GAMGI_PHYS_PARALLELEPIPED - 1 || model == GAMGI_PHYS_SPHERE - 1)
  {
  /******************
   * enable widgets *
   ******************/

  gtk_widget_set_sensitive (label_v1, TRUE);
  gtk_widget_set_sensitive (entry_v1, TRUE);
  }
else
  {
  /*******************
   * disable widgets *
   *******************/

  gtk_entry_set_text (GTK_ENTRY (entry_v1), "");
  gtk_widget_set_sensitive (entry_v1, FALSE);
  gtk_widget_set_sensitive (label_v1, FALSE);
  }

if (model == GAMGI_PHYS_PARALLELEPIPED - 1)
  {
  /******************
   * enable widgets *
   ******************/

  gtk_widget_set_sensitive (label_v2, TRUE);
  gtk_widget_set_sensitive (label_v3, TRUE);
  gtk_widget_set_sensitive (label_v12, TRUE);
  gtk_widget_set_sensitive (label_v13, TRUE);
  gtk_widget_set_sensitive (label_v23, TRUE);

  gtk_widget_set_sensitive (entry_v2, TRUE);
  gtk_widget_set_sensitive (entry_v3, TRUE);
  gtk_widget_set_sensitive (entry_v12, TRUE);
  gtk_widget_set_sensitive (entry_v13, TRUE);
  gtk_widget_set_sensitive (entry_v23, TRUE);
  }
else
  {
  /*******************
   * disable widgets *
   *******************/

  gtk_entry_set_text (GTK_ENTRY (entry_v2), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v3), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v12), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v13), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v23), "");

  gtk_widget_set_sensitive (entry_v2, FALSE);
  gtk_widget_set_sensitive (entry_v3, FALSE);
  gtk_widget_set_sensitive (entry_v12, FALSE);
  gtk_widget_set_sensitive (entry_v13, FALSE);
  gtk_widget_set_sensitive (entry_v23, FALSE);

  gtk_widget_set_sensitive (label_v2, FALSE);
  gtk_widget_set_sensitive (label_v3, FALSE);
  gtk_widget_set_sensitive (label_v12, FALSE);
  gtk_widget_set_sensitive (label_v13, FALSE);
  gtk_widget_set_sensitive (label_v23, FALSE);
  }

/*******************************************
 * set default for origin and axes vectors *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
if (model == GAMGI_PHYS_PRIMITIVE - 1 || model == GAMGI_PHYS_WIGNER - 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PRIMITIVE - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
if (model == GAMGI_PHYS_PRIMITIVE - 1 || model == GAMGI_PHYS_WIGNER - 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PRIMITIVE - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

/***************************************
 * set Borders combo box, Nodes button *
 ***************************************/

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_borders");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
borders = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

button_nodes = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_nodes");

if (model != GAMGI_PHYS_PARALLELEPIPED - 1 &&
model != GAMGI_PHYS_SPHERE - 1 && model != GAMGI_PHYS_PROJECTION - 1)
  {
  /****************************
   * enable Borders combo box *
   ****************************/

  sensitive[GAMGI_PHYS_FACES - 1] = TRUE;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_faces)) == FALSE)
    sensitive[GAMGI_PHYS_ALL - 1] = TRUE;
  }
else
  {
  /*****************************
   * disable Borders combo box *
   *****************************/

  if (borders == GAMGI_PHYS_ALL - 1 || borders == GAMGI_PHYS_FACES - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_EDGES - 1);
  sensitive[GAMGI_PHYS_ALL - 1] = FALSE;
  sensitive[GAMGI_PHYS_FACES - 1] = FALSE;
  }

if (model != GAMGI_PHYS_PROJECTION - 1)
  {
  /***********************
   * enable Nodes button *
   ***********************/

  gtk_widget_set_sensitive (button_nodes, TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_nodes), TRUE);

  /***********************
   * enable Borders None *
   ***********************/

  sensitive[GAMGI_PHYS_NONE - 1] = TRUE;
  }
else
  {
  /************************
   * disable Nodes button *
   ************************/

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_nodes), FALSE);
  gtk_widget_set_sensitive (button_nodes, FALSE);

  /************************
   * Disable Borders None *
   ************************/

  if (borders == GAMGI_PHYS_NONE - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_EDGES - 1);
  sensitive[GAMGI_PHYS_NONE - 1] = FALSE;
  }
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

  static_button (dialog, "button_system");
  static_button (dialog, "button_lattice");
  static_button (dialog, "button_group");

  static_button (dialog, "button_translation");
  static_button (dialog, "button_rotation");
  }

}

/**************** internal function **************
 *                                               *
 *                   STATIC_RESET                *
 *                                               *
 * Reset the cell create dialog, cleaning only   *
 * the x,y,z and e1,e2,e3 entries, because the   *
   user might wish to create more similar cells. *
 *                                               *
 *********************************************** */

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

/*********************
 * initialize origin *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_X);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Y);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Z);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

/**************************
 * initialize orientation *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E1);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E2);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E3);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *combo;
GtkWidget *entry;
const char *name;
double origin[3], euler[3];
int o1, o2, o3;
gamgi_enum o4;

cell = gamgi_engine_create_cell ();
gamgi_engine_start_cell (cell);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_cell",
"Cell", cell->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Name", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/***************************************
 * get Bravais lattice and space group *
 ***************************************/

if (gamgi_gtk_cell_symmetry_create (&cell->lattice, 
&cell->group, window) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Symmetry", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/**************************
 * get lattice parameters *
 **************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->a, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->b, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->c, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ab");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->ab, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ac");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->ac, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bc");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE && gamgi_io_token_double_scan (name, 
&cell->bc, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*****************************************
 * check and complete lattice parameters *
 *****************************************/

if (gamgi_gtk_cell_symmetry_parameters (cell, cell->lattice, &cell->a, 
&cell->b, &cell->c, &cell->ab, &cell->ac, &cell->bc) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Parameter Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/****************
 * Get position *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Origin Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Origin Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Origin Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/****************
 * Get rotation *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &euler[0], 0.0, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &euler[1], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE &&
gamgi_io_token_double_scan (name, &euler[2], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*******************
 * get Volume page *
 *******************/

/*********
 * Model *
 *********/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
cell->model = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**************
 * N1, N2, N3 *
 **************/

cell->n1 = 1;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_int_scan (name, 
&cell->n1, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data n1", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

cell->n2 = 1;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_int_scan (name, 
&cell->n2, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data n2", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

cell->n3 = 1;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_int_scan (name, 
&cell->n3, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Cell Data n3", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*****************************
 * V1, V2, V3, V12, V13, V23 *
 *****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v1, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v2, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v3, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v12");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v12, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v13");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v13, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v23");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gtk_widget_is_sensitive (entry) == TRUE && gamgi_io_token_double_scan (name, 
&cell->v23, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*********************************************************
 * the 3 angles together cannot be too close to 0 or 360 *
 *                                                       *
 * each angle cannot be larger than the sum of the other *
 * two, or smaller than the difference of the other two  *
 *********************************************************/

if (cell->model == GAMGI_PHYS_PARALLELEPIPED)
  {
  if (cell->v12 + cell->v13 + cell->v23 > 360.0 - GAMGI_MATH_TOLERANCE_ANGLE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    gamgi_engine_remove_cell (cell);
    return;
    }

  if (
  cell->v12 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v13 + cell->v23 ||
  cell->v13 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v12 + cell->v23 ||
  cell->v23 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v12 + cell->v13 )
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    gamgi_engine_remove_cell (cell);
    return;
    }

  if (
  cell->v12 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v13 - cell->v23) ||
  cell->v13 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v12 - cell->v23) ||
  cell->v23 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v12 - cell->v13) )
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    gamgi_engine_remove_cell (cell);
    return;
    }

  }

/*******************
 * get Origin page *
 *******************/

/***************
 * Origin node *
 ***************/

o1 = 0; 
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &o1, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid origin node data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

o2 = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &o2, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid origin node data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

o3 = 0;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &o3, -INT_MAX, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid origin node data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
o4 = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (gamgi_math_node_check (cell->lattice, o4) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid origin node data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/**************************************************
 * Origin node: Conventional or Primitive vectors *
 **************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
cell->origin_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/********
 * Axes *
 ********/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_yes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell->axes = TRUE;

/*******************************************
 * Axes: Conventional or Primitive vectors *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
cell->axes_vectors = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*****************
 * get View page *
 *****************/

/*******
 * red *
 *******/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell->red, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*********
 * green *
 *********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell->green, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/********
 * blue *
 ********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &cell->blue, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*************
 * Get scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &cell->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Scale Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

/*************************
 * get borders combo box *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
cell->borders = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**************************************
 * get faces and nodes toggle buttons *
 **************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_faces");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell->faces = TRUE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_nodes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cell->nodes = TRUE;

/*****************************************
 * Something in the cell must be visible *
 *****************************************/

if (cell->faces == FALSE && cell->nodes == FALSE && cell->borders == GAMGI_PHYS_NONE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid View Data", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

if (cell->model == GAMGI_PHYS_WIGNER && (cell->lattice == GAMGI_PHYS_TRICLINIC_P ||
cell->lattice == GAMGI_PHYS_MONOCLINIC_P || cell->lattice == GAMGI_PHYS_MONOCLINIC_C) )
  {
  gamgi_gtk_dialog_message_create ("Error", "Sorry, not supported yet", window);
  gamgi_engine_remove_cell (cell);
  return;
  }

if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice, 
  o1, o2, o3, o4, &cell->o1, &cell->o2, &cell->o3);
else
  { cell->o1 = o1; cell->o2 = o2; cell->o3 = o3; }

gamgi_phys_cell_create (cell);

gamgi_gtk_cell_align_rotation_get (cell, euler, window);
gamgi_math_position_cell_rotation_set (cell, euler);

gamgi_gtk_cell_align_translation_get (cell, origin, window);
gamgi_math_position_cell_translation_set (cell, origin);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

gamgi_engine_link_cell_layer (cell, window->layer);

/*********************************************************
 * Redraw gl_area image and reset the cell create dialog *
 *********************************************************/

gtk_widget_queue_draw (window->area);
static_reset (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];

/*******************
 * start Type page *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), gamgi->cell->model - 1);
static_model (NULL, window);

/***********************
 * start Position page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_CELL_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E1);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E2);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_PHYS_CELL_E3);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*************************************
 * initialize second level dialogs:  *
 * align translation, align rotation *
 *************************************/

gamgi_gtk_cell_align_start ();

/*********************
 * Start Origin page *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
sprintf (token, "%d", GAMGI_PHYS_CELL_O1);
gtk_entry_set_text (GTK_ENTRY(entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
sprintf (token, "%d", GAMGI_PHYS_CELL_O2);
gtk_entry_set_text (GTK_ENTRY(entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
sprintf (token, "%d", GAMGI_PHYS_CELL_O3);
gtk_entry_set_text (GTK_ENTRY(entry), token);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

if (gamgi->cell->axes == TRUE)
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_yes");
else
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_axes_no");
gtk_button_clicked (GTK_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

/*******************
 * Start View page *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), gamgi->cell->borders - 1);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_faces");
if (gamgi->cell->faces == TRUE)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
else
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_nodes");
if (gamgi->cell->nodes == TRUE)
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
else
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->cell->red);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->cell->green);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->cell->blue);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_CELL_CREATE_PRESS        *
 *                                             *
 * Receives the 2D window x,y coordinates,     *
 * determines the 3D x,y,z object coordinates, *
 * prints them in the x, y, z entries and      *
 * runs static_ok (), as if the user had       *
 * pressed the Ok button.                      *
 *                                             *
 ***********************************************/

void gamgi_gtk_cell_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *entry;
GtkWidget *button;
double x_3d, y_3d, z_3d;
char string[GAMGI_ENGINE_TOKEN];

/******************************
 * local mouse selection only *
 ******************************/

if (window_dialog != window_mouse) return;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  { gamgi_gtk_cell_align_press (window_dialog, x, y); return; }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_rotation");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  { gamgi_gtk_cell_align_press (window_dialog, x, y); return; }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
if (gtk_widget_is_sensitive (entry) == FALSE) return;

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
 *             GAMGI_GTK_CELL_CREATE                *
 *                                                  *
 *  Creates the dialog window used to create cells. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cell_create (GtkWidget *widget, void *data)
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

dialog = gamgi_gtk_dialog_task0_create ("Cell Create", window);
window->action = GAMGI_GTK_CELL_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*************
 * Cell Name *
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
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
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

/*****************
 * Symmetry page *
 *****************/

/************************************************
 * symmetry section (including level 2 dialogs) *
 ************************************************/

gamgi_gtk_cell_symmetry (notebook, window);

/*************************************
 * Volume page: Model, Length, Angle *
 *************************************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Volume");
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
 * Model combo box *
 *******************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Model");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_model), window);
g_object_set_data (G_OBJECT (dialog), "combo_model", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Conventional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Primitive", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Wigner-Seitz", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Parallelepiped", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Sphere", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Projection", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*******************************
 * Cell, Length, Angle entries * 
 *******************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (7, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

/*********
 * Cells *
 *********/

label = gtk_label_new ("Cells");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_n1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("N1");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_n1", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_n2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("N2");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_n2", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_n3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("N3");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_n3", label);
gtk_widget_show (label);

/**********
 * Length *
 **********/

label = gtk_label_new ("Length");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V1");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v1", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V2");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v2", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V3");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v3", label);
gtk_widget_show (label);

/*********
 * Angle *
 *********/

label = gtk_label_new ("Angle");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v23", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V23");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v23", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v13", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V13");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v13", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_v12", entry);
gtk_widget_show (entry);

label = gtk_label_new ("V12");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_v12", label);
gtk_widget_show (label);

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

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_toggle_button_new_with_label ("Align");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_cell_align_translation), window);
g_object_set_data (G_OBJECT (dialog), "button_translation", button);
gtk_widget_show (button);

/***********
 * new row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_x", label);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_y", label);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_z", label);
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

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("    Rotation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_toggle_button_new_with_label ("Align");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_cell_align_rotation), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation", button);
gtk_widget_show (button);

/***********
 * new row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("E1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_e1", label);
gtk_widget_show (label);

label = gtk_label_new ("E2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_e2", label);
gtk_widget_show (label);

label = gtk_label_new ("E3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_e3", label);
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

/***************
 * Origin page *
 ***************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Origin");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * 1st row: Node *
 *****************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Node");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("O1");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("O2");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("O3");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o1", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o2", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_o3", entry);
gtk_widget_show (entry);

/************************
 * 2nd row: o4, Vectors *
 ************************/

hbox_left = gtk_hbox_new (FALSE, 25);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_end (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_vectors), window);
g_object_set_data (G_OBJECT (dialog), "combo_origin_vectors", combo);
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

label = gtk_label_new ("Vectors");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_end (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_o4", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "000", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "011", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "101", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "110", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "111", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "122", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "211", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (7);
g_object_set_data (G_OBJECT (dialog), "sensitive_o4", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

label = gtk_label_new ("O4");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/**************************
 * 3rd row: Axes, Vectors *
 **************************/

hbox_left = gtk_hbox_new (FALSE, 25);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_end (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

combo = gtk_combo_box_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_axes_vectors", combo);
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

label = gtk_label_new ("Vectors");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_end (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

button = gtk_radio_button_new_with_label (NULL, "No");
gtk_box_pack_end (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_no", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Yes");
gtk_box_pack_end (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_axes_yes", button);
gtk_widget_show (button);

label = gtk_label_new ("Axes");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

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

/**********************************
 * 1st row: Borders, Faces, Nodes *
 **********************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Borders");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_borders", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Faces", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Edges", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (4);
g_object_set_data (G_OBJECT (dialog), "sensitive_borders", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

button = gtk_toggle_button_new_with_label ("Faces");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_faces), window);
g_object_set_data (G_OBJECT (dialog), "button_faces", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("Nodes");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_nodes", button);
gtk_widget_show (button);

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

/******************
 * 3rd row: Scale *
 ******************/

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
