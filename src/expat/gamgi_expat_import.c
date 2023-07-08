/******************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import.c
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
#include "gamgi_expat.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_window_create.h"
#include "gamgi_mesa_lists.h"
#include "gamgi_mesa_rotate_out.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_hash.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_chem_bond.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_reciprocal.h"
#include "gamgi_io_token.h" 
#include "gamgi_io_file.h"
#include "gamgi_expat_import.h"
#include "gamgi_expat_import_object.h"
#include "gamgi_expat_import_config.h"
#include "gamgi_expat_import_help.h"
#include "gamgi_io_error.h"
#include "gamgi_global_remove.h"

static void static_remove (gamgi_gml *gml, gamgi_bool valid)
{
gamgi_slist *slist;
gamgi_dlist *dlist;
gamgi_object *object;

free (gml->id);
free (gml->ref);
free (gml->id_object);
free (gml->ref_object);
free (gml->ref_line);
free (gml->ref_file);

gamgi_math_hash_remove (gml->hash, GAMGI_EXPAT_HASH_SIZE);

/*****************************
 * remove list of file names *
 *****************************/

slist = gml->files;
while (slist != NULL)
  {
  free (slist->data);
  slist = gamgi_engine_slist_remove_start (slist);
  }

/*************************************************************
 * For well behaved input files, gml->parent will be NULL at *
 * this stage. Otherwise, gml->parent stack must be removed. *
 *************************************************************/

slist = gml->ml.parent;
while (slist != NULL)
  slist = gamgi_engine_slist_remove_start (slist);

/*************************************************
 * 1) gml->object_start->data must be NULL, as   *
 *    the first dlist node is always for <gml>.  *
 *    Elsewhere, dlist->data is always non NULL. *
 *                                               *
 * 2) gml->object_start will be NULL if an       *
 *    error occurs before or inside <gml>.       *
 *************************************************/

dlist = gml->object_start;
if (dlist != NULL) dlist = gamgi_engine_dlist_remove_start (dlist);

/****************
 * data cleanup *
 ****************/
 
while (dlist != NULL)
  {
  object = GAMGI_CAST_OBJECT dlist->data;
  if (valid == FALSE) gamgi_engine_remove_object (object);
  dlist = gamgi_engine_dlist_remove_start (dlist);
  }

/******************
 * config cleanup *
 ******************/

