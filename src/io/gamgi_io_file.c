/******************************************
 *
 * $GAMGI/src/io/gamgi_io_file.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_expat.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_count.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_mesa_area.h"
#include "gamgi_expat_import.h"
#include "gamgi_expat_export.h"
#include "gamgi_io_socket.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_io_xyz.h"

#include <fcntl.h>

static gamgi_bool static_tokens (char *string, char *tokens,
int n_tokens, gamgi_bool end)
{
gamgi_bool start;
char *substring;
int i, length = 0;

for (i = 0; i < n_tokens; i++)
  {
  if (tokens[0] == '\0') { start = TRUE; tokens++; }
  else start = FALSE;

  if ((substring = strstr (string, tokens)) == NULL) return FALSE;
  if (start == FALSE && substring != string) return FALSE;

  length = strlen (tokens);
  string = substring;
  tokens += length;
  }

if (end == FALSE && strstr (string + strlen (string) - length,
tokens - length) == NULL) return FALSE;

return TRUE;
}

static gamgi_bool static_pattern (char *string, char *pattern)
{
gamgi_bool start, end;
char *tokens = pattern;
int i, n_tokens;

/********************************************
 * convert the whole pattern sequence to a  *
 * list of patterns separated by '\0' chars *
 ********************************************/

n_tokens = 0;
for (i = 0; *pattern != '\0'; pattern++)
  {
  if (*pattern == '*')
    {
    if (*(pattern + 1) == '*') continue;
    tokens[i++] = '\0';
    n_tokens++;
    }
  else tokens[i++] = *pattern;
  }
tokens[i] = '\0';

if (tokens[0] == '\0') start = TRUE;
else start = FALSE;

if (tokens[i - 1] == '\0') end = TRUE;
else end = FALSE;

if (start == TRUE && end == TRUE) n_tokens--;
if (start == FALSE && end == FALSE) n_tokens++;

return static_tokens (string, tokens, n_tokens, end);
}

static gamgi_bool static_line (int socket,
char *buffer, char *line, int *available)
{
static char *start;
char *end;
int vacant, copied;

/*******************
 * initialize line *
 *******************/

strcpy (line, "");
vacant = GAMGI_ENGINE_LINE - 1;

do
  {
  /********************************
   * read from socket into buffer *
   ********************************/

  if (*available == 0)
    {
    *available = read (socket, buffer, GAMGI_IO_BUFFER);
    if (*available <= 0) return FALSE;

    start = buffer;
    }

  end = strchr (start, '\n');
  if (end == NULL) end = start + *available - 1;

  /********************
   * \n is not copied *
   ********************/

  copied = end - start;
  if (copied > vacant) return FALSE;
  strncat (line, start, copied);
  vacant -= copied;

  /****************
   * add 1 for \n *
   ****************/

  *available -= copied + 1;
  start += copied + 1;
  }
  while (end == NULL);

return TRUE;
}

static char *static_read (int socket, 
int code, char *buffer, char *line)
{
char more[5], end[5];
int available = 0;

sprintf (more, "%d-", code);
sprintf (end, "%d ", code);

do
  {
  if (static_line (socket, buffer, line, &available) == FALSE || 
  strlen (line) < 4) return NULL;

  line[4] = '\0';

  if (strcmp (line, end) == 0) return line;

  if (strcmp (line, more) != 0) return NULL;
  }
  while (1 > 0);
}

static gamgi_bool static_passive (unsigned char socket, char *line, 
unsigned char *a1, unsigned char *a2, unsigned char *a3, 
unsigned char *a4, unsigned char *p1, unsigned char *p2)
{
int n1, n2, n3, n4;

/**********************
 * retrieve ip number *
 **********************/

line = strpbrk (line+5, "0123456789");
if (sscanf (line, " %d, %d, %d, %d ", &n1, &n2, &n3, &n4) != 4)
  return FALSE;

*a1 = n1; *a2 = n2; *a3 = n3; *a4 = n4; 

/************************
 * retrieve port number *
 ************************/

line = strchr (line+1, ','); line = strchr (line+1, ',');
line = strchr (line+1, ','); line = strchr (line+1, ',');
if (sscanf (line+1, " %d, %d ", &n1, &n2) != 2)
  return FALSE;

*p1 = n1; *p2 = n2;

return TRUE;
}

