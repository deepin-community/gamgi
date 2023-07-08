/*************************************************
 *
 * $GAMGI/src/gtk/light/gamgi_gtk_light_modify.c
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
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

struct {
gamgi_bool local;
gamgi_bool new[22];
char name[GAMGI_ENGINE_TOKEN];
gamgi_enum type;
float direction[3];
float position[3];
float angle;
gamgi_bool absolute;
float ambient[3], specular[3], diffuse[3];
float constant, linear, quadratic, radial; } cache;

enum { STATIC_NAME = 0, STATIC_TYPE,
STATIC_DIRECTION_X, STATIC_DIRECTION_Y, STATIC_DIRECTION_Z, 
STATIC_POSITION_X, STATIC_POSITION_Y, STATIC_POSITION_Z, 
STATIC_ANGLE, STATIC_AMBIENT_R, STATIC_AMBIENT_G, STATIC_AMBIENT_B,
STATIC_DIFFUSE_R, STATIC_DIFFUSE_G, STATIC_DIFFUSE_B,
STATIC_SPECULAR_R, STATIC_SPECULAR_G, STATIC_SPECULAR_B,
STATIC_CONSTANT, STATIC_LINEAR, STATIC_QUADRATIC, STATIC_RADIAL };

/********************** internal function ********************
 *                                                           *
 *                       STATIC_ABSOLUTE                     *
 *                                                           *
 * Set ambient, diffuse, specular reading modes to absolute. *
 *************************************************************/

static void static_absolute (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_absolute");
gtk_button_clicked (GTK_BUTTON (button));
}

/*********************** internal function *******************
 *                                                           *
 *                        STATIC_RELATIVE                    *
 *                                                           *
 * Set ambient, diffuse, specular reading modes to relative. *
 *************************************************************/

static void static_relative (GtkWidget *dialog)
{
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_relative");
gtk_button_clicked (GTK_BUTTON (button));
}

/****************** internal function *****************
 *                                                    *
 *                    STATIC_COLOR                    *
 *                                                    *
 * Reset ambient, diffuse, specular entries everytime *
 * the absolute or relative buttons are toggled.      *
 ******************************************************/

static void static_color (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_r");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_g");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_b");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_r");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_g");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_b");
gtk_entry_set_text (entry, "");

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_r");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_g");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_b");
gtk_entry_set_text (entry, "");
}

static void static_clamp (float *component, float light_component)
{
*component += light_component;
if (*component > 1.0) *component = 1.0;
if (*component < 0.0) *component = 0.0;
}

static void static_light (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *label_position, *label_direction, *label_angle;
GtkWidget *entry_position_x, *entry_position_y, *entry_position_z;
GtkWidget *entry_direction_x, *entry_direction_y, *entry_direction_z;
GtkWidget *entry_angle;
GtkWidget *label_axial, *label_radial;
GtkWidget *label_constant, *label_linear, *label_quadratic;
GtkWidget *entry_constant, *entry_linear, *entry_quadratic;
GtkWidget *entry_radial;
GtkWidget *combo;
int row;

label_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_position");
entry_position_x = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
entry_position_y = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
entry_position_z = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");

label_direction = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_direction");
entry_direction_x = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
entry_direction_y = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
entry_direction_z = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");

label_angle = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_angle");
entry_angle = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");

label_axial = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_axial");
label_constant = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_constant");
label_linear = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_linear");
label_quadratic = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_quadratic");
entry_constant = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_constant");
entry_linear = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_linear");
entry_quadratic = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_quadratic");

label_radial = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_radial");
entry_radial = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radial");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*********************
 * directional light *
 *********************/

if (row == GAMGI_MESA_DIRECTIONAL - 1)
  {
  gtk_widget_set_sensitive (label_direction, TRUE);
  gtk_widget_set_sensitive (entry_direction_x, TRUE);
  gtk_widget_set_sensitive (entry_direction_y, TRUE);
  gtk_widget_set_sensitive (entry_direction_z, TRUE);

  gtk_widget_set_sensitive (label_position, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_position_x), "");
  gtk_entry_set_text (GTK_ENTRY (entry_position_y), "");
  gtk_entry_set_text (GTK_ENTRY (entry_position_z), "");
  gtk_widget_set_sensitive (entry_position_x, FALSE);
  gtk_widget_set_sensitive (entry_position_y, FALSE);
  gtk_widget_set_sensitive (entry_position_z, FALSE);

  gtk_widget_set_sensitive (label_angle, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_angle), "");
  gtk_widget_set_sensitive (entry_angle, FALSE);

  gtk_widget_set_sensitive (label_axial, FALSE);
  gtk_widget_set_sensitive (label_constant, FALSE);
  gtk_widget_set_sensitive (label_linear, FALSE);
  gtk_widget_set_sensitive (label_quadratic, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_constant), "");
  gtk_entry_set_text (GTK_ENTRY (entry_linear), "");
  gtk_entry_set_text (GTK_ENTRY (entry_quadratic), "");
  gtk_widget_set_sensitive (entry_constant, FALSE);
  gtk_widget_set_sensitive (entry_linear, FALSE);
  gtk_widget_set_sensitive (entry_quadratic, FALSE);

  gtk_widget_set_sensitive (label_radial, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_radial), "");
  gtk_widget_set_sensitive (entry_radial, FALSE);
  }

