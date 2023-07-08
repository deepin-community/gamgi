/*******************************************
 *
 * $GAMGI/src/global/gamgi_global_main.c
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
#include "gamgi_expat.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include <signal.h>

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_statusbar.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_window_create.h"
#include "gamgi_gtk_gamgi_exit.h"
#include "gamgi_mesa_lists.h"
#include "gamgi_mesa_start.h"
#include "gamgi_io_file.h"
#include "gamgi_global_resources.h"

/*******************************************************
 * AINSI C handler for signal interrupts: when Ctrl-C  *
 * is pressed the confirmation box appears. The signal *
 * handler is reinstalled again because on some        *
 * platforms, once a signal is raised, the way of      * 
 * handling it reverts to SIG_DFL, the default, unless *
 * the signal is refreshed with another call to        *
 * signal (). The default on most systems on an        *
 * unhandled Ctrl-C is to abort the program. Thus      *
 * reinstallation of signal handling is a must!        *
 *                                                     *
 *  Source: Advanced C Programming, John W. Perry,     *
 *          PWS Publishing Company, Boston 1998        *
 *******************************************************/  

static void static_handler (int number)
{
signal (SIGINT, static_handler);
gamgi_gtk_gamgi_exit (NULL, gamgi->window_start);
}

gamgi_gamgi *gamgi;

int main (int argc, char **argv)
{
gamgi_window *window;
gamgi_layer *layer;

/******************************************************
 * Always use dot (not comma), as decimal separator,  *
 * so even if locale is set to a comma-based country: *
 * LC_ALL=french; export LC_ALL                       *
 * both interfaces and files will work with dots.     *
 *                                                    *
 * Moreover, XML Schema explicitly allows only dots:  *
 * http://www.w3.org/TR/xmlschema-2/#decimal          *
 *                                                    *
 * To know more about locale: man 1p locale           *
 ******************************************************/

gtk_disable_setlocale ();

/************************************
 * Check if GTK is working properly *
 ************************************/

if (gtk_init_check (&argc, &argv) == FALSE)
  { fprintf (stderr, "Error: GTK not supported\n"); return 1; }

if (gtk_gl_init_check (&argc, &argv) == FALSE)
  { fprintf (stderr, "Error: OpenGL not supported\n"); return 1; }

/***********************************************************
 * Start GAMGI engine, including all basic infra-structure *
 ***********************************************************/

gamgi_engine_create_gamgi ();

/**************************************
 * Get width,height screen dimensions *
 **************************************/

gamgi_global_resources_window ();

/******************************************
 * Start GAMGI default configuration data *
 ******************************************/

gamgi_engine_start_gamgi ();

/**********************************************
 * Load GTK theme specification, if available *
 **********************************************/

gtk_rc_parse ("gtkrc");

/*********************************
 * Create first window and layer *
 *********************************/

window = gamgi_engine_create_window ();
gamgi_engine_start_window (window);
gamgi_mesa_start_window (window, gamgi->window);

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, gamgi->layer);

/**********************************
 * start undo and save mechanisms *
 **********************************/

gamgi_gtk_history_start (layer);

/*********************************************
 *    link layer to window and show window   *
 *                                           *
 * Add time timer to the status bar, updated *
 * every GAMGI_GTK_STATUSBAR_TIME_TIME secs  *
 *********************************************/

gamgi_engine_link_layer_window (layer, window);
gamgi_gtk_window_create (window, layer);

if (window->area == NULL)
  { fprintf (stderr, "Error: OpenGL not supported\n"); return 1; }

/**************************************************
 * Update the time in the status bar every 60 sec *
 **************************************************/

gamgi->timer_time = g_timeout_add (GAMGI_GTK_STATUSBAR_TIME_TIME,
gamgi_gtk_statusbar_time_timeout, NULL);

/*******************************************
 * Create OpenGL pre-compiled shared lists *
 *******************************************/

gamgi_mesa_lists_create ();

/********************************************************
 * Get GAMGI_HELP, BROWSER environment variables to get *
 * paths to local documentation and prefered browser    *
 ********************************************************/

gamgi_global_resources_help (window);

/***************************************
 * Get GAMGI_TEXT environment variable *
 * to get path to local Truetype fonts *
 ***************************************/

gamgi_global_resources_text (window);

/**************************************
 * Get r,g,b number of bits per pixel *
 **************************************/

gamgi_global_resources_layer (window);

/**********************************
 * Get maximum number of lights   *
 * allowed by Mesa/OpenGL library *
 **********************************/

gamgi_global_resources_light ();

/************************************************
 * to disable crtl+c: signal (SIGINT, SIG_IGN); *
 * to enable default: signal (SIGINT, SIG_DFL); *
 *                                              *
 *   In both cases, the default handler will    *
 *  be used, usually closing the application.   *
 ************************************************/

signal (SIGINT, static_handler);

/***************************
 * load command line files *
 ***************************/

gamgi_io_file_shell (argc, argv, window);

/***************************************************
 * Start main loop, where GTK waits for new events *
 ***************************************************/

gtk_main ();

/*******************************************
 * End GAMGI engine, remove lists, objects *
 *    and other basic infra-structure      *
 *******************************************/

gamgi_engine_remove_gamgi ();

return 0;
}
