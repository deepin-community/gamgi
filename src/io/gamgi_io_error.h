/******************************************
 *
 * $GAMGI/src/io/gamgi_io_error.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

gamgi_bool gamgi_io_error_element (const char *element,
char *filename, int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_attribute (const char *attribute,
char *filename, int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_value (const char *value,
char *filename, int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_child (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_missing (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_incompatible (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_empty (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_multiple (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_resources (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_ref (const char *ref, 
char *filename, int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_cdata (char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_header (const char *identifier, 
char *filename, int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_parse (char *filename,   
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_font (char *font, char *filename,
int fileline, gamgi_window *window);

gamgi_bool gamgi_io_error_open (char *filename,
gamgi_window *window);

gamgi_bool gamgi_io_error_read (char *filename,
gamgi_window *window);

gamgi_bool gamgi_io_error_write (char *filename,
gamgi_window *window);

gamgi_bool gamgi_io_error_plane (gamgi_window *window);

gamgi_bool gamgi_io_error_direction (gamgi_window *window);

gamgi_bool gamgi_io_error_local (gamgi_window *window);

gamgi_bool gamgi_io_error_remote (gamgi_window *window);

gamgi_bool gamgi_io_error_browser (gamgi_window *window);

void gamgi_io_error_list (gamgi_slist *start, gamgi_window *window);

/*************
 * Rationale *
 *************

Work in Progress.

 *************
 * Rationale *
 *************/
