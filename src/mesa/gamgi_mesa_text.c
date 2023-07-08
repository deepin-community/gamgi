/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_text.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/*
 * OGLFT: A library for drawing text with OpenGL using the FreeType library
 * Copyright (C) 2002 lignum Computing, Inc.
 * Author: Allen Barnett
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**************************************
 * Code taken from: library OGLFT-0.9 *
 *                                    *
 *  Functions with GLE code:          *
 *  gamgi_mesa_text_horizontal_move   *
 *  gamgi_mesa_text_horizontal_line   *
 *  gamgi_mesa_text_horizontal_conic  *
 *  gamgi_mesa_text_horizontal_cubic  *
 *  gamgi_mesa_text_vertical_move     *
 *  gamgi_mesa_text_vertical_line     *
 *  gamgi_mesa_text_vertical_conic    *
 *  gamgi_mesa_text_vertical_cubic    *
 *  static_glu_begin                  *
 *  static_glu_vertex                 *
 *  static_glu_end                    *
 *  static_glu_combine                *
 *  static_glu_error                  *
 *  static_face                       *
 *  static_compile_solid              *
 *  static_extruded_create            *
 *  static_extruded_remove            *
 *  static_center_solid               *
 *                                    *
 * Code taken by: Carlos Pereira      *
 **************************************/

/*
 * GLE: The Tubing and Extrusion Library
 * Copyright (C) 1991,1994,2003 Linas Vepstas
 *
 * The GLE library is distributed under two licenses:
 * GNU General Public License (GPL) and Public Domain
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*********************************************
 * Code taken from the library: GLE-3.1.0    *
 *                                           *
 * Functions with GLE code: static_extrusion *
 *                                           *
 * Code taken by: Carlos Pereira             *
 *********************************************/

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_array.h"
#include "gamgi_math_vector.h"
#include "gamgi_io_error.h"
#include "gamgi_io_font_sun.h"
#include "gamgi_io_font_hershey.h"

void static_extrusion (int n_contour, double *contour, double *normals,
double scale);

void gamgi_mesa_text_data_wired (gamgi_enum font,
const float **points, const int **driver)
{
switch (font)
  {
  case GAMGI_IO_SUN_ROMAN:
  *points = gamgi_io_font_sun_roman_points;
  *driver = gamgi_io_font_sun_roman_driver;
  break;

  case GAMGI_IO_SUN_ROMAN_MONO:
  *points = gamgi_io_font_sun_roman_mono_points;
  *driver = gamgi_io_font_sun_roman_mono_driver;
  break;

  case GAMGI_IO_HERSHEY_SANS:
  *points = gamgi_io_font_hershey_sans_points;
  *driver = gamgi_io_font_hershey_sans_driver;
  break;

  case GAMGI_IO_HERSHEY_SANS_BOLD:
  *points = gamgi_io_font_hershey_sans_bold_points;
  *driver = gamgi_io_font_hershey_sans_bold_driver;
  break;

  case GAMGI_IO_HERSHEY_SERIF:
  *points = gamgi_io_font_hershey_serif_points;
  *driver = gamgi_io_font_hershey_serif_driver;
  break;

  case GAMGI_IO_HERSHEY_SERIF_BOLD:
  *points = gamgi_io_font_hershey_serif_bold_points;
  *driver = gamgi_io_font_hershey_serif_bold_driver;
  break;

  case GAMGI_IO_HERSHEY_SCRIPT:
  *points = gamgi_io_font_hershey_script_points;
  *driver = gamgi_io_font_hershey_script_driver;
  break;

  case GAMGI_IO_HERSHEY_SCRIPT_BOLD:
  *points = gamgi_io_font_hershey_script_bold_points;
  *driver = gamgi_io_font_hershey_script_bold_driver;
  break;

  case GAMGI_IO_HERSHEY_GOTHIC_ENGLISH:
  *points = gamgi_io_font_hershey_gothic_english_points;
  *driver = gamgi_io_font_hershey_gothic_english_driver;
  break;

  case GAMGI_IO_HERSHEY_GOTHIC_GERMAN:
  *points = gamgi_io_font_hershey_gothic_german_points;
  *driver = gamgi_io_font_hershey_gothic_german_driver;
  break;

  case GAMGI_IO_HERSHEY_GOTHIC_ITALIAN:
  *points = gamgi_io_font_hershey_gothic_italian_points;
  *driver = gamgi_io_font_hershey_gothic_italian_driver;
  break;

  case GAMGI_IO_HERSHEY_SYMBOL_SANS:
  *points = gamgi_io_font_hershey_symbol_sans_points;
  *driver = gamgi_io_font_hershey_symbol_sans_driver;
  break;

  case GAMGI_IO_HERSHEY_SYMBOL_SERIF:
  *points = gamgi_io_font_hershey_symbol_serif_points;
  *driver = gamgi_io_font_hershey_symbol_serif_driver;
  break;

  case GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD:
  *points = gamgi_io_font_hershey_symbol_serif_bold_points;
  *driver = gamgi_io_font_hershey_symbol_serif_bold_driver;
  break;
  }
}

void gamgi_mesa_text_data_solid (gamgi_enum font, char *filename)
{
switch (font)
  {
  case GAMGI_IO_DEJAVU_SANS:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SANS);
  break;

  case GAMGI_IO_DEJAVU_SANS_BOLD:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SANS_BOLD);
  break;

  case GAMGI_IO_DEJAVU_SANS_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SANS_ITALIC);
  break;

  case GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SANS_BOLD_ITALIC);
  break;

  case GAMGI_IO_DEJAVU_SERIF:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SERIF);
  break;

  case GAMGI_IO_DEJAVU_SERIF_BOLD:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SERIF_BOLD);
  break;

  case GAMGI_IO_DEJAVU_SERIF_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SERIF_ITALIC);
  break;

  case GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_SERIF_BOLD_ITALIC);
  break;

  case GAMGI_IO_DEJAVU_MONO:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_MONO);
  break;

  case GAMGI_IO_DEJAVU_MONO_BOLD:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_MONO_BOLD);
  break;

  case GAMGI_IO_DEJAVU_MONO_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_MONO_ITALIC);
  break;

  case GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC:
  strcpy (filename, GAMGI_IO_FILENAME_DEJAVU_MONO_BOLD_ITALIC);
  break;
  }

}

