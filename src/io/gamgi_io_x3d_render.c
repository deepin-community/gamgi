/******************************************
 *
 * $GAMGI/src/io/gamgi_io_x3d_render.c
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
#include "gamgi_math_vector.h"
#include "gamgi_io_file.h"
#include "gamgi_io_font_sun.h"
#include "gamgi_io_font_hershey.h"
#include "gamgi_io_x3d_render.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_text.h"

#define GAMGI_IO_X3D_TOTAL 500
#define GAMGI_IO_X3D_STEP 2
#define GAMGI_IO_X3D_POINT_SIZE 0.025

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

/************* internal function ***********
 *                                         *
 *               STATIC_POINT              *
 *                                         *
 *           render a X3D point            *
 *******************************************/

static void static_point (FILE *fp, int n_points, double *points, 
float *color, int type, int width, int pattern, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
float p[3];
int i;

if (n_points <= 0) return;

/***************
 * Shape start *
 ***************/

gamgi_io_file_fprintf (fp, "<Shape>\n", error);

/********************
 * Appearance start *
 ********************/

if (color != NULL || width == TRUE || pattern == TRUE)
  {
  gamgi_io_file_fprintf (fp, "<Appearance>\n", error);

  /************************
   * LineProperties start *
   ************************/

  if (width == TRUE || pattern == TRUE)
    gamgi_io_file_fprintf (fp, "<LineProperties ", error);

  if (width  == TRUE)
    {
    sprintf (string, "linewidthScaleFactor=\"%d\" ", width);
    gamgi_io_file_fprintf (fp, string, error);
    }

  if (pattern == TRUE)
    {
    sprintf (string, "linetype=\"%d\" ", pattern);
    gamgi_io_file_fprintf (fp, string, error);
    }

  /**********************
   * LineProperties end *
   **********************/

  if (width == TRUE || pattern == TRUE)
    gamgi_io_file_fprintf (fp, "/>\n", error);
			
  if (color != NULL)
    {
    sprintf (string, "<Material emissiveColor=\"%f %f %f\"/>\n", 
    color[0], color[1], color[2]);
    gamgi_io_file_fprintf (fp, string, error);
    }

  gamgi_io_file_fprintf (fp, "</Appearance>\n", error);
  }

/******************
 * Appearance end *
 ******************/

/************************
 * IndexedlineSet start *
 ************************/

gamgi_io_file_fprintf (fp, "<IndexedLineSet coordIndex=\"\n", error);

for (i = 0; i < n_points; i++)
  {
  if (type == 1)
    sprintf (string, "%d %d -1 %d %d -1 %d %d -1 %d %d -1\n", 
    i*8 + 0, i*8 + 1, i*8 + 2, i*8 + 3, i*8 + 4, i*8 + 5, i*8 + 6, i*8 + 7);
  else
    sprintf (string, "%d %d -1 %d %d -1 %d %d -1\n", 
    i*6 + 0, i*6 + 1, i*6 + 2, i*6 + 3, i*6 + 4, i*6 + 5);

  gamgi_io_file_fprintf (fp, string, error);
  }

gamgi_io_file_fprintf (fp, "\">\n", error);

/********************
 * Coordinate start *
 ********************/

gamgi_io_file_fprintf (fp, "<Coordinate point=\"\n", error);

for (i = 0; i < n_points; i++)
  {
  p[0] = points[3*i + 0];
  p[1] = points[3*i + 1];
  p[2] = points[3*i + 2];

  if (type == 1)			
    {
    sprintf (string, "%f %f %f %f %f %f\n",
    p[0] - GAMGI_IO_X3D_POINT_SIZE, p[1] + GAMGI_IO_X3D_POINT_SIZE,
    p[2] - GAMGI_IO_X3D_POINT_SIZE, p[0] + GAMGI_IO_X3D_POINT_SIZE,
    p[1] - GAMGI_IO_X3D_POINT_SIZE, p[2] + GAMGI_IO_X3D_POINT_SIZE);
    gamgi_io_file_fprintf (fp, string, error);

    sprintf (string, "%f %f %f %f %f %f\n",
    p[0] + GAMGI_IO_X3D_POINT_SIZE, p[1] + GAMGI_IO_X3D_POINT_SIZE,
    p[2] - GAMGI_IO_X3D_POINT_SIZE, p[0] - GAMGI_IO_X3D_POINT_SIZE,
    p[1] - GAMGI_IO_X3D_POINT_SIZE, p[2] + GAMGI_IO_X3D_POINT_SIZE);
    gamgi_io_file_fprintf (fp, string, error);

    sprintf (string, "%f %f %f %f %f %f\n",
    p[0] - GAMGI_IO_X3D_POINT_SIZE, p[1] + GAMGI_IO_X3D_POINT_SIZE,
    p[2] + GAMGI_IO_X3D_POINT_SIZE, p[0] + GAMGI_IO_X3D_POINT_SIZE,
    p[1] - GAMGI_IO_X3D_POINT_SIZE, p[2] - GAMGI_IO_X3D_POINT_SIZE);
    gamgi_io_file_fprintf (fp, string, error);

    sprintf (string, "%f %f %f %f %f %f\n",
    p[0] + GAMGI_IO_X3D_POINT_SIZE, p[1] + GAMGI_IO_X3D_POINT_SIZE,
    p[2] + GAMGI_IO_X3D_POINT_SIZE, p[0] - GAMGI_IO_X3D_POINT_SIZE,
    p[1] - GAMGI_IO_X3D_POINT_SIZE, p[2] - GAMGI_IO_X3D_POINT_SIZE);
    gamgi_io_file_fprintf (fp, string, error);
    }
  else
    {
    sprintf (string, "%f %f %f %f %f %f\n",
    p[0] + GAMGI_IO_X3D_POINT_SIZE, p[1], p[2],
    p[0] - GAMGI_IO_X3D_POINT_SIZE, p[1], p[2]);
    gamgi_io_file_fprintf (fp, string, error);

    sprintf (string, "%f %f %f %f %f %f\n",
    p[0], p[1] + GAMGI_IO_X3D_POINT_SIZE, p[2],
    p[0], p[1] - GAMGI_IO_X3D_POINT_SIZE, p[2]);
    gamgi_io_file_fprintf (fp, string, error);

    sprintf (string, "%f %f %f %f %f %f\n",
    p[0], p[1], p[2] + GAMGI_IO_X3D_POINT_SIZE,
    p[0], p[1], p[2] - GAMGI_IO_X3D_POINT_SIZE);
    gamgi_io_file_fprintf (fp, string, error);
    }
  }

/***************************************
 * Coordinate+IndexedLineSet+Shape end *
 ***************************************/

gamgi_io_file_fprintf (fp, "\"/>\n", error);
gamgi_io_file_fprintf (fp, "</IndexedLineSet>\n", error);
gamgi_io_file_fprintf (fp, "</Shape>\n", error);
}

