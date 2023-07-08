/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_window.c
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
#include "gamgi_global.h"

#include <gdk/gdkx.h>
#include <gdk/gdkkeysyms.h>

#include "gamgi_engine_list.h"
#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_number.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_key.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_select.h"
#include "gamgi_gtk_create.h"
#include "gamgi_gtk_help_config.h"
#include "gamgi_gtk_help_current.h"
#include "gamgi_gtk_help_topic.h"
#include "gamgi_gtk_help_search.h"
#include "gamgi_gtk_help_tutorial.h"
#include "gamgi_gtk_help_dialog.h"
#include "gamgi_gtk_help_start.h"
#include "gamgi_gtk_text_copy.h"
#include "gamgi_gtk_text_config.h"
#include "gamgi_gtk_text_create.h"
#include "gamgi_gtk_text_link.h"
#include "gamgi_gtk_text_select.h"
#include "gamgi_gtk_text_modify.h"
#include "gamgi_gtk_text_remove.h"
#include "gamgi_gtk_orbital_copy.h"
#include "gamgi_gtk_orbital_create.h"
#include "gamgi_gtk_orbital_link.h"
#include "gamgi_gtk_orbital_select.h"
#include "gamgi_gtk_orbital_modify.h"
#include "gamgi_gtk_orbital_remove.h"
#include "gamgi_gtk_bond_config.h"
#include "gamgi_gtk_bond_create.h"
#include "gamgi_gtk_bond_select.h"
#include "gamgi_gtk_bond_link.h"
#include "gamgi_gtk_bond_modify.h"
#include "gamgi_gtk_bond_remove.h"
#include "gamgi_gtk_atom_copy.h"
#include "gamgi_gtk_atom_config.h"
#include "gamgi_gtk_atom_create.h"
#include "gamgi_gtk_atom_select.h"
#include "gamgi_gtk_atom_link.h"
#include "gamgi_gtk_atom_modify.h"
#include "gamgi_gtk_atom_measure.h"
#include "gamgi_gtk_atom_remove.h"
#include "gamgi_gtk_direction_copy.h"
#include "gamgi_gtk_direction_create.h"
#include "gamgi_gtk_direction_select.h"
#include "gamgi_gtk_direction_link.h"
#include "gamgi_gtk_direction_modify.h"
#include "gamgi_gtk_direction_measure.h"
#include "gamgi_gtk_direction_remove.h"
#include "gamgi_gtk_plane_copy.h"
#include "gamgi_gtk_plane_create.h"
#include "gamgi_gtk_plane_select.h"
#include "gamgi_gtk_plane_link.h"
#include "gamgi_gtk_plane_modify.h"
#include "gamgi_gtk_plane_measure.h"
#include "gamgi_gtk_plane_remove.h"
#include "gamgi_gtk_group_create.h"
#include "gamgi_gtk_group_copy.h"
#include "gamgi_gtk_group_select.h"
#include "gamgi_gtk_group_link.h"
#include "gamgi_gtk_group_modify.h"
#include "gamgi_gtk_group_measure.h"
#include "gamgi_gtk_group_remove.h"
#include "gamgi_gtk_molecule_create.h"
#include "gamgi_gtk_molecule_copy.h"
#include "gamgi_gtk_molecule_select.h"
#include "gamgi_gtk_molecule_link.h"
#include "gamgi_gtk_molecule_measure.h"
#include "gamgi_gtk_molecule_remove.h"
#include "gamgi_gtk_cluster_create.h"
#include "gamgi_gtk_cluster_copy.h"
#include "gamgi_gtk_cluster_select.h"
#include "gamgi_gtk_cluster_link.h"
#include "gamgi_gtk_cluster_measure.h"
#include "gamgi_gtk_cluster_remove.h"
#include "gamgi_gtk_cell_create.h"
#include "gamgi_gtk_cell_config.h"
#include "gamgi_gtk_cell_copy.h"
#include "gamgi_gtk_cell_select.h"
#include "gamgi_gtk_cell_link.h"
#include "gamgi_gtk_cell_modify.h"
#include "gamgi_gtk_cell_measure.h"
#include "gamgi_gtk_cell_remove.h"
#include "gamgi_gtk_assembly_create.h"
#include "gamgi_gtk_assembly_copy.h"
#include "gamgi_gtk_assembly_select.h"
#include "gamgi_gtk_assembly_link.h"
#include "gamgi_gtk_assembly_remove.h"
#include "gamgi_gtk_light_copy.h"
#include "gamgi_gtk_light_create.h"
#include "gamgi_gtk_light_select.h"
#include "gamgi_gtk_light_config.h"
#include "gamgi_gtk_light_link.h"
#include "gamgi_gtk_light_modify.h"
#include "gamgi_gtk_light_remove.h"
#include "gamgi_gtk_layer_create.h"
#include "gamgi_gtk_layer_copy.h"
#include "gamgi_gtk_layer_select.h"
#include "gamgi_gtk_layer_link.h"
#include "gamgi_gtk_layer_modify.h"
#include "gamgi_gtk_layer_remove.h"
#include "gamgi_gtk_layer_config.h"
#include "gamgi_gtk_window_create.h"
#include "gamgi_gtk_window_copy.h"
#include "gamgi_gtk_window_select.h"
#include "gamgi_gtk_window_link.h"
#include "gamgi_gtk_window_config.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_gtk_file_import.h"
#include "gamgi_gtk_file_export.h"
#include "gamgi_gtk_gamgi_about.h"
#include "gamgi_gtk_gamgi_reset.h"
#include "gamgi_gtk_gamgi_exit.h"
#include "gamgi_gtk_gamgi_config.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_copy.h"
#include "gamgi_gtk_link.h"
#include "gamgi_gtk_modify.h"
#include "gamgi_gtk_measure.h"
#include "gamgi_gtk_animate.h"
#include "gamgi_gtk_remove.h"
#include "gamgi_mesa_area.h"
#include "gamgi_mesa_start.h"
#include "gamgi_math_vector.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_scale.h"
#include "gamgi_global_selection.h"