void gamgi_mesa_text_font_create (gamgi_enum font, int **glyphs)
{
int i;

/**********************************************
 * arrays start at 0, enumerations start at 1 *
 **********************************************/

glyphs[font - 1] = (int *) malloc (sizeof (int) * GAMGI_IO_ASCII_RANGE);
for (i = 0; i < GAMGI_IO_ASCII_RANGE; i++)
  glyphs[font - 1][i] = 0;
}

void gamgi_mesa_text_font_remove (gamgi_enum font, int **glyphs)
{
/**********************************************
 * arrays start at 0, enumerations start at 1 *
 **********************************************/

free (glyphs[font - 1]);
glyphs[font - 1] = NULL;
}

void gamgi_mesa_text_font_start (void)
{
int i;
 
/****************************
 * create font glyph arrays *
 ****************************/

i = GAMGI_IO_FONTS_WIRED + GAMGI_IO_FONTS_SOLID;
gamgi->glyphs = (int **) malloc (i * sizeof (int *));
gamgi->n_glyphs = (int *) malloc (i * sizeof (int));
while (i-- > 0) { gamgi->glyphs[i] = NULL; gamgi->n_glyphs[i] = 0; }
}

void gamgi_mesa_text_font_end (void)
{
int i;

/****************************
 * delete font glyph arrays *
 ****************************/

i = GAMGI_IO_FONTS_WIRED + GAMGI_IO_FONTS_SOLID;
while (i-- > 0) gamgi_mesa_text_font_remove (i + 1, gamgi->glyphs);
free (gamgi->glyphs);
free (gamgi->n_glyphs);
}

int gamgi_mesa_text_horizontal_move (FT_Vector* to, gamgi_extruded* extruded)
{
if (extruded->open == TRUE) gluTessEndContour (extruded->tesselator);

extruded->last[0] = to->x;
extruded->last[1] = to->y;
extruded->last[2] = 0.0;

gluTessBeginContour (extruded->tesselator);
extruded->open = TRUE;

return 0;
}

int gamgi_mesa_text_horizontal_line (FT_Vector* to, gamgi_extruded* extruded)
{
double* vertex;

vertex = (double *) malloc (3 * sizeof (double));
vertex[0] = to->x;
vertex[1] = to->y;
vertex[2] = 0.0;
gamgi_math_vector_copy (vertex, extruded->last);
gamgi_math_vector_scale (vertex, vertex, extruded->scale);

/**************************************
 * pass vertex to OpenGL and save its *
 * address so it can be removed later *
 **************************************/

gluTessVertex (extruded->tesselator, vertex, vertex);
extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
extruded->vertices->data = vertex;

return 0;
}

/*********************************************************
 * TrueType and Type1 files describe the boundaries of   *
 * glyphs with quadratic and cubic curves, respectively. *
 * Since OpenGL can only draw straight lines, these      *
 * curves have to be tessellated. The number of steps    *
 * used to tessellate each curved segment of a glyph     *
 * outline is fixed for all glyphs in the face.          *
 *********************************************************/

int gamgi_mesa_text_horizontal_conic (FT_Vector* control, 
FT_Vector* to, gamgi_extruded* extruded)
{
double* vertex;
double to_vertex[3];
double control_vertex[3];
double b[2], c[2], d[2], f[2], df[2], d2f[2];
int i;

to_vertex[0] = to->x;
to_vertex[1] = to->y;
to_vertex[2] = 0.0;

control_vertex[0] = control->x;
control_vertex[1] = control->y;
control_vertex[2] = 0.0;

b[0] = extruded->last[0] - 2 * control_vertex[0] + to_vertex[0];
b[1] = extruded->last[1] - 2 * control_vertex[1] + to_vertex[1];

c[0] = -2 * extruded->last[0] + 2 * control_vertex[0];
c[1] = -2 * extruded->last[1] + 2 * control_vertex[1];

d[0] = extruded->last[0];
d[1] = extruded->last[1];

f[0] = d[0];
f[1] = d[1];
df[0] = c[0] * extruded->delta1 + b[0] * extruded->delta2;
df[1] = c[1] * extruded->delta1 + b[1] * extruded->delta2;
d2f[0] = 2 * b[0] * extruded->delta2;
d2f[1] = 2 * b[1] * extruded->delta2;

for (i = 0; i < GAMGI_MESA_EXTRUDED_STEPS - 1; i++)
  {
  f[0] += df[0];
  f[1] += df[1];

  vertex = (double *) malloc (3 * sizeof (double));
  vertex[0] = f[0];
  vertex[1] = f[1];
  vertex[2] = 0.0;
  gamgi_math_vector_scale (vertex, vertex, extruded->scale);

  /**************************************
   * pass vertex to OpenGL and save its *
   * address so it can be removed later *
   **************************************/

  gluTessVertex (extruded->tesselator, vertex, vertex);
  extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
  extruded->vertices->data = vertex;

  df[0] += d2f[0];
  df[1] += d2f[1];
  }

vertex = (double *) malloc (3 * sizeof (double));
vertex[0] = to->x;
vertex[1] = to->y;
vertex[2] = 0.0;
gamgi_math_vector_scale (vertex, vertex, extruded->scale);

/**************************************
 * pass vertex to OpenGL and save its *
 * address so it can be removed later *
 **************************************/

gluTessVertex (extruded->tesselator, vertex, vertex);
extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
extruded->vertices->data = vertex;

extruded->last[0] = to_vertex[0];
extruded->last[1] = to_vertex[1];
extruded->last[2] = to_vertex[2];

return 0;
}

/*********************************************************
 * TrueType and Type1 files describe the boundaries of   *
 * glyphs with quadratic and cubic curves, respectively. *
 * Since OpenGL can only draw straight lines, these      *
 * curves have to be tessellated. The number of steps    *
 * used to tessellate each curved segment of a glyph     *
 * outline is fixed for all glyphs in the face.          *
 *********************************************************/

