/***********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_search.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include <ctype.h>

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_help_dialog.h"
#include "gamgi_io_token.h"

static gamgi_bool static_search (char *keyword)
{
if (strcmp (keyword, "help") == 0) return TRUE;
else if (strcmp (keyword, "text") == 0) return TRUE;
else if (strcmp (keyword, "orbital") == 0) return TRUE;
else if (strcmp (keyword, "bond") == 0) return TRUE;
else if (strcmp (keyword, "atom") == 0) return TRUE;
else if (strcmp (keyword, "direction") == 0) return TRUE;
else if (strcmp (keyword, "plane") == 0) return TRUE;
else if (strcmp (keyword, "group") == 0) return TRUE;
else if (strcmp (keyword, "molecule") == 0) return TRUE;
else if (strcmp (keyword, "cluster") == 0) return TRUE;
else if (strcmp (keyword, "cell") == 0) return TRUE;
else if (strcmp (keyword, "arrow") == 0) return TRUE;
else if (strcmp (keyword, "shape") == 0) return TRUE;
else if (strcmp (keyword, "graph") == 0) return TRUE;
else if (strcmp (keyword, "assembly") == 0) return TRUE;
else if (strcmp (keyword, "light") == 0) return TRUE;
else if (strcmp (keyword, "layer") == 0) return TRUE;
else if (strcmp (keyword, "window") == 0) return TRUE;
else if (strcmp (keyword, "gamgi") == 0) return TRUE;

return FALSE;
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
char string[GAMGI_ENGINE_LINE];
char file[GAMGI_ENGINE_LINE];
const char *name;

/************
 * Get word *
 ************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_keyword");
name = gtk_entry_get_text (GTK_ENTRY (entry));

/************************
 * convert to lowercase *
 ************************/

gamgi_io_token_lower (name, token);

/*******************************
 * check if word is searchable *
 *******************************/

if (static_search (token) == FALSE)
  {
  sprintf (string, "Keyword \"%s\" not available", name);
  gamgi_gtk_dialog_message_create ("Error", string, window);
  return;
  }

/*********************************
 * set first letter uppercase to *
 * keyword in Help dialog title  *
 *                               *
 *    add filename extension     *
 *                               *
 * create an help dialog level1  *
 *  to show help information     * 
 *********************************/

sprintf (string, "Help %c%s", toupper (token[0]), token + 1);
sprintf (file, "keywords/%s", token);

gamgi_gtk_help_dialog_import (string, file, window, 0, 0);
}

/****************** external function ***************
 *                                                  *
 *                GAMGI_GTK_HELP_SEARCH             *
 *                                                  *
 * Searches the help system looking for a keyword.  *
 * All visible GAMGI labels must be valid keywords. *
 *                                                  *
 ****************************************************/

void gamgi_gtk_help_search (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *button;
GtkWidget *vbox_dialog;
GtkWidget *hbox_left, *hbox_center;
GtkWidget *label;
GtkWidget *entry;

dialog = gamgi_gtk_dialog_task0_create ("Help Search", window);
window->action = GAMGI_GTK_HELP_SEARCH;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Keyword");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (dialog), "entry_keyword", entry);
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

gtk_widget_show (dialog);
}
