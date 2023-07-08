/*************************************************
 *
 * $GAMGI/src/gtk/light/gamgi_gtk_light_create.c
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
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_io_token.h"

static void static_light (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label_direction, *label_position, *label_angle;
GtkWidget *entry_direction_x, *entry_direction_y, *entry_direction_z;
GtkWidget *entry_position_x, *entry_position_y, *entry_position_z;
GtkWidget *entry_angle;
GtkWidget *label_axial, *label_radial;
GtkWidget *label_constant, *label_linear, *label_quadratic;
GtkWidget *entry_constant, *entry_linear, *entry_quadratic;
GtkWidget *entry_radial;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

label_direction = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_direction");
entry_direction_x = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
entry_direction_y = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
entry_direction_z = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");

label_position = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_position");
entry_position_x = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
entry_position_y = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
entry_position_z = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");

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

/*********************
 * directional light *
 *********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "radio_button_directional");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (label_direction, TRUE);
  gtk_widget_set_sensitive (entry_direction_x, TRUE);
  gtk_widget_set_sensitive (entry_direction_y, TRUE);
  gtk_widget_set_sensitive (entry_direction_z, TRUE);

  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_X);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_x), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_y), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_z), token);

  gtk_widget_set_sensitive (label_position, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_position_x), "");
  gtk_widget_set_sensitive (entry_position_x, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_position_y), "");
  gtk_widget_set_sensitive (entry_position_y, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_position_z), "");
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

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "radio_button_positional");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (label_direction, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_x), "");
  gtk_widget_set_sensitive (entry_direction_x, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_y), "");
  gtk_widget_set_sensitive (entry_direction_y, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_z), "");
  gtk_widget_set_sensitive (entry_direction_z, FALSE);

  gtk_widget_set_sensitive (label_position, TRUE);
  gtk_widget_set_sensitive (entry_position_x, TRUE);
  gtk_widget_set_sensitive (entry_position_y, TRUE);
  gtk_widget_set_sensitive (entry_position_z, TRUE);

  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_X);
  gtk_entry_set_text (GTK_ENTRY (entry_position_x), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry_position_y), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry_position_z), token);

  gtk_entry_set_text (GTK_ENTRY (entry_angle), "");
  gtk_widget_set_sensitive (entry_angle, FALSE);
  gtk_widget_set_sensitive (label_angle, FALSE);

  gtk_widget_set_sensitive (label_axial, TRUE);
  gtk_widget_set_sensitive (label_constant, TRUE);
  gtk_widget_set_sensitive (label_linear, TRUE);
  gtk_widget_set_sensitive (label_quadratic, TRUE);
  gtk_widget_set_sensitive (entry_constant, TRUE);
  gtk_widget_set_sensitive (entry_linear, TRUE);
  gtk_widget_set_sensitive (entry_quadratic, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION, 
  GAMGI_MESA_LIGHT_CONSTANT);
  gtk_entry_set_text (GTK_ENTRY (entry_constant), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION, 
  GAMGI_MESA_LIGHT_LINEAR);
  gtk_entry_set_text (GTK_ENTRY (entry_linear), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION, 
  GAMGI_MESA_LIGHT_QUADRATIC);
  gtk_entry_set_text (GTK_ENTRY (entry_quadratic), token);

  gtk_widget_set_sensitive (label_radial, FALSE);
  gtk_entry_set_text (GTK_ENTRY (entry_radial), "");
  gtk_widget_set_sensitive (entry_radial, FALSE);
  }

/**************
 * spot light *
 **************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "radio_button_spot");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (label_direction, TRUE);
  gtk_widget_set_sensitive (entry_direction_x, TRUE);
  gtk_widget_set_sensitive (entry_direction_y, TRUE);
  gtk_widget_set_sensitive (entry_direction_z, TRUE);

  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_X);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_x), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_y), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_DIRECTION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry_direction_z), token);

  gtk_widget_set_sensitive (label_position, TRUE);
  gtk_widget_set_sensitive (entry_position_x, TRUE);
  gtk_widget_set_sensitive (entry_position_y, TRUE);
  gtk_widget_set_sensitive (entry_position_z, TRUE);

  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_X);
  gtk_entry_set_text (GTK_ENTRY (entry_position_x), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Y);
  gtk_entry_set_text (GTK_ENTRY (entry_position_y), token);
  sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_MESA_LIGHT_POSITION_Z);
  gtk_entry_set_text (GTK_ENTRY (entry_position_z), token);

  gtk_widget_set_sensitive (label_angle, TRUE);
  gtk_widget_set_sensitive (entry_angle, TRUE);
  sprintf (token, "%.*f", gamgi->gamgi->angle, 
  GAMGI_MESA_LIGHT_ANGLE);
  gtk_entry_set_text (GTK_ENTRY (entry_angle), token);

  gtk_widget_set_sensitive (label_axial, TRUE);
  gtk_widget_set_sensitive (label_constant, TRUE);
  gtk_widget_set_sensitive (label_linear, TRUE);
  gtk_widget_set_sensitive (label_quadratic, TRUE);
  gtk_widget_set_sensitive (entry_constant, TRUE);
  gtk_widget_set_sensitive (entry_linear, TRUE);
  gtk_widget_set_sensitive (entry_quadratic, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  GAMGI_MESA_LIGHT_CONSTANT);
  gtk_entry_set_text (GTK_ENTRY (entry_constant), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  GAMGI_MESA_LIGHT_LINEAR);
  gtk_entry_set_text (GTK_ENTRY (entry_linear), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION,
  GAMGI_MESA_LIGHT_QUADRATIC);
  gtk_entry_set_text (GTK_ENTRY (entry_quadratic), token);

  gtk_widget_set_sensitive (label_radial, TRUE);
  gtk_widget_set_sensitive (entry_radial, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_ATTENUATION, 
  (float) GAMGI_MESA_LIGHT_RADIAL);
  gtk_entry_set_text (GTK_ENTRY (entry_radial), token);
  }
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_light *light;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;
GtkEntry *entry;
const char *name;
const char *direction_x, *direction_y, *direction_z;
int ambient = 0, diffuse = 0, specular = 0;

/*******************************************************************************
 * OpenGL contexts cannot have more than gamgi->lights lights enabled at       *
 * the same time. In GAMGI this limit applies only for each layer because      *
 * lights are swittched off everytime GAMGI draws a new layer. As the number   *
 * of possible layers in GAMGI is limited by computer memory only , the number *
 * of lights visible at the same time is also limited by computer memory only. *
 *******************************************************************************/

