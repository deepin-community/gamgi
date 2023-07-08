/*******************************************
 *
 * $GAMGI/src/io/gamgi_io_xyz.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"
#include "gamgi_gtk.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_count.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_io_token.h"
#include "gamgi_io_file.h"
#include "gamgi_io_error.h"
#include "gamgi_mesa_start.h"
#include "gamgi_chem_atom.h"

int static_line (char *buffer, int size, 
int *offset, char *line, int *available)
{
while (*offset < size)
  {
  if (buffer[*offset] != '\n')
    {
    if (*available == GAMGI_ENGINE_LINE - 1) return -1;
    line[(*available)++] = buffer[(*offset)++];
    }
  else
    {
    line[*available] = '\0';
    *available = 0;
    (*offset)++;
    return 1;
    }
  }

return 0;
}

static int static_parse (char *line, gamgi_xyz *xyz)
{
gamgi_atom *atom;
char token[GAMGI_ENGINE_TOKEN];
char dummy;

if (gamgi_io_token_check (line) == FALSE) return 1;

switch (xyz->fileline)
  {
  case 1:
  /********************************************
   * use a dummy char to ensure that there is *
   * nothing in the line after the valid data *
   ********************************************/

  if (sscanf (line, "%d %c\n", &xyz->count, &dummy) != 1 || 
  xyz->count <= 0) return -1;
  break;

  case 2:
  /**********************************
   * comment line with a maximum of *
   * GAMGI_ENGINE_LINE-1 characters *
   **********************************/
  break;

  default:
  /*******************************************
   * when an error is found the molecule is  *
   * removed, including all the linked atoms *
   *******************************************/

  atom = gamgi_engine_create_atom ();
  gamgi_engine_start_atom (atom);
  gamgi_mesa_start_atom (atom, gamgi->atom);
  gamgi_engine_link_atom_molecule (atom, xyz->molecule);

  /********************************************
   * use a dummy char to ensure that there is *
   * nothing in the line after the valid data *
   ********************************************/

  if (sscanf (line, "%3s %lf %lf %lf %c\n", atom->object.name, 
  &atom->position[0], &atom->position[1], &atom->position[2], &dummy) != 4 || 
  gamgi_chem_atom_number (atom->object.name, &atom->element) == FALSE ||
  atom->element < 0 || atom->element > GAMGI_CHEM_ATOM_MAX) return -1;

  /***************************
   * set default atomic data *
   ***************************/

  gamgi_chem_atom_name (atom->element, token);
  strcpy (atom->object.name, token);

  gamgi_io_token_lower (token, token+10);
  strcpy (atom->object.label, token+10);

  atom->mass = gamgi->atom->mass[atom->element];
  atom->radius = gamgi->atom->radius[atom->element];

  atom->red = gamgi->atom->red[atom->element];
  atom->green = gamgi->atom->green[atom->element];
  atom->blue = gamgi->atom->blue[atom->element];
  }

xyz->fileline++;

return 1;
}

static gamgi_bool static_import (char *buffer, 
int size, char *line, int *available, gamgi_xyz *xyz)
{
gamgi_bool valid;
int offset = 0;

while ((valid = static_line (buffer, size, &offset, line, available)) > 0
&& (valid = static_parse (line, xyz)) > 0);

if (valid < 0) return FALSE;

return TRUE;
}

static gamgi_xyz *static_create (gamgi_window *window)
{
gamgi_xyz *xyz;

xyz = (gamgi_xyz *) malloc (sizeof (gamgi_xyz));

xyz->window = window;
xyz->molecule = NULL;
xyz->fileline = 1;
xyz->count = 0;

return xyz;
}

