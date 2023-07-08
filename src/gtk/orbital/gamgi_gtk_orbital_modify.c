/*****************************************************
 *
 * $GAMGI/src/gtk/orbital/gamgi_gtk_orbital_modify.c
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

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_chem_orbital.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

enum { STATIC_STYLE_WIRED = 0, STATIC_STYLE_SOLID, STATIC_STYLE_LOCAL };
enum { STATIC_PHASE_YES = 0, STATIC_PHASE_NO, STATIC_PHASE_LOCAL };
enum { STATIC_FRAME_YES = 0, STATIC_FRAME_NO, STATIC_FRAME_LOCAL };
enum { STATIC_AXES_RADIUS = 0, STATIC_AXES_UNIT, STATIC_AXES_BOHR,
STATIC_AXES_NONE, STATIC_AXES_LOCAL };
enum { STATIC_NAME = 0, STATIC_NUMBERS, STATIC_CHARGE,
STATIC_STYLE, STATIC_DENSITY, STATIC_SAMPLING, STATIC_RADIUS,
STATIC_SEED, STATIC_PHASE, STATIC_FRAME, STATIC_OCTANTS, STATIC_AXES,
STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y, STATIC_TRANSLATION_Z,
STATIC_ROTATION_E1, STATIC_ROTATION_E2, STATIC_ROTATION_E3,
STATIC_BASE_RED, STATIC_BASE_GREEN, STATIC_BASE_BLUE,
STATIC_PHASE_RED, STATIC_PHASE_GREEN, STATIC_PHASE_BLUE,
STATIC_FRAME_RED, STATIC_FRAME_GREEN, STATIC_FRAME_BLUE,
STATIC_SCALE };

static struct {
gamgi_bool local;
gamgi_bool new[STATIC_SCALE + 1];
char name[GAMGI_ENGINE_TOKEN];
int n, l, m;
double charge;
gamgi_enum style;
double density, radius;
int sampling, seed;
gamgi_bool phase, frame;
double octants[8];
double axes;
gamgi_bool translation_absolute, rotation_absolute;
double translation[3], rotation[3];
float base_r, base_g, base_b;
float frame_r, frame_g, frame_b;
float phase_r, phase_g, phase_b;
double scale; } cache;

static void static_name (gamgi_window *window, char *name)
{
gamgi_orbital *orbital;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
char string[GAMGI_ENGINE_STRING];
int active;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_orbital");

orbital = GAMGI_CAST_ORBITAL gamgi_gtk_object_number (dialog,
"entry_orbital", GAMGI_ENGINE_ORBITAL, window, string);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));

if (active == TRUE || orbital == NULL)
  {
  if (name == NULL)
    sprintf (string, "%s", "");
  else
    sprintf (string, "%s", name);
  }
else
  {
  if (name == NULL)
    sprintf (string, "%s %d", orbital->object.name, orbital->object.number);
  else
    sprintf (string, "%s %d", name, orbital->object.number);
  }

gtk_entry_set_text (GTK_ENTRY (entry), string);
}

static void static_phase (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *label_red, *label_green, *label_blue;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
int phase;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_phase_blue");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_phase");
phase = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (phase == STATIC_PHASE_NO)
  {
  gtk_widget_set_sensitive (label, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (label, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  }

if (phase == STATIC_PHASE_YES)
  {
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_PHASE_B);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  }
}

static void static_frame (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *label_red, *label_green, *label_blue;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
int frame;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_frame_blue");

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_frame");
frame = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (frame == STATIC_FRAME_NO)
  {
  gtk_widget_set_sensitive (label, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE); 
  }
else
  {
  gtk_widget_set_sensitive (label, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  }

  
if (frame == STATIC_FRAME_YES)
  {
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_R);
  gtk_entry_set_text (GTK_ENTRY (entry_red), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_G);
  gtk_entry_set_text (GTK_ENTRY (entry_green), token);
  sprintf (token, "%.*f", GAMGI_MATH_DECIMAL_COLOR, GAMGI_MESA_ORBITAL_FRAME_B);
  gtk_entry_set_text (GTK_ENTRY (entry_blue), token);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  }
}

static void static_style (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *combo;
char token[GAMGI_ENGINE_TOKEN];
int n, style;

/**********************************************************************
 * when no specific orbital or style is set, sampling should be empty *
 **********************************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_numbers");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
style = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (style == STATIC_STYLE_LOCAL ||
gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  gtk_entry_set_text (GTK_ENTRY (entry), "");
else
  {
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
  n = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  sprintf (token, "%d", gamgi_chem_orbital_sampling (1 + n, 1 + style));
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }
}

static void static_m (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
gamgi_enum style;
char token[GAMGI_ENGINE_TOKEN];
double radius;
int n, l, m;
int sampling;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
n = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) + 1;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
m = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) - 5;

/**********************
 * set default charge *
 **********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
sprintf (token, "%.*f", gamgi->gamgi->charge, GAMGI_CHEM_ORBITAL_CHARGE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * set default name *
 ********************/

