/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_projection.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_phys.h"
#include "gamgi_math.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

void gamgi_phys_projection_wulff (double *spherical, double *planar)
{
double scale;

scale = 1.0;

/*********************************************
 * +Z: project (x, y, z) on the plane z = 0, *
 * along the line defined with (0, 0, -1)    *
 *********************************************/

if (spherical[2] > 0) scale = 1 + spherical[2];

/*********************************************
 * -Z: project (x, y, z) on the plane z = 0, *
 * along the line defined with (0, 0, +1)    *
 *********************************************/

if (spherical[2] < 0) scale = 1 - spherical[2];

planar[0] = spherical[0] / scale;
planar[1] = spherical[1] / scale;
planar[2] = 0.0;
}

void gamgi_phys_projection_schmidt (double *spherical, double *planar)
{
double scale;
double length2, length3;

length2 = spherical[0] * spherical[0] + spherical[1] * spherical[1];
length3 = 2.0;

/**********************************************
 * +Z: project (x, y, z) on the plane z = +1, *
 * so the distance to (0, 0, +1) is preserved *
 **********************************************/

if (spherical[2] > 0)
  length3 = length2 + (1 - spherical[2]) * (1 - spherical[2]);

/**********************************************
 * -Z: project (x, y, z) on the plane z = -1, *
 * so the distance to (0, 0, -1) is preserved *
 **********************************************/

if (spherical[2] < 0)
  length3 = length2 + (1 + spherical[2]) * (1 + spherical[2]);

/***********************************
 * scale down by sqrt (2) to get   *
 * a unit radius projection circle *
 ***********************************/

scale = sqrt (2) * sqrt (length2 / length3);

planar[0] = spherical[0] / scale;
planar[1] = spherical[1] / scale;
planar[2] = 0.0;
}

void gamgi_phys_projection_create (gamgi_enum model,
double **points, int *n_points, int **loops, int *n_loops)
{
int n;

/****************************************************
 * Pole: just one point; Trace: create space enough *
 * to store all the points needed to render the     *
 * horizontal circumpherence, the worst case.       *
 ****************************************************/

n = 1;
if (model == GAMGI_PHYS_TRACE) n += GAMGI_MESA_PLANE_SLICES;
*points = (double *) malloc (3 * n * sizeof (double));
*n_points = n;

/******************************************************************
 * In a stereographic projection, loops[0] is the number          *
 * of points used to draw a pole (0 or 1), or a trace (0,         *
 * GAMGI_MESA_PLANE_SLICES/2 + 1, or GAMGI_MESA_PLANE_SLICES + 1) *
 ******************************************************************/

*loops = (int *) calloc (1, sizeof (int));
*n_loops = 1;
}

void gamgi_phys_projection_pole (gamgi_net net, 
double *direction, double *points, int *loops)
{
/***************************************************
 * Vector direction has been already rotated. When *
 * it points to the half-sphere below, set the     *
 * number of loops to 0, to signal the graphic     *
 * engine that the pole should not be rendered.   *
 ***************************************************/

loops[0] = 0;
if (direction[2] < -GAMGI_MATH_TOLERANCE) return;

loops[0] = 1;
(net) (direction, points);
}

void gamgi_phys_projection_trace (gamgi_net net,
double *direction, double *points, int *loops)
{
double rotate[9];
double x[3], z[3], axis[3], xy[3], xyz[3];
double length, angle, offset;
int i;

/***************************************************
 * Vector direction has been already rotated. When *
 * it points to the half-sphere below, set the     *
 * number of loops to 0, to signal the graphic     *
 * engine that the trace should not be rendered.   *
 ***************************************************/

loops[0] = 0;
if (direction[2] < -GAMGI_MATH_TOLERANCE) return;

/***************************************
 * direction and axis are unit vectors *
 *                                     *
 *  axis lies in the horizontal plane  *
 ***************************************/

gamgi_math_vector_absolute (z, 0.0, 0.0, 1.0);
gamgi_math_vector_cross (z, direction, axis);
length = gamgi_math_vector_length (axis);
if (length > GAMGI_MATH_TOLERANCE_LENGTH)
  {
  /****************************************************
   *                draw arch above                   *
   *                                                  *
   *   the rotation matrix transforms the initial     *
   *   circumpherence trace into a spherical trace    *
   *                                                  *
   * offset defines the starting angle for the        *
   * circumpherence arch that is above after rotation *
   ****************************************************/

  /***********************
   * get rotation matrix *
   ***********************/

  gamgi_math_vector_scale (axis, axis, 1.0 / length);
  angle = acos (gamgi_math_vector_dot (z, direction));
  gamgi_math_matrix_rotation (angle * GAMGI_MATH_RAD_DEG, axis, rotate);

  /********************
   * get angle offset *
   ********************/

  gamgi_math_vector_absolute (x, 1.0, 0.0, 0.0);
  offset = acos (gamgi_math_vector_dot (x, axis));
  if (axis[1] < 0) offset = 2.0 * GAMGI_MATH_PI - offset;

  /************************
   * get number of points *
   ************************/

  loops[0] = GAMGI_MESA_PLANE_SLICES / 2 + 1;
  }
else
  {
  /**********************************
   * draw horizontal circumpherence *
   *  (direction is paralel to z)   *
   **********************************/

  gamgi_math_matrix_unit (rotate);
  offset = 0.0;
  loops[0] = GAMGI_MESA_PLANE_SLICES + 1;
  }

/*******************************
 * 1) get circumpherence trace *
 * 2) get spherical trace      *
 * 3) get projected trace      *
 *******************************/

angle = 2.0 * GAMGI_MATH_PI / GAMGI_MESA_PLANE_SLICES;
for (i = 0; i < loops[0]; i++)
  {
  gamgi_math_vector_absolute (xy, cos (offset + i * angle), 
  sin (offset + i * angle), 0.0);

  gamgi_math_matrix_vector (rotate, xy, xyz);
  (net) (xyz, points + 3*i);
  }

}