/********************
 * positional light *
 ********************/

else if (row == GAMGI_MESA_POSITIONAL - 1)
  {
  gtk_widget_set_sensitive (label_direction, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_x), "");
  gtk_entry_set_text (GTK_ENTRY (entry_direction_y), "");
  gtk_entry_set_text (GTK_ENTRY (entry_direction_z), "");
  gtk_widget_set_sensitive (entry_direction_x, FALSE);
  gtk_widget_set_sensitive (entry_direction_y, FALSE);
  gtk_widget_set_sensitive (entry_direction_z, FALSE);

  gtk_widget_set_sensitive (label_position, TRUE);
  gtk_widget_set_sensitive (entry_position_x, TRUE);
  gtk_widget_set_sensitive (entry_position_y, TRUE);
  gtk_widget_set_sensitive (entry_position_z, TRUE);

  gtk_widget_set_sensitive (label_angle, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_angle), "");
  gtk_widget_set_sensitive (entry_angle, FALSE);

  gtk_widget_set_sensitive (label_axial, TRUE);
  gtk_widget_set_sensitive (label_constant, TRUE);
  gtk_widget_set_sensitive (label_linear, TRUE);
  gtk_widget_set_sensitive (label_quadratic, TRUE);
  gtk_widget_set_sensitive (entry_constant, TRUE);
  gtk_widget_set_sensitive (entry_linear, TRUE);
  gtk_widget_set_sensitive (entry_quadratic, TRUE);

  gtk_widget_set_sensitive (label_radial, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_radial), "");
  gtk_widget_set_sensitive (entry_radial, FALSE);
  }

/**************
 * spot light *
 **************/

else if (row == GAMGI_MESA_SPOT - 1)
  {
  gtk_widget_set_sensitive (label_direction, TRUE);
  gtk_widget_set_sensitive (entry_direction_x, TRUE);
  gtk_widget_set_sensitive (entry_direction_y, TRUE);
  gtk_widget_set_sensitive (entry_direction_z, TRUE);

  gtk_widget_set_sensitive (label_position, TRUE);
  gtk_widget_set_sensitive (entry_position_x, TRUE);
  gtk_widget_set_sensitive (entry_position_y, TRUE);
  gtk_widget_set_sensitive (entry_position_z, TRUE);

  gtk_widget_set_sensitive (label_angle, TRUE);
  gtk_widget_set_sensitive (entry_angle, TRUE);

  gtk_widget_set_sensitive (label_axial, TRUE);
  gtk_widget_set_sensitive (label_constant, TRUE);
  gtk_widget_set_sensitive (label_linear, TRUE);
  gtk_widget_set_sensitive (label_quadratic, TRUE);
  gtk_widget_set_sensitive (entry_constant, TRUE);
  gtk_widget_set_sensitive (entry_linear, TRUE);
  gtk_widget_set_sensitive (entry_quadratic, TRUE);

  gtk_widget_set_sensitive (label_radial, TRUE);
  gtk_widget_set_sensitive (entry_radial, TRUE);
  }

/***************
 * local light *
 ***************/

