/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_dialog.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include <stdarg.h>
#include <gdk/gdkx.h>

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

/******************* external function *******************
 *                                                       *
 *               GAMGI_GTK_DIALOG_REMOVE                 *
 *                                                       *
 * Receives the address of a pointer to a dialog widget, * 
 *   removes the dialog and sets the pointer to NULL.    *
 *                                                       *
 *********************************************************/

void gamgi_gtk_dialog_remove (GtkWidget *widget, void *data)
{
GtkWidget **dialog = (GtkWidget **) data;

gtk_widget_destroy (*dialog);
*dialog = NULL;
}

/************* external function ***************
 *                                             *
 *          GAMGI_GTK_DIALOG_DELETE            *
 *                                             *
 * Converts the function with three arguments, *
 * required by the delete_event signal, into   *
 * the function with two arguments, required   *
 * by the clicked signal.                      *
 *                                             *
 ***********************************************/

int gamgi_gtk_dialog_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
gamgi_gtk_dialog_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

void gamgi_gtk_dialog_global_remove (void)
{
/*************************
 * remove global dialogs *
 *************************/

if (gamgi->message != NULL)
  gamgi_gtk_dialog_remove (gamgi->message, &gamgi->message);
if (gamgi->question != NULL)
  gamgi_gtk_dialog_remove (gamgi->question, &gamgi->question);
}

/************* external function ******************
 *                                                *
 *       GAMGI_GTK_DIALOG_TASK0_REMOVE            *
 *                                                *
 * Remove a dialog of level zero: If there is a   *
 * dialog of level one or a message dialog remove *
 * them; although message dialogs are modal, a    *
 * dialog level zero can be removed with the      *
 * window manager decorations; it is assumed that *
 * question dialogs are not created on top of     *
 * dialog windows.                                *
 *                                                *
 **************************************************/

void gamgi_gtk_dialog_task0_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

if (gamgi->message != NULL)
  gamgi_gtk_dialog_remove (NULL, &gamgi->message);
if (window->dialog1 != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->dialog1);
gamgi_gtk_dialog_remove (NULL, &window->dialog0);

window->action = FALSE;
}

/************* external function *****************
 *                                               *
 *       GAMGI_GTK_DIALOG_TASK1_REMOVE           *
 *                                               *
 * Remove a dialog of level one: If there is a   *
 * a message dialog remove it. Although message  *
 * dialogs are modal, a dialog level one can be  *
 * removed with the window manager decorations;  *
 * it is assumed that question dialogs are not   *
 * created on top of dialog windows.             *
 *                                               *
 *************************************************/

void gamgi_gtk_dialog_task1_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;

if (gamgi->message != NULL)
  gamgi_gtk_dialog_remove (NULL, &gamgi->message);
gamgi_gtk_dialog_remove (NULL, &window->dialog1);
}

/************* external function ***************
 *                                             *
 *       GAMGI_GTK_DIALOG_TASK0_DELETE         *
 *                                             *
 * Converts the function with three arguments, *
 * required by the delete_event signal, into   *
 * the function with two arguments, required   *
 * by the clicked signal.                      *
 *                                             *
 ***********************************************/

