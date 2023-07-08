/*******************************************
 *
 * $GAMGI/src/io/gamgi_io_x3d.c
 *
 * Copyright (C) 2008 Tiago Farto
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_array.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_io_file.h"
#include "gamgi_io_error.h"
#include "gamgi_io_x3d_render.h"

/************* internal function ***********
 *                                         *
 *                STATIC_SCALE             *
 *                                         *
 *     open the scale node of an object    *
 *******************************************/

static void static_scale (FILE *fp, double scale, double *center, 
gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];

/*******************
 * Transform start *
 *******************/

/*********************************************************
 * move the object center to the origin, scale the       *
 * object, move the object back to its original position *
 *********************************************************/

if (fabs (scale - 1.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
  gamgi->gamgi->length, center[0], 
  gamgi->gamgi->length, center[1], 
  gamgi->gamgi->length, center[2]);
  gamgi_io_file_fprintf (fp, string, error);

  sprintf (string, "<Transform scale=\"%.*f %.*f %.*f\">\n", 
  GAMGI_MATH_DECIMAL_SCALE, scale,
  GAMGI_MATH_DECIMAL_SCALE, scale,
  GAMGI_MATH_DECIMAL_SCALE, scale);
  gamgi_io_file_fprintf (fp, string, error);

  sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
  gamgi->gamgi->length, -center[0],
  gamgi->gamgi->length, -center[1],
  gamgi->gamgi->length, -center[2]);
  gamgi_io_file_fprintf (fp, string, error);
  }
}

/************* internal function ***********
 *                                         *
 *               STATIC_UNSCALE            *
 *                                         *
 *     close the scale node of an object   *
 *******************************************/

static void static_unscale (FILE *fp, double scale, gamgi_bool *error)
{
/*****************
 * Transform end *
 *****************/

if (fabs (scale - 1.0) > GAMGI_MATH_TOLERANCE)
  gamgi_io_file_fprintf(fp, "</Transform>\n</Transform>\n</Transform>\n", error);
}

static void static_line (FILE *fp, gamgi_bool lights, float *color, 
double *start, double *end, int width, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];

/***************
 * Shape start *
 ***************/

gamgi_io_file_fprintf (fp, "<Shape>\n", error);

/************************
 * Appearance start+end *
 ************************/

gamgi_io_file_fprintf (fp, "<Appearance>\n", error);

if (lights == TRUE)
  sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
  GAMGI_MATH_DECIMAL_COLOR, color[0],
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
else
  sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
  GAMGI_MATH_DECIMAL_COLOR, color[0],
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
gamgi_io_file_fprintf (fp, string, error);

sprintf (string, "<LineProperties linewidthScaleFactor=\"%d\"/>\n", width);
gamgi_io_file_fprintf (fp, string, error);

gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

/******************
 * Line start+end *
 ******************/

gamgi_io_file_fprintf (fp, "<IndexedLineSet coordIndex=\"0 1 -1\">\n", error);
sprintf (string, "<Coordinate point=\"%.*f %.*f %.*f %.*f %.*f %.*f\"/>\n",
gamgi->gamgi->length, start[0],
gamgi->gamgi->length, start[1],
gamgi->gamgi->length, start[2],
gamgi->gamgi->length, end[0],
gamgi->gamgi->length, end[1],
gamgi->gamgi->length, end[2]);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "</IndexedLineSet>\n", error);

/*************
 * Shape end *
 *************/

gamgi_io_file_fprintf (fp, "</Shape>\n", error);
}

static void static_cylinder (FILE *fp, gamgi_bool lights, float *color,
double length, double radius, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];

/***************
 * Shape start *
 ***************/

gamgi_io_file_fprintf (fp, "<Shape>\n", error);

/************************
 * Appearance start+end *
 ************************/

gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
if (lights == TRUE)
  sprintf (string, "<Material shininess=\"%.*f\" diffuseColor=\"%.*f %.*f %.*f\"/>\n",
  GAMGI_MATH_DECIMAL_SHININESS, gamgi->light->shininess,
  GAMGI_MATH_DECIMAL_COLOR, color[0],
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
else
  sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
  GAMGI_MATH_DECIMAL_COLOR, color[0],
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
gamgi_io_file_fprintf (fp, string, error);
gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

/**********************
 * Cylinder start+end *
 **********************/

sprintf (string, "<Cylinder height=\"%.*f\" radius=\"%.*f\" top=\"false\" bottom=\"false\"/>\n",
gamgi->gamgi->length, length, gamgi->gamgi->length, radius);
gamgi_io_file_fprintf (fp, string, error);

/*************
 * Shape end *
 *************/

gamgi_io_file_fprintf (fp, "</Shape>\n", error);
}

static void static_text (FILE *fp, gamgi_text *text, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
double axis[3];
double phi;

/*************************
 * scale Transform start *
 *************************/

static_scale (fp, text->object.scale, text->center, error);

/*******************************
 * rotate+move Transform start *
 *******************************/

gamgi_math_quaternion_to_axis (text->quaternion, &phi, axis);
sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\" rotation=\"%.*f %.*f %.*f %.*f\">\n",
gamgi->gamgi->length, text->origin[0], 
gamgi->gamgi->length, text->origin[1], 
gamgi->gamgi->length, text->origin[2],
gamgi->gamgi->length, axis[0], 
gamgi->gamgi->length, axis[1], 
gamgi->gamgi->length, axis[2], 
gamgi->gamgi->angle, phi);
gamgi_io_file_fprintf (fp, string, error);

/***************
 * Shape start *
 ***************/

gamgi_io_file_fprintf (fp, "<Shape>\n", error);

/************************
 * Appearance start+end *
 ************************/

gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
if (lights == TRUE)
  sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, text->red, 
  GAMGI_MATH_DECIMAL_COLOR, text->green, 
  GAMGI_MATH_DECIMAL_COLOR, text->blue);
else
  sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, text->red, 
  GAMGI_MATH_DECIMAL_COLOR, text->green, 
  GAMGI_MATH_DECIMAL_COLOR, text->blue);
gamgi_io_file_fprintf(fp, string, error);
gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

/*************************
 * export geometric data *
 *************************/

if (text->font > GAMGI_IO_FONTS_WIRED)
  gamgi_io_x3d_render_solid (fp, text, error);
else
  gamgi_io_x3d_render_wired (fp, text, error);
	
/************************************
 * Shape, rotate+move Transform end *
 ************************************/

gamgi_io_file_fprintf (fp, "</Shape>\n", error);
gamgi_io_file_fprintf (fp, "</Transform>\n", error);

/***********************
 * child objects: text *
 ***********************/
	
