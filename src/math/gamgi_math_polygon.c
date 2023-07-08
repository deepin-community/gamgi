/******************************************
 *
 * $GAMGI/src/math/gamgi_math_polygon.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"

#include "gamgi_engine_array.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

static int static_compare (const void *a, const void *b)
{
double *vertex_a = (double *) a;
double *vertex_b = (double *) b;

/*******************************************************
 * elements are swaped if the return value is positive *
 *******************************************************/

if (vertex_a[3] > vertex_b[3]) return 1;

return -1;
}

static void static_intersect (double a, double b, double c, 
double *r2, double *s1, double *s2, gamgi_darray *darray)
{
double point[4];
double below, above;

/**************
 * start data *
 **************/

above = a * (r2[0] - s1[0]) + b * (r2[1] - s1[1]) + c * (r2[2] - s1[2]);
below = a * (s2[0] - s1[0]) + b * (s2[1] - s1[1]) + c * (s2[2] - s1[2]);

/**************************************************
 * store 4-coordinate points: the 4th coordinate  *
 * will be used later to order the polygon points *
 **************************************************/

point[3] = 0.0;

/********************************************************
 * the plane includes the whole segment: save both ends *
 ********************************************************/

if (fabs (above) < GAMGI_MATH_TOLERANCE && 
fabs (below) < GAMGI_MATH_TOLERANCE)
  {
  gamgi_math_vector_copy (s1, point);
  gamgi_engine_darray_push (darray, point);
  gamgi_math_vector_copy (s2, point);
  gamgi_engine_darray_push (darray, point);
  return;
  }

/**************************************************
 * the plane is paralel to the segment: ignore it *
 **************************************************/

if (fabs (below) < GAMGI_MATH_TOLERANCE) return;

/*******************************************************
 * the plane intersects the segment: save intersection *
 *******************************************************/

if (above / below > -GAMGI_MATH_TOLERANCE && 
above / below < 1.0 + GAMGI_MATH_TOLERANCE)
  {
  gamgi_math_vector_sub (s2, s1, point);
  gamgi_math_vector_scale (point, point, above / below);
  gamgi_math_vector_add (s1, point, point);
  gamgi_engine_darray_push (darray, point);
  return;
  }
  
/*******************************************************
 * the plane does not intersect the segment: ignore it *
 *******************************************************/
}

void gamgi_math_polygon_normal (double *r1, 
double *r2, double *r3, double *normal)
{
double v1[3], v2[3];

/********************************************************
 * determine the triangle normal vector, pointing up    *
 * when the corners are entered counter-clockwise,      *
 * pointing down when the corners are entered clockwise *
 ********************************************************/

gamgi_math_vector_sub (r3, r2, v1);
gamgi_math_vector_sub (r1, r2, v2);
gamgi_math_vector_cross (v1, v2, normal);
gamgi_math_vector_normal (normal);
}

gamgi_bool gamgi_math_polygon_check_loops (int *loops,
int n_loops, double *points, int n_points)
{
double center[3], radial[3], base[3], cross[3], cross_old[3];
int n_faces, n_vertices, i, j;

/**************************************
 * check only objects with array data *
 **************************************/

if (n_loops == 0) return TRUE;

/***************************
 * check face construction *
 ***************************/

i = 0;
n_faces = loops[i++];
if (n_faces <= 0) return FALSE;

while (n_faces-- > 0)
  {
  if (i >= n_loops) return FALSE;
  n_vertices = loops[i++];
  if (n_vertices <= 0) return FALSE;

  /*******************
   * get face center *
   *******************/

  gamgi_math_vector_zero (center);
  for (j = i; j < i + n_vertices; j++)
    {
    if (j >= n_loops) return FALSE;
    if (loops[j] >= n_points) return FALSE;
    gamgi_math_vector_add (center, points + 3 * loops[j], center);
    }
  gamgi_math_vector_scale (center, center, 1.0 / n_vertices);

  /*********************************************************************
   * check face contour: cross vectors must point all to the same side *
   *********************************************************************/

  j = i + n_vertices - 1;
  if (j >= n_loops) return FALSE;
  gamgi_math_vector_sub (points + 3 * loops[j], center, base);
  gamgi_math_vector_sub (points + 3 * loops[i], center, radial);
  gamgi_math_vector_cross (base, radial, cross_old);
  for (j = i; j < i + n_vertices; j++)
    {
    gamgi_math_vector_sub (points + 3 * loops[j], center, radial);
    gamgi_math_vector_cross (base, radial, cross);
    if (gamgi_math_vector_dot (cross, cross_old)
    < -GAMGI_MATH_TOLERANCE_DOT) return FALSE;

    gamgi_math_vector_copy (cross, cross_old);
    gamgi_math_vector_copy (radial, base);
    }
  i += n_vertices;
  }

return TRUE;
}

gamgi_bool gamgi_math_polygon_check_colors (int *loops,
int n_loops, float *colors, int n_colors, int *paints, int n_paints)
{
int n_faces, i;

/**************************************
 * check only objects with array data *
 **************************************/

if (n_loops == 0) return TRUE;

/**********************************
 * check array size compatibility *
 **********************************/

n_faces = loops[0];
if (n_colors < 1 || n_colors > n_faces) return FALSE;
if (n_paints != n_colors) return FALSE;

/**********************
 * check colors array *
 **********************/

for (i = 0; i < 3 * n_colors; i++)
  if (colors[i] < 0.0 || colors[i] > 1.0) return FALSE;

/**********************
 * check paints array *
 **********************/

for (i = 0; i < n_colors; i++)
  if (paints[i] <= 0 || paints[0] >= n_loops) return FALSE;

return TRUE;
}

