/***********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_dialog.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include <ctype.h>

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_io_token.h"
#include "gamgi_io_file.h"
#include "gamgi_io_error.h"
#include "gamgi_expat_import.h"

static gamgi_bool static_browser (char *fullname, 
char *shortname, gamgi_window *window)
{
char command[GAMGI_ENGINE_LINE];

/******************************
 * check browser availability *
 ******************************/

if (gamgi->help->browser == NULL)
  return gamgi_io_error_browser (window);

/********************************************************
 * check command size: add 4 for 2 spaces, '&' and '\0' *
 ********************************************************/

if (strlen (gamgi->help->browser) + strlen (fullname) + 4 > GAMGI_ENGINE_LINE)
  return gamgi_io_error_open (shortname, window);

/********************************
 * array is larger than command *
 ********************************/

sprintf (command, "%s %s &", gamgi->help->browser, fullname);
system (command);

return TRUE;
}

char *static_source (gamgi_window *window)
{
char *source;

if (gamgi->help->source_in == TRUE)
  {
  /*********************************************
   * get help documentation from local address *
   *********************************************/

  source = gamgi->help->local;
  if (source == NULL) gamgi_io_error_local (window);
  }
else
  {
  /**********************************************
   * get help documentation from remote address *
   **********************************************/

  source = gamgi->help->remote;
  if (source == NULL) gamgi_io_error_remote (window);
  }

return source; 
}

GtkWidget *gamgi_gtk_help_dialog_text (GtkWidget *vbox)
{
GtkWidget *scrolled_window;
GtkWidget *text;

/**************************
 * create scrolled window *
 **************************/

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
gtk_widget_show (scrolled_window);

/******************************
 * create and initialize text *
 ******************************/

text = gamgi_gtk_dialog_text_create ();
gtk_container_add (GTK_CONTAINER (scrolled_window), text);
gtk_widget_show (text);

return text;
}

GtkWidget *gamgi_gtk_help_dialog_page (GtkWidget *notebook, char *name)
{
GtkWidget *label;
GtkWidget *vbox_page;

/********
 * page *
 ********/

vbox_page = gtk_vbox_new (TRUE, 0);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 5);
label = gtk_label_new (name);
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_widget_show (vbox_page);

return vbox_page;
}

GtkWidget *gamgi_gtk_help_dialog_notebook (GtkWidget *vbox_dialog)
{
GtkWidget *notebook;

/************
 * notebook *
 ************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, TRUE, TRUE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
gtk_widget_set_size_request (notebook,
GAMGI_GTK_TEXT_WIDTH_MIN, GAMGI_GTK_TEXT_HEIGHT_MIN);
gtk_widget_show (notebook);

return notebook;
}

GtkWidget *gamgi_gtk_help_dialog_window (char *title, gamgi_window *window)
{
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *button;

dialog = gamgi_gtk_dialog_help_create (title, window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox_dialog = gtk_vbox_new (FALSE, 0);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 5);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_remove), &window->help);
gtk_widget_show (button);

return vbox_dialog;
}

gamgi_bool gamgi_gtk_help_dialog_import (char *title, 
char *file, gamgi_window *window, int n_pages, int page, ...)
{
GtkWidget *notebook;
GtkWidget *vbox_dialog, *vbox_page;
GtkWidget *text;
char fullname[GAMGI_IO_FULLNAME];
char token[GAMGI_ENGINE_TOKEN];
char *shortname;
va_list list;
char *source, *name;
int i;

/***********************************************
 * The allowed values are:                     *
 * if (n_pages == 0) page = 0;                 *
 * if (n_pages == N && N > 0) page = 1,..., N; *
 ***********************************************/
 
if (page > n_pages) page = n_pages;
if (page == 0 && n_pages > 0) page = 1;

/****************************
 * get documentation source *
 ****************************/

source = static_source (window);
if (source == NULL) return FALSE;

if (gamgi->help->agent_in == FALSE)
  {
  /*****************************
   * use a browser to see file *
   *****************************/

  if (page == 0)
    {
    /**********************************
     * check size, accounting for the *
     * extra characters: / .html '\0' *
     **********************************/

    if (strlen (source) + strlen (file) + 7 > GAMGI_IO_FULLNAME)
      return gamgi_io_error_open (file, window);
    sprintf (fullname, "%s/%s.html", source, file);
    }
  else
    {
    /***********************************
     * get file for notebook open page *
     ***********************************/

    name = NULL;
    va_start (list, page);
    for (i = 0; i < page; i++)
      name = va_arg (list, char *);
    va_end (list);

    /************************************
     * check size, accounting for the   *
     * extra characters: / _ .html '\0' *
     ************************************/

    if (strlen (source) + strlen (file) + strlen (name) + 8 > GAMGI_IO_FULLNAME)
      return gamgi_io_error_open (file, window);
    sprintf (fullname, "%s/%s_%s.html", source, file, name);
    }

  shortname = gamgi_io_file_shortname (fullname);

  return static_browser (fullname, shortname, window);
  }

/*************************
 * use gamgi to see file *
 *************************/

vbox_dialog = gamgi_gtk_help_dialog_window (title, window);
notebook = gamgi_gtk_help_dialog_notebook (vbox_dialog);

if (page == 0)
  {
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), FALSE);
  vbox_page = gamgi_gtk_help_dialog_page (notebook, "");
  text = gamgi_gtk_help_dialog_text (vbox_page);
  
  /**********************************
   * check size, accounting for the *
   * extra characters: / .html '\0' *
   **********************************/

  if (strlen (source) + strlen (file) + 7 > GAMGI_IO_FULLNAME)
    return gamgi_io_error_open (file, window);
  sprintf (fullname, "%s/%s.html", source, file);

  if (gamgi_expat_import_html (fullname, text, window) == FALSE)
    { gamgi_gtk_dialog_remove (NULL, &window->help); return FALSE; }
  }
else
  {
  /***********************************
   * get file for notebook open page *
   ***********************************/

  va_start (list, page);
  for (i = 0; i < n_pages; i++)
    {
    name = va_arg (list, char *);

    sprintf (token, "%c%s", toupper (name[0]), name + 1);
    vbox_page = gamgi_gtk_help_dialog_page (notebook, token);
    text = gamgi_gtk_help_dialog_text (vbox_page);

    /************************************
     * check size, accounting for the   *
     * extra characters: / _ .html '\0' *
     ************************************/

    if (strlen (source) + strlen (file) + strlen (name) + 8 > GAMGI_IO_FULLNAME)
      { gamgi_io_error_open (file, window); 
        gamgi_gtk_dialog_remove (NULL, &window->help); 
        va_end (list); return FALSE; }

    sprintf (fullname, "%s/%s_%s.html", source, file, name);

    if (gamgi_expat_import_html (fullname, text, window) == FALSE)
      { gamgi_gtk_dialog_remove (NULL, &window->help); 
        va_end (list); return FALSE; }
    }

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), page - 1);
  va_end (list);
  }

gtk_widget_show (window->help);

return TRUE;
}

void gamgi_gtk_help_dialog_tips (GtkWidget *widget, void *data)
{
gamgi_gtk_help_dialog_import ("Help Tips",
"tips", GAMGI_CAST_WINDOW data, 0, 0);
}
