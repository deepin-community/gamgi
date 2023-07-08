/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_wigner.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************************************************
 *                     Bibliography                       *
 *                                                        *
 * Burns G., Glazer A. M., Space Groups for Solid State,  *
 * Scientists, Academic Press (1990)                      *
 * Pictures for all 24 Wigner-Setz cells                  *
 *                                                        *
 * Ed. Hahn Th., International Tables for Crystalography, *
 * Volume A - Space Group Symmetry, Kluwer (2002)         *
 * Table 9.1.8.1., pg 748, with all 24 Wigner-Setz cells  *
 **********************************************************/

#include "gamgi_engine.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"

#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_polygon.h"
#include "gamgi_math_sort.h"
#include "gamgi_phys_cell.h"

int points_offset, loops_offset, lines_offset;

/********************* internal function *****************
 *                                                       *
 *                     STATIC_CLEAN_WIGNER               *
 *                                                       *
 * 1) use loops, lines and mark arrays to mark needed    *
 * (1) and uneeded (0) points, and create loops_new and  *
 * lines_new to allocate the real number of loops and    *
 * lines; 2) call clean () to create a new points array, *
 * containing only the good points.                      *
 *                                                       *
 *********************************************************/

static void static_clean_wigner (gamgi_cell *cell)
{
int *mark, *loops, *loops_new, *lines, *lines_new;
int n_loops, n_lines, n_vertices, i;

/**************
 * start mark *
 **************/

mark = (int *) malloc (sizeof (int) * cell->n_points);

i = 0;
while (i < cell->n_nodes) mark [i++] = 1;
while (i < cell->n_points) mark [i++] = 0;

/*********************************************
 * flag mark with loops and create new loops *
 *********************************************/

loops_new = NULL;

loops = cell->loops;
if (loops != NULL)
  {
  i = 0;
  n_loops = loops[i++];
  while (n_loops-- > 0)
    {
    if (loops[i] == -1) break;

    n_vertices = loops[i++];
    while (n_vertices-- > 0) mark [loops[i++]] = 1;
    }

  cell->n_loops = i;
  loops_new = (int *) malloc (sizeof (int) * cell->n_loops);
  }

/*********************************************
 * flag mark with lines and create new lines *
 *********************************************/

lines_new = NULL;

lines = cell->lines;
if (lines != NULL)
  {
  i = 0;
  n_lines = lines[i++];
  while (n_lines-- > 0)
    {
    if (lines[i] == -1) break;

    mark [lines[i++]] = 1;
    mark [lines[i++]] = 1;
    }

  cell->n_lines = i;
  lines_new = (int *) malloc (sizeof (int) * cell->n_lines);
  }

gamgi_phys_cell_clean (cell, mark, loops_new, lines_new);
}

/********************** internal function *****************
 *                                                        *
 *                        STATIC_BODY                     *
 *                                                        *
 * Creates all the points and lines needed to represent P *
 * Orthorhombic, Tetragonal and Cubic Wigner-Seitz cells, *
 * when using the All option, in the Borders option menu. *
 *                                                        *
 **********************************************************/

static void static_body (gamgi_cell *cell)
{
double start_x, start_y, start_z;
double x, y, z;
double *points;
int n1, n2, n3;
int i, j, k;
int offset;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

points = cell->points;
offset = 3 * cell->n_nodes;

/**********************************************
 * set vertex points for 6 faces x 4 vertices *
 **********************************************/

start_x = -cell->a1[0]/2;
start_y = -cell->a2[1]/2;
start_z = -cell->a3[2]/2;

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      x = k*cell->a1[0] + j*cell->a2[0] + i*cell->a3[0];
      y = k*cell->a1[1] + j*cell->a2[1] + i*cell->a3[1];
      z = k*cell->a1[2] + j*cell->a2[2] + i*cell->a3[2];

      points [offset + 0] = start_x + x;
      points [offset + 1] = start_y + y;
      points [offset + 2] = start_z + z;
      offset += 3;
      }
    }
  }

}

/******************** internal function **************
 *                                                   *
 *                     STATIC_FACES                  *
 *                                                   *
 * Creates all the points and inner lines needed to  *
 * represent P Orthorhombic, Tetragonal and Cubic    *
 * Wigner-Seitz cells, when using the Faces option,  *
 * in the Borders option menu. Outer lines are drawn *
 * elsewhere, by static_edges.                       *
 *                                                   *
 *****************************************************/

static void static_faces (gamgi_cell *cell)
{
int i, n1, n2, n3;
int *lines;
int vertices_offset;
double x_offset, y_offset, z_offset;
double *points;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

points = cell->points;

/********************************
 * 4 * (n1 + n2 + n3 - 3) lines *
 ********************************/

cell->n_lines = 1 + 8 * (n1 + n2 + n3 - 3);
lines = (int *) malloc (sizeof (int) * cell->n_lines);
lines[0] = 4 * (n1 + n2 + n3 - 3);
cell->lines = lines;

/**********
 * offset *
 **********/

lines_offset = 1;
points_offset = cell->n_nodes + 8;
vertices_offset = 3 * points_offset;

x_offset = - cell->a1[0]/2;
y_offset = - cell->a2[1]/2;
z_offset = - cell->a3[2]/2;

/*************
 * around a1 *
 *************/

for (i = 1; i < n1; i++)
  {
  /************
   * vertices *
   ************/

  points [vertices_offset + 0] = x_offset + i * cell->a1[0];
  points [vertices_offset + 1] = y_offset;
  points [vertices_offset + 2] = z_offset;
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + i * cell->a1[0];
  points [vertices_offset + 1] = y_offset;
  points [vertices_offset + 2] = z_offset + n3 * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + i * cell->a1[0];
  points [vertices_offset + 1] = y_offset + n2 * cell->a2[1];
  points [vertices_offset + 2] = z_offset + n3 * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + i * cell->a1[0];
  points [vertices_offset + 1] = y_offset + n2 * cell->a2[1];
  points [vertices_offset + 2] = z_offset;
  vertices_offset += 3;

  /*********
   * lines *
   *********/

  lines[lines_offset++] = 0 + points_offset;
  lines[lines_offset++] = 1 + points_offset;

  lines[lines_offset++] = 1 + points_offset;
  lines[lines_offset++] = 2 + points_offset;

  lines[lines_offset++] = 2 + points_offset;
  lines[lines_offset++] = 3 + points_offset;

  lines[lines_offset++] = 3 + points_offset;
  lines[lines_offset++] = 0 + points_offset;

  points_offset += 4;
  }

/*************
 * around a2 *
 *************/

for (i = 1; i < n2; i++)
  {
  /************
   * vertices *
   ************/

  points [vertices_offset + 0] = x_offset;
  points [vertices_offset + 1] = y_offset + i * cell->a2[1];
  points [vertices_offset + 2] = z_offset;
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + n1 * cell->a1[0];
  points [vertices_offset + 1] = y_offset + i * cell->a2[1];
  points [vertices_offset + 2] = z_offset;
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + n1 * cell->a1[0];
  points [vertices_offset + 1] = y_offset + i * cell->a2[1];
  points [vertices_offset + 2] = z_offset + n3 * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset;
  points [vertices_offset + 1] = y_offset + i * cell->a2[1];
  points [vertices_offset + 2] = z_offset + n3 * cell->a3[2];
  vertices_offset += 3;

  /*********
   * lines *
   *********/

  lines[lines_offset++] = 0 + points_offset;
  lines[lines_offset++] = 1 + points_offset;

  lines[lines_offset++] = 1 + points_offset;
  lines[lines_offset++] = 2 + points_offset;

  lines[lines_offset++] = 2 + points_offset;
  lines[lines_offset++] = 3 + points_offset;

  lines[lines_offset++] = 3 + points_offset;
  lines[lines_offset++] = 0 + points_offset;

  points_offset += 4;
  }

/*************
 * around a3 *
 *************/

for (i = 1; i < n3; i++)
  {
  
  /************
   * vertices *
   ************/

  points [vertices_offset + 0] = x_offset;
  points [vertices_offset + 1] = y_offset;
  points [vertices_offset + 2] = z_offset + i * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + n1 * cell->a1[0];
  points [vertices_offset + 1] = y_offset;
  points [vertices_offset + 2] = z_offset + i * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset + n1 * cell->a1[0];
  points [vertices_offset + 1] = y_offset + n2 * cell->a2[1];
  points [vertices_offset + 2] = z_offset + i * cell->a3[2];
  vertices_offset += 3;

  points [vertices_offset + 0] = x_offset;
  points [vertices_offset + 1] = y_offset + n2 * cell->a2[1];
  points [vertices_offset + 2] = z_offset + i * cell->a3[2];
  vertices_offset += 3;

  /*********
   * lines *
   *********/

  lines[lines_offset++] = 0 + points_offset;
  lines[lines_offset++] = 1 + points_offset;

  lines[lines_offset++] = 1 + points_offset;
  lines[lines_offset++] = 2 + points_offset;

  lines[lines_offset++] = 2 + points_offset;
  lines[lines_offset++] = 3 + points_offset;

  lines[lines_offset++] = 3 + points_offset;
  lines[lines_offset++] = 0 + points_offset;

  points_offset += 4;
  }

}

