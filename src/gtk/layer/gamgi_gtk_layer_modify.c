/*************************************************
 *
 * $GAMGI/src/gtk/layer/gamgi_gtk_layer_modify.c
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
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

static void static_press (gamgi_object *object, void *data);

static struct {
gamgi_bool local;
gamgi_bool new[21];
char name[GAMGI_ENGINE_TOKEN];
gamgi_enum type;
double top, near, far;
double eye[3], center[3], up[3];
gamgi_enum visibility_in, visibility_out;
gamgi_enum axes;
float red, green, blue;
double scale; } cache;

enum { STATIC_NAME = 0, STATIC_TYPE, STATIC_TOP, STATIC_NEAR,
STATIC_FAR, STATIC_EYE_X, STATIC_EYE_Y, STATIC_EYE_Z,
STATIC_CENTER_X, STATIC_CENTER_Y, STATIC_CENTER_Z, STATIC_UP_X,
STATIC_UP_Y, STATIC_UP_Z, STATIC_VISIBILITY_IN, STATIC_VISIBILITY_OUT,
STATIC_AXES, STATIC_RED, STATIC_GREEN, STATIC_BLUE, STATIC_SCALE };

/******************** internal function *****************
 *                                                      *
 *                      STATIC_RESET                    *
 *                                                      *
 * Reset the layer modify dialog, cleaning all entries. *
 *                                                      *
 ********************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkEntry *entry;

/*****************
 * clean entries *
 *****************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_top");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_near");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_far");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_x");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_y");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_eye_z");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_x");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_y");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_center_z");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_x");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_y");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_up_z");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (entry, "");

/***************
 * reset menus *
 ***************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_projection");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_in");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_out");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 2);
}

/************ internal function *************
 *                                          *
 *                STATIC_LIST               *
 *                                          *
 ********************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_layer");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /*********************************
   * modify a single layer directly *
   *********************************/

  gtk_label_set_text (GTK_LABEL (label), "Layer");
  }
else
  {
  /*********************************************
   * modify multiple layers previously selected *
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

/************************
 * type, top, near, far *
 ************************/

cache.new[STATIC_TYPE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_projection");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2) { cache.new[STATIC_TYPE] = TRUE; cache.type = row + 1; }

cache.new[STATIC_TOP] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_top");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_TOP] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.top, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_NEAR] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_near");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NEAR] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.near, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_FAR] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_far");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_FAR] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.far, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid volume parameter", window);
    return FALSE;
    }
  }

/*******************
 * eye, center, up *
 *******************/