double gamgi_math_polygon_area (int n_points, double *points)
{
double center[3], base[3], radial[3], normal[3];
double area;
int n;

if (n_points < 3) return 0.0;

/**************
 * get center *
 **************/

gamgi_math_vector_zero (center);
for (n = 0; n < n_points; n++)
  gamgi_math_vector_add (center, points + 3*n, center);
gamgi_math_vector_scale (center, center, 1.0/n_points);

/*************************************************
 * Get area for each parallelogram going from    *
 * center to base and radial (starting with last *
 * point for base and first pont for radial),    *
 * sum for all radial points and divide by 2.    *
 *************************************************/
  
area = 0.0;
gamgi_math_vector_sub (points + 3 * (n_points - 1), center, base);
for (n = 0; n < n_points; n++)
  {
  gamgi_math_vector_sub (points + 3 * n, center, radial);
  gamgi_math_vector_cross (base, radial, normal);
  area += gamgi_math_vector_length (normal);

  gamgi_math_vector_copy (radial, base);
  }

return area / 2.0;
}

double gamgi_math_polygon_area_faces (int *faces, double *points)
{
double center[3], radial[3], base[3], cross[3];
double area;
int n_faces, n_vertices, i, j;

i = 0;
n_faces = faces[i++];

/***************************************************************
 * measure the accumulated area of the various faces (surface) *
 ***************************************************************/

area = 0.0;
while (n_faces-- > 0)
  {
  n_vertices = faces[i++];

  /*******************
   * get face center *
   *******************/

  gamgi_math_vector_zero (center);
  for (j = i; j < i + n_vertices; j++)
    gamgi_math_vector_add (center, points + 3 * faces[j], center);
  gamgi_math_vector_scale (center, center, 1.0 / n_vertices);

  /*****************
   * get face area *
   *****************/

  j = i + n_vertices - 1;
  gamgi_math_vector_sub (points + 3 * faces[j], center, base);
  for (j = i; j < i + n_vertices; j++)
    {
    gamgi_math_vector_sub (points + 3 * faces[j], center, radial);
    gamgi_math_vector_cross (base, radial, cross);
    area += gamgi_math_vector_length (cross);

    gamgi_math_vector_copy (radial, base);
    }

  i += n_vertices;
  }

area /= 2.0;

return area;
}

double gamgi_math_polygon_length_lines (int *lines, double *points)
{
double sub[3];
double length;
int vertex1, vertex2;
int i, n_lines;

/*******************************************************************
 * measure the accumulated length of the various lines (perimeter) *
 *******************************************************************/

i = 0;
n_lines = lines[i++];

length = 0.0;
while (n_lines-- > 0)
  {
  vertex1 = 3 * lines[i++];
  vertex2 = 3 * lines[i++];
  gamgi_math_vector_sub (points + vertex1, points + vertex2, sub);
  length += gamgi_math_vector_length (sub);
  }

return length;
}

void gamgi_math_polygon_lines (int n_points, int **lines, int *n_lines)
{
int *array;
int size, n;

/******************
 * allocate array *
 ******************/

size = n_points + 1;
array = (int *) malloc (size * sizeof (int));

/*****************************
 * lines[0]: number of lines *
 *****************************/

array[0] = 1;

/******************
 * polygon points *
 ******************/

for (n = 0; n < n_points; n++) array[n + 1] = n;

*lines = array;
*n_lines = size;
}

void gamgi_math_polygon_loops (int n_points, int **loops, int *n_loops)
{
int *array;
int size, n;

/******************
 * allocate array *
 ******************/

size = n_points + 2;
array = (int *) malloc (size * sizeof (int));

/********************************
 * loops[0]: number of polygons *
 * loops[1]: number of vertices *
 ********************************/

array[0] = 1;
array[1] = n_points;

/******************
 * polygon points *
 ******************/

for (n = 0; n < n_points; n++) array[n + 2] = n;

*loops = array;
*n_loops = size;
}

void gamgi_math_polygon_parallelepiped (double *array, int k0, int k1, 
int j0, int j1, int i0, int i1, double *v1, double *v2, double *v3)
{
int offset;
int i, j, k;

/**************************
 * set parallelogram grid *
 **************************/

offset = 0;
for (i = i0; i <= i1; i++)
  {
  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      gamgi_math_vector_translation (k, j, i, v1, v2, v3, array + offset);
      offset += 3;
      }
    }
  }
}

void gamgi_math_polygon_body (int points_offset,
int n1, int n2, int n3, int **lines, int *n_lines)
{
int *array;
int size, start;
int lines_offset;
int i, j;

/*************************************************************
 * build array containing all the information needed to draw *
 * inner lines in conventional and primitive cell volumes.   *
 *************************************************************/

size = 1 + 2 * ((n1 + 1) * (n2 + 1) + (n1 + 1) * (n3 + 1) + (n2 + 1) * (n3 + 1));
array = (int *) malloc (size * sizeof (int));

array[0] = (n1 + 1) * (n2 + 1) + (n1 + 1) * (n3 + 1) + (n2 + 1) * (n3 + 1);
lines_offset = 1;

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    start = points_offset + j * (n1+1) + i * (n2*(n1+1) + n1+1);
    array[lines_offset++] = start;
    array[lines_offset++] = start + n1;
    }
  }

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n1; j++)
    {
    start = points_offset + j + i * (n2*(n1+1) + n1+1);
    array[lines_offset++] = start;
    array[lines_offset++] = start + n2*(n1+1);
    }
  }

for (i = 0; i <= n2; i++)
  {
  for (j = 0; j <= n1; j++)
    {
    start = points_offset + j + i * (n1+1);
    array[lines_offset++] = start;
    array[lines_offset++] = start + n3*(n1+1)*(n2+1);
    }
  }

*lines = array;
*n_lines = size;
}