gamgi_chem_orbital_name (n, l, m, token);
static_name (window, token);

/****************
 * set sampling *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
style = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (style == STATIC_STYLE_LOCAL) gtk_entry_set_text (GTK_ENTRY (entry), "");
else
  {
  sampling = gamgi_chem_orbital_sampling (n, 1 + style);
  sprintf (token, "%d", sampling);
  gtk_entry_set_text (GTK_ENTRY (entry), token);
  }

/****************************
 * set default radius, seed *
 ****************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
radius = gamgi_chem_orbital_radius (n, l, m, GAMGI_CHEM_ORBITAL_CHARGE);
sprintf (token, "%.*f", gamgi->gamgi->length, radius);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
sprintf (token, "%d", GAMGI_MATH_SEED);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/********************
 * set default axes *
 ********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), FALSE);
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

static void static_type_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 5);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_scope_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_LOCAL);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_volume_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *button;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_phase");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_PHASE_LOCAL);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_frame");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_FRAME_LOCAL);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_octants");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_AXES_LOCAL);
}

static void static_position_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/*********************
 * clean translation *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/******************
 * clean rotation *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_view_clean (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/***************
 * clean color *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/***************
 * clean scale *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_numbers (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *combo;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];

hbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "hbox_numbers");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_numbers");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  gtk_widget_set_sensitive (hbox, TRUE);
  
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_WIRED);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
  sprintf (token, "%.*E", 1, GAMGI_CHEM_ORBITAL_DENSITY);
  gtk_entry_set_text (GTK_ENTRY (entry), token);

  static_m (NULL, window);
  }
else
  {
  static_type_clean (window);
  static_scope_clean (window);
  gtk_widget_set_sensitive (hbox, FALSE);
  static_name (window, NULL);
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_AXES_LOCAL);
  }
}

static void static_octants (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *table;
GtkWidget *button;

table = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "table_octants");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_octants");

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  gtk_widget_set_sensitive (table, TRUE);
else
  {
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_1");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_2");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_3");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_4");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_5");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_6");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_7");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_8");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
  gtk_widget_set_sensitive (table, FALSE);
  }
}

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
 *                  STATIC_RELATIVE                *
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
 *                   STATIC_ORIGIN                 *
 *                                                 *
 * Reset the origin entries everytime the absolute *
 * or relative origin buttons are toggled.         *
 *                                                 *
 ***************************************************/

static void static_origin (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (entry, "");
}

/***************** internal function **************
 *                                                *
 *                    STATIC_ANGLE                *
 *                                                *
 * Reset the angle entries everytime the absolute *
 * or relative angle buttons are toggled.         *
 *                                                *
 **************************************************/

static void static_angle (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (entry, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (entry, "");
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_RESET                  *
 *                                                     *
 * Reset the text modify dialog, cleaning all entries. *
 *                                                     *
 *******************************************************/

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *button;

/********************
 * reset everything *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_orbital");
gtk_entry_set_text (GTK_ENTRY (entry), "");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_numbers");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

static_type_clean (window);
static_scope_clean (window);
static_volume_clean (window);
static_position_clean (window);
static_view_clean (window);
}

/****************** internal function **************
 *                                                 *
 *                     STATIC_LIST                 *
 *                                                 *
 ***************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_orbital");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /************************************
   * modify a single orbital directly *
   ************************************/

  gtk_label_set_text (GTK_LABEL (label), "Orbital");
  static_absolute (dialog);
  }
else
  {
  /************************************************
   * modify multiple orbitals previously selected *
   ************************************************/

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

/***********************************
 * quantum numbers, nucleus charge *
 ***********************************/

cache.new[STATIC_NUMBERS] = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_numbers");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  cache.new[STATIC_NUMBERS] = TRUE;
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
  cache.n = 1 + gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
  cache.l = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
  cache.m = gtk_combo_box_get_active (GTK_COMBO_BOX (combo)) - 5;
  }

cache.new[STATIC_CHARGE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_CHARGE] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.charge, 1.0, GAMGI_CHEM_ATOM_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid atomic charge", window);
    return FALSE;
    }
  }