else
  {
  gtk_widget_set_sensitive (label_direction, TRUE);
  gtk_widget_set_sensitive (entry_direction_x, TRUE);
  gtk_widget_set_sensitive (entry_direction_y, TRUE);
  gtk_widget_set_sensitive (entry_direction_z, TRUE);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_x), "");
  gtk_entry_set_text (GTK_ENTRY (entry_direction_y), "");
  gtk_entry_set_text (GTK_ENTRY (entry_direction_z), "");

  gtk_widget_set_sensitive (label_position, TRUE);
  gtk_widget_set_sensitive (entry_position_x, TRUE);
  gtk_widget_set_sensitive (entry_position_y, TRUE);
  gtk_widget_set_sensitive (entry_position_z, TRUE);
  gtk_entry_set_text (GTK_ENTRY (entry_position_x), "");
  gtk_entry_set_text (GTK_ENTRY (entry_position_y), "");
  gtk_entry_set_text (GTK_ENTRY (entry_position_z), "");

  gtk_widget_set_sensitive (label_angle, TRUE);
  gtk_widget_set_sensitive (entry_angle, TRUE);
  gtk_entry_set_text (GTK_ENTRY (entry_angle), "");

  gtk_widget_set_sensitive (label_axial, TRUE);
  gtk_widget_set_sensitive (label_constant, TRUE);
  gtk_widget_set_sensitive (label_linear, TRUE);
  gtk_widget_set_sensitive (label_quadratic, TRUE);
  gtk_widget_set_sensitive (entry_constant, TRUE);
  gtk_widget_set_sensitive (entry_linear, TRUE);
  gtk_widget_set_sensitive (entry_quadratic, TRUE);
  gtk_entry_set_text (GTK_ENTRY (entry_constant), "");
  gtk_entry_set_text (GTK_ENTRY (entry_linear), "");
  gtk_entry_set_text (GTK_ENTRY (entry_quadratic), "");

  gtk_widget_set_sensitive (label_radial, TRUE);
  gtk_widget_set_sensitive (entry_radial, TRUE);
  gtk_entry_set_text (GTK_ENTRY (entry_radial), "");
  }
}

static void static_default (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
int row;

/***********************
 * update light status *
 ***********************/

static_light (window);

/******************
 * get light menu *
 ******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/***********************
 * set widget defaults *
 ***********************/

if (row == GAMGI_MESA_DIRECTIONAL - 1 || row == GAMGI_MESA_SPOT - 1)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_X);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

if (row == GAMGI_MESA_POSITIONAL - 1 || row == GAMGI_MESA_SPOT - 1)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_X);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_constant");
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  (float) GAMGI_MESA_LIGHT_CONSTANT);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_linear");
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  (float) GAMGI_MESA_LIGHT_LINEAR);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_quadratic");
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  (float) GAMGI_MESA_LIGHT_QUADRATIC);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

if (row == GAMGI_MESA_SPOT - 1)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
  sprintf (token, "%.*f", gamgi->gamgi->angle, 
  (float) GAMGI_MESA_LIGHT_ANGLE);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radial");
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  (float) GAMGI_MESA_LIGHT_RADIAL);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

}

/********************* internal function ****************
 *                                                      *
 *                        STATIC_RESET                  *
 *                                                      *
 * Reset the light modify dialog, cleaning all entries. *
 *                                                      *
 ********************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;
GtkWidget *combo;

/***************
 * reset light *
 ***************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_light");
gtk_entry_set_text (entry, "");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);

/****************
 * reset colors *
 ****************/

static_color (NULL, window);

/*******************************
 * reset geometry, attenuation *
 *******************************/

static_default (NULL, window);
}

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_light");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /**********************************
   * modify a single light directly *
   **********************************/

  gtk_label_set_text (GTK_LABEL (label), "Light");
  static_absolute (dialog);
  }
else
  {
  /**********************************************
   * modify multiple lights previously selected *
   **********************************************/

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

/************************************
 * type, direction, position, angle *
 ************************************/

cache.new[STATIC_TYPE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 3) { cache.new[STATIC_TYPE] = TRUE; cache.type = 1 + row; }

cache.new[STATIC_DIRECTION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIRECTION_X] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.direction[0], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light direction", window);
    return FALSE;
    }
  }