/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_CROSS        *
 *                                         *
 *    draws points of the type "cross"     * 
 *******************************************/

void gamgi_io_x3d_render_cross (FILE *fp, int n_points, 
double *points, float *color, gamgi_bool *error)
{
static_point (fp, n_points, points, color, 0, 4, 0, error);
}


/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_CHESS        *
 *                                         *
 *    draws points of the type "chess"     *
 *******************************************/

void gamgi_io_x3d_render_chess (FILE *fp, int n_points, 
double *points,  float *color, gamgi_bool *error)
{
static_point (fp, n_points, points, color, 1, 4, 0, error);
}

/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_POINTS       *
 *                                         *
 *    draws points of the type "normal"    *
 *******************************************/

void gamgi_io_x3d_render_points (FILE *fp, int n_points, 
double *points, float *color, gamgi_bool *error)
{
static_point (fp, n_points, points, color, 0, 3, 0, error);
}

/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_LOOPS        *
 *                                         *
 *    draws line loops (connected lines)   *
 *******************************************/

void gamgi_io_x3d_render_loops (FILE *fp, int *loops, 
double *points, gamgi_bool * error)
{
char string[GAMGI_ENGINE_LINE];
int i, j, c, p, n_loops;
	
if (loops == NULL) return;

n_loops = loops[0];
	
gamgi_io_file_fprintf (fp, "<IndexedLineSet coordIndex='", error);

p = 1;
c = 0;
for (i = 0; i < n_loops; i++)
  {
  for (j = 0; j < loops[p]; j++)
    {
    sprintf (string, "%d ", c);
    gamgi_io_file_fprintf(fp, string, error);
    c++;
    }
  sprintf (string, "%d -1", c - loops[p]);
  gamgi_io_file_fprintf(fp, string, error);
  if (i != n_loops - 1)
  gamgi_io_file_fprintf(fp, " ", error);
		
  p += loops[p]+1;
  }
	
gamgi_io_file_fprintf (fp, "'>\n<Coordinate point='", error);
	
i = 1;
while (n_loops-- > 0)
  {
  p = loops[i++];
  while (p-- > 0)
    {
    sprintf (string, "%f %f %f", 
    points[3*loops[i]], points[3*loops[i]+1], points[3*loops[i]+2]);
    gamgi_io_file_fprintf (fp, string, error);
    i++;
			
    if (p > 0 || n_loops > 0) gamgi_io_file_fprintf (fp, ", ", error);
    }
  }
gamgi_io_file_fprintf(fp, "'/>\n</IndexedLineSet>\n", error);
}