for (dlist = text->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

/***********************
 * scale Transform end *
 ***********************/

static_unscale (fp, text->object.scale, error);
}

static void static_orbital (FILE *fp, gamgi_orbital *orbital, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
int *dots;
char string[GAMGI_ENGINE_LINE];
double axis[3];
double phi;
float background[4];
float color[3];

/*************************
 * scale Transform start *
 *************************/

static_scale (fp, orbital->object.scale, orbital->center, error);

/*******************************
 * rotate+move Transform start *
 *******************************/

gamgi_math_quaternion_to_axis (orbital->quaternion, &phi, axis);
sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\" rotation=\"%.*f %.*f %.*f %.*f\">\n",
gamgi->gamgi->length, orbital->origin[0],
gamgi->gamgi->length, orbital->origin[1],
gamgi->gamgi->length, orbital->origin[2],
gamgi->gamgi->length, axis[0],
gamgi->gamgi->length, axis[1],
gamgi->gamgi->length, axis[2],
gamgi->gamgi->angle, phi);
gamgi_io_file_fprintf (fp, string, error);

glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

dots = orbital->dots;
if (orbital->style == GAMGI_MESA_WIRED)
  {
  /*************************
   * export geometric data *
   *************************/

  glLineWidth (4.0);
  if (dots[1] > 0)
    { 
    color[0] = orbital->base_r;
    color[1] = orbital->base_g;
    color[2] = orbital->base_b;
    gamgi_io_x3d_render_points (fp, dots[1],
    orbital->points + 3 * dots[0], color, error);
    }

  if (dots[2] > 0)
    {
    color[0] = orbital->phase_r;
    color[1] = orbital->phase_g;
    color[2] = orbital->phase_b;
    gamgi_io_x3d_render_points (fp, dots[2],
    orbital->points + 3 * dots[0] + 3 * dots[1], color, error);
    }
  }

if (orbital->style == GAMGI_MESA_SOLID)
  {
  if (dots[1] > 0)
    {
    /***************
     * Shape start *
     ***************/

    /************************
     * Appearance start+end *
     ************************/

    gamgi_io_file_fprintf (fp, "<Shape>\n", error);
    gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
    if (lights == TRUE)
      sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_r,
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_g,
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_b);
    else
      sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_r,
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_g,
      GAMGI_MATH_DECIMAL_COLOR, orbital->base_b);
    gamgi_io_file_fprintf (fp, string, error);
    gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

    /*************************
     * export geometric data *
     *************************/

    gamgi_io_x3d_render_triangles (fp, dots[1] / 3,
    orbital->points + 3 * dots[0], error);
    gamgi_io_file_fprintf (fp, "</Shape>\n", error);

    /*************
     * Shape end *
     *************/
    }

  if (dots[2] > 0)
    {
    /***************
     * Shape start *
     ***************/

    /************************
     * Appearance start+end *
     ************************/

    gamgi_io_file_fprintf (fp, "<Shape>\n", error);
    gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
    if (lights == TRUE)
      sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_r,
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_g,
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_b);
    else
      sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_r,
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_g,
      GAMGI_MATH_DECIMAL_COLOR, orbital->phase_b);
    gamgi_io_file_fprintf (fp, string, error);
    gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

    /*************************
     * export geometric data *
     *************************/

    gamgi_io_x3d_render_triangles (fp, dots[2] / 3,
    orbital->points + 3 * dots[0] + 3 * dots[1], error);
    gamgi_io_file_fprintf (fp, "</Shape>\n", error);

    /*************
     * Shape end *
     *************/
    }
  }

/*****************************
 * rotate+move Transform end *
 *****************************/

gamgi_io_file_fprintf (fp, "</Transform>\n", error);

/***********************
 * child objects: text *
 ***********************/

for (dlist = orbital->text_start; dlist!=NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

/***********************
 * scale Transform end *
 ***********************/

static_unscale (fp, orbital->object.scale, error);
}

static void static_bond (FILE *fp, gamgi_bond *bond, gamgi_atom *atom, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
double endpoint[3];
double center[3], sub[3], axis[3];
double length, radius;
double angle = 0.0;
float background[4];
float color[3];

glGetFloatv (GL_COLOR_CLEAR_VALUE, background);
	
if (bond->atom2 == NULL) return;

/***************************************
 * when color is a bond property, this *
 * function is executed only for atom1 *
 ***************************************/

if (bond->color == TRUE && bond->atom2 == atom) return;

if (bond->color == FALSE)
  {
  color[0] = atom->red;
  color[1] = atom->green;
  color[2] = atom->blue;

  /*****************************************************
   * the point where color changes from atom1 to atom2 *
   *****************************************************/

  endpoint[0] = (bond->atom1->position[0] + bond->atom2->position[0]) / 2.0;
  endpoint[1] = (bond->atom1->position[1] + bond->atom2->position[1]) / 2.0;
  endpoint[2] = (bond->atom1->position[2] + bond->atom2->position[2]) / 2.0;
  }
else
  {		
  color[0] = bond->red;
  color[1] = bond->green;
  color[2] = bond->blue;
		
  /*********************************** 
   * this code runs for atom1 only,  *
   * so the endpoint is always atom2 *
   ***********************************/

  endpoint[0] = bond->atom2->position[0];
  endpoint[1] = bond->atom2->position[1];
  endpoint[2] = bond->atom2->position[2];
  }
	
if (bond->draw == gamgi_mesa_bond_draw_cylinder)
  {
  /******************
   * draw cylinders *
   ******************/

  radius = bond->object.scale * bond->size * gamgi->atom->min;

  center[0] = (atom->position[0] + endpoint[0]) / 2.0;
  center[1] = (atom->position[1] + endpoint[1]) / 2.0;
  center[2] = (atom->position[2] + endpoint[2]) / 2.0;

  if (atom == bond->atom1)
    gamgi_math_vector_sub (atom->position, endpoint, sub);
  else
     gamgi_math_vector_sub (endpoint, atom->position, sub);
  length = gamgi_math_vector_length (sub);
  angle = acos (sub[1] / length);
  gamgi_math_vector_absolute (axis, sub[2], 0.0, -sub[0]);
  gamgi_math_vector_normal (axis);
		
  sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\" rotation=\"%.*f %.*f %.*f %.*f\">\n", 
  gamgi->gamgi->length, center[0], 
  gamgi->gamgi->length, center[1], 
  gamgi->gamgi->length, center[2],
  gamgi->gamgi->length, axis[0], 
  gamgi->gamgi->length, axis[1], 
  gamgi->gamgi->length, axis[2], 
  gamgi->gamgi->angle, angle);
  gamgi_io_file_fprintf (fp, string, error);
		
  if (bond->order <= 1.5)
    {
    /**********************************************
     * single bond: rendered as a single cylinder *
     **********************************************/

    static_cylinder (fp, lights, color, length, radius, error);
    }
  else if (bond->order <= 2.5)
    {
    /********************************************************************
     * double bond: render a central cylinder and two lateral cylinders *
     ********************************************************************/

    static_cylinder (fp, lights, color, length, radius, error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n", 
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0, 
    gamgi->gamgi->length, -radius);
    gamgi_io_file_fprintf (fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 2.0, error);
    gamgi_io_file_fprintf(fp, "</Transform>\n", error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n", 
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0, 
    gamgi->gamgi->length, radius);
    gamgi_io_file_fprintf(fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 2.0, error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);
    }
  else
    {
    /*********************************************************************
     * triple bond: render a central cylinder and four lateral cylinders *
     *********************************************************************/

    static_cylinder (fp, lights, color, length, radius, error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
    gamgi->gamgi->length, -radius,
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0);
    gamgi_io_file_fprintf (fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 3.0, error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
    gamgi->gamgi->length, radius,
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0);
    gamgi_io_file_fprintf (fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 3.0, error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0,
    gamgi->gamgi->length, -radius);
    gamgi_io_file_fprintf (fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 3.0, error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);

    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
    gamgi->gamgi->length, 0.0, gamgi->gamgi->length, 0.0,
    gamgi->gamgi->length, radius);
    gamgi_io_file_fprintf (fp, string, error);
    static_cylinder (fp, lights, color, length, radius / 3.0, error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);
    }
  gamgi_io_file_fprintf(fp, "</Transform>\n", error);
  }