static void static_menu_position (GtkMenu *menu,
int *x, int *y, gamgi_bool *push, void *data)
{
GtkRequisition size;
GtkWidget *button  = (GtkWidget *) data;
GtkAllocation allocation;
GdkWindow *drawable;

/********************************
 * set position for pop up menu *
 ********************************/

gtk_widget_size_request (button, &size);
drawable = gtk_widget_get_window (button);
gdk_window_get_origin (drawable, x, y);
gtk_widget_get_allocation (button, &allocation);
*x += allocation.x;
*y += allocation.y + size.height / 2 ;

/****************************
 * set size for pop up menu *
 ****************************/

gtk_widget_size_request (GTK_WIDGET (menu), &size);
size.width = GAMGI_GTK_WINDOW_WIDTH / 10 + 10;
gtk_widget_set_size_request (GTK_WIDGET (menu), size.width, size.height);

*push = TRUE;
}

static int static_menu (GtkWidget *button, GdkEventButton *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *menu;
int class;
int mask;

menu = (GtkWidget *) g_object_get_data (G_OBJECT (button), "menu");

if (event->type == GDK_BUTTON_PRESS)
  {
  /*************************************************************
   * convert 1,2,3 to BUTTON1_MASK, BUTTON2_MASK, BUTTON3_MASK *
   *************************************************************/

  mask = gamgi_gtk_dialog_button_mask (event->button);
  if (mask == gamgi->gamgi->button1)
    {
    window->class = FALSE;

    gtk_menu_popup (GTK_MENU (menu), NULL, NULL, static_menu_position, 
    button, event->button, event->time);
    }
  else if (mask == gamgi->gamgi->button2) 
    {
    class = GAMGI_POINTER_TO_INT (g_object_get_data (G_OBJECT (menu), "select"));

    if (class == GAMGI_ENGINE_GAMGI && window->class != FALSE)
      gamgi_gtk_object_focus_global (window);
    else if (class == GAMGI_ENGINE_WINDOW && window->class == class)
      gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window, window);
    else if (class == GAMGI_ENGINE_LAYER && window->class == class)
      gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);
    else { window->class = class; return TRUE; }

    window->class = FALSE;
    gamgi_gtk_dialog_beep ();
    }
  return TRUE;
  }

return FALSE;
}

static GtkWidget *static_item_create (char *name_label, char *name_color)
{
GtkWidget *item;
GtkWidget *label;
GdkColor  color;

item = gtk_menu_item_new ();

label = gtk_label_new (name_label);
gtk_misc_set_alignment (GTK_MISC (label), 0.1, 0.5);
gtk_widget_show (label);

gtk_container_add (GTK_CONTAINER (item), label);

if (name_color != NULL)
  {
  gdk_color_parse (name_color, &color);
  gamgi_gtk_dialog_color_fg (label, color);
  }

return item;
}

static GtkWidget *static_item_insert (GtkWidget *menu, char *string_item,
char *string_color, gamgi_callback2 function, gamgi_window *window)
{
GtkWidget *item;

item = static_item_create (string_item, string_color);
gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
gtk_widget_show (item);

if (function != NULL)
  g_signal_connect (item, "activate", G_CALLBACK (function), window);

return item;
}

static void static_button_menu (GtkWidget *table, GtkWidget *button, 
int x0, int  y0, int  x1, int y1)
{
gtk_table_attach_defaults (GTK_TABLE (table), button, x0, y0, x1, y1);
gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
gtk_widget_show (button);
}