/******************************************
 * style, density, sampling, radius, seed *
 ******************************************/

cache.new[STATIC_STYLE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < 2) { cache.new[STATIC_STYLE] = TRUE; cache.style = row + 1; }

cache.new[STATIC_DENSITY] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_DENSITY] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.density, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid probability density", window);
    return FALSE;
    }
  }

cache.new[STATIC_SAMPLING] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SAMPLING] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.sampling, 1, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid sampling data", window);
    return FALSE;
    }
  }

cache.new[STATIC_RADIUS] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RADIUS] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid sampling radius", window);
    return FALSE;
    }
  }

cache.new[STATIC_SEED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_SEED] = TRUE;
  if (gamgi_io_token_int_scan (name,
  &cache.seed, 1, INT_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid sampling seed", window);
    return FALSE;
    }
  }

/*******************************
 * phase, frame, octants, axes *
 *******************************/

cache.new[STATIC_PHASE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_phase");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < STATIC_PHASE_LOCAL)
  {
  cache.new[STATIC_PHASE] = TRUE;
  if (row == STATIC_PHASE_YES) cache.phase = TRUE;
  if (row == STATIC_PHASE_NO) cache.phase = FALSE;
  }

cache.new[STATIC_FRAME] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_frame");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < STATIC_FRAME_LOCAL)
  {
  cache.new[STATIC_FRAME] = TRUE;
  if (row == STATIC_FRAME_YES) cache.frame = TRUE;
  if (row == STATIC_FRAME_NO) cache.frame = FALSE;
  }

cache.new[STATIC_OCTANTS] = FALSE;
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_octants");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  {
  cache.new[STATIC_OCTANTS] = TRUE;
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_1");
  cache.octants[0] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_2");
  cache.octants[1] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_3");
  cache.octants[2] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_4");
  cache.octants[3] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_5");
  cache.octants[4] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_6");
  cache.octants[5] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_7");
  cache.octants[6] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_8");
  cache.octants[7] = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  }

cache.new[STATIC_AXES] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row < STATIC_AXES_LOCAL)
  {
  cache.new[STATIC_AXES] = TRUE;
  cache.axes = 1 + row;
  }

/******************************
 * translation: type, x, y, z *
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital translation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital translation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital translation", window);
    return FALSE;
    }
  }

/******************************
 * rotation: type, e1, e2, e3 *
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital rotation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital rotation", window);
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
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital rotation", window);
    return FALSE;
    }
  }

/**************
 * base color *
 **************/
  
cache.new[STATIC_BASE_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BASE_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.base_r, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_BASE_GREEN] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BASE_GREEN] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.base_g, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

cache.new[STATIC_BASE_BLUE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_BASE_BLUE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.base_b, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid color", window);
    return FALSE;
    }
  }

/***************
 * phase color *
 ***************/
  
cache.new[STATIC_PHASE_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_PHASE_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.phase_r, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    return FALSE;
    }
  }

cache.new[STATIC_PHASE_GREEN] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_PHASE_GREEN] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.phase_g, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    return FALSE;
    }
  }

cache.new[STATIC_PHASE_BLUE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_PHASE_BLUE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.phase_b, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid phase color", window);
    return FALSE;
    }
  }

/***************
 * frame color *
 ***************/
  
cache.new[STATIC_FRAME_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_FRAME_RED] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.frame_r, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    return FALSE;
    }
  }

cache.new[STATIC_FRAME_GREEN] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_FRAME_GREEN] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.frame_g, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    return FALSE;
    }
  }

cache.new[STATIC_FRAME_BLUE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_FRAME_BLUE] = TRUE;
  if (gamgi_io_token_float_scan (name,
  &cache.frame_b, 0.0, 1.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid frame color", window);
    return FALSE;
    }
  }

/*********
 * scale *
 *********/

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