cache.new[STATIC_EYE_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_EYE_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.eye[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_EYE_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_EYE_Y] = TRUE;
  if (gamgi_io_token_double_scan (name, 
  &cache.eye[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_EYE_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_EYE_Z] = TRUE;
  if (gamgi_io_token_double_scan (name, 
  &cache.eye[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_CENTER_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CENTER_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.center[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_CENTER_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CENTER_Y] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.center[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_CENTER_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CENTER_Z] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.center[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_UP_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_UP_X] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.up[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_UP_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_UP_Y] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.up[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

cache.new[STATIC_UP_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_UP_Z] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.up[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid view parameter", window);
    return FALSE;
    }
  }

/********************************
 * visibility_in, visibility_ut *
 ********************************/

cache.new[STATIC_VISIBILITY_IN] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_in");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 3)
  { cache.new[STATIC_VISIBILITY_IN] = TRUE;
  cache.visibility_in = row + 1; }

cache.new[STATIC_VISIBILITY_OUT] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_out");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 3)
  { cache.new[STATIC_VISIBILITY_OUT] = TRUE;
  cache.visibility_out = row + 1; }

/*********************************
 * axes, red, green, blue, scale *
 *********************************/

cache.new[STATIC_AXES] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2)
  {
  cache.new[STATIC_AXES] = TRUE;
  if (row == GAMGI_GTK_YES - 1) cache.axes = TRUE;
  if (row == GAMGI_GTK_NO - 1) cache.axes = FALSE;
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid layer color", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid layer color", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid layer color", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid layer scale", window);
    return FALSE;
    }
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_layer *layer, gamgi_window *window)
{
gamgi_enum type;
gamgi_enum visibility_in, visibility_out, axes;
char name[GAMGI_ENGINE_TOKEN];
double eye[3], center[3], up[3];
double x[3], z[3], up_z[3];
double top, near, far, scale;
double length;
float red, green, blue;

/**************************
 * import data from layer *
 **************************/

strcpy (name, layer->object.name);

if (layer->perspective == TRUE)
  type = GAMGI_MESA_PERSPECTIVE;
else
  type = GAMGI_MESA_ORTHOGRAPHIC;
top = layer->top;
near = layer->near;
far = layer->far;

gamgi_math_vector_copy (layer->eye, eye);
gamgi_math_vector_copy (layer->center, center);
gamgi_math_vector_copy (layer->up, up);

visibility_in = layer->visibility_in;
visibility_out = layer->visibility_out;

axes = layer->axes;
red = layer->color[0];
green = layer->color[1];
blue = layer->color[2];
scale = layer->object.scale;

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_TYPE] == TRUE) type = cache.type;
if (cache.new[STATIC_TOP] == TRUE) top = cache.top;
if (cache.new[STATIC_NEAR] == TRUE) near = cache.near;
if (cache.new[STATIC_FAR] == TRUE) far = cache.far;

if (cache.new[STATIC_EYE_X] == TRUE) eye[0] = cache.eye[0];
if (cache.new[STATIC_EYE_Y] == TRUE) eye[1] = cache.eye[1];
if (cache.new[STATIC_EYE_Z] == TRUE) eye[2] = cache.eye[2];
if (cache.new[STATIC_CENTER_X] == TRUE) center[0] = cache.center[0];
if (cache.new[STATIC_CENTER_Y] == TRUE) center[1] = cache.center[1];
if (cache.new[STATIC_CENTER_Z] == TRUE) center[2] = cache.center[2];
if (cache.new[STATIC_UP_X] == TRUE) up[0] = cache.up[0];
if (cache.new[STATIC_UP_Y] == TRUE) up[1] = cache.up[1];
if (cache.new[STATIC_UP_Z] == TRUE) up[2] = cache.up[2];

if (cache.new[STATIC_VISIBILITY_IN] == TRUE)
  visibility_in = cache.visibility_in;
if (cache.new[STATIC_VISIBILITY_OUT] == TRUE)
  visibility_out = cache.visibility_out;

if (cache.new[STATIC_AXES] == TRUE) axes = cache.axes;
if (cache.new[STATIC_RED] == TRUE) red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

if (far < near + GAMGI_MATH_TOLERANCE_LENGTH)
  {
  /*********************************
   * far must be farther than near *
   *********************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid view parameters", window);
  return FALSE;
  }

gamgi_math_vector_sub (eye, center, z);
length = gamgi_math_vector_length (z);
if (length < near || length > far)
  {
  /*********************************************************
   * the distance eye-center must be in the near-far range *
   *********************************************************/

  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid view parameters", window);
  return FALSE;
  }
gamgi_math_vector_normal (z);

length = gamgi_math_vector_dot (up, z);
gamgi_math_vector_scale (z, up_z, length);
gamgi_math_vector_sub (up, up_z, up);
length = gamgi_math_vector_length (up);
if (length < GAMGI_MATH_TOLERANCE_LENGTH)
  {
  /********************************************************
   * the up component normal to eye-center cannot be zero *
   ********************************************************/
  
  if (cache.local == TRUE) gamgi_gtk_dialog_message_create
  ("Error", "Invalid view parameters", window);
  return FALSE;
  }
gamgi_math_vector_normal (up);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 * *************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/************************
 * export data to layer *
 ************************/

strcpy (layer->object.name, name);

if (type == GAMGI_MESA_PERSPECTIVE)
  layer->perspective = TRUE;
else
  layer->perspective = FALSE;
layer->top = top;
layer->near = near;
layer->far = far;

gamgi_math_vector_copy (eye, layer->eye);
gamgi_math_vector_copy (center, layer->center);
gamgi_math_vector_copy (up, layer->up);

layer->visibility_in = visibility_in;
layer->visibility_out = visibility_out;

layer->axes = axes;
layer->color[0] = red;
layer->color[1] = green;
layer->color[2] = blue;
layer->object.scale = scale;

/************************
 * set referential axes *
 ************************/

gamgi_math_vector_cross (up, z, x);
gamgi_math_matrix_absolute (layer->referential,
x[0], layer->up[0], z[0], x[1], layer->up[1], z[1], x[2], layer->up[2], z[2]);

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

if (gamgi->layers == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of layer objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid layer identity", window);
    return;
    }
  }

if (static_grab (window) == FALSE) return;

/*************************************************
 * For all windows: save layers as undo layers   *
 *                                               *
 * Modify listed objects and show error message  *
 * with objects that could not be modified       *
 *                                               *
 * For all windows: set current layer as new     *
 * current object,layer and redraw gl_area image *
 *************************************************/

gamgi_global_selection_undo ();

start = NULL;
for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_LAYER dlist->data, window) == FALSE)
    {
    start = gamgi_engine_slist_add_start (start);
    start->data = dlist->data;
    }