int gamgi_mesa_text_horizontal_cubic (FT_Vector* control1, 
FT_Vector* control2, FT_Vector* to, gamgi_extruded* extruded)
{
double* vertex;
double to_vertex[3];
double control1_vertex[3];
double control2_vertex[3];
double a[2], b[2], c[2], d[2], f[2], df[2], d2f[2], d3f[2];
int i;

to_vertex[0] = to->x;
to_vertex[1] = to->y;
to_vertex[2] = 0.0;

control1_vertex[0] = control1->x;
control1_vertex[1] = control1->y;
control1_vertex[2] = 0.0;

control2_vertex[0] = control2->x;
control2_vertex[1] = control2->y;
control2_vertex[2] = 0.0;

a[0] = -extruded->last[0] + 
3 * control1_vertex[0] -3 * control2_vertex[0] + to_vertex[0];
a[1] = -extruded->last[1] + 
3 * control1_vertex[1] -3 * control2_vertex[1] + to_vertex[1];

b[0] = 3 * extruded->last[0] - 6 * control1_vertex[0] + 
3 * control2_vertex[0];
b[1] = 3 * extruded->last[1] - 6 * control1_vertex[1] + 
3 * control2_vertex[1];

c[0] = -3 * extruded->last[0] + 3 * control1_vertex[0];
c[1] = -3 * extruded->last[1] + 3 * control1_vertex[1];

d[0] = extruded->last[0];
d[1] = extruded->last[1];

f[0] = d[0];
f[1] = d[1];
df[0] = c[0] * extruded->delta1 + b[0] * extruded->delta2 + 
a[0] * extruded->delta3;
df[1] = c[1] * extruded->delta1 + b[1] * extruded->delta2 + 
a[1] * extruded->delta3;

d2f[0] = 2 * b[0] * extruded->delta2 + 6 * a[0] * extruded->delta3;
d2f[1] = 2 * b[1] * extruded->delta2 + 6 * a[1] * extruded->delta3;
d3f[0] = 6 * a[0] * extruded->delta3;
d3f[1] = 6 * a[1] * extruded->delta3;

for (i = 0; i < GAMGI_MESA_EXTRUDED_STEPS - 1; i++)
  {
  f[0] += df[0];
  f[1] += df[1];

  vertex = (double *) malloc (3 * sizeof (double));
  vertex[0] = f[0];
  vertex[1] = f[1];
  vertex[2] = 0.0;
  gamgi_math_vector_scale (vertex, vertex, extruded->scale);

  /**************************************
   * pass vertex to OpenGL and save its *
   * address so it can be removed later *
   **************************************/

  gluTessVertex (extruded->tesselator, vertex, vertex);
  extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
  extruded->vertices->data = vertex;

  df[0] += d2f[0];
  df[1] += d2f[1];
  d2f[0] += d3f[0];
  d2f[1] += d3f[1];
  }

vertex = (double *) malloc (3 * sizeof (double));
vertex[0] = to->x;
vertex[1] = to->y;
vertex[2] = 0.0;
gamgi_math_vector_scale (vertex, vertex, extruded->scale);

/**************************************
 * pass vertex to OpenGL and save its *
 * address so it can be removed later *
 **************************************/

gluTessVertex (extruded->tesselator, vertex, vertex);
extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
extruded->vertices->data = vertex;

extruded->last[0] = to_vertex[0];
extruded->last[1] = to_vertex[1];
extruded->last[2] = to_vertex[2];

return 0;
}

int gamgi_mesa_text_vertical_move (FT_Vector* to, gamgi_extruded* extruded)
{
if (extruded->open == TRUE)
  {
  gamgi_engine_darray_push (extruded->normals, extruded->normals->array);

  static_extrusion (extruded->contour->offset, 
  extruded->contour->array, extruded->normals->array + 2, extruded->scale);

  gamgi_engine_darray_pop (extruded->contour, 0);
  gamgi_engine_darray_pop (extruded->normals, 0); 
  }

extruded->last[0] = to->x;
extruded->last[1] = to->y;
extruded->last[2] = 0.0;

extruded->open = TRUE;

return 0;
}

int gamgi_mesa_text_vertical_line (FT_Vector* to, gamgi_extruded* extruded)
{
double vertex[2];
double normal[2];

vertex[0] = to->x;
vertex[1] = to->y;

normal[0] = extruded->normal_y * (vertex[1] - extruded->last[1]);
normal[1] = extruded->normal_x * (vertex[0] - extruded->last[0]);

gamgi_engine_darray_push (extruded->contour, vertex);
gamgi_engine_darray_push (extruded->normals, normal);

extruded->last[0] = to->x;
extruded->last[1] = to->y;
extruded->last[2] = 0.0;

return 0;
}

/*********************************************************
 * TrueType and Type1 files describe the boundaries of   *
 * glyphs with quadratic and cubic curves, respectively. *
 * Since OpenGL can only draw straight lines, these      *
 * curves have to be tessellated. The number of steps    *
 * used to tessellate each curved segment of a glyph     *
 * outline is fixed for all glyphs in the face.          *
 *********************************************************/

int gamgi_mesa_text_vertical_conic (FT_Vector* control, 
FT_Vector* to, gamgi_extruded* extruded)
{
double vertex[2];
double normal[2];
double to_vertex[2];
double control_vertex[2];
double b[2], c[2], d[2], f[2], df[2], d2f[2];
int i;

to_vertex[0] = to->x;
to_vertex[1] = to->y;

control_vertex[0] = control->x;
control_vertex[1] = control->y;

b[0] = extruded->last[0] - 2 * control_vertex[0] + to_vertex[0];
b[1] = extruded->last[1] - 2 * control_vertex[1] + to_vertex[1];

c[0] = -2 * extruded->last[0] + 2 * control_vertex[0];
c[1] = -2 * extruded->last[1] + 2 * control_vertex[1];

d[0] = extruded->last[0];
d[1] = extruded->last[1];

f[0] = d[0];
f[1] = d[1];
df[0] = c[0] * extruded->delta1 + b[0] * extruded->delta2;
df[1] = c[1] * extruded->delta1 + b[1] * extruded->delta2;
d2f[0] = 2 * b[0] * extruded->delta2;
d2f[1] = 2 * b[1] * extruded->delta2;

for (i = 0; i < GAMGI_MESA_EXTRUDED_STEPS - 1; i++) 
  {
  f[0] += df[0];
  f[1] += df[1];

  vertex[0] = f[0];
  vertex[1] = f[1];

  normal[0] = extruded->normal_y * df[1];
  normal[1] = extruded->normal_x * df[0];

  gamgi_engine_darray_push (extruded->contour, vertex);
  gamgi_engine_darray_push (extruded->normals, normal);

  df[0] += d2f[0];
  df[1] += d2f[1];
  }

vertex[0] = to->x;
vertex[1] = to->y;

normal[0] = extruded->normal_y * df[1];
normal[1] = extruded->normal_x * df[0];

gamgi_engine_darray_push (extruded->contour, vertex);
gamgi_engine_darray_push (extruded->normals, normal);

extruded->last[0] = to_vertex[0];
extruded->last[1] = to_vertex[1];
extruded->last[2] = 0.0;

return 0;
}

