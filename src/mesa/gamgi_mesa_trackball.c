/*
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */

/***********************************************
 * Code taken from the package GTKGLAREA-1.2.1 *
 *                           by Carlos Pereira *
 ***********************************************/

#include "gamgi_engine.h"
#include "gamgi_mesa.h"

#include "gamgi_math_vector.h"

/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 * the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 */

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */

double gamgi_mesa_trackball_project (double r, 
double x, double y)
{
double d, t, z;

d = sqrt (x*x + y*y);
if (d < r * 0.70710678118654752440)
  { 
  /*****************
   * Inside sphere *
   *****************/

  z = sqrt (r*r - d*d);
  }
else
  {
  /****************
   * On hyperbola *
   ****************/

  t = r / 1.41421356237309504880;
  z = t*t / d;
  }

return z;
}

/*
 * Simulate a track-ball. Pass the x and y coordinates of the last and 
 * current positions of the mouse, scaled so they are from (-1.0 ... 1.0). 
 * It is assumed that the arguments to this routine are in the range 
 * (-1.0 ... 1.0). The resulting rotation is returned as a quaternion rotation 
 * in the first paramater.
 *
 * Project the points onto the virtual trackball, then figure out the axis 
 * of rotation, which is the cross product of P1 P2 and O P1 (O is the center 
 * of the ball, 0, 0, 0). Note: This is a deformed trackball: is a trackball 
 * in the center, but is deformed into a hyperbolic sheet of rotation away from 
 * the center. This particular function was chosen after trying out several 
 * variations.
 */

void gamgi_mesa_trackball (double p1x, double p1y, double p2x, double p2y, 
double *axis, double *angle)
{
double p1[3], p2[3], d[3];
double t;

if (p1x == p2x && p1y == p2y)
  {
  /*****************
   * Zero rotation *
   *****************/

  gamgi_math_vector_zero (axis);
  axis[2] = 1.0;
  *angle = 0.0;
  return;
  }

/**********************************************
 * First, figure out z-coordinates for        *
 * projection of P1 and P2 to deformed sphere *
 **********************************************/

gamgi_math_vector_absolute (p1, p1x, p1y, 
gamgi_mesa_trackball_project (GAMGI_MESA_ROTATE_SPEED, p1x, p1y));
gamgi_math_vector_absolute (p2, p2x, p2y, 
gamgi_mesa_trackball_project (GAMGI_MESA_ROTATE_SPEED, p2x, p2y));

/***********************************************
 * Now, we want the cross product of P1 and P2 *
 ***********************************************/

gamgi_math_vector_cross (p2, p1, axis);

/***************************************************
 * Figure out how much to rotate around that axis. *
 ***************************************************/

gamgi_math_vector_sub (p1, p2, d);
t = gamgi_math_vector_length (d) / (2.0 * GAMGI_MESA_ROTATE_SPEED);

/************************************************
 * Avoid problems with out-of-control values... *
 ************************************************/

if (t > 1.0) t = 1.0;
if (t < -1.0) t = -1.0;
*angle = 2.0 * asin (t);
}
