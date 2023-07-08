/********************************************
 *
 * $GAMGI/src/math/gamgi_math_diophantine.c
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

int gamgi_math_diophantine_gcd (int a, int b)
{
/*****************************
 * recursive Euler algorithm *
 *****************************/

if (b == 0) return a;
else return (gamgi_math_diophantine_gcd (b, a % b));
}

gamgi_bool gamgi_math_diophantine_soluble (int a, int b, int c, int d)
{
int gcd;

gcd = gamgi_math_diophantine_gcd (a, b);
gcd = gamgi_math_diophantine_gcd (gcd, c);

if (gcd == 0) { if (d == 0) return TRUE; else return FALSE; }

if (d % gcd == 0) return TRUE;

return FALSE;
}

void gamgi_math_diophantine_gcd_extended (int a, int b, int *x, int *y, int *gcd)
{
int quotient, temp;
int x0 = 1;
int y0 = 0;
int x1 = 0;
int y1 = 1;

/****************************
 * extended Euler algorithm *
 ****************************/

while (b != 0)
  {
  temp = b;
  quotient = a / b;
  b = a % b;
  a = temp;

  temp = x1;
  x1 = x0 - quotient * x1;
  x0 = temp;

  temp = y1;
  y1 = y0 - quotient * y1;
  y0 = temp;
  }

*x = x0;
*y = y0;
*gcd = a;
}

void gamgi_math_diophantine_swap (int *old1, int *old2, 
int *new1, int *new2, int *swap)
{
int temp;

/******************************************
 * swap equation terms, so the first term *
 * in the first equation is not zero      *
 ******************************************/

/*******************
 * set terms 1,2,3 *
 *******************/

new1[0] = old1[0]; new1[1] = old1[1]; new1[2] = old1[2];
new2[0] = old2[0]; new2[1] = old2[1]; new2[2] = old2[2];

swap[0] = 0;
swap[1] = 1;
swap[2] = 2;

/******************
 * swap terms 1,2 *
 ******************/

if (new1[0] == 0)
  {
  temp = new1[0];
  new1[0] = new1[1];
  new1[1] = temp;

  temp = new2[0];
  new2[0] = new2[1];
  new2[1] = temp;

  swap[0] = 1;
  swap[1] = 0;
  }

/******************
 * swap terms 1,3 *
 ******************/

if (new1[0] == 0)
  {
  temp = new1[0];
  new1[0] = new1[2];
  new1[2] = temp;

  temp = new2[0];
  new2[0] = new2[2];
  new2[2] = temp;

  swap[0] = 2;
  swap[2] = 1;
  }
}

gamgi_bool gamgi_math_diophantine_intersect (int *a, int *b, 
int n1, int n2, int *out)
{
int aa, bb, cc;
int dd, ee, ff;
int x, y, z, t;
int gcd1, gcd2;

/********************************************************************************
 * To solve a system of two linear Diophantine equations, with three variables: *
 *                                                                              *
 * a1x+a2y+a3z = a4                                                             *
 * b1x+b2y+b3z = b4                                                             *
 *                                                                              *
 * we can replace x from the first into the second equation, assuming that      *
 * a1 is not 0, to obtain a simple Diophantine equation with two variables      *
 * (If a1 is zero, just swap the coeffecients, as in our case at least one      *
 * of coeffecients a1,a2,a3, the plane indices, must be non zero):              *
 *                                                                              *
 * Ay+Bz = C, with A = a1b2-b1a2, B = a1b3-b1a3, C = a1b4-b1a4                  *
 *                                                                              *
 * If A = B = 0 no solution can be found (this never occurs, unless the         *
 * two plane families are the same). If (A,B)|C, then a particular solution     *
 * x0,y0 exists, given by the extended Euclidean algorithm above, and the       *
 * general solutions is:                                                        *
 *                                                                              *
 * y = y0 + mB/(A,B)                                                            *
 * z = z0 - mA/(A,B) with m = -2, -1, 0, 1, 2, etc.                             *
 *                                                                              *
 * Replacing y,z in the first system equation, we obtain another Diophantine    *
 * equation with two variables:                                                 *
 *                                                                              *
 * Dx+Em=F, with D=a1, E=(a2B - a3A)/(A,B), F = a4 - a2y0 - a3z0                *
 *                                                                              *
 * If D = E = 0 no solution can be found (this never occurs, as a1              *
 * is non zero by construction). If (D,E)|F then a particular solution          *
 * x0,m0 exists, given again by the extended Euclidean algorithm, and           *
 * the general solution is:                                                     *
 *                                                                              *
 * x = x0 + nE/(D,E)                                                            *
 * m = m0 - nD/(D,E) with n = -2, -1, 0, 1, 2, etc.                             *
 *                                                                              *
 * Therefore the initial system either is impossible or has infinite solutions, *
 * generated by the parameter n:                                                *
 *                                                                              *
 * x = x0 + n E/(D,E)                                                           *
 * y = y0 + m0B/(A,B) - nBD/(A,B)(D,E)                                          *
 * z = z0 - m0A/(A,B) + nAD/(A,B)(D,E)                                          *
 ********************************************************************************/

/*****************************************************
 * if aa == 0 && bb == 0 then the two plane families *
 * are the same, for example: (+1+1-1) and (-1-1+1)  *
 *                                                   *
 * if cc % gcd1 != 0 then no valid solutions exist   *
 * (no node belongs simultaneosusly to both planes)  *
 *****************************************************/

aa = a[0] * b[1] - b[0] * a[1];
bb = a[0] * b[2] - b[0] * a[2];
cc = a[0] * n2 - b[0] * n1;
if (aa == 0 && bb == 0) return FALSE;

gamgi_math_diophantine_gcd_extended (aa, bb, &y, &z, &gcd1);
if (cc % gcd1 != 0) return FALSE;

y *= cc / gcd1;
z *= cc / gcd1;

/*****************************************************
 *        by construction, a[0] != 0, so             *
 *        (dd == 0 && ee == 0) is never true         *
 *                                                   *
 * if ff % gcd2 != 0 then no valid solutions exist   *
 * (no node belongs simultaneosusly to both planes)  *
 *****************************************************/

dd = a[0];
ee = (a[1] * bb - a[2] * aa) / gcd1;
ff = n1 - a[1] * y - a[2] * z;
if (dd == 0 && ee == 0) return FALSE;

gamgi_math_diophantine_gcd_extended (dd, ee, &x, &t, &gcd2);
if (ff % gcd2 != 0) return FALSE;

x *= ff / gcd2;
t *= ff / gcd2;

y += t * bb / gcd1;
z -= t * aa / gcd1;

out[0] = x;
out[1] = y;
out[2] = z;

return TRUE;
}
