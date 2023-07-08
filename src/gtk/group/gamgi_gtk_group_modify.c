/*************************************************
 *
 * $GAMGI/src/gtk/group/gamgi_gtk_group_modify.c
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
#include "gamgi_engine_copy.h"
#include "gamgi_engine_copy_out.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_filter.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_number.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_group_link.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_draw.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_text.h"
#include "gamgi_chem_polytope.h"
#include "gamgi_chem_atom.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_position.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_global_selection.h"

/*******************************************
 * enumerations: GAMGI property, GTK combo *
 *******************************************/

enum { STATIC_NAME, STATIC_REFERENCE, STATIC_ATOM, STATIC_STRUCTURE,
STATIC_FILTER, STATIC_VISIBLE, STATIC_COPLANAR, STATIC_ELEMENT, STATIC_NUMBER,
STATIC_TRANSLATION_X, STATIC_TRANSLATION_Y, STATIC_TRANSLATION_Z,
STATIC_ROTATION_E1, STATIC_ROTATION_E2, STATIC_ROTATION_E3, STATIC_STYLE,
STATIC_RED, STATIC_GREEN, STATIC_BLUE, STATIC_SCALE };

enum { STATIC_REFERENCE_CONTAINER, STATIC_REFERENCE_POLYTOPE, STATIC_REFERENCE_LOCAL };
enum { STATIC_STRUCTURE_COPY, STATIC_STRUCTURE_LINK, STATIC_STRUCTURE_NONE };
enum { STATIC_FILTER_CLASS, STATIC_FILTER_CHILD, STATIC_FILTER_IDENTICAL,
STATIC_FILTER_NONE };
enum { STATIC_NUMBER_NUMBER, STATIC_NUMBER_RADIUS };
enum { STATIC_STYLE_WIRED, STATIC_STYLE_SOLID, STATIC_STYLE_LOCAL };
enum { STATIC_COLOR_YES, STATIC_COLOR_NO };

static struct {
gamgi_bool local;
gamgi_bool new[STATIC_SCALE + 1];
char name[GAMGI_ENGINE_TOKEN];
gamgi_atom *atom;
gamgi_enum reference, structure, filter;
double visible, coplanar, radius;
int element, number;
gamgi_bool translation_absolute, rotation_absolute;
double translation[3], rotation[3];
gamgi_enum style, color;
float red, green, blue;
double scale; } cache;

static void static_text (gamgi_text *text, gamgi_window *window)
{
gamgi_dlist *dlist;
char line[GAMGI_ENGINE_LINE];
double center[3];
gamgi_enum style, font;

if (cache.new[STATIC_STYLE] == TRUE)
  {
  if (cache.style == STATIC_STYLE_WIRED)
    { style = GAMGI_MESA_WIRED; font = 1; }

  if (cache.style == STATIC_STYLE_SOLID)
    { style = GAMGI_MESA_SOLID; font = 1 + GAMGI_IO_FONTS_WIRED; }
  
  if (gamgi_mesa_text_center (font, text->string,
  text->length, center, line) == FALSE)
    {
    if (cache.local == TRUE)
      gamgi_gtk_dialog_message_create ("Warning", line, window);
    return;
    }
  gamgi_math_vector_copy (center, text->center);

  gamgi_mesa_text_font_decrease (text);
  text->style = style;
  text->font = font;
  gamgi_mesa_text_font_increase (text, NULL);
  }

if (cache.new[STATIC_RED] == TRUE) text->red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) text->green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) text->blue = cache.blue;

for (dlist = text->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);
}

static void static_orbital (gamgi_orbital *orbital, gamgi_window *window)
{
gamgi_dlist *dlist;

for (dlist = orbital->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);
}

static void static_bond (gamgi_bond *bond, gamgi_window *window)
{
gamgi_dlist *dlist;

if (cache.new[STATIC_STYLE] == TRUE)
  {
  if (cache.style == STATIC_STYLE_WIRED)
    bond->draw = gamgi_mesa_bond_draw_line;
  if (cache.style == STATIC_STYLE_SOLID)
    bond->draw = gamgi_mesa_bond_draw_cylinder;
  }

if (cache.color == STATIC_COLOR_YES) bond->color = TRUE;
if (cache.new[STATIC_RED] == TRUE) bond->red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) bond->green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) bond->blue = cache.blue;

for (dlist = bond->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);

for (dlist = bond->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (GAMGI_CAST_ORBITAL dlist->data, window);
}