/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_LINES        *
 *                                         *
 *               draws lines               *
 *******************************************/

void gamgi_io_x3d_render_lines (FILE *fp, 
int *lines, double *points, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int i, n_lines;

n_lines = lines[0];
if (n_lines == 0) return;

gamgi_io_file_fprintf(fp, "<IndexedLineSet coordIndex='", error);
	
for (i = 0; i <n_lines; i++)
  {
  sprintf (string, "%d %d -1", i*2, i*2+1);
  gamgi_io_file_fprintf (fp, string, error);

  if (i != n_lines - 1)
    gamgi_io_file_fprintf (fp, " ", error);
  }
	
gamgi_io_file_fprintf(fp, "'>\n<Coordinate point='", error);
	
i = 1;
while (n_lines-- > 0)
  {
  sprintf (string, "%f %f %f, ", 
  points[3*lines[i]], points[3*lines[i]+1], points[3*lines[i]+2]);
  gamgi_io_file_fprintf(fp, string, error);
  i++;
  sprintf (string, "%f %f %f", 
  points[3*lines[i]], points[3*lines[i]+1], points[3*lines[i]+2]);
  gamgi_io_file_fprintf(fp, string, error);
  i++;

  if (n_lines>0)
  gamgi_io_file_fprintf(fp, ", ", error);
  }

gamgi_io_file_fprintf(fp, "'/>\n</IndexedLineSet>\n", error);
}

/************* external function ***********
 *                                         *
 *      GAMGI_IO_X3D_RENDER_TRIANGLES      *
 *                                         *
 *       render a triangle-based face      *
 *******************************************/

void gamgi_io_x3d_render_triangles (FILE *fp,
int n_triangles, double *points, gamgi_bool *error)
{
char string[GAMGI_ENGINE_LINE];
int n_points, i;

if (n_triangles == 0) return;
n_points = 3 * n_triangles;

gamgi_io_file_fprintf (fp, "<IndexedTriangleSet ", error);
gamgi_io_file_fprintf (fp, "solid=\"false\" ", error);
gamgi_io_file_fprintf (fp, "ccw=\"true\" ", error);
gamgi_io_file_fprintf (fp, "normalPerVertex=\"false\" ", error);
gamgi_io_file_fprintf (fp, "index=\"\n", error);

for (i = 0; i < n_points; i += 3)
  {
  sprintf (string, "%d %d %d\n", i + 0, i + 1, i + 2);
  gamgi_io_file_fprintf (fp, string, error);
  }
gamgi_io_file_fprintf (fp, "\">\n<Coordinate point=\"\n", error);

for (i = 0; i < n_points; i++)
  {
  sprintf (string, "%f %f %f\n",
  points[3 * i + 0], points[3 * i + 1] + 10, points[3 * i + 2]);
  gamgi_io_file_fprintf (fp, string, error);
  }

gamgi_io_file_fprintf (fp, "\"/>\n</IndexedTriangleSet>\n", error);
}

/************* external function ***********
 *                                         *
 *      GAMGI_IO_X3D_RENDER_LINE_STRIP     *
 *                                         *
 *            draws a line strip           *
 *******************************************/

void gamgi_io_x3d_render_line_strip (FILE *fp, 
int n_points, double * points, gamgi_bool * error)
{
char string[GAMGI_ENGINE_LINE];
int i;
	
if (n_points == 0) return;
	
gamgi_io_file_fprintf(fp, "<IndexedLineSet coordIndex='", error);
	
for (i = 0; i < n_points; i++)
  {
  sprintf (string, "%d", i);
  gamgi_io_file_fprintf(fp, string, error);
		
  if (i != n_points - 1)
    gamgi_io_file_fprintf (fp, " ", error);
  else
   gamgi_io_file_fprintf(fp, " -1'>\n<Coordinate point='", error);
  }

while (n_points-- > 0)
  {
  sprintf (string, "%f %f %f", 
  *(points+3*n_points), *(points+3*n_points+1), *(points+3*n_points+2));
  gamgi_io_file_fprintf (fp, string, error);

  if (n_points > 0)
  gamgi_io_file_fprintf (fp, ", ", error);
  }

gamgi_io_file_fprintf (fp, "'/>\n</IndexedLineSet>\n", error);
}