void gamgi_math_polygon_faces (int points_offset,
int n1, int n2, int n3, int **lines, int *n_lines)
{
int *array;
int size;
int lines_offset;
int i;

/*************************************************************
 * build array containing all the information needed to draw *
 * outer lines in conventional and primitive cell volumes.   *
 *************************************************************/

size = 1 + 8 * (n1 + n2 + n3 - 3);
array = (int *) malloc (size * sizeof (int));

array[0] = 4 * (n1 + n2 + n3 - 3);
lines_offset = 1;

for (i = 1; i < n1; i++)
  {
  array[lines_offset++] = points_offset + 0 + i;
  array[lines_offset++] = points_offset + n3*(n1+1)*(n2+1) + i;
  array[lines_offset++] = points_offset + n3*(n1+1)*(n2+1) + i;
  array[lines_offset++] = points_offset + n2*(n1+1) + n3*(n1+1)*(n2+1) + i;
  array[lines_offset++] = points_offset + n2*(n1+1) + n3*(n1+1)*(n2+1) + i;
  array[lines_offset++] = points_offset + n2*(n1+1) + i;
  array[lines_offset++] = points_offset + n2*(n1+1) + i;
  array[lines_offset++] = points_offset + 0 + i;
  }

for (i = 1; i < n2; i++)
  {
  array[lines_offset++] = points_offset + 0 + i*(n1+1);
  array[lines_offset++] = points_offset + n1 + i*(n1+1);
  array[lines_offset++] = points_offset + n1 + i*(n1+1);
  array[lines_offset++] = points_offset + n1 + n3*(n1+1)*(n2+1) + i*(n1+1);
  array[lines_offset++] = points_offset + n1 + n3*(n1+1)*(n2+1) + i*(n1+1);
  array[lines_offset++] = points_offset + n3*(n1+1)*(n2+1) + i*(n1+1);
  array[lines_offset++] = points_offset + n3*(n1+1)*(n2+1) + i*(n1+1);
  array[lines_offset++] = points_offset + 0 + i*(n1+1);
  }

for (i = 1; i < n3; i++)
  {
  array[lines_offset++] = points_offset + 0 + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n2*(n1+1) + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n2*(n1+1) + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n1 + n2*(n1+1) + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n1 + n2*(n1+1) + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n1 + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + n1 + i*(n1+1)*(n2+1);
  array[lines_offset++] = points_offset + 0 + i*(n1+1)*(n2+1);
  }

*lines = array;
*n_lines = size;
}

void gamgi_math_polygon_edges (int points_offset, 
int n1, int n2, int n3, int **loops, int *n_loops)
{
int *array;
int size;

/********************************************************
 * build array containing all the information needed    *
 * to draw the six wired or solid polygons used to show *
 * conventional, primitive, and parallelogram volumes.  *
 ********************************************************/

size = 1 + 6 * 5;
array = (int *) malloc (size * sizeof (int));

array[0] = 6;

/**************************
 * face |- vector 1 start *
 **************************/

array[1] = 4;
array[2] = points_offset + 0;
array[3] = points_offset + n3*(n1+1)*(n2+1);
array[4] = points_offset + n2*(n1+1) + n3*(n1+1)*(n2+1);
array[5] = points_offset + n2*(n1+1);

/************************
 * face |- vector 1 end *
 ************************/

array[6] = 4;
array[7] = points_offset + n1;
array[8] = points_offset + n1 + n2*(n1+1);
array[9] = points_offset + n1 + n2*(n1+1) + n3*(n1+1)*(n2+1);
array[10] = points_offset + n1 + n3*(n1+1)*(n2+1);

/**************************
 * face |- vector 2 start *
 **************************/

array[11] = 4;
array[12] = points_offset + 0;
array[13] = points_offset + n1;
array[14] = points_offset + n1 + n3*(n1+1)*(n2+1);
array[15] = points_offset + n3*(n1+1)*(n2+1);

/************************
 * face |- vector 2 end *
 ************************/

array[16] = 4;
array[17] = points_offset + n2*(n1+1);
array[18] = points_offset + n2*(n1+1) + n3*(n1+1)*(n2+1);
array[19] = points_offset + n1 + n2*(n1+1) + n3*(n1+1)*(n2+1);
array[20] = points_offset + n1 + n2*(n1+1);

/**************************
 * face |- vector 3 start *
 **************************/

array[21] = 4;
array[22] = points_offset + 0;
array[23] = points_offset + n2*(n1+1);
array[24] = points_offset + n1 + n2*(n1+1);
array[25] = points_offset + n1;

/************************
 * face |- vector 3 end *
 ************************/

array[26] = 4;
array[27] = points_offset + n3*(n1+1)*(n2+1);
array[28] = points_offset + n1 + n3*(n1+1)*(n2+1);
array[29] = points_offset + n1 + n2*(n1+1) + n3*(n1+1)*(n2+1);
array[30] = points_offset + n2*(n1+1) + n3*(n1+1)*(n2+1);

*loops = array;
*n_loops = size;
}

void gamgi_math_polygon_lines_v12 (int *lines, int *lines_offset, 
int points_offset, gamgi_bool line1, gamgi_bool line2, gamgi_bool line3, 
gamgi_bool line4, gamgi_bool line5, gamgi_bool line6, gamgi_bool line7, 
gamgi_bool line8, gamgi_bool line9, gamgi_bool line10, gamgi_bool line11, 
gamgi_bool line12, gamgi_bool line13, gamgi_bool line14, gamgi_bool line15, 
gamgi_bool line16, gamgi_bool line17, gamgi_bool line18)
{
int lo;

lo = *lines_offset;

/**************************
 * horizontal lines below *
 **************************/ 

if (line1 == TRUE)
  {
  lines[lo++] = points_offset + 0;
  lines[lo++] = points_offset + 6;
  }

if (line2 == TRUE)
  {
  lines[lo++] = points_offset + 1;
  lines[lo++] = points_offset + 5;
  }

if (line3 == TRUE)
  {
  lines[lo++] = points_offset + 6;
  lines[lo++] = points_offset + 7;
  }

if (line4 == TRUE)
  {
  lines[lo++] = points_offset + 5;
  lines[lo++] = points_offset + 4;
  }

if (line5 == TRUE)
  {
  lines[lo++] = points_offset + 7;
  lines[lo++] = points_offset + 1;
  }

if (line6 == TRUE)
  {
  lines[lo++] = points_offset + 4;
  lines[lo++] = points_offset + 0;
  }

/**************************
 * horizontal lines above *
 **************************/

if (line7 == TRUE)
  {
  lines[lo++] = points_offset + 2;
  lines[lo++] = points_offset + 8;
  }

if (line8 == TRUE)
  {
  lines[lo++] = points_offset + 3;
  lines[lo++] = points_offset + 11;
  }

if (line9 == TRUE)
  {
  lines[lo++] = points_offset + 8;
  lines[lo++] = points_offset + 9;
  }

if (line10 == TRUE)
  {
  lines[lo++] = points_offset + 11;
  lines[lo++] = points_offset + 10;
  }

if (line11 == TRUE)
  {
  lines[lo++] = points_offset + 9;
  lines[lo++] = points_offset + 3;
  }

if (line12 == TRUE)
  {
  lines[lo++] = points_offset + 10;
  lines[lo++] = points_offset + 2;
  }

/**************************
 * lateral vertical lines *
 **************************/

if (line13 == TRUE)
  {
  lines[lo++] = points_offset + 0;
  lines[lo++] = points_offset + 2;
  }

if (line14 == TRUE)
  {
  lines[lo++] = points_offset + 1;
  lines[lo++] = points_offset + 3;
  }

if (line15 == TRUE)
  {
  lines[lo++] = points_offset + 4;
  lines[lo++] = points_offset + 8;
  }

if (line16 == TRUE)
  {
  lines[lo++] = points_offset + 7;
  lines[lo++] = points_offset + 11;
  }

if (line17 == TRUE)
  {
  lines[lo++] = points_offset + 5;
  lines[lo++] = points_offset + 9;
  }

if (line18 == TRUE)
  {
  lines[lo++] = points_offset + 6;
  lines[lo++] = points_offset + 10;
  }

*lines_offset = lo;
}

