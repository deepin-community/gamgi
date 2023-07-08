/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_measure.c
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
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_start.h"
#include "gamgi_chem_geometry.h"
#include "gamgi_chem_symmetry.h"
#include "gamgi_phys_reciprocal.h"
#include "gamgi_phys_voronoi.h"
#include "gamgi_phys_cell.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_io_token.h"

static void static_reciprocal_bravais (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_entry_constant");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_PHYS_RECIPROCAL_DEFAULT);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_reciprocal_start (gamgi_layer *layer_old, 
gamgi_layer *layer)
{
/*************************
 * give non-default name *
 *************************/

strcpy (layer->object.name, "Analysis");

/****************************************
 * copy projection,modelview parameters *
 ****************************************/

layer->perspective = layer_old->perspective;
layer->top = layer_old->top;
layer->near = layer_old->near;
layer->far = layer_old->far;

gamgi_math_vector_copy (layer_old->eye, layer->eye);
gamgi_math_vector_copy (layer_old->center, layer->center);
gamgi_math_vector_copy (layer_old->up, layer->up);

gamgi_math_matrix_copy (layer_old->referential, layer->referential);
 
/************************
 * copy view parameters *
 ************************/

layer->visibility_in = layer_old->visibility_in;
layer->visibility_out = layer_old->visibility_out;

layer->axes = layer_old->axes;

layer->color[0] = layer_old->color[0];
layer->color[1] = layer_old->color[1];
layer->color[2] = layer_old->color[2];

layer->object.scale = layer_old->object.scale;
}

static gamgi_layer *static_reciprocal_create (gamgi_layer *layer_old, 
gamgi_window *window)
{
gamgi_layer *layer;

/********************
 * create new layer *
 ********************/

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
static_reciprocal_start (layer_old, layer);

/**********************************
 * start undo and save mechanisms *
 **********************************/

gamgi_gtk_history_start (layer);

return layer;
}

static void static_voronoi_color (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label_red, *label_green, *label_blue;
char token[GAMGI_ENGINE_TOKEN];

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_blue");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_label_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_label_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_label_blue");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_CLUSTER_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_CLUSTER_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_CLUSTER_B);
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

static void static_voronoi_faces (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *button;
gamgi_bool *sensitive;
int row;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_color");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /**********************************************************************
   * in solid mode, color can be automatic (default) or set by the user *
   **********************************************************************/

  gtk_widget_set_sensitive (button, TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  }
else
  {
  /************************************************
   * in wired mode, color must be set by the user *
   ************************************************/
  
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  gtk_widget_set_sensitive (button, FALSE);
  }

/*************************
 * get borders combo box *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_combo_borders");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "voronoi_sensitive_borders");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*********************************************
   * in solid mode, disable ALL borders option *
   *********************************************/

  if (row == GAMGI_PHYS_ALL - 1)
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_PHYS_FACES - 1);
  sensitive[GAMGI_PHYS_ALL - 1] = FALSE;
  }
else
  {
  sensitive[GAMGI_PHYS_ALL - 1] = TRUE;
  }

static_voronoi_color (button, window);
}

static void static_voronoi_layer (gamgi_cluster *cluster, gamgi_window *window)
{
gamgi_layer *layer;

/********************
 * create new layer *
 ********************/

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, gamgi->layer);
layer->visibility_out = GAMGI_GTK_NONE;
strcpy (layer->object.name, "Analysis");

/**********************************
 * start undo and save mechanisms *
 **********************************/

gamgi_gtk_history_start (layer);

/****************************************
 * add Voronoi tesselation to new layer *
 ****************************************/

gamgi_engine_link_cluster_layer (cluster, layer);

/****************************************************
 * set up layer as current layer and current object *
 ****************************************************/

gamgi_engine_link_layer_window (layer, window);
gamgi_gtk_object_layer_local (layer, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer, window);

/******************************************
 * redraw image: show Voronoi tesselation *
 ******************************************/

gtk_widget_queue_draw (window->area);
}

void gamgi_gtk_measure_geometry_ok (gamgi_object *object, gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *text;
gamgi_enum action;

/**************************************
 * create and initialize text widget, *
 * to show the Geometry report        *
 **************************************/

text = gamgi_gtk_dialog_text_create ();

/***************************
 * apply voronoi algorithm *
 ***************************/

if (gamgi_chem_geometry (object, text, window->layer) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid object data", window);
  gtk_widget_destroy (text);
  return;
  }

/*************************************************************
 * create and initialize text dialog showing Geometry report *
 *************************************************************/

action = FALSE;
if (window->action == GAMGI_GTK_GROUP_MEASURE)
  action = GAMGI_GTK_GROUP_MEASURE_GEOMETRY;
if (window->action == GAMGI_GTK_MOLECULE_MEASURE)
  action = GAMGI_GTK_MOLECULE_MEASURE_GEOMETRY;
if (window->action == GAMGI_GTK_CLUSTER_MEASURE)
  action = GAMGI_GTK_CLUSTER_MEASURE_GEOMETRY;
if (window->action == GAMGI_GTK_CELL_MEASURE)
  action = GAMGI_GTK_CELL_MEASURE_GEOMETRY;

dialog = gamgi_gtk_dialog_report_create (text, 
"Measure Geometry", action, window);

gtk_widget_show (dialog);
}