/************* external function ***********
 *                                         *
 *        GAMGI_IO_X3D_RENDER_POLYGONS     *
 *                                         *
 *                draws polygons           *
 *******************************************/

void gamgi_io_x3d_render_polygons (FILE *fp, 
int *loops, double *points, gamgi_bool * error)
{
	char str[256];
	int p, i, j, n_loops, c;
	int * n_vertices;
	
	double ** point;
	
	p = 0;
	c = 0;
	
	n_loops = loops[p++];
	n_vertices = malloc(sizeof(int)*n_loops);
	point = malloc(sizeof(double *)*n_loops);
	
	gamgi_io_file_fprintf(fp, "<IndexedFaceSet solid='false' coordIndex='", error);

	for(i=0; i<n_loops; i++)
	{
		n_vertices[i] = loops[p++];
		point[i] = malloc(n_vertices[i]*sizeof(double)*3);
		
		for(j=0; j<n_vertices[i]; j++)
		{
			if(i==0 && j==0)
				sprintf(str, "%d", c);
			else
				sprintf(str, " %d", c);
			
			gamgi_io_file_fprintf(fp, str, error);
			
			c++;
			
			point[i][j*3] = points[3 * loops[p]];
			point[i][j*3+1] = points[3 * loops[p]+1];
			point[i][j*3+2] = points[3 * loops[p]+2];
			
			p++;
		}
		
		sprintf(str, " -1");
		
		gamgi_io_file_fprintf(fp, str, error);
	}
	
	gamgi_io_file_fprintf(fp, "'>\n<Coordinate point='", error);
	for(i=0; i<n_loops; i++)
	{
		for(j=0; j<n_vertices[i]; j++)
		{
			sprintf(str, "%f %f %f", point[i][j*3], point[i][j*3+1], point[i][j*3+2]);
			gamgi_io_file_fprintf(fp, str, error);
			if(!(i==n_loops-1&&j==n_vertices[i]-1))
				gamgi_io_file_fprintf(fp, ", ", error);
		}
	}
		
	gamgi_io_file_fprintf(fp, "'/>\n</IndexedFaceSet>\n", error);
	
	for(i=0; i<n_loops; i++) free(point[i]);
	free(point);
}


/* draws colored polygons */
void gamgi_io_x3d_render_polygons_color (FILE *fp, int *loops, 
double *points, int *paints, float *colors, gamgi_bool * error)
{
	char str[256];
	int p, i, j, n_loops, c;
	int * n_vertices;
	
	double ** point;
	
	p = 0;
	c = 0;
	
	n_loops = loops[p++];
	n_vertices = malloc(sizeof(int)*n_loops);
	point = malloc(sizeof(double *)*n_loops);
	
	gamgi_io_file_fprintf(fp, "<IndexedFaceSet solid='false' coordIndex='", error);

	for(i=0; i<n_loops; i++)
	{
		n_vertices[i] = loops[p++];
		point[i] = malloc(n_vertices[i]*sizeof(double)*3);

		for(j=0; j<n_vertices[i]; j++)
		{
			if(i==0 && j==0)
				sprintf(str, "%d", c);
			else
				sprintf(str, " %d", c);
			
			gamgi_io_file_fprintf(fp, str, error);
			
			c++;
			
			point[i][j*3] = points[3 * loops[p]];
			point[i][j*3+1] = points[3 * loops[p]+1];
			point[i][j*3+2] = points[3 * loops[p]+2];
			
			p++;
		}
		
		sprintf(str, " -1");
		
		gamgi_io_file_fprintf(fp, str, error);
	}
	
	gamgi_io_file_fprintf(fp, "' colorPerVertex='false' colorIndex='", error);

	c=-1;
	p=1;
	for(i=0; i<n_loops; i++)
	{
		if(paints[0] == p)
		{
			c++;
			paints++;
		}
		
		p += loops[p]+1;
		
		sprintf(str, "%d", c);
		gamgi_io_file_fprintf(fp, str, error);

		if(i<n_loops)
			gamgi_io_file_fprintf(fp, " ", error);
	}
	
	gamgi_io_file_fprintf(fp, "'>\n<Coordinate point='", error);
	for(i=0; i<n_loops; i++)
	{
		for(j=0; j<n_vertices[i]; j++)
		{
			sprintf(str, "%f %f %f", point[i][j*3], point[i][j*3+1], point[i][j*3+2]);
			gamgi_io_file_fprintf(fp, str, error);
			if(!(i==n_loops-1&&j==n_vertices[i]-1))
				gamgi_io_file_fprintf(fp, ", ", error);
		}
	}
		
	gamgi_io_file_fprintf(fp, "'/>\n<Color color='", error);

	for(i=0; i<=c; i++)
	{
		sprintf(str, "%f %f %f", colors[0], colors[1], colors[2]);
		gamgi_io_file_fprintf(fp, str, error);
		colors += 3;
		
		if(i!=c)
			gamgi_io_file_fprintf(fp, ", ", error);
	}
	
	gamgi_io_file_fprintf(fp, "'/>\n</IndexedFaceSet>\n", error);
	
	
	
	for(i=0; i<n_loops; i++) free(point[i]);
	free(point);
}