gamgi_bool gamgi_io_xyz_parse (int fd,
char *body, int size, void *data)
{
gamgi_xyz *xyz = (gamgi_xyz *) data;
char buffer[GAMGI_IO_BUFFER];
char line[GAMGI_ENGINE_LINE];
int available = 0;
int count = 0;

/***************************************************
 * after parsing the HTTP header, the buffer still *
 * contains the initial part of the file, which    *
 * must be parsed before reading more.             *
 *                                                 *
 * Local files and files obtained by FTP don't     *
 * have a header, so buffer is NULL and size is 0  *
 ***************************************************/

if (static_import (body, size, line, &available, xyz) == FALSE)
  return gamgi_io_error_parse (xyz->filename, xyz->fileline, xyz->window);

while (1 > 0)
  {
  size = read (fd, buffer, GAMGI_IO_BUFFER);
  if (size < 0) return gamgi_io_error_read (xyz->filename, xyz->window);

  /***************
   * end of file *
   ***************/

  if (size == 0) break;

  if (static_import (buffer, size, line, &available, xyz) == FALSE)
    return gamgi_io_error_parse (xyz->filename, xyz->fileline, xyz->window);
  }

/**********************************************
 * bad termination: last line has no newlline *
 **********************************************/

if (available != 0) 
  return gamgi_io_error_parse (xyz->filename, xyz->fileline, xyz->window);

/***************************************
 * make sure the number of atoms found *
 * is equal to the number in the file  *
 ***************************************/

gamgi_engine_count_molecule (xyz->molecule, GAMGI_ENGINE_ATOM, &count);
if (xyz->count != count)
  return gamgi_io_error_parse (xyz->filename, xyz->fileline, xyz->window);

return TRUE;
}

gamgi_bool gamgi_io_xyz_import (char *fullname, gamgi_window *window)
{
gamgi_xyz *xyz;
char *filename;
gamgi_bool valid;

/********************************************
 * make sure fullname is an acceptable name *
 ********************************************/

filename = gamgi_io_file_name (fullname, window);
if (filename == NULL) return FALSE;

/********************
 * create resources *
 ********************/

xyz = static_create (window);
xyz->filename = filename;

/*******************************
 * this molecule contains all  *
 * the atoms found in the file *
 *******************************/

xyz->molecule = gamgi_engine_create_molecule ();
gamgi_engine_start_molecule (xyz->molecule);
strcpy (xyz->molecule->object.name, "Molecule");

/*******************************************
 * get local, HTTP or FTP file and read it *
 *******************************************/

valid = gamgi_io_file_protocol (fullname,
xyz->filename, gamgi_io_xyz_parse, window, xyz);

/*******************************************
 * remove everything if the file was wrong *
 * link to the current layer if it was ok  *
 *******************************************/

if (valid == FALSE)
  gamgi_engine_remove_molecule (xyz->molecule);
else
  gamgi_engine_link_molecule_layer (xyz->molecule, xyz->window->layer);

/******************
 * free resources *
 ******************/

free (xyz);

return valid;
}

gamgi_bool gamgi_io_xyz_export (FILE *fp,
gamgi_window *window, gamgi_bool *error)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
char name[GAMGI_ENGINE_TOKEN];
int number = 0;

dlist = gamgi_engine_dlist_atom_object (window->focus, NULL);

gamgi_engine_count_object (window->focus, GAMGI_ENGINE_ATOM, &number);
sprintf (string, "%d\n", number);
gamgi_io_file_fprintf (fp, string, error);

sprintf (string, "Made with GAMGI %s\n", GAMGI_IO_VERSION);
gamgi_io_file_fprintf (fp, string, error);

while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  gamgi_chem_atom_name (atom->element, name);

  sprintf (string, "%3s %15.*f %15.*f %15.*f\n", name,
  gamgi->gamgi->length, atom->position[0],
  gamgi->gamgi->length, atom->position[1],
  gamgi->gamgi->length, atom->position[2]);

  gamgi_io_file_fprintf (fp, string, error);
  dlist = gamgi_engine_dlist_remove_start (dlist);
  }

return *error;
}