gamgi_bool gamgi_io_file_patterns (char *file, const char *filters)
{
char patterns[GAMGI_GTK_FILE_PATTERNS + 1];
char *pattern;

/******************************************
 * divide sequence in individual patterns *
 ******************************************/
 
strcpy (patterns, filters);
pattern = strtok (patterns, " ,");
do
  {
  if (static_pattern (file, pattern) == TRUE) return TRUE;
  }
  while ((pattern = strtok (NULL, " ,")) != NULL);
   
return FALSE;
}

char *gamgi_io_file_shortname (const char *fullname)
{
char *shortname;

shortname = strrchr (fullname, '/');
if (shortname != NULL) shortname++;
else shortname = (char *) fullname;

return shortname;
}

char *gamgi_io_file_extension (const char *fullname)
{
char *extension;

/**********************
 * get file extension *
 **********************/

extension = strrchr (fullname, '.');
if (extension != NULL)
  {
  extension++;

  /*******************************
   * make sure the dot is in the *
   * file and not in a directory *
   *******************************/

  if (strchr (extension, '/') != NULL)
    extension = NULL;
  }

return extension;
}

/***************** external function *********************
 *                                                       *
 *********************************************************/

void gamgi_io_file_fprintf (FILE *fp, char *string, gamgi_bool *error)
{
/***************************************************************
 * We set the error flag as FALSE before the first fprintf     *
 * call and check its state only after the last fprintf. If    *
 * error is now TRUE, then at least one fprintf call failed,   *
 * in which case we show an error message saying that we were  *
 * unable to successfully write the file and ask the user if   *
 * we should remove it (it is certainly corrupted, but reading *
 * it might give the user a clue anyway about what went wrong) *
 *                                                             *
 * The upside of this procedure is that our code is much       *
 * simpler because we are not taking action immediately        *
 * if fprintf fails (at this stage the file is already open    *
 * in write mode, with sucess, so permission errors are not    *
 * likely to occur here, a typical error would occur here      *
 * for example if the hard disk filesystem fills up). We       *
 * can do this because we are not compromising Gamgi or the    *
 * system, just the file, which we remove (or not) in the end. *
 *                                                             *
 * The downside of this procedure is that our message in       *
 * the end cannot tell the user where the failure occurred.    *
 * This should not be a problem though, because as soon as     *
 * one fprintf fails, all the other should fail as well and    *
 * inspecting directly the file should clarify everything.     *
 ***************************************************************/

if (fprintf (fp, "%s", string) < 0) *error = TRUE;
}

char *gamgi_io_file_name (const char *fullname, 
gamgi_window *window)
{
char *name, *shortname;

shortname = gamgi_io_file_shortname (fullname);

/************************************************
 * make sure the shortname used for example     *
 * in error messages is not too long and always *
 * fits inside the message string arrays.       *
 ************************************************/

if (gamgi_io_token_size_check (shortname, GAMGI_IO_SHORTNAME) == FALSE)
  {
  gamgi_io_error_open (shortname, window);
  return NULL;
  }

/***************************************
 * make sure fullname is not too long, *
 * so filename and hostname always fit *
 * inside their string arrays          *
 **************************************/

if (gamgi_io_token_size_check (fullname, GAMGI_IO_FULLNAME) == FALSE)
  {
  gamgi_io_error_open (shortname, window);
  return NULL;
  }

/***************************************
 * HTTP label must be in the beginning *
 ***************************************/

name = strstr (fullname, "http://");
if (name != NULL && name != fullname)
  {
  gamgi_io_error_open (shortname, window);       
  return NULL;
  }

/**************************************
 * FTP label must be in the beginning *
 **************************************/

name = strstr (fullname, "ftp://");
if (name != NULL && name != fullname)
  {
  gamgi_io_error_open (shortname, window);       
  return NULL;
  }

return shortname;
}