else
  {
  /**************
   * draw lines *
   **************/

  if (bond->order <= 1.5)
    {
    /******************************************************************
     * bonds rendered as a single line: render the whole bond segment *
     ******************************************************************/

    static_line (fp, lights, color, atom->position, endpoint, 2, error);
    }
  else if (bond->order <= 2.5)
    {
    /*********************************************************************
     * bonds rendered as a double line: 1) render the whole bond segment *
     * with bond color; 2) render a inner segment with background color  *
     *********************************************************************/

    static_line (fp, lights, background, atom->position, endpoint, 2, error);
    static_line (fp, lights, color, atom->position, endpoint, 6, error);
    }
  else
    {
    /***************************************************************
     * bonds rendered as a triple line: 1) render the whole bond   *
     * segment with bond color; 2) render inner segment with       *
     * background color; 3) render central segment with bond color *
     ***************************************************************/

    static_line (fp, lights, color, atom->position, endpoint, 2, error);
    static_line (fp, lights, background, atom->position, endpoint, 6, error);
    static_line (fp, lights, color, atom->position, endpoint, 10, error);
    }
  }

/**************************************************
 * when color is inherited from the parent atoms, *
 * this function is run for each atom, but the    *
 * section below is executed only for atom 1      *
 **************************************************/

if (bond->color == FALSE && atom == bond->atom2) return;

/********************************
 * child objects: orbital, text *
 ********************************/

center[0] = (bond->atom1->position[0] + bond->atom2->position[0]) / 2.0;
center[1] = (bond->atom1->position[1] + bond->atom2->position[1]) / 2.0;
center[2] = (bond->atom1->position[2] + bond->atom2->position[2]) / 2.0;

static_scale (fp, bond->object.scale, center, error);

for (dlist = bond->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);
	
for (dlist = bond->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

static_unscale (fp, bond->object.scale, error);
}

static void static_atom (FILE *fp, gamgi_atom *atom, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
double radius;
float size;
	
static_scale (fp, atom->object.scale, atom->position, error);
	
if (atom->draw == gamgi_mesa_atom_draw_sphere || atom->bond_start == NULL)
  {	
  sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n", 
  gamgi->gamgi->length, atom->position[0],
  gamgi->gamgi->length, atom->position[1],
  gamgi->gamgi->length, atom->position[2]);
  gamgi_io_file_fprintf(fp, string, error);
			
  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);
  if (lights == TRUE)
    sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, atom->red,
    GAMGI_MATH_DECIMAL_COLOR, atom->green,
    GAMGI_MATH_DECIMAL_COLOR, atom->blue);
  else
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, atom->red,
    GAMGI_MATH_DECIMAL_COLOR, atom->green,
    GAMGI_MATH_DECIMAL_COLOR, atom->blue);
  gamgi_io_file_fprintf (fp, string, error);

  if (atom->draw != gamgi_mesa_atom_draw_sphere)
    {
    sprintf (string, "<LineProperties linewidthScaleFactor=\"%d\"/>\n",
    gamgi->atom->width);
    gamgi_io_file_fprintf (fp, string, error);
    }
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

  if (atom->draw == gamgi_mesa_atom_draw_sphere)
    {
    radius = atom->size * (atom->variancy * atom->radius +
    (1 - atom->variancy) * gamgi->atom->min);
    sprintf (string, "<Sphere radius=\"%.*f\"/>\n", 
    gamgi->gamgi->length, radius);
    gamgi_io_file_fprintf (fp, string, error);
    }

  if (atom->draw == gamgi_mesa_atom_draw_cross)
    {
    size = 0.5 * gamgi->atom->size * gamgi->atom->min;
    gamgi_io_file_fprintf (fp, "<IndexedLineSet coordIndex=\""
    "0 1 -1 2 3 -1 4 5 -1\">\n", error);
    sprintf (string, "<Coordinate point=\""
    "%.*f 0 0 %.*f 0 0 0 %.*f 0 0 %.*f 0 0 0 %.*f 0 0 %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_SIZE, -size, GAMGI_MATH_DECIMAL_SIZE, size, 
    GAMGI_MATH_DECIMAL_SIZE, -size, GAMGI_MATH_DECIMAL_SIZE, size, 
    GAMGI_MATH_DECIMAL_SIZE, -size, GAMGI_MATH_DECIMAL_SIZE, size);
    gamgi_io_file_fprintf(fp, string, error);
    gamgi_io_file_fprintf(fp, "</IndexedLineSet>\n", error);
    }
			
  gamgi_io_file_fprintf(fp, "</Shape>\n</Transform>\n", error);	
  }
	
/**************************************
 * child objects: bond, orbital, text *
 **************************************/

for (dlist = atom->bond_start; dlist != NULL; dlist = dlist->next)
  static_bond (fp, GAMGI_CAST_BOND dlist->data, atom, lights, error);

for (dlist = atom->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);
	
for (dlist = atom->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
	
static_unscale (fp, atom->object.scale, error);
}