int gamgi_gtk_dialog_task0_delete (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
gamgi_gtk_dialog_task0_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

/************* external function ***************
 *                                             *
 *      GAMGI_GTK_DIALOG_TASK1_DELETE          *
 *                                             *
 * Converts the function with three arguments, *
 * required by the delete_event signal, into   *
 * the function with two arguments, required   *
 * by the clicked signal.                      *
 *                                             *
 ***********************************************/

int gamgi_gtk_dialog_task1_delete (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
gamgi_gtk_dialog_task1_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

void gamgi_gtk_dialog_color (GdkColor *color, 
float red, float green, float blue)
{
color->pixel = 0;
color->red = 65535 * red;
color->green = 65535 * green;
color->blue = 65535 * blue;
}

void gamgi_gtk_dialog_color_base (GtkWidget *widget, GdkColor color)
{
GtkStyle *style;

style = gtk_style_copy (gtk_widget_get_style (widget));
style->base[GTK_STATE_NORMAL] = color;
style->base[GTK_STATE_ACTIVE] = color;
style->base[GTK_STATE_PRELIGHT] = color;
style->base[GTK_STATE_SELECTED] = color;
style->base[GTK_STATE_INSENSITIVE] = color;

gtk_widget_set_style (widget, style);
g_object_unref (style);
}

void gamgi_gtk_dialog_color_bg (GtkWidget *widget, GdkColor color)
{
GtkStyle *style;

style = gtk_style_copy (gtk_widget_get_style (widget));
style->bg[GTK_STATE_NORMAL] = color;
style->bg[GTK_STATE_ACTIVE] = color;
style->bg[GTK_STATE_PRELIGHT] = color;
style->bg[GTK_STATE_SELECTED] = color;
style->bg[GTK_STATE_INSENSITIVE] = color;

gtk_widget_set_style (widget, style);
g_object_unref (style);
}

void gamgi_gtk_dialog_color_fg (GtkWidget *widget, GdkColor color)
{
GtkStyle *style;

style = gtk_style_copy (gtk_widget_get_style (widget));
style->fg[GTK_STATE_NORMAL] = color;
style->fg[GTK_STATE_ACTIVE] = color;
style->fg[GTK_STATE_PRELIGHT] = color;
style->fg[GTK_STATE_SELECTED] = color;
style->fg[GTK_STATE_INSENSITIVE] = color;

gtk_widget_set_style (widget, style);
g_object_unref (style);
}

void gamgi_gtk_dialog_color_default (GtkWidget *widget, gamgi_window *window)
{
GtkWidget *hbox;
GtkWidget *label;
GtkStyle *style;
GList *list;

/****************************************************
 * use the default color, taken from the time label *
 ****************************************************/

hbox = gtk_statusbar_get_message_area (GTK_STATUSBAR (window->statusbar_time));
list = gtk_container_get_children (GTK_CONTAINER (hbox));
label = GTK_WIDGET (list->data);
g_list_free (list);

style = gtk_style_copy (gtk_widget_get_style (label));
gtk_widget_set_style (widget, style);
g_object_unref (style);
}

/*************** external function **************
 *                                              *
 *             GAMGI_GTK_DIALOG_BEEP            *
 *                                              *
 * Sound a beep if enabled in the configuration *
 *                                              *
 ************************************************/

void gamgi_gtk_dialog_beep (void)
{
if (gamgi->gamgi->beep == TRUE) gdk_beep ();
}

int gamgi_gtk_dialog_button_mask (int button)
{
switch (button)
  {
  case 1: return GDK_BUTTON1_MASK;
  case 2: return GDK_BUTTON2_MASK;
  case 3: return GDK_BUTTON3_MASK;
  }

/***************************
 * ignore action otherwise *
 ***************************/

return FALSE;
}

int gamgi_gtk_dialog_button_unmask (int mask)
{
switch (mask)
  {
  case GDK_BUTTON1_MASK: return 1;
  case GDK_BUTTON2_MASK: return 2;
  case GDK_BUTTON3_MASK: return 3;
  }

/***************************
 * ignore action otherwise *
 ***************************/

return FALSE;
}

/********* external function **********
 *                                    *
 *  GAMGI_GTK_DIALOG_BUTTON_CREATE    *
 *                                    *
 * Create a button with a given label *
 *   and a given foreground color.    *
 *                                    *
 **************************************/

GtkWidget *gamgi_gtk_dialog_button_create (char *name_label, char *name_color)
{
GtkWidget *button;
GtkWidget *label;
GdkColor  color;

button = gtk_button_new ();

label = gtk_label_new (name_label);
gtk_misc_set_alignment (GTK_MISC (label), 0.5, 0.5);
gtk_container_add (GTK_CONTAINER (button), label);
gtk_widget_show (label);

if (name_color != NULL)
  {
  gdk_color_parse (name_color, &color);
  gamgi_gtk_dialog_color_fg (label, color);
  }

return button;
}

void gamgi_gtk_dialog_titles_scan (GtkCellLayout *layout,
GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, void *data)
{
gamgi_bool sensitive = TRUE;

if (gtk_tree_model_iter_has_child (model, iter) == TRUE) sensitive = FALSE;

g_object_set (renderer, "sensitive", sensitive, NULL);
}

void gamgi_gtk_dialog_sensitive_scan (GtkCellLayout *layout, 
GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, void *data)
{
GtkTreePath *path;
int *indices;
int *sensitive = (int *) data;

path = gtk_tree_model_get_path (model, iter);
indices = gtk_tree_path_get_indices (path);
g_object_set (renderer, "sensitive", sensitive[indices[0]], NULL);
gtk_tree_path_free (path);
}

gamgi_bool *gamgi_gtk_dialog_sensitive_create (int rows)
{
gamgi_bool *sensitive;
int i;

sensitive = (gamgi_bool *) malloc (sizeof (gamgi_bool) * rows);
for (i = 0; i < rows; i++) sensitive[i] = TRUE;

return sensitive;
}

/************** external function *************
 *                                            *
 *            GAMGI_GTK_DIALOG_PAGE           *
 *                                            *
 * Get the notebook page currently active     *
 * in the dialog level zero, starting from 1. *
 * Return 0 if no notebook is currently open. *
 **********************************************/

int gamgi_gtk_dialog_page (gamgi_window *window)
{
GtkWidget *notebook;

/*******************************
 * second level dialog is open *
 *******************************/

if (window->dialog1 != NULL) return 0;

/********************************
 * first level dialog is closed *
 ********************************/

if (window->dialog0 == 0) return 0;

/*************************************************
 * first level dialog is open without a notebook *
 *************************************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (window->dialog0), "notebook");
if (notebook == NULL) return 0;

/**********************************************
 * first level dialog is open with a notebook *
 **********************************************/

return 1 + gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
}

/******************** external function ***********************
 *                                                            *
 *            GAMGI_GTK_DIALOG_MESSAGE_CREATE                 *
 *                                                            *
 * Create a message dialog with one button: If there is a     *
 * previous question dialog remove it. Create a window with   *
 * fixed size and make it transient; connect the delete_event *
 * signal to the function gamgi_gtk_dialog_delete and the     *
 * cancel button to the function gamgi_gtk_dialog_remove.     *
 *                                                            *
 **************************************************************/

void gamgi_gtk_dialog_message_create (char *title, 
char *message, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *label;

if (gamgi->message != NULL) return;

gamgi->message = dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (dialog), title);
gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_NONE);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window->window));
g_signal_connect (dialog, "delete_event",
G_CALLBACK (gamgi_gtk_dialog_delete), &gamgi->message);

