/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_render.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"

#include "gamgi_math_vector.h"

/*********************************************************
 * mask arrays, used to represent planes and directions, *
 * in Stereographic Projection and Reciprocal Lattice    *
 *********************************************************/

static const char unsigned cross5[30] = {
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0xff, 0xfe,
0xff, 0xfe,
0xff, 0xfe,
0xff, 0xfe,
0xff, 0xfe,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0 };

static const char unsigned chess5[30] = {
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0x07, 0xc0,
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e,
0xf8, 0x3e };

static const char unsigned cross4[24] = {
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00,
0xff, 0xf0,
0xff, 0xf0,
0xff, 0xf0,
0xff, 0xf0,
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00 };

static const char unsigned chess4[24] = {
0xf0, 0xf0,
0xf0, 0xf0,
0xf0, 0xf0,
0xf0, 0xf0,
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00,
0x0f, 0x00,
0xf0, 0xf0,
0xf0, 0xf0,
0xf0, 0xf0,
0xf0, 0xf0 };

static const char unsigned cross3[18] = {
0x1c, 0x00,
0x1c, 0x00,
0x1c, 0x00,
0xff, 0x80,
0xff, 0x80,
0xff, 0x80,
0x1c, 0x00,
0x1c, 0x00,
0x1c, 0x00 };

static const char unsigned chess3[18] = {
0xe3, 0x80,
0xe3, 0x80,
0xe3, 0x80,
0x1c, 0x00,
0x1c, 0x00,
0x1c, 0x00,
0xe3, 0x80,
0xe3, 0x80,
0xe3, 0x80 };

static const char unsigned cross2[6] = {
0x30, 
0x30, 
0xfc,
0xfc,
0x30,
0x30 };

static const char unsigned chess2[6] = {
0xcc, 
0xcc,
0x30,
0x30,
0xcc,
0xcc };

static const char unsigned cross1[3] = {
0x40,
0xe0,
0x40 };

static const char unsigned chess1[3] = {
0xa0,
0x40,
0xa0 };

static void static_mask (int n_points, double *points, 
int width, const char unsigned *bitmap)
{
float x, y, w, h;

w = h = 3.0 * width;
x = y = 1.5 * width;

glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
while (n_points-- > 0)
  { glRasterPos3dv (points + 3 * n_points);
    glBitmap (w, h, x, y, 0, 0, bitmap); }
}

void gamgi_mesa_render_points_cross (int n_points, double *points, int width)
{
glPushMatrix ();
glTranslatef (0.0, 0.0, GAMGI_MESA_OFFSET_BITMAP);

if (width > 5) width = 5;
if (width < 1) width = 1;

switch (width)
  {
  case 1:
  static_mask (n_points, points, width, cross1);
  break;

  case 2:
  static_mask (n_points, points, width, cross2);
  break;

  case 3:
  static_mask (n_points, points, width, cross3);
  break;

  case 4:
  static_mask (n_points, points, width, cross4);
  break;

  case 5:
  static_mask (n_points, points, width, cross5);
  break;
  }

glPopMatrix ();
}

void gamgi_mesa_render_points_chess (int n_points, double *points, int width)
{
glPushMatrix ();
glTranslatef (0.0, 0.0, GAMGI_MESA_OFFSET_BITMAP);

if (width > 5) width = 5;
if (width < 1) width = 1;

switch (width)
  {
  case 1:
  static_mask (n_points, points, width, chess1);
  break;

  case 2:
  static_mask (n_points, points, width, chess2);
  break;

  case 3:
  static_mask (n_points, points, width, chess3);
  break;

  case 4:
  static_mask (n_points, points, width, chess4);
  break;

  case 5:
  static_mask (n_points, points, width, chess5);
  break;
  }

glPopMatrix ();
}

void gamgi_mesa_render_points (int n_points, double *points)
{
glBegin (GL_POINTS);
while (n_points-- > 0) glVertex3dv (points + 3 * n_points);

glEnd ();
}

void gamgi_mesa_render_line_strip (int n_points, double *points)
{
glBegin (GL_LINE_STRIP);
while (n_points-- > 0) glVertex3dv (points + 3 * n_points);
glEnd ();
}

