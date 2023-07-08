/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_axes.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_cell.h"

static void static_sphere (float size)
{
glPushMatrix ();
glScalef (size, size, size);
glCallList (GAMGI_MESA_SPHERE);
glPopMatrix ();
}

static void static_axis (double *v, float size)
{
double small[3], big[3];
double length, angle;
double height;

/*************************************************************
 * cylinder height = axis length - radius of two end spheres *
 *                                                           *
 * get angle to rotate cylinder: 0 and 180 degrees           *
 * will be handled as special cases. Initially cylinder      *
 * is built from xy plane along +z axis.                     *
 *                                                           *
 * scale vector v from length to size: vector small          *
 * scale vector v from length to height: vector big          *
 *                                                           *
 * translate small vector to draw first sphere,              *
 * then translate big vector to draw second sphere           *
 *************************************************************/
 
length = gamgi_math_vector_length (v);
angle = acos (v[2] / length) * GAMGI_MATH_RAD_DEG;
height = length - 2 * size;
gamgi_math_vector_scale (v, small, size / length);
gamgi_math_vector_scale (v, big, height / length);

/*****************************************************
 * save initial position, translate, draw 1st sphere *
 *****************************************************/

glPushMatrix ();
glTranslatef (small[0], small[1], small[2]);
static_sphere (size);

/****************************************************
 * save position, rotate cylinder, restore position *
 ****************************************************/

glPushMatrix ();
if (length - fabs (v[2]) > GAMGI_MATH_TOLERANCE_LENGTH)
  glRotatef (angle, -v[1], v[0], 0.0);
else
  glRotatef (angle, 1.0, 0.0, 0.0);
glScalef (size, size, height);
glCallList (GAMGI_MESA_CYLINDER_1);
glPopMatrix ();

/********************************************************
 * translate, draw 2nd sphere, restore initial position *
 ********************************************************/

glTranslatef (big[0], big[1], big[2]);
static_sphere (size);
glPopMatrix ();
}

void gamgi_mesa_axes (double *origin,
double *a, double *b, double *c, float size)
{
glTranslatef (origin[0], origin[1], origin[2]);

/*********************************
 * draw axes along a,b,c vectors *
 *********************************/

static_axis (a, size);
static_axis (b, size);
static_axis (c, size);
}

void gamgi_mesa_axes_color (double *origin,
double *a, double *b, double *c, float size)
{
glTranslatef (origin[0], origin[1], origin[2]);

/*********************************
 * draw axes along a,b,c vectors *
 *********************************/

glColor3f (GAMGI_MESA_AXIS_1_R, GAMGI_MESA_AXIS_1_G, GAMGI_MESA_AXIS_1_B);
static_axis (a, size);

glColor3f (GAMGI_MESA_AXIS_2_R, GAMGI_MESA_AXIS_2_G, GAMGI_MESA_AXIS_2_B);
static_axis (b, size);

glColor3f (GAMGI_MESA_AXIS_3_R, GAMGI_MESA_AXIS_3_G, GAMGI_MESA_AXIS_3_B);
static_axis (c, size);
}

void gamgi_mesa_axes_orbital (gamgi_orbital *orbital,
gamgi_bool color, float size)
{
double offset[3], v1[3], v2[3], v3[3];
double radius, length;

/************************************************************
 * when a frame is present axes are positioned at the lower *
 * frame corner, otherwise they are placed at the origin    *
 ************************************************************/

radius = orbital->radius; 
if (orbital->frame == TRUE)
  gamgi_math_vector_absolute (offset, -radius, -radius, -radius); 
else
  gamgi_math_vector_zero (offset);

/*****************************************************
 * axes length can be 1.0, 0.5292 (the Bohr radius), *
 * the radius (frame no) or 2 * radius (frame yes)   *
 *****************************************************/

length = 1.0;
if (orbital->axes == GAMGI_CHEM_BOHR) length = GAMGI_CHEM_A0;
else if (orbital->axes == GAMGI_CHEM_RADIUS)
  { length = (orbital->frame == TRUE) ? 2 * radius : radius; }

gamgi_math_vector_absolute (v1, length, 0.0, 0.0);
gamgi_math_vector_absolute (v2, 0.0, length, 0.0);
gamgi_math_vector_absolute (v3, 0.0, 0.0, length);

/************************************************************
 * TRUE is for mesa_draw, FALSE is for mesa_pick, mesa_grab *
 ************************************************************/

if (color == TRUE)
  gamgi_mesa_axes_color (offset, v1, v2, v3, size);
else
  gamgi_mesa_axes (offset, v1, v2, v3, size);
}

void gamgi_mesa_axes_cell (gamgi_cell *cell, gamgi_bool color, float size)
{
double *v1, *v2, *v3;
double offset[3];

gamgi_math_cell_offset (cell, offset);

/**************************************************
 * build axes for conventional or primitive cells *
 **************************************************/

if (cell->axes_vectors == GAMGI_PHYS_CONVENTIONAL)
  { v1 = cell->a1; v2 = cell->a2; v3 = cell->a3; }
else
  { v1 = cell->p1; v2 = cell->p2; v3 = cell->p3; }

/************************************************************
 * TRUE is for mesa_draw, FALSE is for mesa_pick, mesa_grab *
 ************************************************************/

if (color == TRUE)
  gamgi_mesa_axes_color (offset, v1, v2, v3, size);
else
  gamgi_mesa_axes (offset, v1, v2, v3, size);
}

void gamgi_mesa_axes_layer (gamgi_layer *layer, gamgi_bool color, float size)
{
double offset[3], v1[3], v2[3], v3[3];

gamgi_math_vector_zero (offset);

gamgi_math_vector_absolute (v1, 1.0, 0.0, 0.0);
gamgi_math_vector_absolute (v2, 0.0, 1.0, 0.0);
gamgi_math_vector_absolute (v3, 0.0, 0.0, 1.0);

/*************************************************
 * TRUE is for mesa_draw, FALSE is for mesa_pick *
 * (mesa_grab does not render layer axes)        *
 *************************************************/

if (color == TRUE)
  gamgi_mesa_axes_color (offset, v1, v2, v3, size);
else
  gamgi_mesa_axes (offset, v1, v2, v3, size);
}