gamgi_bool gamgi_io_file_http (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data)
{
int fd, size, number;
char buffer[GAMGI_IO_BUFFER];
char filename[GAMGI_ENGINE_LINE];
char hostname[GAMGI_ENGINE_LINE];
char *body;
gamgi_bool valid;

/*****************************
 * get hostname and filename *
 *****************************/

fullname += strlen("http://");

size = strcspn (fullname, "/");
strcpy (filename, fullname + size);

strncpy (hostname, fullname, size);
hostname[size] = '\0';

/*******************
 * open connection *
 *******************/

fd = gamgi_io_socket_name ("http", hostname);
if (fd < 0) return gamgi_io_error_open (shortname, window);

/*****************************************
 * write instructions to the HTTP server *
 *****************************************/

sprintf (buffer, "GET %s HTTP/1.1\r\n" "Host: %s\r\n"
"Pragma: no-cache\r\n" "Connection: close\r\n\r\n",
filename, hostname);
write (fd, buffer, strlen (buffer));

/**********************
 * handle HTTP header *
 **********************/

/***************************
 * check header first line *
 ***************************/

size = read (fd, buffer, GAMGI_IO_BUFFER);
if (size <= 0 || sscanf (buffer, "%*s %d %*s \n", &number) != 1
|| number != GAMGI_IO_HTTP_OK)
  {
  close (fd);
  return gamgi_io_error_open (shortname, window);
  }

do
  {
  /*******************************
   * ignore everything and just  *
   * search for header last line *
   *******************************/

  body = strstr (buffer, "\r\n\r\n");
  if (body != NULL) break;

  size = read (fd, buffer, GAMGI_IO_BUFFER);
  if (size <= 0) 
  {
  close (fd);
  return gamgi_io_error_open (shortname, window);
  }

  } while (1 > 0);

if (body == NULL)
  {
  close (fd);
  return gamgi_io_error_open (shortname, window);
  }

/********************************************
 * move forward the 4 char "\r\n\r\n" and   *
 * discount the header characters in buffer *
 ********************************************/

body += 4;
size -= body - buffer;

/*******************
 * parse HTTP file *
 *******************/

valid = (parser) (fd, body, size, data);

close (fd);

return valid;
}