/************* internal function ***************
 *                                             *
 *           GAMGI_IO_X3D_RENDER_WIRED         *
 *                                             *
 * draws wired text using the hard coded fonts *
 ***********************************************/

void gamgi_io_x3d_render_wired (FILE *fp, gamgi_text *text, gamgi_bool *error)
{
int c = 0;
int i, j;
int *driver;
float *points;
char str[256];
float spacing;

printf ("ola\n");

gamgi_mesa_text_data_wired (text->font, (const float **) &points, (const int **) &driver);
gamgi_io_file_fprintf (fp, "<IndexedLineSet coordIndex='", error);
	
		
	for(i=0; i<text->length; i++)
	{
		j = driver[text->string[i]-GAMGI_IO_ASCII_START];
	
		while(points[j] != GAMGI_IO_CHAR)
		{
			if(points[j] == GAMGI_IO_STROKE)
			{
				gamgi_io_file_fprintf(fp, "-1, ", error);
				j++;
			}
			else
			{
				sprintf(str, "%d, ", c);
				gamgi_io_file_fprintf(fp, str, error);			
				c++;
				j+=2;
			}
		}
		
		if(i<text->length-1)
			gamgi_io_file_fprintf(fp, "-1, ", error);
	}
	
	gamgi_io_file_fprintf(fp, "-1'>\n<Coordinate point='", error);
	spacing = 0.0f;
	
	for(i=0; i<text->length; i++)
	{
		j = driver[text->string[i]-GAMGI_IO_ASCII_START];
		
		while(points[j] != GAMGI_IO_CHAR)
		{
			if(points[j] != GAMGI_IO_STROKE)
			{
				sprintf(str, "%f %f 0.0", points[j]/GAMGI_MESA_STROKED_SCALE+spacing, points[j+1]/GAMGI_MESA_STROKED_SCALE);
				gamgi_io_file_fprintf(fp, str, error);
				j+=2;
				
				if(i<text->length-1 || points[j] != GAMGI_IO_CHAR)
					gamgi_io_file_fprintf(fp, ", ", error);
			}
			else
			{
				j++;
			}
		}
		
		spacing += points[text->string[i]-GAMGI_IO_ASCII_START+2]/GAMGI_MESA_STROKED_SCALE;
	}
	
	gamgi_io_file_fprintf(fp, "'/>\n</IndexedLineSet>\n", error);
}

void x3d_font_extrusion(glu_tessdata * data)
{
	int i;
	double depth = GAMGI_MESA_EXTRUDED_DEPTH * GAMGI_MESA_EXTRUDED_SCALE;
	
	int n_contour = data->extruded->contour->offset;
	double *contour = data->extruded->contour->array;
	double scale = data->extruded->scale;
	
	int itemp[4];
	double dtemp[3];

	data->polygon = GL_TRIANGLE_STRIP;
	data->vertex = 0;
	
	for(i = 0; i <= n_contour; i++)
	{		
		if(i == n_contour)
			i = n_contour = 0;

		if(data->vertex>1)
		{
			itemp[0] = data->vcount/3-2;
			itemp[1] = data->vcount/3-1;
			itemp[2] = data->vcount/3;
			itemp[3] = -1;
			
			data->ocount += 4;
			
			gamgi_engine_iarray_push(data->order, itemp);
		}
		
		if(contour[2*i + 0] == contour[2*i + 0])
			dtemp[0] = data->x + contour[2*i + 0] * scale;
		else
			dtemp[0] = data->x;
		
		if(contour[2*i + 1] == contour[2*i + 1])
			dtemp[1] = data->y + contour[2*i + 1] * scale;
		else
			dtemp[1] = data->y;
		
		dtemp[2] = data->z - depth / 2.0;
		
		data->vcount += 3;
		data->vertex++;
		
		gamgi_engine_darray_push(data->vert, dtemp);
		
		if(data->vertex>1)
		{
			itemp[0] = data->vcount/3-2;
			itemp[1] = data->vcount/3-1;
			itemp[2] = data->vcount/3;
			itemp[3] = -1;
			
			data->ocount += 4;
			
			gamgi_engine_iarray_push(data->order, itemp);
		}
		
		if(contour[2*i + 0] == contour[2*i + 0])
			dtemp[0] = data->x + contour[2*i + 0] * scale;
		else
			dtemp[0] = data->x;
		
		if(contour[2*i + 1] == contour[2*i + 1])
			dtemp[1] = data->y + contour[2*i + 1] * scale;
		else
			dtemp[1] = data->y;
		
		dtemp[2] = data->z + depth / 2.0;
		
		data->vcount += 3;
		data->vertex++;
		
		gamgi_engine_darray_push(data->vert, dtemp);
	}
}