/*********************************************************
 * TrueType and Type1 files describe the boundaries of   *
 * glyphs with quadratic and cubic curves, respectively. *
 * Since OpenGL can only draw straight lines, these      *
 * curves have to be tessellated. The number of steps    *
 * used to tessellate each curved segment of a glyph     *
 * outline is fixed for all glyphs in the face.          *
 *********************************************************/

int gamgi_mesa_text_vertical_cubic (FT_Vector* control1, 
FT_Vector* control2, FT_Vector* to, gamgi_extruded* extruded)
{
double vertex[2];
double normal[2];
double to_vertex[2];
double control1_vertex[2];
double control2_vertex[2];
double a[2], b[2], c[2], d[2], f[2], df[2], d2f[2], d3f[2];
int i;

to_vertex[0] = to->x;
to_vertex[1] = to->y;

control1_vertex[0] = control1->x;
control1_vertex[1] = control1->y;

control2_vertex[0] = control2->x;
control2_vertex[1] = control2->y;

a[0] = -extruded->last[0] + 3 * control1_vertex[0]
-3 * control2_vertex[0] + to_vertex[0];
a[1] = -extruded->last[1] + 3 * control1_vertex[1]
-3 * control2_vertex[1] + to_vertex[1];

b[0] = 3 * extruded->last[0] - 6 * control1_vertex[0] + 
3 * control2_vertex[0];
b[1] = 3 * extruded->last[1] - 6 * control1_vertex[1] + 
3 * control2_vertex[1];

c[0] = -3 * extruded->last[0] + 3 * control1_vertex[0];
c[1] = -3 * extruded->last[1] + 3 * control1_vertex[1];

d[0] = extruded->last[0];
d[1] = extruded->last[1];

f[0] = d[0];
f[1] = d[1];
df[0] = c[0] * extruded->delta1 + b[0] * extruded->delta2 + a[0] * extruded->delta3;
df[1] = c[1] * extruded->delta1 + b[1] * extruded->delta2 + a[1] * extruded->delta3;
d2f[0] = 2 * b[0] * extruded->delta2 + 6 * a[0] * extruded->delta3;
d2f[1] = 2 * b[1] * extruded->delta2 + 6 * a[1] * extruded->delta3;
d3f[0] = 6 * a[0] * extruded->delta3;
d3f[1] = 6 * a[1] * extruded->delta3;

for (i = 0; i < GAMGI_MESA_EXTRUDED_STEPS - 1; i++)
  {
  f[0] += df[0];
  f[1] += df[1];

  vertex[0] = f[0];
  vertex[1] = f[1];

  normal[0] = extruded->normal_y * df[1];
  normal[1] = extruded->normal_x * df[0];

  gamgi_engine_darray_push (extruded->contour, vertex);
  gamgi_engine_darray_push (extruded->normals, normal);

  df[0] += d2f[0];
  df[1] += d2f[1];
  d2f[0] += d3f[0];
  d2f[1] += d3f[1];
  }

vertex[0] = to->x;
vertex[1] = to->y;

normal[0] = extruded->normal_y * df[1];
normal[1] = extruded->normal_x * df[0];

gamgi_engine_darray_push (extruded->contour, vertex);
gamgi_engine_darray_push (extruded->normals, normal);

extruded->last[0] = to_vertex[0];
extruded->last[1] = to_vertex[1];
extruded->last[2] = 0.0;

return 0;
}

/*****************************************************
 * This routine draws extrusions with square ends,   *
 * cut at 90 degrees to the length of the extrusion. *
 *****************************************************/

void static_extrusion (int n_contour, double *contour, double *normals, 
double scale)
{
int i;
double point[3]; 
double norm[3]; 
double depth;

/**************************************************************
 * the glyph z coordinate ranges between -depth/2 and depth/2 *
 *                                                            *
 *  z coordinates are independent of the face units per EM    *
 **************************************************************/

depth = GAMGI_MESA_EXTRUDED_DEPTH * GAMGI_MESA_EXTRUDED_SCALE;

/**********************
 * save graphic state *
 **********************/

glPushMatrix ();

/***************************************
 * draw all the lateral segments and   *
 * connect back with the first segment *
 ***************************************/

norm [2] = 0.0;
glBegin (GL_TRIANGLE_STRIP);
for (i = 0; i <= n_contour; i++)
  {
  if (i == n_contour) i = n_contour = 0;

  norm [0] = normals[2*i + 0];
  norm [1] = normals[2*i + 1];
  gamgi_math_vector_normal (norm);
  glNormal3dv (norm);

  point [0] = contour[2*i + 0] * scale;
  point [1] = contour[2*i + 1] * scale;
  point [2] = -depth / 2.0;
  glVertex3dv (point);

  point [2] = depth / 2.0;
  glVertex3dv (point);
  }
glEnd ();

/**********************************
 * restore previous graphic state *
 **********************************/

glPopMatrix ();
}

static void static_glu_begin (GLenum polygon)
{ 
glBegin (polygon);
}

static void static_glu_vertex (double* vertex)
{
glVertex3dv (vertex);
}

static void static_glu_end (void) 
{
glEnd ();
}

static void static_glu_combine (double coords[3], void* vertices[4], 
float weights[4], void** output, gamgi_extruded* extruded)
{
double* vertex;

/**************************************
 * pass vertex to OpenGL and save its *
 * address so it can be removed later *
 **************************************/

vertex = (double *) malloc (3 * sizeof (double));
gamgi_math_vector_copy (coords, vertex);
*output = vertex;

extruded->vertices = gamgi_engine_slist_add_start (extruded->vertices);
extruded->vertices->data = vertex;
}