static GtkWidget *static_button_tool (GtkWidget *hbox, char *name,
gamgi_callback2 handler_function, gamgi_window *window)
{
GtkWidget *button = gamgi_gtk_dialog_button_create (name, NULL);

gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (handler_function), window);
gtk_widget_show (button);

return button;
}

static int static_delete (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
gamgi_gtk_window_remove_confirm (widget, data);

/*******************************
 * TRUE: do not destroy window *
 *******************************/

return TRUE;
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window;
gamgi_layer *layer;

window = gamgi_engine_create_window ();
gamgi_engine_start_window (window);
gamgi_mesa_start_window (window, gamgi->window);

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, gamgi->layer);

/**********************************
 * start undo and save mechanisms *
 **********************************/

layer->undo = gamgi_engine_dlist_add_start (NULL);
layer->save = gamgi_engine_dlist_add_start (NULL);
layer->undo->data = layer;
layer->save->data = layer;

/****************************************
 * link layer to window and show window *
 ****************************************/

gamgi_engine_link_layer_window (layer, window);
gamgi_gtk_window_create (window, layer);
}

static void static_create (GtkWidget *widget, void *data)
{
static_ok (widget, data);
}

/*************************
 * bottom area callbacks *
 *************************/

static double static_ruler_position (double max,
double step, gamgi_window *window)
{
double position;
int sign;

/***********************
 * continous variation *
 ***********************/

gtk_ruler_get_range (GTK_RULER (window->ruler), NULL, NULL, &position, NULL);
position = position / GAMGI_GTK_RULER_MAX * max;
if (step < GAMGI_MATH_TOLERANCE) return position;

/******************
 * step variation *
 ******************/

sign = 0;
if (position < 0.0) sign = -1;
if (position > 0.0) sign = +1;
position = sign * position;

modf (position / step, &position);
position = (position + 1) * step;
if (position > max) position = max;
position = sign * position;

return position;
}

static void static_ruler_start (gamgi_window *window)
{
double lower, upper, max;

window->mode = FALSE;
gtk_ruler_get_range (GTK_RULER (window->ruler), &lower, &upper, NULL, &max);
gtk_ruler_set_range (GTK_RULER (window->ruler), lower, upper, 0, max);
gtk_widget_set_sensitive (window->ruler, FALSE);
}

static void static_ruler_press (GtkWidget *widget,
GdkEventButton *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
char string[GAMGI_ENGINE_TOKEN * 4];
double axis[3], translate[3];
double position;
int mask;

/*************************************************************
 * convert 1,2,3 to BUTTON1_MASK, BUTTON2_MASK, BUTTON3_MASK *
 *************************************************************/

mask = gamgi_gtk_dialog_button_mask (event->button);

/*****************************
 * 1) import data from ruler *
 * 2) apply action to object *
 * 3) export data to ruler   *
 *****************************/

switch (window->mode)
  {
  case GAMGI_GTK_ROTATE:
  position = static_ruler_position (gamgi->window->rotate_max,
  gamgi->window->rotate_step, window);

  gamgi_math_vector_absolute (axis, 0.0, 0.0, 0.0);
  if (mask == gamgi->gamgi->button1) axis[0] = 1.0;
  if (mask == gamgi->gamgi->button2) axis[1] = 1.0;
  if (mask == gamgi->gamgi->button3) axis[2] = 1.0;
  gamgi_mesa_rotate (window, window->focus, 
  position * GAMGI_MATH_DEG_RAD, axis);

  if (window->axis == mask)
    window->total = fmod (window->total + position, 360.0);
  else window->total = position;
  window->axis = mask;

  sprintf (string, "Total: %.*f Rotate: %.*f",
  gamgi->gamgi->angle, window->total,
  gamgi->gamgi->angle, position);
  break;

  case GAMGI_GTK_MOVE:
  position = static_ruler_position (gamgi->window->move_max,
  gamgi->window->move_step, window);

  gamgi_math_vector_absolute (translate, 0.0, 0.0, 0.0);
  if (mask == gamgi->gamgi->button1) translate[0] = position;
  if (mask == gamgi->gamgi->button2) translate[1] = position;
  if (mask == gamgi->gamgi->button3) translate[2] = position;
  gamgi_mesa_translate (window, window->focus, translate);

  if (window->axis == mask) window->total += position;
  else window->total = position;
  window->axis = mask;

  sprintf (string, "Total: %.*f Move: %.*f",
  gamgi->gamgi->length, window->total,
  gamgi->gamgi->length, position);
  break;

  case GAMGI_GTK_SCALE:
  position = static_ruler_position (gamgi->window->scale_max - 1.0,
  gamgi->window->scale_step, window);

  gamgi_mesa_scale (window, window->focus, position);

  if (position > 0) position += 1.0;
  else position = 1 / (1.0 - position);
  window->total *= position;

  sprintf (string, "Total: %.*f Scale: %.*f",
  GAMGI_MATH_DECIMAL_SCALE, window->total,
  GAMGI_MATH_DECIMAL_SCALE, position);
  break;

  default:
  return;
  }

gamgi_gtk_statusbar_message_show (window, string, NULL, -1);
}