static void static_atom (gamgi_atom *atom, gamgi_window *window)
{
gamgi_dlist *dlist;

if (cache.new[STATIC_STYLE] == TRUE)
  {
  if (cache.style == STATIC_STYLE_WIRED)
    atom->draw = gamgi_mesa_atom_draw_cross;
  if (cache.style == STATIC_STYLE_SOLID)
    atom->draw = gamgi_mesa_atom_draw_sphere;
  }

if (cache.new[STATIC_RED] == TRUE) atom->red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) atom->green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) atom->blue = cache.blue;

for (dlist = atom->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);

for (dlist = atom->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (GAMGI_CAST_ORBITAL dlist->data, window);

for (dlist = atom->bond_start; dlist != NULL; dlist = dlist->next)
  static_bond (GAMGI_CAST_BOND dlist->data, window);
}

static void static_direction (gamgi_direction *direction, gamgi_window *window)
{
gamgi_dlist *dlist;

if (cache.new[STATIC_RED] == TRUE) direction->red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) direction->green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) direction->blue = cache.blue;

for (dlist = direction->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);

for (dlist = direction->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (GAMGI_CAST_ORBITAL dlist->data, window);

for (dlist = direction->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (GAMGI_CAST_ATOM dlist->data, window);
}

static void static_plane (gamgi_plane *plane, gamgi_window *window)
{
gamgi_dlist *dlist;

if (cache.new[STATIC_RED] == TRUE) plane->red = cache.red;
if (cache.new[STATIC_GREEN] == TRUE) plane->green = cache.green;
if (cache.new[STATIC_BLUE] == TRUE) plane->blue = cache.blue;

for (dlist = plane->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);

for (dlist = plane->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (GAMGI_CAST_ORBITAL dlist->data, window);

for (dlist = plane->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (GAMGI_CAST_ATOM dlist->data, window);

for (dlist = plane->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (GAMGI_CAST_DIRECTION dlist->data, window);
}

static void static_group (gamgi_group *group, gamgi_window *window)
{
gamgi_dlist *dlist;

if (group->reference == GAMGI_CHEM_POLYTOPE)
  {
  if (cache.new[STATIC_STYLE] == TRUE) group->faces = cache.style;
  if (cache.new[STATIC_RED] == TRUE) group->red = cache.red;
  if (cache.new[STATIC_GREEN] == TRUE) group->green = cache.green;
  if (cache.new[STATIC_BLUE] == TRUE) group->blue = cache.blue;
  }

for (dlist = group->text_start; dlist != NULL; dlist = dlist->next)
  static_text (GAMGI_CAST_TEXT dlist->data, window);

for (dlist = group->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (GAMGI_CAST_ORBITAL dlist->data, window);

for (dlist = group->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (GAMGI_CAST_ATOM dlist->data, window);

for (dlist = group->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (GAMGI_CAST_DIRECTION dlist->data, window);

for (dlist = group->plane_start; dlist != NULL; dlist = dlist->next)
  static_plane (GAMGI_CAST_PLANE dlist->data, window);

for (dlist = group->group_start; dlist != NULL; dlist = dlist->next)
  static_group (GAMGI_CAST_GROUP dlist->data, window);
}

static void static_remove (gamgi_group *group)
{
gamgi_dlist *dlist;

dlist = group->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->group_start = NULL;
group->group_end = NULL;
 
dlist = group->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data); 
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->plane_start = NULL;
group->plane_end = NULL;

dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->direction_start = NULL;
group->direction_end = NULL;

dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->atom_start = NULL;
group->atom_end = NULL;

dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->orbital_start = NULL;
group->orbital_end = NULL;

dlist = group->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
group->text_start = NULL;
group->text_end = NULL;
}

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
const char *name;
int row;

/***************************************************
 * when the group entry is empty, look for a group *
 ***************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return GAMGI_ENGINE_GROUP;

/*******************************************
 * otherwise select which object to select *
 *******************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row == STATIC_REFERENCE_CONTAINER) return GAMGI_ENGINE_GROUP;
if (row == STATIC_REFERENCE_POLYTOPE) return GAMGI_ENGINE_ATOM;
if (row == STATIC_REFERENCE_LOCAL) return GAMGI_ENGINE_GROUP;

/********************************
 * this case should never occur *
 ********************************/

return FALSE;
}

static void static_number (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
gtk_entry_set_text (GTK_ENTRY (entry), "");
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

static void static_color (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry_red, *entry_green, *entry_blue;
GtkWidget *label_red, *label_green, *label_blue;
int row;

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

entry_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
entry_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
entry_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
label_red = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_red");
label_green = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_green");
label_blue = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_blue");

if (row == STATIC_COLOR_YES)
  {
  gtk_widget_set_sensitive (entry_red, TRUE);
  gtk_widget_set_sensitive (entry_green, TRUE);
  gtk_widget_set_sensitive (entry_blue, TRUE);
  gtk_widget_set_sensitive (label_red, TRUE);
  gtk_widget_set_sensitive (label_green, TRUE);
  gtk_widget_set_sensitive (label_blue, TRUE);
  }
else
  {
  gtk_entry_set_text (GTK_ENTRY (entry_red), "");
  gtk_entry_set_text (GTK_ENTRY (entry_green), "");
  gtk_entry_set_text (GTK_ENTRY (entry_blue), "");
  gtk_widget_set_sensitive (entry_red, FALSE);
  gtk_widget_set_sensitive (entry_green, FALSE);
  gtk_widget_set_sensitive (entry_blue, FALSE);
  gtk_widget_set_sensitive (label_red, FALSE);
  gtk_widget_set_sensitive (label_green, FALSE);
  gtk_widget_set_sensitive (label_blue, FALSE);
  }
}

static void static_start_polytope (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;
GtkWidget *button;
char token[GAMGI_ENGINE_TOKEN];

/************************************
 * start Structure, Filter policies *
 ************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STRUCTURE_COPY);

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_FILTER_CLASS);

/******************************
 * start Number of neighbours *
 ******************************/

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_number");
gtk_button_clicked (GTK_BUTTON (button));

/*******************************************
 * start face tolerance: Visible, Coplanar *
 *******************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_visible");
sprintf (token, "%.*E", 2, GAMGI_CHEM_POLYTOPE_VISIBLE);
gtk_entry_set_text (GTK_ENTRY (entry), token);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_coplanar");
sprintf (token, "%.*E", 2, GAMGI_CHEM_POLYTOPE_COPLANAR);
gtk_entry_set_text (GTK_ENTRY (entry), token);

/**********************
 * start Style: Solid *
 **********************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_SOLID);
}

static void static_start_position (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;

/***********************
 * start Position page *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_start_view (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

/*******************
 * start View page *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_LOCAL);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_reference (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *vbox;
GtkWidget *combo;
GtkWidget *label;
GtkWidget *entry;
gamgi_bool *sensitive;
int row;

/************
 * get data *
 ************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_view");
if (row == STATIC_REFERENCE_CONTAINER)
  gtk_widget_hide (vbox);
else
  { gtk_widget_show (vbox); static_start_view (window); }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_color");
vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
if (row == STATIC_REFERENCE_POLYTOPE)
  {
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_central");
  gtk_widget_set_sensitive (label, TRUE);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom");
  gtk_widget_set_sensitive (label, TRUE);
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  gtk_widget_set_sensitive (entry, TRUE);

  gtk_widget_show (vbox);
  static_start_polytope (window);
  sensitive[STATIC_COLOR_NO] = TRUE;
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_COLOR_NO);
  }
else
  {
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_central");
  gtk_widget_set_sensitive (label, FALSE);
  label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_atom");
  gtk_widget_set_sensitive (label, FALSE);
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  gtk_widget_set_sensitive (entry, FALSE);

  gtk_widget_hide (vbox);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_COLOR_YES);
  sensitive[STATIC_COLOR_NO] = FALSE;
  }

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
if (row == STATIC_REFERENCE_LOCAL)
  { gtk_widget_show (vbox); static_start_position (window); }
else
  gtk_widget_hide (vbox);
}

static void static_translation (GtkWidget *widget, void *data)
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

static void static_rotation (GtkWidget *widget, void *data)
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

static void static_reset (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
GtkWidget *entry;

/********************
 * clean group name *
 ********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*************************
 * reset group reference *
 *************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_LOCAL);
static_reference (NULL, window);

/***********************
 * reset Position data *
 ***********************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/*******************
 * reset View data *
 *******************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_LOCAL);

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
gtk_entry_set_text (GTK_ENTRY (entry), "");
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
gtk_entry_set_text (GTK_ENTRY (entry), "");

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

/******************** internal function *****************
 *                                                      *
 *                        STATIC_LIST                   *
 *                                                      *
 ********************************************************/

static void static_list (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *label, *button;
gamgi_bool *sensitive;

sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_reference");

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_group");
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_list");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == FALSE)
  {
  /**********************************
   * modify a single group directly *
   **********************************/

  gtk_label_set_text (GTK_LABEL (label), "Group");
  sensitive[STATIC_REFERENCE_CONTAINER] = TRUE;
  sensitive[STATIC_REFERENCE_POLYTOPE] = TRUE;
  static_absolute (dialog);
  }
else
  {
  /**********************************************
   * modify multiple groups previously selected *
   **********************************************/

  gtk_label_set_text (GTK_LABEL (label), "Name");
  sensitive[STATIC_REFERENCE_CONTAINER] = FALSE;
  sensitive[STATIC_REFERENCE_POLYTOPE] = FALSE;
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
GtkWidget *vbox;
gamgi_bool visible;
char token[GAMGI_ENGINE_TOKEN];
const char *name;
int row;

/***********************
 * get group reference *
 ***********************/

cache.new[STATIC_REFERENCE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
cache.reference = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (cache.reference != STATIC_REFERENCE_LOCAL) cache.new[STATIC_REFERENCE] = TRUE;

/********************
 * get central atom *
 ********************/

cache.new[STATIC_ATOM] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
if (gtk_widget_is_sensitive (entry) == TRUE)
  {
  cache.new[STATIC_ATOM] = TRUE;
  cache.atom = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (dialog,
  "entry_atom", GAMGI_ENGINE_ATOM, NULL);
  if (cache.atom == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid central atom", window);
    return FALSE;
    }
  }

/**********************************************
 * scope: structure, filter, visible, coplanar *
 ***********************************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_scope");
visible = gtk_widget_get_visible (vbox);

cache.new[STATIC_STRUCTURE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_structure");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE)
  {
  cache.new[STATIC_STRUCTURE] = TRUE;
  cache.structure = 1 + row;
  }

cache.new[STATIC_FILTER] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_filter");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (visible == TRUE)
  {
  cache.new[STATIC_FILTER] = TRUE;
  cache.filter = 1 + row;
  }

cache.new[STATIC_VISIBLE] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_visible");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE)
  {
  cache.new[STATIC_VISIBLE] = TRUE;
  if (gamgi_io_token_double_scan (name, &cache.visible,
  0.0, GAMGI_CHEM_POLYTOPE_VISIBLE_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid visible face tolerance", window);
    return FALSE;
    }
  }

cache.new[STATIC_COPLANAR] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_coplanar");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE)
  {
  if (gamgi_io_token_double_scan (name, &cache.coplanar,
  0.0, GAMGI_CHEM_POLYTOPE_COPLANAR_MAX) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid coplanar face tolerance", window);
    return FALSE;
    }
  }

/***********************************
 * get element for neighbour atoms *
 ***********************************/

cache.element = -1;
cache.new[STATIC_ELEMENT] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_element");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ELEMENT] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE ||
  gamgi_chem_atom_number (token, &cache.element) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid neighbours element", window);
    return FALSE;
    }
  }

/*********************************************************************
 * number of neighbours or maximum radius, counting from the nearest *
 * one, so for example radius = 1.1 means the range [r, 1.1r] where  *
 * r is the distance from central to the nearest neighbour           *
 *********************************************************************/

cache.number = 0; cache.radius = DBL_MAX;
cache.new[STATIC_NUMBER] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_number");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NUMBER] = TRUE;
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_number");
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
    {
    if (gamgi_io_token_int_scan (name,
    &cache.number, 2, INT_MAX) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid neighbours number", window);
      return FALSE;
      }
    }
  else
    {
    if (gamgi_io_token_double_scan (name,
    &cache.radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error",
      "Invalid neighbours maximum radius", window);
      return FALSE;
      }
    }
  }

/******************************
 * translation: type, x, y, z *
 ******************************/

vbox = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "vbox_position");
visible = gtk_widget_get_visible (vbox);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_translation_absolute");
if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)) == TRUE)
  cache.translation_absolute = TRUE;
else
  cache.translation_absolute = FALSE;

cache.new[STATIC_TRANSLATION_X] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
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
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
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
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
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
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
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
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
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
if (visible == TRUE && gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_ROTATION_E3] = TRUE;
  if (gamgi_io_token_double_scan (name,
  &cache.rotation[2], 0.0, 360.0) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid angle data", window);
    return FALSE;
    }
  }

/*****************************************
 * style, color, red, green, blue, scale *
 *****************************************/

cache.new[STATIC_STYLE] = FALSE;
combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
if (row != STATIC_STYLE_LOCAL)
  {
  cache.new[STATIC_STYLE] = TRUE;
  cache.style = row;
  }

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
cache.color = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

cache.new[STATIC_RED] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_RED] = TRUE;
  if (gamgi_io_token_float_scan (name, &cache.red, 0.0, 1.0) == FALSE)
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
  if (gamgi_io_token_float_scan (name, &cache.green, 0.0, 1.0) == FALSE)
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
  if (gamgi_io_token_float_scan (name, &cache.blue, 0.0, 1.0) == FALSE)
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

static gamgi_bool static_apply (gamgi_group *group, gamgi_window *window)
{
gamgi_group *group_old;
gamgi_object *parent;
gamgi_dlist *dlist;
GtkWidget *dialog;
GtkWidget *text;
GtkWidget *entry;
gamgi_enum style;
char name[GAMGI_ENGINE_TOKEN];
char string[GAMGI_ENGINE_STRING];
char line[GAMGI_ENGINE_LINE] = "";
double euler[3], euler_old[3], origin[3], origin_old[3];
double scale;
float red, green, blue;
int out[4];

/**************************
 * import data from group *
 **************************/

strcpy (name, group->object.name);

gamgi_math_position_group_translation_get (group, origin_old);
if (cache.translation_absolute == TRUE)
  gamgi_math_vector_copy (origin_old, origin);
else
  gamgi_math_vector_zero (origin);

gamgi_math_position_group_rotation_get (group, euler_old);
if (cache.rotation_absolute == TRUE)
  gamgi_math_vector_copy (euler_old, euler);
else
  gamgi_math_vector_zero (euler);

style = group->faces;
if (cache.color == STATIC_COLOR_YES)
  { red = group->red; green = group->green; blue = group->blue; }
else
  { red = -1; green = -1; blue = -1; }
scale = group->object.scale;

/******************
 * apply new data *
 ******************/

if (cache.new[STATIC_NAME] == TRUE) strcpy (name, cache.name);

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

if (cache.new[STATIC_STYLE] == TRUE) style = cache.style;
if (cache.color == STATIC_COLOR_YES)
  {
  if (cache.new[STATIC_RED] == TRUE) red = cache.red;
  if (cache.new[STATIC_GREEN] == TRUE) green = cache.green;
  if (cache.new[STATIC_BLUE] == TRUE) blue = cache.blue;
  }
else
  {
  if (cache.new[STATIC_ELEMENT] == TRUE)
    {
    red = gamgi->atom->red[cache.element];
    green = gamgi->atom->green[cache.element];
    blue = gamgi->atom->blue[cache.element];
    }
  }

if (cache.new[STATIC_SCALE] == TRUE) scale = cache.scale;

/****************************
 * check data compatibility *
 ****************************/

dlist = NULL;
if (cache.reference == STATIC_REFERENCE_POLYTOPE)
  {
  /**********************
   * check central atom *
   **********************/

  if (gamgi_engine_find_object (GAMGI_CAST_OBJECT cache.atom,
  window->focus) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid searching scope for central atom", window);
    return FALSE;
    }

  if (gamgi_engine_find_object (GAMGI_CAST_OBJECT cache.atom,
  GAMGI_CAST_OBJECT group) == TRUE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid searching scope for central atom", window);
    return FALSE;
    }

  /**********************************
   * check central atom, neighbours *
   **********************************/

  if (gamgi_engine_find_object (window->focus,
  GAMGI_CAST_OBJECT window->layer) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid searching scope for neighbour atoms", window);
    return FALSE;
    }

  if (gamgi_engine_find_object (window->focus,
  GAMGI_CAST_OBJECT group) == TRUE)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid searching scope for neighbour atoms", window);
    return FALSE;
    }

  dlist = gamgi_engine_dlist_atom_object (window->focus, NULL);
  dlist = gamgi_engine_filter_atom (dlist, 0, cache.filter,
  GAMGI_ENGINE_GROUP, window->focus);
  if (dlist == NULL)
    {
    gamgi_gtk_dialog_message_create ("Error",
    "Invalid searching scope for neighbour atoms", window);
    return FALSE;
    }
  }

/***************************************
 * Save current layer as undo buffer   *
 * layer so it can be recovered later. *
 ***************************************/

if (cache.local == TRUE)
  gamgi_gtk_history_update (window->layer, window->layer->undo, GAMGI_ENGINE_UNDO);

if (cache.reference != STATIC_REFERENCE_LOCAL)
  {
  /**************************************************
   * reset group: free arrays, remove child objects *
   **************************************************/

  static_remove (group);

  free (group->points);
  free (group->loops);
  group->points = NULL;
  group->loops = NULL;
  group->n_points = 0;
  group->n_loops = 0;

  group->origin[0] = 0.0;
  group->origin[1] = 0.0;
  group->origin[2] = 0.0;
  group->center[0] = 0.0;
  group->center[1] = 0.0;
  group->center[2] = 0.0;
  group->quaternion[0] = 0.0;
  group->quaternion[1] = 0.0;
  group->quaternion[2] = 0.0;
  group->quaternion[3] = 1.0;

  group->faces = FALSE;
  }

if (cache.reference == STATIC_REFERENCE_CONTAINER)
  {
  /****************
   * set new data *
   ****************/

  strcpy (group->object.name, name);
  group->reference = GAMGI_CHEM_CONTAINER;

  /*************************************
   *  launch a Group Link dialog       *
   *  containing already the name and  *
   *  number of the new group.         *
   *                                   *
   *   The old dialog is removed       *
   *   before creating the new one     *
   *                                   *
   * no need to redraw gl_area because *
   * an empty group is invisible       *
   *                                   *
   * Insert group's name and number    *
   *  in new dialog's group entry      *
   *************************************/

  gamgi_gtk_group_link (NULL, window);

  dialog = window->dialog0;
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
  sprintf (string, "%s %d", group->object.name, group->object.number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

if (cache.reference == STATIC_REFERENCE_POLYTOPE)
  {
  /********************************************************
   * When the polytope algorithm fails to produce a valid *
   * polytope, we need to go back to the old Group object *
   ********************************************************/

  group_old = gamgi_engine_create_group ();
  gamgi_engine_start_group (group_old);
  gamgi_engine_copy_out_group (group, group_old);
  parent = group->object.object;

  /****************************************
   * each polytope group is automatically *
   * linked to the central atom parent    *
   ****************************************/

  gamgi_engine_unlink_group (group);

  /****************
   * set new data *
   ****************/

  strcpy (group->object.name, name);
  group->reference = GAMGI_CHEM_POLYTOPE;
  group->faces = style;
  group->red = red;
  group->green = green;
  group->blue = blue;
  group->object.scale = scale;

  /***********************************
   * create and initialize text      *
   * widget, to show Polytope report *
   ***********************************/

  text = gamgi_gtk_dialog_text_create ();

  /********************************************************
   * apply Polytope algorithm: independent is the minimum *
   * number of independent atom positions that were found *
   ********************************************************/

  if (gamgi_chem_polytope (dlist, group, cache.atom, FALSE, cache.structure,
  cache.visible, cache.coplanar, cache.element, cache.number, cache.radius,
  out, text) == FALSE || out[GAMGI_MATH_POLYTOPE_1] == 1)
    {
    gamgi_engine_copy_out_group (group_old, group);
    gamgi_engine_link_object_object (GAMGI_CAST_OBJECT group, parent);
    gamgi_engine_remove_group (group_old);
    gtk_widget_destroy (text);

    gamgi_gtk_dialog_message_create ("Error",
    "Invalid atom coordinates", window);
    gtk_widget_destroy (text);
    return FALSE;
    }
  else if (out[GAMGI_MATH_POLYTOPE_2] == 1)
    {
    sprintf (string, "Created one Direction, ");
    strcat (line, string);
    }
  else if (out[GAMGI_MATH_POLYTOPE_3] == 1)
    {
    sprintf (string, "Created one Plane, ");
    strcat (line, string);
    }
  else if (out[GAMGI_MATH_POLYTOPE_4] == 1)
    {
    gamgi_engine_remove_group (group_old);

    dialog = gamgi_gtk_dialog_report_create (text,
    "Group Modify Polytope", GAMGI_GTK_GROUP_MODIFY_POLYTOPE, window);
    gtk_widget_show (dialog);
    }

  if (out[GAMGI_MATH_POLYTOPE_2] + out[GAMGI_MATH_POLYTOPE_3] > 0)
    {
    gamgi_engine_remove_group (group);
    gamgi_engine_remove_group (group_old);
    gtk_widget_destroy (text);

    strcat (line, "due to improper atom coordinates");
    gamgi_gtk_dialog_message_create ("Warning", line, window);
    }
  }

if (cache.reference == STATIC_REFERENCE_LOCAL)
  {
  strcpy (group->object.name, name);

  if (group->reference == GAMGI_CHEM_CONTAINER) static_group (group, window);
  else
    {
    group->faces = style;
    group->red = red;
    group->green = green;
    group->blue = blue;
    }

  group->object.scale = scale;

  gamgi_math_position_group_rotation_set (group, euler);
  gamgi_math_position_group_translation_set (group, origin);
  }

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

if (gamgi->groups == NULL)
  { gamgi_gtk_dialog_message_create ("Error",
  "Empty list of group objects", window); return; }

/****************************
 * Pick name (possibly new) *
 ****************************/

cache.new[STATIC_NAME] = FALSE;
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == TRUE)
  {
  cache.new[STATIC_NAME] = TRUE;
  if (gamgi_io_token_alpha_scan (name,
  cache.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
    {
    gamgi_gtk_dialog_message_create ("Error", "Invalid group identity", window);
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
for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  if (static_apply (GAMGI_CAST_GROUP dlist->data, window) == FALSE)
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
gamgi_group *group;
GtkWidget *dialog = window->dialog0;
gamgi_bool focus;

cache.local = TRUE;

/******************************************
 * pick group and get name (possibly new) *
 *                                        *
 * check if focus object belongs to group *
 ******************************************/

cache.new[STATIC_NAME] = TRUE;
group = GAMGI_CAST_GROUP gamgi_gtk_object_number (dialog,
"entry_group", GAMGI_ENGINE_GROUP, window, cache.name);
if (group == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid group identity", window);
  return;
  }

focus = FALSE;
if (gamgi_engine_find_object (window->focus,
GAMGI_CAST_OBJECT group) == TRUE) focus = TRUE;

/********************************************
 * collect data from dialog and modify cell *
 ********************************************/

if (static_grab (window) == FALSE) return;
if (static_apply (group, window) == FALSE) return;

/*************************************************************
 * when focus object is affected, change it to current layer *
 *************************************************************/

if (focus == TRUE)
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);

/*****************************************
 * redraw gl_area image and reset dialog *
 *****************************************/

gtk_widget_queue_draw (window->area);
if (cache.reference == STATIC_REFERENCE_LOCAL)
  static_reset (window);
}

/********************* internal function ******************
 *                                                        *
 *                         STATIC_OK                      *
 *                                                        *
 * This is the function that actually modifies a group or *
 * list of groups, triggered when the user presses Ok.    *
 **********************************************************/

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
GtkWidget *notebook;
GtkWidget *combo;
GtkRequisition size;

/***************************************************
 * Set notebook current size as its maximum size:  *
 * this is needed because pages are shown/hidden,  *
 * changing the currently needed size for dialog.  *
 * Fixing the dialog size only partly solves the   *
 * problem because the page size can still change. *
 ***************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_widget_size_request (GTK_WIDGET (notebook), &size);
gtk_widget_set_size_request (notebook, size.width, size.height);

/********************************************
 * hide pages AFTER measuring notebook size *
 ********************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_LOCAL);
static_reference (NULL, window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_group *group;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *combo;
char string[2 * GAMGI_ENGINE_TOKEN];
double origin[3], euler[3];
gamgi_enum class;
gamgi_bool *sensitive;

class = static_class (window);
if (class == GAMGI_ENGINE_GROUP)
  {
  group = GAMGI_CAST_GROUP object;

  static_absolute (dialog);

  /****************************
   * get Group identification *
   ****************************/

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
  sprintf (string, "%s %d", object->name, object->number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  /***************************
   * set Type combo to Local *
   ***************************/

  combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_reference");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_REFERENCE_LOCAL);
  static_reference (NULL, window);

  /***********************************
   * set Group position, orientation *
   ***********************************/

  gamgi_math_position_group_translation_get (group, origin);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_x");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_y");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_z");
  sprintf (string, "%.*f", gamgi->gamgi->length, origin[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  gamgi_math_position_group_rotation_get (group, euler);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e1");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[0]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e2");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[1]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_e3");
  sprintf (string, "%.*f", gamgi->gamgi->angle, euler[2]);
  gtk_entry_set_text (GTK_ENTRY (entry), string);

  /*********************************
   * set Group style, color, scale *
   *********************************/

  if (group->reference == GAMGI_CHEM_POLYTOPE)
    {
    combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), group->faces);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_red");
    sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, group->red);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_green");
    sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, group->green);
    gtk_entry_set_text (GTK_ENTRY (entry), string);

    entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_blue");
    sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, group->blue);
    gtk_entry_set_text (GTK_ENTRY (entry), string);
    }

  if (group->reference == GAMGI_CHEM_CONTAINER)
    {
    combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_style");
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_STYLE_LOCAL);

    sensitive = (gamgi_bool *) g_object_get_data (G_OBJECT (dialog), "sensitive_color");
    combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_color");
    sensitive[STATIC_COLOR_NO] = TRUE;
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), STATIC_COLOR_NO);
    }

  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_scale");
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_SCALE, group->object.scale);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