/***************** internal function **********
 *                                            *
 *                  STATIC_EDGES              *
 *                                            *
 * Creates all the points and lines needed to *
 * represent P Orthorhombic, Tetragonal and   *
 * Cubic Wigner-Seitz cells, when using the   *
 * Edges option, in the Borders option menu.  *
 *                                            *
 **********************************************/

static void static_edges (gamgi_cell *cell)
{
double *points;
double start_x, start_y, start_z;
int n1, n2, n3;
int offset;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

points = cell->points;

/**********************************************
 * set vertex points for 6 faces x 4 vertices *
 **********************************************/

offset = 3 * cell->n_nodes;

start_x = -cell->a1[0]/2;
start_y = -cell->a2[1]/2;
start_z = -cell->a3[2]/2;

/*****************************
 * vertices below face (001) *
 *****************************/

/************
 * vertex 1 *
 ************/

points [offset + 0] = start_x;
points [offset + 1] = start_y;
points [offset + 2] = start_z;
offset += 3;

/************
 * vertex 2 *
 ************/

points [offset + 0] = start_x + n1 * cell->a1[0];
points [offset + 1] = start_y;
points [offset + 2] = start_z;
offset += 3;

/************
 * vertex 3 *
 ************/

points [offset + 0] = start_x;
points [offset + 1] = start_y + n2 * cell->a2[1];
points [offset + 2] = start_z;
offset += 3;

/************
 * vertex 4 *
 ************/

points [offset + 0] = start_x + n1 * cell->a1[0];
points [offset + 1] = start_y + n2 * cell->a2[1];
points [offset + 2] = start_z;
offset += 3;

/*****************************
 * vertices above face (001) *
 *****************************/

/************
 * vertex 5 *
 ************/

points [offset + 0] = start_x;
points [offset + 1] = start_y;
points [offset + 2] = start_z + n3 * cell->a3[2];
offset += 3;

/************
 * vertex 6 *
 ************/

points [offset + 0] = start_x + n1 * cell->a1[0];
points [offset + 1] = start_y;
points [offset + 2] = start_z + n3 * cell->a3[2];
offset += 3;

/************
 * vertex 7 *
 ************/

points [offset + 0] = start_x;
points [offset + 1] = start_y + n2 * cell->a2[1];
points [offset + 2] = start_z + n3 * cell->a3[2];
offset += 3;

/************
 * vertex 8 *
 ************/

points [offset + 0] = start_x + n1 * cell->a1[0];
points [offset + 1] = start_y + n2 * cell->a2[1];
points [offset + 2] = start_z + n3 * cell->a3[2];
offset += 3;
}

static void static_orthorhombic_p (gamgi_cell *cell)
{
int n1, n2, n3;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

cell->n_points = cell->n_nodes;

switch (cell->borders)
  {
  case FALSE:
  if (cell->faces == FALSE) break;

  case GAMGI_PHYS_EDGES:
  cell->n_points += 8; 
  break;

  case GAMGI_PHYS_FACES:
  cell->n_points += 8 + 4 * (n1 + n2 + n3 - 3);
  break;

  case GAMGI_PHYS_ALL:
  cell->n_points += (n1 + 1) * (n2 + 1) * (n3 + 1);
  break;
  }

cell->points = (double *) malloc (sizeof (double) * 3 * cell->n_points);

switch (cell->borders)
  {
  case FALSE:
  if (cell->faces == FALSE) break;

  case GAMGI_PHYS_EDGES:
  static_edges (cell);
  gamgi_math_polygon_edges (cell->n_nodes, 1, 1, 1, &cell->loops, &cell->n_loops);
  break;

  case GAMGI_PHYS_FACES:
  static_edges (cell);
  gamgi_math_polygon_edges (cell->n_nodes, 1, 1, 1, &cell->loops, &cell->n_loops);
  static_faces (cell);
  break;

  case GAMGI_PHYS_ALL:
  static_body (cell);
  gamgi_math_polygon_body (cell->n_nodes, n1, n2, n3, &cell->lines, &cell->n_lines);
  break;
  }

}

