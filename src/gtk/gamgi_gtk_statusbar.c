/******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_statusbar.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include <time.h>

#include "gamgi_gtk_dialog.h"

/***************** internal function ******************
 *                                                    *
 *                    STATIC_SHOW                     *
 *                                                    *
 * Every message posted in the statusbar goes through *
 * this function. First remove the old message (even  *
 * if it does not exist) and then write the new one.  *
 ******************************************************/

static void static_show (GtkWidget *statusbar, int context, char *text)
{
gtk_statusbar_pop (GTK_STATUSBAR (statusbar), GPOINTER_TO_INT (context));
gtk_statusbar_push (GTK_STATUSBAR (statusbar), GPOINTER_TO_INT (context), text);
}

/*************** internal function ************
 *                                            *
 *                 STATIC_ALIGN               *
 *                                            *
 * All messages posted in centered sections   *
 * of the statusbar go through this function. *
 * Align the label in its middle position.    *
 **********************************************/

static void static_align (GtkWidget *statusbar, int context, char *text)
{
GtkWidget *hbox;
GtkWidget *label;
GList *list;

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (statusbar));

list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);
static_show (statusbar, context, text);
}

/************ internal function *********
 *                                      *
 *              STATIC_TIMER            *
 *                                      *
 * Remove a timer. All statusbar timers *
 * are removed through this function.   *
 ****************************************/

static void static_timer (int *timer)
{
if (*timer != 0) g_source_remove (*timer);
*timer = 0;
}

/***************** external function *****************
 *                                                   *
 *          GAMGI_GTK_STATUSBAR_ACCUMULATOR          *
 *                                                   *
 * This function resets the ruler accumulator to 0.0 *
 * (addition neutral element) for Rotate,Mode modes  *
 * and 1.0 (product neutral element) for Scale mode. *
 *****************************************************/

void gamgi_gtk_statusbar_total (gamgi_window *window)
{
/***************************
 * reset ruler accumulator *
 ***************************/

if (window->mode == GAMGI_GTK_SCALE) window->total = 1.0;
else window->total = 0.0;
}

/*********** internal function ************
 *                                        *
 *               STATIC_COLOR             *
 *                                        *
 * Select text color for messages written *
 * in a given section of the statusbar.   *
 ******************************************/

void gamgi_gtk_statusbar_color (GtkWidget *label,
char *color_name, gamgi_window *window)
{
GdkColor color;
GtkWidget *hbox;
GtkWidget *label_time;
GtkStyle *style;
GList *list;

if (color_name == NULL)
  {
  /****************************************************
   * use the default color, taken from the time label *
   ****************************************************/

  hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_time));
  list = gtk_container_get_children (GTK_CONTAINER (hbox));
  label_time = GTK_WIDGET (list->data);
  g_list_free (list);

  style = gtk_style_copy (gtk_widget_get_style (label_time));
  gtk_widget_set_style (label, style);
  g_object_unref (style);
  }
else
  {
  gdk_color_parse (color_name, &color);
  gamgi_gtk_dialog_color_fg (label, color);
  }
}

/************************** external function ******************
 *                                                             *
 *                GAMGI_GTK_STATUSBAR_MESSAGE_TIMEOUT          *
 *                                                             *
 * This function is executed when the time for the statusbar   *
 * message section runs out. Remove the timer and the message. *
 ***************************************************************/

gamgi_bool gamgi_gtk_statusbar_message_timeout (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

static_show (window->statusbar_message, window->context_message, "");
static_timer (&window->timer_message);

return TRUE;
}

/*********************** external function ***********************
 *                                                               *
 *                 GAMGI_GTK_STATUSBAR_FOCUS_TIMEOUT             *
 *                                                               *
 * This function is executed when the time for the statusbar     *
 * focus section runs out. Remove the timer but not the message. *
 * The text color becomes the default (the time section color).  *
 *****************************************************************/

gamgi_bool gamgi_gtk_statusbar_focus_timeout (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkStyle *style;
GtkWidget *hbox;
GtkWidget *label;
GList *list;

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_time));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