if (window->layer->light == gamgi->max_lights)
  {
  gamgi_gtk_dialog_message_create ("Error", "Light resources exceeded", window);
  return;
  }

/*********
 * light *
 *********/

light = gamgi_engine_create_light ();
gamgi_engine_start_light (light);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_light",
"Light", light->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Light Name", window);
  gamgi_engine_remove_light (light);
  return;
  }

/*************
 * ambient r *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_r");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  ambient++;
  if (gamgi_io_token_float_scan (name, &light->ambient[0], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }  

/*************
 * ambient g *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_g");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  ambient++;
  if (gamgi_io_token_float_scan (name, &light->ambient[1], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/*************
 * ambient b *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_b");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  ambient++;
  if (gamgi_io_token_float_scan (name, &light->ambient[2], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/*************
 * diffuse r *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_r");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  diffuse++;
  if (gamgi_io_token_float_scan (name, &light->diffuse[0], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }  

/*************
 * diffuse g *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_g");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  diffuse++;
  if (gamgi_io_token_float_scan (name, &light->diffuse[1], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/*************
 * diffuse b *
 *************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_b");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  diffuse++;
  if (gamgi_io_token_float_scan (name, &light->diffuse[2], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/**************
 * specular r *
 **************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_r");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  specular++;
  if (gamgi_io_token_float_scan (name, &light->specular[0], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }  

/**************
 * specular g *
 **************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_g");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  specular++;
  if (gamgi_io_token_float_scan (name, &light->specular[1], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/**************
 * specular b *
 **************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_specular_b");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE)
  {
  specular++;
  if (gamgi_io_token_float_scan (name, &light->specular[2], 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

if (ambient == 1 || ambient == 2)
  {
  /******************************************************
   * error: user must indicate all r,g,b values or none *
   ******************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_light (light);
  return;
  }

if (specular == 1 || specular == 2)
  {
  /******************************************************
   * error: user must indicate all r,g,b values or none *
   ******************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_light (light);
  return;
  }

if (diffuse == 1 || diffuse == 2)
  {
  /******************************************************
   * error: user must indicate all r,g,b values or none *
   ******************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_light (light);
  return;
  }

/*****************************************************************
 * error: light must have ambient, diffuse or specular comonents *
 *****************************************************************/

if (light->ambient[0] + light->ambient[1] + light->ambient[2] == 0
&& light->diffuse[0] + light->diffuse[1] + light->diffuse[2] == 0
&& light->specular[0] + light->specular[1] + light->specular[2] == 0)
  {
  /***************************************************************
   * error: ambient, diffuse or specular color must be indicated *
   ***************************************************************/

  gamgi_gtk_dialog_message_create ("Error", "Invalid Color Data", window);
  gamgi_engine_remove_light (light);
  return;
  }