static void static_glu_begin (GLenum polygon, glu_tessdata *data)
{ 
data->polygon = polygon;
data->vertex = 0;
	
if (polygon == GL_TRIANGLE_FAN)
  data->fancenter = data->vcount / 3;
}

static void static_glu_vertex (double* vertex, glu_tessdata *data)
{	
int temp[4];
double point[3];

switch (data->polygon)
  {
  case GL_TRIANGLES:		/* 0, 1, 2, -1, 3, 4, 5, -1... */
  if ((data->vertex + 1) % 3 == 0)
    {
    temp[0] = data->vcount / 3 - 2;
    temp[1] = data->vcount / 3 - 1;
    temp[2] = data->vcount / 3 - 0;
    temp[3] = -1;
				
    data->ocount += 4;
    gamgi_engine_iarray_push(data->order, temp);
    }
  break;

  case GL_TRIANGLE_STRIP:	/*Strip: 0,1,2,-1,1,2,3,-1,2,3,4,-1... "n-2, n-1, n, -1" */
  if (data->vertex > 1)
    {
    temp[0] = data->vcount / 3 - 2;
    temp[1] = data->vcount / 3 - 1;
    temp[2] = data->vcount / 3 - 0;
    temp[3] = -1;
				
    data->ocount += 4;
    gamgi_engine_iarray_push(data->order, temp);
    }
  break;

  case GL_TRIANGLE_FAN:		/* Fan: 0,1,2,-1,0,2,3,-1,0,3,4,-1... "0, n-1, n, -1" */
  if (data->vertex > 1)
    {
    temp[0] = data->fancenter;
    temp[1] = data->vcount / 3 - 1;
    temp[2] = data->vcount / 3;
    temp[3] = -1;
				
    data->ocount += 4;
    gamgi_engine_iarray_push(data->order, temp);
    }
  break;
  }

point[0] = vertex[0] + data->x;
point[1] = vertex[1] + data->y;
point[2] = vertex[2] + data->z;
	
data->vcount += 3;
data->vertex++;
	
gamgi_engine_darray_push(data->vert, point);
}


static void static_glu_combine (double coords[3], void *vertices[4], 
float weights[4], void** output, glu_tessdata *data)
{
double *vertex;
gamgi_extruded *extruded = data->extruded;

/***************************************************
 * vertices and weights data is used to calculate  *
 * averaged points and is not used in this context *
 ***************************************************/

vertex = (double *) malloc (3 * sizeof(double));
gamgi_math_vector_copy(coords, vertex);

*output = vertex;
extruded->extra = gamgi_engine_slist_add_start (extruded->extra);
extruded->extra->data = vertex;
}

static int static_vertical_move (FT_Vector *to, glu_tessdata *data)
{
if (data->extruded->open == TRUE)
  {
  gamgi_engine_darray_push(data->extruded->normals, data->extruded->normals->array);

  x3d_font_extrusion (data);

  gamgi_engine_darray_pop(data->extruded->contour, 0);
  gamgi_engine_darray_pop(data->extruded->normals, 0); 
  }

data->extruded->last[0] = to->x;
data->extruded->last[1] = to->y;
data->extruded->last[2] = 0.0;

data->extruded->open = TRUE;

return 0;
}

static int static_vertical_line (FT_Vector *to, glu_tessdata *data)
{
double vertex[2];
double normal[2];
gamgi_extruded * extruded = data->extruded;

vertex[0] = to->x;
vertex[1] = to->y;

normal[0] = extruded->normal_y * (vertex[1] - extruded->last[1]);
normal[1] = extruded->normal_x * (vertex[0] - extruded->last[0]);

gamgi_engine_darray_push (extruded->contour, vertex);
gamgi_engine_darray_push (extruded->normals, normal);

extruded->last[0] = vertex[0];
extruded->last[1] = vertex[1];
extruded->last[2] = 0.0;

return 0;
}

