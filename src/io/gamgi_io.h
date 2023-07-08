/*******************************************
 *
 * $GAMGI/src/io/gamgi_io.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_IO
#define GAMGI_IO

/******** libraries *******
 **************************/

#include <unistd.h>
#include <ft2build.h>
#include FT_GLYPH_H
#include FT_OUTLINE_H

/********************************************
 * This type defines pointers to functions  *
 * used to parse files, one for each format *
 *********************************************/

typedef gamgi_bool (* gamgi_parser) (int fd,
char *body, int size, void *data);

/**************************************
 * structure used to import xyz files *
 **************************************/

typedef struct _gamgi_xyz {
gamgi_window *window;
char *filename;
int fileline;
gamgi_molecule *molecule;
int count;
} gamgi_xyz;

/**************************************************
 * structure used to render extruded fonts:       *
 * dynamic arrays cannot be used to save vertices *
 * and extra, because addresses must be preserved *
 **************************************************/

typedef struct _gamgi_extruded {
FT_Outline_Funcs horizontal, vertical;
GLUtesselator* tesselator;

gamgi_slist *vertices, *extra;
gamgi_darray *contour, *normals;

double last[3];
double delta1, delta2, delta3, scale;
int normal_x, normal_y;
gamgi_bool open; } gamgi_extruded;

/********** enumerations **********
 **********************************/

/************************************************************
 * All GAMGI enumerations start at 1 and share FALSE for 0. *
 ************************************************************/

/*******************************************************************
 * Extruded (Solid) and Stroked (Wired) fonts implemented in GAMGI *
 *******************************************************************/

enum { GAMGI_IO_SUN_ROMAN = 1, GAMGI_IO_SUN_ROMAN_MONO,
GAMGI_IO_HERSHEY_SANS, GAMGI_IO_HERSHEY_SANS_BOLD,
GAMGI_IO_HERSHEY_SERIF, GAMGI_IO_HERSHEY_SERIF_BOLD,
GAMGI_IO_HERSHEY_SCRIPT, GAMGI_IO_HERSHEY_SCRIPT_BOLD, 
GAMGI_IO_HERSHEY_GOTHIC_ENGLISH, GAMGI_IO_HERSHEY_GOTHIC_GERMAN, 
GAMGI_IO_HERSHEY_GOTHIC_ITALIAN, GAMGI_IO_HERSHEY_SYMBOL_SANS,
GAMGI_IO_HERSHEY_SYMBOL_SERIF, GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD,

GAMGI_IO_DEJAVU_SANS, GAMGI_IO_DEJAVU_SANS_BOLD,
GAMGI_IO_DEJAVU_SANS_ITALIC, GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC,
GAMGI_IO_DEJAVU_SERIF, GAMGI_IO_DEJAVU_SERIF_BOLD,
GAMGI_IO_DEJAVU_SERIF_ITALIC, GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC,
GAMGI_IO_DEJAVU_MONO, GAMGI_IO_DEJAVU_MONO_BOLD,
GAMGI_IO_DEJAVU_MONO_ITALIC, GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC };

/************* macros ************
 *********************************/

/*****************************************
 * BUFSIZ is a standard buffer size,     *
 * often 8192, mentioned for example     *
 * by Dennis Ritchie and Brian Kernighan *
 * in "The ANSI C Programming Language." *
 *****************************************/

#define GAMGI_IO_BUFFER BUFSIZ

/*******************************
 * this is the data shown in   *
 * the Gamgi->About GUI dialog *
 *******************************/

#define GAMGI_IO_DATE "25th February 2022"
#define GAMGI_IO_VERSION "0.17.5g"
#define GAMGI_IO_AUTHORS "Carlos Pereira et al."
#define GAMGI_IO_LICENSE "GPL"
#define GAMGI_IO_WEBSITE "http://www.gamgi.org/"

/*****************************************
 * used to build local,remote Help paths *
 *****************************************/

#define GAMGI_IO_PATH_LOCAL ""
#define GAMGI_IO_PATH_REMOTE GAMGI_IO_WEBSITE

/***************************
 * current GML identifiers *
 ***************************/

#define GAMGI_IO_GML_SYSTEM "http://www.gamgi.org/dtd/gamgi.dtd"
#define GAMGI_IO_GML_PUBLIC ""
#define GAMGI_IO_GML_GMLNS ""

/********************************
 * Strict XHTML 1.0 identifiers *
 ********************************/

#define GAMGI_IO_HTML_PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
#define GAMGI_IO_HTML_SYSTEM "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
#define GAMGI_IO_HTML_XMLNS "http://www.w3.org/1999/xhtml"

/***********************
 * X3D 3.1 identifiers *
 ***********************/

#define GAMGI_IO_X3D_PUBLIC "ISO//Web3D//DTD X3D 3.1//EN"
#define GAMGI_IO_X3D_SYSTEM "http://www.web3d.org/specifications/x3d-3.1.dtd"
#define GAMGI_IO_X3D_PROFILE "Immersive"
#define GAMGI_IO_X3D_VERSION "3.1"

/********************************
 * These are the maximum number *
 * of characters that can be    *
 * used to designate files:     *
 * 1) the fullname, including   *
 *   protocol, hostname,        *
 *   pathname and filename;     *
 * 2) the filename only;        *
 * 3) the pathname used in:     *
 *    gamgi->help->local,       *
 *    gamgi->help->remote,      *
 *    gamgi->help->browser      *
 ********************************/

#define GAMGI_IO_FULLNAME 150
#define GAMGI_IO_SHORTNAME 50
#define GAMGI_IO_PATHNAME 100

#define GAMGI_IO_TITLE_FONT "-misc-fixed-medium-r-*-*-*-140-*-*-*-*-*-*"