vbox = gtk_vbox_new (TRUE, 0);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

label = gtk_label_new (message);
gtk_misc_set_padding (GTK_MISC (label), 10, 10);
gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
gtk_widget_show (label);

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_remove), &gamgi->message);

gtk_widget_show (dialog);
gtk_grab_add (dialog);
}

/******************** external function ***************************
 *                                                                *
 *             GAMGI_GTK_DIALOG_QUESTION_CREATE                   *
 *                                                                *
 * Create a modal question dialog with two buttons: If there is a *
 * previous question dialog remove it. This is required because   *
 * a new question dialog can always be created through the window *
 * manager decoration. Create a window with fixed size and make   *
 * it transient; connect the delete_event signal to the function  *
 * gamgi_gtk_dialog_delete and the cancel button to the function  *
 * gamgi_gtk_dialog_remove.                                       *
 *                                                                *
 ******************************************************************/

void gamgi_gtk_dialog_question_create (char *title, 
char *question, gamgi_callback2 function_ok, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *hbox;
GtkWidget *vbox;
GtkWidget *button;
GtkWidget *label;

if (gamgi->question != NULL) 
  gamgi_gtk_dialog_remove (NULL, &gamgi->question);

gamgi->question = dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (dialog), title);
gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_NONE);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window->window));
g_signal_connect (dialog, "delete_event",
G_CALLBACK (gamgi_gtk_dialog_delete), &gamgi->question);

vbox = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

label = gtk_label_new (question);
gtk_misc_set_padding (GTK_MISC (label), 10, 10);
gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
gtk_widget_show (label);

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (function_ok), window);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
gtk_widget_show (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_remove), &gamgi->question);