void gamgi_math_polygon_loops_v12 (int *loops, int *loops_offset, 
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3, 
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7, 
gamgi_bool loop8)
{
int lo;

lo = *loops_offset;

/********************************
 * face 1: normal to vector -a3 *
 ********************************/ 
   
if (loop1 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 6;
  loops[lo + 3] = points_offset + 7;
  loops[lo + 4] = points_offset + 1;
  loops[lo + 5] = points_offset + 5;
  loops[lo + 6] = points_offset + 4;
  lo += 7;
  }

/********************************
 * face 2: normal to vector +a3 *
 ********************************/

if (loop2 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 2;
  loops[lo + 2] = points_offset + 8;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 3;
  loops[lo + 5] = points_offset + 11;
  loops[lo + 6] = points_offset + 10;
  lo += 7;
  }

/***********************************
 * face 3: normal to vector -a1-a2 *
 ***********************************/

if (loop3 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 4;
  loops[lo + 3] = points_offset + 8;
  loops[lo + 4] = points_offset + 2;
  lo += 5;
  }

/********************************
 * face 4: normal to vector -a2 *
 ********************************/

if (loop4 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 4;
  loops[lo + 2] = points_offset + 5;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 8;
  lo += 5;
  }

/***********************************
 * face 5: normal to vector +a1-a2 *
 ***********************************/

if (loop5 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 5;
  loops[lo + 2] = points_offset + 1;
  loops[lo + 3] = points_offset + 3;
  loops[lo + 4] = points_offset + 9;
  lo += 5;
  }

/***********************************
 * face 6: normal to vector +a1+a2 *
 ***********************************/

if (loop6 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 7;
  loops[lo + 3] = points_offset + 11;
  loops[lo + 4] = points_offset + 3;
  lo += 5;
  }

/********************************
 * face 7: normal to vector +a2 *
 ********************************/

if (loop7 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 7;
  loops[lo + 2] = points_offset + 6;
  loops[lo + 3] = points_offset + 10;
  loops[lo + 4] = points_offset + 11;
  lo += 5;
  }

/***********************************
 * face 8: normal to vector -a1+a2 *
 ***********************************/

if (loop8 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 6;
  loops[lo + 2] = points_offset + 0;
  loops[lo + 3] = points_offset + 2;
  loops[lo + 4] = points_offset + 10;
  lo += 5;
  }

*loops_offset = lo;
}

void gamgi_math_polygon_loops_v14 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3, 
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7, 
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11, 
gamgi_bool loop12)
{
int lo;

lo = *loops_offset;

/******************************************
 * face 1: normal to hexagonal bellow top *
 ******************************************/
   
if (loop1 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 6;
  loops[lo + 3] = points_offset + 4;
  loops[lo + 4] = points_offset + 2;
  lo += 5;
  }
   
/*******************************************
 * face 2: normal to hexagonal bellow left *
 *******************************************/

if (loop2 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 10;
  loops[lo + 3] = points_offset + 8;
  loops[lo + 4] = points_offset + 6;
  lo += 5;
  }

/********************************************
 * face 3: normal to hexagonal bellow right *
 ********************************************/

if (loop3 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 2;
  loops[lo + 3] = points_offset + 12;
  loops[lo + 4] = points_offset + 10;
  lo += 5;
  }

/********************************************
 * face 4: normal to hexagonal above bottom *
 ********************************************/

if (loop4 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 9;
  loops[lo + 3] = points_offset + 11;
  loops[lo + 4] = points_offset + 13;
  lo += 5;
  }

/*******************************************
 * face 5: normal to hexagonal above right *
 *******************************************/

if (loop5 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 13;
  loops[lo + 3] = points_offset + 3;
  loops[lo + 4] = points_offset + 5;
  lo += 5;
  }

/******************************************
 * face 6: normal to hexagonal above left *
 ******************************************/

if (loop6 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 5;
  loops[lo + 3] = points_offset + 7;
  loops[lo + 4] = points_offset + 9;
  lo += 5;
  }

/***************************************
 * face 7: normal to hexagonal [1 0 0] *
 ***************************************/

if (loop7 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 12;
  loops[lo + 2] = points_offset + 2;
  loops[lo + 3] = points_offset + 3;
  loops[lo + 4] = points_offset + 13;
  lo += 5;
  }

/***************************************
 * face 8: normal to hexagonal [1 1 0] *
 ***************************************/

if (loop8 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 2;
  loops[lo + 2] = points_offset + 4;
  loops[lo + 3] = points_offset + 5;
  loops[lo + 4] = points_offset + 3;
  lo += 5;
  }

/***************************************
 * face 9: normal to hexagonal [0 1 0] *
 ***************************************/

if (loop9 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 4;
  loops[lo + 2] = points_offset + 6;
  loops[lo + 3] = points_offset + 7;
  loops[lo + 4] = points_offset + 5;
  lo += 5;
  }

/*****************************************
 * face 10: normal to hexagonal [-1 0 0] *
 *****************************************/

if (loop10 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 6;
  loops[lo + 2] = points_offset + 8;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 7;
  lo += 5;
  }

/******************************************
 * face 11: normal to hexagonal [-1 -1 0] *
 ******************************************/

if (loop11 == TRUE)
 {
 loops[lo + 0] = 4;
 loops[lo + 1] = points_offset + 8;
 loops[lo + 2] = points_offset + 10;
 loops[lo + 3] = points_offset + 11;
 loops[lo + 4] = points_offset + 9;
 lo += 5;
 }

/*****************************************
 * face 12: normal to hexagonal [0 -1 0] *
 *****************************************/

if (loop12 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 10;
  loops[lo + 2] = points_offset + 12;
  loops[lo + 3] = points_offset + 13;
  loops[lo + 4] = points_offset + 11;
  lo += 5;
  }

*loops_offset = lo;
}

