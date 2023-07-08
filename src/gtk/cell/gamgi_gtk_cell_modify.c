/***********************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_modify.c
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
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_gtk_cell_align.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_mesa_center.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_phys_cell.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_NAME = 0, STATIC_LATTICE_GROUP, STATIC_A, STATIC_B,
STATIC_C, STATIC_AB, STATIC_AC, STATIC_BC, STATIC_MODEL, STATIC_N1, STATIC_N2,
STATIC_N3, STATIC_V1, STATIC_V2, STATIC_V3, STATIC_V12, STATIC_V13, STATIC_V23,
STATIC_O1, STATIC_O2, STATIC_O3, STATIC_O4, STATIC_ORIGIN_VECTORS, STATIC_AXES,
STATIC_AXES_VECTORS, STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y,
STATIC_TRANSLATION_Z, STATIC_ROTATION_E1, STATIC_ROTATION_E2,
STATIC_ROTATION_E3, STATIC_BORDERS, STATIC_FACES, STATIC_NODES,
STATIC_RED, STATIC_GREEN, STATIC_BLUE, STATIC_SCALE };

static struct {
gamgi_bool local;
gamgi_bool new[STATIC_SCALE + 1];
char name[GAMGI_ENGINE_TOKEN];
int lattice, group;
double a, b, c, ab, ac, bc;
gamgi_enum model;
int n1, n2, n3;
double v1, v2, v3, v12, v13, v23;
int o1, o2, o3;
gamgi_enum o4, origin_vectors, axes, axes_vectors;
gamgi_bool translation_absolute, rotation_absolute;
double translation[3], rotation[3];
gamgi_enum borders, faces, nodes;
float red, green, blue;
double scale; } cache;

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
 *                   STATIC_RELATIVE               *
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
 *               STATIC_TRANSLATION_CLEAN          *
 *                                                 *
 * Reset the origin entries everytime the absolute *
 * or relative origin buttons are toggled.         *
 *                                                 *
 ***************************************************/

static void static_translation_clean (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/***************** internal function **************
 *                                                *
 *                STATIC_ROTATION_CLEAN           *
 *                                                *
 * Reset the angle entries everytime the absolute *
 * or relative angle buttons are toggled.         *
 *                                                *
 **************************************************/

static void static_rotation_clean (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_vectors (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_enum lattice, vectors;
int row;

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

else if (row == GAMGI_PHYS_PRIMITIVE - 1)
  {
  vectors = GAMGI_PHYS_PRIMITIVE;
  lattice = GAMGI_PHYS_CUBIC_P;
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
  }

else
  {
  vectors = GAMGI_PHYS_CONVENTIONAL;
  lattice = gamgi_gtk_cell_symmetry_lattice (dialog);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 7);
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

/******************************************
 * get Borders sensitive, Faces combo box *
 ******************************************/

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_borders");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == GAMGI_GTK_YES - 1)
  {
  /*****************************
   * disable Borders combo box *
   *****************************/

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row == GAMGI_PHYS_ALL - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_FACES - 1);
  sensitive[GAMGI_PHYS_ALL - 1] = FALSE;
  }
else
  {
  /****************************
   * enable Borders combo box *
   ****************************/

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
  row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  if (row != GAMGI_PHYS_PARALLELEPIPED - 1 &&
  row != GAMGI_PHYS_SPHERE - 1 && row != GAMGI_PHYS_PROJECTION - 1)
    sensitive[GAMGI_PHYS_ALL - 1] = TRUE;
  }
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
GtkWidget *combo;
GtkWidget *vbox_faces, *vbox_nodes;
GtkWidget *combo_faces, *combo_nodes, *combo_borders;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
int model, faces, borders;
gamgi_bool *sensitive;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
model = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/**********************************************
 * set N1, N2, N3 entries and Faces combo box *
 **********************************************/

entry_n1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
entry_n2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
entry_n3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");

label_n1 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n1");
label_n2 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n2");
label_n3 = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_n3");

vbox_faces = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_faces");
combo_faces = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
faces = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_faces));