static void static_glu_error (GLenum error_code)
{
printf ("error during tessellation: %s\n", gluErrorString (error_code));
}

static void static_face (gamgi_extruded *extruded, 
FT_OutlineGlyph glyph)
{
gluTessBeginPolygon (extruded->tesselator, extruded);
extruded->open = FALSE;

/*****************************************************************
 * the FreeType glyph decomposition routine traverses the        *
 * outlines of the font by calling the various routines stored   *
 * in extruded. These in turn call the GLU tesselation routines. *
 *****************************************************************/

FT_Outline_Decompose (&glyph->outline, &extruded->horizontal, extruded);
if (extruded->open == TRUE) gluTessEndContour (extruded->tesselator);
gluTessEndPolygon (extruded->tesselator);
}

gamgi_bool static static_glyph (FT_Face face, gamgi_extruded *extruded,
int ascii, FT_Glyph *glyph, char *error)
{
int index;

/***********************
 * load TrueType glyph *
 ***********************/

index = FT_Get_Char_Index (face, ascii);
if (index == 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to get index for glyph %c", ascii);
  return FALSE;
  }

if (FT_Load_Glyph (face, index, FT_LOAD_NO_SCALE) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to load glyph %c", ascii);
  return FALSE;
  }

if (FT_Get_Glyph (face->glyph, glyph) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to get glyph %c", ascii);
  return FALSE;
  }

return TRUE;
}

static gamgi_bool static_compile_wired (const float *points,
const int *driver, int c)
{
int i;

/**************************
 * create new OpenGL list *
 **************************/

glBegin (GL_LINE_STRIP);

i = driver[c];
while (points[i] != GAMGI_IO_CHAR)
  {
  if (points[i] == GAMGI_IO_STROKE)
    { glEnd (); glBegin (GL_LINE_STRIP); i += 1; }
  else
    {glVertex2f (points[i] / GAMGI_MESA_STROKED_SCALE,
    points[i+1] / GAMGI_MESA_STROKED_SCALE); i += 2; }
  }

glEnd ();
glTranslatef (points[c + 2] / GAMGI_MESA_STROKED_SCALE, 0.0, 0.0);

return TRUE;
}

static gamgi_bool static_compile_solid (FT_Face face,
gamgi_extruded *extruded, int c, char *error)
{
gamgi_slist *slist;
FT_OutlineGlyph glyph;
double depth;
int ascii;

/***********************
 * load TrueType glyph *
 ***********************/

ascii = c + GAMGI_IO_ASCII_START;
if (static_glyph (face, extruded, ascii, (void *) &glyph, error) == FALSE)
  return FALSE;

/**************************************************************
 * In theory, TrueType contours are defined clockwise and     *
 * Type1 contours are defined counter-clockwise. Trust the    *
 * flag set by FreeType to indicate this since it is critical *
 * to get the correct orientation of the surface normals.     *
***************************************************************/

if (glyph->outline.flags & FT_OUTLINE_REVERSE_FILL)
  { extruded->normal_x = -1; extruded->normal_y = 1; }
else
  { extruded->normal_x = 1; extruded->normal_y = -1; }

/**************************************************************
 * the glyph z coordinate ranges between -depth/2 and depth/2 * 
 *                                                            *
 *  z coordinates are independent of the face units per EM    *
 **************************************************************/

depth = GAMGI_MESA_EXTRUDED_DEPTH * GAMGI_MESA_EXTRUDED_SCALE;

/****************************************************
 * draw character upper face: clockwise orientation *
 ****************************************************/

glPushMatrix ();
glFrontFace (GL_CW);
glTranslatef (0.0, 0.0, depth / 2.0);
glNormal3f (0.0, 0.0, 1.0);
static_face (extruded, glyph);
glPopMatrix ();

/***********************************************************
 * draw character lower face: counterclockwise orientation *
 ***********************************************************/

glPushMatrix ();
glFrontFace (GL_CCW);
glTranslatef (0.0, 0.0, -depth / 2.0);
glNormal3f (0.0, 0.0, -1.0);
static_face (extruded, glyph);
glPopMatrix ();

/****************************************************************
 * draw character lateral contour: counterclockwise orientation *
 *                                                              *
 * the FreeType glyph decomposition routine traverses the       *
 * outlines of the font by calling the various routines stored  *
 * in extruded. These in turn call the static_extrusion routine *
 ****************************************************************/

extruded->open = FALSE;
FT_Outline_Decompose (&glyph->outline, &extruded->vertical, extruded);
if (extruded->open == TRUE)
  {
  /**********************************************************************
   * since we cannot predict when the contour is going to end (its      *
   * end is signaled by calling this routine, i.e., the contour ends    *
   * when another is started abruptly), only the lineTo and arcTo       *
   * functions generate contour points. The upshot is that the normals, *
   * which are computed for the current segment, are one behind the     *
   * segment described in the the contour array. To make things match   *
   * up at the end, the first normal is copied to the end of the normal *
   * array and the extrusion routine is passed the list of normals      *
   * starting at the second entry.                                      *
   **********************************************************************/

  gamgi_engine_darray_push (extruded->normals, extruded->normals->array);

  static_extrusion (extruded->contour->offset,
  extruded->contour->array, extruded->normals->array + 2, extruded->scale);

  gamgi_engine_darray_pop (extruded->contour, 0);
  gamgi_engine_darray_pop (extruded->normals, 0);
  }

/****************************************
 * advance to new initial position,     *
 * preparing to draw the next character *
 ****************************************/

glTranslatef (face->glyph->advance.x * extruded->scale,
face->glyph->advance.y * extruded->scale, 0.0);

/*************************************
 * free resources for this character *
 *************************************/

FT_Done_Glyph ((FT_Glyph) glyph);

for (slist = extruded->vertices; slist != NULL; slist = slist->next)
  {
  free (slist->data);
  slist = gamgi_engine_slist_remove_start (slist);
  }
extruded->vertices = NULL;

return TRUE;
}

/***************** internal function ********************
 *                                                      *
 ********************************************************/