static gamgi_bool static_apply (gamgi_orbital *orbital, gamgi_window *window)
{
char name[GAMGI_ENGINE_TOKEN];
gamgi_enum style, phase, frame;
gamgi_bool new;
double origin[3], origin_old[3];
double euler[3], euler_old[3];
double zero[3];
double charge, density, radius, axes, scale;
float base_r, base_g, base_b;
float phase_r, phase_g, phase_b;
float frame_r, frame_g, frame_b;
int octants[8];
int n, l, m, sampling, seed;
int i;

/**********************************
 * import old data (from orbital) *
 **********************************/

strcpy (name, orbital->object.name);
n = orbital->n;
m = orbital->m;
l = orbital->l;
charge = orbital->charge;

style = orbital->style;
density = orbital->density;
sampling = orbital->sampling;
radius = orbital->radius;
seed = orbital->seed;

phase = orbital->phase;
frame = orbital->frame;
for (i = 0; i < 8; i++)
  octants[i] = orbital->octants[i];
axes = orbital->axes;

gamgi_math_position_orbital_translation_get (orbital, origin_old);
if (cache.translation_absolute == TRUE)
  gamgi_math_vector_copy (origin_old, origin);
else
  gamgi_math_vector_zero (origin);

gamgi_math_position_orbital_rotation_get (orbital, euler_old);
if (cache.rotation_absolute == TRUE)
  gamgi_math_vector_copy (euler_old, euler);
else
  gamgi_math_vector_zero (euler);

base_r =  orbital->base_r;
base_g =  orbital->base_g;
base_b =  orbital->base_b;
phase_r = orbital->phase_r;
phase_g = orbital->phase_g;
phase_b = orbital->phase_b;
frame_r = orbital->frame_r;
frame_g = orbital->frame_g;
frame_b = orbital->frame_b;
scale = orbital->object.scale;

/********************************
 * import new data (from cache) *
 ********************************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);
if (cache.new[STATIC_NUMBERS] == TRUE) { n = cache.n; l = cache.l; m = cache.m; }
if (cache.new[STATIC_CHARGE] == TRUE) charge = cache.charge;

if (cache.new[STATIC_STYLE] == TRUE) style = cache.style;
if (cache.new[STATIC_DENSITY] == TRUE) density = cache.density;
if (cache.new[STATIC_SAMPLING] == TRUE) sampling = cache.sampling;
if (cache.new[STATIC_RADIUS] == TRUE) radius = cache.radius;
if (cache.new[STATIC_SEED] == TRUE) seed = cache.seed;

if (cache.new[STATIC_PHASE] == TRUE) phase = cache.phase;
if (cache.new[STATIC_FRAME] == TRUE) frame = cache.frame;
if (cache.new[STATIC_OCTANTS] == TRUE)
  for (i = 0; i < 8; i++) octants[i] = cache.octants[i];
if (cache.new[STATIC_AXES] == TRUE) axes = cache.axes;

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

if (cache.new[STATIC_BASE_RED] == TRUE) base_r = cache.base_r;
if (cache.new[STATIC_BASE_GREEN] == TRUE) base_g = cache.base_g;
if (cache.new[STATIC_BASE_BLUE] == TRUE) base_b = cache.base_b;
if (cache.new[STATIC_BASE_RED] == TRUE) phase_r = cache.phase_r;
if (cache.new[STATIC_BASE_GREEN] == TRUE) phase_g = cache.phase_g;
if (cache.new[STATIC_BASE_BLUE] == TRUE) phase_b = cache.phase_b;
if (cache.new[STATIC_BASE_RED] == TRUE) frame_r = cache.frame_r;
if (cache.new[STATIC_BASE_GREEN] == TRUE) frame_g = cache.frame_g;
if (cache.new[STATIC_BASE_BLUE] == TRUE) frame_b = cache.frame_b;
if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/*******************************************************
 * check if array data must be recalculated: changes   *
 * in phase, frame, but not axes require recalculation *
 *******************************************************/

new = FALSE;
if (orbital->n != n) new = TRUE;
if (orbital->l != l) new = TRUE;
if (orbital->m != m) new = TRUE;
if (orbital->style != style) new = TRUE;
if (orbital->density != density) new = TRUE;
if (orbital->sampling != sampling) new = TRUE;
if (orbital->radius != radius) new = TRUE;
if (orbital->seed != seed) new = TRUE;
if (orbital->phase != phase) new = TRUE;
if (orbital->frame != frame) new = TRUE;
if (orbital->octants[0] != octants[0]) new = TRUE;
if (orbital->octants[1] != octants[1]) new = TRUE;
if (orbital->octants[2] != octants[2]) new = TRUE;
if (orbital->octants[3] != octants[3]) new = TRUE;
if (orbital->octants[4] != octants[4]) new = TRUE;
if (orbital->octants[5] != octants[5]) new = TRUE;
if (orbital->octants[6] != octants[6]) new = TRUE;
if (orbital->octants[7] != octants[7]) new = TRUE;

/**********************************************
 * set default data, when changing array data *
 **********************************************/

if (new == TRUE && cache.new[STATIC_SAMPLING] == FALSE)
  sampling = gamgi_chem_orbital_sampling (n, style);
if (new == TRUE && cache.new[STATIC_RADIUS] == FALSE)
  radius = gamgi_chem_orbital_radius (n, l, m, charge);
  
/****************************
 * check data compatibility *
 ****************************/

if (style == GAMGI_MESA_SOLID &&
sampling > GAMGI_CHEM_ORBITAL_SLICES_MAX)
  {
  if (cache.local == TRUE)
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid sampling data", window);
  return FALSE;
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

/********************************************
 * export data (to orbital): reset position *
 ********************************************/

strcpy (orbital->object.name, name);
orbital->n = n;
orbital->m = m;
orbital->l = l;
orbital->charge = charge;

orbital->style = style;
orbital->density = density;
orbital->sampling = sampling;
orbital->radius = radius;
orbital->seed = seed;

orbital->phase = phase;
orbital->frame = frame;
for (i = 0; i < 8; i++)
  orbital->octants[i] = octants[i];
orbital->axes = axes;

orbital->base_r = base_r;
orbital->base_g = base_g;
orbital->base_b = base_b;
orbital->phase_r = phase_r;
orbital->phase_g = phase_g;
orbital->phase_b = phase_b;
orbital->frame_r = frame_r;
orbital->frame_g = frame_g;
orbital->frame_b = frame_b;
orbital->object.scale = scale;

if (new == TRUE)
  {
  /******************
   * rebuild arrays *
   ******************/

  orbital->dots[0] = 0;
  orbital->dots[1] = 0;
  orbital->dots[2] = 0;
  free (orbital->points);
  orbital->points = NULL;
  orbital->n_points = 0;
  free (orbital->lines);
  orbital->lines = NULL;
  orbital->n_lines = 0;
  gamgi_chem_orbital_create (orbital);
  }

gamgi_math_vector_zero (zero);
gamgi_math_position_orbital_rotation_set (orbital, zero);
gamgi_math_position_orbital_translation_set (orbital, zero);

gamgi_math_position_orbital_rotation_set (orbital, euler);
gamgi_math_position_orbital_translation_set (orbital, origin);

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

if (gamgi->orbitals == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of orbital objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_orbital");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid orbital identity", window);
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
for (dlist = gamgi->orbitals; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_ORBITAL dlist->data, window) == FALSE)
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
gamgi_orbital *orbital;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/********************************************
 * pick orbital and get name (possibly new) *
 *                                          *
 * check if focus object belongs to orbital *
 ********************************************/

cache.new[STATIC_NAME] = TRUE;
orbital = GAMGI_CAST_ORBITAL gamgi_gtk_object_number (dialog,
"entry_orbital", GAMGI_ENGINE_ORBITAL, window, cache.name);
if (orbital == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid orbital identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT orbital) == TRUE) focus = TRUE;