else if (class == GAMGI_ENGINE_ATOM)
  {
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_atom");
  sprintf (string, "%s %d", object->name, object->number);
  gtk_entry_set_text (GTK_ENTRY (entry), string);
  }

}

/*************** external function *************
 *                                             *
 *         GAMGI_GTK_GROUP_MODIFY_PRESS        *
 *                                             *
 *                                             *
 ***********************************************/

void gamgi_gtk_group_modify_press (gamgi_window *window_mouse,
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
static_class (window_dialog), TRUE, static_press);
}

/******************* external function **************
 *                                                  *
 *                GAMGI_GTK_GROUP_MODIFY            *
 *                                                  *
 * Creates the dialog window used to create groups. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_group_modify (GtkWidget *widget, void *data)
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

dialog = gamgi_gtk_dialog_task0_create ("Group Modify", window);
window->action = GAMGI_GTK_GROUP_MODIFY;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Group objects *
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

label = gtk_label_new ("Group");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_group", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_group", entry);
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
 * Type page *
 *************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Type ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_type", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 20);
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

label = gtk_label_new ("Reference");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_reference), window);
g_object_set_data (G_OBJECT (dialog), "combo_reference", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Container", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Polytope", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Local", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (3);
g_object_set_data (G_OBJECT (dialog), "sensitive_reference", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

/************
 * Polytope *
 ************/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

