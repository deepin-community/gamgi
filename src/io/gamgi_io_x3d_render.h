/******************************************
 *
 * $GAMGI/src/io/gamgi_io_x3d_render.h
 *
 * Copyright (C) 2008 Tiago Farto
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

/*********************************
 * Code taken from: OGLFT-0.9    *
 *                               *
 * Functions with OGLFT code:    *
 * x3d_font_extrusion            *
 * static_glu_begin              *
 * static_glu_vertex             *
 * static_glu_combine            *
 * static_vertical_move          *
 * static_vertical_line          *
 * static_vertical_conic         *
 * static_vertical_cubic         *
 * x3d_font_face                 *
 * static_extruded_create        *
 * static_extruded_remove        *
 * gamgi_io_x3d_render_solid     *
 *                               *
 * Code taken by: Carlos Pereira *
 *********************************/

/*
 * GLE: The Tubing and Extrusion Library
 * Copyright (C) 1991,1994,2003 Linas Vepstas
 *
 * The GLE library is distributed under two licenses:
 * GNU General Public License (GPL) and Public Domain
 *
 *  This program is free software; you can redistribute it and/or modify
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

/***********************************************
 * Code taken from: GLE-3.1.0                  *
 *                                             *
 * Functions with GLE code: x3d_font_extrusion *
 *                                             *
 * Code taken by: Carlos Pereira               *
 ***********************************************/

#define GAMGI_IO_X3D_DIRECTION_WIDTH 4.0

typedef struct
{
gamgi_extruded * extruded;

GLenum polygon;         /* polygon type */
int vcount;                     /* vertex points count */
int ocount;                     /* order points count */
int vertex;                     /* auxiliary vertex count */
int fancenter;                  /* center of the triangle fan */

float x, y, z;                  /* offset */

gamgi_darray * vert;    /* vertex array */
gamgi_iarray * order;   /* vertex order array */
} glu_tessdata;

/**********************
 * internal functions *
 **********************

static void static_point (FILE *fp, int n_points, double *points, 
float *color, int type, int width, int pattern, gamgi_bool *error);

 **********************
 * external functions *
 **********************/

void gamgi_io_x3d_render_cross (FILE *fp, int n_points, 
double *points, float *color, gamgi_bool *error);

void gamgi_io_x3d_render_chess (FILE *fp, int n_points, 
double *points,  float *color, gamgi_bool *error);

void gamgi_io_x3d_render_points (FILE *fp, int n_points, 
double *points, float *color, gamgi_bool *error);

void gamgi_io_x3d_render_loops (FILE *fp, int *loops, 
double *points, gamgi_bool *error);

void gamgi_io_x3d_render_lines (FILE *fp, 
int *lines, double *points, gamgi_bool *error);

void gamgi_io_x3d_render_triangles (FILE *fp,
int n_triangles, double * points, gamgi_bool *error);

void gamgi_io_x3d_render_line_strip (FILE *fp, 
int n_points, double * points, gamgi_bool *error);

void gamgi_io_x3d_render_polygons (FILE *fp, 
int *loops, double *points, gamgi_bool * error);

void gamgi_io_x3d_render_polygons_color (FILE *fp, int *loops, 
double *points, int *paints, float *colors, gamgi_bool * error);

void gamgi_io_x3d_render_wired (FILE *fp, gamgi_text *text, gamgi_bool *error);

void x3d_font_extrusion (glu_tessdata *data);

/**********************
 * internal functions *
 **********************

static void static_glu_begin (GLenum polygon, glu_tessdata *data);

static void static_glu_vertex (double* vertex, glu_tessdata *data);

static void static_glu_combine (double coords[3], void *vertices[4], 
float weights[4], void** output, glu_tessdata *data);

static int static_vertical_move (FT_Vector *to, glu_tessdata *data);

static int static_vertical_line (FT_Vector *to, glu_tessdata *data);

static int static_vertical_conic (FT_Vector *control, FT_Vector *to, glu_tessdata *data);

static int static_vertical_cubic (FT_Vector *control1, 
FT_Vector *control2, FT_Vector* to, glu_tessdata *data);

static void x3d_font_face (FT_OutlineGlyph glyph, glu_tessdata *data);

 **********************
 * external functions *
 **********************/

gamgi_extruded *static_extruded_create (gamgi_enum font, 
FT_Library *freetype, FT_Face *face);

gamgi_bool static_extruded_remove (FT_Library freetype, 
FT_Face face, gamgi_extruded *extruded);

void gamgi_io_x3d_render_solid (FILE *fp, gamgi_text *text, gamgi_bool *error);