cache.new[STATIC_DIRECTION_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIRECTION_Y] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.direction[1], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light direction", window);
    return FALSE;
    }
  }

cache.new[STATIC_DIRECTION_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIRECTION_Z] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.direction[2], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light direction", window);
    return FALSE;
    }
  }

cache.new[STATIC_POSITION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_POSITION_X] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.position[0], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light position", window);
    return FALSE;
    }
  }

cache.new[STATIC_POSITION_Y] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_POSITION_Y] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.position[1], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light position", window);
    return FALSE;
    }
  }

cache.new[STATIC_POSITION_Z] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_POSITION_Z] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.position[2], -FLT_MAX, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light position", window);
    return FALSE;
    }
  }

cache.new[STATIC_ANGLE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ANGLE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.angle, 0.0, 90.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light angle", window);
    return FALSE;
    }
  }

/*******************************************
 * color: type, ambient, diffuse, specular *
 *******************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_absolute");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.absolute = TRUE;
else
  cache.absolute = FALSE;

cache.new[STATIC_AMBIENT_R] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_AMBIENT_R] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.ambient[0], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_AMBIENT_G] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_AMBIENT_G] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.ambient[1], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_AMBIENT_B] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_AMBIENT_B] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.ambient[2], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_DIFFUSE_R] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIFFUSE_R] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.diffuse[0], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_DIFFUSE_G] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIFFUSE_G] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.diffuse[1], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_DIFFUSE_B] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DIFFUSE_B] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.diffuse[2], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_SPECULAR_R] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_r");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SPECULAR_R] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.specular[0], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_SPECULAR_G] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_g");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SPECULAR_G] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.specular[1], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

cache.new[STATIC_SPECULAR_B] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_b");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SPECULAR_B] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.specular[2], -1.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
    return FALSE;
    }
  }

if (cache.absolute == TRUE && (cache.ambient[0] < 0.0 ||
cache.ambient[1] < 0.0 || cache.ambient[2] < 0.0 || cache.diffuse[0] < 0.0 ||
cache.diffuse[1] < 0.0 || cache.diffuse[2] < 0.0 || cache.specular[0] < 0.0 ||
cache.specular[1] < 0.0 || cache.specular[2] < 0.0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
  return FALSE;
  }

/***************************************
 * constant, linear, quadratic, radial *
 ***************************************/

cache.new[STATIC_CONSTANT] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_constant");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CONSTANT] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.constant, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light attenuation", window);
    return FALSE;
    }
  }

cache.new[STATIC_LINEAR] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_linear");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_LINEAR] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.linear, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light attenuation", window);
    return FALSE;
    }
  }

cache.new[STATIC_QUADRATIC] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_quadratic");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_QUADRATIC] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.quadratic, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light attenuation", window);
    return FALSE;
    }
  }

cache.new[STATIC_RADIAL] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radial");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RADIAL] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.radial, 0.0, FLT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light attenuation", window);
    return FALSE;
    }
  }

return TRUE;
}