static void static_direction (FILE *fp, gamgi_direction *direction, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
char string[GAMGI_ENGINE_LINE];
double axis[3];
double phi;
float color[3];

static_scale (fp, direction->object.scale, direction->center, error);
	
if (direction->loops[0] != 0)
  {
  sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n",
  gamgi->gamgi->length, direction->origin[0], 
  gamgi->gamgi->length, direction->origin[1],
  gamgi->gamgi->length, direction->origin[2]);
  gamgi_io_file_fprintf (fp, string, error);
		
  if (direction->model == GAMGI_PHYS_LINE)
    {
    /******************************
     * render direction as a line *
     ******************************/

    gamgi_math_quaternion_to_axis (direction->quaternion, &phi, axis);
    sprintf (string, "<Transform rotation=\"%.*f %.*f %.*f %.*f\">\n", 
    gamgi->gamgi->length, axis[0],
    gamgi->gamgi->length, axis[1],
    gamgi->gamgi->length, axis[2],
    gamgi->gamgi->angle, phi);
    gamgi_io_file_fprintf(fp, string, error);
			
    gamgi_io_file_fprintf (fp, "<Shape>\n", error);

    gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, direction->red,
    GAMGI_MATH_DECIMAL_COLOR, direction->green,
    GAMGI_MATH_DECIMAL_COLOR, direction->blue);
    gamgi_io_file_fprintf (fp, string, error);
    sprintf (string, "<LineProperties linewidthScaleFactor=\"%.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_SCALE, GAMGI_IO_X3D_DIRECTION_WIDTH);
    gamgi_io_file_fprintf (fp, string, error);
    gamgi_io_file_fprintf (fp, "</Appearance>\n", error);
			
    gamgi_io_x3d_render_lines (fp, direction->loops, direction->points, error);

    gamgi_io_file_fprintf (fp, "</Shape>\n", error);
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);
    }
  else if (direction->model == GAMGI_PHYS_POLE)
    {
    /*****************************************************************
     * stereographic projection: render direction as a chess bitmask *
     *****************************************************************/

    color[0] = direction->red;
    color[1] = direction->green;
    color[2] = direction->blue;
    gamgi_io_x3d_render_chess (fp, direction->loops[0], direction->points, color, error);
    }
  else if (direction->model == GAMGI_PHYS_TRACE)
    {
    /*********************************************************
     * stereographic projection: render direction as an arch *
     *********************************************************/

    gamgi_io_file_fprintf (fp, "<Shape>\n", error);

    gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
    if (lights)
      sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n", 
      GAMGI_MATH_DECIMAL_COLOR, direction->red,
      GAMGI_MATH_DECIMAL_COLOR, direction->green,
      GAMGI_MATH_DECIMAL_COLOR, direction->blue);
    else
      sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
      GAMGI_MATH_DECIMAL_COLOR, direction->red,
      GAMGI_MATH_DECIMAL_COLOR, direction->green,
      GAMGI_MATH_DECIMAL_COLOR, direction->blue);
    gamgi_io_file_fprintf (fp, string, error);
    gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

    gamgi_io_x3d_render_line_strip (fp, direction->loops[0], direction->points, error);
    gamgi_io_file_fprintf (fp, "</Shape>\n", error);
    }
		
  gamgi_io_file_fprintf (fp, "</Transform>\n", error);
  }
	
/**************************************
 * child objects: atom, orbital, text *
 **************************************/

for (dlist = direction->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = direction->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = direction->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
	
if (direction->loops[0] != 0)
  static_unscale (fp, direction->object.scale, error);
}

static void static_plane (FILE *fp, gamgi_plane *plane, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
double axis[3];
double phi;
float color[3];

static_scale (fp, plane->object.scale, plane->center, error);

if (plane->loops[0] != 0)
  {
  if (plane->model == GAMGI_PHYS_VECTOR)
    {
    /*******************************************************
     * reciprocal lattice: render plane as a cross bitmask *
     *******************************************************/

    color[0] = plane->red;
    color[1] = plane->green;
    color[2] = plane->blue;
    gamgi_io_x3d_render_cross (fp, plane->loops[0], plane->points, color, error);
    }
  else
    {
    sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n", 
    gamgi->gamgi->length, plane->origin[0],
    gamgi->gamgi->length, plane->origin[1],
    gamgi->gamgi->length, plane->origin[2]);
    gamgi_io_file_fprintf (fp, string, error);
			
    if (plane->model == GAMGI_PHYS_POLYGON)
      {
      /*****************************
       * render plane as a polygon *
       *****************************/

      gamgi_math_quaternion_to_axis (plane->quaternion, &phi, axis);
      sprintf (string, "<Transform rotation=\"%.*f %.*f %.*f %.*f\">\n", 
      gamgi->gamgi->length, axis[0],
      gamgi->gamgi->length, axis[1],
      gamgi->gamgi->length, axis[2],
      gamgi->gamgi->angle, phi);
      gamgi_io_file_fprintf (fp, string, error);
				
      gamgi_io_file_fprintf (fp, "<Shape>\n", error);

      gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
      if (lights == TRUE)
        sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
        GAMGI_MATH_DECIMAL_COLOR, plane->red,
        GAMGI_MATH_DECIMAL_COLOR, plane->green,
        GAMGI_MATH_DECIMAL_COLOR, plane->blue);
      else
        sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
        GAMGI_MATH_DECIMAL_COLOR, plane->red,
        GAMGI_MATH_DECIMAL_COLOR, plane->green,
        GAMGI_MATH_DECIMAL_COLOR, plane->blue);
      gamgi_io_file_fprintf (fp, string, error);
      gamgi_io_file_fprintf(fp, "</Appearance>\n", error);
				
      gamgi_io_x3d_render_polygons (fp, plane->loops, plane->points, error);
				
      gamgi_io_file_fprintf(fp, "</Shape>\n", error);
      gamgi_io_file_fprintf(fp, "</Transform>\n", error);
      }
    else if (plane->model == GAMGI_PHYS_POLE)
      {
      /*************************************************************
       * stereographic projection: render plane as a cross bitmask *
       *************************************************************/

      color[0] = plane->red;
      color[1] = plane->green;
      color[2] = plane->blue;
      gamgi_io_x3d_render_cross (fp, plane->loops[0], plane->points, color, error);
      }
    else if (plane->model == GAMGI_PHYS_TRACE)
      {
      /*****************************************************
       * stereographic projection: render plane as an arch *
       *****************************************************/

      gamgi_io_file_fprintf(fp, "<Shape>\n", error);

      gamgi_io_file_fprintf(fp, "<Appearance>\n", error);
      if (lights == TRUE)
        sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n", 
        GAMGI_MATH_DECIMAL_COLOR, plane->red,
        GAMGI_MATH_DECIMAL_COLOR, plane->green,
        GAMGI_MATH_DECIMAL_COLOR, plane->blue);
      else
        sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
        GAMGI_MATH_DECIMAL_COLOR, plane->red,
        GAMGI_MATH_DECIMAL_COLOR, plane->green,
        GAMGI_MATH_DECIMAL_COLOR, plane->blue);
      gamgi_io_file_fprintf (fp, string, error);
      gamgi_io_file_fprintf (fp, "</Appearance>\n", error);
				
      gamgi_io_x3d_render_line_strip (fp, plane->loops[0], plane->points, error);
      gamgi_io_file_fprintf (fp, "</Shape>\n", error);
      }
			
    gamgi_io_file_fprintf (fp, "</Transform>\n", error);
    }
  }
	
