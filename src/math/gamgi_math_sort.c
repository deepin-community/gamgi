/******************************************
 *
 * $GAMGI/src/math/gamgi_math_sort.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

/********* external function ***********
 *                                     *
 *          GAMGI_MATH_SORT_MAX        *
 *                                     *
 *        Get maximum of two long.      *
 *                                     *
 ***************************************/

long gamgi_math_sort_max (long a0, long b0)
{
return (a0 > b0) ? a0 : b0;
}

/********* external function ***********
 *                                     *
 *          GAMGI_MATH_SORT_MIN        *
 *                                     *
 *        Get minimum of two long.      *
 *                                     *
 ***************************************/

long gamgi_math_sort_min (long a0, long b0)
{
return (a0 < b0) ? a0 : b0;
}

/******** external function *********
 *                                  *
 *       GAMGI_MATH_SORT_INT2       *
 *                                  *
 * Sort 2 ints in increasing order. *
 *                                  *
 ************************************/

void gamgi_math_sort_int2 (int a0, int b0, 
int *a1, int *b1)
{
if (a0 < b0)
  {
  *a1 = a0;
  *b1 = b0;
  }
else
  {
  *a1 = b0;
  *b1 = a0;
  }
}

/********* external function *********
 *                                   *
 *       GAMGI_MATH_SORT_LONG2       *
 *                                   *
 * Sort 2 longs in increasing order. *
 *                                   *
 *************************************/

void gamgi_math_sort_long2 (long a0, long b0,
long *a1, long *b1)
{
if (a0 < b0)
  {
  *a1 = a0;
  *b1 = b0;
  }
else
  {
  *a1 = b0;
  *b1 = a0;
  }

}

/********* external function ***********
 *                                     *
 *       GAMGI_MATH_SORT_DOUBLE2       *
 *                                     *
 * Sort 2 doubles in increasing order. *
 *                                     *
 ***************************************/

void gamgi_math_sort_double2 (double a0, double b0, 
double *a1, double *b1)
{
if (a0 < b0)
  {
  *a1 = a0;
  *b1 = b0;
  }
else
  {
  *a1 = b0;
  *b1 = a0;
  }
}

/******** external function *********
 *                                  *
 *       GAMGI_MATH_SORT_INT3       *
 *                                  *
 * Sort 3 ints in increasing order. *
 *                                  *
 ************************************/

void gamgi_math_sort_int3 (int a0, int b0, int c0, 
int *a1, int *b1, int *c1)
{
gamgi_math_sort_int2 (a0, b0, a1, b1);                      
gamgi_math_sort_int2 (*b1, c0, b1, c1);
gamgi_math_sort_int2 (*a1, *b1, a1, b1);
}

/********* external function *********
 *                                   *
 *        GAMGI_MATH_SORT_LONG3      *
 *                                   *
 * Sort 3 longs in increasing order. *
 *                                   *
 *************************************/

void gamgi_math_sort_long3 (long a0, long b0, long c0,
long *a1, long *b1, long *c1)
{
gamgi_math_sort_long2 (a0, b0, a1, b1);
gamgi_math_sort_long2 (*b1, c0, b1, c1);
gamgi_math_sort_long2 (*a1, *b1, a1, b1);
}

/********* external function ***********
 *                                     *
 *       GAMGI_MATH_SORT_DOUBLE3       *
 *                                     *
 * Sort 3 doubles in increasing order. *
 *                                     *
 ***************************************/

void gamgi_math_sort_double3 (double a0, double b0, double c0, 
double *a1, double *b1, double *c1)
{
gamgi_math_sort_double2 (a0, b0, a1, b1);
gamgi_math_sort_double2 (*b1, c0, b1, c1);
gamgi_math_sort_double2 (*a1, *b1, a1, b1);
}

/************* external function **********
 *                                        *
 *         GAMGI_MATH_SORT_DOUBLE2V       *
 *                                        *
 * Sort 2 arrays of doubles in increasing *
 *   order, according to their lengths.   *
 *                                        *
 ******************************************/

void gamgi_math_sort_double2v (double *a0, double *b0,
double *a1, double *b1)
{
double length_a0, length_b0;
double aux[3];

length_a0 = gamgi_math_vector_length (a0);
length_b0 = gamgi_math_vector_length (b0);

if (length_a0 < length_b0)
  {
  gamgi_math_vector_copy (a0, aux);
  gamgi_math_vector_copy (b0, b1);
  }
else
  {
  gamgi_math_vector_copy (b0, aux);
  gamgi_math_vector_copy (a0, b1);
  }
gamgi_math_vector_copy (aux, a1);
}

/************* external function **********
 *                                        *
 *         GAMGI_MATH_SORT_DOUBLE3V       *
 *                                        *
 * Sort 3 arrays of doubles in increasing *
 *   order, according to their lengths.   *
 *                                        *
 ******************************************/

void gamgi_math_sort_double3v (double *a0, double *b0, double *c0,
double *a1, double *b1, double *c1)
{
gamgi_math_sort_double2v (a0, b0, a1, b1);
gamgi_math_sort_double2v (b1, c0, b1, c1);
gamgi_math_sort_double2v (a1, b1, a1, b1);
}

/******************** external function *****************
 *                                                      *
 *                GAMGI_MATH_SORT_DIRECTION             *
 *                                                      *
 * Sort the length parameters a,b in increasing order   *
 * (pa,pb) and change the angle parameters that are     *
 * affected by the sorting operation ac,bc accordingly  *
 * (pac,pbc). The sorting operation is described by     *
 * matrix r. The accumulated transformation is recorded *
 * on matrix m.                                         *
 *                                                      *
 ********************************************************/

void gamgi_math_sort_direction (double a, double b,
double *pa, double *pb, double ac, double bc,
double *pac, double *pbc, double *rc, double *m)
{
double n[9];

/********************************
 * sorting criterium: *pa < *pb *
 ********************************/

if (a > b)
  {
  /************************
   * swap lengths, angles *
   ************************/

  *pa = b;
  *pb = a;
  *pac = bc;
  *pbc = ac;

  /*********************************************************
   * The new transformation matrix is obtained multiplying *
   * the old transformation matrix by the rotation matrix. *
   *********************************************************/

  gamgi_math_matrix_copy (m, n);
  gamgi_math_matrix_matrix (rc, n, m);
  }
else
  {
  /***********************
   * set lengths, angles *
   ***********************/

  *pa = a;
  *pb = b;
  *pac = ac;
  *pbc = bc;

  /**********************************************
   * The transformation matrix remains the same *
   **********************************************/
  }
}