static void static_ruler_motion (GtkWidget *widget,
GdkEventMotion *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
char string[GAMGI_ENGINE_TOKEN * 4];
double position;

switch (window->mode)
  {
  case GAMGI_GTK_ROTATE:
  position = static_ruler_position (gamgi->window->rotate_max, 
  gamgi->window->rotate_step, window);

  sprintf (string, "Total: %.*f Rotate: %.*f", 
  gamgi->gamgi->angle, window->total, 
  gamgi->gamgi->angle, position);
  break;

  case GAMGI_GTK_MOVE:
  position = static_ruler_position (gamgi->window->move_max,
  gamgi->window->move_step, window);

  sprintf (string, "Total: %.*f Move: %.*f", 
  gamgi->gamgi->length, window->total, 
  gamgi->gamgi->length, position);
  break;

  case GAMGI_GTK_SCALE:
  position = static_ruler_position (gamgi->window->scale_max - 1.0,
  gamgi->window->scale_step, window);
  if (position > 0) position += 1.0;
  else position = 1 / (1.0 - position);

  sprintf (string, "Total: %.*f Scale: %.*f", 
  GAMGI_MATH_DECIMAL_SCALE, window->total, 
  GAMGI_MATH_DECIMAL_SCALE, position);
  break;

  default:
  return;
  }

gamgi_gtk_statusbar_message_show (window, string, NULL, -1);
}

static void static_ruler_on (gamgi_window *window, gamgi_enum mode)
{
GdkColor color;
GtkWidget *rotate, *move, *scale;
GtkWidget *button = NULL;
GtkWidget *label;

rotate = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_rotate");
move = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_move");
scale = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_scale");

label = gtk_bin_get_child (GTK_BIN (rotate));
gamgi_gtk_statusbar_color (label, NULL, window);
label = gtk_bin_get_child (GTK_BIN (move));
gamgi_gtk_statusbar_color (label, NULL, window);
label = gtk_bin_get_child (GTK_BIN (scale));
gamgi_gtk_statusbar_color (label, NULL, window);

/*******************************************
 * mode must be only rotate, move or scale *
 *******************************************/

window->mode = mode;
gtk_widget_set_sensitive (window->ruler, TRUE);
if (mode == GAMGI_GTK_ROTATE) button = rotate;
if (mode == GAMGI_GTK_MOVE) button = move;
if (mode == GAMGI_GTK_SCALE) button = scale;

gamgi_gtk_dialog_color (&color,
GAMGI_GTK_ACTIVE_R, GAMGI_GTK_ACTIVE_G, GAMGI_GTK_ACTIVE_B);
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_dialog_color_fg (label, color);

static_ruler_motion (NULL, NULL, window);
}

static void static_ruler_off (gamgi_window *window)
{
GtkWidget *button;
GtkWidget *label;

button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_rotate");
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_statusbar_color (label, NULL, window);
button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_move");
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_statusbar_color (label, NULL, window);
button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_scale");
label = gtk_bin_get_child (GTK_BIN (button));
gamgi_gtk_statusbar_color (label, NULL, window);

static_ruler_start (window);
gamgi_gtk_statusbar_message_timeout (window);
}

static void static_axes (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *button;
GtkWidget *label;
GdkColor color;

button = (GtkWidget *) g_object_get_data (G_OBJECT (window->window), "button_axes");
label = gtk_bin_get_child (GTK_BIN (button));

if (window->axes == FALSE)
  {
  window->axes = TRUE;
  gamgi_gtk_dialog_color (&color,
  GAMGI_GTK_ACTIVE_R, GAMGI_GTK_ACTIVE_G, GAMGI_GTK_ACTIVE_B);
  gamgi_gtk_dialog_color_fg (label, color);
  }
else
  {
  window->axes = FALSE;
  gamgi_gtk_statusbar_color (label, NULL, window);
  }

/***************************
 * reset ruler accumulator *
 ***************************/

gamgi_gtk_statusbar_total (window);
}

static void static_rotate (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

window->total = 0.0;

if (window->mode == GAMGI_GTK_ROTATE)
  static_ruler_off (window);
else
  static_ruler_on (window, GAMGI_GTK_ROTATE);
}

static void static_move (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

window->total = 0.0;

if (window->mode == GAMGI_GTK_MOVE)
  static_ruler_off (window);
else
  static_ruler_on (window, GAMGI_GTK_MOVE);
}