/*************************************************
 * child objects: direction, atom, orbital, text *
 *************************************************/
	
for (dlist = plane->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = plane->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = plane->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = plane->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

if (plane->loops[0] != 0)
  static_unscale (fp, plane->object.scale, error);
}

static void static_group (FILE *fp, gamgi_group *group, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
char string[GAMGI_ENGINE_LINE];
double axis[4];
double phi;
float background[4];

glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

static_scale (fp, group->object.scale, group->center, error);

gamgi_math_quaternion_to_axis (group->quaternion, &phi, axis);

if (group->loops != NULL)
  {
  sprintf (string, 
  "<Transform translation=\"%.*f %.*f %.*f\" rotation=\"%.*f %.*f %.*f %.*f\">\n",
  gamgi->gamgi->length, group->origin[0],
  gamgi->gamgi->length, group->origin[1],
  gamgi->gamgi->length, group->origin[2],
  gamgi->gamgi->length, axis[0],
  gamgi->gamgi->length, axis[1],
  gamgi->gamgi->length, axis[2],
  gamgi->gamgi->angle, phi);
  gamgi_io_file_fprintf (fp, string, error);

  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);
  sprintf (string, "<LineProperties linewidthScaleFactor=\"%.*f\"/>",
  GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_POLYHEDRON_WIDTH);
  gamgi_io_file_fprintf (fp, string, error);

  if (group->faces == FALSE)
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, group->red,
    GAMGI_MATH_DECIMAL_COLOR, group->green,
    GAMGI_MATH_DECIMAL_COLOR, group->blue);
  else if (lights == TRUE)
    sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, background[0],
    GAMGI_MATH_DECIMAL_COLOR, background[1],
    GAMGI_MATH_DECIMAL_COLOR, background[2]);
  else
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
    GAMGI_MATH_DECIMAL_COLOR, background[0],
    GAMGI_MATH_DECIMAL_COLOR, background[1],
    GAMGI_MATH_DECIMAL_COLOR, background[2]);
  gamgi_io_file_fprintf (fp, string, error);
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

  gamgi_io_x3d_render_loops (fp, group->loops, group->points, error);

  if (group->faces == TRUE)
    {
    gamgi_io_file_fprintf (fp, "</Shape>\n<Shape>\n", error);

    gamgi_io_file_fprintf (fp, "<Appearance>\n", error);
    if (lights == TRUE)
      sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, group->red,
      GAMGI_MATH_DECIMAL_COLOR, group->green,
      GAMGI_MATH_DECIMAL_COLOR, group->blue);
    else
      sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n",
      GAMGI_MATH_DECIMAL_COLOR, group->red,
      GAMGI_MATH_DECIMAL_COLOR, group->green,
      GAMGI_MATH_DECIMAL_COLOR, group->blue);
    gamgi_io_file_fprintf (fp, string, error);
    gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

    gamgi_io_x3d_render_polygons (fp, group->loops, group->points, error);
    }

  gamgi_io_file_fprintf (fp, "</Shape>\n", error);
  gamgi_io_file_fprintf(fp, "</Transform>\n", error);
  }

/**********************************
 * child objects: group, plane,   *
 * direction, atom, orbital, text * 
 **********************************/

for (dlist = group->group_start; dlist!=NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);

for (dlist = group->plane_start; dlist!=NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);

for (dlist = group->direction_start; dlist!=NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = group->atom_start; dlist!=NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = group->orbital_start; dlist!=NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = group->text_start; dlist!=NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

static_unscale (fp, group->object.scale, error);
}

static void static_molecule (FILE *fp, gamgi_molecule *molecule, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist *dlist;
double center[3];
	
gamgi_math_vector_zero (center);
static_scale (fp, molecule->object.scale, center, error);

/**********************************
 * child objects: group, plane,   *
 * direction, atom, orbital, text * 
 **********************************/
	
for (dlist = molecule->group_start; dlist != NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);
	
for (dlist = molecule->plane_start; dlist != NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);
	
for (dlist = molecule->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = molecule->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);
	
for (dlist = molecule->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = molecule->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
	
static_unscale (fp, molecule->object.scale, error);
}

static void static_cluster (FILE *fp, gamgi_cluster *cluster, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
char string[GAMGI_ENGINE_LINE];
double axis[4];
double phi;
float background[4];
	
glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

static_scale (fp, cluster->object.scale, cluster->center, error);

gamgi_math_quaternion_to_axis (cluster->quaternion, &phi, axis);

if (cluster->loops != NULL)
  {
  sprintf (string, 
  "<Transform translation=\"%.*f %.*f %.*f\" rotation=\"%.*f %.*f %.*f %.*f\">\n",
  gamgi->gamgi->length, cluster->origin[0], 
  gamgi->gamgi->length, cluster->origin[1], 
  gamgi->gamgi->length, cluster->origin[2],
  gamgi->gamgi->length, axis[0], 
  gamgi->gamgi->length, axis[1],
  gamgi->gamgi->length, axis[2], 
  gamgi->gamgi->angle, phi);
  gamgi_io_file_fprintf (fp, string, error);

  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);
  sprintf (string, "<LineProperties linewidthScaleFactor=\"%.*f\"/>", 
  GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_POLYHEDRON_WIDTH);
  gamgi_io_file_fprintf (fp, string, error);

  if (cluster->faces == FALSE)
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_COLOR, cluster->colors[0],
    GAMGI_MATH_DECIMAL_COLOR, cluster->colors[1],
    GAMGI_MATH_DECIMAL_COLOR, cluster->colors[2]);
  else if (lights == TRUE)
    sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_COLOR, background[0],
    GAMGI_MATH_DECIMAL_COLOR, background[1],
    GAMGI_MATH_DECIMAL_COLOR, background[2]);
  else
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_COLOR, background[0],
    GAMGI_MATH_DECIMAL_COLOR, background[1],
    GAMGI_MATH_DECIMAL_COLOR, background[2]);
  gamgi_io_file_fprintf (fp, string, error);
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

  gamgi_io_x3d_render_loops (fp, cluster->loops, cluster->points, error);

  if (cluster->faces == TRUE)
    {
    gamgi_io_file_fprintf (fp, "</Shape>\n<Shape>\n", error);
    gamgi_io_x3d_render_polygons_color (fp, cluster->loops, 
    cluster->points, cluster->paints, cluster->colors, error);
    }

  gamgi_io_file_fprintf (fp, "</Shape>\n", error);

  if (cluster->atoms > 0)
    gamgi_io_x3d_render_points (fp, cluster->atoms, 
    cluster->points, cluster->colors, error);

  gamgi_io_file_fprintf(fp, "</Transform>\n", error);
  }

