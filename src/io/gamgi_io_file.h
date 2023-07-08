/******************************************
 *
 * $GAMGI/src/io/gamgi_io_file.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************


static gamgi_bool static_tokens (char *string, char *tokens,
int n_tokens, gamgi_bool end);

static gamgi_bool static_pattern (char *string, char *pattern);

static gamgi_bool static_line (int socket,
char *buffer, char *line, int *available);

static char *static_read (int socket, 
int code, char *buffer, char *line);

static gamgi_bool static_passive (unsigned char socket, char *line,
unsigned char *a1, unsigned char *a2, unsigned char *a3,
unsigned char *a4, unsigned char *p1, unsigned char *p2);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_io_file_patterns (char *file, const char *filters);

char *gamgi_io_file_shortname (const char *fullname);

char *gamgi_io_file_extension (const char *fullname);

void gamgi_io_file_fprintf (FILE *fp, char *string, gamgi_bool *error);

char *gamgi_io_file_name (const char *filename, gamgi_window *window);

gamgi_bool gamgi_io_file_http (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data);

gamgi_bool gamgi_io_file_ftp (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data);

gamgi_bool gamgi_io_file_local (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data);

gamgi_bool gamgi_io_file_protocol (const char *fullname, char *filename, 
gamgi_parser parser, gamgi_window *window, void *data);

gamgi_bool gamgi_io_file_read (char *filename, gamgi_window *window);

gamgi_bool gamgi_io_file_shell (int argc,
char **argv, gamgi_window *window);

gamgi_bool gamgi_io_file_export_ps (FILE *fp,
int width, int height, gamgi_bool *error);

gamgi_bool gamgi_io_file_export_ppm (FILE *fp,
int width, int height, gamgi_bool *error);

gamgi_bool gamgi_io_file_export_png (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error);

gamgi_bool gamgi_io_file_export_jpeg (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error);

gamgi_bool gamgi_io_file_export_tiff (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error);

gamgi_bool gamgi_io_file_write (char *filename, gamgi_window *window);

void gamgi_io_file_overwrite (char *filename, 
gamgi_callback2 function, gamgi_window *window);

/*************
 * Rationale *
 *************

 *************
 * Rationale *
 *************/