static void static_scale (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

window->total = 1.0;

if (window->mode == GAMGI_GTK_SCALE)
  static_ruler_off (window);
else
  static_ruler_on (window, GAMGI_GTK_SCALE);
}

void gamgi_gtk_window_create (gamgi_window *window, gamgi_layer *layer)
{
GtkWidget *vbox_main;
GtkWidget *frame;
GtkWidget *table;
GtkWidget *button;
GtkWidget *menu;
GtkWidget *separator;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *event_box;
GtkWidget *item;
GtkAccelGroup *group;
GdkWindow *drawable;
char string[2 * GAMGI_ENGINE_TOKEN];

/**************** 
 * setup window *
 ****************/

window->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
sprintf (string, "%s %d", window->object.name, window->object.number);
gtk_window_set_title (GTK_WINDOW (window->window), string);

gtk_window_move (GTK_WINDOW (window->window), window->origin_x, window->origin_y);
gtk_window_set_default_size (GTK_WINDOW (window->window), window->width, window->height);
gtk_widget_set_size_request (window->window, GAMGI_GTK_WINDOW_WIDTH_MIN,
GAMGI_GTK_WINDOW_HEIGHT_MIN);

g_signal_connect (window->window, 
"delete_event", G_CALLBACK (static_delete), window);
g_signal_connect_after (window->window, 
"key_press_event", G_CALLBACK (gamgi_gtk_handle_key_press), window);

vbox_main = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (window->window), vbox_main);
gtk_widget_show (vbox_main);

/******************
 * setup top menu *
 ******************/

window->top = frame = gtk_frame_new (NULL);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
gtk_box_pack_start (GTK_BOX (vbox_main), frame, FALSE, TRUE, 0);
gtk_widget_show (frame); 

table = gtk_table_new (2, 10, TRUE);
gtk_container_add (GTK_CONTAINER (frame), table);
gtk_widget_show (table);

/********************
 * setup gamgi menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Gamgi", NULL);
static_button_menu (table, button, 0, 1, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_gamgi", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_GAMGI);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "About", NULL, gamgi_gtk_gamgi_about, window);
static_item_insert (menu, "Reset", NULL, gamgi_gtk_gamgi_reset, window);
item = static_item_insert (menu, "Exit", "red", gamgi_gtk_gamgi_exit, window);

/*********************************
 * add accelerator: press Ctr-C  *
 * to activate menu_item to call *
 *    gamgi_gtk_gamgi_exit       *
 *********************************/

group = gtk_accel_group_new ();
gtk_window_add_accel_group (GTK_WINDOW (window->window), group);
gtk_widget_add_accelerator (item, "activate",
group, GDK_C, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_gamgi_config, window);

/*******************
 * setup file menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("File", NULL);
static_button_menu (table, button, 1, 2, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_file", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) FALSE);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Import", NULL, gamgi_gtk_file_import, window);
static_item_insert (menu, "Export", NULL, gamgi_gtk_file_export, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/*********************
 * setup window menu *
 *********************/

