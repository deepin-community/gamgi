/********************************************
 *
 * $GAMGI/src/math/gamgi_math_diophantine.h
 *
 * Copyright (C) 2006 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

 **********************
 * external functions *
 **********************/

int gamgi_math_diophantine_gcd (int a, int b);

gamgi_bool gamgi_math_diophantine_soluble (int a, int b, int c, int d);

void gamgi_math_diophantine_gcd_extended (int a, int b, int *x, int *y, int *gcd);

void gamgi_math_diophantine_swap (int *old1, int *old2,
int *new1, int *new2, int *swap);

gamgi_bool gamgi_math_diophantine_intersect (int *a, int *b,
int n1, int n2, int *out);

/*************
 * Rationale *
 *************


 *************
 * Rationale *
 *************/