if (model == GAMGI_PHYS_CONVENTIONAL - 1 ||
model == GAMGI_PHYS_PRIMITIVE - 1 || model == GAMGI_PHYS_WIGNER - 1)
  {
  name = gtk_entry_get_text (GTK_ENTRY (entry_n1));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N1);
    gtk_entry_set_text (GTK_ENTRY (entry_n1), token);
    }
  name = gtk_entry_get_text (GTK_ENTRY (entry_n2));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N2);
    gtk_entry_set_text (GTK_ENTRY (entry_n2), token);
    }
  name = gtk_entry_get_text (GTK_ENTRY (entry_n3));
  if (gamgi_io_token_check (name) == FALSE)
    {
    sprintf (token, "%d", GAMGI_PHYS_CELL_N3);
    gtk_entry_set_text (GTK_ENTRY (entry_n3), token);
    }
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_n1), "");
  gtk_entry_set_text (GTK_ENTRY (entry_n2), "");
  gtk_entry_set_text (GTK_ENTRY (entry_n3), "");
  }

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

  gtk_widget_set_sensitive (vbox_faces, TRUE);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_faces), 2);
  }
else
  {
  /*******************
   * disable widgets *
   *******************/

  gtk_widget_set_sensitive (entry_n1, FALSE);
  gtk_widget_set_sensitive (entry_n2, FALSE);
  gtk_widget_set_sensitive (entry_n3, FALSE);

  gtk_widget_set_sensitive (label_n1, FALSE);
  gtk_widget_set_sensitive (label_n2, FALSE);
  gtk_widget_set_sensitive (label_n3, FALSE);

  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_faces), GAMGI_GTK_NO - 1);
  gtk_widget_set_sensitive (vbox_faces, FALSE);
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

if (model != GAMGI_PHYS_PARALLELEPIPED - 1 && model != GAMGI_PHYS_SPHERE - 1)
  gtk_entry_set_text (GTK_ENTRY (entry_v1), "");

if (model == GAMGI_PHYS_CONVENTIONAL - 1 || model == GAMGI_PHYS_PRIMITIVE - 1 ||
model == GAMGI_PHYS_WIGNER - 1 || model == GAMGI_PHYS_PROJECTION - 1)
  {
  /******************
   * disable widget *
   ******************/

  gtk_widget_set_sensitive (entry_v1, FALSE);
  gtk_widget_set_sensitive (label_v1, FALSE);
  }
else
  {
  /*****************
   * enable widget *
   *****************/

  gtk_widget_set_sensitive (label_v1, TRUE);
  gtk_widget_set_sensitive (entry_v1, TRUE);
  }

if (model != GAMGI_PHYS_PARALLELEPIPED - 1)
  {
  /**********************
   * initialize widgets *
   **********************/

  gtk_entry_set_text (GTK_ENTRY (entry_v2), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v3), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v12), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v13), "");
  gtk_entry_set_text (GTK_ENTRY (entry_v23), "");
  }

if (model == GAMGI_PHYS_PARALLELEPIPED - 1 || model == 6)
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
if (model == 6) gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
else if (model == GAMGI_PHYS_PRIMITIVE - 1 || model == GAMGI_PHYS_WIGNER - 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PRIMITIVE - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
if (model == 6) gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
else if (model == GAMGI_PHYS_PRIMITIVE - 1 || model == GAMGI_PHYS_WIGNER - 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_PRIMITIVE - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_CONVENTIONAL - 1);

/******************************************
 * set Borders combo box, Nodes combo box *
 ******************************************/

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_borders");
combo_borders = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
borders = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_borders));

if (model == GAMGI_PHYS_PARALLELEPIPED - 1 ||
model == GAMGI_PHYS_SPHERE - 1 || model == GAMGI_PHYS_PROJECTION - 1)
  {
  /******************************
   * disable Borders All, Faces *
   ******************************/

  if (borders == GAMGI_PHYS_ALL - 1 || borders == GAMGI_PHYS_FACES - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_borders), GAMGI_PHYS_EDGES - 1);
  sensitive[GAMGI_PHYS_ALL - 1] = FALSE;
  sensitive[GAMGI_PHYS_FACES - 1] = FALSE;
  }
else
  {
  /*****************************
   * enable Borders All, Faces *
   *****************************/

  if (faces != GAMGI_GTK_YES - 1)
    sensitive[GAMGI_PHYS_ALL - 1] = TRUE;
  sensitive[GAMGI_PHYS_FACES - 1] = TRUE;
  }

vbox_nodes = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_nodes");
combo_nodes = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_nodes");