void gamgi_gtk_measure_symmetry_ok (gamgi_object *object, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *text;
GtkWidget *entry;
GtkWidget *button;
const char *name;
float initial, final;
gamgi_bool periodic;
gamgi_enum action;

/***********************************************
 * chemical or crystallographic point symmetry *
 ***********************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_button_yes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  periodic = TRUE;
else
  periodic = FALSE;

/********************************
 * Get initial,final tolerances *
 ********************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_entry_initial");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &initial, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid tolerance data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_entry_final");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &final, 0.0, initial) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid tolerance data", window);
  return;
  }

/**************************************
 * create and initialize text widget, * 
 * to show the point symmetry report  *
 **************************************/

text = gamgi_gtk_dialog_text_create ();

/**********************************
 * determine symmetry information *
 **********************************/

if (gamgi_chem_symmetry (object, 
periodic, initial, final, text) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid object data", window);
  gtk_widget_destroy (text);
  return;
  }

/*************************************************************
 * create and initialize text dialog showing Symmetry report *
 *************************************************************/

action = FALSE;
if (window->action == GAMGI_GTK_GROUP_MEASURE)
  action = GAMGI_GTK_GROUP_MEASURE_SYMMETRY;
if (window->action == GAMGI_GTK_MOLECULE_MEASURE)
  action = GAMGI_GTK_MOLECULE_MEASURE_SYMMETRY;
if (window->action == GAMGI_GTK_CLUSTER_MEASURE)
  action = GAMGI_GTK_CLUSTER_MEASURE_SYMMETRY;

dialog = gamgi_gtk_dialog_report_create (text, 
"Measure Symmetry", action, window);

gtk_widget_show (dialog);
}

void gamgi_gtk_measure_symmetry_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/*****************************
 * no periodicity by default *
 *****************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_button_no");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

/*****************************************
 * initialise lower,upper length factors *
 *****************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_entry_initial");
sprintf (token, "%f", GAMGI_CHEM_SYMMETRY_TOLERANCE_INITIAL);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "symmetry_entry_final");
sprintf (token, "%f", GAMGI_CHEM_SYMMETRY_TOLERANCE_FINAL);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

void gamgi_gtk_measure_symmetry (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top;

/*****************
 * Symmetry page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Symmetry");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "symmetry_vbox_page", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/************
 * Symmetry *
 ************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/***********************************************
 * chemical or crystallographic point symmetry *
 ***********************************************/

label = gtk_label_new ("Periodicity");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Yes");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "symmetry_button_yes", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "No");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "symmetry_button_no", button);
gtk_widget_show (button);

/*************
 * tolerance *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Tolerance");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "symmetry_entry_initial", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Initial");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "symmetry_entry_final", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Final");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);
}

void gamgi_gtk_measure_reciprocal_ok (gamgi_object *object, gamgi_window *window)
{
gamgi_layer *layer_new;
GtkWidget *dialog = window->dialog0;
gamgi_cell *cell, *cell_new;
GtkWidget *entry;
GtkWidget *button;
const char *name;
double factor;
gamgi_bool primitive, planes;

cell = GAMGI_CAST_CELL object;

/************************************
 * Add primitive lattice and planes *
 ************************************/

primitive = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_lattice");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /************************************************
   * adding the reciprocal of the primitive       *
   * lattice is useful only for centered lattices *
   ************************************************/

  if (cell->model != GAMGI_PHYS_CONVENTIONAL ||
  gamgi_phys_cell_primitive (cell->lattice) == cell->lattice)
    {
    gamgi_gtk_dialog_message_create ("Error", 
    "Redundant primitive lattice for non centered lattice", window);
    return;
    }
  primitive = TRUE;
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_planes");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  planes = TRUE;
else
  planes = FALSE;

/******************************************
 * Get reciprocal lattice constant factor *
 ******************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_bravais");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_entry_constant");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /*****************************************************
   * determine reciprocal lattice constant according   *
   * to Bravais polar lattice method, so direct and    *
   * reciprocal lattices have the same volume per node *
   *****************************************************/

  factor = gamgi_math_vector_mix (cell->p1, cell->p2, cell->p3);
  factor = pow (factor, 2.0/3.0);
  }