/****************
 * Central Atom *
 ****************/

label = gtk_label_new ("Central");
gtk_box_pack_start (GTK_BOX (vbox_top_top), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_central", label);
gtk_widget_show (label);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Atom");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_atom", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_atom", entry);
gtk_widget_show (entry);

/************************
 * Polytope: Scope page *
 ************************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" Scope ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_scope", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 10);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*********************
 * Structure, Filter *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Structure");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_structure", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Copy", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Link", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Filter");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "combo_filter", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Class", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Child", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Identical", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "None", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*********************
 * Visible, Coplanar *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 15);
gtk_box_pack_end (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Visible");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "entry_visible", entry);
gtk_widget_show (entry);

/**********
 * column *
 **********/

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Coplanar");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (dialog), "entry_coplanar", entry);
gtk_widget_show (entry);

/**************
 * Neighbours *
 **************/

vbox_top_top = gtk_vbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("Neighbours");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (label);

label = gtk_label_new ("Element");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_SHRINK, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_object_set_data (G_OBJECT (dialog), "entry_element", entry);
gtk_widget_show (entry);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_table_attach (GTK_TABLE (table), hbox_left, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
gtk_widget_show (hbox_left);

button = gtk_radio_button_new_with_label (NULL, "Number");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_number), window);
g_object_set_data (G_OBJECT (dialog), "button_number", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Radius");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_number), window);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 1, 2, GTK_SHRINK, GTK_FILL, 0, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_number", entry);
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
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_translation), window);
g_object_set_data (G_OBJECT (dialog), "button_translation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_translation), window);
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

label = gtk_label_new ("Rotation");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

button = gtk_radio_button_new_with_label (NULL, "Absolute");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_rotation), window);
g_object_set_data (G_OBJECT (dialog), "button_rotation_absolute", button);
gtk_widget_show (button);