/*********************************************
 * read direction data: this data is used    *
 * differently in positional and directional *
 * lights, so read it now and analyse later. *
 *********************************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_direction_x");
direction_x = gtk_entry_get_text (entry);
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_direction_y");
direction_y = gtk_entry_get_text (entry);
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_direction_z");
direction_z = gtk_entry_get_text (entry);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "radio_button_directional");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /*************************************
   * directional light: read direction *
   *************************************/
  
  if (gamgi_io_token_float_scan (direction_x, &light->position[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  if (gamgi_io_token_float_scan (direction_y, &light->position[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  if (gamgi_io_token_float_scan (direction_z, &light->position[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /************************************************************
   * invert the vector direction, so it has the same physical *
   * meaning in directional and positional lights, always     *
   * pointing from the light source to the light destination  *
   ************************************************************/

  light->position[0] *= -1.0;
  light->position[1] *= -1.0;
  light->position[2] *= -1.0;

  /************************
   * set directional flag *
   ************************/

  light->position[3] = 0.0;
  }
else
  {
  /*********************************************************
   * positional light: read position and axial attenuation *
   *********************************************************/

  /**************
   * position x *
   **************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_position_x");
  name = gtk_entry_get_text (entry);

  if (gamgi_io_token_float_scan (name, &light->position[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /**************
   * position y *
   **************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_position_y");
  name = gtk_entry_get_text (entry);

  if (gamgi_io_token_float_scan (name, &light->position[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /**************
   * position z *
   **************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_position_z");
  name = gtk_entry_get_text (entry);

  if (gamgi_io_token_float_scan (name, &light->position[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /***********************
   * set positional flag *
   ***********************/

  light->position[3] = 1.0;

  /************************
   * constant attenuation *
   ************************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_constant");
  name = gtk_entry_get_text (entry);

  light->constant = GAMGI_MESA_LIGHT_CONSTANT;
  if (gamgi_io_token_check (name) == TRUE &&
  gamgi_io_token_float_scan (name, &light->constant, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Attenuation", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /**********************
   * linear attenuation *
   **********************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_linear");
  name = gtk_entry_get_text (entry);

  light->linear = GAMGI_MESA_LIGHT_LINEAR;
  if (gamgi_io_token_check (name) == TRUE &&
  gamgi_io_token_float_scan (name, &light->linear, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Attenuation", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /*************************
   * quadratic attenuation *
   *************************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_quadratic");
  name = gtk_entry_get_text (entry);

  light->quadratic = GAMGI_MESA_LIGHT_QUADRATIC;
  if (gamgi_io_token_check (name) == TRUE &&
  gamgi_io_token_float_scan (name, &light->quadratic, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Attenuation", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "radio_button_spot");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  /************************************************************
   * spot light: read direction, angle and radial attenuation *
   ************************************************************/

  /*************
   * direction *
   *************/

  if (gamgi_io_token_float_scan (direction_x, 
  &light->direction[0], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  if (gamgi_io_token_float_scan (direction_y, 
  &light->direction[1], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  if (gamgi_io_token_float_scan (direction_z, 
  &light->direction[2], -DBL_MAX, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }

  /*********
   * angle *
   *********/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_angle");
  name = gtk_entry_get_text (entry);

  light->angle = GAMGI_MESA_LIGHT_ANGLE;
  if (gamgi_io_token_check (name) == TRUE &&
  gamgi_io_token_float_scan (name, &light->angle, 0.0, 90.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Geometry", window);
    gamgi_engine_remove_light (light);
    return;
    }
    
  /**********************
   * radial attenuation *
   **********************/

  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_radial");
  name = gtk_entry_get_text (entry);

  light->radial = GAMGI_MESA_LIGHT_RADIAL;
  if (gamgi_io_token_check (name) == TRUE &&
  gamgi_io_token_float_scan (name, &light->radial, 0.0, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid Light Attenuation", window);
    gamgi_engine_remove_light (light);
    return;
    }
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);
   
gamgi_engine_link_light_layer (light, window->layer);

/******************************************
 * Remove dialog and redraw gl_area image *
 ******************************************/

gamgi_gtk_dialog_task0_remove (NULL, window);
gtk_widget_queue_draw (window->area);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->ambient[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->ambient[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ambient_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->ambient[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->diffuse[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->diffuse[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_diffuse_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->diffuse[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_r");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->specular[0]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_g");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->specular[1]);
gtk_entry_set_text (GTK_ENTRY (entry), token);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_specular_b");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, gamgi->light->specular[2]);
gtk_entry_set_text (GTK_ENTRY (entry), token);

static_light (NULL, window);
}

/**************** external function ******************
 *                                                   *
 *             GAMGI_GTK_LIGHT_CREATE                *
 *                                                   *
 *  Creates the dialog window used to create lights. *
 *                                                   *
 *****************************************************/

void gamgi_gtk_light_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Light Create", window);
window->action = GAMGI_GTK_LIGHT_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/**************
 * Light name *
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
g_object_set_data (G_OBJECT (dialog), "entry_light", entry);
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

button = gtk_radio_button_new_with_label (NULL, "Directional");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_light), window);
g_object_set_data (G_OBJECT (dialog), "radio_button_directional", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Positional");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_light), window);
g_object_set_data (G_OBJECT (dialog), "radio_button_positional", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Spot");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_light), window);
g_object_set_data (G_OBJECT (dialog), "radio_button_spot", button);
gtk_widget_show (button);

/************
 * geometry *
 ************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (4, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
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

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), hbox_center, FALSE, FALSE, 0);
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
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
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