if (model == GAMGI_PHYS_PROJECTION - 1)
  {
  /*****************
   * disable Nodes *
   *****************/

  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_nodes), GAMGI_GTK_NO - 1);
  gtk_widget_set_sensitive (vbox_nodes, FALSE);

  /************************
   * disable Borders None *
   ************************/

  if (borders == GAMGI_PHYS_NONE - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_borders), GAMGI_PHYS_EDGES - 1);
  sensitive[GAMGI_PHYS_NONE - 1] = FALSE;
  }
else
  {
  /****************
   * enable Nodes *
   ****************/

  gtk_widget_set_sensitive (vbox_nodes, TRUE);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_nodes), 2);

  /***********************
   * enable Borders None *
   ***********************/

  sensitive[GAMGI_PHYS_NONE - 1] = TRUE;
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

/******************** internal function ****************
 *                                                     *
 *                       STATIC_RESET                  *
 *                                                     *
 * Reset the cell modify dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *vbox;
GtkWidget *combo;

/*******************
 * clean Cell name *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***********************************************************
 * clean symmetry: System, Lattice, Group, Lengths, Angles *
 ***********************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_system");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ab");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ac");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bc");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/****************************************************
 * clean Volume page: Model, Cells, Lengths, Angles *
 ****************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 6);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v1");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v2");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v3");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v12");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v13");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v23");
gtk_widget_set_sensitive (entry, TRUE);
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***********************
 * clean Position page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***********************
 * clean Rotation page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*********************
 * clean Origin page *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 7);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

/*********************************************************
 * clean View page: Borders, Faces, Nodes, Color, Scale  *
 *                                                       *
 * enable the vboxes first THEN update the combo boxes!  *
 *********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 4);

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_faces");
gtk_widget_set_sensitive (vbox, TRUE);
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_nodes");
gtk_widget_set_sensitive (vbox, TRUE);
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_nodes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");

static_model (NULL, window);
}

/****************** internal function **************
 *                                                 *
 *                    STATIC_LIST                  *
 *                                                 *
 ***************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_cell");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * modify a single cell directly *
   *********************************/

  gtk_label_set_text (GTK_LABEL (label), "Cell");
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
const char *name;
int row;

/***************************************
 * get Bravais lattice and space group *
 ***************************************/

cache.new[STATIC_LATTICE_GROUP] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE) cache.new[STATIC_LATTICE_GROUP] = TRUE;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE) cache.new[STATIC_LATTICE_GROUP] = TRUE;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE) cache.new[STATIC_LATTICE_GROUP] = TRUE;

if (cache.new[STATIC_LATTICE_GROUP] == TRUE &&
gamgi_gtk_cell_symmetry_create (&cache.lattice, &cache.group, window) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid lattice symmetry", window);
  return FALSE;
  }

/***********************
 * a, b, c, ab, ac, bc *
 ***********************/

cache.new[STATIC_A] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_A] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.a, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_B] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_B] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.b, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_C] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_C] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.c, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_AB] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ab");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_AB] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.ab, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_AC] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ac");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_AC] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.ac, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_BC] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bc");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BC] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.bc, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid lattice parameter", window);
    return FALSE;
    }
  }

/********************************************************
 * volume: model, n1, n2, n3, v1, v2, v3, v12, v13, v23 *
 ********************************************************/

cache.new[STATIC_MODEL] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 6) { cache.new[STATIC_MODEL] = TRUE; cache.model = row + 1; }

cache.new[STATIC_N1] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_N1] = TRUE;
  if (gamgi_io_token_int_scan (name, 
  &cache.n1, 1, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_N2] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_N2] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.n2, 1, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_N3] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_N3] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.n3, 1, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V1] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V1] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v1, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V2] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V2] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v2, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V3] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V3] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v3, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V12] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v12");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V12] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v12, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V13] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v13");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V13] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v13, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

cache.new[STATIC_V23] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v23");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_V23] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.v23, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

/******************************
 * translation: mode, x, y, z *
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid position", window);
    return FALSE;
    }
  }

/******************************
 * rotation: mode, e1, e2, e3 *
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
    return FALSE;
    }
  }

/***************************
 * o1, o2, o3, vectors, o4 *
 ***************************/

cache.new[STATIC_O1] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_O1] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.o1, -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid origin node", window);
    return FALSE;
    }
  }

