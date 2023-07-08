/******************************************
 *
 * $GAMGI/src/expat/gamgi_expat.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_EXPAT
#define GAMGI_EXPAT

/******** libraries *******
 **************************/

#include <expat.h>

/********** types *********
 **************************/

typedef struct _gamgi_ml {
XML_Parser parser;
gamgi_window *window;
char *filename;
gamgi_slist *parent;
gamgi_bool valid;
} gamgi_ml;

typedef struct _gamgi_html {
gamgi_ml ml;

/********************************************************
 * the actual buffer and position where text is written *
 ********************************************************/

GtkTextBuffer *buffer;
GtkTextIter iter;

/**********************************
 * flags controlling if character *
 * data is accepted or refused    *
 **********************************/

gamgi_bool contents;
gamgi_bool delimiter;
gamgi_bool preserve;

/*******************************
 * current text size and color *
 *******************************/
 
char size[GAMGI_ENGINE_TOKEN];
char color[GAMGI_ENGINE_TOKEN]; } gamgi_html;

typedef struct _gamgi_gml {
gamgi_ml ml;

/******************************************
 * list of all object and config elements *
 ******************************************/

gamgi_dlist *object_start;
gamgi_dlist *object_end;

/**********************
 * hash table used to *
 * speed up id search *
 **********************/

gamgi_slist **hash;

char *id;
int id_used;
int id_total;
gamgi_dlist **id_object;

char *ref;
int ref_used;
int ref_total;
gamgi_dlist **ref_object;

/***********************************************************************
 * used to import polygon arrays in Directions,Planes,Groups,Clusters: *
 * int points_driver, int colors_driver, float colors, double points   *
 ***********************************************************************/

gamgi_enum type;
void *array;
int size;
int count;
char line[GAMGI_ENGINE_LINE];
int used;

/*************************************
 * to properly report error messages *
 *************************************/

int *ref_line;
char **ref_file;

gamgi_slist *files; } gamgi_gml;

/********** enumerations **********
 **********************************/

/************************************************************
 * All GAMGI enumerations start at 1 and share FALSE for 0. *
 ************************************************************/

/*************************************************
 * This enumeration lists all HTML tags that     *
 * GAMGI accepts, when reading Help XHTML files. *
 *************************************************/

enum { GAMGI_EXPAT_HTML = 1, GAMGI_EXPAT_HEAD, GAMGI_EXPAT_TITLE, 
GAMGI_EXPAT_LINK, GAMGI_EXPAT_BODY, GAMGI_EXPAT_DIV, GAMGI_EXPAT_SPAN, 
GAMGI_EXPAT_UL, GAMGI_EXPAT_OL, GAMGI_EXPAT_LI, GAMGI_EXPAT_A, 
GAMGI_EXPAT_IMG, GAMGI_EXPAT_PRE, GAMGI_EXPAT_BR, GAMGI_EXPAT_P, 
GAMGI_EXPAT_H1, GAMGI_EXPAT_H3, GAMGI_EXPAT_B };

enum { GAMGI_EXPAT_INT = 1, GAMGI_EXPAT_FLOAT, GAMGI_EXPAT_DOUBLE };

/************* macros ************
 *********************************/

/*************************************************
 * GAMGI_EXPAT_GML acts as a continuation of the *
 * object enumeration defined in gamgi_engine.h  *
 *************************************************/

#define GAMGI_EXPAT_GML GAMGI_ENGINE_GAMGI + 1

#define GAMGI_EXPAT_BOND_BASE 20
#define GAMGI_EXPAT_BOND_FACTOR 2

#define GAMGI_EXPAT_FILE_BASE 1
#define GAMGI_EXPAT_FILE_FACTOR 2

/************************************************
 * Control which information should be exported *
 ************************************************/

#define GAMGI_EXPAT_CONFIG TRUE
#define GAMGI_EXPAT_OBJECT TRUE

/********************************************
 *       Control OUTPUT style:              *
 *                                          *
 * HEADER_GML: write XML (GML) header       *
 * HEADER_X3D: write XML (X3D) header       *
 *                                          *
 * START: indentation space before element  *
 * MIDDLE: interval space before parameter  *
 * END: interval space before element end   *
 *                                          *
 * LINE: maximum output line size           *
 * DEPTH: maximum output indentation depth  *
 ****************************************** */

#define GAMGI_EXPAT_HEADER_GML TRUE
#define GAMGI_EXPAT_HEADER_X3D TRUE

#define GAMGI_EXPAT_VERSION "1.0"
#define GAMGI_EXPAT_ENCODING "UTF-8"
#define GAMGI_EXPAT_STANDALONE "yes"

#define GAMGI_EXPAT_START 2
#define GAMGI_EXPAT_MIDDLE 1
#define GAMGI_EXPAT_END 0

#define GAMGI_EXPAT_LINE 80
#define GAMGI_EXPAT_DEPTH_MIN 0
#define GAMGI_EXPAT_DEPTH_MAX 10

/**************************************
 *    Choose whether to write IDs     *
 *  for objects of each given class.  *
 *                                    *
 * Bonded atoms will have an ID even  *
 * if GAMGI_EXPAT_ID_ATOM is FALSE,   *
 * because this is the only way bonds *
 * have to identify its parent atoms. *
 **************************************/

#define GAMGI_EXPAT_ID_TEXT FALSE
#define GAMGI_EXPAT_ID_ORBITAL FALSE
#define GAMGI_EXPAT_ID_BOND FALSE
#define GAMGI_EXPAT_ID_ATOM FALSE
#define GAMGI_EXPAT_ID_DIRECTION FALSE
#define GAMGI_EXPAT_ID_PLANE FALSE
#define GAMGI_EXPAT_ID_GROUP FALSE
#define GAMGI_EXPAT_ID_MOLECULE FALSE
#define GAMGI_EXPAT_ID_CLUSTER FALSE
#define GAMGI_EXPAT_ID_CELL FALSE
#define GAMGI_EXPAT_ID_ARROW FALSE
#define GAMGI_EXPAT_ID_SHAPE FALSE
#define GAMGI_EXPAT_ID_GRAPH FALSE
#define GAMGI_EXPAT_ID_ASSEMBLY FALSE
#define GAMGI_EXPAT_ID_LIGHT FALSE
#define GAMGI_EXPAT_ID_LAYER FALSE
#define GAMGI_EXPAT_ID_WINDOW FALSE

/****************************************************
 * Hash multipliers: 31 (used by Kernighan), 37     *
 * (used in the Java library); Hash numbers: prime  *
 * numbers. Source: The Practice of Programming,    *
 * Brian Kernighan, Rob Pike, Addison-Wesley, 1999  *
 ****************************************************/

#define GAMGI_EXPAT_HASH_MULTIPLIER 31
#define GAMGI_EXPAT_HASH_SIZE 1037

/****************
 * object casts *
 ****************/

#define GAMGI_CAST_ML (gamgi_ml *)
#define GAMGI_CAST_GML (gamgi_gml *)
#define GAMGI_CAST_HTML (gamgi_html *)

#endif
