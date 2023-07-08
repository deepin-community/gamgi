/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_text.h
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
 *  Functions with OGLFT code:        *
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

/*********************************************
 * Code taken from the library: GLE-3.1.0    *
 *                                           *
 * Functions with GLE code: static_extrusion *
 *                                           *
 * Code taken by: Carlos Pereira             *
 *********************************************/

/**********************
 * external functions *
 **********************/

void gamgi_mesa_text_data_wired (gamgi_enum font,
const float **points, const int **driver);

void gamgi_mesa_text_data_solid (gamgi_enum font, char *filename);

void gamgi_mesa_text_font_create (gamgi_enum font, int **glyphs);

void gamgi_mesa_text_font_remove (gamgi_enum font, int **glyphs);

void gamgi_mesa_text_font_start (void);

void gamgi_mesa_text_font_end (void);

int gamgi_mesa_text_horizontal_move (FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_horizontal_line (FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_horizontal_conic (FT_Vector* control,
FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_horizontal_cubic (FT_Vector* control1,
FT_Vector* control2, FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_vertical_move (FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_vertical_line (FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_vertical_conic (FT_Vector* control,
FT_Vector* to, gamgi_extruded* extruded);

int gamgi_mesa_text_vertical_cubic (FT_Vector* control1,
FT_Vector* control2, FT_Vector* to, gamgi_extruded* extruded);

/**********************
 * internal functions *
 **********************

void static_extrusion (int n_contour, double *contour, double *normals, 
double scale);

static void static_glu_begin (GLenum polygon);

static void static_glu_vertex (double* vertex)

static void static_glu_end (void);

static void static_glu_combine (double coords[3], void* vertices[4], 
float weights[4], void** output, gamgi_extruded* extruded);

static void static_glu_error (GLenum error_code);

static void static_face (gamgi_extruded *extruded, 
FT_OutlineGlyph glyph);

gamgi_bool static static_glyph (FT_Face face, gamgi_extruded *extruded,
int ascii, FT_OutlineGlyph *glyph, char *error);

static gamgi_bool static_compile_wired (const float *points,
const int *driver, int c);

static gamgi_bool static_compile_solid (FT_Face face,
gamgi_extruded *extruded, int c, char *error);

static gamgi_bool static_compile (gamgi_enum font,
const float *points, const int *driver, FT_Face face,
gamgi_extruded *extruded, int c, char *error);

static gamgi_extruded *static_extruded_create (gamgi_enum font,
FT_Library *freetype, FT_Face *face, char *error);

static gamgi_bool static_extruded_remove (FT_Library freetype,
FT_Face face, gamgi_extruded *extruded, char *error);

static gamgi_bool static_start (gamgi_enum font,
const float **points, const int **driver, FT_Library *freetype, 
FT_Face *face, gamgi_extruded **extruded, char *error);

static gamgi_bool static_end (gamgi_enum font, FT_Library freetype,
FT_Face face, gamgi_extruded *extruded, char *error);

static gamgi_bool static_create (gamgi_enum font, int ascii,
int *n_glyphs, int **glyphs, const float *points, const int *driver,
FT_Face face, gamgi_extruded *extruded, char *error);

static void static_remove (gamgi_enum font,
int ascii, int *n_glyphs, int **glyphs);

static void static_decrease (gamgi_enum font, char *string, int length);

static gamgi_bool static_new (gamgi_enum font,
char *string, int length, int **glyphs);

static gamgi_bool static_center_wired (char *string,
int length, const float *points, double *center);

static gamgi_bool static_center_solid (char *string, int length,
FT_Face face, gamgi_extruded *extruded, double *center, char *error);

static gamgi_bool static_font_remove (gamgi_enum font,
int *n_glyphs, int **glyphs);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_mesa_text_font_increase (gamgi_text *text, char *error);

void gamgi_mesa_text_font_decrease (gamgi_text *text);

gamgi_bool gamgi_mesa_text_center (gamgi_enum font,
char *string, int length, double *center, char *error);

void gamgi_mesa_text_create (gamgi_text *text, const char *contents);

void gamgi_mesa_text_remove (gamgi_text *text);

void gamgi_mesa_text_draw (gamgi_text *text);

/*************
 * Rationale *
 *************

Done, solid fonts require external .ttf files.

 *************
 * Rationale *
 *************/