cache.new[STATIC_O2] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_O2] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.o2, -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid origin node", window);
    return FALSE;
    }
  }

cache.new[STATIC_O3] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_O3] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.o3, -INT_MAX, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid origin node", window);
    return FALSE;
    }
  }

cache.new[STATIC_ORIGIN_VECTORS] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_ORIGIN_VECTORS] = TRUE;
  cache.origin_vectors = row + 1;
  }

cache.new[STATIC_O4] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 7) { cache.new[STATIC_O4] = TRUE; cache.o4 = row; }

cache.new[STATIC_AXES] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_AXES] = TRUE;
  if (row == GAMGI_GTK_YES - 1) cache.axes = TRUE;
  if (row == GAMGI_GTK_NO - 1) cache.axes = FALSE;
  }

cache.new[STATIC_AXES_VECTORS] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_AXES_VECTORS] = TRUE;
  cache.axes_vectors = row + 1;
  }

/*************************
 * borders, faces, nodes *
 *************************/

cache.new[STATIC_BORDERS] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 4)
  {
  cache.new[STATIC_BORDERS] = TRUE;
  cache.borders = 1 + row;
  }

cache.new[STATIC_FACES] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_FACES] = TRUE;
  if (row == GAMGI_GTK_YES - 1) cache.faces = TRUE;
  if (row == GAMGI_GTK_NO - 1) cache.faces = FALSE;
  }

cache.new[STATIC_NODES] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_nodes");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_NODES] = TRUE;
  if (row == GAMGI_GTK_YES - 1) cache.nodes = TRUE;
  if (row == GAMGI_GTK_NO - 1) cache.nodes = FALSE;
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

static gamgi_bool static_apply (gamgi_cell *cell, gamgi_window *window)
{
char name[GAMGI_ENGINE_TOKEN];
double euler_old[3], origin_old[3];
double euler[3], origin[3];
double zero[3];
double a, b, c, ab, ac, bc;
double v1, v2, v3, v12, v13, v23;
double scale;
float red, green, blue;
int n1, n2, n3;
int o1, o2, o3;
gamgi_enum o4;
gamgi_enum origin_vectors, axes_vectors;
gamgi_bool axes;
int lattice, group;
int model, borders, nodes, faces;

/*************************
 * import data from cell *
 *************************/

strcpy (name, cell->object.name);

lattice = cell->lattice;
group = cell->group;
a = 0.0;
b = 0.0;
c = 0.0;
ab = 0.0;
ac = 0.0;
bc = 0.0;

model = cell->model;
n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
v1 = cell->v1;
v2 = cell->v2;
v3 = cell->v3;
v12 = cell->v12;
v13 = cell->v13;
v23 = cell->v23;

gamgi_math_position_cell_translation_get (cell, origin_old);
if (cache.translation_absolute == TRUE)
  gamgi_math_vector_copy (origin_old, origin);
else
  gamgi_math_vector_zero (origin);

gamgi_math_position_cell_rotation_get (cell, euler_old);
if (cache.rotation_absolute == TRUE)
  gamgi_math_vector_copy (euler_old, euler);
else
  gamgi_math_vector_zero (euler);

if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_conventional (cell->lattice,
  cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);
else
  { o1 = cell->o1; o2 = cell->o2; o3 = cell->o3; o4 = FALSE; }
origin_vectors = cell->origin_vectors;
axes = cell->axes;
axes_vectors = cell->axes_vectors;

borders = cell->borders;
faces = cell->faces;
nodes = cell->nodes;
red = cell->red;
green = cell->green;
blue = cell->blue;
scale = cell->object.scale;

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_LATTICE_GROUP] == TRUE)
  { lattice = cache.lattice; group = cache.group; }
if (cache.new[STATIC_A] == TRUE) a = cache.a;
if (cache.new[STATIC_B] == TRUE) b = cache.b;
if (cache.new[STATIC_C] == TRUE) c = cache.c;
if (cache.new[STATIC_AB] == TRUE) ab = cache.ab;
if (cache.new[STATIC_AC] == TRUE) ac = cache.ac;
if (cache.new[STATIC_BC] == TRUE) bc = cache.bc;

