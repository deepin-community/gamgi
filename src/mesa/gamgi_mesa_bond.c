/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_bond.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_global.h"

void gamgi_mesa_bond_draw_line_1 (gamgi_bond *bond)
{
glCallList (GAMGI_MESA_LINE);

glLineWidth (1);
}

void gamgi_mesa_bond_draw_line_2 (gamgi_bond *bond)
{
int width = gamgi->atom->width;
float color[4], background[4];

/*****************************
 * the 2 lines are separated *
 * by one pixel lines with   *
 * the current layer color   *
 *                           *
 * the 2 lines pattern is:   *
 * width = 1 sequence = 1 1  *
 * width = 2 sequence = 2 2  *
 * width = 3 sequence = 3 3  *
 * width = 4 sequence = 4 4  *
 *****************************/

glGetFloatv (GL_CURRENT_COLOR, color);
glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

glLineWidth (2 * width + 1);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();

glColor3fv (background);

glLineWidth (1);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();

glColor3fv (color);
}

void gamgi_mesa_bond_draw_line_3 (gamgi_bond *bond)
{
int width = gamgi->atom->width;
float color[4], background[4];

/******************************
 * width = 1 sequence = 1 1 1 *
 * width = 2 sequence = 2 2 2 *
 * width = 3 sequence = 2 3 2 *
 * width = 4 sequence = 2 4 2 *
 ******************************/
   
glGetFloatv (GL_CURRENT_COLOR, color);
glGetFloatv (GL_COLOR_CLEAR_VALUE, background);

glLineWidth (width + (3 - 1 / width) * 2);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();

glColor3fv (background);

glLineWidth (width + 2);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();

glColor3fv (color);

glLineWidth (width);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();

glLineWidth (1);
}

void gamgi_mesa_bond_draw_line (gamgi_bond *bond)
{
if (bond->order <= 1.5)
  gamgi_mesa_bond_draw_line_1 (bond);
else if (bond->order <= 2.5)
  gamgi_mesa_bond_draw_line_2 (bond);
else
  gamgi_mesa_bond_draw_line_3 (bond);
}

void gamgi_mesa_bond_draw_cylinder_1 (gamgi_bond *bond)
{
glCallList (GAMGI_MESA_CYLINDER_1);
}

void gamgi_mesa_bond_draw_cylinder_3 (gamgi_bond *bond)
{
glCallList (GAMGI_MESA_CYLINDER_3);
}

void gamgi_mesa_bond_draw_cylinder_5 (gamgi_bond *bond)
{
glCallList (GAMGI_MESA_CYLINDER_5);
}

void gamgi_mesa_bond_draw_cylinder (gamgi_bond *bond)
{
if (bond->order <= 1.5)
  gamgi_mesa_bond_draw_cylinder_1 (bond);
else if (bond->order <= 2.5)
  gamgi_mesa_bond_draw_cylinder_3 (bond);
else
  gamgi_mesa_bond_draw_cylinder_5 (bond);
}