else if (gamgi_io_token_double_scan (name, &factor,
GAMGI_PHYS_RECIPROCAL_MIN, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Factor Data", window);
  return;
  }

/*******************************************************
 * Create new layer to contain the reciprocal lattice. *
 *                                                     *
 * Create new cell to contain the reciprocal lattice.  *
 * For centered lattices, the corresponding primitive  *
 * lattice can also be transformed in its reciprocal   *
 * lattice, thus requiring a second cell. This option  *
 * should be available only for conventional cells.    *
 *                                                     *
 * The real lattice uses a subset of the lines used by *
 * the primitive lattice, so it should be added in the *
 * end, to be rendered after the primitive lattice, to *
 * overwrite the primitive, so both cells are visible. *
 *                                                     *
 * If required, create plane objects for all nodes     *
 * (except 000) of the new reciprocal lattice volume.  *
 *******************************************************/

layer_new = static_reciprocal_create (window->layer, window);

if (primitive == TRUE)
  {
  cell_new = gamgi_phys_reciprocal_primitive (cell, factor, window);
  gamgi_engine_link_cell_layer (cell_new, layer_new);
  }

cell_new = gamgi_phys_reciprocal (cell, factor, planes, window);
gamgi_engine_link_cell_layer (cell_new, layer_new);

/****************************************************
 * set up layer as current layer and current object *
 ****************************************************/

gamgi_engine_link_layer_window (layer_new, window);
gamgi_gtk_object_layer_local (layer_new, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer_new, window);

gamgi_gtk_dialog_task0_remove (NULL, window);

/*****************************************
 * redraw image: show reciprocal lattice *
 *****************************************/

gtk_widget_queue_draw (window->area);
}

void gamgi_gtk_measure_reciprocal_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *button;

/****************
 * set notebook *
 ****************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 0);

/*******************
 * initialize view *
 *******************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_lattice");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_planes");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "reciprocal_button_bravais");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  if (GAMGI_PHYS_RECIPROCAL_BRAVAIS == FALSE)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  }
else
  {
  if (GAMGI_PHYS_RECIPROCAL_BRAVAIS == TRUE)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  }

static_reciprocal_bravais (button, window);
}

void gamgi_gtk_measure_reciprocal (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top;

/*******************
 * Reciprocal page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Reciprocal");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "reciprocal_vbox_page", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/******************************************
 * primitive lattice; conventional planes *
 ******************************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Primitive");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_toggle_button_new_with_label ("Lattice");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "reciprocal_button_lattice", button);
gtk_widget_show (button);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Conventional");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_toggle_button_new_with_label ("Planes");
gtk_box_pack_start (GTK_BOX (hbox_left_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "reciprocal_button_planes", button);
gtk_widget_show (button);

/******************************************************
 * reciprocal lattice constant, Bravais polar lattice *
 ******************************************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Constant");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "reciprocal_entry_constant", entry);
gtk_widget_show (entry);

button = gtk_toggle_button_new_with_label ("Bravais");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_reciprocal_bravais), window);
g_object_set_data (G_OBJECT (dialog), "reciprocal_button_bravais", button);
gtk_widget_show (button);
}

void gamgi_gtk_measure_voronoi_ok (gamgi_object *object, gamgi_window *window)
{
gamgi_cluster *cluster;
GtkWidget *dialog = window->dialog0;
GtkWidget *text;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
gamgi_bool periodic;
gamgi_enum borders;
double offset, variancy;
double apex, area, vertex;
float rgb[3];
float *color;
gamgi_enum action;

/**************************************
 * boundary section: periodic, offset *
 **************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_periodic");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  periodic = TRUE;
else
  periodic = FALSE;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_offset");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &offset, 0.0, DBL_MAX) == FALSE
|| (periodic == FALSE && offset < GAMGI_MATH_TOLERANCE_LENGTH))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Boundary Data", window);
  return;
  }

/***********************************************
 *           polyhedra weight variancy         *
 * 0.0: normal tesselation                     *
 * 1.0: radical tesselation with atomic radius *
 ***********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_variancy");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &variancy, 
0.0, GAMGI_PHYS_VORONOI_VARIANCY) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Variancy Data", window);
  return;
  }

/*****************************************
 * tolerance section: apex, area, vertex *
 *****************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_apex");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &apex, 
0.0, GAMGI_PHYS_VORONOI_TOLERANCE) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Tolerance Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_area");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &area, 
0.0, GAMGI_PHYS_VORONOI_TOLERANCE) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Tolerance Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_vertex");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &vertex, 
0.0, GAMGI_PHYS_VORONOI_TOLERANCE) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Tolerance Data", window);
  return;
  }

/***************************************
 * view section: color, red,green,blue *
 ***************************************/

