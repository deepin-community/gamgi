/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_atom.c
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

void gamgi_mesa_atom_draw_cross (gamgi_atom *atom)
{
if (atom->bond_start == NULL) glCallList (GAMGI_MESA_CROSS);
else 
  {
  glPointSize (gamgi->atom->width);
  glBegin (GL_POINTS);
  glVertex3d (0.0, 0.0, 0.0);
  glEnd ();
  glPointSize (1.0);
  }
}

void gamgi_mesa_atom_draw_sphere (gamgi_atom *atom)
{
glCallList (GAMGI_MESA_SPHERE);
glNormal3d (0.0, 0.0, 1.0);
}