/***********************************************
 * collect data from dialog and modify orbital *
 ***********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (orbital, window) == FALSE) return;

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
static_reset (window);
static_numbers (NULL, window);
static_octants (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_orbital *orbital;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button;
char string[2 * GAMGI_ENGINE_TOKEN];
double rotate[9], euler[3];

orbital = GAMGI_CAST_ORBITAL object;

static_absolute (dialog);

/*******************************
 * set quantum numbers, charge *
 *******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_numbers");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_n");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), orbital->n - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_l");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), orbital->l);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_m");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), orbital->m + 5);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_charge");
sprintf (string, "%.*f", gamgi->gamgi->charge, orbital->charge);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/***********************************************
 * set orbital identification (after numbers!) *
 ***********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_orbital");
sprintf (string, "%s %d", orbital->object.name, orbital->object.number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/******************
 * set scope data *
 ******************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_density");
sprintf (string, "%.*E", 1, orbital->density);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_sampling");
sprintf (string, "%d", orbital->sampling);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_radius");
sprintf (string, "%.*f", gamgi->gamgi->length, orbital->radius);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_seed");
sprintf (string, "%d", orbital->seed);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*******************
 * set volume data *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), orbital->style - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_phase");
if (orbital->phase == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_frame");
if (orbital->frame == TRUE)
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_YES - 1);
else
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), GAMGI_GTK_NO - 1);

/***********
 * octants *
 ***********/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_octants");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_1");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[0]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_2");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[1]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_3");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[2]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_4");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[3]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_5");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[4]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_6");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[5]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_7");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[6]);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_8");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), orbital->octants[7]);

