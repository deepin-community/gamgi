/*****************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export.c
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
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_sort.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_atom.h"
#include "gamgi_expat_export_config.h"
#include "gamgi_expat_export_object.h"
#include "gamgi_io_file.h"
#include "gamgi_io_x3d.h"
#include "gamgi_global_selection.h"

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_space (FILE *fp, int space, gamgi_bool *error)
{
while (space-- > 0) gamgi_io_file_fprintf (fp, " ", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static int static_end (FILE *fp, char *string, gamgi_bool *error)
{
static_space (fp, GAMGI_EXPAT_END, error);
gamgi_io_file_fprintf (fp, string, error);

return GAMGI_EXPAT_END;
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static int static_middle (FILE *fp, char *string, gamgi_bool *error)
{
static_space (fp, GAMGI_EXPAT_MIDDLE, error);
gamgi_io_file_fprintf (fp, string, error);

return GAMGI_EXPAT_MIDDLE;
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static int static_start (FILE *fp, char *string, int depth, gamgi_bool *error)
{
gamgi_io_file_fprintf (fp, "\n", error);
depth = gamgi_math_sort_min (depth, GAMGI_EXPAT_DEPTH_MAX);
static_space (fp, depth * GAMGI_EXPAT_START, error);
gamgi_io_file_fprintf (fp, string, error);

return depth * GAMGI_EXPAT_START;
}

/***************** external function *********************
 *                                                       *
 *********************************************************/

void gamgi_expat_export_cdata (FILE *fp,
char *string, int depth, int *column, gamgi_bool *error)
{
int length = strlen (string);

if (*column >= 0 && *column + length < GAMGI_EXPAT_LINE)
  *column += length + static_middle (fp, string, error);
else
  *column = length + static_start (fp, string, depth, error);
}

/***************** external function *********************
 *                                                       *
 *********************************************************/

void gamgi_expat_export_attribute (FILE *fp,
char *string, int depth, int *column, gamgi_bool *error)
{
int length = strlen (string);

/*******************************************************************
 * to guarantee that the element can still be closed in the same   *
 * line, in the worst case (/>) we need to allow for 2 more spaces *
 *******************************************************************/

if (*column + length < GAMGI_EXPAT_LINE - GAMGI_EXPAT_END - 2)
  *column += length + static_middle (fp, string, error);
else
  *column = length + static_start (fp, string, depth, error);
}

/***************** external function *********************
 *                                                       *
 *********************************************************/

void gamgi_expat_export_element (FILE *fp, char *element,
char *string, int depth_parent, int *depth_last, int *column,
gamgi_bool *output, gamgi_bool *error)
{
int depth;

if (*output == FALSE)
  {
  *output = TRUE;
  depth = depth_parent + 1;
  if (depth > *depth_last) gamgi_io_file_fprintf (fp, ">", error);
  if (depth - *depth_last == 2) *column = strlen ("<gamgi>") + 
  static_start (fp, "<gamgi>", depth - 1, error);
  *column = strlen (element) + static_start (fp, element, depth, error);
  *depth_last = depth;
  }

gamgi_expat_export_attribute (fp, string, *depth_last, column, error);
}

void gamgi_expat_export_bond (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error, 
int element1, int element2, double min, double max)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_bond_local (window, fp, depth_parent,
depth_last, &output, error, element1, element2, min, max);

if (output == TRUE) static_end (fp, "/>", error);
}

void gamgi_expat_export_atom (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *error, int element,
double mass, double radius, float red, float green, float blue)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_atom_local (window, fp, depth_parent,
depth_last, &output, error, element, mass, radius, red, green, blue);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_cdata_double (int size, double *array,
FILE *fp, int depth, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
int column, i;

/****************
 * double array *
 ****************/

column = -1;
for (i = 0; i < size; i++)
  {
  sprintf (string, "%.*f", gamgi->gamgi->length, array[i]);
  gamgi_expat_export_cdata (fp, string, depth, &column, error);
  }
}

static void static_cdata_float (int size, float *array,
FILE *fp, int depth, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
int column, i;

/***************
 * float array *
 ***************/

column = -1;
for (i = 0; i < size; i++)
  {
  sprintf (string, "%.*f", GAMGI_MATH_DECIMAL_COLOR, array[i]);
  gamgi_expat_export_cdata (fp, string, depth, &column, error);
  }
}

static void static_cdata_int (int size, int *array,
FILE *fp, int depth, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
int column, i;

/*************
 * int array *
 *************/

column = -1;
for (i = 0; i < size; i++)
  {
  sprintf (string, "%d", array[i]);
  gamgi_expat_export_cdata (fp, string, depth, &column, error);
  }
}