/********************************************
 * child objects: cluster, molecule, group, *
 * plane, direction, atom, orbital, text    *
 ********************************************/

for (dlist = cluster->cluster_start; dlist!=NULL; dlist = dlist->next)
  static_cluster (fp, GAMGI_CAST_CLUSTER dlist->data, lights, error);

for (dlist = cluster->molecule_start; dlist!=NULL; dlist = dlist->next)
  static_molecule (fp, GAMGI_CAST_MOLECULE dlist->data, lights, error);

for (dlist = cluster->group_start; dlist!=NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);

for (dlist = cluster->plane_start; dlist!=NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);

for (dlist = cluster->direction_start; dlist!=NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = cluster->atom_start; dlist!=NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = cluster->orbital_start; dlist!=NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = cluster->text_start; dlist!=NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

static_unscale (fp, cluster->object.scale, error);
}

static void static_cell (FILE *fp, gamgi_cell *cell, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
char string[GAMGI_ENGINE_LINE];
double axis[3];
double phi;
float background[4];
float color[3];

/****************************
 * scale, translate, rotate *
 ****************************/

static_scale (fp, cell->object.scale, cell->center, error);

sprintf (string, "<Transform translation=\"%.*f %.*f %.*f\">\n", 
gamgi->gamgi->length, cell->origin[0], 
gamgi->gamgi->length, cell->origin[1], 
gamgi->gamgi->length, cell->origin[2]);
gamgi_io_file_fprintf (fp, string, error);
	
if (cell->model != GAMGI_PHYS_PROJECTION)
  {
  gamgi_math_quaternion_to_axis (cell->quaternion, &phi, axis);
  sprintf (string, "<Transform rotation=\"%.*f %.*f %.*f %.*f\">\n", 
  gamgi->gamgi->length, axis[0],
  gamgi->gamgi->length, axis[1],
  gamgi->gamgi->length, axis[2],
  gamgi->gamgi->angle, phi);
  gamgi_io_file_fprintf (fp, string, error);
  }
	
glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

/*********
 * Shape *
 *********/

if (cell->loops != NULL && cell->borders != FALSE && 
(cell->lattice != GAMGI_PHYS_HEXAGONAL_P || 
(cell->model != GAMGI_PHYS_WIGNER && cell->model != GAMGI_PHYS_CONVENTIONAL)) &&
(cell->lattice != GAMGI_PHYS_ORTHORHOMBIC_C || cell->model != GAMGI_PHYS_WIGNER))
  {
  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);

  if (cell->faces == TRUE)
    {
    color[0] = background[0];
    color[1] = background[1];
    color[2] = background[2];
    }
  else
    {
    color[0] = cell->red;
    color[1] = cell->green;
    color[2] = cell->blue;
    }

  sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, color[0], 
  GAMGI_MATH_DECIMAL_COLOR, color[1], 
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
  gamgi_io_file_fprintf (fp, string, error);
		
  sprintf (string, "<LineProperties linewidthScaleFactor=\"%.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_CELL_LINE);
  gamgi_io_file_fprintf (fp, string, error);
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);
		
  gamgi_io_x3d_render_loops (fp, cell->loops, cell->points, error);
  gamgi_io_file_fprintf (fp, "</Shape>\n", error);
  }
	
if (cell->lines != NULL)
  {
  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);

  if (cell->faces == TRUE)
    {
    color[0] = background[0];
    color[1] = background[1];
    color[2] = background[2];
    }
  else
    {
    color[0] = cell->red;
    color[1] = cell->green;
    color[2] = cell->blue;
    }

  sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, color[0], 
  GAMGI_MATH_DECIMAL_COLOR, color[1], 
  GAMGI_MATH_DECIMAL_COLOR, color[2]);
  gamgi_io_file_fprintf (fp, string, error);

  sprintf (string, "<LineProperties linewidthScaleFactor=\"%.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_SCALE, GAMGI_MESA_CELL_LINE);
  gamgi_io_file_fprintf (fp, string, error);
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

  gamgi_io_x3d_render_lines (fp, cell->lines, cell->points, error);
  gamgi_io_file_fprintf (fp, "</Shape>\n", error);
  }
	
if (cell->faces == TRUE)
  {
  gamgi_io_file_fprintf (fp, "<Shape>\n<Appearance>\n", error);

  if (lights == TRUE)
    sprintf (string, "<Material diffuseColor=\"%.*f %.*f %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_COLOR, cell->red, 
    GAMGI_MATH_DECIMAL_COLOR, cell->green, 
    GAMGI_MATH_DECIMAL_COLOR, cell->blue);
  else
    sprintf (string, "<Material emissiveColor=\"%.*f %.*f %.*f\"/>\n", 
    GAMGI_MATH_DECIMAL_COLOR, cell->red, 
    GAMGI_MATH_DECIMAL_COLOR, cell->green, 
    GAMGI_MATH_DECIMAL_COLOR, cell->blue);
  gamgi_io_file_fprintf (fp, string, error);
  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);

  gamgi_io_x3d_render_polygons (fp, cell->loops, cell->points, error);
  gamgi_io_file_fprintf(fp, "</Shape>\n", error);
  }

if (cell->nodes == TRUE)
  gamgi_io_x3d_render_points (fp, cell->n_nodes, cell->points, color,error);

if (cell->model != GAMGI_PHYS_PROJECTION)
  gamgi_io_file_fprintf (fp, "</Transform>\n", error);
	
gamgi_io_file_fprintf (fp, "</Transform>\n", error);
	
/********************************************
 * child objects: cluster, molecule, group, *
 * plane, direction, atom, orbital, text    * 
 ********************************************/
	
for (dlist = cell->cluster_start; dlist != NULL; dlist = dlist->next)
  static_cluster (fp, GAMGI_CAST_CLUSTER dlist->data, lights, error);