static void static_orthorhombic_c (gamgi_cell *cell, double a,
double b, double c, double *m)
{
int i, j, k, n1, n2, n3, n4;
int vertices_offset;
int *loops, *lines;
double t[9], v[3];
double x, y, z, aux1, aux2;
double *points;
gamgi_bool k0, k1, j0, j1, i0, i1;
gamgi_bool local1, local2, local3, local4, local5, local6;
gamgi_bool local7, local8, local9, local10, local11, local12;
gamgi_bool local13, local14, local15, local16, local17, local18;
gamgi_bool global_all, global_faces, global_edges;

n1 = cell->n1;
n2 = cell->n2;
n3 = (cell->borders == GAMGI_PHYS_EDGES) ? 1 : cell->n3;
n4 = (cell->borders == GAMGI_PHYS_EDGES) ? cell->n3 : 1;

/***************
 * 12 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 12);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 6 faces x 4 vertices + 2 faces x 6 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (6 * 5 + 2 * 7);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 8;
cell->loops = loops;

/************************************************
 * 2 x (6 vertical lines + 12 horizontal lines) *
 ************************************************/

i = cell->n_lines = 1 + cell->n_nodes * 2 * (6 + 12);
lines = (int *) malloc (sizeof (int) * i);
while (i-- > 1) lines[i] = -1;
lines[0] = cell->n_nodes * (6 + 12);
cell->lines = lines;

/**********
 * offset *
 **********/

loops_offset = 1;
lines_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

gamgi_math_matrix_inverse (m, t);

aux1 = (pow(b, 2) + pow(a, 2)) / (4*b);
aux2 = (pow(b, 2) - pow(a, 2)) / (4*b);

global_edges = (cell->borders != GAMGI_PHYS_EDGES);
global_faces = (cell->borders == GAMGI_PHYS_FACES);
global_all = (cell->borders == GAMGI_PHYS_ALL);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /*****************************************
       * vertices inside the conventional cell *
       *****************************************/

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, -aux1, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, aux1, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, -aux1, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, aux1, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * vertices on faces of the conventional cell *
       **********************************************/

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, a/2, -aux2, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (v, a/2, aux2, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, -aux2, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, aux2, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, a/2, -aux2, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, a/2, aux2, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, -aux2, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, aux2, -c/2 + n4 * c);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************
       * build faces *
       ***************/

      k0 = (k == 0);
      k1 = (k == n1 - 1);
      j0 = (j == 0);
      j1 = (j == n2 - 1);
      i0 = (i == 0);
      i1 = (i == n3 - 1);

      /*********
       * loops *
       *********/

      if (cell->faces == TRUE)
        {
        if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
          {
          gamgi_math_polygon_loops_v12 (cell->loops, &loops_offset, points_offset,
          (i0), (i1), (j0), (k1), (k1 || j1), (j1), (k0), (j0 || k0));
          }

        else if (m[0] * m[4] == 1.0)
          {
          /*********
           * 1 2 3 *
           *********/

          gamgi_math_polygon_loops_v12 (cell->loops, &loops_offset, points_offset,
          (i0), (i1), (k1), (k1 || j1), (j1), (k0), (k0 || j0), (j0));
          }

        else if (m[3] * m[8] == 1.0)
          {
          /*********
           * 2 1 3 *
           *********/

          gamgi_math_polygon_loops_v12 (cell->loops, &loops_offset, points_offset,
          (i0), (i1), (j0), (j0 || k1), (k1), (j1), (j1 || k0), (k0));
          }

        }


      /*********
       * lines *
       *********/

      if (cell->borders != FALSE)
        {
        if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
          {
          local1 = (i0 && (global_edges || (k0 || j0)));
          local2 = (i0 && (k1 || j1));
          local3 = (i0 && k0);
          local4 = (i0 && (global_edges || k1));
          local5 = (i0 && j1);
          local6 = (i0 && (global_edges || j0));

          local7 = (global_all || j0 || (global_faces && i1));
          local8 = (j1);
          local9 = (global_all || k1 || (global_faces && i1));
          local10 = (k0);
          local11 = (global_all || (k1 || j1) || (global_faces && i1));
          local12 = (k0 || j0);

          local13 = (global_all || j0);
          local14 = (j1);
          local15 = (global_all || (k1 || j0));
          local16 = (k0 || j1);
          local17 = (global_all || k1);
          local18 = (k0);
          }

        else if (m[0] * m[4] == 1.0)
          {
          /*********
           * 1 2 3 *
           *********/

          local1 = (i0 && j0);
          local2 = (i0 && (global_edges || j1));
          local3 = (i0 && (j0 || k0));
          local4 = (i0 && (global_edges || (j1 || k1)));
          local5 = (i0 && k0);
          local6 = (i0 && (global_edges || k1));

          local7 = (global_all || k1 || (global_faces && i1));
          local8 = (k0);
          local9 = (global_all || (j1 || k1) || (global_faces && i1));
          local10 = (j0 || k0);
          local11 = (global_all || j1 || (global_faces && i1));
          local12 = (j0);

          local13 = (global_all || (k1 || j0));
          local14 = (k0 || j1);
          local15 = (global_all || k1);
          local16 = (k0);
          local17 = (global_all || j1);
          local18 = (j0);
          }

        else if (m[3] * m[8] == 1.0)
          {
          /*********
           * 2 1 3 *
           *********/
          
          local1 = (i0 && k0);
          local2 = (i0 && (global_edges || k1));
          local3 = (i0 && (k0 || j1));
          local4 = (i0 && (global_edges || (k1 || j0)));
          local5 = (i0 && j1);
          local6 = (i0 && (global_edges || j0));

          local7 = (global_all || j0 || (global_faces && i1));
          local8 = (j1);
          local9 = (global_all || (k1 || j0) || (global_faces && i1));
          local10 = (k0 || j1);
          local11 = (global_all || k1 || (global_faces && i1));
          local12 = (k0);

          local13 = (global_all || (j0 || k0));
          local14 = (j1 || k1);
          local15 = (global_all || j0);
          local16 = (j1);
          local17 = (global_all || k1);
          local18 = (k0);
          }

        else
          {
          /***********************************
           * this unneeded initialization    *
           * avoids a O3 compilation warning *
           ***********************************/

          local1 = local2 = local3 = local4 = local5 = local6 = 0;
          local7 = local8 = local9 = local10 = local11 = local12 = 0;
          local13 = local14 = local15 = local16 = local17 = local18 = 0;
          }

        gamgi_math_polygon_lines_v12 (cell->lines, &lines_offset, 
        points_offset, local1, local2, local3, local4, local5, local6, 
        local7, local8, local9, local10, local11, local12, local13, 
        local14, local15, local16, local17, local18);
        }

      points_offset += 12;
      }
    }
  }
}