gtk_widget_show (dialog);
gtk_grab_add (dialog);
}

GtkWidget *gamgi_gtk_dialog_text_create (void)
{
GtkWidget *text;
GdkColor color;
GtkTextBuffer *buffer;

text = gtk_text_view_new ();
gtk_text_view_set_editable (GTK_TEXT_VIEW (text), FALSE);
gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), FALSE);
gtk_text_view_set_justification (GTK_TEXT_VIEW (text), GTK_JUSTIFY_LEFT);
gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text), 10);
gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text), 10);
gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_WORD);
buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));

/************************
 * set background color *
 ************************/

gamgi_gtk_dialog_color (&color, gamgi->gamgi->background[0],
gamgi->gamgi->background[1],  gamgi->gamgi->background[2]);
gamgi_gtk_dialog_color_base (text, color);

/**************************************
 * set text, title, bold, link colors *
 **************************************/

gamgi_gtk_dialog_color (&color, gamgi->gamgi->foreground[0], 
gamgi->gamgi->foreground[1], gamgi->gamgi->foreground[2]);
gtk_text_buffer_create_tag (buffer, "color_text", 
"foreground-gdk", &color, NULL);

gamgi_gtk_dialog_color (&color, gamgi->gamgi->title[0],
gamgi->gamgi->title[1], gamgi->gamgi->title[2]);
gtk_text_buffer_create_tag (buffer, "color_title", 
"foreground-gdk", &color, NULL);

gamgi_gtk_dialog_color (&color, gamgi->gamgi->bold[0],
gamgi->gamgi->bold[1], gamgi->gamgi->bold[2]);
gtk_text_buffer_create_tag (buffer, "color_bold", 
"foreground-gdk", &color, NULL);

gamgi_gtk_dialog_color (&color, gamgi->gamgi->link[0],
gamgi->gamgi->link[1], gamgi->gamgi->link[2]);
gtk_text_buffer_create_tag (buffer, "color_link", 
"foreground-gdk", &color, NULL);

/**************************
 * set normal, large size *
 **************************/

gtk_text_buffer_create_tag (buffer, "size_normal", 
"scale", GAMGI_GTK_TEXT_NORMAL, NULL);

gtk_text_buffer_create_tag (buffer, "size_large",
"scale", GAMGI_GTK_TEXT_LARGE, NULL);

return text;
}

void gamgi_gtk_dialog_text_insert (GtkTextBuffer *buffer,
GtkTextIter *iter, char *size, char *color, char *raw, ...)
{
va_list argument;
char format[GAMGI_ENGINE_LINE];
char line[GAMGI_ENGINE_LINE];
char *string;
int integer, length, i, j;
double real;

va_start (argument, raw);
length = strlen (raw);

i = 0; j = 0;
while (i < length)
  {
  format[j++] = raw[i++];

  /*********************
   * no % or last char *
   *********************/

  if (raw[i-1] != '%' || i == length) continue;

  /******************
   * %% = escaped % *
   ******************/

  if (raw[i] == '%') { i++; continue; }

  while (i < length)
    {
    format[j++] = raw[i++];
    if (raw[i-1] == 'd')
      {
      integer = va_arg (argument, int);
      format[j] = '\0';
      sprintf (line, format, integer);
      }
    else if (raw[i-1] == 'f' || raw[i-1] == 'e')
      {
      integer = va_arg (argument, int);
      real = va_arg (argument, double);
      format[j] = '\0';
      sprintf (line, format, integer, real);
      }
    else if (raw[i-1] == 's')
      {
      string = va_arg (argument, char *);
      format[j] = '\0';
      sprintf (line, format, string);
      }
    else continue;

    gtk_text_buffer_insert_with_tags_by_name (buffer, iter, line, -1, size, color, NULL);
    j = 0;
    break;
    }
  }

format[j] = '\0';
gtk_text_buffer_insert_with_tags_by_name (buffer, iter, format, -1, size, color, NULL);
va_end (argument);
}