void gamgi_math_polygon_loops_v18 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3, 
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7, 
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11, 
gamgi_bool loop12)
{
int lo;

lo = *loops_offset;

/********************************
 * face 1: normal to vector -a1 *
 ********************************/ 
   
if (loop1 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 14;
  loops[lo + 2] = points_offset + 2;
  loops[lo + 3] = points_offset + 6;
  loops[lo + 4] = points_offset + 15;
  loops[lo + 5] = points_offset + 9;
  loops[lo + 6] = points_offset + 5;
  lo += 7;
  }
   
/********************************
 * face 2: normal to vector +a1 *
 ********************************/

if (loop2 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 16;
  loops[lo + 2] = points_offset + 4;
  loops[lo + 3] = points_offset + 8;
  loops[lo + 4] = points_offset + 17;
  loops[lo + 5] = points_offset + 7;
  loops[lo + 6] = points_offset + 3;
  lo += 7;
  }

/********************************
 * face 3: normal to vector -a2 *
 ********************************/
   
if (loop3 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 10;
  loops[lo + 2] = points_offset + 3;
  loops[lo + 3] = points_offset + 7;
  loops[lo + 4] = points_offset + 11;
  loops[lo + 5] = points_offset + 6;
  loops[lo + 6] = points_offset + 2;
  lo += 7;
  }
   
/********************************
 * face 4: normal to vector +a2 *
 ********************************/
   
if (loop4 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 12;
  loops[lo + 2] = points_offset + 5;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 13;
  loops[lo + 5] = points_offset + 8;
  loops[lo + 6] = points_offset + 4;
  lo += 7;
  }
   
/**************************************
 * face 5: normal to vector -a1-a2-a3 *
 **************************************/

if (loop5 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 10;
  loops[lo + 3] = points_offset + 2;
  loops[lo + 4] = points_offset + 14;
  lo += 5;
  }

/**************************************
 * face 6: normal to vector +a1+a2+a3 *
 **************************************/

if (loop6 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 17;
  loops[lo + 3] = points_offset + 8;
  loops[lo + 4] = points_offset + 13;
  lo += 5;
  }

/**************************************
 * face 7: normal to vector +a1-a2-a3 *
 **************************************/

if (loop7 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 16;
  loops[lo + 3] = points_offset + 3;
  loops[lo + 4] = points_offset + 10;
  lo += 5;
  }

/**************************************
 * face 8: normal to vector -a1+a2+a3 *
 **************************************/

if (loop8 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 13;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 15;
  lo += 5;
  }

/**************************************
 * face 9: normal to vector -a1+a2-a3 *
 **************************************/

if (loop9 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 14;
  loops[lo + 3] = points_offset + 5;
  loops[lo + 4] = points_offset + 12;
  lo += 5;
  }

/***************************************
 * face 10: normal to vector +a1-a2+a3 *
 ***************************************/

if (loop10 == TRUE)
 {
 loops[lo + 0] = 4;
 loops[lo + 1] = points_offset + 1;
 loops[lo + 2] = points_offset + 11;
 loops[lo + 3] = points_offset + 7;
 loops[lo + 4] = points_offset + 17;
 lo += 5;
 }

/***************************************
 * face 11: normal to vector -a1-a2+a3 *
 ***************************************/

if (loop11 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 1;
  loops[lo + 2] = points_offset + 15;
  loops[lo + 3] = points_offset + 6;
  loops[lo + 4] = points_offset + 11;
  lo += 5;
  }

/***************************************
 * face 12: normal to vector +a1+a2-a3 *
 ***************************************/

if (loop12 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 12;
  loops[lo + 3] = points_offset + 4;
  loops[lo + 4] = points_offset + 16;
  lo += 5;
  }

*loops_offset = lo;
}