color = NULL;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_color");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  color = rgb;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (color != NULL &&
gamgi_io_token_float_scan (name, &color[0], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (color != NULL &&
gamgi_io_token_float_scan (name, &color[1], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (color != NULL &&
gamgi_io_token_float_scan (name, &color[2], 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  return;
  }

/*****************************************************
 * create cluster to contain the Voronoi tesselation *
 *****************************************************/

cluster = gamgi_engine_create_cluster ();
gamgi_engine_start_cluster (cluster);
strcpy (cluster->object.name, "Voronoi");

/**************************************
 * get faces and atoms toggle buttons *
 **************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_faces");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cluster->faces = TRUE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_atoms");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cluster->atoms = TRUE;

/********************************
 * get borders combo box option *
 ********************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_combo_borders");
borders = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

/******************************************
 * create and initialize text widget,     *
 * to show the Voronoi tesselation report *
 ******************************************/

text = gamgi_gtk_dialog_text_create ();

/***************************
 * apply voronoi algorithm *
 ***************************/

if (gamgi_phys_voronoi (object, cluster, periodic, 
offset, variancy, apex, area, vertex, color, borders, text) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid object data", window);
  gamgi_engine_remove_cluster (cluster);
  gtk_widget_destroy (text);
  return;
  }

/************************************************************
 * create and initialize layer showing Voronoi tesselation  *
 *                                                          *
 * create and initialize text dialog showing Voronoi report *
 ************************************************************/

static_voronoi_layer (cluster, window);

action = FALSE;
if (window->action == GAMGI_GTK_GROUP_MEASURE)
  action = GAMGI_GTK_GROUP_MEASURE_VORONOI;
if (window->action == GAMGI_GTK_MOLECULE_MEASURE)
  action = GAMGI_GTK_MOLECULE_MEASURE_VORONOI;
if (window->action == GAMGI_GTK_CLUSTER_MEASURE)
  action = GAMGI_GTK_CLUSTER_MEASURE_VORONOI;
if (window->action == GAMGI_GTK_CELL_MEASURE)
  action = GAMGI_GTK_CELL_MEASURE_VORONOI;

dialog = gamgi_gtk_dialog_report_create (text, 
"Measure Voronoi", action, window);

gtk_widget_show (dialog);
}

void gamgi_gtk_measure_voronoi_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget* entry;
GtkWidget* button;
GtkWidget* combo;
char token[GAMGI_ENGINE_TOKEN];

/*************************
 * initialize boundaries *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_offset");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_LENGTH, GAMGI_PHYS_VORONOI_BORDER);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/****************************************
 * initialize partition weight variancy *
 * for normal Voronoi tesselation: 0.0  *
 ****************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_variancy");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_LENGTH, 0.0);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/************************
 * initialize tolerance *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_apex");
sprintf (token, "%.*E", 2, GAMGI_PHYS_VORONOI_APEX);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_area");
sprintf (token, "%.*E", 2, GAMGI_PHYS_VORONOI_AREA);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_entry_vertex");
sprintf (token, "%.*E", 2, GAMGI_PHYS_VORONOI_VERTEX);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************
 * initialize view *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_combo_borders");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_atoms");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "voronoi_button_faces");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

static_voronoi_faces (button, window);
}

void gamgi_gtk_measure_voronoi (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *table;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

/****************
 * Voronoi page *
 ****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Voronoi");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "voronoi_vbox_page1", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * boundaries *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Boundaries");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Planar");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Periodic");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_button_periodic", button);
gtk_widget_show (button);

/***********
 * 2nd row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********
 * offset *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Offset");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_offset", entry);
gtk_widget_show (entry);

/************
 * variancy *
 ************/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Variancy");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_variancy", entry);
gtk_widget_show (entry);

/******************
 * Tolerance page *
 ******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Tolerance");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "voronoi_vbox_page2", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*************
 * tolerance *
 *************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Apex");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Area");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Vertex");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_EXPAND, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_apex", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_EXPAND, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_area", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_EXPAND, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_vertex", entry);
gtk_widget_show (entry);

/*************
 * View page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "voronoi_vbox_page3", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***********************
 * Seeds,Faces,Borders *
 ***********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/***********
 * borders *
 ***********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Borders");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_combo_borders", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Faces", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "voronoi_sensitive_borders", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/*********
 * faces *
 *********/

button = gtk_toggle_button_new_with_label ("Faces");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_voronoi_faces), window);
g_object_set_data (G_OBJECT (dialog), "voronoi_button_faces", button);
gtk_widget_show (button);

/*********
 * atoms *
 *********/

button = gtk_toggle_button_new_with_label ("Atoms");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_button_atoms", button);
gtk_widget_show (button);

/*********************
 * second row: color *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label (" Color ");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_voronoi_color), window);
g_object_set_data (G_OBJECT (dialog), "voronoi_button_color", button);
gtk_widget_show (button);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_label_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_label_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "voronoi_label_blue", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "voronoi_entry_blue", entry);
gtk_widget_show (entry);
}