if (cache.new[STATIC_MODEL] == TRUE) model = cache.model;
if (cache.new[STATIC_N1] == TRUE) n1 = cache.n1;
if (cache.new[STATIC_N2] == TRUE) n2 = cache.n2;
if (cache.new[STATIC_N3] == TRUE) n3 = cache.n3;
if (cache.new[STATIC_V1] == TRUE) v1 = cache.v1;
if (cache.new[STATIC_V2] == TRUE) v2 = cache.v2;
if (cache.new[STATIC_V3] == TRUE) v3 = cache.v3;
if (cache.new[STATIC_V12] == TRUE) v12 = cache.v12;
if (cache.new[STATIC_V13] == TRUE) v13 = cache.v13;
if (cache.new[STATIC_V23] == TRUE) v23 = cache.v23;

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

if (cache.new[STATIC_O1] == TRUE) o1 = cache.o1;
if (cache.new[STATIC_O2] == TRUE) o2 = cache.o2;
if (cache.new[STATIC_O3] == TRUE) o3 = cache.o3;
if (cache.new[STATIC_O4] == TRUE) o4 = cache.o4;
if (cache.new[STATIC_ORIGIN_VECTORS] == TRUE) origin_vectors = cache.origin_vectors;
if (cache.new[STATIC_AXES] == TRUE) axes = cache.axes;
if (cache.new[STATIC_AXES_VECTORS] == TRUE) axes_vectors = cache.axes_vectors;

if (cache.new[STATIC_BORDERS] == TRUE) borders = cache.borders;
if (cache.new[STATIC_FACES] == TRUE) faces = cache.faces;
if (cache.new[STATIC_NODES] == TRUE) nodes = cache.nodes;
if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

if (gamgi_gtk_cell_symmetry_parameters (cell, lattice, &a, &b, &c, &ab, &ac, &bc) == FALSE)
  {
  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid Parameter Data", window);
  return FALSE;
  }

if (model == GAMGI_PHYS_PARALLELEPIPED)
  {
  /*********************************************************
   * The three angles together must be smaller than 360,   *
   * each angle cannot be larger than the sum of the other *
   * two, or smaller than the difference of the other two. *
   *********************************************************/

  if (v12 + v13 + v23 > 360.0 - GAMGI_MATH_TOLERANCE_ANGLE)
    {
    if (cache.local == TRUE) gamgi_gtk_dialog_message_create
    ("Error", "Invalid volume parameters", window);
    return FALSE;
    }

  if (v12 + GAMGI_MATH_TOLERANCE_ANGLE > v13 + v23
  || v13 + GAMGI_MATH_TOLERANCE_ANGLE > v12 + v23
  || v23 + GAMGI_MATH_TOLERANCE_ANGLE > v12 + v13)
    {
    if (cache.local == TRUE) gamgi_gtk_dialog_message_create
    ("Error", "Invalid volume parameters", window);
    return FALSE;
    }

  if (v12 - GAMGI_MATH_TOLERANCE_ANGLE < abs (v13 - v23)
  || v13 - GAMGI_MATH_TOLERANCE_ANGLE < abs (v12 - v23)
  || v23 - GAMGI_MATH_TOLERANCE_ANGLE < abs (v12 - v13))
    {
    if (cache.local == TRUE) gamgi_gtk_dialog_message_create
    ("Error", "Invalid volume parameters", window);
    return FALSE;
    }
  }