static gamgi_bool static_compile (gamgi_enum font, 
const float *points, const int *driver, FT_Face face, 
gamgi_extruded *extruded, int c, char *error)
{
gamgi_bool valid;

/*********************
 * start OpenGL list *
 *********************/

glNewList (font * GAMGI_IO_ASCII_RANGE + c, GL_COMPILE);

/*******************************
 * render wired or solid glyph *
 *******************************/

if (font <= GAMGI_IO_FONTS_WIRED)
  valid = static_compile_wired (points, driver, c);
else
  valid = static_compile_solid (face, extruded, c, error);

/*******************
 * end OpenGL list *
 *******************/

glEndList ();

return valid;
}

/******************************************************************
 * The tessellation of curves use always the same number of       *
 * increments, regardless of length (this should be enough).      *
 *                                                                *
 * When curves are tessellated, we use the forward difference     *
 * algorithm from Foley and van Dam for parametric curves (pg.    *
 * 511 of 2nd Ed. in C). So, the step size, delta, is in the      *
 * parametric variable which is always on the interval [0,1].     *
 * Therefore, delta = 1/tessellation_steps                        *
 *                                                                *
 * For vector rendition modes, FreeType is allowed to generate    *
 * the lines and arcs at the original face definition resolution. *
 * To get to the proper glyph size, the vertices are scaled       *
 * before they're passed to the GLU tessellation routines.        *
 *                                                                *
 * The source format of outlines is a collection of closed paths  *
 * called contours. Each contour delimits an outer or inner       *
 * region of the glyph, and can be made of either line segments   *
 * or Bezier arcs.                                                *
 *                                                                *
 * The arcs are defined through control points, and can be either *
 * second-order (these are conic Beziers) or third-order (cubic   *
 * Beziers) polynomials, depending on the font format. Note that  *
 * conic Beziers are usually called quadratic Beziers in the      *
 * literature. Hence, each point of the outline has an associated *
 * flag indicating its type (normal or control point). And        *
 * scaling the points will scale the whole outline.               *
 ******************************************************************/

static gamgi_extruded *static_extruded_create (gamgi_enum font,
FT_Library *freetype, FT_Face *face, char *error)
{
gamgi_extruded *extruded;
char fullname[GAMGI_IO_FULLNAME];
char filename[GAMGI_ENGINE_STRING];

/*********************
 * get font filename *
 *********************/

gamgi_mesa_text_data_solid (font, filename);

/*********************
 * get font fullname *
 *********************/

if (gamgi->text->path == NULL)
  {
  if (error != NULL)
    sprintf (error, "Unable to get font location");
  return NULL;
  }

/***********************************************
 * check fullname size: add 2 for '/' and '\0' *
 ***********************************************/

if (strlen (gamgi->text->path) + strlen (filename) + 2 > GAMGI_IO_FULLNAME)
  {
  if (error != NULL)
    sprintf (error, "Unable to handle font filename");
  return NULL;
  }

/************************************************************
 * always put a "/" at the end of the user-controlled local *
 * pathname: a "/" too much is ok, a "/" too little is ko   *
 ************************************************************/

sprintf (fullname, "%s/%s", gamgi->text->path, filename);

/*****************************
 * create Freetype resources *
 *****************************/

if (FT_Init_FreeType (freetype) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to open the FreeType library");
  return NULL;
  }

if (FT_New_Face (*freetype, fullname, 0, face) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to open the font file %s", filename);
  FT_Done_FreeType (*freetype);
  return NULL;
  }

/****************************************************
 * As of FreeType 2.1: only a UNICODE charmap       *
 * is automatically activated. If no charmap is     *
 * activated automatically, just use the first one. *
 ****************************************************/

if ((*face)->charmap == 0 && (*face)->num_charmaps > 0)
  FT_Select_Charmap (*face, (*face)->charmaps[0]->encoding);

/**************************
 * create Gamgi resources *
 **************************/

extruded = (gamgi_extruded *) malloc (sizeof (gamgi_extruded));

/***************************************
 * functions used to render horizontal *
 * polygons: lower and upper faces     *
 ***************************************/

extruded->horizontal.move_to = (FT_Outline_MoveTo_Func) gamgi_mesa_text_horizontal_move;
extruded->horizontal.line_to = (FT_Outline_LineTo_Func) gamgi_mesa_text_horizontal_line;
extruded->horizontal.conic_to = (FT_Outline_ConicTo_Func) gamgi_mesa_text_horizontal_conic;
extruded->horizontal.cubic_to = (FT_Outline_CubicTo_Func) gamgi_mesa_text_horizontal_cubic;
extruded->horizontal.shift = 0;
extruded->horizontal.delta = 0;

/*************************************
 * functions used to render vertical *
 * polygons: lateral contour         *
 *************************************/

extruded->vertical.move_to = (FT_Outline_MoveTo_Func) gamgi_mesa_text_vertical_move;
extruded->vertical.line_to = (FT_Outline_LineTo_Func) gamgi_mesa_text_vertical_line;
extruded->vertical.conic_to = (FT_Outline_ConicTo_Func) gamgi_mesa_text_vertical_conic;
extruded->vertical.cubic_to = (FT_Outline_CubicTo_Func) gamgi_mesa_text_vertical_cubic;
extruded->vertical.shift = 0;
extruded->vertical.delta = 0;

/********************
 * tesselation data *
 ********************/

extruded->delta1 = 1.0 / GAMGI_MESA_EXTRUDED_STEPS;
extruded->delta2 = extruded->delta1 * extruded->delta1;
extruded->delta3 = extruded->delta2 * extruded->delta1;

/***********************
 * character size data *
 ***********************/

/**************************************************************
 * Sets the character dimensions of a face object. The width  *
 * and height values are expressed in 26.6 fractional points. *
 *                                                            *
 * If the horizontal or vertical resolution values are zero,  *
 * a default value of 72dpi is used. If one of the character  *
 * dimensions is zero, its value is set equal to the other.   *
 **************************************************************/

FT_Set_Char_Size (*face, 0, (*face)->units_per_EM, 0, 0);
extruded->scale = GAMGI_MESA_EXTRUDED_SCALE / (*face)->units_per_EM;

/**********************************************************
 * horizontal polygons require OpenGL tesselation, which  *
 * needs vertices and sometimes extra vertices. These     *
 * vertices must be allocated and their addresses saved,  *
 * so later they can be removed. Dynamic arrays cannot be *
 * used for this because the vertices addresses must be   *
 * kept for OpenGL to use, and realloc can change them.   *
 **********************************************************/

extruded->vertices = NULL;

/**************************************************************
 * contour and normals contain all the information needed to  *
 * render the vertical polygons that form the lateral contour *
 **************************************************************/

extruded->contour = gamgi_engine_darray_create (GAMGI_MESA_EXTRUDED_START, 
GAMGI_MESA_EXTRUDED_STEP, 2);
extruded->normals = gamgi_engine_darray_create (GAMGI_MESA_EXTRUDED_START, 
GAMGI_MESA_EXTRUDED_STEP, 2);

/*************************
 * create Mesa resources *
 *************************/

extruded->tesselator = gluNewTess ();

/*****************************************
 * tesselation OpenGL callback functions *
 *****************************************/

gluTessCallback (extruded->tesselator, GLU_TESS_VERTEX, 
(gamgi_tesselation) static_glu_vertex);
gluTessCallback (extruded->tesselator, GLU_TESS_BEGIN, 
(gamgi_tesselation) static_glu_begin);
gluTessCallback (extruded->tesselator, GLU_TESS_END, 
(gamgi_tesselation) static_glu_end);
gluTessCallback (extruded->tesselator, GLU_TESS_COMBINE_DATA, 
(gamgi_tesselation) static_glu_combine);
gluTessCallback (extruded->tesselator, GLU_TESS_ERROR, 
(gamgi_tesselation) static_glu_error);

return extruded;
}

