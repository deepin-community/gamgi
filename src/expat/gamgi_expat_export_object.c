/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export_object.c
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

#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_orbital.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_direction.h"
#include "gamgi_expat_export.h"
#include "gamgi_io_token.h"

void gamgi_expat_export_object_text (gamgi_text *text, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
char *buffer;
double rotate[9], euler[3];
int length;

/*******************
 * name (optional) *
 *******************/

if (strcmp (text->object.name, "Text") != 0)
  {
  sprintf (string, "name=\"%s\"", text->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_TEXT == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/***********************
 * contents (required) *
 ***********************/

/************************************************************
 * translate XML special characters to predefined entities: *
 *    < = &lt; > = &gt; & = &amp; ' = &pos; " = &quot;      *
 ************************************************************/

length = strlen ("contents=\"\"") + gamgi_expat_export_length (text) + 1;
buffer = (char *) malloc (length * sizeof (char));

strcpy (buffer, "contents=\"");
gamgi_expat_export_string (text, buffer + strlen ("contents=\""));
strcat (buffer, "\"");

gamgi_expat_export_attribute (fp, buffer, depth, column, error);
free (buffer);

/********************
 * style (optional) *
 ********************/

if (text->style != gamgi->text->style)
  {
  if (text->style == GAMGI_MESA_WIRED)
    sprintf (string, "style=\"wired\"");
  if (text->style == GAMGI_MESA_SOLID)
    sprintf (string, "style=\"solid\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*******************
 * font (optional) *
 *******************/
 
if (text->style == GAMGI_MESA_WIRED && 
text->font != gamgi->text->wired)
  {
  if (text->font == GAMGI_IO_SUN_ROMAN)
    sprintf (string, "font=\"roman\"");
  else if (text->font == GAMGI_IO_SUN_ROMAN_MONO)
    sprintf (string, "font=\"roman_mono\"");
  else if (text->font == GAMGI_IO_HERSHEY_SANS)
    sprintf (string, "font=\"sans\"");
  else if (text->font == GAMGI_IO_HERSHEY_SANS_BOLD)
    sprintf (string, "font=\"sans_bold\"");
  else if (text->font == GAMGI_IO_HERSHEY_SERIF)
    sprintf (string, "font=\"serif\"");
  else if (text->font == GAMGI_IO_HERSHEY_SERIF_BOLD)
    sprintf (string, "font=\"serif_bold\"");
  else if (text->font == GAMGI_IO_HERSHEY_SCRIPT)
    sprintf (string, "font=\"script\"");
  else if (text->font == GAMGI_IO_HERSHEY_SCRIPT_BOLD)
    sprintf (string, "font=\"script_bold\"");
  else if (text->font == GAMGI_IO_HERSHEY_GOTHIC_ENGLISH)
    sprintf (string, "font=\"gothic_english\"");
  else if (text->font == GAMGI_IO_HERSHEY_GOTHIC_GERMAN)
    sprintf (string, "font=\"gothic_german\"");
  else if (text->font == GAMGI_IO_HERSHEY_GOTHIC_ITALIAN)
    sprintf (string, "font=\"gothic_italian\"");
  else if (text->font == GAMGI_IO_HERSHEY_SYMBOL_SANS)
    sprintf (string, "font=\"symbol_sans\"");
  else if (text->font == GAMGI_IO_HERSHEY_SYMBOL_SERIF)
    sprintf (string, "font=\"symbol_serif\"");
  else if (text->font == GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD)
    sprintf (string, "font=\"symbol_serif_bold\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (text->style == GAMGI_MESA_SOLID && 
text->font != gamgi->text->solid)
  {
  if (text->font == GAMGI_IO_DEJAVU_SANS)
    sprintf (string, "font=\"sans\"");
  else if (text->font == GAMGI_IO_DEJAVU_SANS_BOLD)
    sprintf (string, "font=\"sans_bold\"");
  else if (text->font == GAMGI_IO_DEJAVU_SANS_ITALIC)
    sprintf (string, "font=\"sans_italic\"");
  else if (text->font == GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC)
    sprintf (string, "font=\"sans_bold_italic\"");
  else if (text->font == GAMGI_IO_DEJAVU_SERIF)
    sprintf (string, "font=\"serif\"");
  else if (text->font == GAMGI_IO_DEJAVU_SERIF_BOLD)
    sprintf (string, "font=\"serif_bold\"");
  else if (text->font == GAMGI_IO_DEJAVU_SERIF_ITALIC)
    sprintf (string, "font=\"serif_italic\"");
  else if (text->font == GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC)
    sprintf (string, "font=\"serif_bold_italic\"");
  else if (text->font == GAMGI_IO_DEJAVU_MONO)
    sprintf (string, "font=\"mono\"");
  else if (text->font == GAMGI_IO_DEJAVU_MONO_BOLD)
    sprintf (string, "font=\"mono_bold\"");
  else if (text->font == GAMGI_IO_DEJAVU_MONO_ITALIC)
    sprintf (string, "font=\"mono_italic\"");
  else if (text->font == GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC)
    sprintf (string, "font=\"mono_bold_italic\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (text->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (text->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (text->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, text->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, text->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, text->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (text->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (text->red - GAMGI_MESA_TEXT_R) > GAMGI_MATH_TOLERANCE
|| fabs (text->green - GAMGI_MESA_TEXT_G) > GAMGI_MATH_TOLERANCE
|| fabs (text->blue - GAMGI_MESA_TEXT_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, text->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, text->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, text->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (text->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, text->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_orbital (gamgi_orbital *orbital, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
char token[GAMGI_ENGINE_TOKEN];

double rotate[9], euler[3];
double radius;
int sampling, i;

/*******************
 * name (optional) *
 *******************/

if (strcmp (orbital->object.name, "Orbital") != 0)
  {
  sprintf (string, "name=\"%s\"", orbital->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_ORBITAL == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/****************************
 * n,l,m numbers (required) *
 ****************************/

sprintf (string, "n=\"%d\"", orbital->n);
gamgi_expat_export_attribute (fp, string, depth, column, error);
sprintf (string, "l=\"%d\"", orbital->l);
gamgi_expat_export_attribute (fp, string, depth, column, error);
sprintf (string, "m=\"%d\"", orbital->m);
gamgi_expat_export_attribute (fp, string, depth, column, error);

/*********************
 * charge (optional) *
 *********************/

if (fabs (orbital->charge - GAMGI_CHEM_ORBITAL_CHARGE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "charge=\"%.*f\"", gamgi->gamgi->charge, orbital->charge);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/****************************************************
 * style density, sampling, radius, seed (optional) *
 ****************************************************/

if (orbital->style != GAMGI_MESA_ORBITAL_STYLE)
  {
  if (orbital->style == GAMGI_MESA_WIRED)
    sprintf (string, "style=\"wired\"");
  if (orbital->style == GAMGI_MESA_SOLID)
    sprintf (string, "style=\"solid\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (orbital->density - GAMGI_CHEM_ORBITAL_DENSITY) > GAMGI_MATH_TOLERANCE_ORBITAL)
  {
  sprintf (string, "density=\"%.*E\"", 1, orbital->density);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

sampling = gamgi_chem_orbital_sampling (orbital->n, orbital->style);
if (orbital->sampling != sampling)
  {
  sprintf (string, "sampling=\"%d\"", orbital->sampling);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

radius = gamgi_chem_orbital_radius (orbital->n, orbital->l, orbital->m, orbital->charge);
if (fabs (orbital->radius - radius) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "radius=\"%.*f\"", gamgi->gamgi->length, orbital->radius);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (orbital->seed != GAMGI_MATH_SEED)
  {
  sprintf (string, "seed=\"%d\"", orbital->seed);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/******************************************
 * phase, frame, octants, axes (optional) *
 ******************************************/

if (orbital->phase == FALSE)
  gamgi_expat_export_attribute (fp, "phase=\"no\"", depth, column, error);

if (orbital->frame == FALSE)
  gamgi_expat_export_attribute (fp, "frame=\"no\"", depth, column, error);

token[8] = '\0';
for (i = 0; i < 8; i++) token[i] = (orbital->octants[i] == 0) ? '0' : '1';
if (strcmp (token, "11111111") != 0)
  {
  sprintf (string, "octants=\"%s\"", token);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (orbital->axes == GAMGI_CHEM_RADIUS)
  gamgi_expat_export_attribute (fp, "axes=\"radius\"", depth, column, error);
else if (orbital->axes == GAMGI_CHEM_UNIT)
  gamgi_expat_export_attribute (fp, "axes=\"unit\"", depth, column, error);
else if (orbital->axes == GAMGI_CHEM_BOHR)
  gamgi_expat_export_attribute (fp, "axes=\"bohr\"", depth, column, error);

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (orbital->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (orbital->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (orbital->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, orbital->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, orbital->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, orbital->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (orbital->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*******************************************
 * red,green,blue base (optional, coupled) *
 *******************************************/

if (fabs (orbital->base_r - GAMGI_MESA_ORBITAL_BASE_R) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->base_g - GAMGI_MESA_ORBITAL_BASE_G) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->base_b - GAMGI_MESA_ORBITAL_BASE_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "base_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->base_r);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "base_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->base_g);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "base_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->base_b);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************************************
 * red,green,blue phase (optional, coupled) *
 ********************************************/

if (fabs (orbital->phase_r - GAMGI_MESA_ORBITAL_PHASE_R) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->phase_g - GAMGI_MESA_ORBITAL_PHASE_G) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->phase_b - GAMGI_MESA_ORBITAL_PHASE_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "phase_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_r);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "phase_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_g);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "phase_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->phase_b);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************************************
 * red,green,blue frame (optional, coupled) *
 ********************************************/
 
if (fabs (orbital->frame_r - GAMGI_MESA_ORBITAL_FRAME_R) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->frame_g - GAMGI_MESA_ORBITAL_FRAME_G) > GAMGI_MATH_TOLERANCE
 || fabs (orbital->frame_b - GAMGI_MESA_ORBITAL_FRAME_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "frame_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_r);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "frame_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_g);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "frame_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, orbital->frame_b);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (orbital->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, orbital->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_bond (gamgi_bond *bond, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_BOND == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*******************
 * name (optional) *
 *******************/

gamgi_chem_bond_name (bond, string);

if (strcmp (bond->object.name, string) != 0)
  {
  sprintf (string, "name=\"%s\"", bond->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/***************************************
 * ids for parent1, parent2 (required) *
 *                                     *
 *   unused bond->object.object and    *
 *   bond->object.dlist addresses      *
 *     are used to save atom ids       *
 ***************************************/

sprintf (string, "parent1=\"%d\"", GAMGI_POINTER_TO_INT (bond->object.object));
gamgi_expat_export_attribute (fp, string, depth, column, error);

sprintf (string, "parent2=\"%d\"", GAMGI_POINTER_TO_INT (bond->object.dlist));
gamgi_expat_export_attribute (fp, string, depth, column, error);

/*******************************
 * covalency, order (optional) *
 *******************************/

if (bond->covalency + GAMGI_MATH_TOLERANCE >= 0)
  {
  sprintf (string, "covalency=\"%.*f\"", GAMGI_MATH_DECIMAL_COVALENCY, bond->covalency);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (bond->order - 1.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "order=\"%.*f\"", GAMGI_MATH_DECIMAL_ORDER, bond->order);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * style (optional) *
 ********************/

if (bond->draw == gamgi_mesa_bond_draw_line &&
gamgi->bond->draw != gamgi_mesa_bond_draw_line)
  gamgi_expat_export_attribute (fp, "style=\"wired\"", depth, column, error);

if (bond->draw == gamgi_mesa_bond_draw_cylinder &&
gamgi->bond->draw != gamgi_mesa_bond_draw_cylinder)
  gamgi_expat_export_attribute (fp, "style=\"solid\"", depth, column, error);

/*******************
 * size (optional) *
 *******************/

if (bond->draw == gamgi_mesa_bond_draw_cylinder &&
fabs (bond->size - gamgi->bond->size) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "size=\"%.*f\"", GAMGI_MATH_DECIMAL_SIZE, bond->size);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }
 
/********************
 * color (optional) *
 ********************/

if (bond->color != gamgi->bond->color)
  {
  if (bond->color == TRUE)
     sprintf (string, "color=\"yes\"");
  else
     sprintf (string, "color=\"no\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (bond->color == TRUE)
  {
  if (fabs (bond->red - gamgi->bond->red) > GAMGI_MATH_TOLERANCE
  || fabs (bond->green - gamgi->bond->green) > GAMGI_MATH_TOLERANCE
  || fabs (bond->blue - gamgi->bond->blue) > GAMGI_MATH_TOLERANCE)
    {
    sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, bond->red);
    gamgi_expat_export_attribute (fp, string, depth, column, error);

    sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, bond->green);
    gamgi_expat_export_attribute (fp, string, depth, column, error);

    sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, bond->blue);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (bond->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, bond->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_atom (gamgi_atom *atom, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
char token[GAMGI_ENGINE_TOKEN];

/*******************************************
 * Although GAMGI is currently recognizing *
 * only elements until Rg[111], therefore  *
 * with 2 char names, the source code is   *
 * ready to accept elements with 3 char    *
 * names such as UUo[118], so after adding *
 * the end '\0' char, we need element[4].  *
 *******************************************/

/*************************************
 * id (optional, required if bonded) *
 *************************************/

if (GAMGI_EXPAT_ID_ATOM == TRUE || atom->mark > 0)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  atom->mark = 0;
  }

/*******************
 * name (optional) *
 *******************/

gamgi_chem_atom_name (atom->element, token);
if (strcmp (atom->object.name, token) != 0)
  {
  sprintf (string, "name=\"%s\"", atom->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************************************
 * element, x,y,z coordinates (required) *
 *****************************************/

sprintf (string, "element=\"%s\"", token);
gamgi_expat_export_attribute (fp, string, depth, column, error);

sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, atom->position[0]);
gamgi_expat_export_attribute (fp, string, depth, column, error);

sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, atom->position[1]);
gamgi_expat_export_attribute (fp, string, depth, column, error);

sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, atom->position[2]);
gamgi_expat_export_attribute (fp, string, depth, column, error);

/******************************************
 * label, mass, radius, charge (optional) *
 *                                        *
 * radius, mass is -1 when undefined      *
 ******************************************/

gamgi_io_token_lower (token, token+10);
if (strcmp (atom->object.label, token+10) != 0)
  {
  sprintf (string, "label=\"%s\"", atom->object.label);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (atom->mass - gamgi->atom->mass[atom->element]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "mass=\"%.*f\"", gamgi->gamgi->mass, atom->mass);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (atom->radius - gamgi->atom->radius[atom->element]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "radius=\"%.*f\"", gamgi->gamgi->length, atom->radius);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (atom->charge - GAMGI_CHEM_ATOM_CHARGE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "charge=\"%.*f\"", gamgi->gamgi->charge, atom->charge);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * temperature, occupancy (optional) *
 *************************************/

if (fabs (atom->temperature - gamgi->atom->temperature) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "temperature=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_OCCUPANCY, atom->temperature);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (atom->occupancy - 1.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "occupancy=\"%.*f\"", 
  GAMGI_MATH_DECIMAL_OCCUPANCY, atom->occupancy);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************************
 * r,g,b (optional, coupled) *
 *****************************/

if (fabs (atom->red - gamgi->atom->red[atom->element]) > GAMGI_MATH_TOLERANCE
|| fabs (atom->green - gamgi->atom->green[atom->element]) > GAMGI_MATH_TOLERANCE
|| fabs (atom->blue - gamgi->atom->blue[atom->element]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, atom->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, atom->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, atom->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * style (optional) *
 ********************/

if (atom->draw == gamgi_mesa_atom_draw_cross &&
gamgi->atom->draw != gamgi_mesa_atom_draw_cross)
  gamgi_expat_export_attribute (fp, "style=\"wired\"", depth, column, error);

if (atom->draw == gamgi_mesa_atom_draw_sphere &&
gamgi->atom->draw != gamgi_mesa_atom_draw_sphere)
  gamgi_expat_export_attribute (fp, "style=\"solid\"", depth, column, error);

/*****************************
 * variancy, size (optional) *
 *****************************/

if (atom->draw == gamgi_mesa_atom_draw_sphere && 
fabs (atom->variancy - gamgi->atom->variancy) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "variancy=\"%.*f\"", GAMGI_MATH_DECIMAL_VARIANCY, atom->variancy);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (atom->draw == gamgi_mesa_atom_draw_sphere &&
fabs (atom->size - gamgi->atom->size) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "size=\"%.*f\"", GAMGI_MATH_DECIMAL_SIZE, atom->size);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (atom->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, atom->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_direction (gamgi_direction *direction, FILE *fp,
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
double rotation[9];
double euler[3];

/*******************
 * name (optional) *
 *******************/

sprintf (string, "[%d.%d.%d]", 
direction->uvw[0], direction->uvw[1], direction->uvw[2]);

if (direction->reference != GAMGI_ENGINE_CELL ||
strcmp (direction->object.name, string) != 0)
  {
  sprintf (string, "name=\"%s\"", direction->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_DIRECTION == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/******************************
 * export direction reference *
 * (how direction is defined) *
 ******************************/

if (direction->reference != FALSE)
  {
  if (direction->reference == GAMGI_ENGINE_CELL)
    sprintf (string, "reference=\"cell\"");
  if (direction->reference == GAMGI_ENGINE_ATOM)
    sprintf (string, "reference=\"atoms\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (direction->reference == GAMGI_ENGINE_CELL)
  {
  /****************************************
   * this is a crystallographic direction *
   ****************************************/

  /******************
   * uvw (required) *
   ******************/
  
  sprintf (string, "u=\"%d\" v=\"%d\" w=\"%d\"",
  direction->uvw[0], direction->uvw[1], direction->uvw[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  /**********************
   * vectors (optional) *
   **********************/

  if (direction->vectors != GAMGI_PHYS_DIRECTION_VECTORS)
    {
    if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
      sprintf (string, "vectors=\"conventional\"");
    if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
      sprintf (string, "vectors=\"primitive\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /*****************************
   * projection net (optional) *
   *****************************/

  if (direction->net != NULL && direction->net != GAMGI_PHYS_PROJECTION_NET)
    {
    if (direction->net == gamgi_phys_projection_wulff)
      sprintf (string, "net=\"wulff\"");
    if (direction->net == gamgi_phys_projection_schmidt)
      sprintf (string, "net=\"schmidt\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /*************************************
   * line is default for 3D space,     *
   * pole is default for 2D projection *
   *************************************/

  if (direction->model == GAMGI_PHYS_TRACE)
    {
    sprintf (string, "model=\"trace\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /*******************
   * node (optional) *
   *******************/

  if (gamgi_phys_direction_zero (direction->node) == FALSE)
    {
    sprintf (string, "o1=\"%d\"", direction->node[0]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    sprintf (string, "o2=\"%d\"", direction->node[1]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    sprintf (string, "o3=\"%d\"", direction->node[2]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  if (direction->node[3] != FALSE)
    {
    if (direction->node[3] == GAMGI_PHYS_110)
      sprintf (string, "o4=\"110\"");
    if (direction->node[3] == GAMGI_PHYS_101)
      sprintf (string, "o4=\"101\"");
    if (direction->node[3] == GAMGI_PHYS_011)
      sprintf (string, "o4=\"011\"");
    if (direction->node[3] == GAMGI_PHYS_111)
      sprintf (string, "o4=\"111\"");
    if (direction->node[3] == GAMGI_PHYS_211)
      sprintf (string, "o4=\"211\"");
    if (direction->node[3] == GAMGI_PHYS_122)
      sprintf (string, "o4=\"122\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }
  }

/*************
 * Line page *
 *************/
 
/**************************************************
 * initial,final x,y,z points (required, coupled) *
 **************************************************/

if (direction->model == GAMGI_PHYS_LINE)
  {
  sprintf (string, "begin_x=\"%.*f\"", gamgi->gamgi->length, direction->points[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "begin_y=\"%.*f\"", gamgi->gamgi->length, direction->points[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "begin_z=\"%.*f\"", gamgi->gamgi->length, direction->points[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "end_x=\"%.*f\"", gamgi->gamgi->length, direction->points[3]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "end_y=\"%.*f\"", gamgi->gamgi->length, direction->points[4]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "end_z=\"%.*f\"", gamgi->gamgi->length, direction->points[5]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * Position page *
 *****************/

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (direction->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (direction->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (direction->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, direction->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, direction->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, direction->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * center: x,y,z (optional, coupled) *
 *************************************/

if (fabs (direction->center[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (direction->center[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (direction->center[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"", gamgi->gamgi->length, direction->center[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_y=\"%.*f\"", gamgi->gamgi->length, direction->center[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_z=\"%.*f\"", gamgi->gamgi->length, direction->center[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (direction->quaternion, rotation);
gamgi_math_euler_from_matrix (rotation, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************
 * View page *
 *************/

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (direction->red - gamgi->direction->red) > GAMGI_MATH_TOLERANCE
|| fabs (direction->green - gamgi->direction->green) > GAMGI_MATH_TOLERANCE
|| fabs (direction->blue - gamgi->direction->blue) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, direction->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, direction->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, direction->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (direction->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, direction->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_plane (gamgi_plane *plane, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
double rotation[9];
double euler[3];

/*******************
 * name (optional) *
 *******************/

sprintf (string, "(%d.%d.%d)",
plane->hkl[0], plane->hkl[1], plane->hkl[2]);

if (plane->reference != GAMGI_ENGINE_CELL ||
strcmp (plane->object.name, string) != 0)
  {
  sprintf (string, "name=\"%s\"", plane->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_PLANE == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************
 * export plane reference *
 * (how plane is defined) *
 **************************/

if (plane->reference != FALSE)
  {
  if (plane->reference == GAMGI_ENGINE_CELL)
    sprintf (string, "reference=\"cell\"");
  if (plane->reference == GAMGI_ENGINE_ATOM)
    sprintf (string, "reference=\"atoms\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (plane->reference == GAMGI_ENGINE_CELL)
  {
  /************************************
   * this is a crystallographic plane *
   ************************************/

  /******************
   * hkl (required) *
   ******************/

  sprintf (string, "h=\"%d\" k=\"%d\" l=\"%d\"",
  plane->hkl[0], plane->hkl[1], plane->hkl[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  /********************
   * order (optional) *
   ********************/

  if (plane->order != 1)
    {
    sprintf (string, "order=\"%d\"", plane->order);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /**********************
   * vectors (optional) *
   **********************/

  if (plane->vectors != GAMGI_PHYS_PLANE_VECTORS)
    {
    if (plane->vectors == GAMGI_PHYS_CONVENTIONAL)
      sprintf (string, "vectors=\"conventional\"");
    if (plane->vectors == GAMGI_PHYS_PRIMITIVE)
      sprintf (string, "vectors=\"primitive\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /*****************************
   * projection net (optional) *
   *****************************/

  if (plane->net != NULL && plane->net != GAMGI_PHYS_PROJECTION_NET)
    {
    if (plane->net == gamgi_phys_projection_wulff)
      sprintf (string, "net=\"wulff\"");
    if (plane->net == gamgi_phys_projection_schmidt)
      sprintf (string, "net=\"schmidt\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  /*************************************
   * polygon is default for 3D space,  *
   * pole is default for 2D projection *
   *************************************/

  if (plane->model == GAMGI_PHYS_TRACE)
    {
    sprintf (string, "model=\"trace\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  if (plane->model == GAMGI_PHYS_VECTOR)
    {
    sprintf (string, "model=\"vector\"");
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }

  }

/*****************
 * Position page *
 *****************/

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (plane->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (plane->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (plane->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, plane->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, plane->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, plane->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * center: x,y,z (optional, coupled) *
 *************************************/

if (fabs (plane->center[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (plane->center[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (plane->center[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"", gamgi->gamgi->length, plane->center[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_y=\"%.*f\"", gamgi->gamgi->length, plane->center[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_z=\"%.*f\"", gamgi->gamgi->length, plane->center[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (plane->quaternion, rotation);
gamgi_math_euler_from_matrix (rotation, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************
 * View page *
 *************/

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (plane->red - gamgi->plane->red) > GAMGI_MATH_TOLERANCE
|| fabs (plane->green - gamgi->plane->green) > GAMGI_MATH_TOLERANCE
|| fabs (plane->blue - gamgi->plane->blue) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, plane->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, plane->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, plane->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (plane->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, plane->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_group (gamgi_group *group, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
double rotate[9], euler[3];

/*******************
 * name (optional) *
 *******************/

if (strcmp (group->object.name, "Group") != 0)
  {
  sprintf (string, "name=\"%s\"", group->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_GROUP == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/******************
 * Reference page *
 ******************/

if (group->reference == GAMGI_CHEM_POLYTOPE)
  gamgi_expat_export_attribute (fp, "reference=\"polytope\"", depth, column, error);

/*****************
 * Position page *
 *****************/

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (group->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (group->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (group->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, group->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, group->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, group->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * center: x,y,z (optional, coupled) *
 *************************************/

if (fabs (group->center[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (group->center[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (group->center[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"", gamgi->gamgi->length, group->center[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_y=\"%.*f\"", gamgi->gamgi->length, group->center[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_z=\"%.*f\"", gamgi->gamgi->length, group->center[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (group->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************
 * View page *
 *************/

/********************
 * style (optional) *
 ********************/

if (group->faces == TRUE)
  gamgi_expat_export_attribute (fp, "faces=\"yes\"", depth, column, error);

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (group->red - GAMGI_MESA_GROUP_R) > GAMGI_MATH_TOLERANCE
|| fabs (group->green - GAMGI_MESA_GROUP_G) > GAMGI_MATH_TOLERANCE
|| fabs (group->blue - GAMGI_MESA_GROUP_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, group->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, group->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, group->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (group->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, group->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_molecule (gamgi_molecule *molecule, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (molecule->object.name, "Molecule") != 0)
  {
  sprintf (string, "name=\"%s\"", molecule->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_MOLECULE == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (molecule->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, molecule->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_cluster (gamgi_cluster *cluster, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
double rotate[9], euler[3];

/*******************
 * name (optional) *
 *******************/

if (strcmp (cluster->object.name, "Cluster") != 0)
  {
  sprintf (string, "name=\"%s\"", cluster->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_CLUSTER == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/******************
 * Reference page *
 ******************/

if (cluster->reference == GAMGI_CHEM_POLYTOPE)
  gamgi_expat_export_attribute (fp, "reference=\"polytope\"", depth, column, error);

/*****************
 * Position data *
 *****************/

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

if (fabs (cluster->origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (cluster->origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (cluster->origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, cluster->origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, cluster->origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, cluster->origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * center: x,y,z (optional, coupled) *
 *************************************/

if (fabs (cluster->center[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (cluster->center[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (cluster->center[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"", gamgi->gamgi->length, cluster->center[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_y=\"%.*f\"", gamgi->gamgi->length, cluster->center[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_z=\"%.*f\"", gamgi->gamgi->length, cluster->center[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_quaternion_to_matrix (cluster->quaternion, rotate);
gamgi_math_euler_from_matrix (rotate, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************
 * atoms,faces (optional) *
 **************************/

if (cluster->atoms > 0)
  {
  sprintf (string, "atoms=\"%d\"", cluster->atoms);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cluster->faces == TRUE)
  gamgi_expat_export_attribute (fp, "faces=\"yes\"", depth, column, error);

/********************
 * scale (optional) *
 ********************/

if (fabs (cluster->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, cluster->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_cell (gamgi_cell *cell, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
double euler[3], origin[3];
int o1, o2, o3, o4;

/*******************
 * name (optional) *
 *******************/

if (strcmp (cell->object.name, "Cell") != 0)
  {
  sprintf (string, "name=\"%s\"", cell->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_CELL == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/***********************
 * symmetry (required) *
 ***********************/

if (cell->group == 0)
  {
  gamgi_gtk_cell_symmetry_export (cell, system, lattice);

  sprintf (string, "system=\"%s\"", system);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "lattice=\"%s\"", lattice);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }
else
  {
  sprintf (string, "group=\"%d\"", cell->group);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*********************************
 * lattice parameters (required) *
 *********************************/

sprintf (string, "a=\"%.*f\"", gamgi->gamgi->length, cell->a);
gamgi_expat_export_attribute (fp, string, depth, column, error);

if (cell->lattice == GAMGI_PHYS_TRICLINIC_P || 
cell->lattice == GAMGI_PHYS_MONOCLINIC_P ||
cell->lattice == GAMGI_PHYS_MONOCLINIC_C ||
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_P ||
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_C ||
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_I ||
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_F)
  {
  sprintf (string, "b=\"%.*f\"", gamgi->gamgi->length, cell->b);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->lattice != GAMGI_PHYS_CUBIC_P &&
cell->lattice != GAMGI_PHYS_CUBIC_I &&
cell->lattice != GAMGI_PHYS_CUBIC_F)
  {
  sprintf (string, "c=\"%.*f\"", gamgi->gamgi->length, cell->c);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->lattice == GAMGI_PHYS_TRICLINIC_P)
  {
  sprintf (string, "ab=\"%.*f\"", gamgi->gamgi->angle, cell->ab);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->lattice == GAMGI_PHYS_TRICLINIC_P 
|| cell->lattice == GAMGI_PHYS_MONOCLINIC_P
|| cell->lattice == GAMGI_PHYS_MONOCLINIC_C)
  {
  sprintf (string, "ac=\"%.*f\"", gamgi->gamgi->angle, cell->ac);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->lattice == GAMGI_PHYS_TRICLINIC_P)
  {
  sprintf (string, "bc=\"%.*f\"", gamgi->gamgi->angle, cell->bc);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/***************************************
 * reciprocal lattice: export constant *
 ***************************************/

if (cell->reciprocal != 0.0)
  {
  sprintf (string, "reciprocal=\"%.*f\"", gamgi->gamgi->length, cell->reciprocal);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*********************
 * volume (optional) *
 *********************/

if (cell->model != gamgi->cell->model)
  {
  if (cell->model == GAMGI_PHYS_CONVENTIONAL)
    sprintf (string, "model=\"%s\"", "conventional");
  else if (cell->model == GAMGI_PHYS_PRIMITIVE)
    sprintf (string, "model=\"%s\"", "primitive");
  else if (cell->model == GAMGI_PHYS_WIGNER)
    sprintf (string, "model=\"%s\"", "wigner");
  else if (cell->model == GAMGI_PHYS_PARALLELEPIPED)
    sprintf (string, "model=\"%s\"", "parallelepiped");
  else if (cell->model == GAMGI_PHYS_SPHERE)
    sprintf (string, "model=\"%s\"", "sphere");
  else if (cell->model == GAMGI_PHYS_PROJECTION)
    sprintf (string, "model=\"%s\"", "projection");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if ((cell->model == GAMGI_PHYS_CONVENTIONAL
|| cell->model == GAMGI_PHYS_PRIMITIVE
|| cell->model == GAMGI_PHYS_WIGNER) &&
(cell->n1 != 1 || cell->n2 != 1 || cell->n3 != 1))
  {
  /********************************
   * n1,n2,n3 (optional, coupled) *
   ********************************/

  sprintf (string, "n1=\"%d\"", cell->n1);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "n2=\"%d\"", cell->n2);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "n3=\"%d\"", cell->n3);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->model == GAMGI_PHYS_PARALLELEPIPED 
|| cell->model == GAMGI_PHYS_SPHERE)
  {
  /*****************
   * v1 (required) *
   *****************/

  sprintf (string, "v1=\"%.*f\"", gamgi->gamgi->length, cell->v1);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (cell->model == GAMGI_PHYS_PARALLELEPIPED)
  {
  /********************************
   * v2,v3,v12,v13,v23 (required) *
   ********************************/

  sprintf (string, "v2=\"%.*f\"", gamgi->gamgi->length, cell->v2);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "v3=\"%.*f\"", gamgi->gamgi->length, cell->v3);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "v12=\"%.*f\"", gamgi->gamgi->angle, cell->v12);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "v13=\"%.*f\"", gamgi->gamgi->angle, cell->v13);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "v23=\"%.*f\"", gamgi->gamgi->angle, cell->v23);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * origin: x,y,z (optional, coupled) *
 *************************************/

gamgi_math_position_cell_translation_get (cell, origin);

if (fabs (origin[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (origin[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (origin[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "x=\"%.*f\"", gamgi->gamgi->length, origin[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "y=\"%.*f\"", gamgi->gamgi->length, origin[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "z=\"%.*f\"", gamgi->gamgi->length, origin[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************************
 * Euler angles: e1,e2,e3 (optional, coupled) *
 **********************************************/

gamgi_math_position_cell_rotation_get (cell, euler);

if (fabs (euler[0] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[1] - 0.0) > GAMGI_MATH_TOLERANCE
|| fabs (euler[2] - 0.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "e1=\"%.*f\"", gamgi->gamgi->angle, euler[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e2=\"%.*f\"", gamgi->gamgi->angle, euler[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "e3=\"%.*f\"", gamgi->gamgi->angle, euler[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*********************************
 * o1,o2,o3, (optional, coupled) *
 * o4, origin_vectors (optional) *
 *********************************/

if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_conventional (cell->lattice, 
  cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);
else
  { o1 = cell->o1; o2 = cell->o2; o3 = cell->o3; o4 = FALSE; }

if (o1 != 0 || o2 != 0 || o3 != 0)
  {
  sprintf (string, "o1=\"%d\"", o1);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "o2=\"%d\"", o2);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "o3=\"%d\"", o3);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (o4 != FALSE)
  {
  if (o4 == GAMGI_PHYS_110) sprintf (string, "o4=\"110\"");
  if (o4 == GAMGI_PHYS_101) sprintf (string, "o4=\"101\"");
  if (o4 == GAMGI_PHYS_011) sprintf (string, "o4=\"011\"");
  if (o4 == GAMGI_PHYS_111) sprintf (string, "o4=\"111\"");
  if (o4 == GAMGI_PHYS_211) sprintf (string, "o4=\"211\"");
  if (o4 == GAMGI_PHYS_122) sprintf (string, "o4=\"122\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*********************************************************
 * by default, vectors are conventional for conventional *
 * parallelepiped, sphere and projection cells, and      *
 * primitive for primitive and wigner-seitz cells        *
 *********************************************************/

if (o4 == FALSE && (o1 != 0 || o2 != 0 || 03 != 0))
  {
  if (cell->origin_vectors == GAMGI_PHYS_PRIMITIVE &&
  (cell->model != GAMGI_PHYS_PRIMITIVE && cell->model != GAMGI_PHYS_WIGNER))
    gamgi_expat_export_attribute (fp,
    "origin_vectors=\"primitive\"", depth, column, error);

  if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL &&
  (cell->model == GAMGI_PHYS_PRIMITIVE || cell->model == GAMGI_PHYS_WIGNER))
    gamgi_expat_export_attribute (fp,
    "origin_vectors=\"conventional\"", depth, column, error);
  }

/*********************************
 * axes, axes_vectors (optional) *
 *********************************/

if (cell->axes != gamgi->cell->axes)
  {
  if (cell->axes == TRUE)
    gamgi_expat_export_attribute (fp, "axes=\"yes\"", depth, column, error);
  else
    gamgi_expat_export_attribute (fp, "axes=\"no\"", depth, column, error);
  }

/*********************************************************
 * by default, vectors are conventional for conventional *
 * parallelepiped, sphere and projection cells, and      *
 * primitive for primitive and wigner-seitz cells        *
 *********************************************************/

if (cell->axes == TRUE)
  {
  if (cell->axes_vectors == GAMGI_PHYS_PRIMITIVE &&
  (cell->model != GAMGI_PHYS_PRIMITIVE && cell->model != GAMGI_PHYS_WIGNER))
    gamgi_expat_export_attribute (fp,
    "axes_vectors=\"primitive\"", depth, column, error);

  if (cell->axes_vectors == GAMGI_PHYS_CONVENTIONAL &&
  (cell->model == GAMGI_PHYS_PRIMITIVE || cell->model == GAMGI_PHYS_WIGNER))
    gamgi_expat_export_attribute (fp,
    "axes_vectors=\"conventional\"", depth, column, error);
  }

/**********************************
 * borders,faces,nodes (optional) *
 **********************************/

if (cell->borders != gamgi->cell->borders && cell->model != GAMGI_PHYS_PROJECTION)
  {
  if (cell->borders == GAMGI_PHYS_ALL)
    gamgi_expat_export_attribute (fp, "borders=\"all\"", depth, column, error);
  else if (cell->borders == GAMGI_PHYS_FACES)
    gamgi_expat_export_attribute (fp, "borders=\"faces\"", depth, column, error);
  else if (cell->borders == GAMGI_PHYS_EDGES)
    gamgi_expat_export_attribute (fp, "borders=\"edges\"", depth, column, error);
  else if (cell->borders == GAMGI_PHYS_NONE)
    gamgi_expat_export_attribute (fp, "borders=\"none\"", depth, column, error);
  }

if (cell->faces != gamgi->cell->faces && cell->model != GAMGI_PHYS_PROJECTION
&& cell->model != GAMGI_PHYS_PARALLELEPIPED && cell->model != GAMGI_PHYS_SPHERE)
  {
  if (cell->faces == TRUE)
    gamgi_expat_export_attribute (fp, "faces=\"yes\"", depth, column, error);
  else
    gamgi_expat_export_attribute (fp, "faces=\"no\"", depth, column, error);
  }

if (cell->nodes != gamgi->cell->nodes && cell->model != GAMGI_PHYS_PROJECTION)
  {
  if (cell->nodes == TRUE)
    gamgi_expat_export_attribute (fp, "nodes=\"yes\"", depth, column, error);
  else
    gamgi_expat_export_attribute (fp, "nodes=\"no\"", depth, column, error);
  }

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (cell->red - GAMGI_MESA_CELL_R) > GAMGI_MATH_TOLERANCE
|| fabs (cell->green - GAMGI_MESA_CELL_G) > GAMGI_MATH_TOLERANCE
|| fabs (cell->blue - GAMGI_MESA_CELL_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, cell->red);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, cell->green);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, cell->blue);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (cell->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, cell->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_arrow (gamgi_arrow *arrow, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (arrow->object.name, "Arrow") != 0)
  {
  sprintf (string, "name=\"%s\"", arrow->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_ARROW == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (arrow->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, arrow->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_shape (gamgi_shape *shape, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (shape->object.name, "Shape") != 0)
  {
  sprintf (string, "name=\"%s\"", shape->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_SHAPE == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (shape->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, shape->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_graph (gamgi_graph *graph, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (graph->object.name, "Graph") != 0)
  {
  sprintf (string, "name=\"%s\"", graph->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_GRAPH == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (graph->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, graph->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_assembly (gamgi_assembly *assembly, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (assembly->object.name, "Assembly") != 0)
  {
  sprintf (string, "name=\"%s\"", assembly->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_ASSEMBLY == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (assembly->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, assembly->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_light (gamgi_light *light, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (light->object.name, "Light") != 0)
  {
  sprintf (string, "name=\"%s\"", light->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_LIGHT == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/************************************
 * red,green,blue ambient (coupled) *
 ************************************/

if (light->ambient[0] > GAMGI_MATH_TOLERANCE
|| light->ambient[1] > GAMGI_MATH_TOLERANCE
|| light->ambient[2] > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "ambient_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->ambient[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "ambient_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->ambient[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "ambient_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->ambient[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/************************************
 * red,green,blue diffuse (coupled) *
 ************************************/

if (light->diffuse[0] > GAMGI_MATH_TOLERANCE
|| light->diffuse[1] > GAMGI_MATH_TOLERANCE
|| light->diffuse[2] > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "diffuse_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "diffuse_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "diffuse_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->diffuse[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************************
 * red,green,blue specular (coupled) *
 *************************************/

if (light->specular[0] > GAMGI_MATH_TOLERANCE
|| light->specular[1] > GAMGI_MATH_TOLERANCE
|| light->specular[2] > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "specular_r=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->specular[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "specular_g=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->specular[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "specular_b=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, light->specular[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (light->position[3] - 0.0) < GAMGI_MATH_TOLERANCE)
  {
  /**************************************************
   * direction_x,direction_y,direction_z (required) *
   **************************************************/

  /************************************************************
   * invert the vector direction, so it has the same physical *
   * meaning in directional and positional lights, always     *
   * pointing from the light source to the light destination  *
   ************************************************************/

  sprintf (string, "direction_x=\"%.*f\"", gamgi->gamgi->length, -light->position[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "direction_y=\"%.*f\"", gamgi->gamgi->length, -light->position[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "direction_z=\"%.*f\"", gamgi->gamgi->length, -light->position[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }
else
  {
  /***********************************************
   * position_x,position_y,position_z (required) *
   ***********************************************/

  sprintf (string, "position_x=\"%.*f\"", gamgi->gamgi->length, light->position[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "position_y=\"%.*f\"", gamgi->gamgi->length, light->position[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "position_z=\"%.*f\"", gamgi->gamgi->length, light->position[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  if (fabs (light->angle - 180.0) > GAMGI_MATH_TOLERANCE)
    {
    /**************************************************
     * direction_x,direction_y,direction_z (required) *
     **************************************************/

    sprintf (string, "direction_x=\"%.*f\"", gamgi->gamgi->length, light->direction[0]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    sprintf (string, "direction_y=\"%.*f\"", gamgi->gamgi->length, light->direction[1]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    sprintf (string, "direction_z=\"%.*f\"", gamgi->gamgi->length, light->direction[2]);
    gamgi_expat_export_attribute (fp, string, depth, column, error);

    /********************
     * angle (required) *
     ********************/

    sprintf (string, "angle=\"%.*f\"", gamgi->gamgi->angle, light->angle);
    gamgi_expat_export_attribute (fp, string, depth, column, error);
    }
  }

/***********************************************
 * constant,linear,quadratic,radial (optional) *
 ***********************************************/

if (fabs (light->constant - GAMGI_MESA_LIGHT_CONSTANT) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "constant=\"%.*f\"", GAMGI_MATH_DECIMAL_ATTENUATION, light->constant);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (light->linear - GAMGI_MESA_LIGHT_LINEAR) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "linear=\"%.*f\"", GAMGI_MATH_DECIMAL_ATTENUATION, light->linear);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (light->quadratic - GAMGI_MESA_LIGHT_QUADRATIC) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "quadratic=\"%.*f\"", GAMGI_MATH_DECIMAL_ATTENUATION, light->quadratic);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (light->radial - GAMGI_MESA_LIGHT_RADIAL) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "radial=\"%.*f\"", GAMGI_MATH_DECIMAL_ATTENUATION, light->radial);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_layer (gamgi_layer *layer, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (layer->object.name, "Layer") != 0)
  {
  sprintf (string, "name=\"%s\"", layer->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_LAYER == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************
 * perspective (optional) *
 **************************/

if (layer->perspective != gamgi->layer->perspective)
  {
  if (layer->perspective == TRUE)
     sprintf (string, "perspective=\"yes\"");
  else
     sprintf (string, "perspective=\"no\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/***************************
 * top,near,far (optional) *
 ***************************/

if (fabs (layer->top - gamgi->layer->top) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "top=\"%.*f\"", gamgi->gamgi->length, layer->top);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (layer->near - gamgi->layer->near) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "near=\"%.*f\"", gamgi->gamgi->length, layer->near);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (fabs (layer->far - gamgi->layer->far) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "far=\"%.*f\"", gamgi->gamgi->length, layer->far);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************************************
 * eye_x,eye_y,eye_z (optional, coupled) *
 *****************************************/

if (fabs (layer->eye[0] - gamgi->layer->eye[0]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->eye[1] - gamgi->layer->eye[1]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->eye[2] - gamgi->layer->eye[2]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "eye_x=\"%.*f\"", gamgi->gamgi->length, layer->eye[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "eye_y=\"%.*f\"", gamgi->gamgi->length, layer->eye[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "eye_z=\"%.*f\"", gamgi->gamgi->length, layer->eye[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************************************
 * center_x,center_y,center_z (optional, coupled) *
 **************************************************/

if (fabs (layer->center[0] - gamgi->layer->center[0]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->center[1] - gamgi->layer->center[1]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->center[2] - gamgi->layer->center[2]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "center_x=\"%.*f\"", gamgi->gamgi->length, layer->center[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_y=\"%.*f\"", gamgi->gamgi->length, layer->center[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "center_z=\"%.*f\"", gamgi->gamgi->length, layer->center[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************************
 * up_x,up_y,up_z (optional, coupled) *
 **************************************/

if (fabs (layer->up[0] - gamgi->layer->up[0]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->up[1] - gamgi->layer->up[1]) > GAMGI_MATH_TOLERANCE
|| fabs (layer->up[2] - gamgi->layer->up[2]) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "up_x=\"%.*f\"", gamgi->gamgi->length, layer->up[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "up_y=\"%.*f\"", gamgi->gamgi->length, layer->up[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  sprintf (string, "up_z=\"%.*f\"", gamgi->gamgi->length, layer->up[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*******************************************
 * visibility_in,visibility_out (optional) *
 *******************************************/

if (layer->visibility_in != GAMGI_GTK_ALL)
  {
  if (layer->visibility_in == GAMGI_GTK_PARTIAL)
    sprintf (string, "visibility_in=\"partial\"");
  else if (layer->visibility_in == GAMGI_GTK_NONE)
    sprintf (string, "visibility_in=\"none\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (layer->visibility_out != GAMGI_GTK_ALL)
  {
  if (layer->visibility_out == GAMGI_GTK_PARTIAL)
    sprintf (string, "visibility_out=\"partial\"");
  else if (layer->visibility_out == GAMGI_GTK_NONE)
    sprintf (string, "visibility_out=\"none\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**************************************
 * red,green,blue (optional, coupled) *
 **************************************/

if (fabs (layer->color[0] - GAMGI_MESA_LAYER_R) > GAMGI_MATH_TOLERANCE
|| fabs (layer->color[1] - GAMGI_MESA_LAYER_G) > GAMGI_MATH_TOLERANCE
|| fabs (layer->color[2] - GAMGI_MESA_LAYER_B) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "red=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, layer->color[0]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "green=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, layer->color[1]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "blue=\"%.*f\"", GAMGI_MATH_DECIMAL_COLOR, layer->color[2]);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/********************
 * scale (optional) *
 ********************/

if (fabs (layer->object.scale - GAMGI_MESA_SCALE) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "scale=\"%.*f\"", GAMGI_MATH_DECIMAL_SCALE, layer->object.scale);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

}

void gamgi_expat_export_object_window (gamgi_window *window, FILE *fp, 
int depth, int *column, int *id, gamgi_bool *error)
{
char string[GAMGI_ENGINE_STRING];

/*******************
 * name (optional) *
 *******************/

if (strcmp (window->object.name, "Window") != 0)
  {
  sprintf (string, "name=\"%s\"", window->object.name);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*****************
 * id (optional) *
 *****************/

if (GAMGI_EXPAT_ID_WINDOW == TRUE)
  {
  sprintf (string, "id=\"%d\"", (*id)++);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/*************************
 * x,y origin (optional) *
 *************************/

gtk_window_get_position (GTK_WINDOW (window->window),
&window->origin_x, &window->origin_y);

if (window->origin_x != GAMGI_GTK_WINDOW_ORIGIN_X ||
window->origin_y != GAMGI_GTK_WINDOW_ORIGIN_Y)
  {
  sprintf (string, "x=\"%d\"", window->origin_x);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "y=\"%d\"", window->origin_y);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/**********************************
 *  width,height size (optional)  *
 *                                *
 * exported windows should not    *
 * exceed 2 times the screen size *
 **********************************/

gtk_window_get_size (GTK_WINDOW (window->window), &window->width, &window->height);

if (window->width > 2 * gamgi->width) window->width = 2 * gamgi->width;
if (window->height > 2 * gamgi->height) window->height = 2 * gamgi->height;

if (window->width != GAMGI_GTK_WINDOW_WIDTH ||
window->height != GAMGI_GTK_WINDOW_HEIGHT)
  {
  sprintf (string, "width=\"%d\"", window->width);
  gamgi_expat_export_attribute (fp, string, depth, column, error);

  sprintf (string, "height=\"%d\"", window->height);
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

/******************************************
 * show top,medium,bottom,full (optional) *
 ******************************************/

if (window->top_flag != TRUE)
  {
  sprintf (string, "top=\"no\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (window->medium_flag != TRUE)
  {
  sprintf (string, "medium=\"no\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }

if (window->bottom_flag != TRUE)
  {
  sprintf (string, "bottom=\"no\"");
  gamgi_expat_export_attribute (fp, string, depth, column, error);
  }
}

