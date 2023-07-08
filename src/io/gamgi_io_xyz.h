/*******************************************
 *
 * $GAMGI/src/io/gamgi_io_xyz.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

int static_line (char *buffer, int size,
int *offset, char *line, int *available);

static int static_parse (char *line, gamgi_xyz *xyz);

static gamgi_bool static_import (char *buffer,
int size, char *line, int *available, gamgi_xyz *xyz);

static gamgi_xyz *static_create (gamgi_window *window);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_io_xyz_parse (int fd, char *body, int size, void *data);

gamgi_bool gamgi_io_xyz_import (char *filename, gamgi_window *window);

gamgi_bool gamgi_io_xyz_export (FILE *fp, gamgi_window *window, 
gamgi_bool *error);

/*************
 * Rationale *
 *************

 *************
 * Rationale *
 *************/