static gamgi_bool static_extruded_remove (FT_Library freetype,
FT_Face face, gamgi_extruded *extruded, char *error)
{
gamgi_bool valid = TRUE;

/*************************
 * remove Mesa resources *
 *************************/

gluDeleteTess (extruded->tesselator);

/**************************
 * remove Gamgi resources *
 **************************/

gamgi_engine_darray_remove (extruded->contour);
gamgi_engine_darray_remove (extruded->normals);
free (extruded);

/*****************************
 * remove Freetype resources *
 *****************************/

if (FT_Done_Face (face) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to close the font");
  valid = FALSE;
  }

if (FT_Done_FreeType (freetype) != 0)
  {
  if (error != NULL)
    sprintf (error, "Unable to close the FreeType library");
  valid = FALSE;
  }

return valid;
}

static gamgi_bool static_start (gamgi_enum font,
const float **points, const int **driver, FT_Library *freetype, 
FT_Face *face, gamgi_extruded **extruded, char *error)
{
if (font > GAMGI_IO_FONTS_WIRED)
  {
  /******************************************************
   * solid font: get font filename and create resources *
   ******************************************************/

  *extruded = static_extruded_create (font, freetype, face, error);
  if (*extruded == NULL) return FALSE;
  }
else
  {
  /*******************************
   * wired font: get font arrays *
   *******************************/

  gamgi_mesa_text_data_wired (font, points, driver);
  }

return TRUE;
}

static gamgi_bool static_end (gamgi_enum font, FT_Library freetype,
FT_Face face, gamgi_extruded *extruded, char *error)
{
if (font > GAMGI_IO_FONTS_WIRED)
  {
  /******************************
   * solid font: free resources *
   ******************************/

  return static_extruded_remove (freetype, face, extruded, error);
  }

return TRUE;
}

static gamgi_bool static_create (gamgi_enum font, int ascii,
int *n_glyphs, int **glyphs, const float *points, const int *driver,
FT_Face face, gamgi_extruded *extruded, char *error)
{
int c = ascii - GAMGI_IO_ASCII_START;

/***********************************
 *       increase font count       *
 *                                 *
 * increase glyph count and create *
 * its OpenGL list if necessary    *
 ***********************************/

n_glyphs[font - 1]++;
glyphs[font - 1][c]++;
if (glyphs[font - 1][c] == 1)
  if (static_compile (font, points, driver, 
  face, extruded, c, error) == FALSE) return FALSE;

return TRUE;
}

static void static_remove (gamgi_enum font, 
int ascii, int *n_glyphs, int **glyphs)
{
int c = ascii - GAMGI_IO_ASCII_START;

/************************************
 *       decrease font count        *
 *                                  *
 * decrease glyph count and remove  *
 * its OpenGL list if not necessary *
 ************************************/

n_glyphs[font - 1]--;
glyphs[font - 1][c]--;
if (glyphs[font - 1][c] == 0)
  glDeleteLists (font * GAMGI_IO_ASCII_RANGE + c, 1);
}

static void static_decrease (gamgi_enum font, char *string, int length)
{
int **glyphs = gamgi->glyphs;
int *n_glyphs = gamgi->n_glyphs;
int i;

/**********************************************
 * arrays start at 0, enumerations start at 1 *
 **********************************************/

/****************************************************
 * remove characters, one each time. If the font is *
 * not in use anymore, remove also its glyphs array *
 ****************************************************/

for (i = 0; i < length; i++)
  static_remove (font, string[i], n_glyphs, glyphs);

if (n_glyphs[font - 1] == 0)
  gamgi_mesa_text_font_remove (font, glyphs);
}

static gamgi_bool static_new (gamgi_enum font,
char *string, int length, int **glyphs)
{
int i, c;

/*****************************************************
 * scan text string looking for new glyphs: the font *
 * glyph array is guaranteed to exist at this stage  *
 *****************************************************/

for (i = 0; i < length; i++)
  {
  c = string[i] - GAMGI_IO_ASCII_START;
  if (glyphs[font - 1][c] == 0) return TRUE;
  }

return FALSE;
}

static gamgi_bool static_center_wired (char *string, 
int length, const float *points, double *center)
{
int i, ascii;
double width;
double offset;

/*************************************************************************
 * center x coordinate: text is horizontally aligned, from left to right *
 *************************************************************************/

offset = 0.0;
gamgi_math_vector_zero (center);
for (i = 0; i < length; i++)
  {
  /**************************************
   * sum the centers for all characters *
   **************************************/

  ascii = (int) string[i];
  width = points[ascii - GAMGI_IO_ASCII_START + 2];
  center[0] += offset + width/2.0;
  offset += width;
  }
center[0] /= length;

/**********************************************************************
 * center y coordinate: the average point between font bottom and top *
 **********************************************************************/

center[1] = (points[0] + points[1]) / 2.0;

/**************************************************
 * center z coordinate: wired fonts have no depth *
 **************************************************/

center[2] = 0.0;

/****************
 * scale center *
 ****************/

gamgi_math_vector_scale (center, center, 1.0 / GAMGI_MESA_STROKED_SCALE);

return TRUE;
}