static void static_cdata (char *name, char *type, int size,
void *array, gamgi_window *window, FILE *fp, int depth_parent,
int *depth_last, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<data") + static_start (fp, "<data", depth, error);
*depth_last = depth;

sprintf (string, "name=\"%s\"", name);
gamgi_expat_export_attribute (fp, string, depth, &column, error);

sprintf (string, "type=\"%s\"", type);
gamgi_expat_export_attribute (fp, string, depth, &column, error);

sprintf (string, "size=\"%d\"", size);
gamgi_expat_export_attribute (fp, string, depth, &column, error);

gamgi_io_file_fprintf (fp, ">", error);

/**************
 * cdata here *
 **************/

if (strcmp (type, "double") == 0)
  static_cdata_double (size, array, fp, depth, error);
if (strcmp (type, "float") == 0)
  static_cdata_float (size, array, fp, depth, error);
if (strcmp (type, "int") == 0)
  static_cdata_int (size, array, fp, depth, error);

static_start (fp, "</data>", depth, error);
}

static void static_config_help (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_help (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_text (gamgi_window *window, 
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_text (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_orbital (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_orbital (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_bond (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_bond_global (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_atom (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_atom_global (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_direction (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_direction (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_plane (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_plane (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_group (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_group (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_molecule (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_molecule (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_cluster (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_cluster (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_cell (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_cell (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_arrow (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_arrow (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_shape (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_shape (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_graph (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_graph (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_assembly (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_assembly (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_light (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_light (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_layer (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_layer (window, fp,
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_window (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;

gamgi_expat_export_config_window (window, fp, 
depth_parent, depth_last, &output, error);

if (output == TRUE) static_end (fp, "/>", error);
}

static void static_config_gamgi (gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bool output = FALSE;
int depth;

gamgi_expat_export_config_gamgi (window, fp,
depth_parent, depth_last, &output, error);

depth = depth_parent + 1;

static_config_window (window, fp, depth, depth_last, error);
static_config_layer (window, fp, depth, depth_last, error);
static_config_light (window, fp, depth, depth_last, error);
static_config_assembly (window, fp, depth, depth_last, error);
static_config_graph (window, fp, depth, depth_last, error);
static_config_shape (window, fp, depth, depth_last, error);
static_config_arrow (window, fp, depth, depth_last, error);
static_config_cell (window, fp, depth, depth_last, error);
static_config_cluster (window, fp, depth, depth_last, error);
static_config_molecule (window, fp, depth, depth_last, error);
static_config_group (window, fp, depth, depth_last, error);
static_config_plane (window, fp, depth, depth_last, error);
static_config_direction (window, fp, depth, depth_last, error);

static_config_atom (window, fp, depth, depth_last, error);
gamgi_chem_atom_export (window, fp, depth, depth_last, error);

static_config_bond (window, fp, depth, depth_last, error);
gamgi_chem_bond_export (window, fp, depth, depth_last, error);

static_config_orbital (window, fp, depth, depth_last, error);
static_config_text (window, fp, depth, depth_last, error);
static_config_help (window, fp, depth, depth_last, error);

if (*depth_last == depth)
  static_end (fp, "/>", error);
else if (*depth_last == depth + 1)
  static_start (fp, "</gamgi>", depth, error);
}

static void static_config (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error)
{
static_config_gamgi (window, fp, depth_parent, depth_last, error);
}

static void static_bond_add (gamgi_object *parent, gamgi_bond *bond)
{
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;
gamgi_dlist *dlist = NULL;

switch (parent->class)
  {
  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION parent;
  dlist = gamgi_engine_dlist_add_end (direction->bond_end);
  direction->bond_end = dlist;
  if (dlist->before == NULL) direction->bond_start = dlist;
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE parent;
  dlist = gamgi_engine_dlist_add_end (plane->bond_end);
  plane->bond_end = dlist;
  if (dlist->before == NULL) plane->bond_start = dlist;
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  dlist = gamgi_engine_dlist_add_end (group->bond_end);
  group->bond_end = dlist;
  if (dlist->before == NULL) group->bond_start = dlist;
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  dlist = gamgi_engine_dlist_add_end (molecule->bond_end);
  molecule->bond_end = dlist;
  if (dlist->before == NULL) molecule->bond_start = dlist;
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  dlist = gamgi_engine_dlist_add_end (cluster->bond_end);
  cluster->bond_end = dlist;
  if (dlist->before == NULL) cluster->bond_start = dlist;
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  dlist = gamgi_engine_dlist_add_end (cell->bond_end);
  cell->bond_end = dlist;
  if (dlist->before == NULL) cell->bond_start = dlist;
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  dlist = gamgi_engine_dlist_add_end (assembly->bond_end);
  assembly->bond_end = dlist;
  if (dlist->before == NULL) assembly->bond_start = dlist;
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  dlist = gamgi_engine_dlist_add_end (layer->bond_end);
  layer->bond_end = dlist;
  if (dlist->before == NULL) layer->bond_start = dlist;
  break;

  /************************
   * Control should never *
   *  reach this point    *
   ************************/

  default:
  break;
  }

dlist->data = bond;
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_bond_parent (gamgi_bond *bond, gamgi_window *window,
gamgi_object *scope, int *id, gamgi_bool *error)
{
gamgi_object *parent;

if (bond->first == TRUE)
  {
  parent = gamgi_engine_find_root (GAMGI_CAST_OBJECT bond->atom1,
  GAMGI_CAST_OBJECT bond->atom2, scope);
  if (parent != NULL)
    {
    /**********************************************
     * If the parent is out of range, the second  *
     * atom will not appear and this bond can be  *
     * safely discarded without further action.   *
     *                                            *
     *  If a common parent is found, then the     *
     *  second atom must necessarily show up:     *
     *  we don't need to care about half bonds.   *
     *                                            *
     * atom id numbers must be saved in a place   *
     * that the bond can reach, in order to allow *
     * the bond later on to identify its parent   *
     * atoms, when the bond is actually exported  *
     * (at the end of the common parent block)    *
     *                                            *
     *  the first and second atom ids are saved   *
     *  in the unused bond->object.object and     *
     *  bond->object.dlist addresses.             *
     *                                            *
     * atom->mark is used to flag when the atom   *
     * id identifiers must be written. If atoms   *
     * A,B form a bond between atoms B,A, atom A  *
     * is scanned first but marked last, so all   *
     * atoms must be marked in the first scan,    *
     * before their <atom> blocks are exported.   *
     **********************************************/

    bond->atom1->mark = TRUE;
    bond->atom2->mark = TRUE;
    static_bond_add (parent, bond);
    bond->object.object = GAMGI_INT_TO_POINTER (*id);
    bond->first = FALSE;
    }
  }
else
  {
  bond->object.dlist = GAMGI_INT_TO_POINTER (*id);
  bond->first = TRUE;
  }

}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_text (gamgi_text *text,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<text") + static_start (fp, "<text", depth, error);
*depth_last = depth;

gamgi_expat_export_object_text (text, fp, depth, &column, id, error);

/*******************
 * write text list *
 *******************/

dlist = text->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</text>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_orbital (gamgi_orbital *orbital,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<orbital") + static_start (fp, "<orbital", depth, error);
*depth_last = depth;

gamgi_expat_export_object_orbital (orbital, fp, depth, &column, id, error);

/*******************
 * write text list *
 *******************/

dlist = orbital->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</orbital>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_bond (gamgi_bond *bond,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<bond") + static_start (fp, "<bond", depth, error);
*depth_last = depth;

gamgi_expat_export_object_bond (bond, fp, depth, &column, id, error);

/***************************************
 * clean object and dlist addresses    *
 * that were used to keep the atom ids *
 ***************************************/

bond->object.object = NULL;
bond->object.dlist = NULL;

/******************
 * child elements *
 ******************/

dlist = bond->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

dlist = bond->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</bond>", depth, error);
else
  static_end (fp, "/>", error);
}

static void static_object_atom (gamgi_atom *atom,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<atom") + static_start (fp, "<atom", depth, error);
*depth_last = depth;

/*******************************************
 * bonds are written at the bottom of the  *
 * first block that contains the two atoms *
 *                                         *
 * bonds must be checked before printing   *
 * atom data, to see if ids must be added  *
 *******************************************/

dlist = atom->bond_start;
while (dlist != NULL)
  { static_bond_parent (GAMGI_CAST_BOND dlist->data, window, scope, id, error);
    dlist = dlist->next; }

gamgi_expat_export_object_atom (atom, fp, depth, &column, id, error);

/******************
 * child elements *
 ******************/

dlist = atom->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

dlist = atom->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</atom>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_direction (gamgi_direction *direction,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<direction") + static_start (fp, "<direction", depth, error);
*depth_last = depth;

gamgi_expat_export_object_direction (direction, fp, depth, &column, id, error);

/*******************
 * write atom list *
 *******************/

dlist = direction->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = direction->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

direction->bond_start = NULL;
direction->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = direction->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = direction->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</direction>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_plane (gamgi_plane *plane, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<plane") + static_start (fp, "<plane", depth, error);
*depth_last = depth;

gamgi_expat_export_object_plane (plane, fp, depth, &column, id, error);

/************************
 * write direction list *
 ************************/

dlist = plane->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = plane->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = plane->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

plane->bond_start = NULL;
plane->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = plane->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = plane->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***************************************************
 * write cdata: each point has 3 coordinates x,y,z *
 ***************************************************/

if (plane->model == GAMGI_PHYS_POLYGON)
  {
  static_cdata ("points", "double", 3 * plane->n_points,
  plane->points, window, fp, depth, depth_last, error);
  static_cdata ("points_driver", "int", plane->n_loops,
  plane->loops, window, fp, depth, depth_last, error);
  }

if (depth < *depth_last)
  static_start (fp, "</plane>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_group (gamgi_group *group, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<group") + static_start (fp, "<group", depth, error);
*depth_last = depth;

gamgi_expat_export_object_group (group, fp, depth, &column, id, error);

/********************
 * write group list *
 ********************/

dlist = group->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = group->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = group->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = group->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = group->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

group->bond_start = NULL;
group->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = group->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = group->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***************************************************
 * write cdata: each point has 3 coordinates x,y,z *
 ***************************************************/

if (group->reference == GAMGI_CHEM_POLYTOPE)
  {
  static_cdata ("points", "double", 3 * group->n_points,
  group->points, window, fp, depth, depth_last, error);
  static_cdata ("points_driver", "int", group->n_loops,
  group->loops, window, fp, depth, depth_last, error);
  }

if (depth < *depth_last)
  static_start (fp, "</group>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_molecule (gamgi_molecule *molecule, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<molecule") + static_start (fp, "<molecule", depth, error);
*depth_last = depth;

gamgi_expat_export_object_molecule (molecule, fp, depth, &column, id, error);

/********************
 * write group list *
 ********************/

dlist = molecule->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = molecule->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = molecule->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = molecule->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = molecule->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

molecule->bond_start = NULL;
molecule->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = molecule->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = molecule->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</molecule>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_cluster (gamgi_cluster *cluster, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<cluster") + static_start (fp, "<cluster", depth, error);
*depth_last = depth;

gamgi_expat_export_object_cluster (cluster, fp, depth, &column, id, error);

/**********************
 * write cluster list *
 **********************/

dlist = cluster->cluster_start;
while (dlist != NULL)
  { static_object_cluster (GAMGI_CAST_CLUSTER dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************
 * write molecule list *
 ***********************/

dlist = cluster->molecule_start;
while (dlist != NULL)
  { static_object_molecule (GAMGI_CAST_MOLECULE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write group list *
 ********************/

dlist = cluster->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = cluster->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = cluster->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = cluster->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = cluster->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

cluster->bond_start = NULL;
cluster->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = cluster->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = cluster->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************************************
 * write cdata: each point has 3 coordinates   *
 * x,y,z and each color has 3 components r,g,b *
 ***********************************************/

if (cluster->reference == GAMGI_CHEM_POLYTOPE)
  {
  if (cluster->points != NULL)
    static_cdata ("points", "double", 3 * cluster->n_points,
    cluster->points, window, fp, depth, depth_last, error);
  if (cluster->loops != NULL)
    static_cdata ("points_driver", "int", cluster->n_loops,
    cluster->loops, window, fp, depth, depth_last, error);
  if (cluster->colors != NULL)
    static_cdata ("colors", "float", 3 * cluster->n_colors,
    cluster->colors, window, fp, depth, depth_last, error);
  if (cluster->paints != NULL)
    static_cdata ("colors_driver", "int", cluster->n_paints,
    cluster->paints, window, fp, depth, depth_last, error);
  }

if (depth < *depth_last)
  static_start (fp, "</cluster>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_cell (gamgi_cell *cell, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<cell") + static_start (fp, "<cell", depth, error);
*depth_last = depth;

gamgi_expat_export_object_cell (cell, fp, depth, &column, id, error);

/**********************
 * write cluster list *
 **********************/

dlist = cell->cluster_start;
while (dlist != NULL)
  { static_object_cluster (GAMGI_CAST_CLUSTER dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************
 * write molecule list *
 ***********************/

dlist = cell->molecule_start;
while (dlist != NULL)
  { static_object_molecule (GAMGI_CAST_MOLECULE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write group list *
 ********************/

dlist = cell->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = cell->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = cell->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = cell->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = cell->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

cell->bond_start = NULL;
cell->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = cell->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = cell->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</cell>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_arrow (gamgi_arrow *arrow,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<arrow") + static_start (fp, "<arrow", depth, error);
*depth_last = depth;

gamgi_expat_export_object_arrow (arrow, fp, depth, &column, id, error);

/*******************
 * write text list *
 *******************/

dlist = arrow->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</arrow>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_shape (gamgi_shape *shape,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<shape") + static_start (fp, "<shape", depth, error);
*depth_last = depth;

gamgi_expat_export_object_shape (shape, fp, depth, &column, id, error);

/*******************
 * write text list *
 *******************/

dlist = shape->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</shape>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_graph (gamgi_graph *graph,
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<graph") + static_start (fp, "<graph", depth, error);
*depth_last = depth;

gamgi_expat_export_object_graph (graph, fp, depth, &column, id, error);

/*******************
 * write text list *
 *******************/

dlist = graph->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</graph>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_assembly (gamgi_assembly *assembly, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<assembly") + static_start (fp, "<assembly", depth, error);
*depth_last = depth;

gamgi_expat_export_object_assembly (assembly, fp, depth, &column, id, error);

/***********************
 * write assembly list *
 ***********************/

dlist = assembly->assembly_start;
while (dlist != NULL)
  { static_object_assembly (GAMGI_CAST_ASSEMBLY dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write graph list *
 ********************/

dlist = assembly->graph_start;
while (dlist != NULL)
  { static_object_graph (GAMGI_CAST_GRAPH dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write shape list *
 ********************/

dlist = assembly->shape_start;
while (dlist != NULL)
  { static_object_shape (GAMGI_CAST_SHAPE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write arrow list *
 ********************/

dlist = assembly->arrow_start;
while (dlist != NULL)
  { static_object_arrow (GAMGI_CAST_ARROW dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write cell list *
 *******************/

dlist = assembly->cell_start;
while (dlist != NULL)
  { static_object_cell (GAMGI_CAST_CELL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/**********************
 * write cluster list *
 **********************/

dlist = assembly->cluster_start;
while (dlist != NULL)
  { static_object_cluster (GAMGI_CAST_CLUSTER dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************
 * write molecule list *
 ***********************/

dlist = assembly->molecule_start;
while (dlist != NULL)
  { static_object_molecule (GAMGI_CAST_MOLECULE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write group list *
 ********************/

dlist = assembly->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = assembly->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = assembly->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = assembly->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = assembly->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

assembly->bond_start = NULL;
assembly->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = assembly->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = assembly->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</assembly>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_light (gamgi_light *light, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent, 
int *depth_last, int *id, gamgi_bool *error)
{
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<light") + static_start (fp, "<light", depth, error);
*depth_last = depth;

gamgi_expat_export_object_light (light, fp, depth, &column, id, error);

if (depth < *depth_last)
  static_start (fp, "</light>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_layer (gamgi_layer *layer, 
gamgi_window *window, gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<layer") + static_start (fp, "<layer", depth, error);
*depth_last = depth;

gamgi_expat_export_object_layer (layer, fp, depth, &column, id, error);

/********************
 * write light list *
 ********************/

dlist = layer->light_start;
while (dlist != NULL)
  { static_object_light (GAMGI_CAST_LIGHT dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************
 * write assembly list *
 ***********************/

dlist = layer->assembly_start;
while (dlist != NULL)
  { static_object_assembly (GAMGI_CAST_ASSEMBLY dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write graph list *
 ********************/

dlist = layer->graph_start;
while (dlist != NULL)
  { static_object_graph (GAMGI_CAST_GRAPH dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write shape list *
 ********************/

dlist = layer->shape_start;
while (dlist != NULL)
  { static_object_shape (GAMGI_CAST_SHAPE dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write arrow list *
 ********************/

dlist = layer->arrow_start;
while (dlist != NULL)
  { static_object_arrow (GAMGI_CAST_ARROW dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write cell list *
 *******************/

dlist = layer->cell_start;
while (dlist != NULL)
  { static_object_cell (GAMGI_CAST_CELL dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/**********************
 * write cluster list *
 **********************/

dlist = layer->cluster_start;
while (dlist != NULL)
  { static_object_cluster (GAMGI_CAST_CLUSTER dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/***********************
 * write molecule list *
 ***********************/

dlist = layer->molecule_start;
while (dlist != NULL)
  { static_object_molecule (GAMGI_CAST_MOLECULE dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write group list *
 ********************/

dlist = layer->group_start;
while (dlist != NULL)
  { static_object_group (GAMGI_CAST_GROUP dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/********************
 * write plane list *
 ********************/

dlist = layer->plane_start;
while (dlist != NULL)
  { static_object_plane (GAMGI_CAST_PLANE dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/************************
 * write direction list *
 ************************/

dlist = layer->direction_start;
while (dlist != NULL)
  { static_object_direction (GAMGI_CAST_DIRECTION dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write atom list *
 *******************/

dlist = layer->atom_start;
while (dlist != NULL)
  { static_object_atom (GAMGI_CAST_ATOM dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write bond list *
 *******************/

dlist = layer->bond_start;
while (dlist != NULL)
  { static_object_bond (GAMGI_CAST_BOND dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

layer->bond_start = NULL;
layer->bond_end = NULL;

/**********************
 * write orbital list *
 **********************/

dlist = layer->orbital_start;
while (dlist != NULL)
  { static_object_orbital (GAMGI_CAST_ORBITAL dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

/*******************
 * write text list *
 *******************/

dlist = layer->text_start;
while (dlist != NULL)
  { static_object_text (GAMGI_CAST_TEXT dlist->data,
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</layer>", depth, error);
else
  static_end (fp, "/>", error);
}

/***************** internal function *********************
 *                                                       *
 *********************************************************/

static void static_object_window (gamgi_window *window,
gamgi_object *scope, FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
int column, depth;

depth = depth_parent + 1;
if (depth > *depth_last)
  gamgi_io_file_fprintf (fp, ">", error);
column = strlen ("<window") + static_start (fp, "<window", depth, error);
*depth_last = depth;

gamgi_expat_export_object_window (window, fp, depth, &column, id, error);

/********************
 * write layer list *
 ********************/

dlist = window->layer_start;
while (dlist != NULL)
  { static_object_layer (GAMGI_CAST_LAYER dlist->data, 
    window, scope, fp, depth, depth_last, id, error);
    dlist = dlist->next; }

if (depth < *depth_last)
  static_start (fp, "</window>", depth, error);
else
  static_end (fp, "/>", error);
}

static void static_bond_atom (gamgi_atom *atom, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
int mark;

/***********************************
 * save atom->mark as this is      *
 * reset in static_object_atom     *
 *                                 *
 * set scope to atom, so bonds are *
 * ignored in static_object_atom   *
 ***********************************/

if (atom->mark == 0)
  {
  mark = atom->mark = *id;
  static_object_atom (atom, window, GAMGI_CAST_OBJECT atom,
  fp, depth_parent, depth_last, id, error);
  atom->mark = mark;
  }
}

static void static_bond_id (gamgi_bond *bond, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
bond->object.object = GAMGI_INT_TO_POINTER (bond->atom1->mark);
bond->object.dlist = GAMGI_INT_TO_POINTER (bond->atom2->mark);
static_object_bond (bond, window, GAMGI_CAST_OBJECT bond,
fp, depth_parent, depth_last, id, error);
}

static void static_bond_single (gamgi_bond *bond, gamgi_window *window,
FILE *fp, int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
static_bond_atom (bond->atom1, window, fp, depth_parent, depth_last, id, error);
static_bond_atom (bond->atom2, window, fp, depth_parent, depth_last, id, error);
static_bond_id (bond, window, fp, depth_parent, depth_last, id, error);
bond->atom1->mark = 0;
bond->atom2->mark = 0;
}

static void static_object_text_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_text *text;
gamgi_dlist *dlist;

/*******************
 * write text list *
 *******************/

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, TRUE);

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  if (text->mark == FALSE) static_object_text (text, window,
  GAMGI_CAST_OBJECT text, fp, depth_parent, depth_last, id, error);
  }

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, FALSE);
}

static void static_object_orbital_list (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/**********************
 * write orbital list *
 **********************/

for (dlist = gamgi->orbitals; dlist != NULL; dlist = dlist->next)
  static_object_orbital (GAMGI_CAST_ORBITAL dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_bond_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;
gamgi_bond *bond;

/*******************
 * write bond list *
 *******************/

/**********************************************
 * export atoms, saving ids in atom->mark and *
 * ignoring lists of bonds pointed from atoms *
 **********************************************/

for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  static_bond_atom (bond->atom1, window,
  fp, depth_parent, depth_last, id, error);
  static_bond_atom (bond->atom2, window,
  fp, depth_parent, depth_last, id, error);
  }

/***********************************************
 * export bonds, using ids saved in atom->mark *
 ***********************************************/

for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  static_bond_id (GAMGI_CAST_BOND dlist->data,
  window, fp, depth_parent, depth_last, id, error);

/***************
 * clean atoms *
 ***************/

for (dlist = gamgi->bonds; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  bond->atom1->mark = 0;
  bond->atom2->mark = 0;
  }
}

static void static_object_atom_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/*******************
 * write atom list *
 *******************/

for (dlist = gamgi->atoms; dlist != NULL; dlist = dlist->next)
  static_object_atom (GAMGI_CAST_ATOM dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_direction_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/************************
 * write direction list *
 ************************/

for (dlist = gamgi->directions; dlist != NULL; dlist = dlist->next)
  static_object_direction (GAMGI_CAST_DIRECTION dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_plane_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write plane list *
 ********************/

for (dlist = gamgi->planes; dlist != NULL; dlist = dlist->next)
  static_object_plane (GAMGI_CAST_PLANE dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_group_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_group *group;
gamgi_dlist *dlist;

/**********************
 * write cluster list *
 **********************/

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, TRUE);

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  if (group->mark == FALSE) static_object_group (group, window,
  GAMGI_CAST_OBJECT group, fp, depth_parent, depth_last, id, error);
  }

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, FALSE);
}

static void static_object_molecule_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/***********************
 * write molecule list *
 ***********************/

for (dlist = gamgi->molecules; dlist != NULL; dlist = dlist->next)
  static_object_molecule (GAMGI_CAST_MOLECULE dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_cluster_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

/**********************
 * write cluster list *
 **********************/

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, TRUE);

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  if (cluster->mark == FALSE) static_object_cluster (cluster, window,
  GAMGI_CAST_OBJECT cluster, fp, depth_parent, depth_last, id, error);
  }

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, FALSE);
}

static void static_object_cell_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/*******************
 * write cell list *
 *******************/

for (dlist = gamgi->cells; dlist != NULL; dlist = dlist->next)
  static_object_cell (GAMGI_CAST_CELL dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_arrow_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write arrow list *
 ********************/

for (dlist = gamgi->arrows; dlist != NULL; dlist = dlist->next)
  static_object_arrow (GAMGI_CAST_ARROW dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_shape_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write shape list *
 ********************/

for (dlist = gamgi->shapes; dlist != NULL; dlist = dlist->next)
  static_object_shape (GAMGI_CAST_SHAPE dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_graph_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write graph list *
 ********************/

for (dlist = gamgi->graphs; dlist != NULL; dlist = dlist->next)
  static_object_graph (GAMGI_CAST_GRAPH dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_assembly_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_assembly *assembly;
gamgi_dlist *dlist;

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, TRUE);

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  {
  assembly = GAMGI_CAST_ASSEMBLY dlist->data;
  if (assembly->mark == FALSE) static_object_assembly (assembly, window,
  GAMGI_CAST_OBJECT assembly, fp, depth_parent, depth_last, id, error);
  }

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, FALSE);
}

static void static_object_light_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write light list *
 ********************/

for (dlist = gamgi->lights; dlist != NULL; dlist = dlist->next)
  static_object_light (GAMGI_CAST_LIGHT dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_layer_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/********************
 * write layer list *
 ********************/

for (dlist = gamgi->layers; dlist != NULL; dlist = dlist->next)
  static_object_layer (GAMGI_CAST_LAYER dlist->data, window,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_window_list (FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_dlist *dlist;

/*********************
 * write window list *
 *********************/

for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  static_object_window (GAMGI_CAST_WINDOW dlist->data,
  GAMGI_CAST_OBJECT dlist->data, fp, depth_parent, depth_last, id, error);
}

static void static_object_gamgi_list (FILE *fp, int depth_parent,
int *depth_last, int *id, gamgi_bool *error)
{
gamgi_window *window;

/********************
 * write gamgi list *
 ********************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  static_object_window (window, GAMGI_CAST_OBJECT window,
  fp, depth_parent, depth_last, id, error);
}

void static_object_list (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, int *id, gamgi_bool *error)
{
switch (gamgi->focus)
  {
  case GAMGI_ENGINE_TEXT:
  static_object_text_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_ORBITAL:
  static_object_orbital_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_BOND:
  static_object_bond_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_ATOM:
  static_object_atom_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_DIRECTION:
  static_object_direction_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_PLANE:
  static_object_plane_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_GROUP:
  static_object_group_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_MOLECULE:
  static_object_molecule_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_CLUSTER:
  static_object_cluster_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_CELL:
  static_object_cell_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_ARROW:
  static_object_arrow_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_SHAPE:
  static_object_shape_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_GRAPH:
  static_object_graph_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  static_object_assembly_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_LIGHT:
  static_object_light_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_LAYER:
  static_object_layer_list (window, fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_WINDOW:
  static_object_window_list (fp,
  depth_parent, depth_last, id, error);
  break;

  case GAMGI_ENGINE_GAMGI:
  static_object_gamgi_list (fp,
  depth_parent, depth_last, id, error);
  break;

  default:
  return;
  }
}

static void static_object (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_object *object = window->focus;
int id = 1;

if (gamgi->focus != FALSE)
  { static_object_list (window, fp,
    depth_parent, depth_last, &id, error); return; }

switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  static_object_text (GAMGI_CAST_TEXT object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_ORBITAL:
  static_object_orbital (GAMGI_CAST_ORBITAL object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_BOND:
  static_bond_single (GAMGI_CAST_BOND object,
  window, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_ATOM:
  static_object_atom (GAMGI_CAST_ATOM object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_DIRECTION:
  static_object_direction (GAMGI_CAST_DIRECTION object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_PLANE:
  static_object_plane (GAMGI_CAST_PLANE object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_GROUP:
  static_object_group (GAMGI_CAST_GROUP object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_MOLECULE:
  static_object_molecule (GAMGI_CAST_MOLECULE object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_CLUSTER:
  static_object_cluster (GAMGI_CAST_CLUSTER object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_CELL:
  static_object_cell (GAMGI_CAST_CELL object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_ARROW:
  static_object_arrow (GAMGI_CAST_ARROW object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_SHAPE:
  static_object_shape (GAMGI_CAST_SHAPE object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_GRAPH:
  static_object_graph (GAMGI_CAST_GRAPH object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  static_object_assembly (GAMGI_CAST_ASSEMBLY object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_LIGHT:
  static_object_light (GAMGI_CAST_LIGHT object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_LAYER:
  static_object_layer (GAMGI_CAST_LAYER object, 
  window, object, fp, depth_parent, depth_last, &id, error);
  break;

  case GAMGI_ENGINE_WINDOW:
  static_object_window (window,
  object, fp, depth_parent, depth_last, &id, error);
  break;
  }
}

static void static_header_xml (FILE *fp, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int column;

/********************************
 * first row: XML common header *
 ********************************/

gamgi_io_file_fprintf (fp, "<?xml", error);
column = strlen ("<?xml");

sprintf (string, "version=\"%s\"", GAMGI_EXPAT_VERSION);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

sprintf (string, "encoding=\"%s\"", GAMGI_EXPAT_ENCODING);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

sprintf (string, "standalone=\"%s\"", GAMGI_EXPAT_STANDALONE);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

static_end (fp, "?>\n", error);
}

static void static_header_gml (FILE *fp, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int column;

/*************
 * first row *
 *************/

static_header_xml (fp, error);

/**************
 * second row *
 **************/

gamgi_io_file_fprintf (fp, "<!DOCTYPE", error);
column = strlen ("<!DOCTYPE");

gamgi_expat_export_attribute (fp, "gml", 0, &column, error);

gamgi_expat_export_attribute (fp, "SYSTEM", 0, &column, error);

sprintf (string, "\"%s\"", GAMGI_IO_GML_SYSTEM);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

static_end (fp, ">\n", error);
}

static void static_header_x3d (FILE *fp, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int column;

/*************
 * first row *
 *************/

static_header_xml (fp, error);

/**************
 * second row *
 **************/

gamgi_io_file_fprintf (fp, "<!DOCTYPE", error);
column = strlen ("<!DOCTYPE");

gamgi_expat_export_attribute (fp, "X3D", 0, &column, error);

gamgi_expat_export_attribute (fp, "PUBLIC", 0, &column, error);

sprintf (string, "\"%s\"", GAMGI_IO_X3D_PUBLIC);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

sprintf (string, "\"%s\"", GAMGI_IO_X3D_SYSTEM);
gamgi_expat_export_attribute (fp, string, 0, &column, error);

static_end (fp, ">\n", error);
}

int gamgi_expat_export_length (gamgi_text *text)
{
char *s;
int i;

/***********************************************
 * get buffer size (add 1 to count final '\0') *
 ***********************************************/

i = 0;
for (s = text->string; *s != '\0'; s++)
  {
  i++;
  if (*s == '<' || *s == '>') i += 3;
  else if (*s == '&') i += 4;
  else if (*s == '\'' || *s == '\"') i += 5;
  }

return i;
}

void gamgi_expat_export_string (gamgi_text *text, char *buffer)
{
char *s;
int i;

/******************************
 * translate string to buffer *
 ******************************/

i = 0;
for (s = text->string; *s != '\0'; s++)
  {
  switch (*s)
    {
    case '<':
    buffer[i++] = '&'; buffer[i++] = 'l';
    buffer[i++] = 't'; buffer[i++] = ';';
    break;

    case '>':
    buffer[i++] = '&'; buffer[i++] = 'g';
    buffer[i++] = 't'; buffer[i++] = ';';
    break;

    case '&':
    buffer[i++] = '&'; buffer[i++] = 'a';
    buffer[i++] = 'm'; buffer[i++] = 'p';
    buffer[i++] = ';';
    break;

    case '\'':
    buffer[i++] = '&'; buffer[i++] = 'a';
    buffer[i++] = 'p'; buffer[i++] = 'o';
    buffer[i++] = 's'; buffer[i++] = ';';
    break;

    case '\"':
    buffer[i++] = '&'; buffer[i++] = 'q';
    buffer[i++] = 'u'; buffer[i++] = 'o';
    buffer[i++] = 't'; buffer[i++] = ';';
    break;

    default:
    buffer[i++] = *s;
    break;
    }
  }
buffer[i] = '\0';
}

void gamgi_expat_export_gml (FILE *fp, gamgi_window *window, gamgi_bool *error)
{
int depth = GAMGI_EXPAT_DEPTH_MIN;
int depth_last = GAMGI_EXPAT_DEPTH_MIN;

/********************
 * write gml header *
 ********************/

if (GAMGI_EXPAT_HEADER_GML == TRUE) static_header_gml (fp, error);

/***********************
 * start gml root node *
 ***********************/

static_space (fp, depth * GAMGI_EXPAT_START, error);
gamgi_io_file_fprintf (fp, "<gml", error);

/************************************
 * write config BEFORE object data! *
 ************************************/

if (GAMGI_EXPAT_CONFIG == TRUE) 
  static_config (window, fp, depth, &depth_last, error);
if (GAMGI_EXPAT_OBJECT == TRUE)
  static_object (window, fp, depth, &depth_last, error);

/*********************
 * end gml root node *
 *********************/

if (depth_last == depth) gamgi_io_file_fprintf (fp, "/>", error);
else static_start (fp, "</gml>", depth, error);
gamgi_io_file_fprintf (fp, "\n", error);
}

void gamgi_expat_export_x3d (FILE *fp, gamgi_window *window, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int depth = 0;

/********************
 * write X3D header *
 ********************/

if (GAMGI_EXPAT_HEADER_X3D == TRUE) static_header_x3d (fp, error);

/***********************
 * start X3D root node *
 ***********************/

sprintf (string, "<X3D profile=\"%s\" version=\"%s\">\n", 
GAMGI_IO_X3D_PROFILE, GAMGI_IO_X3D_VERSION);
gamgi_io_file_fprintf (fp, string, error);

/********************************************************
 * export in X3D format everything in the current layer *
 ********************************************************/

gamgi_io_x3d (fp, window, error);

/*********************
 * end X3D root node *
 *********************/

static_start (fp, "</X3D>", depth, error);
gamgi_io_file_fprintf (fp, "\n", error);
}