/********************** external function *****************
 *                                                        *
 *                GAMGI_GTK_DIALOG_TASK0_CREATE           *
 *                                                        *
 * Create a dialog window of level zero: If there are     *
 * previous dialogs of level zero or one, remove them;    *    
 * create a window with fixed size and make it transient; *
 * connect the delete_event signal to the function        *
 * gamgi_gtk_dialog_task0_delete, used to delete dialogs  *
 * of level zero.                                         *
 *                                                        *
 **********************************************************/
 
GtkWidget *gamgi_gtk_dialog_task0_create (char *title, void *data)
{
gamgi_window *windows, *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;

windows = gamgi->window_start;
do
  {
  windows->action = FALSE;
  if (windows->dialog0 != NULL)
    gamgi_gtk_dialog_remove (NULL, &windows->dialog0);
  if (window->dialog1 != NULL) 
    gamgi_gtk_dialog_remove (NULL, &windows->dialog1);
  }
while ((windows = windows->next) != NULL);

window->dialog0 = dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (dialog), title);
gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_NONE);
gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window->window));
g_signal_connect_after (dialog, "delete_event",
G_CALLBACK (gamgi_gtk_dialog_task0_delete), window);

return dialog;
}

/********************** external function ***********************
 *                                                              *
 *                 GAMGI_GTK_DIALOG_TASK1_CREATE                *
 *                                                              *
 * Create a dialog window of level one: If there is a previous  *
 * level one dialog, remove it; create a window with fixed size *
 * and make it transient; connect the delete_event signal to    *
 * the function gamgi_gtk_dialog_task1_delete, used to delete   *
 * dialogs of level one.                                        *
 *                                                              *
 ****************************************************************/
   
GtkWidget *gamgi_gtk_dialog_task1_create (char *title, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;

if (window->dialog1 != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->dialog1);

window->dialog1 = dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_title (GTK_WINDOW (dialog), title);
gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_NONE);
gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window->window));
g_signal_connect_after (dialog, "delete_event",
G_CALLBACK (gamgi_gtk_dialog_task1_delete), window);

return dialog;
}

/********************** external function ************************
 *                                                               *
 *                 GAMGI_GTK_DIALOG_HELP_CREATE                  *
 *                                                               *
 *****************************************************************/

GtkWidget *gamgi_gtk_dialog_help_create (char *title, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;

if (window->help != NULL)
  gamgi_gtk_dialog_remove (NULL, &window->help);

window->help = dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
gtk_window_set_title (GTK_WINDOW (dialog), title);
gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_NONE);
gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(window->window));

gtk_window_set_default_size (GTK_WINDOW (dialog),
GAMGI_GTK_TEXT_WIDTH, GAMGI_GTK_TEXT_HEIGHT);
gtk_widget_set_size_request (dialog, 
GAMGI_GTK_TEXT_WIDTH_MIN, GAMGI_GTK_TEXT_HEIGHT_MIN);

g_signal_connect (dialog, "delete_event",
G_CALLBACK (gamgi_gtk_dialog_delete), &window->help);

return dialog;
}

/********************** external function ************************
 *                                                               *
 *                GAMGI_GTK_DIALOG_REPORT_CREATE                 *
 *                                                               *
 *****************************************************************/

GtkWidget *gamgi_gtk_dialog_report_create (GtkWidget *text, 
char *title, gamgi_enum action, gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *vbox;
GtkWidget *hbox;
GtkWidget *button;
GtkWidget *scrolled_window;

dialog = gamgi_gtk_dialog_task0_create (title, window);
window->action = action;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
gtk_window_set_default_size (GTK_WINDOW (dialog),
GAMGI_GTK_TEXT_WIDTH, GAMGI_GTK_TEXT_HEIGHT);
gtk_widget_set_size_request (dialog,
GAMGI_GTK_TEXT_WIDTH_MIN, GAMGI_GTK_TEXT_HEIGHT_MIN);

vbox = gtk_vbox_new (FALSE, 3);
gtk_container_add (GTK_CONTAINER (dialog), vbox);
gtk_widget_show (vbox);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
gtk_widget_show (scrolled_window);

gtk_container_add (GTK_CONTAINER (scrolled_window), text);
gtk_widget_show (text);

hbox = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);
gtk_widget_show (hbox);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

return dialog;
}