for (dlist = cell->molecule_start; dlist != NULL; dlist = dlist->next)
  static_molecule (fp, GAMGI_CAST_MOLECULE dlist->data, lights, error);

for (dlist = cell->group_start; dlist != NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);

for (dlist = cell->plane_start; dlist != NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);

for (dlist = cell->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = cell->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = cell->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = cell->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

static_unscale (fp, cell->object.scale, error);
}

static void static_arrow (FILE *fp, gamgi_arrow *arrow, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;

/***********************
 * child objects: text *
 ***********************/

for (dlist = arrow->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
}

static void static_shape (FILE * fp, gamgi_shape *shape, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;

/***********************
 * child objects: text *
 ***********************/

for (dlist = shape->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
}

static void static_graph (FILE *fp, gamgi_graph *graph, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
	
/***********************
 * child objects: text * 
 ***********************/
 
for (dlist = graph->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);
}

static void static_assembly (FILE *fp, gamgi_assembly *assembly, 
gamgi_bool lights, gamgi_bool *error)
{
gamgi_dlist * dlist;
double center[3];
	
gamgi_math_vector_zero (center);
static_scale (fp, assembly->object.scale, center, error);
	
/******************************************
 * child objects: assembly, graph, arrow, *
 * shape, cell, cluster, molecule, group, *
 * plane, direction, atom, orbital, text  *
 ******************************************/
	
for (dlist = assembly->assembly_start; dlist != NULL; dlist = dlist->next)
  static_assembly (fp, GAMGI_CAST_ASSEMBLY dlist->data, lights, error);

for (dlist = assembly->graph_start; dlist != NULL; dlist = dlist->next)
  static_graph (fp, GAMGI_CAST_GRAPH dlist->data, lights, error);

for (dlist = assembly->arrow_start; dlist != NULL; dlist = dlist->next)
  static_arrow (fp, GAMGI_CAST_ARROW dlist->data, lights, error);

for (dlist = assembly->shape_start; dlist != NULL; dlist = dlist->next)
  static_shape (fp, GAMGI_CAST_SHAPE dlist->data, lights, error);

for (dlist = assembly->cell_start; dlist != NULL; dlist = dlist->next)
  static_cell (fp, GAMGI_CAST_CELL dlist->data, lights, error);

for (dlist = assembly->cluster_start; dlist != NULL; dlist = dlist->next)
  static_cluster (fp, GAMGI_CAST_CLUSTER dlist->data, lights, error);

for (dlist = assembly->molecule_start; dlist != NULL; dlist = dlist->next)
  static_molecule (fp, GAMGI_CAST_MOLECULE dlist->data, lights, error);

for (dlist = assembly->group_start; dlist != NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);

for (dlist = assembly->plane_start; dlist != NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);

for (dlist = assembly->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = assembly->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);

for (dlist = assembly->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);

for (dlist = assembly->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

static_unscale (fp, assembly->object.scale, error);
}

static void static_light (FILE *fp, gamgi_light *light, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
double direction[3], position[3];
float color[3];
float ambient;
	
direction[0] = light->direction[0];
direction[1] = light->direction[1];
direction[2] = light->direction[2];

position[0] = light->position[0];
position[1] = light->position[1];
position[2] = light->position[2];

/***********************************************************************
 *      set ambient as the average of the three ambient components     *
 *                                                                     *
 * set color as the sum (limited to 1.0) of diffuse and specular color *
 ***********************************************************************/

ambient = (light->ambient[0] + light->ambient[1] + light->ambient[2]) / 3;

color[0] = light->diffuse[0] + light->specular[0];
if (color[0] > 1.0) color[0] = 1.0;
color[1] = light->diffuse[1] + light->specular[1];
if (color[1] > 1.0) color[1] = 1.0;
color[2] = light->diffuse[2] + light->specular[2];
if (color[2] > 1.0) color[2] = 1.0;

if (light->position[3] == 0.0)
  {
  /*********************
   * directional light *
   *********************/

  sprintf (string, "<DirectionalLight ambientIntensity=\"%.*f\""
  " color=\"%.*f %.*f %.*f\" direction=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, ambient, 
  GAMGI_MATH_DECIMAL_COLOR, color[0], 
  GAMGI_MATH_DECIMAL_COLOR, color[1], 
  GAMGI_MATH_DECIMAL_COLOR, color[2], 
  gamgi->gamgi->length, direction[0], 
  gamgi->gamgi->length, direction[1],
  gamgi->gamgi->length, direction[2]);
  }
else if (light->angle == 180.0)
  {
  /********************
   * positional light *
   ********************/

  sprintf (string, "<PointLight ambientIntensity=\"%.*f\""
  " color=\"%.*f %.*f %.*f\" location=\"%.*f %.*f %.*f\"/>\n", 
  GAMGI_MATH_DECIMAL_COLOR, ambient,
  GAMGI_MATH_DECIMAL_COLOR, color[0], 
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2], 
  gamgi->gamgi->length, position[0],
  gamgi->gamgi->length, position[1],
  gamgi->gamgi->length, position[2]);
  }
else
  {
  /**************
   * spot light *
   **************/

  sprintf (string, "<SpotLight ambientIntensity=\"%.*f\""
  " color=\"%.*f %.*f %.*f\" cutOffAngle=\"%.*f\""
  " direction=\"%.*f %.*f %.*f\" location=\"%.*f %.*f %.*f\"/>\n",
  GAMGI_MATH_DECIMAL_COLOR, ambient,
  GAMGI_MATH_DECIMAL_COLOR, color[0],
  GAMGI_MATH_DECIMAL_COLOR, color[1],
  GAMGI_MATH_DECIMAL_COLOR, color[2],
  gamgi->gamgi->angle, light->angle * GAMGI_MATH_DEG_RAD,
  gamgi->gamgi->length, direction[0], 
  gamgi->gamgi->length, direction[1], 
  gamgi->gamgi->length, direction[2], 
  gamgi->gamgi->length, position[0],
  gamgi->gamgi->length, position[1],
  gamgi->gamgi->length, position[2]);
  }

gamgi_io_file_fprintf (fp, string, error);
}

static void static_layer (FILE *fp, gamgi_layer *layer, gamgi_bool *error)
{
gamgi_dlist *dlist;
char string[GAMGI_ENGINE_LINE];
double normal[3];
double scale;
float color[4];
gamgi_bool lights;

/********************************************************
 * The exported scene describes only the current layer! *
 ********************************************************/

gamgi_io_file_fprintf (fp, "<Scene>\n", error);

/***************************************************************
 * Apparently, the GAMGI OpenGL lightning seems to be slightly *
 * better simulated in X3D when headLight is set to false.     *
 *                                                             *
 * Possible values are: WALK, FLY, EXAMINE, LOOKAT, ANY, NONE  *
 * values can be combined, for example: type='"EXAMINE" "ANY"' *
 *                                                             *
 * Apparently, the GAMGI OpenGL lightning seems to be slightly *
 * better simulated in X3D when global is set to false, which  *
 * means that lights apply only to the local node.             *
 ***************************************************************/

gamgi_io_file_fprintf (fp, "<NavigationInfo headlight=\"false\"", error);
gamgi_io_file_fprintf (fp, " type='\"ANY\"'/>\n", error);

gamgi_io_file_fprintf (fp, "<DirectionalLight global=\"false\"/>\n", error);

/**************
 * projection *
 **************/

if (layer->perspective == TRUE)
  {
  gamgi_io_file_fprintf (fp, "<Viewpoint description=\"perspective\"", error);
  sprintf (string, " fieldOfView=\"%.*f\"\n", 
  gamgi->gamgi->length, 2.0 * atan ((2.0 * layer->top) * 0.5 / layer->near));
  }
else
  {
  gamgi_io_file_fprintf (fp, "<OrthoViewpoint description=\"orthographic\"", error);
  sprintf (string, " fieldOfView=\"%.*f %.*f %.*f %.*f\"", 
  gamgi->gamgi->length, -layer->top, 
  gamgi->gamgi->length, -layer->top, 
  gamgi->gamgi->length, layer->top, 
  gamgi->gamgi->length, layer->top);
  }
gamgi_io_file_fprintf (fp, string, error);

/*************
 * modelview *
 *************/

gamgi_math_vector_sub (layer->eye, layer->center, normal);
gamgi_math_vector_normal (normal);
sprintf (string, "orientation=\"%.*f %.*f %.*f %.*f\"", 
gamgi->gamgi->length, normal[0], 
gamgi->gamgi->length, normal[1], 
gamgi->gamgi->length, normal[2],
gamgi->gamgi->length, 0.0);
gamgi_io_file_fprintf (fp, string, error);

sprintf (string, " position=\"%.*f %.*f %.*f\"/>\n", 
gamgi->gamgi->length, layer->eye[0], 
gamgi->gamgi->length, layer->eye[1], 
gamgi->gamgi->length, layer->eye[2]);
gamgi_io_file_fprintf (fp, string, error);

glGetFloatv (GL_COLOR_CLEAR_VALUE, color);
sprintf (string, "<Background skyColor=\"%.*f %.*f %.*f\"/>\n", 
GAMGI_MATH_DECIMAL_COLOR, color[0], 
GAMGI_MATH_DECIMAL_COLOR, color[1], 
GAMGI_MATH_DECIMAL_COLOR, color[2]);
gamgi_io_file_fprintf (fp, string, error);

scale = layer->object.scale;
if (fabs (scale - 1.0) > GAMGI_MATH_TOLERANCE)
  {
  sprintf (string, "<Transform scale=\"%.*f %.*f %.*f\">\n", 
  GAMGI_MATH_DECIMAL_SCALE, scale,
  GAMGI_MATH_DECIMAL_SCALE, scale, 
  GAMGI_MATH_DECIMAL_SCALE, scale);
  gamgi_io_file_fprintf (fp, string, error);
  }

if (layer->light_start == NULL) lights = FALSE;
else lights = TRUE;

/*************************************************
 * child objects: light, assembly, graph, shape, *
 * arrow, cell, cluster, molecule, group, plane, *
 * direction, atom, orbital, text                *
 *************************************************/

for (dlist = layer->light_start; dlist != NULL; dlist = dlist->next)
  static_light (fp, GAMGI_CAST_LIGHT dlist->data, error);

for (dlist = layer->assembly_start; dlist != NULL; dlist = dlist->next)
  static_assembly (fp, GAMGI_CAST_ASSEMBLY dlist->data, lights, error);

for (dlist = layer->graph_start; dlist != NULL; dlist = dlist->next)
  static_graph (fp, GAMGI_CAST_GRAPH dlist->data, lights, error);
	
for (dlist = layer->shape_start; dlist != NULL; dlist = dlist->next)
  static_shape (fp, GAMGI_CAST_SHAPE dlist->data, lights, error);
	
for (dlist = layer->arrow_start; dlist != NULL; dlist = dlist->next)
  static_arrow (fp, GAMGI_CAST_ARROW dlist->data, lights, error);
	
for (dlist = layer->cell_start; dlist != NULL; dlist = dlist->next)
  static_cell(fp, GAMGI_CAST_CELL dlist->data, lights, error);

for (dlist = layer->cluster_start; dlist != NULL; dlist = dlist->next)
  static_cluster(fp, GAMGI_CAST_CLUSTER dlist->data, lights, error);

for (dlist = layer->molecule_start; dlist != NULL; dlist = dlist->next)
  static_molecule(fp, GAMGI_CAST_MOLECULE dlist->data, lights, error);
	
for (dlist = layer->group_start; dlist != NULL; dlist = dlist->next)
  static_group (fp, GAMGI_CAST_GROUP dlist->data, lights, error);
	
for (dlist = layer->plane_start; dlist != NULL; dlist = dlist->next)
  static_plane (fp, GAMGI_CAST_PLANE dlist->data, lights, error);

for (dlist = layer->direction_start; dlist != NULL; dlist = dlist->next)
  static_direction (fp, GAMGI_CAST_DIRECTION dlist->data, lights, error);

for (dlist = layer->atom_start; dlist != NULL; dlist = dlist->next)
  static_atom (fp, GAMGI_CAST_ATOM dlist->data, lights, error);
	
for (dlist = layer->orbital_start; dlist != NULL; dlist = dlist->next)
  static_orbital (fp, GAMGI_CAST_ORBITAL dlist->data, lights, error);
	
for (dlist = layer->text_start; dlist != NULL; dlist = dlist->next)
  static_text (fp, GAMGI_CAST_TEXT dlist->data, lights, error);

if (fabs (scale - 1.0) > GAMGI_MATH_TOLERANCE)
  gamgi_io_file_fprintf (fp, "</Transform>\n", error);

gamgi_io_file_fprintf (fp, "</Scene>", error);
}

/************ external function **********
 *                                       *
 *               GAMGI_IO_X3D            *
 *                                       *
 *      Main x3d writing function call   *
 *****************************************/

gamgi_bool gamgi_io_x3d (FILE *fp, gamgi_window *window, gamgi_bool *error)
{
/*********************************
 * export only the current layer *
 *********************************/

static_layer (fp, window->layer, error);

return TRUE;
}