/************
 * set axes *
 ************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_axes");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), orbital->axes - 1);

/*********************
 * set position data *
 *********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
sprintf (string, "%.*f", gamgi->gamgi->length, orbital->origin[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
sprintf (string, "%.*f", gamgi->gamgi->length, orbital->origin[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
sprintf (string, "%.*f", gamgi->gamgi->length, orbital->origin[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

gamgi_math_quaternion_to_matrix (orbital->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[0]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[1]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
sprintf (string, "%.*f", gamgi->gamgi->angle, euler[2]);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/*****************
 * set view data *
 *****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->base_r);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->base_g);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_base_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->base_b);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_r);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_g);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_phase_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_b);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_red");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_r);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_green");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_g);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_frame_blue");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_b);
gtk_entry_set_text (GTK_ENTRY (entry), string);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, orbital->object.scale);
gtk_entry_set_text (GTK_ENTRY (entry), string);
}

/*************** external function *************
 *                                             *
 *         GAMGI_GTK_ORBITAL_MODIFY_PRESS      *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_orbital_modify_press (gamgi_window *window_mouse,
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
GAMGI_ENGINE_ORBITAL, FALSE, static_press);
}

/*************** external function *************
 *                                             *
 *           GAMGI_GTK_ORBITAL_MODIFY          *
 *                                             *
 ***********************************************/

void gamgi_gtk_orbital_modify (GtkWidget *widget, void *data)
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

dialog = gamgi_gtk_dialog_task0_create ("Orbital Modify", window);
window->action = GAMGI_GTK_ORBITAL_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*******************
 * Orbital objects *
 *******************/

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

label = gtk_label_new ("Orbital");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_orbital", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_orbital", entry);
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

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("Numbers");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_numbers), window);
g_object_set_data (G_OBJECT (dialog), "button_numbers", button);
gtk_widget_show (button);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "hbox_numbers", hbox_left);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Phase");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_phase), window);
g_object_set_data (G_OBJECT (dialog), "combo_phase", combo);
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

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Frame");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed", G_CALLBACK (static_frame), window);
g_object_set_data (G_OBJECT (dialog), "combo_frame", combo);
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

/***********
 * Octants *
 ***********/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (hbox_left), vbox_center, FALSE, FALSE, 0);
gtk_widget_show (vbox_center);

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

button = gtk_toggle_button_new_with_label ("Octants");
gtk_box_pack_start (GTK_BOX (vbox_top_top), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_octants), window);
g_object_set_data (G_OBJECT (dialog), "button_octants", button);
gtk_widget_show (button);

table = gtk_table_new (2, 4, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_left), table, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "table_octants", table);
gtk_widget_show (table);

button = gtk_toggle_button_new_with_label ("-3");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_1", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-4");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_2", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-2");
gtk_table_attach (GTK_TABLE (table), button, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_3", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("-1");
gtk_table_attach (GTK_TABLE (table), button, 1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_4", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+3");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 1, 2, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_5", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+4");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 1, 2, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_6", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+2");
gtk_table_attach (GTK_TABLE (table), button, 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 1);
g_object_set_data (G_OBJECT (dialog), "button_7", button);
gtk_widget_show (button);

button = gtk_toggle_button_new_with_label ("+1");
gtk_table_attach (GTK_TABLE (table), button, 3, 4, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 1);
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
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
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

vbox_center = gtk_vbox_new (TRUE, 10);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

/**********
 * Origin *
 **********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Translation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_origin), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_relative", button);
gtk_widget_show (button);

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

/*********
 * Angle *
 *********/

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("    Rotation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_angle), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_relative", button);
gtk_widget_show (button);

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