/**************************************************************************
 * valid characters in alphanumeric tokens in GAMGI dialogs and GML files *
 **************************************************************************/

#define GAMGI_IO_TEXT \
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()[]{}<>_.+-"

/******************************************
 * valid characters in GAMGI text objects *
 ******************************************/

#define GAMGI_IO_FONT_ASCII \
"\t\n !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

/***************************************
 * valid characters in HTML Help files *
 ***************************************/

#define GAMGI_IO_FONT_88591 \
"\t\n !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~¢£¤¥¦§¨©ª«¬­®¯°±µ¶º»ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"

/********************************************
 * valid characters in cell occupancy rules *
 ********************************************/

#define GAMGI_IO_RULES "0123456789*_"

/*********************************************************
 * valid separators in GAMGI dialogs and HTML Help files *
 *********************************************************/

#define GAMGI_IO_DELIMITERS "\t\n "

/**************************************
 * marks used in font data, to signal *
 * stroke and character termination.  *
 **************************************/

#define GAMGI_IO_STROKE 500.0
#define GAMGI_IO_CHAR 1000.0

/******************************
 * Currently GAMGI supports   *
 * stroked characters with    *
 * ascii codes from 32 to 126 *
 ******************************/

#define GAMGI_IO_ASCII_START 32
#define GAMGI_IO_ASCII_RANGE 95

/**********************************************************
 * number of fonts: Wired (stroked), Solid (extruded) and *
   Total (Wired + Solid - Common). Four fonts are common  *
   to both styles: Sans, Sans_Bold, Serif, Serif_Bold     *
 **********************************************************/

#define GAMGI_IO_FONTS_WIRED 14
#define GAMGI_IO_FONTS_SOLID 12
#define GAMGI_IO_FONTS_TOTAL 22

/*****************************************
 * text defaults: wired font, solid font *
 *****************************************/

#define GAMGI_IO_TEXT_WIRED GAMGI_IO_SUN_ROMAN
#define GAMGI_IO_TEXT_SOLID GAMGI_IO_DEJAVU_SANS

/**********************************************
 * Truetype fonts: default path and filenames *
 **********************************************/

#define GAMGI_IO_PATH_TRUETYPE "/usr/share/fonts/truetype/ttf-dejavu/"

#define GAMGI_IO_FILENAME_DEJAVU_SANS "DejaVuSans.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SANS_BOLD "DejaVuSans-Bold.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SANS_ITALIC "DejaVuSans-Oblique.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SANS_BOLD_ITALIC "DejaVuSans-BoldOblique.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SERIF "DejaVuSerif.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SERIF_BOLD "DejaVuSerif-Bold.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SERIF_ITALIC "DejaVuSerif-Italic.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_SERIF_BOLD_ITALIC "DejaVuSerif-BoldItalic.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_MONO "DejaVuSansMono.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_MONO_BOLD "DejaVuSansMono-Bold.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_MONO_ITALIC "DejaVuSansMono-Oblique.ttf"
#define GAMGI_IO_FILENAME_DEJAVU_MONO_BOLD_ITALIC "DejaVuSansMono-BoldOblique.ttf"

/*******************************
 * Size of the font arrays     *
 * in gamgi_io_font_sun.c      *
 * and gamgi_io_font_hershey.c *
 *******************************/

#define GAMGI_IO_POINTS_ROMAN 2418
#define GAMGI_IO_POINTS_ROMAN_MONO 2418
#define GAMGI_IO_POINTS_SANS 2357
#define GAMGI_IO_POINTS_SANS_BOLD 5029
#define GAMGI_IO_POINTS_SERIF 4359
#define GAMGI_IO_POINTS_SERIF_BOLD 7537
#define GAMGI_IO_POINTS_SCRIPT 3845
#define GAMGI_IO_POINTS_SCRIPT_BOLD 5423
#define GAMGI_IO_POINTS_GOTHIC_ENGLISH 8667
#define GAMGI_IO_POINTS_GOTHIC_GERMAN 9199
#define GAMGI_IO_POINTS_GOTHIC_ITALIAN 7390
#define GAMGI_IO_POINTS_SYMBOL_SANS 2629
#define GAMGI_IO_POINTS_SYMBOL_SERIF 4599
#define GAMGI_IO_POINTS_SYMBOL_SERIF_BOLD 7484

/**********************************
 * codes used in the FTP protocol *
 * to determine if the server is  *
 * accepting the client requests  *
 **********************************/

#define GAMGI_IO_FTP_GREETINGS 220
#define GAMGI_IO_FTP_USER 331
#define GAMGI_IO_FTP_PASSWORD 230
#define GAMGI_IO_FTP_TYPE 200
#define GAMGI_IO_FTP_PASSIVE 227
#define GAMGI_IO_FTP_FILE 150
#define GAMGI_IO_FTP_READ 226

/**********************************
 * code used in the HTTP protocol *
 * to determine if the server is  *
 * accepting the client request   *
 **********************************/

#define GAMGI_IO_HTTP_OK 200

/************************************************
 * maximum color number in a ppm file: 255 (for *
 * 8 bits depth) or 65535 (for 16 bits depth)   *
 ************************************************/

#define GAMGI_IO_PPM_MAX 255

/********************************************
 * by default pnmtopng uses -compression 6  *
 * by default pnmtojpeg uses -quality 75    *
 * by default pnmtotiff uses no compression *
 ********************************************/

#define GAMGI_IO_PNMTOPNG "pnmtopng -compression 6"
#define GAMGI_IO_PNMTOJPEG "pnmtojpeg -quality 75"
#define GAMGI_IO_PNMTOTIFF "pnmtotiff"

#endif
