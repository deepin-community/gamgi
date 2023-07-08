/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_key.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

#include <gdk/gdkkeysyms.h>

#include "gamgi_gtk_gamgi_exit.h"

void gamgi_gtk_handle_key_press (GtkWidget *widget, 
GdkEventKey *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_window *window_new;
static gamgi_bool fullscreen = FALSE;
GdkWindow *drawable;

switch (event->keyval)
  {
  case GDK_Escape:
  if (fullscreen == FALSE)
    {
    drawable = gtk_widget_get_window (window->window);
    gdk_window_raise (drawable);
    if (window->top_flag == TRUE) gtk_widget_hide (window->top);
    if (window->medium_flag == TRUE) gtk_widget_hide (window->medium);
    if (window->bottom_flag == TRUE) gtk_widget_hide (window->bottom);
    gtk_window_fullscreen (GTK_WINDOW (window->window));
    fullscreen = TRUE;
    }
  else
    {
    if (window->top_flag == TRUE) gtk_widget_show (window->top);
    if (window->medium_flag == TRUE) gtk_widget_show (window->medium);
    if (window->bottom_flag == TRUE) gtk_widget_show (window->bottom);
    gtk_window_unfullscreen (GTK_WINDOW (window->window));
    fullscreen = FALSE;
    }
  break;

  case GDK_F1:
  if (fullscreen == TRUE) return;
  if (window->top_flag == TRUE)
    { gtk_widget_hide (window->top); window->top_flag = FALSE; }
  else
    { gtk_widget_show (window->top); window->top_flag = TRUE; }
  break;

  case GDK_F2:
  if (fullscreen == TRUE) return;
  if (window->medium_flag == TRUE)
    { gtk_widget_hide (window->medium); window->medium_flag = FALSE; }
  else
    { gtk_widget_show (window->medium); window->medium_flag = TRUE; }
  break;

  case GDK_F3:
  if (fullscreen == TRUE) return;
  if (window->bottom_flag == TRUE)
    { gtk_widget_hide (window->bottom); window->bottom_flag = FALSE; }
  else
    { gtk_widget_show (window->bottom); window->bottom_flag = TRUE; }
  break;

  case GDK_Page_Up:
  if ((window_new = window->next) == NULL) window_new = gamgi->window_start;
  if (gamgi->window_start != gamgi->window_end && fullscreen == TRUE)
    {
    if (window->top_flag == TRUE) gtk_widget_show (window->top);
    if (window->medium_flag == TRUE) gtk_widget_show (window->medium);
    if (window->bottom_flag == TRUE) gtk_widget_show (window->bottom);
    gtk_window_unfullscreen (GTK_WINDOW (window->window));

    if (window_new->top_flag == TRUE) gtk_widget_hide (window_new->top);
    if (window_new->medium_flag == TRUE) gtk_widget_hide (window_new->medium);
    if (window_new->bottom_flag == TRUE) gtk_widget_hide (window_new->bottom);
    gtk_window_fullscreen (GTK_WINDOW (window_new->window));
    }
  drawable = gtk_widget_get_window (window_new->window);
  gdk_window_raise (drawable);
  break;

  case GDK_Page_Down:
  if ((window_new = window->before) == NULL) window_new = gamgi->window_end;
  if (gamgi->window_start != gamgi->window_end && fullscreen == TRUE)
    {
    if (window->top_flag == TRUE) gtk_widget_show (window->top);
    if (window->medium_flag == TRUE) gtk_widget_show (window->medium);
    if (window->bottom_flag == TRUE) gtk_widget_show (window->bottom);
    gtk_window_unfullscreen (GTK_WINDOW (window->window));

    if (window_new->top_flag == TRUE) gtk_widget_hide (window_new->top);
    if (window_new->medium_flag == TRUE) gtk_widget_hide (window_new->medium);
    if (window_new->bottom_flag == TRUE) gtk_widget_hide (window_new->bottom);
    gtk_window_fullscreen (GTK_WINDOW (window_new->window));
    }
  drawable = gtk_widget_get_window (window_new->window);
  gdk_window_raise (drawable);
  break;

  case GDK_End:
  gamgi_gtk_gamgi_exit (NULL, window);
  break;
  }

}