style = gtk_style_copy (gtk_widget_get_style (label));

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_focus));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gtk_widget_set_style (label, style);
g_object_unref (style);

static_timer (&window->timer_focus);

return TRUE;
}

/*********************** external function ***********************
 *                                                               *
 *                 GAMGI_GTK_STATUSBAR_LAYER_TIMEOUT             *
 *                                                               *
 * This function is executed when the time for the statusbar     *
 * layer section runs out. Remove the timer but not the message. *
 * The text color becomes the default (the time section color).  *
 *****************************************************************/

gamgi_bool gamgi_gtk_statusbar_layer_timeout (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkStyle *style;
GtkWidget *hbox;
GtkWidget *label;
GList *list;

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_time));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

style = gtk_style_copy (gtk_widget_get_style (label));

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_layer));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gtk_widget_set_style (label, style);
g_object_unref (style);

static_timer (&window->timer_layer);

return TRUE;
}

/*********************** external function ************************
 *                                                                *
 *                 GAMGI_GTK_STATUSBAR_TIME_TIMEOUT               *
 *                                                                *
 * This function is executed when the time for the statusbar      *
 * time section runs out. Recalculate the time, keep the timer,   *
 * and update the new time in all the windows. Use default color. *
 ******************************************************************/

gamgi_bool gamgi_gtk_statusbar_time_timeout (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
time_t time_data;
char string[GAMGI_ENGINE_TOKEN];

time (&time_data);
strftime (string, 10, "%H : %M", localtime (&time_data));

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  /**********************************************
   * When loading files with multiple windows,  *
   * gamgi windows must be created immediately  *
   * but gtk windows can be created only in the *
   * end, therefore listed gamgi windows might  *
   * have undefined gtk window and status bar.  *
   **********************************************/

  if (window->window == NULL) continue;
  static_align (window->statusbar_time, window->context_time, string);
  }

return TRUE;
}

/*********************** external function ********************
 *                                                            *
 *                GAMGI_GTK_STATUSBAR_MESSAGE_SHOW            *
 *                                                            *
 * Post new message (not centered), remove old timer (even if *
 * it does not exist) and add a new one. Use default color.   *
 **************************************************************/

void gamgi_gtk_statusbar_message_show (gamgi_window *window, 
char *text, char *color, int time)
{
GtkWidget *hbox;
GtkWidget *label;
GList *list;

static_show (window->statusbar_message, window->context_message, text);
hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_message));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gamgi_gtk_statusbar_color (label, color, window);

if (time > 0)
  {
  static_timer (&window->timer_message);
  window->timer_message = g_timeout_add (time,
  gamgi_gtk_statusbar_message_timeout, window);
  }
}

/********************** external function *********************
 *                                                            *
 *                GAMGI_GTK_STATUSBAR_FOCUS_SHOW              *
 *                                                            *
 * Post new message (centered), remove old timer (even if     *
 * it does not exist) and add a new one. Use highlight color. *
 **************************************************************/

void gamgi_gtk_statusbar_focus_show (gamgi_window *window)
{
gamgi_object *object;
GtkWidget *hbox;
GtkWidget *label;
char string[GAMGI_ENGINE_STRING];
GList *list;

switch (gamgi->focus)
  {
  case GAMGI_ENGINE_TEXT:
  sprintf (string, "Text List");
  break;

  case GAMGI_ENGINE_ORBITAL:
  sprintf (string, "Orbital List");
  break;

  case GAMGI_ENGINE_BOND:
  sprintf (string, "Bond List");
  break;

  case GAMGI_ENGINE_ATOM:
  sprintf (string, "Atom List");
  break;

  case GAMGI_ENGINE_DIRECTION:
  sprintf (string, "Direction List");
  break;

  case GAMGI_ENGINE_PLANE:
  sprintf (string, "Plane List");
  break;

  case GAMGI_ENGINE_GROUP:
  sprintf (string, "Group List");
  break;

  case GAMGI_ENGINE_MOLECULE:
  sprintf (string, "Molecule List");
  break;

  case GAMGI_ENGINE_CLUSTER:
  sprintf (string, "Cluster List");
  break;

  case GAMGI_ENGINE_CELL:
  sprintf (string, "Cell List");
  break;

  case GAMGI_ENGINE_ARROW:
  sprintf (string, "Arrow List");
  break;

  case GAMGI_ENGINE_SHAPE:
  sprintf (string, "Shape List");
  break;

  case GAMGI_ENGINE_GRAPH:
  sprintf (string, "Graph List");
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  sprintf (string, "Assembly List");
  break;

  case GAMGI_ENGINE_LIGHT:
  sprintf (string, "Light List");
  break;

  case GAMGI_ENGINE_LAYER:
  sprintf (string, "Layer List");
  break;

  case GAMGI_ENGINE_WINDOW:
  sprintf (string, "Window List");
  break;

  case GAMGI_ENGINE_GAMGI:
  sprintf (string, "Gamgi 1");
  break;

  default:
  object = window->focus;
  sprintf (string, "%s %d", object->name, object->number);
  }

/************************************************
 * show new current object with highlight color *
 ************************************************/

static_align (window->statusbar_focus, window->context_focus, string);
hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_focus));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gamgi_gtk_statusbar_color (label, "red", window);

