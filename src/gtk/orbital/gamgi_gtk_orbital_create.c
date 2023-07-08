/*****************************************************
 *
 * $GAMGI/src/gtk/orbital/gamgi_gtk_orbital_create.c
 *
 * Copyright (C) 2013 Carlos Pereira
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
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_area.h"
#include "gamgi_chem_orbital.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"

static void static_phase (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label, *label_red, *label_green, *label_blue;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_blue");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");

button = g_object_get_data (G_OBJECT (dialog), "button_phase");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (label, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);

  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_B);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_widget_set_sensitive (label, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);

  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  }
}

static void static_frame (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label_red, *label_green, *label_blue;
GtkWidget *label;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame");

label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_blue");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");

button = g_object_get_data (G_OBJECT (dialog), "button_frame");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (label, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);

  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_B);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_widget_set_sensitive (label, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);

  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  }
}

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
int n, style;

/************************
 * set default sampling *
 ************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
n = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
style = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
sprintf (token, "%d", gamgi_chem_orbital_sampling (n, style));
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_m (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
double charge, radius;
int n, l, m;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
n = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
m = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) - 5;

/********************
 * set default name *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_orbital");
gamgi_chem_orbital_name (n, l, m, token);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/************************
 * set default sampling *
 ************************/

static_style (NULL, window);

/**********************
 * set default radius *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &charge,
1.0, GAMGI_CHEM_ATOM_MAX) == FALSE) radius = 0.0;
else radius = gamgi_chem_orbital_radius (n, l, m, charge);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
if (radius > 0.0)
  {
  sprintf (token, "%.*f", gamgi->gamgi->length, radius);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
else gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_l (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_bool *sensitive;
int l, m;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
m = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) - 5;

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_m");

/*********************************************
 * 1) reset M if current option is forbidden *
 * 2) disable forbidden M options            *
 * 3) call static_m to update M data         *
 *********************************************/

if (abs (m) > l)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 5);
gamgi_chem_orbital_m (l, sensitive);
static_m (NULL, window);
}

static void static_n (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_bool *sensitive;
int n, l;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
n = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_l");

/*********************************************
 * 1) reset L if current option is forbidden *
 * 2) disable forbidden L options            *
 * 3) call static_l to update L combo, data  *
 *********************************************/

if (l > n - 1)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
gamgi_chem_orbital_l (n, sensitive);
static_l (NULL, window);
}

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E1);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E2);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E3);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_orbital *orbital;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
const char *name;
double origin[3], euler[3];

orbital = gamgi_engine_create_orbital ();
gamgi_engine_start_orbital (orbital);

/************
 * get name *
 ************/

if (gamgi_gtk_object_name (dialog, "entry_orbital",
"Orbital", orbital->object.name) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Orbital Name", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/******************************************************
 * Type page: get n, l, m quantum numbers, get charge *
 ******************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
orbital->n = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
orbital->l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
orbital->m = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) - 5;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name,
&orbital->charge, 1.0, GAMGI_CHEM_ATOM_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid charge", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/**********************************************************
 * Scope page: get style, density, sampling, radius, seed *
 **********************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
orbital->style = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &orbital->density, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid probability density", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &orbital->radius,
GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid sampling radius", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &orbital->sampling, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid sampling data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

if (orbital->style == GAMGI_MESA_SOLID &&
orbital->sampling > GAMGI_CHEM_ORBITAL_SLICES_MAX)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid sampling data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_int_scan (name, &orbital->seed, 1, INT_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid sampling seed", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/************************************************
 * Volume page: get phase, frame, octants, axes *
 ************************************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_phase");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  orbital->phase = TRUE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_frame");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  orbital->frame = TRUE;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_1");
orbital->octants[0] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_2");
orbital->octants[1] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_3");
orbital->octants[2] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_4");
orbital->octants[3] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_5");
orbital->octants[4] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_6");
orbital->octants[5] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_7");
orbital->octants[6] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_8");
orbital->octants[7] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
orbital->axes = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

/*************************
 * get x,y,z coordinates *
 *************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
name = gtk_entry_get_text (GTK_ENTRY (entry));

if (gamgi_io_token_double_scan (name, &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Position Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/********************
 * get Euler angles *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[0], 0.0, 180.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[1], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &euler[2], 0.0, 360.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Angle Data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/********************************************
 * View page: get Base, Phase, Frame colors *
 ********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &orbital->base_r, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid base color", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &orbital->base_g, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid base color", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_float_scan (name, &orbital->base_b, 0.0, 1.0) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid base color", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

if (orbital->phase == TRUE)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->phase_r, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->phase_g, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->phase_b, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }
  }

if (orbital->frame == TRUE)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->frame_r, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->frame_g, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  if (gamgi_io_token_float_scan (name, &orbital->frame_b, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    gamgi_engine_remove_orbital (orbital);
    return;
    }
  }

/*************
 * Get scale *
 *************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_double_scan (name, &orbital->object.scale,
GAMGI_MESA_SCALE_LOWER, GAMGI_MESA_SCALE_UPPER) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid scale", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

gamgi_math_position_orbital_rotation_set (orbital, euler);
gamgi_math_position_orbital_translation_set (orbital, origin);

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/***************************************************************************
 * Wired mode: return FALSE if unable to get the number of points required *
 ***************************************************************************/

if (gamgi_chem_orbital_create (orbital) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid scope data", window);
  gamgi_engine_remove_orbital (orbital);
  return;
  }

/*****************************************
 * Redraw gl_area image and reset dialog *
 *****************************************/

gamgi_engine_link_orbital_layer (orbital, window->layer);

gtk_widget_queue_draw (window->area);

