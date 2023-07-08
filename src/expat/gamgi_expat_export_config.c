/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export_config.c
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
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_expat.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_property.h"
#include "gamgi_expat_export.h"

void gamgi_expat_export_config_help (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_help_class *help_class = gamgi->help;
char string[GAMGI_ENGINE_TOKEN + GAMGI_IO_FULLNAME];
char *name;
int column;

/************************
 * source: local,remote *
 ************************/

/***********************************************************
 * remote source is non-default only when local is defined *
 ***********************************************************/

if (help_class->source_in == FALSE && help_class->local != NULL)
  {
  sprintf (string, "source=\"remote\"");
  gamgi_expat_export_element (fp, "<help", string,
  depth_parent, depth_last, &column, output, error);
  }

/************************
 * agent: gamgi,browser *
 ************************/

if (help_class->agent_in == FALSE)
  {
  sprintf (string, "agent=\"browser\"");
  gamgi_expat_export_element (fp, "<help", string,
  depth_parent, depth_last, &column, output, error);
  }

/*******************************
 * paths: local,remote,browser *
 *******************************/

/***************************************
 * export only if the configuration    *
 * path is different from the path     *
 * defined by the environment variable *
 ***************************************/

name = getenv ("GAMGI_HELP");
if (help_class->local != NULL &&
(name == NULL || strcmp (help_class->local, name) != 0))
  {
  sprintf (string, "local=\"%s\"", help_class->local);
  gamgi_expat_export_element (fp, "<help", string,
  depth_parent, depth_last, &column, output, error);
  }

if (help_class->remote != NULL &&
strcmp (help_class->remote, GAMGI_IO_WEBSITE) != 0)
  {
  sprintf (string, "remote=\"%s\"", help_class->remote);
  gamgi_expat_export_element (fp, "<help", string,
  depth_parent, depth_last, &column, output, error);
  }

/***************************************
 * export only if the configuration    *
 * path is different from the path     *
 * defined by the environment variable *
 ***************************************/

name = getenv ("BROWSER");
if (help_class->browser != NULL &&
(name == NULL || strcmp (help_class->browser, name) != 0))
  {
  sprintf (string, "browser=\"%s\"", help_class->browser);
  gamgi_expat_export_element (fp, "<help", string,
  depth_parent, depth_last, &column, output, error);
  }
}

void gamgi_expat_export_config_text (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_text_class *text_class = gamgi->text;
char string[GAMGI_ENGINE_STRING];
int column;

/***********************
 * style: wired, solid *
 ***********************/

if (text_class->style != GAMGI_MESA_TEXT_STYLE)
  {
  if (text_class->style == GAMGI_MESA_WIRED)
    sprintf (string, "style=\"wired\"");
  if (text_class->style == GAMGI_MESA_SOLID)
    sprintf (string, "style=\"solid\"");
  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);
  }

/**************
 * wired font: *
 **************/