static void static_orthorhombic_f (gamgi_cell *cell, double a,
double b, double c, double *m)
{
int i, j, k, n1, n2, n3;
int vertices_offset;
int *loops;
double t[9], v[3];
double x, y, z, aux1, aux2;
double *points;
gamgi_bool global;
gamgi_bool local1, local2, local3, local4, local5, local6, local7;
gamgi_bool local8, local9, local10, local11, local12, local13, local14;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

/***************
 * 24 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 24);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 8 faces x 6 vertices + 6 faces x 4 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (8 * 7 + 6 * 5);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 14;
cell->loops = loops;

/**********
 * offset *
 **********/

loops_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

/************************
 * auxiliary parameters *
 ************************/

gamgi_math_matrix_inverse (m, t);

aux1 = pow(a, 2) / (4*c);
aux2 = pow(a, 2) / (4*b);

global = (cell->faces == TRUE || cell->borders != FALSE);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /****************************************************
       * vertices on face (-100) of the conventional cell *
       ****************************************************/


      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, -b/4 - aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, -a/4, -b/4, -c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, -b/4 + aux2, -c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, a/4, -b/4, -c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (100) of the conventional cell *
       ***************************************************/

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, b/4 - aux2, c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (v, a/4, b/4, c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, b/4 + aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, -a/4, b/4, c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (0-10) of the conventional cell *
       ****************************************************/

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, -b/4 + aux2, c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, -a/4, -b/4, c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -b/4 - aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, a/4, -b/4, c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (010) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 13 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, b/4 + aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 14 *
       *************/
      
      gamgi_math_vector_absolute (v, a/4, b/4, -c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 15 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, b/4 - aux2, -c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 16 *
       *************/

      gamgi_math_vector_absolute (v, -a/4, b/4, -c/4);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (00-1) of the conventional cell *
       ****************************************************/

      /*************
       * vertex 17 *
       *************/

      gamgi_math_vector_absolute (v, a/2, -b/4 + aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 18 *
       *************/

      gamgi_math_vector_absolute (v, a/2, -b/4 + aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 19 *
       *************/

      gamgi_math_vector_absolute (v, a/2, b/4 - aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 20 *
       *************/

      gamgi_math_vector_absolute (v, a/2, b/4 - aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (001) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 21 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, -b/4 + aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 22 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, b/4 - aux2, c/4 - aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 23 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, b/4 - aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 24 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, -b/4 + aux2, -c/4 + aux1);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3; 

      /**********************************************
       * Set flags defining which faces must be     *
       * drawn, according to the cell orientation.  *
       *                                            *
       * To get these flags, use GAMGI to create a  *
       * Wigner-Seitz solid cell, where all faces   *
       * are drawn but one, and a wired 3x2x1 (to   *
       * recognize the axes) primitive cell: by     *
       * comparing the two cells, see when the face *
       * missing must be drawn and get the flags.   *
       **********************************************/

      if (m[0] * m[4] == 1.0)
        {
        /*********
         * 1 2 3 *
         *********/

        local1 = (i == 0);
        local2 = (i == n3 - 1);
        local3 = (k == n1 - 1 || j == 0);
        local4 = (k == 0 || j == n2 - 1);
        local5 = (k == n1 - 1 || j == n2 - 1 || i == 0);
        local6 = (k == 0 || j == 0 || i == n3 - 1);

        local7 = (k == n1 - 1 || i == 0);
        local8 = (k == 0 || i == n3 - 1);
        local9 = (k == n1 - 1);
        local10 = (k == 0);
        local11 = (j == n2 - 1 || i == 0);
        local12 = (j == 0 || i == n3 - 1);
        local13 = (j == 0);
        local14 = (j == n2 - 1);
        }

      else if (m[3] * m[8] == 1.0)
        {
        /*********
         * 2 1 3 *
         *********/

        local1 = (k == 0);
        local2 = (k == n1 - 1);
        local3 = (j == 0 || i == n3 - 1);
        local4 = (j == n2 - 1 || i == 0);
        local5 = (k == n1 - 1 || j == 0 || i == 0);
        local6 = (k == 0 || j == n2 - 1 || i == n3 - 1);

        local7 = (j == 0);
        local8 = (j == n2 - 1);
        local9 = (k == n1 - 1 || j == 0);
        local10 = (k == 0 || j == n2 - 1);
        local11 = (i == 0);
        local12 = (i == n3 - 1);
        local13 = (k == 0 || i == n3 - 1);
        local14 = (k == n1 - 1 || i == 0);
        }

      else if (m[0] * m[7] == 1.0)
        {
        /*********
         * 1 3 2 *
         *********/

        local1 = (k == n1 - 1 || j == 0);
        local2 = (k == 0 || j == n2 - 1);
        local3 = (i == n3 - 1);
        local4 = (i == 0);
        local5 = (k == n1 - 1 || j == n2 - 1 || i == 0);
        local6 = (k == 0 || j == 0 || i == n3 - 1);

        local7 = (k == n1 - 1);
        local8 = (k == 0);
        local9 = (j == n2 - 1);
        local10 = (j == 0);
        local11 = (k == n1 - 1 || i == 0);
        local12 = (k == 0 || i == n3 - 1);
        local13 = (j == 0 || i == n3 - 1);
        local14 = (j == n2 - 1 || i == 0);
        }

      else if (m[2] * m[3] == 1.0)
        {
        /*********
         * 2 3 1 *
         *********/

        local1 = (j == 0);
        local2 = (j == n2 - 1);
        local3 = (k == 0 || i == n3 - 1);
        local4 = (k == n1 - 1 || i == 0);
        local5 = (k == n1 - 1 || j == 0 || i == n3 - 1);
        local6 = (k == 0 || j == n2 - 1 || i == 0);

        local7 = (j == 0 || i == n3 - 1);
        local8 = (j == n2 - 1 || i == 0);
        local9 = (i == n3 - 1);
        local10 = (i == 0);
        local11 = (k == n1 - 1 || j == 0);
        local12 = (k == 0 || j == n2 - 1);
        local13 = (k == 0);
        local14 = (k == n1 - 1);
        }

      else if (m[1] * m[5] == 1.0)
        {
        /*********
         * 3 1 2 *
         *********/

        local1 = (j == 0 || i == n3 - 1);
        local2 = (j == n2 - 1 || i == 0);
        local3 = (k == n3 - 1);
        local4 = (k == 0);
        local5 = (k == n1 - 1 || j == 0 || i == 0);
        local6 = (k == 0 || j == n2 - 1 || i == n3 - 1);

        local7 = (k == n1 - 1 || j == 0);
        local8 = (k == 0 || j == n2 - 1);
        local9 = (k == n1 - 1 || i == 0);
        local10 = (k == 0 || i == n3 - 1);
        local11 = (j == 0);
        local12 = (j == n2 - 1);
        local13 = (i == n3 - 1);
        local14 = (i == 0);
        }

      else if (m[2] * m[6] == 1.0)
        {
        /*********
         * 3 2 1 *
         *********/

        local1 = (k == 0 || i == n3 - 1);
        local2 = (k == n1 - 1 || i == 0);
        local3 = (j == n2 - 1);
        local4 = (j == 0);
        local5 = (k == n1 - 1 || j == 0 || i == n3 - 1);
        local6 = (k == 0 || j == n2 - 1 || i == 0);

        local7 = (i == n3 - 1);
        local8 = (i == 0);
        local9 = (k == n1 - 1);
        local10 = (k == 0);
        local11 = (j == 0 || i == n3 - 1);
        local12 = (j == n2 - 1 || i == 0);
        local13 = (k == 0 || j == n2 - 1);
        local14 = (k == n1 - 1 || j == 0);
        }

      else
        {
        /***********************************
         * this unneeded initialization    *
         * avoids a O3 compilation warning *
         ***********************************/

        local1 = local2 = local3 = local4 = local5 = local6 = local7 = 0;
        local8 = local9 = local10 = local11 = local12 = local13 = local14 = 0;
        }

      /**************
       * draw faces *
       **************/

      gamgi_math_polygon_loops_v24 (cell->loops, &loops_offset, points_offset,
      (global && local1), (cell->borders == GAMGI_PHYS_ALL || (global && local2)),
      (global && local3), (cell->borders == GAMGI_PHYS_ALL || (global && local4)),
      (global && local5), (cell->borders == GAMGI_PHYS_ALL || (global && local6)),
      (global && local7), (cell->borders == GAMGI_PHYS_ALL || (global && local8)),
      (global && local9), (cell->borders == GAMGI_PHYS_ALL || (global && local10)),
      (global && local11), (cell->borders == GAMGI_PHYS_ALL || (global && local12)),
      (global && local13), (cell->borders == GAMGI_PHYS_ALL || (global && local14)));

      /*******************************
       * number of vertices per cell *
       *******************************/

      points_offset += 24;
      }
    }
  }
}

static void static_orthorhombic_i_high (gamgi_cell *cell,
double a, double b, double c, double *m)
{
int i, j, k, n1, n2, n3;
int *loops;
int vertices_offset;
double x, y, z;
double ac, bc, cc;
double t[9], v[3];
double *points;
gamgi_bool global;
gamgi_bool local1, local2, local3, local4, local5, local6;
gamgi_bool local7, local8, local9, local10, local11, local12;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

/***************
 * 18 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 18);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 8 faces x 4 vertices + 4 faces x 6 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (8 * 5 + 4 * 7);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 12;
cell->loops = loops;

/**********
 * offset *
 **********/

loops_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

gamgi_math_matrix_inverse (m, t);

ac = pow(a, 2) / (4*c);
bc = pow(b, 2) / (4*c);
cc = pow(c, 2) / (4*c);

global = (cell->faces == TRUE || cell->borders != FALSE);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /*****************************************
       * vertices inside the conventional cell *
       *****************************************/

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, 0.0, -ac -bc -cc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, 0.0, 0.0, +ac +bc +cc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * vertices on edges of the conventional cell *
       **********************************************/

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, -b/2, -cc +ac +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, -b/2, -cc +ac +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, +b/2, -cc +ac +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, +b/2, -cc +ac +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, -b/2, +cc -ac -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, -b/2, +cc -ac -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, +b/2, +cc -ac -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, +b/2, +cc -ac -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * vertices on faces of the conventional cell *
       **********************************************/

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -b/2, -ac -cc +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -b/2, +ac +cc -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 13 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, +b/2, -ac -cc +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 14 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, +b/2, +ac +cc -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 15 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, 0.0, -bc -cc +ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 16 *
       *************/
      
      gamgi_math_vector_absolute (v, -a/2, 0.0, +bc +cc -ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 17 *
       *************/

      gamgi_math_vector_absolute (v, +a/2, 0.0, -bc -cc +ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 18 *
       *************/

      gamgi_math_vector_absolute (v, +a/2, 0.0, +bc +cc -ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * Set flags defining which faces must be     *
       * drawn, according to the cell orientation.  *
       *                                            *
       * To get these flags, use GAMGI to create a  *
       * Wigner-Seitz solid cell, where all faces   *
       * are drawn but one, and a wired 3x2x1 (to   *
       * recognize the axes) primitive cell: by     *
       * comparing the two cells, see when the face *
       * missing must be drawn and get the flags.   *
       **********************************************/

      if (m[0] * m[4] == 1.0)
        {
        /*********
         * 1 2 3 *
         *********/

        local1 = (k == 0 || j == 0);
        local2 = (k == n1 - 1 || j == n2 - 1);
        local3 = (j == 0 || i == 0);
        local4 = (j == n2 - 1 || i == n3 - 1);

        local5 = (k == 0 || j == 0 || i == 0);
        local6 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local7 = (i == 0);
        local8 = (i == n3 - 1);
        local9 = (k == 0);
        local10 = (k == n1 - 1);
        local11 = (j == 0);
        local12 = (j == n2 - 1);
        }


      else if (m[3] * m[8] == 1.0)
        {
        /*********
         * 2 1 3 *
         *********/

        local1 = (j == n2 - 1 || i == n3 - 1);
        local2 = (j == 0 || i == 0);
        local3 = (k == 0 || j == 0);
        local4 = (k == n1 - 1 || j == n2 - 1);;

	local5 = (k == 0);
        local6 = (k == n1 - 1);
	local7 = (k == 0 || j == 0 || i == 0);
        local8 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local9 = (j == n2 - 1);
        local10 = (j == 0);
        local11 = (i == n3 - 1);
        local12 = (i == 0);
        }

      else if (m[0] * m[7] == 1.0)
        {
        /*************
         * 1|2 3 1|2 *
         *************/

        local1 = (k == 0 || j == 0);
        local2 = (k == n1 - 1 || j == n2 - 1);
        local3 = (k == n1 - 1 || i == n3 - 1);
        local4 = (k == 0 || i == 0);

        local5 = (j == 0);
        local6 = (j == n2 - 1);
        local7 = (k == n1 - 1);
        local8 = (k == 0);
        local9 = (k == 0 || j == 0 || i == 0);
        local10 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local11 = (i == n3 - 1);
        local12 = (i == 0);
        }

      else if (m[1] * m[5] == 1.0)
        {
        /*************
         * 3 1|2 1|2 *
         *************/

        local1 = (j == n2 - 1 || i == n3 - 1);
        local2 = (j == 0 || i == 0);
        local3 = (k == n1 - 1 || i == n3 - 1);
        local4 = (k == 0 || i == 0);

        local5 = (i == n3 - 1);
        local6 = (i == 0);
        local7 = (j == 0);
        local8 = (j == n2 - 1);
        local9 = (k == 0);
        local10 = (k == n1 - 1);
        local11 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local12 = (k == 0 || j == 0 || i == 0);
        }

      else
        {
        /***********************************
         * this unneeded initialization    *
         * avoids a O3 compilation warning *
         ***********************************/

        local1 = local2 = local3 = local4 = local5 = local6 = 0;
        local7 = local8 = local9 = local10 = local11 = local12 = 0;
        }

      /**************
       * draw faces *
       **************/

      gamgi_math_polygon_loops_v18 (cell->loops, &loops_offset, points_offset,
      (global && local1), (cell->borders == GAMGI_PHYS_ALL || (global && local2)),
      (global && local3), (cell->borders == GAMGI_PHYS_ALL || (global && local4)),
      (global && local5), (cell->borders == GAMGI_PHYS_ALL || (global && local6)),
      (global && local7), (cell->borders == GAMGI_PHYS_ALL || (global && local8)),
      (global && local9), (cell->borders == GAMGI_PHYS_ALL || (global && local10)),
      (global && local11), (cell->borders == GAMGI_PHYS_ALL || (global && local12)));

      /*******************************
       * number of vertices per cell *
       *******************************/

      points_offset += 18;
      }
    }
  }

}

static void static_orthorhombic_i_low (gamgi_cell *cell, 
double a, double b, double c, double *m)
{
int i, j, k, n1, n2, n3;
int *loops;
int vertices_offset;
double x, y, z;
double aa, ba, ca, ab, bb, cb, ac, bc, cc;
double t[9], v[3];
double *points;
gamgi_bool global;
gamgi_bool local1, local2, local3, local4, local5, local6, local7;
gamgi_bool local8, local9, local10, local11, local12, local13, local14;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

/************************************************
 * add 3 points to the end of the points array, *
 * to store the axes changing coordinates x,y,z *
 ************************************************/

/***************
 * 24 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 24);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 8 faces x 6 vertices + 6 faces x 4 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (8 * 7 + 6 * 5);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 14;
cell->loops = loops;

/**********
 * offset *
 **********/

loops_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

gamgi_math_matrix_inverse (m, t);

aa = pow(a, 2) / (4*a);
ba = pow(b, 2) / (4*a);
ca = pow(c, 2) / (4*a);

ab = pow(a, 2) / (4*b);
bb = pow(b, 2) / (4*b);
cb = pow(c, 2) / (4*b);

ac = pow(a, 2) / (4*c);
bc = pow(b, 2) / (4*c);
cc = pow(c, 2) / (4*c);

global = (cell->faces == TRUE || cell->borders != FALSE);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /****************************************************
       * vertices on face (-100) of the conventional cell *
       ****************************************************/

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, -bb -cb +ab, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, 0.0, +bc +cc -ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, +bb +cb -ab, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, 0.0, -bc -cc +ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (100) of the conventional cell *
       ***************************************************/

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, -bb -cb +ab, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, 0.0, -bc -cc +ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, +bb +cb -ab, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, +a/2, 0.0, +bc +cc -ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (0-10) of the conventional cell *
       ****************************************************/

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, +aa +ca -ba, -b/2, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -b/2, +ac +cc -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, -aa -ca +ba, -b/2, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -b/2, -ac -cc +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (010) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 13 *
       *************/

      gamgi_math_vector_absolute (v, +aa +ca -ba, +b/2, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 14 *
       *************/
      
      gamgi_math_vector_absolute (v, 0.0, +b/2, -ac -cc +bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 15 *
       *************/

      gamgi_math_vector_absolute (v, -aa -ca +ba, +b/2, 0.0);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 16 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, +b/2, +ac +cc -bc);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (00-1) of the conventional cell *
       ****************************************************/

      /*************
       * vertex 17 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -ab -bb +cb, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 18 *
       *************/

      gamgi_math_vector_absolute (v, -aa -ba +ca, 0.0, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 19 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, +ab +bb -cb, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 20 *
       *************/

      gamgi_math_vector_absolute (v, +aa +ba -ca, 0.0, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (001) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 21 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, -ab -bb +cb, +c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 22 *
       *************/

      gamgi_math_vector_absolute (v, +aa +ba -ca, 0.0, +c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 23 *
       *************/

      gamgi_math_vector_absolute (v, 0.0, +ab +bb -cb, +c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 24 *
       *************/

      gamgi_math_vector_absolute (v, -aa -ba +ca, 0.0, +c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * Set flags defining which faces must be     *
       * drawn, according to the cell orientation.  *
       *                                            *
       * To get these flags, use GAMGI to create a  *
       * Wigner-Seitz solid cell, where all faces   *
       * are drawn but one, and a wired 3x2x1 (to   *
       * recognize the axes) primitive cell: by     *
       * comparing the two cells, see when the face *
       * missing must be drawn and get the flags.   *
       **********************************************/

      if (m[0] * m[4] == 1.0)
        {
        /*********
         * 1 2 3 *
         *********/

        local1 = (k == 0 || j == 0);
        local2 = (k == n1 - 1 || j == n2 - 1);
        local3 = (j == 0 || i == 0);
        local4 = (j == n2 - 1 || i == n3 - 1);
        local5 = (k == 0 || i == 0);
        local6 = (k == n1 - 1 || i == n3 - 1);

        local7 = (k == 0 || j == 0 || i == 0);
        local8 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local9 = (i == 0);
        local10 = (i == n3 - 1);
        local11 = (k == 0);
        local12 = (k == n1 - 1);
        local13 = (j == 0);
        local14 = (j == n2 - 1);
        }

      else if (m[3] * m[8] == 1.0)
        {
        /*********
         * 2 1 3 *
         *********/

        local1 = (j == n2 - 1 || i == n3 - 1);
        local2 = (j == 0 || i == 0);
        local3 = (k == 0 || j == 0);
        local4 = (k == n1 - 1 || j == n2 - 1);
        local5 = (k == 0 || i == 0);
        local6 = (k == n1 - 1 || i == n3 - 1);

        local7 = (k == 0);
        local8 = (k == n1 - 1);
        local9 = (k == 0 || j == 0 || i == 0);
        local10 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local11 = (j == n2 - 1);
        local12 = (j == 0);
        local13 = (i == n1 - 1);
        local14 = (i == 0);
        }

      else if (m[0] * m[7] == 1.0)
        {
        /*********
         * 1 3 2 *
         *********/

        local1 = (k == 0 || j == 0);
        local2 = (k == n1 - 1 || j == n2 - 1);
        local3 = (k == n1 - 1 || i == n3 - 1);
        local4 = (k == 0 || i == 0);
        local5 = (j == 0 || i == 0);
        local6 = (j == n2 - 1 || i == n3 - 1);

        local7 = (j == 0);
        local8 = (j == n2 - 1);
        local9 = (k == n1 - 1);
        local10 = (k == 0);
        local11 = (k == 0 || j == 0 || i == 0);
        local12 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local13 = (i == n3 - 1);
        local14 = (i == 0);
        }

      else if (m[2] * m[3] == 1.0)
        {
        /*********
         * 2 3 1 *
         *********/

        local1 = (k == 0 || i == 0);
        local2 = (k == n2 - 1 || i == n3 - 1);
        local3 = (k == 0 || j == 0);
        local4 = (k == n1 - 1 || j == n2 - 1);
        local5 = (j == 0 || i == 0);
        local6 = (j == n2 - 1 || i == n3 - 1);

        local7 = (k == 0 || j == 0 || i == 0);
        local8 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local9 = (j == 0);
        local10 = (j == n2 - 1);
        local11 = (i == 0);
        local12 = (i == n3 - 1);
        local13 = (k == 0);
        local14 = (k == n1 - 1);
        }

      else if (m[1] * m[5] == 1.0)
        {
        /*********
         * 3 1 2 *
         *********/

        local1 = (j == n2 - 1 || i == n3 - 1);
        local2 = (j == 0 || i == 0);
        local3 = (k == n1 - 1 || i == n3 - 1);
        local4 = (k == 0 || i == 0);
        local5 = (k == 0 || j == 0);
        local6 = (k == n1 - 1 || j == n2 - 1);

        local7 = (i == n3 - 1);
        local8 = (i == 0);
        local9 = (j == 0);
        local10 = (j == n2 - 1);
        local11 = (k == 0);
        local12 = (k == n1 - 1);
        local13 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local14 = (k == 0 || j == 0  || i == 0);
        }

      else if (m[2] * m[6] == 1.0)
        {
        /*********
         * 3 2 1 *
         *********/
  
        local1 = (k == 0 || i == 0);
        local2 = (k == n1 - 1 || i == n3 - 1);
        local3 = (j == n2 - 1 || i == n3 - 1);
        local4 = (j == 0 || i == 0);
        local5 = (k == 0 || j == 0);
        local6 = (k == n1 - 1 || j == n2 - 1);

        local7 = (k == 0);
        local8 = (k == n1 - 1);
        local9 = (i == n3 - 1);
        local10 = (i == 0);
        local11 = (k == 0 || j == 0 || i == 0);
        local12 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);
        local13 = (j == n2 - 1);
        local14 = (j == 0);
        }

      else
        {
        /***********************************
         * this unneeded initialization    *
         * avoids a O3 compilation warning *
         ***********************************/

        local1 = local2 = local3 = local4 = local5 = local6 = local7 = 0;
        local8 = local9 = local10 = local11 = local12 = local13 = local14 = 0;
        }

      /**************
       * draw faces *
       **************/

      gamgi_math_polygon_loops_v24 (cell->loops, &loops_offset, points_offset,
      (global && local1), (cell->borders == GAMGI_PHYS_ALL || (global && local2)),
      (global && local3), (cell->borders == GAMGI_PHYS_ALL || (global && local4)),
      (global && local5), (cell->borders == GAMGI_PHYS_ALL || (global && local6)),
      (global && local7), (cell->borders == GAMGI_PHYS_ALL || (global && local8)),
      (global && local9), (cell->borders == GAMGI_PHYS_ALL || (global && local10)),
      (global && local11), (cell->borders == GAMGI_PHYS_ALL || (global && local12)),
      (global && local13), (cell->borders == GAMGI_PHYS_ALL || (global && local14)));

      /*******************************
       * number of vertices per cell *
       *******************************/
 
      points_offset += 24;
      }
    }
  }
}

static void static_rhombohedral_high (gamgi_cell *cell,
double a, double b, double c, double *m)
{
int i, j, k, n1, n2, n3;
int *loops;
int vertices_offset;
double x, y, z;
double aa, ac;
double t[9], v[3];
double *points;
gamgi_bool global;
gamgi_bool local1, local2, local3, local4, local5, local6;
gamgi_bool local7, local8, local9, local10, local11, local12;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

/***************
 * 14 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 14);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 6 faces x 4 vertices + 6 faces x 4 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (6 * 5 + 6 * 5);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 12;
cell->loops = loops;

/**********
 * offset *
 **********/

loops_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

gamgi_math_matrix_inverse (m, t);

aa = a / sqrt (3);
ac = pow(a, 2) / (2*c);

global = (cell->faces == TRUE || cell->borders != FALSE);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /****************************************************
       * center vertices, maximum height, above and below *
       ****************************************************/

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, 0, 0, -c/6 - ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, 0, 0, c/6 + ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*****************************************************
       * around vertices, direct rotating, above and below *
       *****************************************************/

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, a/2, aa/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, a/2, aa/2, c/6 - ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, 0, aa, -c/6 + ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (v, 0, aa, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, aa/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, aa/2, c/6 - ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, -a/2, -aa/2, -c/6 + ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, -a/2, -aa/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, 0, -aa, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, 0, -aa, c/6 - ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 13 *
       *************/

      gamgi_math_vector_absolute (v, a/2, -aa/2, -c/6 + ac);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 14 *
       *************/

      gamgi_math_vector_absolute (v, a/2, -aa/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * Set flags defining which faces must be     *
       * drawn, according to the cell orientation.  *
       *                                            *
       * To get these flags, use GAMGI to create a  *
       * Wigner-Seitz solid cell, where all faces   *
       * are drawn but one, and a wired 3x2x1 (to   *
       * recognize the axes) primitive cell: by     *
       * comparing the two cells, see when the face *
       * missing must be drawn and get the flags.   *
       **********************************************/

      local1 = (i == 0);
      local2 = (k == 0);
      local3 = (j == 0);
      local4 = (i == n3 - 1);
      local5 = (k == n1 - 1);
      local6 = (j == n2 - 1);

      local7 = (k == n1 - 1 || j == 0);
      local8 = (k == n1 - 1 || i == 0);
      local9 = (j == n2 - 1 || i == 0);
      local10 = (k == 0 || j == n2 - 1);
      local11 = (k == 0 || i == n3 - 1);
      local12 = (j == 0 || i == n3 - 1);

      /**************
       * draw faces *
       **************/

      gamgi_math_polygon_loops_v14 (cell->loops, &loops_offset, points_offset,
      (global && local1), (cell->borders == GAMGI_PHYS_ALL || (global && local2)),
      (global && local3), (cell->borders == GAMGI_PHYS_ALL || (global && local4)),
      (global && local5), (cell->borders == GAMGI_PHYS_ALL || (global && local6)),
      (global && local7), (cell->borders == GAMGI_PHYS_ALL || (global && local8)),
      (global && local9), (cell->borders == GAMGI_PHYS_ALL || (global && local10)),
      (global && local11), (cell->borders == GAMGI_PHYS_ALL || (global && local12)));

      /*******************************
       * number of vertices per cell *
       *******************************/

      points_offset += 14;
      }
    }
  }
}

static void static_rhombohedral_low (gamgi_cell *cell, 
double a, double b, double c, double *m)
{
int i, j, k, n1, n2, n3;
int *loops;
int vertices_offset;
double x, y, z;
double s, s3, d, d3;
double t[9], v[3];
double *points;
gamgi_bool global;
gamgi_bool local1, local2, local3, local4, local5, local6, local7;
gamgi_bool local8, local9, local10, local11, local12, local13, local14;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

/***************
 * 24 vertices *
 ***************/

cell->n_points = cell->n_nodes * (1 + 24);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 8 faces x 6 vertices + 6 faces x 4 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cell->n_nodes * (8 * 7 + 6 * 5);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cell->n_nodes * 14;
cell->loops = loops;

/**********
 * offset *
 **********/

loops_offset = 1;
points_offset = cell->n_nodes;
vertices_offset = 3 * points_offset;

gamgi_math_matrix_inverse (m, t);

s = a/6 + c*c/(9*a);
s3 = sqrt(3) * s;
d = a/6 - c*c/(9*a);
d3 = sqrt(3) * d;

global = (cell->faces == TRUE || cell->borders != FALSE);

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*cell->p1[0] + j*cell->p2[0] + i*cell->p3[0];
      y = k*cell->p1[1] + j*cell->p2[1] + i*cell->p3[1];
      z = k*cell->p1[2] + j*cell->p2[2] + i*cell->p3[2];

      /****************************************************
       * vertices on face (-100) of the conventional cell *
       ****************************************************/

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (v, -d, -d3, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (v, d, -d3, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (v, d, -s3, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (v, -d, -s3, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (100) of the conventional cell *
       ***************************************************/

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (v, -d, s3, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/
      
      gamgi_math_vector_absolute (v, -d, d3, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (v, d, d3, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (v, d, s3, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (0-10) of the conventional cell *
       ****************************************************/

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (v, (d - 3*s)/2, (d3 + s3)/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (v, -d, d3, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (v, -2*d,0, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (v, (-d - 3*s)/2, (-d3 + s3)/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (010) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 13 *
       *************/

      gamgi_math_vector_absolute (v, 2*d, 0, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 14 *
       *************/

      gamgi_math_vector_absolute (v, d, -d3, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 15 *
       *************/

      gamgi_math_vector_absolute (v, (-d + 3*s)/2, (-d3 - s3)/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 16 *
       *************/

      gamgi_math_vector_absolute (v, (d + 3*s)/2, (d3 - s3)/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /****************************************************
       * vertices on face (00-1) of the conventional cell *
       ****************************************************/

      /*************
       * vertex 17 *
       *************/

      gamgi_math_vector_absolute (v, (-d - 3*s)/2, (d3 - s3)/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 18 *
       *************/

      gamgi_math_vector_absolute (v, (d - 3*s)/2, (-d3 - s3)/2, c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 19 *
       *************/

      gamgi_math_vector_absolute (v, -d, -d3, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 20 *
       *************/

      gamgi_math_vector_absolute (v, -2*d, 0, c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /***************************************************
       * vertices on face (001) of the conventional cell *
       ***************************************************/

      /*************
       * vertex 21 *
       *************/

      gamgi_math_vector_absolute (v, d, d3, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 22 *
       *************/

      gamgi_math_vector_absolute (v, (-d + 3*s)/2, (d3 + s3)/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 23 *
       *************/

      gamgi_math_vector_absolute (v, (d + 3*s)/2, (-d3 + s3)/2, -c/6);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /*************
       * vertex 24 *
       *************/

      gamgi_math_vector_absolute (v, 2*d, 0, -c/2);
      gamgi_math_matrix_vector (t, v, points + vertices_offset);
      gamgi_math_vector_relative (points + vertices_offset, x, y, z);
      vertices_offset += 3;

      /**********************************************
       * Set flags defining which faces must be     *
       * drawn, according to the cell orientation.  *
       *                                            *
       * To get these flags, use GAMGI to create a  *
       * Wigner-Seitz solid cell, where all faces   *
       * are drawn but one, and a wired 3x2x1 (to   *
       * recognize the axes) primitive cell: by     *
       * comparing the two cells, see when the face *
       * missing must be drawn and get the flags.   *
       **********************************************/

      local1 = (k == 0 || j == 0);
      local2 = (k == n1 - 1 || j == n2 - 1);
      local3 = (k == 0 || i == 0);
      local4 = (k == n1 - 1 || i == n3 - 1);
      local5 = (j == n2 - 1 || i == n3 - 1);
      local6 = (j == 0 || i == 0);

      local7 = (k == 0);
      local8 = (k == n1 - 1);
      local9 = (j == n2 - 1);
      local10 = (j == 0);
      local11 = (i == n3 - 1);
      local12 = (i == 0);
      local13 = (k == 0 || j == 0 || i == 0);
      local14 = (k == n1 - 1 || j == n2 - 1 || i == n3 - 1);

      /**************
       * draw faces *
       **************/

      gamgi_math_polygon_loops_v24 (cell->loops, &loops_offset, points_offset,
      (global && local1), (cell->borders == GAMGI_PHYS_ALL || (global && local2)),
      (global && local3), (cell->borders == GAMGI_PHYS_ALL || (global && local4)),
      (global && local5), (cell->borders == GAMGI_PHYS_ALL || (global && local6)),
      (global && local7), (cell->borders == GAMGI_PHYS_ALL || (global && local8)),
      (global && local9), (cell->borders == GAMGI_PHYS_ALL || (global && local10)),
      (global && local11), (cell->borders == GAMGI_PHYS_ALL || (global && local12)),
      (global && local13), (cell->borders == GAMGI_PHYS_ALL || (global && local14)));

      /*******************************
       * number of vertices per cell *
       *******************************/
 
      points_offset += 24;
      }
    }
  }
}

void gamgi_phys_wigner_create (gamgi_cell *cell)
{
double m[9], ra[9], rc[9];
double a, b, c, ab, ac, bc;

gamgi_math_matrix_unit (m);
gamgi_math_matrix_absolute (rc, 0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
gamgi_math_matrix_absolute (ra, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

cell->n_nodes = cell->n1 * cell->n2 * cell->n3;

switch (cell->lattice)
  {
  case GAMGI_PHYS_HEXAGONAL_P:
  static_orthorhombic_c (cell, cell->a, sqrt(3) * cell->b, cell->c, m);
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  if (cell->c < sqrt(3.0/2.0) * cell->a)
    static_rhombohedral_low (cell, cell->a, cell->b, cell->c, m);
  else
    static_rhombohedral_high (cell, cell->a, cell->b, cell->c, m);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C:
  gamgi_math_sort_direction (cell->a, cell->b, &a, &b, cell->ac, cell->bc, &ac, &bc, rc, m);
  static_orthorhombic_c (cell, a, b, cell->c, m);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_F:
  gamgi_math_sort_direction (cell->a, cell->b, &a, &b, cell->ac, cell->bc, &ac, &bc, rc, m);
  gamgi_math_sort_direction (b, cell->c, &b, &c, cell->ab, ac, &ab, &ac, ra, m);
  gamgi_math_sort_direction (a, b, &a, &b, ac, bc, &ac, &bc, rc, m);
  static_orthorhombic_f (cell, a, b, c, m);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_I:
  gamgi_math_sort_direction (cell->a, cell->b, &a, &b, cell->ac, cell->bc, &ac, &bc, rc, m);
  gamgi_math_sort_direction (b, cell->c, &b, &c, cell->ab, ac, &ab, &ac, ra, m);
  if (pow (c, 2) < pow (a, 2) + pow(b, 2))
    {
    gamgi_math_sort_direction (a, b, &a, &b, ac, bc, &ac, &bc, rc, m);
    static_orthorhombic_i_low (cell, a, b, c, m);
    }
  else 
    static_orthorhombic_i_high (cell, a, b, c, m);
  break;

  case GAMGI_PHYS_TETRAGONAL_I:
  if (cell->c < sqrt(2) * cell->a)
    static_orthorhombic_i_low (cell, cell->a, cell->b, cell->c, m);
  else
    static_orthorhombic_i_high (cell, cell->a, cell->b, cell->c, m);
  break;

  case GAMGI_PHYS_CUBIC_I:
  static_orthorhombic_i_low (cell, cell->a, cell->b, cell->c, m);
  break;

  case GAMGI_PHYS_CUBIC_F:
  static_orthorhombic_f (cell, cell->a, cell->b, cell->c, m);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_P: case GAMGI_PHYS_TETRAGONAL_P: case GAMGI_PHYS_CUBIC_P:
  static_orthorhombic_p (cell);
  break;
  }

/*************
 * add nodes *
 *************/

gamgi_math_polygon_parallelepiped (cell->points, 0, cell->n1 - 1, 
0, cell->n2 - 1, 0, cell->n3 - 1, cell->p1, cell->p2, cell->p3);

static_clean_wigner (cell);
}