if (o4 != FALSE && origin_vectors == GAMGI_PHYS_PRIMITIVE)
  {
  /*****************************************************
   * centered nodes: impossible with primitive vectors *
   *****************************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid origin node", window);
  return FALSE;
  }

if (o4 != FALSE && gamgi_math_node_check (lattice, o4) == FALSE)
  {
  /*********************************************************
   * centered nodes: possible only for some centered cells *
   *********************************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid origin node", window);
  return FALSE;
  }

if (borders == GAMGI_PHYS_NONE && faces == FALSE && nodes == FALSE)
  {
  /*****************************************
   * something in the cell must be visible *
   *****************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Incompatible View Data", window);
  return FALSE;
  }

if (borders == GAMGI_PHYS_ALL && faces == TRUE)
  {
  /****************************************************************
   * inside borders and solid faces cannot be used simultaneously *
   ****************************************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Incompatible View Data", window);
  return FALSE;
  }

if ((model == GAMGI_PHYS_PARALLELEPIPED || model == GAMGI_PHYS_SPHERE)
&& (borders == GAMGI_PHYS_ALL || borders == GAMGI_PHYS_FACES))
  {
  /**************************************************************
   * parallelepiped, sphere mode: borders must be edges or none *
   **************************************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Incompatible view data", window);
  return FALSE;
  }

if (model == GAMGI_PHYS_PROJECTION && borders != GAMGI_PHYS_EDGES)
  {
  /******************************************
   * projection mode: borders must be edges *
   ******************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Incompatible view data", window);
  return FALSE;
  }

if (model == GAMGI_PHYS_PROJECTION && nodes == TRUE)
  {
  /*****************************************
   * projection mode: nodes must be hidden *
   *****************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Incompatible view data", window);
  return FALSE;
  }

if (gamgi_gtk_cell_symmetry_parameters (cell,
lattice, &a, &b, &c, &ab, &ac, &bc) == FALSE)
  {
  /*****************************************
   * check and complete lattice parameters *
   *****************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid lattice parameter", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/*****************************************************
 * export data to cell: reset position; reset arrays *
 *****************************************************/

gamgi_math_vector_zero (zero);
gamgi_math_position_cell_rotation_set (cell, zero);
gamgi_math_cell_offset (cell, zero);
gamgi_math_position_cell_translation_set (cell, zero);

free (cell->points);
free (cell->loops);
free (cell->lines);

cell->points = NULL;
cell->loops = NULL;
cell->lines= NULL;

cell->n_loops = 0;
cell->n_lines = 0;
cell->n_nodes = 0;
cell->n_points = 0;

strcpy (cell->object.name, name);
cell->lattice = lattice;
cell->group = group;
cell->a = a;
cell->b = b;
cell->c = c;
cell->ab = ab;
cell->ac = ac;
cell->bc = bc;

cell->model = model;
cell->n1 = n1;
cell->n2 = n2;
cell->n3 = n3;
cell->v1 = v1;
cell->v2 = v2;
cell->v3 = v3;
cell->v12 = v12;
cell->v13 = v13;
cell->v23 = v23;

cell->origin_vectors = origin_vectors;
if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice,
  o1, o2, o3, o4, &cell->o1, &cell->o2, &cell->o3);
else
  { cell->o1 = o1; cell->o2 = o2; cell->o3 = o3; }
cell->axes = axes;
cell->axes_vectors = axes_vectors;

cell->borders = borders;
cell->nodes = nodes;
cell->faces = faces;
cell->red = red;
cell->green = green;
cell->blue = blue;
cell->object.scale = scale;

gamgi_phys_cell_create (cell);

gamgi_gtk_cell_align_rotation_get (cell, euler, window);
gamgi_math_position_cell_rotation_set (cell, euler);
gamgi_gtk_cell_align_translation_get (cell, origin, window);
gamgi_math_position_cell_translation_set (cell, origin);

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

if (gamgi->cells == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of cell objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid cell identity", window);
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
for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_CELL dlist->data, window) == FALSE)
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
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/*****************************************
 * pick cell and get name (possibly new) *
 *                                       *
 * check if focus object belongs to cell *
 *****************************************/

cache.new[STATIC_NAME] = TRUE;
cell = GAMGI_CAST_CELL gamgi_gtk_object_number (dialog,
"entry_cell", GAMGI_ENGINE_CELL, window, cache.name);
if (cell == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid cell identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT cell) == TRUE) focus = TRUE;

/********************************************
 * collect data from dialog and modify cell *
 ********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (cell, window) == FALSE) return;

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

/***********************************
 * initialize combo boxes to Local *
 ***********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 6);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 7);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 4);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_nodes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

/*************************************
 * initialize second level dialogs:  *
 * align translation, align rotation *
 *************************************/

gamgi_gtk_cell_align_start ();
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_cell *cell;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];
double euler[3], origin[3];
int o1, o2, o3;
gamgi_enum o4;

cell = GAMGI_CAST_CELL object;

/***************************************************************
 * Toggles the absolute coordinates radio buttons, because     *
 * cell coordinates are absolute. This in turn triggers the    *
 * coordinate cleaning functions. Therefore static_absolute () *
 * must be called before setting the cell coordinates.         *
 ***************************************************************/

static_reset (window);
static_absolute (dialog);