button = gtk_radio_button_new_with_label (
gtk_radio_button_get_group (GTK_RADIO_BUTTON (button)), "Relative");
gtk_box_pack_start (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "toggled", G_CALLBACK (static_rotation), window);
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

/***********************
 * Polytope: View page *
 ***********************/

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new (" View ");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
g_object_set_data (G_OBJECT (dialog), "vbox_view", vbox_page);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/****************
 * Style, Scale *
 ****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 20);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Style");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), combo, FALSE, FALSE, 0);
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

hbox_left_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_left), hbox_left_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left_left);

label = gtk_label_new ("Scale");
gtk_box_pack_start (GTK_BOX (hbox_left_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_scale", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_scale", entry);
gtk_widget_show (entry);

/*********
 * Color *
 *********/

vbox_top_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), vbox_top_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Color");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_color", label);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_color), window);
g_object_set_data (G_OBJECT (dialog), "combo_color", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Yes", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "No", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

sensitive = gamgi_gtk_dialog_sensitive_create (2);
g_object_set_data (G_OBJECT (dialog), "sensitive_color", sensitive);
gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combo),
renderer, gamgi_gtk_dialog_sensitive_scan, sensitive, free);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (1, 6, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("R");
gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_red", label);
gtk_widget_show (label);

label = gtk_label_new ("G");
gtk_table_attach (GTK_TABLE (table), label, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_green", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
g_object_set_data (G_OBJECT (dialog), "label_blue", label);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_red", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_5);
g_object_set_data (G_OBJECT (dialog), "entry_green", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
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