void gamgi_math_polygon_loops_v24 (int *loops, int *loops_offset,
int points_offset, gamgi_bool loop1, gamgi_bool loop2, gamgi_bool loop3, 
gamgi_bool loop4, gamgi_bool loop5, gamgi_bool loop6, gamgi_bool loop7, 
gamgi_bool loop8, gamgi_bool loop9, gamgi_bool loop10, gamgi_bool loop11, 
gamgi_bool loop12, gamgi_bool loop13, gamgi_bool loop14)
{
int lo;

lo = *loops_offset;

/*************************
 * face 1: |- vector -a1 *
 *************************/ 
   
if (loop1 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 1;
  loops[lo + 3] = points_offset + 2;
  loops[lo + 4] = points_offset + 3;
  lo += 5;
  }
   
/*************************
 * face 2: |- vector +a1 *
 *************************/
   
if (loop2 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 4;
  loops[lo + 2] = points_offset + 5;
  loops[lo + 3] = points_offset + 6;
  loops[lo + 4] = points_offset + 7;
  lo += 5;
  }

/*************************
 * face 3: |- vector -a2 *
 *************************/
   
if (loop3 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 8;
  loops[lo + 2] = points_offset + 9;
  loops[lo + 3] = points_offset + 10;
  loops[lo + 4] = points_offset + 11;
  lo += 5;
  }
   
/*************************
 * face 4: |- vector +a2 *
 *************************/
   
if (loop4 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 12;
  loops[lo + 2] = points_offset + 13;
  loops[lo + 3] = points_offset + 14;
  loops[lo + 4] = points_offset + 15;
  lo += 5;
  }
   
/*************************
 * face 5: |- vector -a3 *
 *************************/
   
if (loop5 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 16;
  loops[lo + 2] = points_offset + 17;
  loops[lo + 3] = points_offset + 18;
  loops[lo + 4] = points_offset + 19;
  lo += 5;
  }

/*************************
 * face 6: |- vector +a3 *
 *************************/
   
if (loop6 == TRUE)
  {
  loops[lo + 0] = 4;
  loops[lo + 1] = points_offset + 20;
  loops[lo + 2] = points_offset + 21;
  loops[lo + 3] = points_offset + 22;
  loops[lo + 4] = points_offset + 23;
  lo += 5;
  }
   
/*******************************
 * face 7: |- vector -a1-a2-a3 *
 *******************************/

if (loop7 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 3;
  loops[lo + 3] = points_offset + 17;
  loops[lo + 4] = points_offset + 16;
  loops[lo + 5] = points_offset + 11;
  loops[lo + 6] = points_offset + 10;
  lo += 7;
  }

/*******************************
 * face 8: |- vector +a1+a2+a3 *
 *******************************/

if (loop8 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 6;
  loops[lo + 2] = points_offset + 12;
  loops[lo + 3] = points_offset + 15;
  loops[lo + 4] = points_offset + 22;
  loops[lo + 5] = points_offset + 21;
  loops[lo + 6] = points_offset + 7;
  lo += 7;
  }

/*******************************
 * face 9: |- vector +a1-a2-a3 *
 *******************************/

if (loop9 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 8;
  loops[lo + 2] = points_offset + 11;
  loops[lo + 3] = points_offset + 16;
  loops[lo + 4] = points_offset + 19;
  loops[lo + 5] = points_offset + 5;
  loops[lo + 6] = points_offset + 4;
  lo += 7;
  }

/********************************
 * face 10: |- vector -a1+a2+a3 *
 ********************************/

if (loop10 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 14;
  loops[lo + 2] = points_offset + 2;
  loops[lo + 3] = points_offset + 1;
  loops[lo + 4] = points_offset + 23;
  loops[lo + 5] = points_offset + 22;
  loops[lo + 6] = points_offset + 15;
  lo += 7;
  }

/********************************
 * face 11: |- vector -a1+a2-a3 *
 ********************************/

if (loop11 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 14;
  loops[lo + 2] = points_offset + 13;
  loops[lo + 3] = points_offset + 18;
  loops[lo + 4] = points_offset + 17;
  loops[lo + 5] = points_offset + 3;
  loops[lo + 6] = points_offset + 2;
  lo += 7;
  }

/********************************
 * face 12: |- vector +a1-a2+a3 *
 ********************************/

if (loop12 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 8;
  loops[lo + 2] = points_offset + 4;
  loops[lo + 3] = points_offset + 7;
  loops[lo + 4] = points_offset + 21;
  loops[lo + 5] = points_offset + 20;
  loops[lo + 6] = points_offset + 9;
  lo += 7;
  }

/********************************
 * face 13: |- vector -a1-a2+a3 *
 ********************************/
     
if (loop13 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 0;
  loops[lo + 2] = points_offset + 10;
  loops[lo + 3] = points_offset + 9;
  loops[lo + 4] = points_offset + 20;
  loops[lo + 5] = points_offset + 23;
  loops[lo + 6] = points_offset + 1;
  lo += 7;
  }
     
/********************************
 * face 14: |- vector +a1+a2-a3 *
 ********************************/

if (loop14 == TRUE)
  {
  loops[lo + 0] = 6;
  loops[lo + 1] = points_offset + 6;
  loops[lo + 2] = points_offset + 5;
  loops[lo + 3] = points_offset + 19;
  loops[lo + 4] = points_offset + 18;
  loops[lo + 5] = points_offset + 13;
  loops[lo + 6] = points_offset + 12;
  lo += 7;
  }

*loops_offset = lo;
}

void gamgi_math_polygon_circumpherence_yz (int *loops, double *points, 
double r, int slices, int *loops_offset, int *points_offset)
{
double angle, step;
int n, lo, po;

lo = *loops_offset;
po = *points_offset;

/*********************
 * yz circumpherence *
 *********************/

loops[lo++] = slices;

angle = 0;
step = GAMGI_MATH_DEG_RAD * 360.0 / slices;
for (n = 0; n < slices; n++)
  {
  points [3 * po + 0] = 0.0;
  points [3 * po + 1] = r * cos (angle);
  points [3 * po + 2] = r * sin (angle);
  angle += step;

  loops[lo++] = po++;
  }

*loops_offset = lo;
*points_offset = po;
}

void gamgi_math_polygon_circumpherence_xz (int *loops, double *points,
double r, int slices, int *loops_offset, int *points_offset)
{
double angle, step;
int n, lo, po;

lo = *loops_offset;
po = *points_offset;

/*********************
 * xz circumpherence *
 *********************/

loops[lo++] = slices;

angle = 0;
step = GAMGI_MATH_DEG_RAD * 360.0 / slices;
for (n = 0; n < slices; n++)
  {
  points [3 * po + 0] = r * cos (angle);
  points [3 * po + 1] = 0.0;
  points [3 * po + 2] = r * sin (angle);
  angle += step;

  loops[lo++] = po++;
  }

*loops_offset = lo;
*points_offset = po;
}

void gamgi_math_polygon_circumpherence_xy (int *loops, double *points,
double r, int slices, int *loops_offset, int *points_offset)
{
double angle, step;
int n, lo, po;

lo = *loops_offset;
po = *points_offset;

/*********************
 * xy circumpherence *
 *********************/

loops[lo++] = slices;

angle = 0;
step = GAMGI_MATH_DEG_RAD * 360.0 / slices;
for (n = 0; n < slices; n++)
  {
  points [3 * po + 0] = r * cos (angle);
  points [3 * po + 1] = r * sin (angle);
  points [3 * po + 2] = 0.0;
  angle += step;

  loops[lo++] = po++;
  }

*loops_offset = lo;
*points_offset = po;
}

