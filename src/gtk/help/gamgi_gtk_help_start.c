/**********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_start.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_help_dialog.h"

static void static_insert (GtkWidget *text)
{
GtkTextBuffer *buffer;
GtkTextIter iter;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

/******************
 * insert message *
 ******************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold", 
"\nWelcome to GAMGI!\n\n");

/************
 * get help *
 ************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"To get help, users can: 1) browse the documentation at ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link",
"http://www.gamgi.org/ ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"or its local copy at ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"$GAMGI/doc/");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"; 2) ask questions by e-mail, to the mailling list ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link",
"gamgi-users@gamgi.org", -1);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"; 3) use the various help mechanisms provided directly in GAMGI "
"and reunited under the ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"button: ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Current");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
", ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Topic");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
", ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Search");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
", ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Tutorial");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
" and ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Tips");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
".\n\n");

/****************
 * Help Current *
 ****************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Current ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"shows instant documentation about the parameters in the task window "
"currently open, so pressing ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Atom->Create ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"and then ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Current ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"will open a window with information about ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Atom->Create");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
".\n\n");

/**************
 * Help Topic *
 **************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Topic ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"allows users to navigate a hierarchic tree, looking for a topic, "
"as in common file selection tools. Pressing a ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"+ ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"symbol opens a tree and pressing the ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"- ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"symbol closes it. Pressing the mouse pointer over a topic (to select it) "
"and pressing ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Ok ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"launches a window with the documentation.\n\n");

/***************
 * Help Search *
 ***************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Search ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"is a search engine, so entering a keyword and pressing ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Ok ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"opens a window with information about the keyword, "
"in the various contexts where it is used in GAMGI.\n\n");

/*****************
 * Help Tutorial *
 *****************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Tutorial ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"describes step-by-step how to execute various tasks, designed to show "
"how GAMGI works. Pressing the mouse pointer over a task (to select it), "
"and pressing ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Ok ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"opens a window with instructions to execute the task.\n\n");

/*************
 * Help Tips *
 *************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Tips ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"reunites a list of advices accumulated throughout years "
"of experience using GAMGI in real conditions, that are likely "
"to prove useful even for experienced users.\n\n");

/***********************
 * GAMGI_HELP variable *
 ***********************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"To indicate where are the local documentation files, "
"please set the ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"GAMGI_HELP ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"environment variable, for example adding a line as this to ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"~/.bashrc");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
":\n\n");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"GAMGI_HELP=~/gamgi/gamgi/doc; export GAMGI_HELP\n\n");

/***********************
 * GAMGI_TEXT variable *
 ***********************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"To indicate where are the TrueType fonts distributed with GAMGI "
"and used in solid mode, please set the ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"GAMGI_TEXT ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"environment variable, for example adding a line as this to ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"~/.bashrc");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
":\n\n");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"GAMGI_TEXT=~/gamgi/gamgi/src/io/fonts; export GAMGI_TEXT\n\n");

/********************
 * BROWSER variable *
 ********************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"To choose a browser to read the documentation, please set the ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"BROWSER ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"environment variable, for example adding a line as this to ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"~/.bashrc");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
":\n\n");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"BROWSER=firefox; export BROWSER\n\n");

/*************
 * more help *
 *************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
"To read more about the GAMGI Help system, please choose ");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"Help->Topic->Interfaces->Help");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text",
".\n\n");

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold",
"We hope GAMGI is good for you!\n");
}

/******************* external function ****************
 *                                                    *
 *                  GAMGI_GTK_HELP_START              *
 *                                                    *
 * Gives a general description of the Gamgi features. *
 *                                                    *
 ******************************************************/

void gamgi_gtk_help_start (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *vbox;
GtkWidget *text;

/**************************
 * create window and text *
 **************************/

vbox = gamgi_gtk_help_dialog_window ("Help Start", window);
text = gamgi_gtk_help_dialog_text (vbox);

/*******************
 * insert contents *
 *******************/

static_insert (text); 

gtk_widget_show (window->help);
}
