/**********************************************
 *
 * $GAMGI/src/global/gamgi_global_resources.c
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
#include "gamgi_io_token.h"

void gamgi_global_resources_window (void)
{
/*************************
 * Get screen dimensions *
 *************************/

gamgi->width = gdk_screen_width();
gamgi->height = gdk_screen_height();
}

void gamgi_global_resources_layer (gamgi_window *window)
{
int r, g, b;

/********************************
 * Get rgb color bits per pixel *
 ********************************/

glGetIntegerv (GL_RED_BITS, &r);
glGetIntegerv (GL_GREEN_BITS, &g);
glGetIntegerv (GL_BLUE_BITS, &b);

/********************************************************
 * Check if 16 or 24 color bits are available per pixel *
 *                                                      *
 * Check if bits are well distributed by the 3 channels *
 ********************************************************/

if (r + g + b != 16 && r + g + b != 24)
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid number of color bits per pixel", window);

if (abs (r - g) > 1 || abs (r - b) > 1 || abs (g - b) > 1)
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid number of color bits per pixel", window);

/**************************************************
 * in practise, it is slightly more convenient to *
 * store the non-vailable than the available bits *
 **************************************************/

gamgi->r = 8 - r;
gamgi->g = 8 - g;
gamgi->b = 8 - b;
}

void gamgi_global_resources_light (void)
{
/******************************************
 * Get maximum number of lights per layer *
 ******************************************/

glGetIntegerv (GL_MAX_LIGHTS, &gamgi->max_lights);
}

void gamgi_global_resources_text (gamgi_window *window)
{
char *path;

/*****************************************************
 * overwrite the text font default path, defined in  *
 * GAMGI_IO_PATH_TRUETYPE, with the path set by      *
 * the environment variable GAMGI_TEXT, if it exists *
 *****************************************************/

if (gamgi_io_token_check_create (getenv ("GAMGI_TEXT"),
&path, GAMGI_IO_PATHNAME) == FALSE)
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid local font path", window);

if (path != NULL)
  {
  gamgi_io_token_remove (&gamgi->text->path);
  gamgi_io_token_create (path, &gamgi->text->path);
  }
}

void gamgi_global_resources_help (gamgi_window *window)
{
/*************************************************
 * currently the remove functions are redundant, *
 * as gamgi_global_resources_help is called only *
 * once, in the beginning, after initializing    *
 * gamgi->help->local, gamgi->help->remote       *
 * and gamgi->help->browser to NULL              *
 *************************************************/
   
/************************************
 * Get path for local documentation *
 ************************************/

gamgi_io_token_remove (&gamgi->help->local);
if (gamgi_io_token_check_create (getenv ("GAMGI_HELP"), 
&gamgi->help->local, GAMGI_IO_PATHNAME) == FALSE)
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid local documentation path", window);

/**************************************
 * Get path for remote documentation: *
 **************************************/

gamgi_io_token_remove (&gamgi->help->remote);
if (gamgi_io_token_check_create (GAMGI_IO_PATH_REMOTE, 
&gamgi->help->remote, GAMGI_IO_PATHNAME) == FALSE)
  gamgi_gtk_dialog_message_create ("Error",
  "Invalid remote documentation path", window);

/*********************************
 * Get path for prefered browser *
 *********************************/

gamgi_io_token_remove (&gamgi->help->browser);
if (gamgi_io_token_check_create (getenv ("BROWSER"), 
&gamgi->help->browser, GAMGI_IO_PATHNAME) == FALSE)
  gamgi_gtk_dialog_message_create ("Error", 
  "Invalid browser path", window);

/***********************************************
 *         Default for Help source:            *
 * The GAMGI variable is defined: use local    *
 * The GAMGI variable is undefined: use remote *
 ***********************************************/

if (gamgi->help->local == NULL)
  gamgi->help->source_in = FALSE;
}