static int static_vertical_conic (FT_Vector *control, FT_Vector *to, glu_tessdata *data)
{
double vertex[2];
double normal[2];
double to_vertex[2];
double control_vertex[2];
double b[2], c[2], d[2], f[2], df[2], d2f[2];
int i;
gamgi_extruded * extruded = data->extruded;

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

static int static_vertical_cubic (FT_Vector *control1, 
FT_Vector *control2, FT_Vector* to, glu_tessdata * data)
{
gamgi_extruded *extruded = data->extruded;
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

static void x3d_font_face (FT_OutlineGlyph glyph, glu_tessdata *data)
{
gluTessBeginPolygon (data->extruded->tesselator, data);
data->extruded->open = FALSE;

FT_Outline_Decompose (&glyph->outline, &data->extruded->horizontal, data->extruded);

if (data->extruded->open == TRUE)
  gluTessEndContour (data->extruded->tesselator);

gluTessEndPolygon (data->extruded->tesselator);
}

gamgi_extruded *static_extruded_create (gamgi_enum font, 
FT_Library *freetype, FT_Face *face)
{
gamgi_extruded *extruded;
char fullname[GAMGI_IO_FULLNAME];
char filename[GAMGI_ENGINE_TOKEN];

gamgi_mesa_text_data_solid (font, filename);

sprintf (fullname, "%s/%s", gamgi->text->path, filename);

FT_Init_FreeType (freetype);

FT_New_Face (*freetype, fullname, 0, face);

if ((*face)->charmap == 0 && (*face)->num_charmaps > 0)
  FT_Select_Charmap (*face, (*face)->charmaps[0]->encoding);

extruded = (gamgi_extruded *) malloc (sizeof (gamgi_extruded));

extruded->horizontal.move_to = (FT_Outline_MoveTo_Func) gamgi_mesa_text_horizontal_move;
extruded->horizontal.line_to = (FT_Outline_LineTo_Func) gamgi_mesa_text_horizontal_line;
extruded->horizontal.conic_to = (FT_Outline_ConicTo_Func) gamgi_mesa_text_horizontal_conic;
extruded->horizontal.cubic_to = (FT_Outline_CubicTo_Func) gamgi_mesa_text_horizontal_cubic;
extruded->horizontal.shift = 0;
extruded->horizontal.delta = 0;

extruded->vertical.move_to = (FT_Outline_MoveTo_Func) static_vertical_move;
extruded->vertical.line_to = (FT_Outline_LineTo_Func) static_vertical_line;
extruded->vertical.conic_to = (FT_Outline_ConicTo_Func) static_vertical_conic;
extruded->vertical.cubic_to = (FT_Outline_CubicTo_Func) static_vertical_cubic;
extruded->vertical.shift = 0;
extruded->vertical.delta = 0;

extruded->delta1 = 1.0 / GAMGI_MESA_EXTRUDED_STEPS;
extruded->delta2 = extruded->delta1 * extruded->delta1;
extruded->delta3 = extruded->delta2 * extruded->delta1;

FT_Set_Char_Size (*face, 0, (*face)->units_per_EM, 0, 0);
extruded->scale = GAMGI_MESA_EXTRUDED_SCALE / (*face)->units_per_EM;

extruded->vertices = NULL;
extruded->extra = NULL;

extruded->contour = gamgi_engine_darray_create(GAMGI_MESA_EXTRUDED_START, 
GAMGI_MESA_EXTRUDED_STEP, 2);
extruded->normals = gamgi_engine_darray_create(GAMGI_MESA_EXTRUDED_START, 
GAMGI_MESA_EXTRUDED_STEP, 2);

extruded->tesselator = gluNewTess ();

gluTessCallback (extruded->tesselator, GLU_TESS_VERTEX_DATA, 
(gamgi_tesselation) static_glu_vertex);
gluTessCallback (extruded->tesselator, GLU_TESS_BEGIN_DATA, 
(gamgi_tesselation) static_glu_begin);
gluTessCallback(extruded->tesselator, GLU_TESS_COMBINE_DATA, 
(gamgi_tesselation) static_glu_combine);

return extruded;
}

gamgi_bool static_extruded_remove (FT_Library freetype, 
FT_Face face, gamgi_extruded *extruded)
{
gamgi_bool valid = TRUE;

gluDeleteTess (extruded->tesselator);
gamgi_engine_darray_remove (extruded->contour);
gamgi_engine_darray_remove (extruded->normals);
free (extruded);

if (FT_Done_Face (face) != 0) valid = FALSE;
if (FT_Done_FreeType (freetype) != 0) valid = FALSE;

return valid;
}

/************* external function ***********
 *                                         *
 *          GAMGI_IO_X3D_RENDER_SOLID      *
 *                                         *
 *              draws solid text           *
 *******************************************/

void gamgi_io_x3d_render_solid (FILE *fp, gamgi_text *text, gamgi_bool *error)
{
glu_tessdata tessdata;
FT_OutlineGlyph glyph;
FT_Library freetype;
FT_Face face;
gamgi_slist *slist;
char string[GAMGI_ENGINE_LINE];
double *vert;
double depth;
int *order;
int ascii, index;
int i;

tessdata.extruded = static_extruded_create (text->font, &freetype, &face);
tessdata.vcount = 0;
tessdata.ocount = 0;
tessdata.x = tessdata.y = 0.0f;

tessdata.vert = gamgi_engine_darray_create (GAMGI_IO_X3D_TOTAL, GAMGI_IO_X3D_STEP, 3);
tessdata.order = gamgi_engine_iarray_create (GAMGI_IO_X3D_TOTAL, GAMGI_IO_X3D_STEP, 4);

for (i = 0; i < text->length; i++)
  {		
  ascii = text->string[i];

  index = FT_Get_Char_Index(face, ascii);
  FT_Load_Glyph(face, index, FT_LOAD_NO_SCALE);
  FT_Get_Glyph(face->glyph, (FT_Glyph *) (void *) &glyph);

  if (glyph->outline.flags & FT_OUTLINE_REVERSE_FILL)
    {
    tessdata.extruded->normal_x = -1;
    tessdata.extruded->normal_y = 1;
    }
  else
    {
    tessdata.extruded->normal_x = 1;
    tessdata.extruded->normal_y = -1;
    }

  depth = GAMGI_MESA_EXTRUDED_DEPTH * GAMGI_MESA_EXTRUDED_SCALE;
		
  tessdata.z = depth/2;
  x3d_font_face(glyph, &tessdata);

  tessdata.z = -depth/2;
  x3d_font_face(glyph, &tessdata);

  tessdata.z = 0.0;

  tessdata.extruded->open = FALSE;
  FT_Outline_Decompose (&glyph->outline, &tessdata.extruded->vertical, &tessdata);
  if (tessdata.extruded->open == TRUE)
    {
    gamgi_engine_darray_push (tessdata.extruded->normals, tessdata.extruded->normals->array);
    x3d_font_extrusion (&tessdata);
    gamgi_engine_darray_pop(tessdata.extruded->contour, 0);
    gamgi_engine_darray_pop(tessdata.extruded->normals, 0);
    }
  
  tessdata.x += face->glyph->advance.x * tessdata.extruded->scale;
  tessdata.y += face->glyph->advance.y * tessdata.extruded->scale;

  FT_Done_Glyph((FT_Glyph) glyph);

  for (slist = tessdata.extruded->vertices; slist != NULL; slist = slist->next)
    {
    free(slist->data);
    slist = gamgi_engine_slist_remove_start(slist);
    }
  tessdata.extruded->vertices = NULL;
  }
	
static_extruded_remove (freetype, face, tessdata.extruded);
	
gamgi_io_file_fprintf (fp, "<IndexedFaceSet solid='false' coordIndex='", error);
	
order = tessdata.order->array;
	
for (i = 0; i < tessdata.ocount-1; i++)
  {
  sprintf (string, "%d, ", order[i]);
  gamgi_io_file_fprintf(fp, string, error);
  }
sprintf (string, "%d", order[i]);
gamgi_io_file_fprintf (fp, string, error);

gamgi_io_file_fprintf (fp, "'>\n<Coordinate point='", error);
	
vert = tessdata.vert->array;
	
for (i = 0; i < tessdata.vcount / 3 - 1; i++)
  {
  sprintf (string, "%f %f %f, ", vert[i*3], vert[i*3+1], vert[i*3+2]);
  gamgi_io_file_fprintf(fp, string, error);
  }
sprintf (string, "%f %f %f", vert[i*3], vert[i*3+1], vert[i*3+2]);
gamgi_io_file_fprintf (fp, string, error);

gamgi_io_file_fprintf(fp, "'/>\n</IndexedFaceSet>\n", error);

gamgi_engine_darray_remove (tessdata.vert);
gamgi_engine_iarray_remove (tessdata.order);
}