button = gamgi_gtk_dialog_button_create ("Window", NULL);
static_button_menu (table, button, 2, 3, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_window", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_WINDOW);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, static_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_window_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_window_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_window_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_window_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_window_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_window_config, window);

/********************
 * setup layer menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Layer", NULL);
static_button_menu (table, button, 3, 4, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_layer", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_LAYER);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_layer_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_layer_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_layer_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_layer_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_layer_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_layer_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_layer_config, window);

/********************
 * setup light menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Light", NULL);
static_button_menu (table, button, 4, 5, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_light", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_LIGHT);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_light_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_light_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_light_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_light_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_light_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_light_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_light_config, window);

/***********************
 * setup assembly menu *
 ***********************/

button = gamgi_gtk_dialog_button_create ("Assembly", NULL);
static_button_menu (table, button, 5, 6, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_assembly", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_ASSEMBLY);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_assembly_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_assembly_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_assembly_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_assembly_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_assembly_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_assembly_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/********************
 * setup graph menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Graph", NULL);
static_button_menu (table, button, 6, 7, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_graph", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_GRAPH);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", "grey", gamgi_gtk_graph_create, window);
static_item_insert (menu, "Copy", "grey", gamgi_gtk_graph_copy, window);
static_item_insert (menu, "Select", "grey", gamgi_gtk_graph_select, window);
static_item_insert (menu, "Link", "grey", gamgi_gtk_graph_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_graph_modify, window);
static_item_insert (menu, "Remove", "grey", gamgi_gtk_graph_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/********************
 * setup shape menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Shape", NULL);
static_button_menu (table, button, 7, 8, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_shape", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_SHAPE);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", "grey", gamgi_gtk_shape_create, window);
static_item_insert (menu, "Copy", "grey", gamgi_gtk_shape_copy, window);
static_item_insert (menu, "Select", "grey", gamgi_gtk_shape_select, window);
static_item_insert (menu, "Link", "grey", gamgi_gtk_shape_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_shape_modify, window);
static_item_insert (menu, "Remove", "grey", gamgi_gtk_shape_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/********************
 * setup arrow menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Arrow", NULL);
static_button_menu (table, button, 8, 9, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_arrow", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_ARROW);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", "grey", gamgi_gtk_arrow_create, window);
static_item_insert (menu, "Copy", "grey", gamgi_gtk_arrow_copy, window);
static_item_insert (menu, "Select", "grey", gamgi_gtk_arrow_select, window);
static_item_insert (menu, "Link", "grey", gamgi_gtk_arrow_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_arrow_modify, window);
static_item_insert (menu, "Remove", "grey", gamgi_gtk_arrow_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/*******************
 * setup cell menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("Cell", NULL);
static_button_menu (table, button, 9, 10, 0, 1);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_cell", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_CELL);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_cell_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_cell_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_cell_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_cell_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_cell_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_cell_measure, window);
static_item_insert (menu, "Animate", "grey", gamgi_gtk_cell_animate, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_cell_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_cell_config, window);

/**********************
 * setup cluster menu *
 **********************/

button = gamgi_gtk_dialog_button_create ("Cluster", NULL);
static_button_menu (table, button, 9, 10, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_cluster", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_CLUSTER);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_cluster_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_cluster_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_cluster_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_cluster_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_cluster_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_cluster_measure, window);
static_item_insert (menu, "Animate", "grey", gamgi_gtk_cluster_animate, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_cluster_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/***********************
 * setup molecule menu *
 ***********************/

button = gamgi_gtk_dialog_button_create ("Molecule", NULL);
static_button_menu (table, button, 8, 9, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_molecule", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_MOLECULE);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_molecule_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_molecule_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_molecule_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_molecule_link, window);
static_item_insert (menu, "Modify", "grey", gamgi_gtk_molecule_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_molecule_measure, window);
static_item_insert (menu, "Animate", "grey", gamgi_gtk_molecule_animate, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_molecule_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/********************
 * setup group menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Group", NULL);
static_button_menu (table, button, 7, 8, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_group", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_GROUP);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_group_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_group_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_group_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_group_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_group_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_group_measure, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_group_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/********************
 * setup plane menu *
 ********************/

button = gamgi_gtk_dialog_button_create ("Plane", NULL);
static_button_menu (table, button, 6, 7, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_plane", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_PLANE);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_plane_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_plane_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_plane_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_plane_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_plane_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_plane_measure, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_plane_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/************************
 * setup direction menu *
 ************************/

button = gamgi_gtk_dialog_button_create ("Direction", NULL);
static_button_menu (table, button, 5, 6, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_direction", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_DIRECTION);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_direction_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_direction_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_direction_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_direction_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_direction_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_direction_measure, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_direction_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/*******************
 * setup atom menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("Atom", NULL);
static_button_menu (table, button, 4, 5, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_atom", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_ATOM);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_atom_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_atom_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_atom_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_atom_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_atom_modify, window);
static_item_insert (menu, "Measure", NULL, gamgi_gtk_atom_measure, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_atom_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_atom_config, window);

/*******************
 * setup bond menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("Bond", NULL);
static_button_menu (table, button, 3, 4, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_bond", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_BOND);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_bond_create, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_bond_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_bond_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_bond_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_bond_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_bond_config, window);

/**********************
 * setup orbital menu *
 **********************/

button = gamgi_gtk_dialog_button_create ("Orbital", NULL);
static_button_menu (table, button, 2, 3, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_orbital", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_ORBITAL);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_orbital_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_orbital_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_orbital_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_orbital_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_orbital_modify, window);
static_item_insert (menu, "Remove", NULL, gamgi_gtk_orbital_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", "grey", NULL, window);

/*******************
 * setup text menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("Text", NULL);
static_button_menu (table, button, 1, 2, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_text", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) GAMGI_ENGINE_TEXT);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Create", NULL, gamgi_gtk_text_create, window);
static_item_insert (menu, "Copy", NULL, gamgi_gtk_text_copy, window);
static_item_insert (menu, "Select", NULL, gamgi_gtk_text_select, window);
static_item_insert (menu, "Link", NULL, gamgi_gtk_text_link, window);
static_item_insert (menu, "Modify", NULL, gamgi_gtk_text_modify, window);
static_item_insert (menu, "Remove", "red", gamgi_gtk_text_remove, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_text_config, window);

/*******************
 * setup help menu *
 *******************/

button = gamgi_gtk_dialog_button_create ("Help", "red");
static_button_menu (table, button, 0, 1, 1, 2);
g_signal_connect (button, "event", G_CALLBACK (static_menu), window);
g_object_set_data (G_OBJECT (window->window), "button_help", button);

menu = gtk_menu_new ();
g_object_set_data (G_OBJECT (menu), "select", (void *) FALSE);
g_object_set_data (G_OBJECT (button), "menu", menu);

static_item_insert (menu, "Current", NULL, gamgi_gtk_help_current, window);
static_item_insert (menu, "Topic", NULL, gamgi_gtk_help_topic, window);
static_item_insert (menu, "Search", NULL, gamgi_gtk_help_search, window);
static_item_insert (menu, "Tutorial", NULL, gamgi_gtk_help_tutorial, window);
static_item_insert (menu, "Tips", NULL, gamgi_gtk_help_dialog_tips, window);
/*
static_item_insert (menu, "Progress", NULL, gamgi_gtk_statusbar_mode, window);
*/
static_item_insert (menu, "Start", "red", gamgi_gtk_help_start, window);

separator = gtk_menu_item_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
gtk_widget_show (separator);

static_item_insert (menu, "Config", NULL, gamgi_gtk_help_config, window);

/****************
 * drawing area *
 ****************/

window->area = gamgi_mesa_area_create (window);
if (window->area == NULL) return;
gtk_box_pack_start (GTK_BOX (vbox_main), window->area, TRUE, TRUE, 0);

/******************
 * statusbar area *
 ******************/

window->bottom = frame = gtk_frame_new (NULL);
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
gtk_box_pack_end (GTK_BOX (vbox_main), frame, FALSE, FALSE, 0);
gtk_widget_show (frame);

vbox = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (frame), vbox);
gtk_widget_show (vbox);

hbox = gtk_hbox_new (FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);
gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

window->statusbar_message = gtk_statusbar_new ();
gtk_box_pack_start (GTK_BOX (hbox), window->statusbar_message, TRUE, TRUE, 0);
window->context_message = gtk_statusbar_get_context_id (GTK_STATUSBAR 
(window->statusbar_message), "message");
gtk_statusbar_set_has_resize_grip  (GTK_STATUSBAR (window->statusbar_message), FALSE);
gtk_widget_show (window->statusbar_message);

window->statusbar_focus = gtk_statusbar_new ();
gtk_widget_set_size_request (window->statusbar_focus, 140, -1);
gtk_box_pack_start (GTK_BOX (hbox), window->statusbar_focus, FALSE, FALSE, 0);
window->context_focus = gtk_statusbar_get_context_id (GTK_STATUSBAR
(window->statusbar_focus), "object");
gtk_statusbar_set_has_resize_grip  (GTK_STATUSBAR (window->statusbar_focus), FALSE);
gtk_widget_show (window->statusbar_focus);

window->statusbar_layer = gtk_statusbar_new ();
gtk_widget_set_size_request (window->statusbar_layer, 140, -1);
gtk_box_pack_start (GTK_BOX (hbox), window->statusbar_layer, FALSE, FALSE, 0);
window->context_layer = gtk_statusbar_get_context_id (GTK_STATUSBAR
(window->statusbar_layer), "layer");
gtk_statusbar_set_has_resize_grip  (GTK_STATUSBAR (window->statusbar_layer), FALSE);
gtk_widget_show (window->statusbar_layer);

window->statusbar_time = gtk_statusbar_new ();
gtk_widget_set_size_request (window->statusbar_time, 80, -1);
gtk_box_pack_start (GTK_BOX (hbox), window->statusbar_time, FALSE, FALSE, 0);
window->context_time = gtk_statusbar_get_context_id (GTK_STATUSBAR
(window->statusbar_time), "time");
gtk_statusbar_set_has_resize_grip  (GTK_STATUSBAR (window->statusbar_time), FALSE);
gtk_widget_show (window->statusbar_time);

window->progress_bar = gtk_progress_bar_new ();
gtk_widget_set_size_request (window->progress_bar, 80, -1);
gtk_box_pack_end (GTK_BOX (hbox), window->progress_bar, FALSE, FALSE, 2);

/*********************************
 * command area: entry get focus *
 *********************************/

hbox = gtk_hbox_new (FALSE, 0);
gtk_container_set_border_width (GTK_CONTAINER (hbox), 2);
gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
gtk_widget_show (hbox);

window->entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox), window->entry, TRUE, TRUE, 0);
gtk_widget_grab_focus (window->entry);
gtk_widget_show (window->entry);

/**************
 * ruler area *
 **************/

window->medium = vbox = gtk_vbox_new (FALSE, 0);
gtk_box_pack_end (GTK_BOX (vbox_main), vbox, FALSE, FALSE, 0);
gtk_widget_show (vbox);

event_box = gtk_event_box_new ();
gtk_box_pack_end (GTK_BOX (vbox), event_box, FALSE, FALSE, 3);
gtk_widget_show (event_box);

window->ruler = gtk_hruler_new ();
gtk_container_add (GTK_CONTAINER (event_box), window->ruler);
gtk_ruler_set_metric (GTK_RULER (window->ruler), GTK_INCHES);
gtk_ruler_set_range (GTK_RULER (window->ruler), -GAMGI_GTK_RULER_MAX, 
GAMGI_GTK_RULER_MAX, 0, 10000);
gtk_widget_set_sensitive (window->ruler, FALSE);
gtk_widget_show (window->ruler);

g_signal_connect (event_box, "button_press_event",
G_CALLBACK (static_ruler_press), window);
g_signal_connect (event_box, "motion-notify-event",
G_CALLBACK (static_ruler_motion), window);

static_ruler_start (window);

/*************
 * tool area *
 *************/

hbox = gtk_hbox_new (FALSE, 0);
gtk_box_pack_end (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
gtk_widget_show (hbox);

button = static_button_tool (hbox, "Axes", static_axes, window);
g_object_set_data (G_OBJECT (window->window), "button_axes", button);

button = static_button_tool (hbox, "Rotate", static_rotate, window);
g_object_set_data (G_OBJECT (window->window), "button_rotate", button);

button = static_button_tool (hbox, "Move", static_move, window);
g_object_set_data (G_OBJECT (window->window), "button_move", button);

button = static_button_tool (hbox, "Scale", static_scale, window);
g_object_set_data (G_OBJECT (window->window), "button_scale", button);

static_button_tool (hbox, "Undo", gamgi_gtk_history_undo, window);
static_button_tool (hbox, "Save", gamgi_gtk_history_save, window);

/****************************
 *         start time       *
 *                          *
 * set layer as current     *
 * layer and current object *
 ****************************/

gamgi_gtk_statusbar_time_timeout (NULL);
gamgi_gtk_object_layer_local (layer, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer, window);

/***********************************************************
 * 1) show and raise new window to the top; 2) hide        *
 * main,tool,status bars; 3) update top menu label colors, *
 * to indicate which objects have lists currently active   *
 ***********************************************************/
 
gtk_widget_show (window->window);
drawable = gtk_widget_get_window (window->window);
gdk_window_raise (drawable);

if (window->top_flag == FALSE) gtk_widget_hide (window->top);
if (window->medium_flag == FALSE) gtk_widget_hide (window->medium);
if (window->bottom_flag == FALSE) gtk_widget_hide (window->bottom);

gamgi_global_selection_update_objects (window);
}

void gamgi_gtk_window_create_menus (gamgi_window *window)
{
/*****************
 * restart menus *
 *****************/

if (window->top_flag == -1)
  {
  window->top_flag = 1;
  gtk_widget_show (window->top);
  }
if (window->medium_flag == -1)
  {
  window->medium_flag = 1;
  gtk_widget_show (window->medium);
  }
if (window->bottom_flag == -1)
  {
  window->bottom_flag = 1;
  gtk_widget_show (window->bottom);
  }
}

void gamgi_gtk_window_create_progress (gamgi_window *window)
{
/*******************************
 * restart status bar progress *
 *******************************/

if (window->demo_flag != 0)
  {
/* GTK 2
  gtk_progress_set_activity_mode (GTK_PROGRESS (window->progress_bar), FALSE);
  gtk_widget_hide (window->progress_bar);
  g_source_remove (window->timer_progress);
  window->timer_progress = 0;
  window->demo_flag = 0;
*/
  }
}

void gamgi_gtk_window_create_data (gamgi_window *window)
{
char string[2 * GAMGI_ENGINE_TOKEN];

/******************************
 * restart window information *
 ******************************/

strcpy (window->object.name, "Window");
window->object.number = gamgi_engine_number_window_get ();
sprintf (string, "%s %d", window->object.name, window->object.number);
gtk_window_set_title (GTK_WINDOW (window->window), string);

window->action = FALSE;
window->class = FALSE;
if (window->axes == TRUE) static_axes (NULL, window);
static_ruler_off (window);
}

gamgi_layer *gamgi_gtk_window_create_layer (gamgi_window *window)
{
gamgi_layer *layer;

/*****************
 * restart layer *
 *****************/

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, gamgi->layer);

gamgi_engine_link_layer_window (layer, window);
gamgi_gtk_object_layer_local (layer, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer, window);

/************************************
 * restart undo and save mechanisms *
 ************************************/

layer->undo = gamgi_engine_dlist_add_start (NULL);
layer->save = gamgi_engine_dlist_add_start (NULL);
layer->undo->data = layer;
layer->save->data = layer;

return layer;
}
