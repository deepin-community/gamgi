/******************************************
 *
 * $GAMGI/src/math/gamgi_math_cell.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"

void gamgi_math_cell_range_half (double *in, double *out)
{
/*********************************
 * convert to range ]-0.5, +0.5[ *
 *********************************/

out[0] = in[0];
if (out[0] < -0.5) out[0] += 1.0;
if (out[0] > +0.5) out[0] -= 1.0;

out[1] = in[1];
if (out[1] < -0.5) out[1] += 1.0;
if (out[1] > +0.5) out[1] -= 1.0;

out[2] = in[2];
if (out[2] < -0.5) out[2] += 1.0;
if (out[2] > +0.5) out[2] -= 1.0;
}

void gamgi_math_cell_range_positive (double *in, double *out)
{
/****************************
 * convert to range [0, +1[ *
 ****************************/

out[0] = in[0]; if (out[0] < 0.0) out[0] += 1.0;
out[1] = in[1]; if (out[1] < 0.0) out[1] += 1.0;
out[2] = in[2]; if (out[2] < 0.0) out[2] += 1.0;
}

/*********** external function **********
 *                                      *
 *         GAMGI_MATH_CELL_VECTORS      *
 *                                      *
 ****************************************/

void gamgi_math_cell_vectors (double a, double b, double c, 
double ab, double ac, double bc, double *a1, double *a2, double *a3)
{
ab *= GAMGI_MATH_DEG_RAD;
ac *= GAMGI_MATH_DEG_RAD;
bc *= GAMGI_MATH_DEG_RAD;

a1[0] = a;
a1[1] = 0.0;
a1[2] = 0.0;

a2[0] = b * cos(ab);
a2[1] = b * sin(ab);
a2[2] = 0.0;

a3[0] = c * cos(ac);
a3[1] = c * (cos(bc) - cos(ab) * cos(ac)) / sin(ab);
a3[2] = c * sqrt (pow (sin(ac), 2) - pow ((cos(bc) - cos(ab) * cos(ac)) / sin(ab), 2));
}

void gamgi_math_cell_parameters (double *a1, double *a2, double *a3,
double *a, double *b, double *c, double *ab, double *ac, double *bc)
{
double cosine;

*a = gamgi_math_vector_length (a1);
*b = gamgi_math_vector_length (a2);
*c = gamgi_math_vector_length (a3);

cosine = gamgi_math_vector_dot (a1, a2) / ((*a) * (*b));
if (cosine < -1.0) cosine = -1.0;
if (cosine > +1.0) cosine = +1.0;
*ab = GAMGI_MATH_RAD_DEG * acos (cosine);

cosine = gamgi_math_vector_dot (a1, a3) / ((*a) * (*c));
if (cosine < -1.0) cosine = -1.0;
if (cosine > +1.0) cosine = +1.0;
*ac = GAMGI_MATH_RAD_DEG * acos (cosine);

cosine = gamgi_math_vector_dot (a2, a3) / ((*b) * (*c));
if (cosine < -1.0) cosine = -1.0;
if (cosine > +1.0) cosine = +1.0;
*bc = GAMGI_MATH_RAD_DEG * acos (cosine);
}

void gamgi_math_cell_primitive (gamgi_enum lattice,
double *a1, double *a2, double *a3, double *p1, double *p2, double *p3)
{
switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I: 
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  p1[0] = (+ a1[0] - a2[0] + a3[0]) / 2;
  p1[1] = (+ a1[1] - a2[1] + a3[1]) / 2;
  p1[2] = (+ a1[2] - a2[2] + a3[2]) / 2;

  p2[0] = (+ a1[0] + a2[0] - a3[0]) / 2;
  p2[1] = (+ a1[1] + a2[1] - a3[1]) / 2;
  p2[2] = (+ a1[2] + a2[2] - a3[2]) / 2;

  p3[0] = (- a1[0] + a2[0] + a3[0]) / 2;
  p3[1] = (- a1[1] + a2[1] + a3[1]) / 2;
  p3[2] = (- a1[2] + a2[2] + a3[2]) / 2;
  break;
 
  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  p1[0] = (a1[0] + a3[0]) / 2;
  p1[1] = (a1[1] + a3[1]) / 2;
  p1[2] = (a1[2] + a3[2]) / 2;

  p2[0] = (a1[0] + a2[0]) / 2;
  p2[1] = (a1[1] + a2[1]) / 2;
  p2[2] = (a1[2] + a2[2]) / 2;

  p3[0] = (a2[0] + a3[0]) / 2;
  p3[1] = (a2[1] + a3[1]) / 2;
  p3[2] = (a2[2] + a3[2]) / 2;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  p1[0] = (a1[0] - a2[0]) / 2;
  p1[1] = (a1[1] - a2[1]) / 2;
  p1[2] = (a1[2] - a2[2]) / 2;

  p2[0] = (a1[0] + a2[0]) / 2;
  p2[1] = (a1[1] + a2[1]) / 2;
  p2[2] = (a1[2] + a2[2]) / 2;

  p3[0] = a3[0];
  p3[1] = a3[1];
  p3[2] = a3[2];
  break;
  
  case GAMGI_PHYS_HEXAGONAL_R:
  p1[0] = (2 * a1[0] + a2[0] + a3[0]) / 3;
  p1[1] = (2 * a1[1] + a2[1] + a3[1]) / 3;
  p1[2] = (2 * a1[2] + a2[2] + a3[2]) / 3;

  p2[0] = (-a1[0] + a2[0] + a3[0]) / 3;
  p2[1] = (-a1[1] + a2[1] + a3[1]) / 3;
  p2[2] = (-a1[2] + a2[2] + a3[2]) / 3;

  p3[0] = (-a1[0] - 2 * a2[0] + a3[0]) / 3;
  p3[1] = (-a1[1] - 2 * a2[1] + a3[1]) / 3;
  p3[2] = (-a1[2] - 2 * a2[2] + a3[2]) / 3;
  break;

  default:
  p1[0] = a1[0];
  p1[1] = a1[1];
  p1[2] = a1[2];

  p2[0] = a2[0];
  p2[1] = a2[1];
  p2[2] = a2[2];

  p3[0] = a3[0];
  p3[1] = a3[1];
  p3[2] = a3[2];
  break;
  }
}

