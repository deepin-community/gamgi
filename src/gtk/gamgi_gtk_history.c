/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_history.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_engine_remove.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_copy.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_global_selection.h"

void static_levels (gamgi_dlist *dlist, int levels)
{
/*********************************************
 * Count buffer layers from start to end and *
 * remove the end layers that are in excess. *
 *********************************************/

while (dlist != NULL)
  {
  if (levels-- < 0)
    {
    gamgi_engine_remove_layer (GAMGI_CAST_LAYER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist);
    }
  else dlist = dlist->next;
  }
}

void gamgi_gtk_history_update (gamgi_layer *layer, 
gamgi_dlist *dlist, int max_levels)
{
gamgi_layer *layer_end;
gamgi_dlist *dlist_end;

static_levels (dlist, max_levels - 1);

/********************************************
 * copy current layer to end of buffer list *
 ********************************************/

layer_end = gamgi_engine_copy_layer (layer);
layer_end->object.dlist = NULL;
layer_end->object.object = NULL;
layer_end->undo = NULL;
layer_end->save = NULL;

dlist_end = gamgi_engine_dlist_add_end (dlist);
dlist_end->data = layer_end;
}

static void static_label (GtkWidget *label, int number)
{
char token[GAMGI_ENGINE_TOKEN];

sprintf (token, "%d", number);
gtk_label_set_text (GTK_LABEL (label), token);
}

static void static_button (GtkWidget *button, int number)
{
GdkColor color;

/*****************************************************
 * show STOP (red) color when the number of options  *
 * available is 0, show MOVE (green) color otherwise *
 *****************************************************/

if (number == 0)
  gamgi_gtk_dialog_color (&color, 
  GAMGI_GTK_STOP_R, GAMGI_GTK_STOP_G, GAMGI_GTK_STOP_B);
else
  gamgi_gtk_dialog_color (&color, 
  GAMGI_GTK_MOVE_R, GAMGI_GTK_MOVE_G, GAMGI_GTK_MOVE_B);

gamgi_gtk_dialog_color_bg (button, color);
}

static void static_count (gamgi_dlist *dlist_now, int *backward, int *forward)
{
gamgi_dlist *dlist;

*backward = 0;
dlist = dlist_now;
while (dlist->next != NULL)
  {
  (*backward)++;
  dlist = dlist->next;
  }

*forward = 0;
dlist = dlist_now;
while (dlist->before != NULL)
  { 
  (*forward)++;
  dlist = dlist->before;
  }

}

static void static_update (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
gamgi_dlist *dlist = window->history;
int backward, forward;
GtkWidget *button, *label;

static_count (dlist, &backward, &forward);

label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_backward");
static_label (label, backward);
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_forward");
static_label (label, forward);

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_backward");
static_button (button, backward);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_forward");
static_button (button, forward);
}

static void static_swap (gamgi_layer *layer_old, gamgi_layer *layer_new)
{
layer_old->object.dlist->data = layer_new;

layer_new->object.dlist = layer_old->object.dlist;
layer_new->object.object = layer_old->object.object;

layer_old->object.dlist = NULL;
layer_old->object.object = NULL;
}

static void static_backward (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_layer *layer_new, *layer_old;
gamgi_dlist *dlist;

dlist = window->history;
layer_old = GAMGI_CAST_LAYER dlist->data;
dlist = dlist->next;
if (dlist == NULL)
  {
  gamgi_gtk_dialog_beep ();
  return;
  }
window->history = dlist;
static_update (window);
layer_new = GAMGI_CAST_LAYER dlist->data;
window->layer = layer_new;
static_swap (layer_old, layer_new);
gtk_widget_queue_draw (window->area);
}

static void static_forward (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_layer *layer_new, *layer_old;
gamgi_dlist *dlist;

dlist = window->history;
layer_old = GAMGI_CAST_LAYER dlist->data;
dlist = dlist->before;
if (dlist == NULL)
  {
  gamgi_gtk_dialog_beep ();
  return;
  }
window->history = dlist;
static_update (window);
layer_new = GAMGI_CAST_LAYER dlist->data;
window->layer = layer_new;
static_swap (layer_old, layer_new);
gtk_widget_queue_draw (window->area);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
gamgi_layer *layer_old, *layer_new;
gamgi_dlist *dlist;
int max_levels;

max_levels = GAMGI_POINTER_TO_INT (g_object_get_data (G_OBJECT (dialog), "max_levels"));

dlist = window->history;
layer_new = GAMGI_CAST_LAYER dlist->data;

while (dlist->before != NULL) dlist = dlist->before;
layer_old = GAMGI_CAST_LAYER dlist->data;

if (layer_new == layer_old)
  {
  /******************************************
   * This is the original layer: save mode. *
   ******************************************/

  gamgi_gtk_history_update (layer_old, dlist, max_levels);
  }
else
  {
  /*************************************************
   * This is not the original layer: recover mode. *
   *************************************************/

  dlist = window->history;
  while (dlist->before != NULL)
    {
    dlist->data = dlist->before->data;
    dlist = dlist->before;
    }

  layer_old->undo->data = layer_new;
  layer_old->save->data = layer_new;
  layer_new->undo = layer_old->undo;
  layer_new->save = layer_old->save;
  layer_old->undo = NULL;
  layer_old->save = NULL;

  /*********************************
   * remove global selection lists *
   *********************************/

  gamgi_global_selection_remove_all ();

  /****************************************************
   * set up layer as current layer and current object *
   ****************************************************/

  gamgi_gtk_object_layer_local (layer_new, window);
  gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT layer_new, window);
  }