gamgi_io_error_list (start, window);

gamgi_global_selection_focus ();
gamgi_mesa_draw_gamgi ();

/****************
 * reset dialog *
 ****************/

static_reset (window);
}

static void static_ok_local (gamgi_window *window)
{
gamgi_layer *layer;
GtkWidget *dialog = window->dialog0;

cache.local = TRUE;

/*******************************************
 * Pick object and get name (possibly new) *
 *******************************************/

cache.new[STATIC_NAME] = TRUE;
layer = GAMGI_CAST_LAYER gamgi_gtk_object_number (dialog,
"entry_layer", GAMGI_ENGINE_LAYER, window, cache.name);
if (layer == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid layer identity", window);
  return;
  }

/*********************************************
 * collect data from dialog and modify layer *
 *********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (layer, window) == FALSE) return;

/**********************************************************************
 * current object,layer are affected, so change them to current layer *
 **********************************************************************/

gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);
gamgi_gtk_object_layer_local (window->layer, window);

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
static_press (GAMGI_CAST_OBJECT window->layer, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_layer *layer;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];

layer = GAMGI_CAST_LAYER object;

static_reset (window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_layer");
sprintf (string, "%s %d", layer->object.name, layer->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/******************************
 * get projection option menu *
 ******************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_projection");
if (layer->perspective == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_PERSPECTIVE - 1);
if (layer->perspective == FALSE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_ORTHOGRAPHIC - 1);

/************************************
 * get projection volume parameters *
 ************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_top");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->top);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_near");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->near);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_far");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->far);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/***************************
 * get position parameters *
 ***************************/

/*******
 * eye *
 *******/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_x");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->eye[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_y");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->eye[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_eye_z");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->eye[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**********
 * center *
 **********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_x");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->center[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_y");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->center[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_center_z");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->center[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/******
 * up *
 ******/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_x");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->up[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_y");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->up[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_up_z");
sprintf (string, "%.*f", gamgi->gamgi->length, layer->up[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**************************
 * set visibility in, out *
 **************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_in");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), layer->visibility_in - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_out");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), layer->visibility_out - 1);

/************************
 * set background color *
 ************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, layer->color[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, layer->color[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, layer->color[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/************************
 * set axes option menu *
 ************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
if (layer->axes == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
if (layer->axes == FALSE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

/*************
 * set scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, layer->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_LAYER_MODIFY_PRESS       *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_layer_modify_press (gamgi_window *window_mouse,
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
GAMGI_ENGINE_LAYER, FALSE, static_press);
}

/***************** external function ****************
 *                                                  *
 *               GAMGI_GTK_LAYER_MODIFY             *
 *                                                  *
 * Creates the dialog window used to modify layers. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_layer_modify (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Layer Modify", window);
window->action = GAMGI_GTK_LAYER_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Layer objects *
 *****************/

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

label = gtk_label_new ("Layer");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_layer", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_layer", entry);
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

/*******************
 * Projection page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Projection");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * projection *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Type");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_projection", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Perspective", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Orthographic", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * volume *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("Top");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Near");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Far");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_top", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_near", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_far", entry);
gtk_widget_show (entry);

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

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

/*******************
 * Eye, Center, Up *
 *******************/

table = gtk_table_new (3, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (table), 5);
gtk_widget_show (table);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Eye");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Center");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Up");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_eye_z", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_center_z", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_up_z", entry);
gtk_widget_show (entry);
  
/*******************
 * Visibility page *
 *******************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Visibility");
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

/**********
 * inside *
 **********/

label = gtk_label_new ("Inside");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 15, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
g_object_set_data (G_OBJECT (dialog), "combo_in", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Partial", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/***********
 * outside *
 ***********/

label = gtk_label_new ("Outside");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 15, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_table_attach (GTK_TABLE (table), combo, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 15, 5);
g_object_set_data (G_OBJECT (dialog), "combo_out", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "All", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Partial", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***********
 * 1st row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

/*********
 * color *
 *********/

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

/***********
 * 2nd row *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/********
 * Axes *
 ********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Axes");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 3);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
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

/*********
 * Scale *
 *********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
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