/********************************************
 * after some time, change to default color *
 ********************************************/

static_timer (&window->timer_focus);
window->timer_focus = g_timeout_add (GAMGI_GTK_STATUSBAR_TIME_FOCUS, 
gamgi_gtk_statusbar_focus_timeout, window);

/***************************
 * reset ruler accumulator *
 ***************************/

gamgi_gtk_statusbar_total (window);
}

/********************** external function *********************
 *                                                            *
 *                GAMGI_GTK_STATUSBAR_LAYER_SHOW              *
 *                                                            *
 * Post new message (centered), remove old timer (even if     *
 * it does not exist) and add a new one. Use highlight color. *
 **************************************************************/

void gamgi_gtk_statusbar_layer_show (gamgi_window *window)
{
gamgi_layer *layer;
GtkWidget *hbox;
GtkWidget *label;
char string[GAMGI_ENGINE_STRING];
GList *list;

/***********************************************
 * show new current layer with highlight color *
 ***********************************************/

layer = window->layer;
sprintf (string, "%s %d", layer->object.name, layer->object.number);

static_align (window->statusbar_layer, window->context_layer, string);
hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_layer));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

gamgi_gtk_statusbar_color (label, "red", window);

/********************************************
 * after some time, change to default color *
 ********************************************/

static_timer (&window->timer_layer);
window->timer_layer = g_timeout_add (GAMGI_GTK_STATUSBAR_TIME_LAYER, 
gamgi_gtk_statusbar_layer_timeout, window);
}

gamgi_bool gamgi_gtk_statusbar_progress (void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
double fraction;

if (window->demo_flag == 1)
  {
  fraction = GAMGI_GTK_STATUSBAR_PROGRESS_FRACTION + 
  gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (window->progress_bar));
  if (fraction > 1.0) fraction = 0.0;
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->progress_bar), fraction);
  }

if (window->demo_flag == 2)
  gtk_progress_bar_pulse (GTK_PROGRESS_BAR (window->progress_bar));

return TRUE;
}

void gamgi_gtk_statusbar_mode (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

if (window->demo_flag == 0)
  {
  window->timer_progress = g_timeout_add (GAMGI_GTK_STATUSBAR_PROGRESS_TIME,
  gamgi_gtk_statusbar_progress, window);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->progress_bar), 0.0);
  gtk_progress_bar_set_orientation (GTK_PROGRESS_BAR (window->progress_bar),
  GTK_PROGRESS_LEFT_TO_RIGHT);
  gtk_widget_show (window->progress_bar);
  window->demo_flag = 1;
  }
else if (window->demo_flag == 1)
  {
  gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (window->progress_bar), 
  GAMGI_GTK_STATUSBAR_PROGRESS_FRACTION);
  window->demo_flag = 2;
  }
else if (window->demo_flag == 2)
  {
  gtk_widget_hide (window->progress_bar);
  g_source_remove (window->timer_progress);
  window->timer_progress = 0;
  window->demo_flag = 0;
  }
}
