/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_lists.c
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
#include "gamgi_global.h"

#include "gamgi_math_vector.h"

void gamgi_mesa_lists_cross (float width)
{
glDeleteLists (GAMGI_MESA_CROSS, 1);
glNewList (GAMGI_MESA_CROSS, GL_COMPILE);
glLineWidth (width);
glBegin (GL_LINES);
glVertex3f (-0.5, 0.0, 0.0);
glVertex3f (0.5, 0.0, 0.0);
glVertex3f (0.0, -0.5, 0.0);
glVertex3f (0.0, 0.5, 0.0);
glVertex3f (0.0, 0.0, -0.5);
glVertex3f (0.0, 0.0, 0.5);
glEnd ();
glLineWidth (1.0);
glEndList ();
}

void gamgi_mesa_lists_sphere (int slices, int stacks)
{
GLUquadricObj *sphere;

sphere = gluNewQuadric ();
gluQuadricDrawStyle (sphere, GLU_FILL);
gluQuadricOrientation (sphere, GLU_OUTSIDE);
gluQuadricNormals (sphere, GLU_SMOOTH);

glDeleteLists (GAMGI_MESA_SPHERE, 1);
glNewList (GAMGI_MESA_SPHERE, GL_COMPILE);
gluSphere (sphere, 1.0, slices, stacks);
glEndList ();

gluDeleteQuadric (sphere);
}

void gamgi_mesa_lists_line (float width)
{
glDeleteLists (GAMGI_MESA_LINE, 1);
glNewList (GAMGI_MESA_LINE, GL_COMPILE);
glLineWidth (width);
glBegin (GL_LINES);
glVertex3f (0.0, 0.0, 0.0);
glVertex3f (0.0, 0.0, 1.0);
glEnd ();
glLineWidth (1.0);
glEndList ();
}

void gamgi_mesa_lists_cylinder_1 (int slices, int stacks)
{
GLUquadricObj *quadric;

quadric = gluNewQuadric ();

gluQuadricDrawStyle (quadric, GLU_FILL);
gluQuadricNormals (quadric, GLU_SMOOTH);

glDeleteLists (GAMGI_MESA_CYLINDER_1, 1);
glNewList (GAMGI_MESA_CYLINDER_1, GL_COMPILE);

gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
glEndList ();

gluDeleteQuadric (quadric);
}

void gamgi_mesa_lists_cylinder_3 (int slices, int stacks)
{
GLUquadricObj *quadric;

quadric = gluNewQuadric ();
gluQuadricDrawStyle (quadric, GLU_FILL);
gluQuadricNormals (quadric, GLU_SMOOTH);

glDeleteLists (GAMGI_MESA_CYLINDER_3, 1);
glNewList (GAMGI_MESA_CYLINDER_3, GL_COMPILE);

gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);

glScalef (0.5, 0.5, 1.0);

glTranslatef (2.0, 0, 0);
gluQuadricOrientation (quadric, GLU_INSIDE);
gluDisk (quadric, 0.0, 1.0, slices, 1);
gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
glTranslatef (0.0, 0, 1.0);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glTranslatef (-4.0, 0, 0);
gluDisk (quadric, 0.0, 1.0, slices, 1);
glTranslatef (0.0, 0, -1.0);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
gluQuadricOrientation (quadric, GLU_INSIDE);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glNormal3d (0.0, 0.0, 1.0);
glEndList ();

gluDeleteQuadric (quadric);
}

void gamgi_mesa_lists_cylinder_5 (int slices, int stacks)
{
GLUquadricObj *quadric;

quadric = gluNewQuadric ();
gluQuadricDrawStyle (quadric, GLU_FILL);
gluQuadricNormals (quadric, GLU_SMOOTH);

glDeleteLists (GAMGI_MESA_CYLINDER_5, 1);
glNewList (GAMGI_MESA_CYLINDER_5, GL_COMPILE);

gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);

glScalef (0.5, 0.5, 1.0);

glTranslatef (2.0, 0, 0);
gluQuadricOrientation (quadric, GLU_INSIDE);
gluDisk (quadric, 0.0, 1.0, slices, 1);
gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
glTranslatef (0.0, 0, 1.0);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glTranslatef (-2.0, 2.0, 0);
gluDisk (quadric, 0.0, 1.0, slices, 1);
glTranslatef (0.0, 0, -1.0);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
gluQuadricOrientation (quadric, GLU_INSIDE);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glTranslatef (-2.0, -2.0, 0);
gluDisk (quadric, 0.0, 1.0, slices, 1);
gluQuadricOrientation (quadric, GLU_OUTSIDE);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
glTranslatef (0.0, 0, 1.0);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glTranslatef (2.0, -2.0, 0);
gluDisk (quadric, 0.0, 1.0, slices, 1);
glTranslatef (0.0, 0, -1.0);
gluCylinder (quadric, 1.0, 1.0, 1.0, slices, stacks);
gluQuadricOrientation (quadric, GLU_INSIDE);
gluDisk (quadric, 0.0, 1.0, slices, 1);

glNormal3d (0.0, 0.0, 1.0);
glEndList ();

gluDeleteQuadric (quadric);
}

void gamgi_mesa_lists_create (void)
{
/*********************************************
 * pre-compiled shared (between top windows) *
 * OpenGL lists to draw objects more quickly *
 *********************************************/

/*********
 * atoms *
 *********/

gamgi_mesa_lists_cross (gamgi->atom->width);
gamgi_mesa_lists_sphere (gamgi->atom->slices, gamgi->atom->slices);

/*********
 * bonds *
 *********/

gamgi_mesa_lists_line (gamgi->atom->width);
gamgi_mesa_lists_cylinder_1 (gamgi->atom->slices, 1);
gamgi_mesa_lists_cylinder_3 (gamgi->atom->slices, 1);
gamgi_mesa_lists_cylinder_5 (gamgi->atom->slices, 1);
}

void gamgi_mesa_lists_remove (void)
{
/*****************************************
 * free OpenGL pre-compiled shared lists *
 *****************************************/

glDeleteLists (GAMGI_MESA_CROSS, 1);
glDeleteLists (GAMGI_MESA_SPHERE, 1);
glDeleteLists (GAMGI_MESA_LINE, 1);
glDeleteLists (GAMGI_MESA_CYLINDER_1, 1);
glDeleteLists (GAMGI_MESA_CYLINDER_3, 1);
glDeleteLists (GAMGI_MESA_CYLINDER_5, 1);
}