static_reset (window);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 5);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
sprintf (token, "%.*f", gamgi->gamgi->charge, GAMGI_CHEM_ORBITAL_CHARGE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * start Scope page *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_MESA_WIRED - 1);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
sprintf (token, "%d", GAMGI_CHEM_ORBITAL_POINTS);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
sprintf (token, "%.*E", 1, GAMGI_CHEM_ORBITAL_DENSITY);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
sprintf (token, "%d", GAMGI_MATH_SEED);
gtk_entry_set_text (GTK_ENTRY (entry), token);

static_m (NULL, window);

/*********************
 * start Volume page *
 *********************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_phase");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_frame");
gtk_button_clicked (GTK_BUTTON (button));

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_1");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_2");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_3");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_4");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_5");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_6");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_7");
gtk_button_clicked (GTK_BUTTON (button));
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_8");
gtk_button_clicked (GTK_BUTTON (button));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_CHEM_NONE - 1);

/***********************
 * start Position page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_X);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_Y);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (token, "%.*f", gamgi->gamgi->length, GAMGI_CHEM_ORBITAL_Z);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E1);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E2);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (token, "%.*f", gamgi->gamgi->angle, GAMGI_CHEM_ORBITAL_E3);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/*******************
 * start View page *
 *******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_red");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_BASE_R);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_green");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_BASE_G);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_blue");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_BASE_B);
gtk_entry_set_text (GTK_ENTRY (entry), token);

static_phase (NULL, window);
static_frame (NULL, window);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_SCALE);
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

/*************** external function *************
 *                                             *
 *       GAMGI_GTK_ORBITAL_CREATE_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_orbital_create_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
GtkWidget *dialog = window_dialog->dialog0;
GtkWidget *entry;
double x_3d, y_3d, z_3d;
char string[GAMGI_ENGINE_TOKEN];

/******************************
 * local mouse selection only *
 ******************************/

if (window_dialog != window_mouse) return;
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

/*************** external function *************
 *                                             *
 *           GAMGI_GTK_ORBITAL_CREATE          *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_orbital_create (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left, *hbox_left_left;
GtkWidget *vbox_dialog, *vbox_page, *vbox_center, *vbox_top, *vbox_top_top;
GtkWidget *button;
GtkWidget *label;
GtkWidget *entry;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;
gamgi_bool *sensitive;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Orbital Create", window);
window->action = GAMGI_GTK_ORBITAL_CREATE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/****************
 * Orbital name *
 ****************/

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
g_object_set_data (G_OBJECT (dialog), "entry_orbital", entry);
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
 * type page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Type ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*******************
 * Quantum numbers *
 *******************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

label = gtk_label_new ("Numbers");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/***********
 * combo N *
 ***********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("N");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_n), window);
g_object_set_data (G_OBJECT (dialog), "combo_n", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "2", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "3", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "4", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "5", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "6", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/***********
 * combo L *
 ***********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("L");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_l), window);
g_object_set_data (G_OBJECT (dialog), "combo_l", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "0", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "2", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "3", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "4", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "5", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (6);
g_object_set_data (G_OBJECT (dialog), "sensitive_l", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/***********
 * combo M *
 ***********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("M");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_m), window);
g_object_set_data (G_OBJECT (dialog), "combo_m", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "-5", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "-4", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "-3", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "-2", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "-1", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "0", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "1", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "2", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "3", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "4", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "5", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (11);
g_object_set_data (G_OBJECT (dialog), "sensitive_m", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/**********
 * charge *
 **********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Charge");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_charge", entry);
gtk_widget_show (entry);

/**************
 * Scope page *
 **************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Scope ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/**************
 * Style menu *
 **************/

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
g_signal_connect (combo, "changed", G_CALLBACK (static_style), window);
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

/*****************
 * 2nd, 3rd rows *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/*********************
 * Density, Sampling *
 *********************/

vbox_top_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_density", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Density");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_sampling", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Sampling");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/****************
 * Radius, Seed *
 ****************/

vbox_top_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_radius", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Radius");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_seed", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Seed");
gtk_box_pack_end (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/***************
 * Volume page *
 ***************/

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

/************************
 * Phase, Frame buttons *
 ************************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("Phase");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_phase), window);
g_object_set_data (G_OBJECT (dialog), "button_phase", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("Frame");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked", G_CALLBACK (static_frame), window);
g_object_set_data (G_OBJECT (dialog), "button_frame", button);
gtk_widget_show (button);

/***********
 * Octants *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Octants");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

table = gtk_table_new (2, 4, TRUE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label ("-3");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_1", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-4");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_2", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-2");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_3", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-1");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_4", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+3");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_5", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+4");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_6", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+2");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_7", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+1");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_8", button);
gtk_widget_show (button);

/********
 * axes *
 ********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Axes");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_axes", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Radius", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Unit", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Bohr", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

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

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/***************
 * Translation *
 ***************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

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

/************
 * Rotation *
 ************/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

label = gtk_label_new ("Rotation");
gtk_box_pack_start (GTK_BOX (hbox_center), label, FALSE, FALSE, 0);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("E1");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E2");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("E3");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
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

/*********
 * Color *
 *********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (3, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Color");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_base_blue", entry);
gtk_widget_show (entry);

/*********
 * Phase *
 *********/

label = gtk_label_new ("Phase");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_phase", label);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_phase_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_phase_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_phase_blue", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_phase_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_phase_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_phase_blue", entry);
gtk_widget_show (entry);

/*********
 * Frame *
 *********/

label = gtk_label_new ("Frame");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_frame", label);
gtk_widget_show (label);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_frame_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_frame_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_frame_blue", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_frame_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_frame_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 5);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_frame_blue", entry);
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