object = GAMGI_CAST_OBJECT gamgi->gamgi;
if (object->object != NULL)
  {
  gamgi_global_remove_gamgi ((gamgi_gamgi_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->window;
if (object->object != NULL) 
  {
  gamgi_global_remove_window ((gamgi_window_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->layer;
if (object->object != NULL) 
  {
  gamgi_global_remove_layer ((gamgi_layer_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->light;
if (object->object != NULL) 
  {
  gamgi_global_remove_light ((gamgi_light_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->assembly;
if (object->object != NULL) 
  {
  gamgi_global_remove_assembly ((gamgi_assembly_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->graph;
if (object->object != NULL) 
  {
  gamgi_global_remove_graph ((gamgi_graph_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->shape;
if (object->object != NULL) 
  {
  gamgi_global_remove_shape ((gamgi_shape_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->arrow;
if (object->object != NULL) 
  {
  gamgi_global_remove_arrow ((gamgi_arrow_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->cell;
if (object->object != NULL) 
  {
  gamgi_global_remove_cell ((gamgi_cell_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->cluster;
if (object->object != NULL) 
  {
  gamgi_global_remove_cluster ((gamgi_cluster_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->molecule;
if (object->object != NULL)
  {
  gamgi_global_remove_molecule ((gamgi_molecule_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->group;
if (object->object != NULL)
  {
  gamgi_global_remove_group ((gamgi_group_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->plane;
if (object->object != NULL)
  {
  gamgi_global_remove_plane ((gamgi_plane_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->direction;
if (object->object != NULL)
  {
  gamgi_global_remove_direction ((gamgi_direction_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->atom;
if (object->object != NULL)
  {
  gamgi_global_remove_atom ((gamgi_atom_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->bond;
if (object->object != NULL)
  {
  gamgi_global_remove_bond ((gamgi_bond_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->orbital;
if (object->object != NULL)
  {
  gamgi_global_remove_orbital ((gamgi_orbital_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->text;
if (object->object != NULL)
  {
  gamgi_global_remove_text ((gamgi_text_class *) object->object);
  object->object = NULL;
  }

object = GAMGI_CAST_OBJECT gamgi->help;
if (object->object != NULL)
  {
  gamgi_global_remove_help ((gamgi_help_class *) object->object);
  object->object = NULL;
  }

free (gml);
}

static void static_config (gamgi_gml *gml)
{
gamgi_object *object, *object_old;
gamgi_atom_class *atom;

object = GAMGI_CAST_OBJECT gamgi->gamgi;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->gamgi = (gamgi_gamgi_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->window;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->window = (gamgi_window_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->layer;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->layer = (gamgi_layer_class *) object->object;
  object->object = object_old;

  /******************************************
   * The number of undo/save allowed levels *
   * might have changed: remove all levels  *
   * in GAMGI that pass the new limit.      *
   *****************************************/

  gamgi_gtk_history_gamgi_reset ();
  }

object = GAMGI_CAST_OBJECT gamgi->light;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->light = (gamgi_light_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->assembly;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->assembly = (gamgi_assembly_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->graph;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->graph = (gamgi_graph_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->shape;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->shape = (gamgi_shape_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->arrow;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->arrow = (gamgi_arrow_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->cell;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->cell = (gamgi_cell_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->cluster;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->cluster = (gamgi_cluster_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->molecule;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->molecule = (gamgi_molecule_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->group;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->group = (gamgi_group_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->plane;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->plane = (gamgi_plane_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->direction;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->direction = (gamgi_direction_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->atom;
if (object->object != NULL)
  {
  atom = (gamgi_atom_class *) object->object;

  /******************************************
   * recreate OpenGL pre-compiled shared    *
   * lists with the new width/slices values *
   ******************************************/

  if (atom->width != gamgi->atom->width)
    {
    gamgi_mesa_lists_cross (atom->width);
    gamgi_mesa_lists_line (atom->width);
    }

  if (atom->slices != gamgi->atom->slices)
    {
    gamgi_mesa_lists_sphere (atom->slices, atom->slices);
    gamgi_mesa_lists_cylinder_1 (atom->slices, 1);
    gamgi_mesa_lists_cylinder_3 (atom->slices, 1);
    gamgi_mesa_lists_cylinder_5 (atom->slices, 1);
    }

  object_old = object;
  gamgi->atom = (gamgi_atom_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->bond;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->bond = (gamgi_bond_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->orbital;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->orbital = (gamgi_orbital_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->text;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->text = (gamgi_text_class *) object->object;
  object->object = object_old;
  }

object = GAMGI_CAST_OBJECT gamgi->help;
if (object->object != NULL)
  {
  object_old = object;
  gamgi->help = (gamgi_help_class *) object->object;
  object->object = object_old;
  }
}

static void static_object (gamgi_gml *gml)
{
gamgi_window *window;
gamgi_layer *layer;
gamgi_cell *cell;
gamgi_plane *plane;
gamgi_direction *direction;
gamgi_object *object;
gamgi_dlist *dlist;

/******************************
 * first object is <gml>, so  *
 * we start with the next one *
 ******************************/

for (dlist = gml->object_start->next; dlist != NULL; dlist = dlist->next)
  {
  object = GAMGI_CAST_OBJECT dlist->data;
  switch (object->class)
    {
    /************************************************************
     * for each new window, window->layer and window->focus     *
     * are updated immediately everytime a new layer is found   *
     * because if an error is found the whole window is removed *
     ************************************************************/

    case GAMGI_ENGINE_WINDOW:
    window = GAMGI_CAST_WINDOW object;
    gamgi_gtk_window_create (window, window->layer);
    break;

    /*****************************************************
     * for the current window, window->layer is set only *
     * here, after error checking, because if an error   *
     * is found we must go back to the initial window    *
     *                                                   *
     * window->focus is kept untouched, to check later   *
     * if the current object is gamgi, the global object *
     *****************************************************/

    case GAMGI_ENGINE_LAYER:
    layer = GAMGI_CAST_LAYER object;
    window = GAMGI_CAST_WINDOW object->object;
    gamgi_engine_link_layer_window (layer, window);
    if (window == gml->ml.window) window->layer = layer;
    break;

    case GAMGI_ENGINE_PLANE:
    gamgi_engine_link_object_object (object, object->object);
    plane = GAMGI_CAST_PLANE object;
    if (plane->reference == GAMGI_ENGINE_CELL)
      {
      /**************************
       * crystallographic plane *
       **************************/

      cell = GAMGI_CAST_CELL object->object;
      if (cell->model == GAMGI_PHYS_PROJECTION)
        {
        gamgi_phys_projection_create (plane->model, &plane->points, 
        &plane->n_points, &plane->loops, &plane->n_loops);
        gamgi_mesa_rotate_out_plane (plane, NULL, NULL, NULL);
        }
      if (plane->model == GAMGI_PHYS_VECTOR)
        gamgi_phys_reciprocal_points (plane);
      }
    break;

    case GAMGI_ENGINE_DIRECTION:
    gamgi_engine_link_object_object (object, object->object);
    direction = GAMGI_CAST_DIRECTION object;
    if (direction->reference == GAMGI_ENGINE_CELL)
      {
      /******************************
       * crystallographic direction *
       ******************************/

      cell = GAMGI_CAST_CELL object->object;
      if (cell->model == GAMGI_PHYS_PROJECTION)
        {
        gamgi_phys_projection_create (direction->model, &direction->points, 
        &direction->n_points, &direction->loops, &direction->n_loops);
        gamgi_mesa_rotate_out_direction (direction, NULL, NULL, NULL);
        }
      }
    break;

    default:
    gamgi_engine_link_object_object (object, object->object);
    break;
    }
  }

/************************************************************
 * Update current window: when this point is reached, the   *
 * top layer (that existed before or was created by the     *
 * importing operation) is always equal to window->layer    *
 * and window->focus retains the original value. When this  *
 * value is gamgi->gamgi, the other windows statusbars must *
 * be updated again. Unless this happens, the other windows *
 * already show the right data when created.                *
 ************************************************************/

window = gml->ml.window;
gamgi_gtk_object_layer_local (window->layer, window);
gamgi_gtk_object_focus_local (GAMGI_CAST_OBJECT window->layer, window);
}

static void static_doctype_gml (void *data,
const char *doctype, const char *system_id,
const char *public_id, int internal)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current header starts  *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (doctype, "gml") != 0)
  {
  ml->valid = gamgi_io_error_header (doctype, 
  ml->filename, fileline, ml->window);
  }
else if (system_id != NULL && strcmp (system_id, GAMGI_IO_GML_SYSTEM) != 0)
  {
  ml->valid = gamgi_io_error_header (system_id, 
  ml->filename, fileline, ml->window);
  }
else if (public_id != NULL && strcmp (public_id, GAMGI_IO_GML_PUBLIC) != 0)
  {
  ml->valid = gamgi_io_error_header (public_id, 
  ml->filename, fileline, ml->window);
  }
}

static void static_doctype_html (void *data,
const char *doctype, const char *system_id,
const char *public_id, int internal)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current header starts  *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (doctype, "html") != 0)
  {
  ml->valid = gamgi_io_error_header (doctype, ml->filename,
  fileline, ml->window);
  }

else if (system_id != NULL && strcmp (system_id, GAMGI_IO_HTML_SYSTEM) != 0)
  {
  ml->valid = gamgi_io_error_header (system_id, ml->filename,
  fileline, ml->window);
  }

else if (public_id != NULL && strcmp (public_id, GAMGI_IO_HTML_PUBLIC) != 0)
  {
  ml->valid = gamgi_io_error_header (public_id, ml->filename,
  fileline, ml->window);
  }
}

static void static_cdata_ml (void *data, 
const char *cdata, int length)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
char line[GAMGI_ENGINE_LINE];
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current cdata starts   *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

/***************************************************
 *   Adding 1 to length is needed because cdata    *
 *   is NOT NULL terminated, so its size is one    *
 *   char larger than reported by EXPAT callback.  *
 *                                                 *
 * If the number of (ignored) char in element      *
 * content cannot fit in a GAMGI line string,      *
 * an error is immediately reported without        *
 * even checking whether the char are good or not. *
 * This test is not as stringent as it looks,      *
 * because EXPAT feeds GAMGI with a new cdata      *
 * batch every time a new line char is found, this *
 * way resetting the char count and allowing an    *
 * unlimited number of delimiter char in element   *
 * content, as long as each line is not larger     *
 * than GAMGI_ENGINE_LINE.                         *
 ***************************************************/ 

if (length + 1 > GAMGI_ENGINE_LINE)
  ml->valid = gamgi_io_error_cdata (ml->filename, 
  fileline, ml->window);
else
  {
  gamgi_io_token_cut (cdata, line, length + 1);

  /***********************************
   * Only delimiter char (by default *
   * defined by GAMGI_IO_DELIMITERS) *
   * are allowed in element content. *
   ***********************************/

  if (gamgi_io_token_check (line) == TRUE)
    ml->valid = gamgi_io_error_cdata (ml->filename, 
    fileline, ml->window);
  }

}

static void static_cdata_gml (void *data,
const char *cdata, int length)
{
gamgi_gml *gml = GAMGI_CAST_GML data;
char *token, *string;
double dvalue;
float fvalue;
int ivalue;
int n, fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current cdata starts   *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/*******************************************************
 * For most elements, only delimiter char (defined by  *
 * GAMGI_IO_DELIMITERS) are allowed as character data. *
 *******************************************************/

if (gml->type == FALSE)
  { static_cdata_ml (data, cdata, length); return; }

/*******************************************************************
 * Character data in Expat is handled like this:                   *
 * 1) when cdata continues, cdata[length] == '\0'                  *
 * 2) when cdata ends and a element starts, cdata[length] == '<'   *
 * 3) when cdata ends and a new line starts cdata[length] == '\n'  *
 * in cases 2 and 3, a new callback is issued, reporting only a    *
 * end of line. In Linux, length = 1, and cdata[length - 1] = '\n' *
 *                                                                 *
 * The whole strategy is: concatenate line until a end             *
 * of line is found, then parse the line and restart again         *
 *                                                                 *
 * To catch ends of line, looking for Char 10 (Linux), Char 13     *
 * (Mac OS X) or Char 10 + Char 13 (Windows) should be enough      *
 *                                                                 *
 * Make sure there is space to end line with a '\0' character      *
 *******************************************************************/

if (gml->used + length + 1 > GAMGI_ENGINE_LINE)
  { gml->ml.valid = gamgi_io_error_cdata (gml->ml.filename,
  fileline, gml->ml.window); return; }

strncpy (gml->line + gml->used, cdata, length);
gml->used += length;

if (cdata[length - 1] != 10 && cdata[length - 1] != 13) return;

/*********************************************
 * parse line looking for tokens that can be *
 * converted into doubles, floats or ints    *
 *********************************************/

gml->line[gml->used] = '\0';

for (string = gml->line; (token = strtok (string, GAMGI_IO_DELIMITERS)) != NULL; string = NULL)
  {
  if (gml->count == gml->size)
    { gml->ml.valid = gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window); return; }

  n = 0;
  if (gml->type == GAMGI_EXPAT_DOUBLE) n = sscanf (token, "%lf", &dvalue);
  if (gml->type == GAMGI_EXPAT_FLOAT) n = sscanf (token, "%f", &fvalue);
  if (gml->type == GAMGI_EXPAT_INT) n = sscanf (token, "%d", &ivalue);

  if (n != 1) { gml->ml.valid = gamgi_io_error_cdata (gml->ml.filename,
  fileline, gml->ml.window); return; }

  if (gml->type == GAMGI_EXPAT_DOUBLE) ((double *) gml->array)[gml->count] = dvalue;
  if (gml->type == GAMGI_EXPAT_FLOAT) ((float *) gml->array)[gml->count] = fvalue;
  if (gml->type == GAMGI_EXPAT_INT) ((int *) gml->array)[gml->count] = ivalue;

  gml->count += 1;
  }

gml->used = 0;
}

static void static_cdata_html (void *data, 
const char *cdata, int length)
{
gamgi_html *html = GAMGI_CAST_HTML data;
gamgi_enum parent;
int i, fileline;
char character;

/******************************************
 * used only in error messages: file line *
 * indicates where current cdata starts   *
 ******************************************/

fileline = XML_GetCurrentLineNumber (html->ml.parser);

/*******************************************************
 * For most elements, only delimiter char (defined by  *
 * GAMGI_IO_DELIMITERS) are allowed as character data. *
 *******************************************************/

parent = GAMGI_POINTER_TO_INT (html->ml.parent->data);

if (parent == GAMGI_EXPAT_HTML || parent == GAMGI_EXPAT_HEAD ||
parent == GAMGI_EXPAT_LINK || parent == GAMGI_EXPAT_UL ||
parent == GAMGI_EXPAT_OL || parent == GAMGI_EXPAT_BR)
  { static_cdata_ml (data, cdata, length); return; }

/*****************************************************
 * insert data, line by line instead of char by char *
 *****************************************************/

for (i = 0; i < length; i++)
  {
   character = cdata[i];

  /**********************************************
   * make sure that every character belongs to  *
   * ISO-8859-1 standard (basically ASCII with  *
   * accents), even if cdata comes from outside *
   * a contents block and thus will be ignored. *
   **********************************************/

  if (strchr (GAMGI_IO_FONT_88591, character) == NULL)
    {
    html->ml.valid = gamgi_io_error_cdata (html->ml.filename,
    fileline, html->ml.window);
    return;
    }

  /***********************************************************
   * if character is outside a <div class="contents> block   *
   * just ignore it (we already tested character for errors) *
   ***********************************************************/

  if (html->contents == FALSE) continue;

  /*****************************************
   * outside <pre></pre> blocks, replace   *
   * multiple delimiters by a single space *
   *****************************************/

  if (html->preserve == FALSE)
    {
    if (strchr (GAMGI_IO_DELIMITERS, character) != NULL)
      {
      if (html->delimiter == TRUE) continue;

      html->delimiter = TRUE;
      character = ' ';
      }
    else  html->delimiter = FALSE;
    }

  /********************************
   * add character to text widget *
   ********************************/

  gtk_text_buffer_insert_with_tags_by_name (html->buffer, 
  &html->iter, &character, 1, html->size, html->color, NULL);
  }
}

static gamgi_bool static_parent_gml (gamgi_enum element, 
gamgi_enum parent)
{
switch (element)
  {
  case GAMGI_ENGINE_TEXT:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_PLANE ||
  parent == GAMGI_ENGINE_DIRECTION ||
  parent == GAMGI_ENGINE_ATOM ||
  parent == GAMGI_ENGINE_BOND ||
  parent == GAMGI_ENGINE_ORBITAL ||
  parent == GAMGI_ENGINE_TEXT ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_ORBITAL:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_PLANE ||
  parent == GAMGI_ENGINE_DIRECTION ||
  parent == GAMGI_ENGINE_ATOM ||
  parent == GAMGI_ENGINE_BOND ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_BOND: case GAMGI_ENGINE_ATOM:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_PLANE ||
  parent == GAMGI_ENGINE_DIRECTION ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_DIRECTION:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_PLANE ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_PLANE:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_GROUP:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_MOLECULE ||
  parent == GAMGI_ENGINE_GROUP ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_MOLECULE:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_CLUSTER:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_CELL ||
  parent == GAMGI_ENGINE_CLUSTER ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_CELL:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_ARROW:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_SHAPE:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_GRAPH:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_ASSEMBLY ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_LIGHT:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_LAYER ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_LAYER:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_WINDOW ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_WINDOW:
  if (parent == GAMGI_EXPAT_GML ||
  parent == GAMGI_ENGINE_GAMGI) return TRUE;
  break;

  case GAMGI_ENGINE_GAMGI:
  if (parent == GAMGI_EXPAT_GML) return TRUE;
  break;

  case GAMGI_EXPAT_GML:
  if (parent == FALSE) return TRUE;
  break;
  }

return FALSE;
}

static gamgi_bool static_parent_html (gamgi_enum element, 
gamgi_enum parent)
{
switch (element)
  {
  case GAMGI_EXPAT_DIV: case GAMGI_EXPAT_UL: case GAMGI_EXPAT_OL: 
  case GAMGI_EXPAT_PRE: case GAMGI_EXPAT_H1: case GAMGI_EXPAT_H3: 
  case GAMGI_EXPAT_SPAN:
  if (parent == GAMGI_EXPAT_BODY || parent == GAMGI_EXPAT_DIV ||
  parent == GAMGI_EXPAT_LI) return TRUE;
  break;
  
  case GAMGI_EXPAT_P: case GAMGI_EXPAT_BR: 
  case GAMGI_EXPAT_IMG: case GAMGI_EXPAT_A:
  if (parent == GAMGI_EXPAT_BODY || parent == GAMGI_EXPAT_DIV ||
  parent == GAMGI_EXPAT_SPAN || parent == GAMGI_EXPAT_LI) return TRUE;
  break;

  case GAMGI_EXPAT_B:
  if (parent == GAMGI_EXPAT_BODY || parent == GAMGI_EXPAT_DIV ||
  parent == GAMGI_EXPAT_SPAN || parent == GAMGI_EXPAT_LI ||
  parent == GAMGI_EXPAT_A || parent == GAMGI_EXPAT_IMG ||
  parent == GAMGI_EXPAT_PRE) return TRUE;
  break;

  case GAMGI_EXPAT_LI:
  if (parent == GAMGI_EXPAT_UL || parent == GAMGI_EXPAT_OL) return TRUE;
  break;

  case GAMGI_EXPAT_BODY: case GAMGI_EXPAT_HEAD: 
  if (parent == GAMGI_EXPAT_HTML) return TRUE;
  break;
  
  case GAMGI_EXPAT_TITLE: case GAMGI_EXPAT_LINK:
  if (parent == GAMGI_EXPAT_HEAD) return TRUE;
  break;
  
  case GAMGI_EXPAT_HTML:
  if (parent == FALSE) return TRUE;
  break;
  }

return FALSE;
}

static char *static_file (char *shortname, gamgi_gml *gml)
{
gamgi_slist *slist;
char *filename;
int length;

/********************************************
 * check if new file name is already listed *
 ********************************************/

slist = gml->files;
while (slist != NULL)
  {
  filename = (char *) slist->data;
  if (strcmp (filename, shortname) == 0) return filename;
  slist = slist->next;
  }

/**********************************************************
 * add new file name to list: gml files can come from     *
 * three different sources: 1) the shell command line;    *
 * 2) the GTK import dialog; 3) external entities,        *
 * imported recursively; When shortname comes from the    *
 * shell or the dialog, it will be around untill the end, *
 * but when shortname comes from external entities it     *
 * will be out of scope when that nested level is left,   *
 * therefore it cannot be used later on, in the id-ref    *
 * linkage process, that must be done only at the very    *
 * end, when all files have been parsed and imported.     *
 * Therefore, shortname must be copied somewhere else.    *
 **********************************************************/

length = strlen (shortname) + 1;
filename = (char *) malloc (sizeof (char) * length);
strcpy (filename, shortname);
gml->files = gamgi_engine_slist_add_start (gml->files);
gml->files->data = filename;

return filename;
}

static gamgi_bool static_import_gml (const char *fullname, 
gamgi_ml *ml)
{
char *shortname, *filename;

/********************************************
 * make sure fullname is an acceptable name *
 ********************************************/

shortname = gamgi_io_file_name (fullname, ml->window);
if (shortname == NULL) return FALSE;

/*****************************************
 * recursive code: save the current file *
 * name before changing to the new one   *
 *****************************************/

filename = ml->filename;

/**************************************************
 * discard the path name, keep only the file name *
 **************************************************/

ml->filename = static_file (shortname, GAMGI_CAST_GML ml);

/*********************************************************************
 * fullname (already checked for errors) indicates the full address, *
 * including the protocol (HTTP, FTP, local), filename is the actual *
 * file to be parsed, gamgi_expat_import_file is the actual parser   *
 *********************************************************************/

ml->valid  = gamgi_io_file_protocol (fullname, 
ml->filename, gamgi_expat_import_file, ml->window, ml);

/**************************************************
 * recursive code: restore the previous file name *
 **************************************************/

ml->filename = filename;

return ml->valid;
}

static gamgi_bool static_import_html (const char *fullname,
gamgi_ml *ml)
{
/************************************************
 *    make sure fullname is an acceptable name  *
 *                                              *
 * discard the pathname, keep only the filename *
 ************************************************/

ml->filename = gamgi_io_file_name (fullname, ml->window);
if (ml->filename == NULL) return FALSE;

/*******************************************
 * get local, HTTP or FTP file and read it *
 *******************************************/

ml->valid  = gamgi_io_file_protocol (fullname, 
ml->filename, gamgi_expat_import_file, ml->window, ml);

return ml->valid;
}

static gamgi_bool static_entity_gml (XML_Parser parser,
const XML_Char *context, const XML_Char *base, const XML_Char *system_id,
const XML_Char *public_id)
{
gamgi_ml *ml = GAMGI_CAST_ML XML_GetUserData (parser);
int fileline;
 
/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (public_id != NULL && strcmp (public_id, GAMGI_IO_GML_PUBLIC) != 0)
  {
  ml->valid = gamgi_io_error_header (public_id, 
  ml->filename, fileline, ml->window);
  }

/*******************************************
 * system_id validity is not checked here  *
 * because it contains the file full name, *
 * which is checked next, using exactly    *
 * the same routines as for any other file *
 *******************************************/
   
/**************************************
 * recursive code: set the new parser *
 **************************************/

ml->parser = XML_ExternalEntityParserCreate (parser, context, NULL);

/*****************************************************
 * check the file full name and import the file data *
 *****************************************************/

ml->valid = static_import_gml (system_id, ml);
XML_ParserFree(ml->parser);

/***********************************************
 * recursive code: restore the previous parser *
 ***********************************************/

ml->parser = parser;

return ml->valid;
}

static gamgi_bool static_entity_html (XML_Parser parser,
const XML_Char *context, const XML_Char *base, const XML_Char *system_id,
const XML_Char *public_id)
{
gamgi_ml *ml = GAMGI_CAST_ML XML_GetUserData (parser);
int fileline;

/******************************************
 * used only in error messages: file name *
 * and file line say where parser is now  *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

/************************************************
 * HTML files should not have external entities *
 ************************************************/

ml->valid = gamgi_io_error_header (public_id,
ml->filename, fileline, ml->window);

return ml->valid;
}

static void static_create_ml (gamgi_ml *ml,
XML_Parser parser, gamgi_window *window)
{
ml->parser = parser;
ml->window = window;
ml->parent = NULL;
ml->valid = TRUE;
}

static gamgi_gml *static_create_gml (XML_Parser parser, 
gamgi_window *window)
{
gamgi_gml *gml;

gml = (gamgi_gml *) malloc (sizeof (gamgi_gml));
static_create_ml (GAMGI_CAST_ML gml, parser, window);

gml->object_start = NULL;
gml->object_end = NULL;

gml->id = NULL;
gml->id_object = NULL;
gml->id_used = 0;
gml->id_total = 0;

gml->ref = NULL;
gml->ref_object = NULL;
gml->ref_used = 0;
gml->ref_total = 0;

gml->type = FALSE;
gml->array = NULL;
gml->size = 0;
gml->count = 0;
gml->used = 0;

gml->ref_line = NULL;
gml->ref_file = NULL;

gml->files = NULL;

gml->hash = gamgi_math_hash_create (GAMGI_EXPAT_HASH_SIZE);

return gml;
}

static gamgi_html *static_create_html (XML_Parser parser,
GtkWidget *text, gamgi_window *window)
{
gamgi_html *html;

html = (gamgi_html *) malloc (sizeof (gamgi_html));
static_create_ml (GAMGI_CAST_ML html, parser, window);

/*********************
 * initialize buffer *
 *********************/

html->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (html->buffer, &html->iter, 0);

/********************************************************
 * contents flag is TRUE inside a <td class="contents"> *
 * element and FALSE elsewhere, to signal which         *
 * character data must be shown in the Help Dialog.     * 
 *                                                      *
 * delimiter flag is FALSE inside a word and TRUE       *
 * outside. Initially it is TRUE, so all delimiters     *
 * before the first word will be discarded.             *
 *                                                      *
 * preserve flags is TRUE inside a <pre></pre> block    *
 * and FALSE outside.                                   *
 ********************************************************/

html->contents = FALSE;
html->delimiter = TRUE;
html->preserve = FALSE;

return html;
}

gamgi_bool gamgi_expat_import_plane (gamgi_plane *plane,
gamgi_object *parent, char *filename, int fileline, gamgi_gml *gml)
{
gamgi_cell *cell;

if (plane->reference != GAMGI_ENGINE_CELL) return TRUE;

/*******************************************
 * check crystallographic planes: polygons *
 * may have a parent that is not a Cell!   *
 *******************************************/

if (parent->class != GAMGI_ENGINE_CELL)
  {
  if (plane->model == FALSE) plane->model = GAMGI_PHYS_POLYGON;

  if (plane->model != GAMGI_PHYS_POLYGON)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (plane->net != NULL)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  return TRUE;
  }
  
cell = GAMGI_CAST_CELL parent;
if (cell->model == GAMGI_PHYS_PROJECTION)
  {
  /****************************************
   * stereographic projection: show the   *
   * pole and Wulff projection by default *
   ****************************************/

  if (plane->model == GAMGI_PHYS_POLYGON || plane->model == GAMGI_PHYS_VECTOR)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (plane->net == NULL)
    plane->net = GAMGI_PHYS_PROJECTION_NET;

  if (plane->model == FALSE) plane->model = GAMGI_PHYS_POLE;
  }
else
  {
  /*******************************************************************
   * real plane: show polygon; reciprocal lattice vector: show cross *
   *******************************************************************/

  if (plane->model == GAMGI_PHYS_POLE || plane->model == GAMGI_PHYS_TRACE)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (plane->net != NULL)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (plane->model == GAMGI_PHYS_VECTOR && cell->reciprocal == 0.0)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (plane->model == FALSE) plane->model = GAMGI_PHYS_POLYGON;
  }

return TRUE;
}

gamgi_bool gamgi_expat_import_direction (gamgi_direction *direction,
gamgi_object *parent, char *filename, int fileline, gamgi_gml *gml)
{
gamgi_cell *cell;

/*************************************
 * check crystallographic directions *
 *************************************/

if (direction->reference != GAMGI_ENGINE_CELL) return TRUE;

/********************************************
 * check crystallographic directions: lines *
 * may have a parent that is not a Cell!    *
 ********************************************/

if (parent->class != GAMGI_ENGINE_CELL)
  {
  if (direction->model == FALSE) direction->model = GAMGI_PHYS_LINE;

  if (direction->model != GAMGI_PHYS_LINE)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (direction->net != NULL)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  return TRUE;
  }

cell = GAMGI_CAST_CELL parent;
if (gamgi_math_node_check (cell->lattice, direction->node[3]) == FALSE)
  return gamgi_io_error_child (filename, fileline, gml->ml.window);

if (cell->model == GAMGI_PHYS_PROJECTION)
  {
  /****************************************
   * stereographic projection: show the   *
   * pole and Wulff projection by default *
   ****************************************/

  if (direction->model == GAMGI_PHYS_LINE)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (direction->net == NULL)
    direction->net = GAMGI_PHYS_PROJECTION_NET;

  if (direction->model == FALSE) direction->model = GAMGI_PHYS_POLE;
  }
else
  {
  /*****************************
   * real direction: show line *
   *****************************/

  if (direction->model == GAMGI_PHYS_POLE || direction->model == GAMGI_PHYS_TRACE)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (direction->net != NULL)
    return gamgi_io_error_child (filename, fileline, gml->ml.window);

  if (direction->model == FALSE) direction->model = GAMGI_PHYS_LINE;
  }

return TRUE;
}

gamgi_dlist *gamgi_expat_import_id_get (char *id, 
gamgi_gml *gml, unsigned int hash)
{
gamgi_slist *slist;
int offset;

/********************************************************
 * scan the hash table list starting at the hash value, *
 * looking for id name. If found return the id object   *
 ********************************************************/

for (slist = gml->hash[hash]; slist != NULL; slist = slist->next)
  {
  offset = GAMGI_POINTER_TO_INT (slist->data);
  if (strcmp (gml->id + offset * GAMGI_ENGINE_TOKEN, id) == 0)
    return gml->id_object[offset];
  }

return NULL;
}

void gamgi_expat_import_id_add (char *id, 
gamgi_gml *gml, unsigned int hash)
{
if (gml->id_used == 0)
  {
  /*****************************************
   * first time: create id and atom arrays *
   *****************************************/

  gml->id_total = GAMGI_EXPAT_BOND_BASE;
  gml->id = (char *) malloc (gml->id_total * GAMGI_ENGINE_TOKEN * sizeof (char));
  gml->id_object = (gamgi_dlist **) malloc (gml->id_total * sizeof (gamgi_dlist *));
  }
else if (gml->id_used == gml->id_total)
  {
  /**************************************************
   * id and object arrays are full: double its size *
   **************************************************/

  gml->id_total *= GAMGI_EXPAT_BOND_FACTOR;
  gml->id = (char *) realloc (gml->id, 
  gml->id_total * GAMGI_ENGINE_TOKEN * sizeof (char));
  gml->id_object = (gamgi_dlist **) realloc (gml->id_object, 
  gml->id_total * sizeof (gamgi_dlist *));
  }

/******************************
 * save the new id and object *
 ******************************/

strcpy (gml->id + GAMGI_ENGINE_TOKEN * gml->id_used, id);
gml->id_object[gml->id_used] = gml->object_end;

(gml->hash)[hash] = gamgi_math_hash_add ((gml->hash)[hash], 
GAMGI_INT_TO_POINTER (gml->id_used++));
}



/**************** external function *************
 *                                              *
 *             GAMGI_EXPAT_IMPORT_REF_ADD       *
 *                                              *
 *   The bond number is half the ref number.    *
 ************************************************/

void gamgi_expat_import_ref_add (char *ref, gamgi_gml *gml)
{
if (gml->ref_used == 0)
  {
  /******************************************
   * first time: create ref and bond arrays *
   ******************************************/

  gml->ref_total = GAMGI_EXPAT_BOND_BASE;
  gml->ref = (char *) malloc (gml->ref_total * GAMGI_ENGINE_TOKEN * sizeof (char));
  gml->ref_object = (gamgi_dlist **) malloc (gml->ref_total * sizeof (gamgi_dlist *));
  gml->ref_line = (int *) malloc (gml->ref_total * sizeof (int));
  gml->ref_file = (char **) malloc (gml->ref_total * sizeof (char *));
  }
else if (gml->ref_used == gml->ref_total)
  {
  /*************************************************
   * ref and bond arrays are full: double its size *
   *************************************************/

  gml->ref_total *= GAMGI_EXPAT_BOND_FACTOR;
  gml->ref = (char *) realloc (gml->ref, 
  gml->ref_total * GAMGI_ENGINE_TOKEN * sizeof (char));
  gml->ref_object = (gamgi_dlist **) realloc (gml->ref_object, 
  gml->ref_total * sizeof (gamgi_dlist *));
  gml->ref_line = (int *) realloc (gml->ref_line,
  gml->ref_total * sizeof (int));
  gml->ref_file = (char **) realloc (gml->ref_file,
  gml->ref_total * sizeof (char *));
  }

/*****************************
 * save the new ref and bond *
 *****************************/

strcpy (gml->ref + GAMGI_ENGINE_TOKEN * gml->ref_used, ref);
gml->ref_object[gml->ref_used] = gml->object_end;
gml->ref_line[gml->ref_used] = XML_GetCurrentLineNumber (gml->ml.parser);
gml->ref_file[gml->ref_used] = gml->ml.filename;
gml->ref_used++;
}

gamgi_bool gamgi_expat_import_connect (gamgi_gml *gml)
{
gamgi_atom *atom1, *atom2;
gamgi_bond *bond;
gamgi_object *object;
gamgi_dlist *dlist_ref;
gamgi_dlist *dlist_id1, *dlist_id2;
char *filename, *identifier;
int fileline, i;
unsigned int hash;

i = 0;
while (i < gml->ref_used)
  {
  /**********************************************
   * used only in error messages: file name and *
   * file line say where ref element started    *
   **********************************************/

  filename = gml->ref_file[i];
  fileline = gml->ref_line[i];

  dlist_ref = gml->ref_object[i];
  object = GAMGI_CAST_OBJECT dlist_ref->data;

  identifier = gml->ref + GAMGI_ENGINE_TOKEN * i;
  hash = gamgi_math_hash_value (identifier,
  GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);
  dlist_id1 = gamgi_expat_import_id_get (identifier, gml, hash);
  if (dlist_id1 == NULL)
    return gamgi_io_error_ref (identifier, 
    filename, fileline, gml->ml.window);

  if (object->class == GAMGI_ENGINE_BOND)
    {
    if ((GAMGI_CAST_OBJECT dlist_id1->data)->class != GAMGI_ENGINE_ATOM)
      return gamgi_io_error_child (filename, fileline, gml->ml.window);

    i++;

    identifier += GAMGI_ENGINE_TOKEN;
    hash = gamgi_math_hash_value (identifier,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);
    dlist_id2 = gamgi_expat_import_id_get (identifier, gml, hash);
    if (dlist_id2 == NULL)
      return gamgi_io_error_ref (identifier, 
      filename, fileline, gml->ml.window);

    if ((GAMGI_CAST_OBJECT dlist_id2->data)->class != GAMGI_ENGINE_ATOM)
      return gamgi_io_error_child (filename, fileline, gml->ml.window);

    bond = GAMGI_CAST_BOND object;
    atom1 = GAMGI_CAST_ATOM dlist_id1->data;
    atom2 = GAMGI_CAST_ATOM dlist_id2->data;

    /********************************
     * check if bond already exists *
     ********************************/

    if (gamgi_chem_bond_get (atom1, atom2) != NULL)
      return gamgi_io_error_multiple (filename, fileline, gml->ml.window);

    /*************************************
     * check if atom1, atom2 are in the  *
     * scope defined by the bond parent  *
     *                                   *
     * bond->object.object is inherited  *
     * from object but not normally      *
     * used, so in the end it should be  *
     * restored to its normal NULL state *
     *************************************/

    if (gamgi_engine_find_root (atom1->object.object, 
    atom2->object.object, object->object) == NULL)
      return gamgi_io_error_child (filename, fileline, gml->ml.window);

    object->object = NULL;

    gamgi_engine_link_bond_atom (bond, atom1);
    gamgi_engine_link_bond_atom (bond, atom2);

    /********************************
     * default name can be set only *
     * after bond parents are known *
     ********************************/

     if (strcmp (bond->object.name, "") == 0)
       gamgi_chem_bond_name (bond, bond->object.name);
  
    /*****************************************************
     *  bond is now linked and if necessary it will      *
     *  be removed from the parent atoms. To avoid a     *
     *  duplication, trying to remove it from the list   *
     *  and from the atoms (and speed up things), bond   *
     *  should be removed now from the object list.      *
     *                                                   *
     * bond is never the first listed object but can be  *
     * the last one: in this case the pointer to the end *
     * of the list must point to the dlist node before.  *
     *****************************************************/

    if (dlist_ref->next == NULL) gml->object_end = dlist_ref->before;

    gamgi_engine_dlist_remove_start (dlist_ref);
    }

  else
    {
    /*********************************************
     * 1) check if new parent is acceptable      *
     * 2) check if object is in old parent scope *
     * 3) new parent replaces old parent         *
     *********************************************/

    if (static_parent_gml (object->class, 
    (GAMGI_CAST_OBJECT dlist_id1->data)->class) == FALSE)
      return gamgi_io_error_child (filename, fileline, gml->ml.window);

    if (gamgi_engine_find_object (GAMGI_CAST_OBJECT dlist_id1->data,
    object->object) == FALSE)
      return gamgi_io_error_child (filename, fileline, gml->ml.window);

    object->object = dlist_id1->data;

    /************************************************
     * check atomic and crystallographic directions *
     ************************************************/

    if (object->class == GAMGI_ENGINE_DIRECTION &&
    gamgi_expat_import_direction (GAMGI_CAST_DIRECTION object,
    object->object, filename, fileline, gml) == FALSE) return FALSE;

    /********************************************
     * check atomic and crystallographic planes *
     ********************************************/

    if (object->class == GAMGI_ENGINE_PLANE &&
    gamgi_expat_import_plane (GAMGI_CAST_PLANE object,
    object->object, filename, fileline, gml) == FALSE) return FALSE;

    /******************************************************
     * each layer cannot have more than gamgi->max_lights *
     * lights, as defined by the Mesa/OpenGL library      *
     ******************************************************/

    if (object->class == GAMGI_ENGINE_LIGHT &&
    gamgi->max_lights == (GAMGI_CAST_LAYER object->object)->light++)
      return gamgi_io_error_resources (filename, fileline, gml->ml.window);
    }

  i++;
  }

return gml->ml.valid;
}

gamgi_bool gamgi_expat_import_file (int fd,
char *body, int size, void *data)
{
gamgi_ml *ml = (gamgi_ml *) data;
char *buffer;
int fileline;
 
/***************************************************
 * after parsing the HTTP header, the buffer still *
 * contains the initial part of the file, which    *
 * must be parsed before reading more.             *
 *                                                 *
 * Local files and files obtained by FTP don't     *
 * have a header, so body is NULL and size is 0    *
 ***************************************************/

if (XML_Parse (ml->parser, body, size, FALSE) == FALSE)
  {
  /**********************************************************************
   * Errors regarding the XML format are detected directly by Expat, so *
   * file line must be obtained only after the error is actually found. *
   **********************************************************************/

  fileline = XML_GetCurrentLineNumber (ml->parser);
  return gamgi_io_error_parse (ml->filename, fileline, ml->window);
  }

if (ml->valid == FALSE) return FALSE;

/*******************************************
 * While the data above is parsed with     *
 * a buffer allocated by GAMGI, the data   *
 * below is parsed with a buffer allocated *
 * by Expat, to speed up Expat parsing.    *
 *******************************************/

while (1 > 0)
  {
  buffer = XML_GetBuffer (ml->parser, GAMGI_IO_BUFFER);
  if (buffer == NULL) return gamgi_io_error_read (ml->filename, ml->window);

  size = read (fd, buffer, GAMGI_IO_BUFFER);
  if (size < 0) return gamgi_io_error_read (ml->filename, ml->window);

  /***************
   * end of file *
   ***************/

  if (size == 0) break;

  if (XML_Parse (ml->parser, buffer, size, FALSE) == FALSE)
    {
    /**********************************************************************
     * Errors regarding the XML format are detected directly by Expat, so *
     * file line must be obtained only after the error is actually found. *
     **********************************************************************/

    fileline = XML_GetCurrentLineNumber (ml->parser);
    return gamgi_io_error_parse (ml->filename, fileline, ml->window);
    }

  /*************************************************
   * Errors not related with the XML format must   *
   * be checked by Gamgi (not Expat) and ml->valid *
   * indicates whether an error was found or not   *
   *************************************************/

  if (ml->valid == FALSE) return FALSE;
  }

/***********************************************
 * check file termination: all previous Expat  *
 * parsing has been done with end = FALSE, so  *
 * Expat will not complain about an incomplete *
 * file. By explicitly setting end = TRUE here *
 * (with a buffer size set to 0 as there is    *
 * nothing more to parse) we force Expat to    *
 * check proper file termination.              *
 ***********************************************/
 
if (XML_Parse (ml->parser, NULL, 0, TRUE) == FALSE)
  {
  /**********************************************************************
   * Errors regarding the XML format are detected directly by Expat, so *
   * file line must be obtained only after the error is actually found. *
   **********************************************************************/

  fileline = XML_GetCurrentLineNumber (ml->parser);
  return gamgi_io_error_parse (ml->filename, fileline, ml->window);
  }

return TRUE;
}

gamgi_bool gamgi_expat_import_parent_gml (gamgi_enum element,
gamgi_gml *gml)
{
gamgi_ml *ml = GAMGI_CAST_ML gml;
int parent;

if (ml->parent == NULL) parent = FALSE;
else
  {
  /****************************************************************
   * When the parent is gamgi (inside a <gamgi></gamgi> block),   *
   * the object callbacks are switched off and this function is   *
   * not called (inside a gamgi block parent checking is direct). *
   ****************************************************************/

  if (ml->parent->data == NULL) parent = GAMGI_EXPAT_GML;
  else parent = (GAMGI_CAST_OBJECT ml->parent->data)->class;
  }

return static_parent_gml (element, parent);
}

gamgi_bool gamgi_expat_import_parent_html (gamgi_enum element,
gamgi_html *html)
{
gamgi_ml *ml = GAMGI_CAST_ML html;
int parent;

if (ml->parent == NULL) parent = FALSE;
else parent = GAMGI_POINTER_TO_INT (ml->parent->data);

return static_parent_html (element, parent);
}

gamgi_bool gamgi_expat_import_gml (char *filename,
gamgi_window *window)
{
XML_Parser parser;
gamgi_gml *gml;
int valid;

/***********************
 * start gml structure *
 ***********************/

parser = XML_ParserCreate (NULL);
gml = static_create_gml (parser, window);
XML_SetUserData (parser, gml);

/************************************
 * declare parser callback handlers *
 ************************************/

XML_SetStartDoctypeDeclHandler (parser, static_doctype_gml);
XML_SetCharacterDataHandler (parser, static_cdata_gml);
XML_SetStartElementHandler (parser, gamgi_expat_import_object_start);
XML_SetEndElementHandler (parser, gamgi_expat_import_object_end);
XML_SetExternalEntityRefHandler (parser, static_entity_gml);

/*****************************************************
 * check the file full name and import the file data *
 *****************************************************/

valid = static_import_gml (filename, GAMGI_CAST_ML gml);
XML_ParserFree(parser);

if (valid == TRUE)
  { 
 /***************************************
  * Save current layer as undo buffer   *
  * layer so it can be recovered later. *
  ***************************************/

  gamgi_gtk_history_update (window->layer, 
  window->layer->undo, GAMGI_ENGINE_UNDO);

  static_object (gml);
  static_config (gml);
  }

static_remove (gml, valid);

return valid;
}

gamgi_bool gamgi_expat_import_html (char *filename,
GtkWidget *text, gamgi_window *window)
{
XML_Parser parser;
gamgi_html *html;
gamgi_bool valid;

/*************************
 * start html structure *
 *************************/

parser = XML_ParserCreate (NULL);
html = static_create_html (parser, text, window);
XML_SetUserData (parser, html);

/************************************
 * declare parser callback handlers *
 ************************************/

XML_SetStartDoctypeDeclHandler (parser, static_doctype_html);
XML_SetCharacterDataHandler (parser, static_cdata_html);
XML_SetStartElementHandler (parser, gamgi_expat_import_help_start);
XML_SetEndElementHandler (parser, gamgi_expat_import_help_end);
XML_SetExternalEntityRefHandler (parser, static_entity_html);

valid = static_import_html (filename, GAMGI_CAST_ML html);
XML_ParserFree (parser);
free (html);

return valid;
}