gamgi_darray *gamgi_math_polygon_diameter (gamgi_cell *cell,
gamgi_direction *direction, double *r1, double *r2)
{
gamgi_darray *darray;
double point[3];
double v[3];
double a, b, c, d;
double f1, f2;

/*******************************************************
 * allocate dynamic array to store 3-coordinate points *
 *******************************************************/

darray = gamgi_engine_darray_create (2, 0, 3);

/******************************************************
 *    get intersection between direction and sphere   *
 *                                                    *
 *             x**2 + y**2 + z**2 = r**2              *
 * (x,y,z) = (x1,y1,z1) + f [(x2,y2,z2) - (x1,y1,z1)] *
 ******************************************************/

gamgi_math_vector_sub (r2, r1, v);
a = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
b = 2 * (r1[0] * v[0] + r1[1] * v[1] + r1[2] * v[2]);
c = r1[0] * r1[0] + r1[1] * r1[1] + r1[2] * r1[2] - cell->v1 * cell->v1;
d = b * b - 4 * a * c;

/*********************************************
 * return if direction is outside the sphere *
 *********************************************/

if (a < GAMGI_MATH_DIRECTION_LENGTH) return darray;
if (d < GAMGI_MATH_DIRECTION_LENGTH) return darray;

f1 = - b - sqrt (d); f1 /= 2 * a;
f2 = - b + sqrt (d); f2 /= 2 * a;

/**************************************************
 * build line from the point where the direction  *
 * enters the sphere to the point where it leaves *
 **************************************************/

point[0] = r1[0] + f1 * v[0];
point[1] = r1[1] + f1 * v[1];
point[2] = r1[2] + f1 * v[2];
gamgi_engine_darray_push (darray, point);

point[0] = r1[0] + f2 * v[0];
point[1] = r1[1] + f2 * v[1];
point[2] = r1[2] + f2 * v[2];
gamgi_engine_darray_push (darray, point);

return darray;
}

gamgi_darray *gamgi_math_polygon_circle (gamgi_cell *cell, 
gamgi_plane *plane, double *normal, int slices, 
double *r1, double *r2, double *r3)
{
gamgi_darray *darray;
double rotate[9];
double up[3], axis[3];
double xy[3], xyz[3];
double point[3];
double length, angle;
double z, r;
int n;

/*******************************************************
 * allocate dynamic array to store 3-coordinate points *
 *******************************************************/

darray = gamgi_engine_darray_create (GAMGI_MATH_PLANE_SIZE, GAMGI_MATH_PLANE_STEP, 3);

/*************************************
 * get distance from plane to origin *
 * and circumpherence radius, return *
 * if plane is outside the sphere    * 
 *************************************/

z = gamgi_math_vector_dot (r1, normal);
r = cell->v1 * cell->v1 - z * z;
if (r < GAMGI_MATH_DIRECTION_LENGTH) return darray;
r = sqrt (r);

/***********************************
 * get matrix to rotate plane from *
 * horizontal to real orientation  *
 ***********************************/

gamgi_math_vector_absolute (up, 0.0, 0.0, 1.0);
gamgi_math_vector_cross (up, normal, axis);
length = gamgi_math_vector_length (axis);
if (length > GAMGI_MATH_TOLERANCE_LENGTH)
  {
  gamgi_math_vector_scale (axis, axis, 1.0 / length);
  angle = gamgi_math_vector_angle (up, normal);
  }
else
  {
  gamgi_math_vector_absolute (axis, 1.0, 0.0, 0.0);
  angle = 0.0; if (normal[2] < 0.0) angle = 180.0;
  }
gamgi_math_matrix_rotation (angle, axis, rotate);

/*******************************************
 * build horizontal circumpherence contour *
 * and rotate, translate to real position  *
 *******************************************/

gamgi_math_vector_scale (normal, normal, z);
angle = 2.0 * GAMGI_MATH_PI / slices;
for (n = 0; n < slices; n++)
  {
  gamgi_math_vector_absolute (xy, 
  r * cos (n * angle), r * sin (n * angle), 0.0);
  gamgi_math_matrix_vector (rotate, xy, xyz);
  gamgi_math_vector_add (xyz, normal, point);
  gamgi_engine_darray_push (darray, point);
  }

return darray;
}

gamgi_darray *gamgi_math_polygon_scan (int *loops, 
double *points, double *r1, double *r2, double *r3)
{
gamgi_darray *darray;
double *s1, *s2;
double a, b, c;
int n_loops, n_vertices;
int i;

/**************************************************
 * store 4-coordinate points: the 4th coordinate  *
 * will be used later to order the polygon points *
 **************************************************/

darray = gamgi_engine_darray_create (GAMGI_MATH_PLANE_SIZE, GAMGI_MATH_PLANE_STEP, 4);

/**********************************************
 * a,b,c are the vector coordinates of        *
 * the cross product of two vectors of the    *
 * plane passing through points r1,r2,r3:     *
 * r21 x r23 = a vec(x) + b vec(y) + c vec(z) *
 **********************************************/

a = (r1[1] - r2[1]) * (r3[2] - r2[2]) - (r3[1] - r2[1]) * (r1[2] - r2[2]);
b = (r3[0] - r2[0]) * (r1[2] - r2[2]) - (r1[0] - r2[0]) * (r3[2] - r2[2]);
c = (r1[0] - r2[0]) * (r3[1] - r2[1]) - (r3[0] - r2[0]) * (r1[1] - r2[1]);

/*****************************************
 * scan cell contour (loops) looking for *
 * segments intersected by the plane     *
 *****************************************/

i = 0;
n_loops = loops[i];
while (n_loops-- > 0)
  {
  n_vertices = loops[++i];

  s1 = points + 3 * loops[i + n_vertices];
  while (n_vertices-- > 0)
    {
    s2 = points + 3 * loops[++i];
    static_intersect (a, b, c, r2, s1, s2, darray);
    s1 = s2;
    }
  }

return darray;
}