void gamgi_mesa_render_lines (int *lines, double *points)
{
int i, n_lines;

glBegin (GL_LINES);

i = 0;
n_lines = lines[i++];
while (n_lines-- > 0)
  {
  glVertex3dv (&points [3 * lines[i++]]);
  glVertex3dv (&points [3 * lines[i++]]);
  }

glEnd ();
}

void gamgi_mesa_render_loops (int *loops, double *points)
{
int i, n_loops, n_vertices;

i = 0;
n_loops = loops[i++];
while (n_loops-- > 0)
  {
  glBegin (GL_LINE_LOOP);

  n_vertices = loops[i++];
  while (n_vertices-- > 0) glVertex3dv (&points [3 * loops[i++]]);

  glEnd ();
  }

}

void gamgi_mesa_render_polygons (int *loops, double *points)
{
int i, n_loops, n_vertices;
double v1[3], v2[3], normal[3];

i = 0;
n_loops = loops[i++];
while (n_loops-- > 0)
  {
  glBegin (GL_POLYGON);

  n_vertices = loops[i++];
  gamgi_math_vector_sub (&points [3 * loops[i + 1]],
  &points [3 * loops[i]], v1);
  gamgi_math_vector_sub (&points [3 * loops[i - 1 + n_vertices]],
  &points [3 * loops[i]], v2);
  gamgi_math_vector_cross (v1, v2, normal);
  glNormal3dv(normal);

  while (n_vertices-- > 0) glVertex3dv (&points [3 * loops[i++]]);

  glEnd ();
  }

glNormal3d (0.0, 0.0, 1.0);
}

void gamgi_mesa_render_polygons_color (int *loops, 
double *points, int *paints, float *colors)
{
int i;
int n_loops, n_vertices;
double v1[3], v2[3], normal[3];

i = 0;
n_loops = loops[i++];
while (n_loops-- > 0)
  {
  /****************************
   * change color: the paints *
   * array is zero-terminated *
   ****************************/

  if (paints[0] == i)
    { glColor3fv (colors); colors += 3; paints++; }

  /***************
   * render face *
   ***************/

  glBegin (GL_POLYGON);
  n_vertices = loops[i++];

  gamgi_math_vector_sub (&points [3 * loops[i + 1]],
  &points [3 * loops[i]], v1);
  gamgi_math_vector_sub (&points [3 * loops[i + n_vertices - 1]],
  &points [3 * loops[i]], v2);
  gamgi_math_vector_cross (v1, v2, normal);
  glNormal3dv(normal);

  while (n_vertices-- > 0) glVertex3dv (&points [3 * loops[i++]]);
  glEnd ();
  }

glNormal3d (0.0, 0.0, 1.0);
}

void gamgi_mesa_render_triangles (int n_triangles, double *points)
{
double v1[3], v2[3], normal[3];
int i;

glBegin(GL_TRIANGLES);

for (i = 0; i < n_triangles; i++)
  {
  /*********************
   * get single normal *
   *********************/

  gamgi_math_vector_sub (points + 3, points, v1);
  gamgi_math_vector_sub (points + 6, points, v2);
  gamgi_math_vector_cross (v1, v2, normal);
  glNormal3dv (normal);

  /*******************
   * render triangle *
   *******************/

  glVertex3dv (points + 0);
  glVertex3dv (points + 3);
  glVertex3dv (points + 6);

  points += 9;
  }

glEnd ();
}

void gamgi_mesa_render_triangle_fan (int *loops, double *points)
{
int i, n_triangles;
double v1[3], v2[3], normal[3];

glBegin (GL_TRIANGLE_FAN);

/*********************
 * get single normal *
 *********************/

gamgi_math_vector_sub (&points [3 * loops[2]],
&points [3 * loops[1]], v1);
gamgi_math_vector_sub (&points [3 * loops[3]],
&points [3 * loops[1]], v2);
gamgi_math_vector_cross (v1, v2, normal);
glNormal3dv(normal);

/********************
 * render triangles *
 ********************/

i = 0;
n_triangles = loops[i++];
glVertex3dv (&points [3 * loops[i++]]);
glVertex3dv (&points [3 * loops[i++]]);
while (n_triangles-- > 0) glVertex3dv (&points [3 * loops[i++]]);

glEnd ();

glNormal3d (0.0, 0.0, 1.0);
}
