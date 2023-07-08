/******************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import.h
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

static void static_remove (gamgi_gml *gml, gamgi_bool valid);

static void static_config (gamgi_gml *gml);

static void static_object (gamgi_gml *gml);

static void static_doctype_gml (void *data,
const char *doctype, const char *system_id,
const char *public_id, int internal);

static void static_doctype_html (void *data,
const char *doctype, const char *system_id,
const char *public_id, int internal);

static void static_cdata_ml (void *data,
const char *cdata, int length);

static void static_cdata_gml (void *data, 
const char *cdata, int length);

static void static_cdata_html (void *data,
const char *cdata, int length);

static gamgi_bool static_parent_gml (gamgi_enum element, 
gamgi_enum parent);

static gamgi_bool static_parent_html (gamgi_enum element,
gamgi_enum parent);

static char *static_file (char *shortname, gamgi_gml *gml);

static gamgi_bool static_import_gml (const char *fullname, 
gamgi_ml *ml);

static gamgi_bool static_import_html (const char *fullname,
gamgi_ml *ml);

static gamgi_bool static_entity_gml (XML_Parser parser,
const XML_Char *context, const XML_Char *base, const XML_Char *system_id,
const XML_Char *public_id);

static gamgi_bool static_entity_html (XML_Parser parser,
const XML_Char *context, const XML_Char *base, const XML_Char *system_id,
const XML_Char *public_id);

static void static_create_ml (gamgi_ml *ml,
XML_Parser parser, gamgi_window *window);

static gamgi_gml *static_create_gml (XML_Parser parser,
gamgi_window *window);

static gamgi_html *static_create_html (XML_Parser parser,
GtkWidget *vbox, gamgi_window *window);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_expat_import_plane (gamgi_plane *plane,
gamgi_object *parent, char *filename, int fileline, gamgi_gml *gml);

gamgi_bool gamgi_expat_import_direction (gamgi_direction *direction,
gamgi_object *parent, char *filename, int fileline, gamgi_gml *gml);

gamgi_dlist *gamgi_expat_import_id_get (char *id, 
gamgi_gml *gml, unsigned int hash);

void gamgi_expat_import_id_add (char *id, 
gamgi_gml *gml, unsigned int hash);

void gamgi_expat_import_ref_add (char *idref, gamgi_gml *gml);

gamgi_bool gamgi_expat_import_connect (gamgi_gml *gml);

gamgi_bool gamgi_expat_import_file (int fd,
char *body, int size, void *data);

gamgi_bool gamgi_expat_import_parent_gml (gamgi_enum element, 
gamgi_gml *gml);

gamgi_bool gamgi_expat_import_parent_html (gamgi_enum element,
gamgi_html *html);

gamgi_bool gamgi_expat_import_gml (char *filename,
gamgi_window *window);

gamgi_bool gamgi_expat_import_html (char *filename,
GtkWidget *vbox, gamgi_window *window);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
