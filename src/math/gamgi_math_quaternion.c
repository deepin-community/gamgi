/*******************************************
 *
 * $GAMGI/src/math/gamgi_math_quaternion.c
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

/****************************************
 * quaternions are defined in GAMGI as  *
 * q[0]i + q[1]j + q[2]k + q[3] so the  *
 * real part is the last coordinate     *
 ****************************************/

/************ external function *********
 *                                      *
 *                                      *
 ****************************************/

/*
 * Quaternions always obey: a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitude
 * will renormalize them.
 *
 * See the following for more information on quaternions:
 *
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */

void gamgi_math_quaternion_normalize (double q[4])
{
int i;
double norm;

norm = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
for (i = 0; i < 4; i++) q[i] /= norm;
}

/************ external function *********
 *                                      *
 *                                      *
 ****************************************/

/*
 * Given two quaternions, add them together to get a third quaternion.
 * Adding quaternions to get a compound rotation is analagous to adding
 * translations to get a compound translation.  When incrementally
 * adding rotations, the first argument here should be the new
 * rotation, the second and third the total rotation (which will be
 * over-written with the resulting new total rotation).
 *
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into out.
 *
 * This routine also normalizes the result every SGI_RENORMCOUNT times
 * it is called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as out (or each other).
 */

void gamgi_math_quaternion_add (double q1[4], double q2[4], double out[4])
{
static int count = 0;
double t1[4], t2[4], t3[4], tf[4];

gamgi_math_vector_copy (q1,t1);
gamgi_math_vector_scale (t1, t1, q2[3]);

gamgi_math_vector_copy (q2,t2);
gamgi_math_vector_scale (t2, t2, q1[3]);

gamgi_math_vector_cross (q1,q2,t3);
gamgi_math_vector_add (t1,t2,tf);
gamgi_math_vector_add (t3,tf,tf);
tf[3] = q1[3] * q2[3] - gamgi_math_vector_dot (q1,q2);

out[0] = tf[0];
out[1] = tf[1];
out[2] = tf[2];
out[3] = tf[3];

if (++count > GAMGI_MATH_RENORM_COUNT)
  {
  count = 0;
  gamgi_math_quaternion_normalize (out);
  }
}

/************ external function *********
 *                                      *
 *                                      *
 ****************************************/

/*
 * This function computes a quaternion based on an axis (defined by
 * the given vector) and an angle about which to rotate. The angle
 * is expressed in radians. The result is put into the third argument.
 */

void gamgi_math_quaternion_from_axis (double phi, double axis[3], double quat[4])
{
if (phi == 0.0)
  { quat[0] = 0.0; quat[1] = 0.0; quat[2] = 0.0; quat[3] = 1.0; }
else
  {
  gamgi_math_vector_normal (axis);
  gamgi_math_vector_copy (axis, quat);
  gamgi_math_vector_scale (quat, quat, sin (phi / 2.0));
  quat[3] = cos (phi / 2.0);
  }
}

void gamgi_math_quaternion_to_axis (double quat[4], double *phi, double axis[3])
{
double angle, scale;

angle = acos (quat[3]);
*phi = 2.0 * angle;

if (angle == 0.0)
  { axis[0] = 1; axis[1] = 0; axis[2] = 0; }
else
  {
  scale = sin (angle);
  axis[0] = quat[0] / scale;
  axis[1] = quat[1] / scale;
  axis[2] = quat[2] / scale;
  }
}

void gamgi_math_quaternion_from_matrix (double m[9], double q[4])
{
double trace;

trace = sqrt (1.0 + m[0] + m[4] + m[8]);

if (trace >= 1.0)
  {
  q[3] = trace / 2.0;
  q[0] = (m[7] - m[5]) / (2.0 * trace);
  q[1] = (m[2] - m[6]) / (2.0 * trace);
  q[2] = (m[3] - m[1]) / (2.0 * trace);
  return;
  }

if (m[0] >= m[4] && m[0] >= m[8])
  {
  q[0] = sqrt (1.0 + m[0] - m[4] - m[8]) / 2.0;
  q[1] = (m[1] + m[3]) / (4.0 * q[0]);
  q[2] = (m[2] + m[6]) / (4.0 * q[0]);
  q[3] = (m[7] - m[5]) / (4.0 * q[0]);
  return;
  }

if (m[4] >= m[0] && m[4] >= m[8])
  {
  q[1] = sqrt (1.0 + m[4] - m[0] - m[8]) / 2.0;
  q[0] = (m[1] + m[3]) / (4.0 * q[1]);
  q[2] = (m[5] + m[7]) / (4.0 * q[1]);
  q[3] = (m[2] - m[6]) / (4.0 * q[1]); 
  return;
  }

if (m[8] >= m[0] && m[8] >= m[4])
  {
  q[2] = sqrt (1.0 + m[8] - m[0] - m[4]) / 2.0;
  q[0] = (m[2] + m[6]) / (4.0 * q[2]);
  q[1] = (m[5] + m[7]) / (4.0 * q[2]);
  q[3] = (m[3] - m[1]) / (4.0 * q[2]);
  return;
  }
}

void gamgi_math_quaternion_to_matrix (double q[4], double m[9])
{
m[0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
m[1] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
m[2] = 2.0 * (q[2] * q[0] + q[1] * q[3]);

m[3] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
m[4] = 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
m[5] = 2.0 * (q[1] * q[2] - q[0] * q[3]);

m[6] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
m[7] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
m[8] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
}

/************ external function *********
 *                                      *
 *                                      *
 ****************************************/

void gamgi_math_quaternion_to_matrix4 (double q[4], double m[16])
{
/****************************************
 * (in OpenGL) m[i][j] = m[j][i] (in C) *
 ****************************************/

m[0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
m[1] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
m[2] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
m[3] = 0.0;

m[4] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
m[5] = 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
m[6] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
m[7] = 0.0;

m[8] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
m[9] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
m[10] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
m[11] = 0.0;

m[12] = 0.0;
m[13] = 0.0;
m[14] = 0.0;
m[15] = 1.0;
}