static gamgi_bool static_center_solid (char *string, int length,
FT_Face face, gamgi_extruded *extruded, double *center, char *error)
{
FT_Glyph glyph;
int i, ascii;
double width;
double offset;

/*************************************************************************
 * center x coordinate: text is horizontally aligned, from left to right *
 *************************************************************************/

offset = 0.0;
gamgi_math_vector_zero (center);
for (i = 0; i < length; i++)
  {
  /***********************
   * load TrueType glyph *
   ***********************/

  ascii = (int) string[i];
  if (static_glyph (face, extruded, ascii, &glyph, error) == FALSE)
    return FALSE;

  /**************************************
   * sum the centers for all characters *
   **************************************/

  width = face->glyph->advance.x;
  center[0] += offset + width/2.0;
  offset += width;

  /**************
   * free glyph *
   **************/

  FT_Done_Glyph (glyph);
  }
center[0] /= length;

/**********************************************************************
 * center y coordinate: the average point between font bottom and top *
 **********************************************************************/

center[1] = (face->descender + face->ascender) / 2.0;

/****************************************************************************
 * center z coordinate: solid fonts are built between -depth/2 and +depth/2 *
 ****************************************************************************/

center[2] = 0.0;

/****************
 * scale center *
 ****************/

gamgi_math_vector_scale (center, center, extruded->scale);

return TRUE;
}

static gamgi_bool static_font_remove (gamgi_enum font,
int *n_glyphs, int **glyphs)
{
if (n_glyphs[font - 1] == 0)
  gamgi_mesa_text_font_remove (font, glyphs);

return FALSE;
}

gamgi_bool gamgi_mesa_text_font_increase (gamgi_text *text, char *error)
{
int **glyphs = gamgi->glyphs;
int *n_glyphs = gamgi->n_glyphs;
gamgi_extruded *extruded = NULL;
const float *points = NULL;
const int *driver = NULL;
FT_Library freetype;
FT_Face face;
gamgi_bool new;
int i;

/**********************************************
 * arrays start at 0, enumerations start at 1 *
 **********************************************/

/***************************************
 * this case occurs when a text object *
 * is created before defining a font   *
 ***************************************/

if (text->font == 0) return TRUE;

/************************************
 * create glyphs array if necessary *
 ************************************/

if (n_glyphs[text->font - 1] == 0)
  gamgi_mesa_text_font_create (text->font, glyphs);

/***************************************************
 * check whether there are new glyphs to compile:  *
 * when we are copying a text or creating a new    *
 * one using already compiled glyphs, we don't     *
 * need to start resources, just to increase fonts *
 ***************************************************/

new = static_new (text->font, text->string, text->length, glyphs);

/************************************************
 * start resources only if there are new glyphs *
 ************************************************/

if (new == TRUE && static_start (text->font, &points,
&driver, &freetype, &face, &extruded, error) == FALSE)
  return static_font_remove (text->font, n_glyphs, glyphs);

/**************************************************
 * create characters, one each time, or remove    *
 * those just created, if an error is found. In   *
 * this case, unset also the font, otherwise char *
 * would be decreased again when text is removed  *
 **************************************************/

for (i = 0; i < text->length; i++)
  if (static_create (text->font, text->string[i], n_glyphs,
  glyphs, points, driver, face, extruded, error) == FALSE)
    {
    static_decrease (text->font, text->string, i); 
    static_end (text->font, freetype, face, extruded, error);
    return static_font_remove (text->font, n_glyphs, glyphs);
    }

/**********************************************
 * end resources only if there are new glyphs *
 **********************************************/

if (new == TRUE && static_end (text->font,
freetype, face, extruded, error) == FALSE)
  return static_font_remove (text->font, n_glyphs, glyphs);

return TRUE;
}

void gamgi_mesa_text_font_decrease (gamgi_text *text)
{
/***************************************
 * this case occurs when a text object *
 * is removed before defining a font   *
 ***************************************/

if (text->font == 0) return;

static_decrease (text->font, text->string, text->length);
}

gamgi_bool gamgi_mesa_text_center (gamgi_enum font,
char *string, int length, double *center, char *error)
{
gamgi_extruded *extruded = NULL;
const float *points = NULL;
const int *driver = NULL;
FT_Library freetype;
FT_Face face;
gamgi_bool valid;

/*******************
 * start resources *
 *******************/

valid = static_start (font, &points,
&driver, &freetype, &face, &extruded, error);

/*************************
 * determine text center *
 *************************/

if (valid == TRUE)
  {
  if (font > GAMGI_IO_FONTS_WIRED)
    valid = static_center_solid (string, length, face, extruded, center, error);
  else
    valid = static_center_wired (string, length, points, center);
  }

/*****************
 * end resources *
 *****************/

if (extruded != NULL)
  static_end (font, freetype, face, extruded, error);

return valid;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_mesa_text_create (gamgi_text *text, const char *contents)
{
int length, ascii, i;

text->length = length = strlen (contents);
text->string = (char *) malloc ((length + 1) * sizeof (char));

for (i = 0; i < length; i++)
  {
  /******************************************************
   *   Replace tabs and newlines by spaces,             *
   * so we have the same behaviour for text objects     *
   * imported from (Gtk) dialogs and (Expat XML) files. *
   *                                                    *
   * Gtk dialogs preserve tabs and newlines but Expat   *
   * replaces them by spaces, according to XML rules,   *
   * as text contents is entered as an attribute value. *
   *                                                    *
   * Futurely we should add some meta characters to     *
   * allow newlines and tabs in GAMGI text contents.    *
   ******************************************************/

  ascii = contents[i];
  if (ascii == '\n' || ascii == '\t') text->string[i] = ' ';
  else text->string[i] = ascii;
  }

text->string[i] = '\0'; 
}

void gamgi_mesa_text_remove (gamgi_text *text)
{
free (text->string);
text->string = NULL;
text->length = 0;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_mesa_text_draw (gamgi_text *text)
{
glListBase (text->font * GAMGI_IO_ASCII_RANGE - GAMGI_IO_ASCII_START);
glCallLists (text->length, GL_BYTE, (unsigned char *) text->string);
}
