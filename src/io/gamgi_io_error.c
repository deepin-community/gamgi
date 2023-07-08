/******************************************
 *
 * $GAMGI/src/io/gamgi_io_error.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_io_token.h"

/***************** external function ********************
 *                                                      *
 *             GAMGI_EXPAT_ERROR_ELEMENT                *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_element (const char *element, 
char *filename, int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];
char shortname[GAMGI_IO_SHORTNAME];

/**************************************************
 * element is wrong, it could be anything.        *
 * Regarding its size, the only guarantee         *
 * at this stage is that it is not larger than    *
 * GAMGI_EXPAT_BUFFER, which is usually a large   *
 * number (equal to BUFSIZ by default, which      *
 * is defined in most systems as 8192). Copying   *
 * only its first GAMGI_IO_SHORTNAME char (and    *
 * filling with '\0' unused positions) guarantees *
 * that the resulting shortname string will not   *
 * blow away the message char array.              *
 **************************************************/

gamgi_io_token_cut (element, shortname, GAMGI_IO_SHORTNAME);

sprintf (message,
"Invalid element \"%s\" at line %d in file \"%s\"",
shortname, fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/****************** external function *******************
 *                                                      *
 *             GAMGI_EXPAT_IMPORT_ATTRIBUTE             *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_attribute (const char *attribute,
char *filename, int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];
char shortname[GAMGI_IO_SHORTNAME];

/**************************************************
 * attribute is wrong, it could be anything.      *
 * Regarding its size, the only guarantee         *
 * at this stage is that it is not larger than    *
 * GAMGI_EXPAT_BUFFER, which is usually a large   *
 * number (equal to BUFSIZ by default, which      *
 * is defined in most systems as 8192). Copying   *
 * only its first GAMGI_IO_SHORTNAME char (and    *
 * filling with '\0' unused positions) guarantees *
 * that the resulting shortname string will not   *
 * blow away the message char array.              *
 **************************************************/

gamgi_io_token_cut (attribute, shortname, GAMGI_IO_SHORTNAME);

sprintf (message,
"Invalid attribute \"%s\" in element starting at line %d in file \"%s\"",
shortname, fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/***************** external function ********************
 *                                                      *
 *              GAMGI_EXPAT_IMPORT_VALUE                *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_value (const char *value,
char *filename, int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];
char shortname[GAMGI_IO_SHORTNAME];

/**************************************************
 * value is wrong, it could be anything.          *
 * Regarding its size, the only guarantee         *
 * at this stage is that it is not larger than    *
 * GAMGI_EXPAT_BUFFER, which is usually a large   *
 * number (equal to BUFSIZ by default, which      *
 * is defined in most systems as 8192). Copying   *
 * only its first GAMGI_IO_SHORTNAME char (and    *
 * filling with '\0' unused positions) guarantees *
 * that the resulting shortname string will not   *
 * blow away the message char array.              *
 **************************************************/

gamgi_io_token_cut (value, shortname, GAMGI_IO_SHORTNAME);

sprintf (message,
"Invalid parameter \"%s\" in element starting at line %d in file \"%s\"",
shortname, fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/***************** external function ********************
 *                                                      *
 *             GAMGI_EXPAT_IMPORT_CHILD                 *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_child (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Invalid parent for element starting at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/***************** external function ********************
 *                                                      *
 *             GAMGI_EXPAT_IMPORT_MISSING               *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_missing (char *filename, 
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Missing parameters in element starting at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/***************** external function ********************
 *                                                      *
 *           GAMGI_EXPAT_IMPORT_INCOMPATIBLE            *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_incompatible (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Incompatible parameters in element starting at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

/***************** external function ********************
 *                                                      *
 *              GAMGI_EXPAT_IMPORT_EMPTY                *
 *                                                      *
 ********************************************************/

gamgi_bool gamgi_io_error_empty (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Invalid empty element ending at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_multiple (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Multiple object definition in element starting at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_resources (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Resources exceeded in element starting at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_ref (const char *ref,
char *filename, int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

/********************************************
 * ref was tested before: it is a safe name *
 ********************************************/

sprintf (message,
"Unknown id \"%s\" in element starting at line %d in file \"%s\"", 
ref, fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_cdata (char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Invalid character data starting at line %d in file %s",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_header (const char *identifier, 
char *filename, int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];
char shortname[GAMGI_IO_SHORTNAME];

/**************************************************
 * identifier is wrong, it could be anything.     *
 * Regarding its size, the only guarantee         *
 * at this stage is that it is not larger than    *
 * GAMGI_EXPAT_BUFFER, which is usually a large   *
 * number (equal to BUFSIZ by default, which      *
 * is defined in most systems as 8192). Copying   *
 * only its first GAMGI_IO_SHORTNAME char (and    *
 * filling with '\0' unused positions) guarantees *
 * that the resulting shortname string will not   *
 * blow away the message char array.              *
 **************************************************/

gamgi_io_token_cut (identifier, shortname, GAMGI_IO_SHORTNAME);

sprintf (message,
"Invalid header identifier \"%s\" \n at line %d in file \"%s\"", 
shortname, fileline, filename); 

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_parse (char *filename, 
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Invalid file format at line %d in file \"%s\"",
fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_font (char *font, char *filename,
int fileline, gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Unable to handle font file \"%s\" in element starting at line %d in file \"%s\"",
font, fileline, filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_open (char *filename,
gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];
char shortname[GAMGI_IO_SHORTNAME];

/*******************************************
 * filename is wrong, it could be anything *
 *******************************************/

gamgi_io_token_cut (filename, shortname, GAMGI_IO_SHORTNAME);

sprintf (message,
"Unable to open file \"%s\"", shortname);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_read (char *filename,
gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Unable to read file \"%s\"", filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_write (char *filename,
gamgi_window *window)
{
char message[GAMGI_ENGINE_LINE];

sprintf (message,
"Unable to write file \"%s\"", filename);

gamgi_gtk_dialog_message_create ("Error", message, window);

return FALSE;
}

gamgi_bool gamgi_io_error_plane (gamgi_window *window)
{
gamgi_gtk_dialog_message_create ("Error",
"Invalid atomic plane", window);

return FALSE;
}

gamgi_bool gamgi_io_error_direction (gamgi_window *window)
{
gamgi_gtk_dialog_message_create ("Error",
"Invalid atomic direction", window);

return FALSE;
}

gamgi_bool gamgi_io_error_local (gamgi_window *window)
{
gamgi_gtk_dialog_message_create ("Error",
"Invalid Local Path", window);

return FALSE;
}

gamgi_bool gamgi_io_error_remote (gamgi_window *window)
{
gamgi_gtk_dialog_message_create ("Error",
"Invalid Remote Path", window);

return FALSE;
}

gamgi_bool gamgi_io_error_browser (gamgi_window *window)
{
gamgi_gtk_dialog_message_create ("Error",
"Invalid Browser Path", window);

return FALSE;
}

void gamgi_io_error_list (gamgi_slist *start, gamgi_window *window)
{
gamgi_object *object;
gamgi_slist *slist;
char line[GAMGI_ENGINE_LINE];
char string[GAMGI_ENGINE_STRING];

if (start == NULL) return;

start = gamgi_engine_slist_invert (start);
sprintf (line, "The following objects could not be handled:\n");
for (slist = start; slist != NULL; slist = slist->next)
  {
  if (slist != start) strcat (line, ", ");
  if (strlen (line) > GAMGI_ENGINE_LINE - GAMGI_ENGINE_STRING)
    { strcat (line, "..."); break; }

  object = GAMGI_CAST_OBJECT slist->data;
  sprintf (string, "%s %d", object->name, object->number);
  strcat (line, string);
  }

gamgi_gtk_dialog_message_create ("Error", line, window);
gamgi_engine_slist_remove_all (start);
}