void gamgi_math_polygon_orientate (gamgi_darray *darray, double *normal)
{
double *points;
double center[3], base[3], radial[3], up[3];
double length, length_max;
int n, n_max, n_points;

points = darray->array;
n_points = darray->offset;

/********************************
 * determine the polygon center *
 ********************************/

gamgi_math_vector_zero (center);
for (n = 0; n < n_points; n++)
  gamgi_math_vector_add (center, points + 4*n, center);
gamgi_math_vector_scale (center, center, 1.0 / n_points);

/***********************************************
 * determine the polygon farthest point from   *
 * center, the new origin when ordering the    *
 * polygon, and use it to define the up vector *
 *                                             *
 *      n_max is initialized only to           *
 *      avoid a O3 compilation warning         *
 ***********************************************/

n_max = -1;
length_max = -DBL_MAX;
for (n = 0; n < n_points; n++)
  {
  gamgi_math_vector_sub (points + 4*n, center, radial);
  length = gamgi_math_vector_length (radial);
  if (length > length_max) { length_max = length; n_max = n; }
  }
gamgi_math_vector_sub (center, points + 4*n_max, up);
gamgi_math_vector_normal (up);

/**************************************************
 * determine the cross product between normal and *
 * up vectors: the new vector lives on the face   *
 * and defines, by construction, the reference    *
 * horizontal direction for the new origin.       *
 **************************************************/

gamgi_math_vector_cross (normal, up, base);

/***************************************************************
 * determine all the radial vectors, going from the farthest   *
 * point (the new origin) to the other polygon vertices.       *
 *                                                             *
 * When using the farthest point as origin (point 0), points   *
 * 1,2 and n-1,n-2 must be convex, making much simpler the     *
 * code to eliminate nonconvex points (Graham's algorithm)     *
 *                                                             *
 * When the radial vectors are oriented clockwise (counter-    *
 * clockwise from the outside), starting with the first radial *
 * vector, the order attribute defined below goes from above   *
 * -1.0 to below +1.0 (the extreme values cannot be reached).  *
 * This order attribute is later used to sort the vectors and  *
 * orientate the vertices. The new origin (the farthest point) *
 * and points with the same orientation are all set to -1.0,   *
 * so they will be in the beginning of the ordered array.      *
 ***************************************************************/

for (n = 0; n < n_points; n++)
  {
  gamgi_math_vector_sub (points + 4*n, points + 4*n_max, radial);
  length = gamgi_math_vector_length (radial);
  if (length < GAMGI_MATH_TOLERANCE) points[4*n + 3] = -1.0;
  else points[4*n + 3] = gamgi_math_vector_dot (radial, base) / length;
  }

/***********************************************
 *  sort the polygon points, to orientate      *
 *  the face counter-clockwise, seen from the  *
 *  outside, (clockwise, seen from the origin) *
 ***********************************************/

qsort (points, n_points, 4 * sizeof (double), static_compare);
}

void gamgi_math_polygon_convex (gamgi_darray *darray, double *normal)
{
double *points;
double radial[3];
double v1[3], v2[3];
double x1, y1, z1;
double x2, y2, z2;
double x3, y3, z3;
double length, length_last;
double order, order_last;
int n, n_points, last;

points = darray->array;
n_points = darray->offset;

/**********************************************
 * overwrite points with the same orientation *
 * that are at the same length (same points)  *
 * or at smaller lengths (nonconvex points)   *
 **********************************************/

last = 0;
length_last = 0.0;
order_last = -1.0;
for (n = 1; n < n_points; n++)
  {
  gamgi_math_vector_sub (points + 4*n, points, radial);
  length = gamgi_math_vector_length (radial);
  order = points[4*n + 3];

  if (order - order_last > GAMGI_MATH_TOLERANCE) last++;
  else if (length <= length_last) continue;

  gamgi_math_vector_copy (points + 4*n, points + 3*last);
  order_last = order;
  length_last = length;
  }
n_points = last + 1;

/******************************************************
 *       overwrite nonconvex AND colinear points      *
 *                                                    *
 * When using the farthest point as origin (point 0), *
 * points 1,2 and n-1,n-2 must be convex, making much *
 * simpler the code to eliminate nonconvex points.    *
 *                                                    *
 * Graham's algorithm: when a nonconvex turn occurs,  *
 * going from point n-1 to n, ignore point n-1 and    *
 * try again going from point n-2 to n, repeating     *
 * until a convex turn occurs. Then move forward      *
 * to point n+1, until scanning the whole polygon.    *
 *                                                    *
 * The algorithm is applied only when n_points > 3:   *
 *      n_points < 3 => the polygon collapsed         *
 *      n_points = 3 => the polygon is convex         *
 ******************************************************/

last = 2;
n = 3;
while (n < n_points)
  {
  x1 = points[3 * (last - 1) + 0];
  y1 = points[3 * (last - 1) + 1];
  z1 = points[3 * (last - 1) + 2];

  x2 = points[3 * last + 0];
  y2 = points[3 * last + 1];
  z2 = points[3 * last + 2];
  
  x3 = points[3 * n + 0];
  y3 = points[3 * n + 1];
  z3 = points[3 * n + 2];

  gamgi_math_vector_absolute (v1, x2 - x1, y2 - y1, z2 - z1);
  gamgi_math_vector_absolute (v2, x3 - x2, y3 - y2, z3 - z2);

  /****************************************
   * seen from the origin, points rotate  *
   * clockwise, so v1xv2 should point to  *
   * the other side, as the normal vector *
   ****************************************/

  if (gamgi_math_vector_mix (v1, v2, normal) < GAMGI_MATH_TOLERANCE) last--;
  else
    {
    last++;
    gamgi_math_vector_copy (points + 3*n, points + 3*last);
    n++;
    }
  }
if (n_points > 3) n_points = last + 1;

/*****************************************************
 * reallocate the array data, now with 3 dimensions, *
 *****************************************************/

gamgi_engine_darray_recreate (darray, n_points, GAMGI_MATH_PLANE_STEP, 3);
}