/***************************
 * set Cell identification *
 ***************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_cell");
sprintf (string, "%s %d", cell->object.name, cell->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/****************
 * set symmetry *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->a);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->b);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->c);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ab");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->ab);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ac");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->ac);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bc");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->bc);
gtk_entry_set_text (GTK_ENTRY (entry), string);

gamgi_gtk_cell_symmetry_press (cell, window);

/*********************************************
 * set Volume: Model, Cells, Lengths, Angles *
 *********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n1");
sprintf (string, "%d", cell->n1);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n2");
sprintf (string, "%d", cell->n2);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_n3");
sprintf (string, "%d", cell->n3);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v1");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->v1);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v2");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->v2);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v3");
sprintf (string, "%.*f", gamgi->gamgi->length, cell->v3);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v12");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->v12);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v13");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->v13);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_v23");
sprintf (string, "%.*f", gamgi->gamgi->angle, cell->v23);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_model");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell->model - 1);
static_model (NULL, window);

/*************************
 * set Position: Lengths *
 *************************/

gamgi_math_position_cell_translation_get (cell, origin);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/************************
 * set Rotation: Angles *
 ************************/

gamgi_math_position_cell_rotation_get (cell, euler);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/***************************************************************
 * set Origin: o1,o2,o3,o4, origin_vectors, axes, axes_vectors *
 ***************************************************************/

if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_conventional (cell->lattice, 
  cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);
else
  { o1 = cell->o1; o2 = cell->o2; o3 = cell->o3; o4 = FALSE; }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o1");
sprintf (string, "%d", o1);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o2");
sprintf (string, "%d", o2);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_o3");
sprintf (string, "%d", o3);
gtk_entry_set_text (GTK_ENTRY (entry), string);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), o4);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell->origin_vectors - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
if (cell->axes == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
if (cell->axes == FALSE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes_vectors");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell->axes_vectors - 1);

/*****************************************************************
 * set View: Faces, Nodes, Borders, Color, Scale (by this order) *
 *****************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_faces");
if (cell->faces == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
if (cell->faces == FALSE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_nodes");
if (cell->nodes == TRUE) 
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1); 
if (cell->nodes == FALSE) 
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), cell->borders - 1);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell->red);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell->green);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, cell->blue);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, cell->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);

static_faces (NULL, window);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_CELL_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_cell_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button;

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

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE) return;

gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_CELL, FALSE, static_press);
}

/*************** external function ******************
 *                                                  *
 *              GAMGI_GTK_CELL_MODIFY               *
 *                                                  *
 *  Creates the dialog window used to create cells. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_cell_modify (GtkWidget *widget, void *data)
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

dialog = gamgi_gtk_dialog_task0_create ("Cell Modify", window);
window->action = GAMGI_GTK_CELL_MODIFY;
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
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Cell");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_cell", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_cell", entry);
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

/*****************
 * Model section *
 *****************/

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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_translation_clean), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_translation_clean), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_relative", button);
gtk_widget_show (button);

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
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
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

label = gtk_label_new ("Rotation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_rotation_clean), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_rotation_clean), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_relative", button);
gtk_widget_show (button);

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
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
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

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (vbox_top_top), table, FALSE, FALSE, 0);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (8);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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
g_object_set_data (G_OBJECT (dialog), "combo_axes", combo);
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

label = gtk_label_new ("Axes");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/**************
 * View page: *
 **************/

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

/**********************************
 * 1st row: Borders, Faces, Nodes *
 **********************************/

hbox_center = gtk_hbox_new (TRUE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

/***********
 * Borders *
 ***********/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_table_attach (GTK_TABLE (table), vbox_top_top, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Borders");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (5);
g_object_set_data (G_OBJECT (dialog), "sensitive_borders", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/*********
 * Faces *
 *********/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_table_attach (GTK_TABLE (table), vbox_top_top, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 10, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_faces", vbox_top_top);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Faces");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_faces), window);
g_object_set_data (G_OBJECT (dialog), "combo_faces", combo);
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

/*********
 * Nodes *
 *********/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_table_attach (GTK_TABLE (table), vbox_top_top, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 10, 0);
g_object_set_data (G_OBJECT (dialog), "vbox_nodes", vbox_top_top);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Nodes");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (vbox_top_top), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_nodes", combo);
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

/******************
 * 2nd row: Color *
 ******************/

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
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
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