static gamgi_bool static_apply (gamgi_light *light, gamgi_window *window)
{
char name[GAMGI_ENGINE_TOKEN];
float direction[3], position[3];
float ambient[3], diffuse[3], specular[3];
float angle, constant, linear, quadratic, radial;
gamgi_bool direction_new, position_new;
gamgi_enum type;

/***************************************************************************
 * import data from light: according to OpenGL, the position vector stores *
 * direction in directional lights and position in positional lights       *
 ***************************************************************************/

strcpy (name, light->object.name);

if (light->position[3] == 0) type = GAMGI_MESA_DIRECTIONAL;
else if (light->angle == 180) type = GAMGI_MESA_POSITIONAL;
else type = GAMGI_MESA_SPOT;

if (type == GAMGI_MESA_DIRECTIONAL)
  {
  direction[0] = -light->position[0];
  direction[1] = -light->position[1];
  direction[2] = -light->position[2];

  position[0] = 0.0;
  position[1] = 0.0;
  position[2] = 0.0;
  }
else
  {
  direction[0] = light->direction[0];
  direction[1] = light->direction[1];
  direction[2] = light->direction[2];
  
  position[0] = light->position[0];
  position[1] = light->position[1];
  position[2] = light->position[2];
  }
angle = light->angle;

if (cache.absolute == TRUE)
  {
  ambient[0] = light->ambient[0];
  ambient[1] = light->ambient[1];
  ambient[2] = light->ambient[2];

  diffuse[0] = light->diffuse[0];
  diffuse[1] = light->diffuse[1];
  diffuse[2] = light->diffuse[2];

  specular[0] = light->specular[0];
  specular[1] = light->specular[1];
  specular[2] = light->specular[2];
  }
else
  {
  ambient[0] =  0.0;
  ambient[1] =  0.0;
  ambient[2] =  0.0;

  diffuse[0] =  0.0;
  diffuse[1] =  0.0;
  diffuse[2] =  0.0;

  specular[0] = 0.0;
  specular[1] = 0.0;
  specular[2] = 0.0;
  }

constant = light->constant;
linear = light->linear;
quadratic = light->quadratic;
radial = light->radial;

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

if (cache.new[STATIC_TYPE] == TRUE) type = cache.type;
if (cache.new[STATIC_DIRECTION_X] == TRUE) direction[0] = cache.direction[0];
if (cache.new[STATIC_DIRECTION_Y] == TRUE) direction[1] = cache.direction[1];
if (cache.new[STATIC_DIRECTION_Z] == TRUE) direction[2] = cache.direction[2];
if (cache.new[STATIC_POSITION_X] == TRUE) position[0] = cache.position[0];
if (cache.new[STATIC_POSITION_Y] == TRUE) position[1] = cache.position[1];
if (cache.new[STATIC_POSITION_Z] == TRUE) position[2] = cache.position[2];
if (cache.new[STATIC_ANGLE] == TRUE) angle = cache.angle;

if (cache.new[STATIC_AMBIENT_R] == TRUE) ambient[0] = cache.ambient[0];
if (cache.new[STATIC_AMBIENT_G] == TRUE) ambient[1] = cache.ambient[1];
if (cache.new[STATIC_AMBIENT_B] == TRUE) ambient[2] = cache.ambient[2];
if (cache.new[STATIC_DIFFUSE_R] == TRUE) diffuse[0] = cache.diffuse[0];
if (cache.new[STATIC_DIFFUSE_G] == TRUE) diffuse[1] = cache.diffuse[1];
if (cache.new[STATIC_DIFFUSE_B] == TRUE) diffuse[2] = cache.diffuse[2];
if (cache.new[STATIC_SPECULAR_R] == TRUE) specular[0] = cache.specular[0];
if (cache.new[STATIC_SPECULAR_G] == TRUE) specular[1] = cache.specular[1];
if (cache.new[STATIC_SPECULAR_B] == TRUE) specular[2] = cache.specular[2];
if (cache.absolute == FALSE)
  {
  static_clamp (&ambient[0], light->ambient[0]);
  static_clamp (&ambient[1], light->ambient[1]);
  static_clamp (&ambient[2], light->ambient[2]);
  
  static_clamp (&diffuse[0], light->diffuse[0]);
  static_clamp (&diffuse[1], light->diffuse[1]);
  static_clamp (&diffuse[2], light->diffuse[2]);

  static_clamp (&specular[0], light->specular[0]);
  static_clamp (&specular[1], light->specular[1]);
  static_clamp (&specular[2], light->specular[2]);
  }

if (cache.new[STATIC_CONSTANT] == TRUE) constant = cache.constant;
if (cache.new[STATIC_LINEAR] == TRUE) linear = cache.linear;
if (cache.new[STATIC_QUADRATIC] == TRUE) quadratic = cache.quadratic;
if (cache.new[STATIC_RADIAL] == TRUE) radial = cache.radial;

/****************************
 * check data compatibility *
 ****************************/

if (cache.type > 3)
  {
  /*************************************************************
   * Local type: the type of light remains the same,           *
   * so adding data specific for other types is wrong.         *
   * 1) a directional light has no position,angle,axial,radial *
   * 2) a positional light has no direction,angle,radial       *
   *************************************************************/

  if (light->position[3] == 0.0 &&
  (cache.new[STATIC_POSITION_X] == TRUE || cache.new[STATIC_POSITION_Y] == TRUE ||
  cache.new[STATIC_POSITION_Z] == TRUE || cache.new[STATIC_ANGLE] == TRUE ||
  cache.new[STATIC_CONSTANT] == TRUE || cache.new[STATIC_LINEAR] == TRUE ||
  cache.new[STATIC_QUADRATIC] == TRUE || cache.new[STATIC_RADIAL] == TRUE))
    {
    gamgi_gtk_dialog_message_create ("Error", "Incompatible light data", window);
    return FALSE;
    }

  if (light->position[3] == 1.0 && light->angle == 180.0 &&
  (cache.new[STATIC_DIRECTION_X] == TRUE || cache.new[STATIC_DIRECTION_Y] == TRUE ||
  cache.new[STATIC_DIRECTION_Z] == TRUE || cache.new[STATIC_ANGLE] == TRUE ||
  cache.new[STATIC_RADIAL] == TRUE))
    {
    gamgi_gtk_dialog_message_create ("Error", "Incompatible light data", window);
    return FALSE;
    }
  }

/***************************************************************************
 * 1) changing from positional to directional light requires new direction *
 * 2) changing from positional to spot light requires new direction        *
 * 3) changing from spot to directional light requires new direction       *
 * 4) changing from directional to spot requires new direction             *
 ***************************************************************************/

direction_new = FALSE;
if (light->position[3] == 1.0 && light->angle == 180.0 &&
type == GAMGI_MESA_DIRECTIONAL) direction_new = TRUE;
if (light->position[3] == 1.0 && light->angle == 180.0 &&
type == GAMGI_MESA_SPOT) direction_new = TRUE;
if (light->angle != 180.0 && type == GAMGI_MESA_DIRECTIONAL)
  direction_new = TRUE;
if (light->position[3] == 0.0 && type == GAMGI_MESA_SPOT)
  direction_new = TRUE;

if (direction_new == TRUE)
  {
  if (cache.new[STATIC_DIRECTION_X] == FALSE || cache.new[STATIC_DIRECTION_Y] == FALSE ||
  cache.new[STATIC_DIRECTION_Z] == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light direction", window);
    return FALSE;
    }
  }

/********************************************************************
 * 1) changing from directional to spot requires new position       *
 * 2) changing from directional to positional requires new position *
 ********************************************************************/

position_new = FALSE;
if (light->position[3] == 0.0 && type == GAMGI_MESA_SPOT)
  position_new = TRUE;
if (light->position[3] == 0.0 && type == GAMGI_MESA_POSITIONAL)
  position_new = TRUE;

if (position_new == TRUE)
  {
  if (cache.new[STATIC_POSITION_X] == FALSE || cache.new[STATIC_POSITION_Y] == FALSE ||
  cache.new[STATIC_POSITION_Z] == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light position", window);
    return FALSE;
    }
  }

if (direction[0] == 0 && direction[1] == 0 && direction[2] == 0)
  {
  /*******************************************************
   * (0,0,0) is valid for position but not for direction *
   *******************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid light direction", window);
  return FALSE;
  }

if (ambient[0] + ambient[1] + ambient[2] == 0
&& diffuse[0] + diffuse[1] + diffuse[2] == 0
&& specular[0] + specular[1] + specular[2] == 0)
  {
  /**********************************************************
   * light must have ambient, diffuse or specular comonents *
   **********************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid light color", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 * *************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/************************
 * export data to light *
 ************************/

strcpy (light->object.name, name);

if (type == GAMGI_MESA_DIRECTIONAL)
  {
  /*********************
   * directional light *
   *********************/

  /************************************************************
   * invert the vector direction, so it has the same physical *
   * meaning in directional and positional lights, always     *
   * pointing from the light source to the light destination  *
   ************************************************************/

  light->position[0] = -direction[0];
  light->position[1] = -direction[1];
  light->position[2] = -direction[2];

  light->position[3] = 0.0;

  light->constant = GAMGI_MESA_LIGHT_CONSTANT;
  light->linear = GAMGI_MESA_LIGHT_LINEAR;
  light->quadratic = GAMGI_MESA_LIGHT_QUADRATIC;
  }
else
  {
  /****************************
   * positional or spot light *
   ****************************/

  light->position[0] = position[0];
  light->position[1] = position[1];
  light->position[2] = position[2];
  light->position[3] = 1.0;

  light->constant = constant;
  light->linear = linear;
  light->quadratic = quadratic;
  }

if (type == GAMGI_MESA_SPOT)
  {
  /**************
   * spot light *
   **************/

  light->direction[0] = direction[0];
  light->direction[1] = direction[1];
  light->direction[2] = direction[2];

  /*******************************************************************
   * if the spot light was a directional or positional light before, *
   * then angle will be 180.0 initially. If the user did not supply  *
   * a new angle at all, then the default 90.0 is provided.          *
   *******************************************************************/

  if (angle == 180.0) light->angle = 90.0;
  else light->angle = angle;

  light->radial = radial;
  }
else
  {
  /***********************************
   * directional or positional light *
   ***********************************/

  light->direction[0] = GAMGI_MESA_LIGHT_DIRECTION_X;
  light->direction[1] = GAMGI_MESA_LIGHT_DIRECTION_Y;
  light->direction[2] = GAMGI_MESA_LIGHT_DIRECTION_Z;

  light->angle = 180.0;
  light->radial = GAMGI_MESA_LIGHT_RADIAL;
  }

light->ambient[0] = ambient[0];
light->ambient[1] = ambient[1];
light->ambient[2] = ambient[2];

light->diffuse[0] = diffuse[0];
light->diffuse[1] = diffuse[1];
light->diffuse[2] = diffuse[2];

light->specular[0] = specular[0];
light->specular[1] = specular[1];
light->specular[2] = specular[2];

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

if (gamgi->lights == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of light objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_light");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid light identity", window);
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
for (dlist = gamgi->lights; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_LIGHT dlist->data, window) == FALSE)
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
gamgi_light *light;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/******************************************
 * pick light and get name (possibly new) *
 *                                        *
 * check if focus object belongs to light *
 ******************************************/

cache.new[STATIC_NAME] = TRUE;
light = GAMGI_CAST_LIGHT gamgi_gtk_object_number (dialog,
"entry_light", GAMGI_ENGINE_LIGHT, window, cache.name);
if (light == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid light identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT light) == TRUE) focus = TRUE;