void gamgi_math_cell_conventional (gamgi_enum lattice, 
double *p1, double *p2, double *p3, double *a1, double *a2, double *a3)
{
switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  a1[0] = p1[0] + p2[0];
  a1[1] = p1[1] + p2[1];
  a1[2] = p1[2] + p2[2];

  a2[0] = p2[0] + p3[0];
  a2[1] = p2[1] + p3[1];
  a2[2] = p2[2] + p3[2];

  a3[0] = p1[0] + p3[0];
  a3[1] = p1[1] + p3[1];
  a3[2] = p1[2] + p3[2];
  break;

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  a1[0] = + p1[0] + p2[0] - p3[0];
  a1[1] = + p1[1] + p2[1] - p3[1];
  a1[2] = + p1[2] + p2[2] - p3[2];

  a2[0] = - p1[0] + p2[0] + p3[0];
  a2[1] = - p1[1] + p2[1] + p3[1];
  a2[2] = - p1[2] + p2[2] + p3[2];

  a3[0] = + p1[0] - p2[0] + p3[0];
  a3[1] = + p1[1] - p2[1] + p3[1];
  a3[2] = + p1[2] - p2[2] + p3[2];
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  a1[0] = + p1[0] + p2[0];
  a1[1] = + p1[1] + p2[1];
  a1[2] = + p1[2] + p2[2];

  a2[0] = - p1[0] + p2[0];
  a2[1] = - p1[1] + p2[1];
  a2[2] = - p1[2] + p2[2];

  a3[0] = p3[0];
  a3[1] = p3[1];
  a3[2] = p3[2];
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  a1[0] = p1[0] - p2[0];
  a1[1] = p1[1] - p2[1];
  a1[2] = p1[2] - p2[2];

  a2[0] = p2[0] - p3[0];
  a2[1] = p2[1] - p3[1];
  a2[2] = p2[2] - p3[2];

  a3[0] = p1[0] + p2[0] + p3[0];
  a3[1] = p1[1] + p2[1] + p3[1];
  a3[2] = p1[2] + p2[2] + p3[2];
  break;
  
  default:
  a1[0] = p1[0];
  a1[1] = p1[1];
  a1[2] = p1[2];

  a2[0] = p2[0];
  a2[1] = p2[1];
  a2[2] = p2[2];

  a3[0] = p3[0];
  a3[1] = p3[1];
  a3[2] = p3[2];
  break;
  }
}

void gamgi_math_cell_offset (gamgi_cell *cell, double *offset)
{
gamgi_math_vector_translation (cell->o1, cell->o2, cell->o3, 
cell->p1, cell->p2, cell->p3, offset);
}

void gamgi_math_cell_center (gamgi_cell *cell, double *center)
{
int n;

gamgi_math_vector_zero (center);

/********************************
 * at least one node must exist *
 ********************************/

for (n = 0; n < cell->n_nodes; n++)
  {
  center[0] += cell->points[3*n + 0];
  center[1] += cell->points[3*n + 1];
  center[2] += cell->points[3*n + 2];
  }

gamgi_math_vector_scale (center, center, 1.0 / cell->n_nodes);
}

void gamgi_math_cell_vector_rotate (gamgi_cell *cell, 
double *pole, double *normal)
{
double rotate[9];

/***********************************************
 * rotate vector according to cell orientation *
 ***********************************************/

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_matrix_vector (rotate, pole, normal);

/****************************
 * normalize rotated vector *
 ****************************/

gamgi_math_vector_normal (normal);
}