gamgi_bool gamgi_io_file_ftp (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data)
{
int fd_main, fd_data, size;
char buffer[GAMGI_IO_BUFFER], line[GAMGI_ENGINE_LINE];
char filename[GAMGI_ENGINE_LINE], hostname[GAMGI_ENGINE_LINE];
char *last;
unsigned char a1, a2, a3, a4, p1, p2;
gamgi_bool valid;

/*****************************
 * get hostname and filename *
 *****************************/

fullname += strlen("ftp://");

size = strcspn (fullname, "/");
strcpy (filename, fullname + size);

strncpy (hostname, fullname, size);
hostname[size] = '\0';

/********************
 * open main socket *
 ********************/

if ((fd_main = gamgi_io_socket_name ("ftp", hostname)) < 0)
  return gamgi_io_error_open (shortname, window);

if (static_read (fd_main, GAMGI_IO_FTP_GREETINGS, buffer, line) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/******************
 * enter username *
 ******************/

sprintf (buffer, "USER anonymous\r\n");
if (write (fd_main, (char *) buffer, strlen (buffer)) < 0)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

if (static_read (fd_main, GAMGI_IO_FTP_USER, buffer, line) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/******************
 * enter password *
 ******************/

sprintf (buffer, "PASS gamgi@www.gamgi.org\r\n");
if (write (fd_main, (char *) buffer, strlen (buffer)) < 0)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

if (static_read (fd_main, GAMGI_IO_FTP_PASSWORD, buffer, line) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/**********************************
 * set data type (text or binary) *
 **********************************/

sprintf (buffer, "TYPE A\r\n");
if (write (fd_main, (char *) buffer, strlen (buffer)) < 0)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

if (static_read (fd_main, GAMGI_IO_FTP_TYPE, buffer, line) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/************************************
 * enter passive mode: get IP, Port *
 ************************************/

sprintf (buffer, "PASV\r\n");
if (write (fd_main, (char *) buffer, strlen (buffer)) < 0)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

if ((last = static_read (fd_main, GAMGI_IO_FTP_PASSIVE, buffer, line)) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

if (static_passive (fd_main, last, &a1, &a2, &a3, &a4, &p1, &p2) == FALSE)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/********************
 * open data socket *
 ********************/

if ((fd_data = gamgi_io_socket_number ("ftp", a1, a2, a3, a4, p1, p2)) < 0)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

/****************
 * ask for file *
 ****************/

sprintf (buffer, "RETR %s\r\n", filename);
if (write (fd_main, (char *) buffer, strlen (buffer)) < 0)
  {
  close (fd_main);
  close (fd_data);
  return gamgi_io_error_open (shortname, window);
  }

if (static_read (fd_main, GAMGI_IO_FTP_FILE, buffer, line) == NULL)
  {
  close (fd_main);
  close (fd_data);
  return gamgi_io_error_open (shortname, window);
  }

/************************************
 * parse FTP file using data socket *
 ************************************/

valid = (parser) (fd_data, NULL, 0, data);

close (fd_data);

if (static_read (fd_main, GAMGI_IO_FTP_READ, buffer, line) == NULL)
  {
  close (fd_main);
  return gamgi_io_error_open (shortname, window);
  }

close (fd_main);

return valid;
}

gamgi_bool gamgi_io_file_local (const char *fullname, char *shortname, 
gamgi_parser parser, gamgi_window *window, void *data)
{
int fd;
gamgi_bool valid;

/*************
 * open file *
 *************/

fd = open (fullname, O_RDONLY);
if (fd < 0) return gamgi_io_error_open (shortname, window);

/**************
 * parse file *
 **************/

valid = (parser) (fd, NULL, 0, data);

/**************
 * close file *
 **************/

close (fd);

return valid;
}

gamgi_bool gamgi_io_file_protocol (const char *fullname, char *filename, 
gamgi_parser parser, gamgi_window *window, void *data)
{
if (strstr (fullname, "http://") != NULL)
  return gamgi_io_file_http (fullname, filename, parser, window, data);
else if (strstr (fullname, "ftp://") != NULL)
  return gamgi_io_file_ftp (fullname, filename, parser, window, data);
else
  return gamgi_io_file_local (fullname, filename, parser, window, data);
}

gamgi_bool gamgi_io_file_export_ps (FILE *fp,
int width, int height, gamgi_bool *error)
{
int components, i;
unsigned char *pixels;
char string[GAMGI_ENGINE_LINE];

/*************
 * grab data *
 *************/

components = 3;
pixels = gamgi_mesa_area_read (0, 0, width, height);

/***********************************************************
 * Write header, adapted from: 1) the header produced      *
 * by Gimp 2.4.6 when exporting .eps encapsulated          *
 * postcript files (without level 2), written by Peter     *
 * Kirchgessner; 2) the book Postcript Language, Tutorial  *
 * and Cookbook, Adobe Systems Inc., Addison-Wesley, 1986  *
 ***********************************************************/

gamgi_io_file_fprintf (fp, "%!PS-Adobe-3.0 EPSF-3.0\n", error);
sprintf (string, "%%%%Creator: GAMGI %s\n", GAMGI_IO_VERSION);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "%%DocumentData: Clean7Bit\n", error);
gamgi_io_file_fprintf (fp, "%%LanguageLevel: 2\n", error);
gamgi_io_file_fprintf (fp, "%%Pages: 1\n", error);
sprintf (string, "%%%%BoundingBox: 0 0 %d %d\n", width, height);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "%%EndComments\n", error);
sprintf (string, "/scanline %d string def\n", width * components);
gamgi_io_file_fprintf (fp, string, error);
sprintf (string, "%d %d scale\n", width, height);
gamgi_io_file_fprintf (fp, string, error);
sprintf (string, "%d %d %d\n", width, height, 8);
gamgi_io_file_fprintf (fp, string, error);
sprintf (string, "[%d 0 0 %d 0 0]\n", width, height);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "{currentfile scanline readhexstring pop}\n", error);
sprintf (string, "false %d\n", components);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "colorimage\n", error);

/********************************************
 * Write data: adapted from the book OpenGL *
 * Programming for the X Window System p98, *
 * Mark J. Kilgard, Addison-Wesley, 1996.   *
 ********************************************/

i = 0;
while (i < width * height * components)
  {
  sprintf (string, "%02x", pixels[i++]);
  gamgi_io_file_fprintf (fp, string, error);
  if (i % 40 == 0) gamgi_io_file_fprintf (fp, "\n", error);
  }
if (i % 40 != 0) gamgi_io_file_fprintf (fp, "\n", error);

/*******************************************
 * show postscript page and free resources *
 *******************************************/
 
gamgi_io_file_fprintf (fp, "showpage\n", error);
free (pixels);

return TRUE;
}

gamgi_bool gamgi_io_file_export_ppm (FILE *fp,
int width, int height, gamgi_bool *error)
{
unsigned char *pixels;
char string[GAMGI_ENGINE_LINE];
int i, components, maximum, offset;

/***************************************************************
 * grab data: assume a maximum depth of 8 bits per rgb channel *
 * so each color component can take values between 0 and 255   *
 ***************************************************************/

components = 3;
pixels = gamgi_mesa_area_read (0, 0, width, height);
maximum = GAMGI_IO_PPM_MAX;

/****************
 * write header *
 ****************/

gamgi_io_file_fprintf (fp, "P3\n", error);

sprintf (string, "#Made with GAMGI %s\n", GAMGI_IO_VERSION);
gamgi_io_file_fprintf (fp, string, error);

sprintf (string, "%d %d\n%d\n", width, height, maximum);
gamgi_io_file_fprintf (fp, string, error);

/**********************************************
 * write data: from end to start, because ppm *
 * formats read data from top to bottom while *
 * OpenGL array grabs data from bottom to top *
 **********************************************/

while (height-- > 0)
  {
  offset = components * width * height;
  for (i = 0; i < width; i++)
    {
    sprintf (string, "%d %d %d ",
    pixels[offset + 0], pixels[offset + 1], pixels[offset + 2]);
    gamgi_io_file_fprintf (fp, string, error);
    offset += components;
    }
  gamgi_io_file_fprintf (fp, "\n", error);
  }

/******************
 * free resources *
 ******************/

free (pixels);

return TRUE;
}

gamgi_bool gamgi_io_file_export_png (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error)
{
char auxname[2 * GAMGI_ENGINE_TOKEN];
char command[2 * GAMGI_ENGINE_LINE];

/**********************************
 * 1) create fullname ppm file    *
 * 2) convert to auxname png file *
 * (with special name, to avoid   *
 * destroying a user's file!)     *
 * 3) move auxname to fullname    *
 **********************************/

gamgi_io_file_export_ppm (fp, width, height, error);
if (*error != 0) return FALSE;

sprintf (auxname, "gamgi%d", getpid ());

sprintf (command, "(%s %s >%s; mv -f %s %s) &",
GAMGI_IO_PNMTOPNG, fullname, auxname, auxname, fullname);
system (command);

return TRUE;
}

gamgi_bool gamgi_io_file_export_jpeg (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error)
{
char auxname[2 * GAMGI_ENGINE_TOKEN];
char command[2 * GAMGI_ENGINE_LINE];

/**********************************
 * 1) create fullname ppm file    *
 * 2) convert to auxname png file *
 * (with special name, to avoid   *
 * destroying a user's file!)     *
 * 3) move auxname to fullname    *
 **********************************/

gamgi_io_file_export_ppm (fp, width, height, error);
if (*error != 0) return FALSE;

sprintf (auxname, "gamgi%d", getpid ());

sprintf (command, "(%s %s >%s; mv -f %s %s) &",
GAMGI_IO_PNMTOJPEG, fullname, auxname, auxname, fullname);
system (command);

return TRUE;
}

gamgi_bool gamgi_io_file_export_tiff (char *fullname,
FILE *fp, int width, int height, gamgi_bool *error)
{
char auxname[2 * GAMGI_ENGINE_TOKEN];
char command[2 * GAMGI_ENGINE_LINE];

/**********************************
 * 1) create fullname ppm file    *
 * 2) convert to auxname png file *
 * (with special name, to avoid   *
 * destroying a user's file!)     *
 * 3) move auxname to fullname    *
 **********************************/

gamgi_io_file_export_ppm (fp, width, height, error);
if (*error != 0) return FALSE;

sprintf (auxname, "gamgi%d", getpid ());

sprintf (command, "(%s -color %s >%s; mv -f %s %s) &",
GAMGI_IO_PNMTOTIFF, fullname, auxname, auxname, fullname);
system (command);

return TRUE;
}

gamgi_bool gamgi_io_file_read (char *fullname, gamgi_window *window)
{
char *extension;

/**********************
 * get file extension *
 **********************/

extension = gamgi_io_file_extension (fullname);

/*******************************
 * read according to extension *
 *******************************/

if (extension == NULL)
  return gamgi_expat_import_gml (fullname, window);
if (strcmp (extension, "xyz") == 0)
  return gamgi_io_xyz_import (fullname, window);
return gamgi_expat_import_gml (fullname, window);
}

gamgi_bool gamgi_io_file_shell (int argc,
char **argv, gamgi_window *window)
{
int i;

/***************************
 * load command line files *
 *                         *
 * if an error is found:   *
 * 1) ignore wrong file;   *
 * 2) show error message;  *
 * 3) ignore next files;   *
 ***************************/

for (i = 1; i < argc; i++)
  {
  if (gamgi_io_file_read (argv[i], window) == FALSE) return FALSE;
  }

return TRUE;
}

gamgi_bool gamgi_io_file_write (char *fullname, gamgi_window *window)
{
gamgi_bool error = FALSE;
GtkAllocation allocation;
FILE *fp;
char *shortname;
char *extension;
int width, height;

/************************************************
 * discard the pathname, keep only the filename *
 ************************************************/

shortname = gamgi_io_file_shortname (fullname);

/*******************************
 * open file in overwrite mode *
 *******************************/

fp = fopen (fullname, "w");
if (fp == NULL)
  {
  gamgi_io_error_write (shortname, window);
  return FALSE;
  }

/**********************
 * get file extension *
 **********************/

extension = gamgi_io_file_extension (fullname);

/*******************************************************
 * write according to extension: when the extension is *
 * nonexistent or unrecognized export a GAMGI xml file *
 *******************************************************/

gtk_widget_get_allocation (window->area, &allocation);
width = allocation.width;
height = allocation.height;

if (extension == NULL)
  gamgi_expat_export_gml (fp, window, &error);
else if (strcmp (extension, "jpeg") == 0 || strcmp (extension, "jpg") == 0)
  gamgi_io_file_export_jpeg (fullname, fp, width, height, &error);
else if (strcmp (extension, "png") == 0)
  gamgi_io_file_export_png (fullname, fp, width, height, &error);
else if (strcmp (extension, "eps") == 0 || strcmp (extension, "ps") == 0)
  gamgi_io_file_export_ps (fp, width, height, &error);
else if (strcmp (extension, "ppm") == 0)
  gamgi_io_file_export_ppm (fp, width, height, &error);
else if (strcmp (extension, "tiff") == 0 || strcmp (extension, "tif") == 0)
  gamgi_io_file_export_tiff (fullname, fp, width, height, &error);
else if (strcmp (extension, "x3d") == 0)
  gamgi_expat_export_x3d (fp, window, &error);
else if (strcmp (extension, "xyz") == 0)
  gamgi_io_xyz_export (fp, window, &error);
else
  gamgi_expat_export_gml (fp, window, &error);

/**************
 * close file *
 **************/

fclose (fp);

/******************************************
 * in the end, check if an error occurred *
 * during writing to file (disk full?)    *
 ******************************************/

if (error == TRUE)
  {
  gamgi_io_error_write (shortname, window);
  return FALSE;
  }

return TRUE;
}

void gamgi_io_file_overwrite (char *filename, 
gamgi_callback2 function, gamgi_window *window)
{
char string[GAMGI_ENGINE_LINE];
char *shortname;
FILE *fp;

/************************************************
 * discard the pathname, keep only the filename *
 ************************************************/

shortname = gamgi_io_file_shortname (filename);

/*****************************
 * open file in append mode, *
 * to check file existence   *
 *****************************/

fp = fopen (filename, "a+");
if (fp == NULL)
  { gamgi_io_error_write (shortname, window); 
    return; }

/*******************
 * overwrite file? *
 *******************/

fseek(fp, 0, SEEK_SET);
fgetc(fp);
if (feof(fp) == 0)
  { fclose (fp);
    sprintf (string, "Overwrite file \"%s\"?", shortname);
    gamgi_gtk_dialog_question_create ("Warning", string, function, window);
    return; }

/*****************************
 * close file in append mode *
 *****************************/

fclose (fp);
(*function) (NULL, window);
}