/*********************************************
 * collect data from dialog and modify light *
 *********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (light, window) == FALSE) return;

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

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 3);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_light *light;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];

light = GAMGI_CAST_LIGHT object;

static_reset (window);
static_absolute (dialog);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_light");
sprintf (string, "%s %d", light->object.name, light->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*********************
 * set type of light *
 *********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_type");

if (light->position[3] == 0.0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_DIRECTIONAL - 1);
else if (light->angle == 180.0)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_POSITIONAL - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_SPOT - 1);
   
/****************
 * get position *
 ****************/

if (light->position[3] == 1.0)
  {
  /***********************************
   * positional light: read position *
   ***********************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
  sprintf (string, "%.*f", gamgi->gamgi->length, light->position[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
  sprintf (string, "%.*f", gamgi->gamgi->length, light->position[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
  sprintf (string, "%.*f", gamgi->gamgi->length, light->position[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  /********************************************
   * positional light: read axial attenuation *
   ********************************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_constant");
  sprintf (string, "%.*f",  GAMGI_MATH_DECIMAL_ATTENUATION, light->constant);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_linear");
  sprintf (string, "%.*f",  GAMGI_MATH_DECIMAL_ATTENUATION, light->linear);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_quadratic");
  sprintf (string, "%.*f",  GAMGI_MATH_DECIMAL_ATTENUATION, light->quadratic);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  if (light->angle != 180.0)
    {
    /*********************************************************
     * spot light: read direction, angle, radial attenuation *
     *********************************************************/

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
    sprintf (string, "%.*f", gamgi->gamgi->length, light->direction[0]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
    sprintf (string, "%.*f", gamgi->gamgi->length, light->direction[1]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");
    sprintf (string, "%.*f", gamgi->gamgi->length, light->direction[2]);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
    sprintf (string, "%.*f", gamgi->gamgi->angle, light->angle);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radial");
    sprintf (string, "%.*f",  GAMGI_MATH_DECIMAL_ATTENUATION, light->radial);
    gtk_entry_set_text (GTK_ENTRY (entry), string);
    }
  }
else
  {
  /*************************************
   * directional light: read direction *
   *************************************/

  /************************************************************
   * invert the vector direction, so it has the same physical *
   * meaning in directional and positional lights, always     *
   * pointing from the light source to the light destination  *
   ************************************************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
  sprintf (string, "%.*f", gamgi->gamgi->length, -light->position[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
  sprintf (string, "%.*f", gamgi->gamgi->length, -light->position[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");
  sprintf (string, "%.*f", gamgi->gamgi->length, -light->position[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

/*********************
 * get ambient color *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_r");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->ambient[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_g");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->ambient[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_b");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->ambient[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*********************
 * get diffuse color *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_r");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_g");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_b");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/**********************
 * get specular color *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_r");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->specular[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_g");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->specular[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_b");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, light->specular[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

static_light (window);
}

/*************** external function *************
 *                                             *
 *          GAMGI_GTK_LIGHT_MODIFY_PRESS       *
 *                                             *
 *                                             *
 ***********************************************/
 
void gamgi_gtk_light_modify_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *button; 

/******************************
 * local mouse selection only *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE ||
window_mouse->layer->light_start == NULL ||
window_dialog != window_mouse) return;

gamgi_mesa_select_object (window_mouse, x, y, 
GAMGI_ENGINE_LIGHT, FALSE, static_press);
}

/**************** external function ******************
 *                                                   *
 *             GAMGI_GTK_LIGHT_MODIFY                *
 *                                                   *
 *  Creates the dialog window used to modify lights. *
 *                                                   *
 *****************************************************/

void gamgi_gtk_light_modify (GtkWidget *widget, void *data)
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

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Light Modify", window);
window->action = GAMGI_GTK_LIGHT_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Light objects *
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

label = gtk_label_new ("Light");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_light", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_light", entry);
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

/*****************
 * Geometry page *
 *****************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Geometry");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/****************
 * illumination *
 ****************/

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
g_signal_connect (combo, "changed",
G_CALLBACK (static_default), window);
g_object_set_data (G_OBJECT (dialog), "combo_type", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Directional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Positional", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Spot", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/************
 * geometry *
 ************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (4, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("Direction");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_direction", label);
gtk_widget_show (label);

label = gtk_label_new ("Position");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_position", label);
gtk_widget_show (label);

label = gtk_label_new ("Angle");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_angle", label);
gtk_widget_show (label);

label = gtk_label_new ("X");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Y");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("Z");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_direction_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_x", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_direction_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_y", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_angle", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_direction_z", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_position_z", entry);
gtk_widget_show (entry);

/**************
 * Color page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Color ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (4, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Ambient");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Diffuse");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Specular");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

/*******
 * red *
 *******/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_ambient_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_diffuse_r", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_specular_r", entry);
gtk_widget_show (entry);

/*********
 * green *
 *********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 10, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_ambient_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 10, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_diffuse_g", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_FILL, GTK_FILL, 10, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_specular_g", entry);
gtk_widget_show (entry);

/********
 * blue *
 ********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_ambient_b", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 3, 4, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_diffuse_b", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 3, 4, GTK_FILL, GTK_FILL, 10, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_specular_b", entry);
gtk_widget_show (entry);

/**********************
 * Absolute, Relative *
 **********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_color), window);
g_object_set_data (G_OBJECT (dialog), "button_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled",
G_CALLBACK (static_color), window);
g_object_set_data (G_OBJECT (dialog), "button_relative", button);
gtk_widget_show (button);

/********************
 * Attenuation page *
 ********************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Attenuation");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********
 * axial *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (3, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Axial");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
g_object_set_data (G_OBJECT (dialog), "label_axial", label);
gtk_widget_show (label);

label = gtk_label_new ("Constant");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_constant", label);
gtk_widget_show (label);

label = gtk_label_new ("Linear");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_linear", label);
gtk_widget_show (label);

label = gtk_label_new ("Quadratic");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_quadratic", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_constant", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_linear", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_quadratic", entry);
gtk_widget_show (entry);

/**********
 * radial *
 **********/

label = gtk_label_new ("Radial");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 15, 0);
g_object_set_data (G_OBJECT (dialog), "label_radial", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 2, 3, GTK_FILL, GTK_FILL, 15, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_radial", entry);
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