if (text_class->wired != GAMGI_IO_TEXT_WIRED)
  {
  if (text_class->wired == GAMGI_IO_SUN_ROMAN)
    sprintf (string, "wired=\"roman\"");
  else if (text_class->wired == GAMGI_IO_SUN_ROMAN_MONO)
    sprintf (string, "wired=\"roman_mono\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SANS)
    sprintf (string, "wired=\"sans\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SANS_BOLD)
    sprintf (string, "wired=\"sans_bold\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SERIF)
    sprintf (string, "wired=\"serif\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SERIF_BOLD)
    sprintf (string, "wired=\"serif_bold\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SCRIPT)
    sprintf (string, "wired=\"script\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SCRIPT_BOLD)
    sprintf (string, "wired=\"script_bold\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_GOTHIC_ENGLISH)
    sprintf (string, "wired=\"gothic_english\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_GOTHIC_GERMAN)
    sprintf (string, "wired=\"gothic_german\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_GOTHIC_ITALIAN)
    sprintf (string, "wired=\"gothic_italian\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SYMBOL_SANS)
    sprintf (string, "wired=\"symbol_sans\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SYMBOL_SERIF)
    sprintf (string, "wired=\"symbol_serif\"");
  else if (text_class->wired == GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD)
    sprintf (string, "wired=\"symbol_serif_bold\"");

  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);
  }

/**************
 * solid font *
 **************/

if (text_class->solid != GAMGI_IO_TEXT_SOLID)
  {
  if (text_class->solid == GAMGI_IO_DEJAVU_SANS)
    sprintf (string, "solid=\"sans\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SANS_BOLD)
    sprintf (string, "solid=\"sans_bold\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SANS_ITALIC)
    sprintf (string, "solid=\"sans_italic\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC)
    sprintf (string, "solid=\"sans_bold_italic\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SERIF)
    sprintf (string, "solid=\"serif\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SERIF_BOLD)
    sprintf (string, "solid=\"serif_bold\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SERIF_ITALIC)
    sprintf (string, "solid=\"serif_italic\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC)
    sprintf (string, "solid=\"serif_bold_italic\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_MONO)
    sprintf (string, "solid=\"mono\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_MONO_BOLD)
    sprintf (string, "solid=\"mono_bold\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_MONO_ITALIC)
    sprintf (string, "solid=\"mono_italic\"");
  else if (text_class->solid == GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC)
    sprintf (string, "solid=\"mono_bold_italic\"");

  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);
  }

/****************************
 * red,green,blue (coupled) *
 ****************************/

if (fabs (text_class->red - GAMGI_MESA_TEXT_R) > GAMGI_MATH_TOLERANCE
|| fabs (text_class->green - GAMGI_MESA_TEXT_G) > GAMGI_MATH_TOLERANCE
|| fabs (text_class->blue - GAMGI_MESA_TEXT_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, text_class->red);
  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, text_class->green);
  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, text_class->blue);
  gamgi_expat_export_element (fp, "<text", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_orbital (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_bond_global (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_bond_class *bond_class = gamgi->bond;
char string[GAMGI_ENGINE_STRING];
int column;

/***********************
 * style: wired, solid *
 ***********************/

if (bond_class->draw != GAMGI_MESA_BOND_DRAW)
  {
  if (bond_class->draw == gamgi_mesa_bond_draw_line)
    sprintf (string, "style=\"wired\"");
  else if (bond_class->draw == gamgi_mesa_bond_draw_cylinder)
    sprintf (string, "style=\"solid\"");
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  }

/************************************
 * global bond limits: lower, upper *
 ************************************/

if (fabs (bond_class->lower - GAMGI_CHEM_BOND_LOWER) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "lower=\"%.*f\"", gamgi->gamgi->length, bond_class->lower);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (bond_class->upper - GAMGI_CHEM_BOND_UPPER) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "upper=\"%.*f\"", gamgi->gamgi->length, bond_class->upper);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  }

/********
 * size *
 ********/

if (fabs (bond_class->size - GAMGI_CHEM_BOND_SIZE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "size=\"%.*f\"", GAMGI_MATH_DECIMAL_SIZE, bond_class->size);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  }

/**************
 * color flag *
 **************/
  
if (bond_class->color != GAMGI_MESA_BOND_COLOR)
  {
  if (bond_class->color == TRUE)
    sprintf (string, "color=\"yes\"");
  else
    sprintf (string, "color=\"no\"");
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  } 

/****************************
 * red,green,blue (coupled) *
 ****************************/

if (fabs (bond_class->red - GAMGI_MESA_BOND_R) > GAMGI_MATH_TOLERANCE
|| fabs (bond_class->green - GAMGI_MESA_BOND_G) > GAMGI_MATH_TOLERANCE
|| fabs (bond_class->blue - GAMGI_MESA_BOND_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, bond_class->red);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, bond_class->green);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, bond_class->blue);
  gamgi_expat_export_element (fp, "<bond", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_bond_local (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error,
int element1, int element2, double min, double max)
{
char string[GAMGI_ENGINE_LINE / 2];
char name1[GAMGI_ENGINE_TOKEN];
char name2[GAMGI_ENGINE_TOKEN];
int column;

gamgi_chem_atom_name (element1, name1);
gamgi_chem_atom_name (element2, name2);

/*******************************************************
 * write first the higher element: element1 > element2 *
 *******************************************************/

sprintf (string, "element1=\"%s\" element2=\"%s\""
" min=\"%.*f\" max=\"%.*f\"", name1, name2, 
gamgi->gamgi->length, min, gamgi->gamgi->length, max);

gamgi_expat_export_element (fp, "<bond", string,
depth_parent, depth_last, &column, output, error);
}

void gamgi_expat_export_config_atom_global (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_atom_class *atom_class = gamgi->atom;
char string[GAMGI_ENGINE_STRING];
int column;

/***************
 * temperature *
 ***************/

if (fabs (atom_class->temperature - GAMGI_CHEM_ATOM_TEMPERATURE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "temperature=\"%.*f\"",
  GAMGI_MATH_DECIMAL_OCCUPANCY, atom_class->temperature);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

/*************************
 * style, size, variancy *
 *************************/

if (atom_class->draw != GAMGI_MESA_ATOM_DRAW)
  {
  if (atom_class->draw == gamgi_mesa_atom_draw_cross)
    sprintf (string, "style=\"wired\"");
  else if (atom_class->draw == gamgi_mesa_atom_draw_sphere)
    sprintf (string, "style=\"solid\"");
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (atom_class->size - GAMGI_CHEM_ATOM_SIZE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "size=\"%.*f\"", GAMGI_MATH_DECIMAL_SIZE, atom_class->size);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (atom_class->variancy - GAMGI_CHEM_ATOM_VARIANCY) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "variancy=\"%.*f\"", GAMGI_MATH_DECIMAL_VARIANCY, atom_class->variancy);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

/*********************************************
 * minimum radius, curved slices, line width *
 *********************************************/

if (fabs (atom_class->min - 
gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_DEFAULT]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "min=\"%.*f\"", gamgi->gamgi->length, atom_class->min);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

if (atom_class->slices != GAMGI_MESA_ATOM_SLICES)
  {
  sprintf (string, "slices=\"%d\"", atom_class->slices);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

if (atom_class->width != GAMGI_MESA_ATOM_WIDTH)
  {
  sprintf (string, "width=\"%d\"", atom_class->width);
  gamgi_expat_export_element (fp, "<atom", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_atom_local (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error,
int element, double mass, double radius, float red, float green, float blue)
{
gamgi_atom_class *atom_class = gamgi->atom;
char string_element[GAMGI_ENGINE_STRING];
char string_attribute[GAMGI_ENGINE_STRING];
char token[GAMGI_ENGINE_TOKEN];
int column;

gamgi_chem_atom_name (element, token);
sprintf (string_element, "<atom element=\"%s\"", token);

if (fabs (atom_class->mass[element] - mass) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string_attribute, "mass=\"%.*f\"",
  gamgi->gamgi->mass, atom_class->mass[element]);
  gamgi_expat_export_element (fp, string_element, string_attribute,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (atom_class->radius[element] - radius) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string_attribute, "radius=\"%.*f\"", 
  gamgi->gamgi->length, atom_class->radius[element]);
  gamgi_expat_export_element (fp, string_element, string_attribute,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (atom_class->red[element] - red) > GAMGI_MATH_TOLERANCE
|| fabs (atom_class->green[element] - green) > GAMGI_MATH_TOLERANCE
|| fabs (atom_class->blue[element] - blue) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string_attribute, "red=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, atom_class->red[element]);
  gamgi_expat_export_element (fp, string_element, string_attribute,
  depth_parent, depth_last, &column, output, error);

  sprintf (string_attribute, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, atom_class->green[element]);
  gamgi_expat_export_element (fp, string_element, string_attribute,
  depth_parent, depth_last, &column, output, error);

  sprintf (string_attribute, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, atom_class->blue[element]);
  gamgi_expat_export_element (fp, string_element, string_attribute,
  depth_parent, depth_last, &column, output, error);
  }
}

void gamgi_expat_export_config_direction (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_direction_class *direction_class = gamgi->direction;
char string[GAMGI_ENGINE_STRING];
int column;

/****************************
 * red,green,blue (coupled) *
 ****************************/

if (fabs (direction_class->red - GAMGI_MESA_DIRECTION_R) > GAMGI_MATH_TOLERANCE
|| fabs (direction_class->green - GAMGI_MESA_DIRECTION_G) > GAMGI_MATH_TOLERANCE
|| fabs (direction_class->blue - GAMGI_MESA_DIRECTION_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, direction_class->red);
  gamgi_expat_export_element (fp, "<direction", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, direction_class->green);
  gamgi_expat_export_element (fp, "<direction", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, direction_class->blue);
  gamgi_expat_export_element (fp, "<direction", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_plane (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_plane_class *plane_class = gamgi->plane;
char string[GAMGI_ENGINE_STRING];
int column;

/****************************
 * red,green,blue (coupled) *
 ****************************/

if (fabs (plane_class->red - GAMGI_MESA_PLANE_R) > GAMGI_MATH_TOLERANCE
|| fabs (plane_class->green - GAMGI_MESA_PLANE_G) > GAMGI_MATH_TOLERANCE
|| fabs (plane_class->blue - GAMGI_MESA_PLANE_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, plane_class->red);
  gamgi_expat_export_element (fp, "<plane", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, plane_class->green);
  gamgi_expat_export_element (fp, "<plane", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, plane_class->blue);
  gamgi_expat_export_element (fp, "<plane", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_group (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_molecule (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_cluster (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_cell (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_cell_class *cell_class = gamgi->cell;
char string[GAMGI_ENGINE_STRING];
int column;

/***************
 * model, axes *
 ***************/

if (cell_class->model != GAMGI_PHYS_CELL_MODEL)
  {
  if (cell_class->model == GAMGI_PHYS_CONVENTIONAL)
    sprintf (string, "model=\"conventional\"");
  else if (cell_class->model == GAMGI_PHYS_PRIMITIVE)
    sprintf (string, "model=\"primitive\"");
  else if (cell_class->model == GAMGI_PHYS_WIGNER)
    sprintf (string, "model=\"wigner\"");
  else if (cell_class->model == GAMGI_PHYS_PARALLELEPIPED)
    sprintf (string, "model=\"parallelepiped\"");
  else if (cell_class->model == GAMGI_PHYS_SPHERE)
    sprintf (string, "model=\"sphere\"");
  else if (cell_class->model == GAMGI_PHYS_PROJECTION)
    sprintf (string, "model=\"projection\"");

  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

if (cell_class->axes != GAMGI_PHYS_CELL_AXES)
  {
  if (cell_class->axes == TRUE)
    sprintf (string, "axes=\"yes\"");
  else
    sprintf (string, "axes=\"no\"");

  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

/*************************
 * borders, faces, nodes *
 *************************/

if (cell_class->borders != GAMGI_PHYS_CELL_BORDERS)
  {
  if (cell_class->borders == GAMGI_PHYS_ALL)
    sprintf (string, "borders=\"all\"");
  else if (cell_class->borders == GAMGI_PHYS_FACES)
    sprintf (string, "borders=\"faces\"");
  else if (cell_class->borders == GAMGI_PHYS_EDGES)
    sprintf (string, "borders=\"edges\"");
  else if (cell_class->borders == GAMGI_PHYS_NONE)
    sprintf (string, "borders=\"none\"");

  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

if (cell_class->faces != GAMGI_PHYS_CELL_FACES)
  {
  if (cell_class->faces == TRUE)
    sprintf (string, "faces=\"yes\"");
  else
    sprintf (string, "faces=\"no\"");

  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

if (cell_class->nodes != GAMGI_PHYS_CELL_NODES)
  {
  if (cell_class->nodes == TRUE)
    sprintf (string, "nodes=\"yes\"");
  else
    sprintf (string, "nodes=\"no\"");

  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

/****************************
 * red,green,blue (coupled) *
 ****************************/

if (fabs (cell_class->red - GAMGI_MESA_CELL_R) > GAMGI_MATH_TOLERANCE
|| fabs (cell_class->green - GAMGI_MESA_CELL_G) > GAMGI_MATH_TOLERANCE
|| fabs (cell_class->blue - GAMGI_MESA_CELL_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, cell_class->red);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "green=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, cell_class->green);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "blue=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, cell_class->blue);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

/************************************************************
 * Global parameters: axes sise, borders width, nodes width *
 ************************************************************/

if (fabs (cell_class->axes_size - GAMGI_MESA_AXES_SIZE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "axes_size=\"%.*f\"",
  GAMGI_MATH_DECIMAL_SIZE, cell_class->axes_size);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

if (cell_class->borders_width != GAMGI_MESA_CELL_LINE)
  {
  sprintf (string, "borders_width=\"%d\"", cell_class->borders_width);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

if (cell_class->nodes_width != GAMGI_MESA_CELL_POINT)
  {
  sprintf (string, "nodes_width=\"%d\"", cell_class->nodes_width);
  gamgi_expat_export_element (fp, "<cell", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_arrow (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_shape (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_graph (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_assembly (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
}

void gamgi_expat_export_config_light (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_light_class *light_class = gamgi->light;
char string[GAMGI_ENGINE_STRING];
int column;

/*************
 * shininess *
 *************/

if (fabs (light_class->shininess - GAMGI_MESA_LIGHT_SHININESS) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "shininess=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_SHININESS, light_class->shininess);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);
  }

/*********************
 * ambient component *
 *********************/

if (fabs (light_class->ambient[0] - GAMGI_MESA_LIGHT_AMBIENT_R) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->ambient[1] - GAMGI_MESA_LIGHT_AMBIENT_G) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->ambient[2] - GAMGI_MESA_LIGHT_AMBIENT_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "ambient_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, light_class->ambient[0]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "ambient_g=\"%.*f\"",  
  GAMGI_MATH_DECIMAL_COLOR, light_class->ambient[1]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "ambient_b=\"%.*f\"",  
  GAMGI_MATH_DECIMAL_COLOR, light_class->ambient[2]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);
  }

/*********************
 * diffuse component *
 *********************/

if (fabs (light_class->diffuse[0] - GAMGI_MESA_LIGHT_DIFFUSE_R) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->diffuse[1] - GAMGI_MESA_LIGHT_DIFFUSE_G) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->diffuse[2] - GAMGI_MESA_LIGHT_DIFFUSE_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "diffuse_r=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->diffuse[0]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "diffuse_g=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->diffuse[1]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "diffuse_b=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->diffuse[2]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);
  }

/**********************
 * specular component *
 **********************/

if (fabs (light_class->specular[0] - GAMGI_MESA_LIGHT_SPECULAR_R) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->specular[1] - GAMGI_MESA_LIGHT_SPECULAR_G) > GAMGI_MATH_TOLERANCE
|| fabs (light_class->specular[2] - GAMGI_MESA_LIGHT_SPECULAR_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "specular_r=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->specular[0]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "specular_g=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->specular[1]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "specular_b=\"%.*f\"",
  GAMGI_MATH_DECIMAL_COLOR, light_class->specular[2]);
  gamgi_expat_export_element (fp, "<light", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_layer (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_layer_class *layer_class = gamgi->layer;
char string[GAMGI_ENGINE_STRING];
int column;

/**************
 * undo, save *
 **************/

if (layer_class->undo != GAMGI_ENGINE_UNDO)
  {
  sprintf (string, "undo=\"%d\"", layer_class->undo);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

if (layer_class->save != GAMGI_ENGINE_SAVE)
  {
  sprintf (string, "save=\"%d\"", layer_class->save);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

/*******************************
 * projection parameters:      *
 * perspective, top, near, far *
 *******************************/

if (layer_class->perspective != GAMGI_MESA_LAYER_PERSPECTIVE)
  {
  if (layer_class->perspective == TRUE)
    sprintf (string, "perspective=\"yes\"");
  else
    sprintf (string, "perspective=\"no\"");
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (layer_class->top - GAMGI_MESA_LAYER_TOP) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "top=\"%.*f\"",
  gamgi->gamgi->length, layer_class->top);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (layer_class->near - GAMGI_MESA_LAYER_NEAR) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "near=\"%.*f\"",
  gamgi->gamgi->length, layer_class->near);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (layer_class->far - GAMGI_MESA_LAYER_FAR) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "far=\"%.*f\"",
  gamgi->gamgi->length, layer_class->far);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);
  }

/*******************************************************
 * modelview parameters: eye_x, eye_y, eye_z (coupled) *
 *******************************************************/

if (fabs (layer_class->eye[0] - GAMGI_MESA_LAYER_EYEX) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->eye[1] - GAMGI_MESA_LAYER_EYEY) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->eye[2] - GAMGI_MESA_LAYER_EYEZ) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "eye_x=\"%.*f\"",
  gamgi->gamgi->length, layer_class->eye[0]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "eye_y=\"%.*f\"",
  gamgi->gamgi->length, layer_class->eye[1]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "eye_z=\"%.*f\"",
  gamgi->gamgi->length, layer_class->eye[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/****************************************************************
 * modelview parameters: center_x, center_y, center_z (coupled) *
 ****************************************************************/

if (fabs (layer_class->center[0] - GAMGI_MESA_LAYER_CENTERX) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->center[1] - GAMGI_MESA_LAYER_CENTERY) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->center[2] - GAMGI_MESA_LAYER_CENTERZ) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"",
  gamgi->gamgi->length, layer_class->center[0]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "center_y=\"%.*f\"",
  gamgi->gamgi->length, layer_class->center[1]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "center_z=\"%.*f\"",
  gamgi->gamgi->length, layer_class->center[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/****************************************************
 * modelview parameters: up_x, up_y, up_z (coupled) *
 ****************************************************/

if (fabs (layer_class->up[0] - GAMGI_MESA_LAYER_UPX) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->up[1] - GAMGI_MESA_LAYER_UPY) > GAMGI_MATH_TOLERANCE
|| fabs (layer_class->up[2] - GAMGI_MESA_LAYER_UPZ) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "up_x=\"%.*f\"",
  gamgi->gamgi->length, layer_class->up[0]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "up_y=\"%.*f\"",
  gamgi->gamgi->length, layer_class->up[1]);
  gamgi_expat_export_element (fp, "<layer", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "up_z=\"%.*f\"",
  gamgi->gamgi->length, layer_class->up[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }
}

void gamgi_expat_export_config_window (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_window_class *window_class = gamgi->window;
char string[GAMGI_ENGINE_STRING];
int column;

/********************
 * ruler parameters *
 ********************/

/**********
 * rotate *
 **********/

if (fabs (window_class->rotate_max - GAMGI_GTK_ROTATE_MAX) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "rotate_max=\"%.*f\"",
  gamgi->gamgi->angle, window_class->rotate_max);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (window_class->rotate_step - GAMGI_GTK_ROTATE_STEP) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "rotate_step=\"%.*f\"",
  gamgi->gamgi->angle, window_class->rotate_step);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

/********
 * move *
 ********/

if (fabs (window_class->move_max - GAMGI_GTK_MOVE_MAX) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "move_max=\"%.*f\"",
  gamgi->gamgi->angle, window_class->move_max);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (window_class->move_step - GAMGI_GTK_MOVE_STEP) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "move_step=\"%.*f\"",
  gamgi->gamgi->angle, window_class->move_step);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

/*********
 * scale *
 *********/

if (fabs (window_class->scale_max - GAMGI_GTK_SCALE_MAX) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale_max=\"%.*f\"",
  gamgi->gamgi->angle, window_class->scale_max);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

if (fabs (window_class->scale_step - GAMGI_GTK_SCALE_STEP) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale_step=\"%.*f\"",
  gamgi->gamgi->angle, window_class->scale_step);
  gamgi_expat_export_element (fp, "<window", string,
  depth_parent, depth_last, &column, output, error);
  }

}

void gamgi_expat_export_config_gamgi (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error)
{
gamgi_gamgi_class *gamgi_class = gamgi->gamgi;
char string[GAMGI_ENGINE_STRING];
int column;

/**************
 * beep sound *
 **************/

if (gamgi_class->beep != GAMGI_GTK_BEEP)
  {
  if (gamgi_class->beep == TRUE)
    sprintf (string, "sound=\"yes\"");
  else
    sprintf (string, "sound=\"no\"");
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/******************
 * pick tolerance *
 ******************/

if (gamgi_class->tolerance != GAMGI_MESA_PICK_TOLERANCE)
  {
  sprintf (string, "tolerance=\"%d\"", gamgi_class->tolerance);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/***************************
 * mouse buttons (coupled) *
 ***************************/

if (gamgi_class->button1 != GDK_BUTTON1_MASK
|| gamgi_class->button2 != GDK_BUTTON2_MASK
|| gamgi_class->button3 != GDK_BUTTON3_MASK)
  {
  sprintf (string, "button1=\"%d\"", gamgi_class->button1);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "button2=\"%d\"", gamgi_class->button2);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "button1=\"%d\"", gamgi_class->button3);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/**********************************************************
 * number of decimal figures: length, angle, charge, mass *
 **********************************************************/

if (gamgi_class->length != GAMGI_MATH_DECIMAL_LENGTH)
  {
  sprintf (string, "length=\"%d\"", gamgi_class->length);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

if (gamgi_class->angle != GAMGI_MATH_DECIMAL_ANGLE)
  {
  sprintf (string, "angle=\"%d\"", gamgi_class->angle);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

if (gamgi_class->charge != GAMGI_MATH_DECIMAL_CHARGE)
  {
  sprintf (string, "charge=\"%d\"", gamgi_class->charge);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

if (gamgi_class->mass != GAMGI_MATH_DECIMAL_MASS)
  {
  sprintf (string, "mass=\"%d\"", gamgi_class->mass);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/**************************************
 * background GtkText color (coupled) *
 **************************************/

if (fabs (gamgi_class->background[0] - GAMGI_GTK_BACKGROUND_R) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->background[1] - GAMGI_GTK_BACKGROUND_G) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->background[2] - GAMGI_GTK_BACKGROUND_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "base_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[0]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "base_g=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[1]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "base_b=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->background[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/**************************************
 * foreground GtkText color (coupled) *
 **************************************/

if (fabs (gamgi_class->foreground[0] - GAMGI_GTK_FOREGROUND_R) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->foreground[1] - GAMGI_GTK_FOREGROUND_G) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->foreground[2] - GAMGI_GTK_FOREGROUND_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "text_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[0]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  
  sprintf (string, "text_g=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[1]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "text_b=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->foreground[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/*********************************
 * title GtkText color (coupled) *
 *********************************/

if (fabs (gamgi_class->title[0] - GAMGI_GTK_TITLE_R) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->title[1] - GAMGI_GTK_TITLE_G) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->title[2] - GAMGI_GTK_TITLE_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "title_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[0]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  
  sprintf (string, "title_g=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[1]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "title_b=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->title[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/********************************
 * bold GtkText color (coupled) *
 ********************************/

if (fabs (gamgi_class->bold[0] - GAMGI_GTK_BOLD_R) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->bold[1] - GAMGI_GTK_BOLD_G) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->bold[2] - GAMGI_GTK_BOLD_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "bold_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[0]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  
  sprintf (string, "bold_g=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[1]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "bold_b=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->bold[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

/********************************
 * link GtkText color (coupled) *
 ********************************/

if (fabs (gamgi_class->link[0] - GAMGI_GTK_LINK_R) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->link[1] - GAMGI_GTK_LINK_G) > GAMGI_MATH_TOLERANCE
|| fabs (gamgi_class->link[2] - GAMGI_GTK_LINK_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "link_r=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[0]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  
  sprintf (string, "link_g=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[1]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);

  sprintf (string, "link_b=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_COLOR, gamgi_class->link[2]);
  gamgi_expat_export_element (fp, "<gamgi", string,
  depth_parent, depth_last, &column, output, error);
  }

}
