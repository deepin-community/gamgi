/******************************************
 *
 * $GAMGI/src/math/gamgi_math_euler.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

/*********** external function **********
 *                                      *
 *                                      *
 ****************************************/

void gamgi_math_euler_from_matrix (double *rotate, double *angle)
{
double rotate_aux[9], axis[3];
double x[3], z[3];
double x_lab[3], y_lab[3], z_lab[3];
double x_aux[3], y_aux[3], z_aux[3];
double length;

/************************
 * set vectors:         *
 * - initial x,y,z axes *
 * - rotated x,z axes   *
 * - projected z axis   *
 ************************/

gamgi_math_vector_absolute (x_lab, 1.0, 0.0, 0.0);
gamgi_math_vector_absolute (y_lab, 0.0, 1.0, 0.0);
gamgi_math_vector_absolute (z_lab, 0.0, 0.0, 1.0);

gamgi_math_vector_absolute (x, rotate[0], rotate[3], rotate[6]);
gamgi_math_vector_absolute (z, rotate[2], rotate[5], rotate[8]);

gamgi_math_vector_absolute (z_aux, z[0], z[1], 0.0);

/********************
 * angle[0] = theta *
 * angle[1] = phi   *
 * angle[2] = psi   *
 ********************/

angle[0] = gamgi_math_vector_angle (z_lab, z);

gamgi_math_vector_cross (z_lab, z, axis);
length = gamgi_math_vector_length (axis);
if (length < GAMGI_MATH_TOLERANCE_LENGTH)
  {
  /***************************
   * z_lab and z are aligned *
   ***************************/

  angle[1] = gamgi_math_vector_angle (x_lab, x);
  if (x[1] < 0) angle[1] = 360 - angle[1]; 
  angle[2] = 0.0;
  }
else
  {
  angle[1] = gamgi_math_vector_angle (x_lab, z_aux);
  if (gamgi_math_vector_dot (y_lab, z_aux) < 0) angle[1] = 360 - angle[1]; 

  gamgi_math_vector_scale (axis, axis, 1.0/length);
  gamgi_math_matrix_rotation (90.0, axis, rotate_aux);
  gamgi_math_matrix_vector (rotate_aux, z, x_aux);
  angle[2] = gamgi_math_vector_angle (x_aux, x);
  gamgi_math_vector_cross (z, x_aux, y_aux);
  if (gamgi_math_vector_dot (x, y_aux) < 0) angle[2] = 360 - angle[2];
  }

}

void gamgi_math_euler_to_matrix (double *angle, double *rotate321)
{
double axis_old[3], axis_new[3];
double rotate1[9], rotate2[9], rotate3[9], rotate21[9];

/*********************************************
 * get rotation matrix for first Euler angle *
 *********************************************/

gamgi_math_vector_absolute (axis_new, 0.0, 1.0, 0.0);
gamgi_math_matrix_rotation (angle[0], axis_new, rotate1);

/**********************************************
 * get rotation matrix for second Euler angle *
 **********************************************/

gamgi_math_vector_absolute (axis_new, 0.0, 0.0, 1.0);
gamgi_math_matrix_rotation (angle[1], axis_new, rotate2);

/*********************************************
 * get rotation matrix for third Euler angle *
 *********************************************/

gamgi_math_matrix_matrix (rotate2, rotate1, rotate21);
gamgi_math_vector_absolute (axis_old, 0.0, 0.0, 1.0);
gamgi_math_matrix_vector (rotate21, axis_old, axis_new);
gamgi_math_matrix_rotation (angle[2], axis_new, rotate3);

/******************************
 * get global rotation matrix *
 ******************************/

gamgi_math_matrix_matrix (rotate3, rotate21, rotate321);
}