window->history = NULL;
gamgi_gtk_dialog_task0_remove (widget, window);
}

static void static_cancel (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_layer *layer_old, *layer_new;
gamgi_dlist *dlist;

dlist = window->history;
layer_old = GAMGI_CAST_LAYER dlist->data;

while (dlist->before != NULL) dlist = dlist->before;
layer_new = GAMGI_CAST_LAYER dlist->data;

if (layer_old != layer_new)
  {
  static_swap (layer_old, layer_new);
  window->layer = layer_new;
  }
window->history = NULL;
gamgi_gtk_dialog_task0_remove (widget, window);

/*********************************************
 * Redraw gl_area image and reset atom entry *
 *********************************************/

gtk_widget_queue_draw (window->area);
}

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
static_cancel (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_create (gamgi_window *window, char *name, int max)
{
GtkWidget *dialog;
GtkWidget *frame;
GtkWidget *button;
GtkWidget *vbox, *vbox1;
GtkWidget *hbox, *hbox1;
GtkWidget *label;
GtkWidget *arrow;

dialog = gamgi_gtk_dialog_task0_create (name, window);
g_object_set_data (G_OBJECT (dialog), "max_levels", GAMGI_INT_TO_POINTER (max));
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

/*********
 * frame *
 *********/

frame = gtk_frame_new (NULL);
gtk_frame_set_label (GTK_FRAME (frame), "Move");
gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
gtk_widget_show (frame);

vbox1 = gtk_vbox_new (FALSE, 5);
gtk_container_set_border_width (GTK_CONTAINER (vbox1), 5);
gtk_container_add (GTK_CONTAINER (frame), vbox1);
gtk_widget_show (vbox1);

/***********
 * 1st row *
 ***********/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox1), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

label = gtk_label_new ("Backward");
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

label = gtk_label_new ("Forward");
gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/***********
 * 2nd row *
 ***********/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox1), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

/************
 * backward *
 ************/

hbox1 = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox), hbox1, FALSE, FALSE, 0);
gtk_widget_show (hbox1);

label = gtk_label_new ("");
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_backward", label);
gtk_widget_show (label);

button = gtk_button_new();
gtk_widget_set_size_request (button, 20, 20);
gtk_box_pack_start (GTK_BOX (hbox1), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_backward", button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_backward), window);
gtk_widget_show (button);

arrow = gtk_arrow_new (GTK_ARROW_LEFT, GTK_SHADOW_OUT);
gtk_container_add (GTK_CONTAINER (button), arrow);
gtk_widget_show (arrow);

/***********
 * forward *
 ***********/

hbox1 = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox), hbox1, FALSE, FALSE, 0);
gtk_widget_show (hbox1);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "label_forward", label);
gtk_widget_show (label);

button = gtk_button_new();
gtk_widget_set_size_request (button, 20, 20);
gtk_box_pack_end (GTK_BOX (hbox1), button, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (dialog), "button_forward", button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_forward), window);
gtk_widget_show (button);

arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_OUT);
gtk_container_add (GTK_CONTAINER (button), arrow);
gtk_widget_show (arrow);

static_update (window);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_cancel), window);

gtk_widget_show (dialog);
gtk_grab_remove (dialog);
gtk_grab_add (dialog);
}

void gamgi_gtk_history_start (gamgi_layer *layer)
{
layer->undo = gamgi_engine_dlist_add_start (NULL);
layer->save = gamgi_engine_dlist_add_start (NULL);
layer->undo->data = layer;
layer->save->data = layer;
}

void gamgi_gtk_history_layer_reset (gamgi_dlist *dlist)
{
dlist = gamgi_engine_dlist_remove_start (dlist);
while (dlist != NULL)
  {
  gamgi_engine_remove_layer (GAMGI_CAST_LAYER dlist->data);
  dlist = gamgi_engine_dlist_remove_start (dlist);
  }
}

void gamgi_gtk_history_window_reset (gamgi_dlist *dlist)
{
gamgi_layer *layer_old, *layer_new;

layer_old = GAMGI_CAST_LAYER dlist->data;

while (dlist->before != NULL) dlist = dlist->before;
layer_new = GAMGI_CAST_LAYER dlist->data;

if (layer_new != layer_old) static_swap (layer_old, layer_new);
}

void gamgi_gtk_history_gamgi_reset (void)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_dlist *dlist;

window = gamgi->window_start;
while (window != NULL)
  {
  dlist = window->layer_start;
  while (dlist != NULL)
    {
    layer = GAMGI_CAST_LAYER (dlist->data);

    static_levels (layer->undo, gamgi->layer->undo);
    static_levels (layer->save, gamgi->layer->save);

    dlist = dlist->next;
    }
  window = window->next;
  }
}

void gamgi_gtk_history_undo (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

window->history = window->layer->undo;

static_create (window, "Undo", GAMGI_ENGINE_UNDO);
}

void gamgi_gtk_history_save (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

window->history = window->layer->save;

static_create (window, "Save", GAMGI_ENGINE_SAVE);
}
