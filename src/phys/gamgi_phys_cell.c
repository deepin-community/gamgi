/********************************************
 *
 * $GAMGI/src/phys/gamgi_phys_cell.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_polygon.h"
#include "gamgi_phys_wigner.h"

int gamgi_phys_cell_conventional_I (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points)
{
int i, j, k;

/*********************************
 * store coordinates for i nodes *
 *********************************/

for (i = i0; i <= i1; i++)
  {
  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      points[offset + 0] = (k + 0.5) * cell->a1[0];
      points[offset + 1] = (j + 0.5) * cell->a2[1];
      points[offset + 2] = (i + 0.5) * cell->a3[2];
      offset += 3;
      }
    }
  }

return offset;
}

int gamgi_phys_cell_conventional_A (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points)
{
int i, j, k;

/*********************************
 * store coordinates for A nodes *
 *********************************/

for (i = i0; i <= i1; i++)
  {
  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      points[offset + 0] = k * cell->a1[0];
      points[offset + 1] = (j +  0.5) * cell->a2[1];
      points[offset + 2] = (i + 0.5) * cell->a3[2];
      offset += 3;
      }
    }
  }

return offset;
}

int gamgi_phys_cell_conventional_B (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points)
{
int i, j, k;

/*********************************
 * store coordinates for B nodes *
 *********************************/

for (i = i0; i <= i1; i++)
  {
  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      points [offset + 0] = (k + 0.5) * cell->a1[0];
      points [offset + 1] = j * cell->a2[1];
      points [offset + 2] = (i + 0.5) * cell->a3[2];
      offset += 3;
      }
    }
  }

return offset;
}

int gamgi_phys_cell_conventional_C (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points)
{
int i, j, k;

/*********************************
 * store coordinates for C nodes *
 *********************************/

for (i = i0; i <= i1; i++)
  {
  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      points[offset + 0] = (k + 0.5) * cell->a1[0] + i*cell->a3[0];
      points[offset + 1] = (j + 0.5) * cell->a2[1];
      points[offset + 2] = i * cell->a3[2];
      offset += 3;
      }
    }
  }

return offset;
}

int gamgi_phys_cell_conventional_R (gamgi_cell *cell, int offset,
int k0, int k1, int j0, int j1, int i0, int i1, double *points)
{
int i, j, k;
double ii, jj, kk;

/*******************************************************
 * store coordinates for R nodes: add a complete layer * 
 * for the first node, and then repeat for the second  *
 * node, so R nodes are consistently ordered along the *
 * c axis. Pattern rules can then be used to control   *
 * occupancy along the direction normal to the layers. *
 *******************************************************/

for (i = i0; i <= i1; i++)
  {
  /**********************************
   * Obverse convention for R nodes *
   * when i is even: 2/3 1/3 1/3    *
   * when i is odd:  1/3 2/3 2/3    *
   *                                *
   * integer division is guaranteed *
   * to work only for integers > 0! *
   **********************************/

  kk = ((abs (i) + 1) % 2 + 1) / 3.0;
  jj = ((abs (i) + 0) % 2 + 1) / 3.0;
  ii = (i - abs (i) % 2) / 2.0 + jj;

  for (j = j0; j <= j1; j++)
    {
    for (k = k0; k <= k1; k++)
      {
      points[offset + 0] = (k + kk) * cell->a1[0] + (j + jj) * cell->a2[0];
      points[offset + 1] = (k + kk) * cell->a1[1] + (j + jj) * cell->a2[1];
      points[offset + 2] = ii * cell->a3[2];
      offset += 3;
      }
    }
  }

return offset;
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static int static_conventional_P (gamgi_cell *cell)
{
int n1, n2, n3;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

/*****************************************
 * build array containing all the points *
 * required for conventional unit cells. *
 *                                       *
 *     store coordinates for P nodes     *
 *****************************************/

cell->n_points = cell->n_nodes;

cell->points = (double *) malloc (cell->n_points * 3 * sizeof (double));
gamgi_math_polygon_parallelepiped (cell->points, 
0, n1, 0, n2, 0, n3, cell->a1, cell->a2, cell->a3);

return 3 * (n1 + 1) * (n2 + 1) * (n3 + 1);
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static int static_primitive_P (gamgi_cell *cell)
{
int n1, n2, n3;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

/*****************************************
 * build array containing all the points *
 * required for primitive unit cells.    *
 *                                       *
 *    store coordinates for p nodes      *
 *****************************************/

cell->n_points = cell->n_nodes = (n1 + 1) * (n2 + 1) * (n3 + 1);

cell->points = (double *) malloc (cell->n_points * 3 * sizeof (double));
gamgi_math_polygon_parallelepiped (cell->points,
0, n1, 0, n2, 0, n3, cell->p1, cell->p2, cell->p3);

return 3 * (n1 + 1) * (n2 + 1) * (n3 + 1);
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_conventional (gamgi_cell *cell)
{
int n1, n2, n3;
int offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

cell->n_nodes = (n1 + 1) * (n2 + 1) * (n3 + 1);

switch (cell->lattice)
  {
  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  cell->n_nodes += (n1 + 1) * n2 * n3 + (n2 + 1) * n1 * n3 + (n3 + 1) * n1 * n2;
  offset = static_conventional_P (cell);
  offset = gamgi_phys_cell_conventional_A (cell, offset,
  0, n1, 0, n2 - 1, 0, n3 - 1, cell->points);
  offset = gamgi_phys_cell_conventional_B (cell, offset,
  0, n1 - 1, 0, n2, 0, n3 - 1, cell->points);
  gamgi_phys_cell_conventional_C (cell, offset,
  0, n1 - 1, 0, n2 - 1, 0, n3, cell->points);
  break;

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I: 
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  cell->n_nodes += n1 * n2 * n3;
  offset = static_conventional_P (cell);
  gamgi_phys_cell_conventional_I (cell, offset,
  0, n1 - 1, 0, n2 - 1, 0, n3 - 1, cell->points);
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  cell->n_nodes += (n3 + 1) * n1 * n2;
  offset = static_conventional_P (cell);
  gamgi_phys_cell_conventional_C (cell, offset,
  0, n1 - 1, 0, n2 - 1, 0, n3, cell->points);
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  cell->n_nodes += 2 * n1 * n2 * n3;
  offset = static_conventional_P (cell);
  gamgi_phys_cell_conventional_R (cell, offset, 
  0, n1 - 1, 0, n2 - 1, 0, 2*n3 - 1, cell->points);
  break;

  default:
  offset = static_conventional_P (cell);
  break;
  }

}

/******************** external function *****************
 *                                                      *
 *                 GAMGI_PHYS_CELL_CLEAN                *
 *                                                      *
 * Each position in the mark vector describes a point   *
 * and indicates whether it is good (1) or bad (0).     *
 * Good points are copied to vector points_new, which   *
 * in turn becomes the new cell->points array. The      *
 * loops and lines arrays are also copied to loops_new  *
 * and lines_new, because the actual number of loops    *
 * and lines may be smaller than the total allocated    *
 * and because the points ordering has changed.         *
 *                                                      *
 ********************************************************/

void gamgi_phys_cell_clean (gamgi_cell *cell, 
int *mark, int *loops_new, int *lines_new)
{
double *points, *points_new;
int *loops, *lines;
int n_points, n_loops, n_lines, n_vertices, i, j;

/*************************************************
 * increment mark and copy points to new points *
 *************************************************/

loops = cell->loops;
lines = cell->lines;

if (loops != NULL || lines != NULL)
  {
  points = cell->points;

  i = 0;
  j = 1;
  while (i < cell->n_points)
    {
    if (mark [i] == 0) j++;
    else mark [i] = j;
    i++;
    }

  n_points = cell->n_points - j + 1;
  points_new = (double *) malloc (sizeof (double) * 3 * n_points);

  i = 0;
  j = 0;
  while (i < cell->n_points)
    {
    if (mark [i] != 0)
      { 
      gamgi_math_vector_copy (points + 3*i, points_new + 3*j);
      j++;
      }
    i++;
    }

  free (points);
  cell->points = points_new;
  cell->n_points = n_points;
  }

/***************************
 * copy loops to new loops *
 ***************************/

if (loops != NULL)
  {
  i = 0;
  n_loops = loops[i++];
  while (n_loops-- > 0)
    {
    if (loops[i] == -1) break;

    loops_new[i] = loops[i];

    n_vertices = loops[i++];
    while (n_vertices-- > 0)
      {
      loops_new[i] = loops[i] - mark[loops[i]] + 1;
      i++;
      }
    }
  loops_new[0] = loops[0] - n_loops - 1;

  free (loops);
  cell->loops = loops_new;
  }

/***************************
 * copy lines to new lines *
 ***************************/

if (lines != NULL)
  {
  i = 0;
  n_lines = lines[i++];
  while (n_lines-- > 0)
    {
    if (lines[i] == -1) break;

    lines_new[i] = lines[i] - mark[lines[i]] + 1;
    i++;
    lines_new[i] = lines[i] - mark[lines[i]] + 1;
    i++;
    }
  lines_new[0] = lines[0] - n_lines - 1;

  free (lines);
  cell->lines = lines_new;
  }

free (mark);
}

gamgi_enum gamgi_phys_cell_primitive (gamgi_enum lattice)
{
switch (lattice)
  {
  case GAMGI_PHYS_MONOCLINIC_C:
  return GAMGI_PHYS_MONOCLINIC_P;

  case GAMGI_PHYS_HEXAGONAL_R:
  return GAMGI_PHYS_HEXAGONAL_P;

  case GAMGI_PHYS_ORTHORHOMBIC_F:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  return GAMGI_PHYS_ORTHORHOMBIC_P;

  case GAMGI_PHYS_TETRAGONAL_I:
  return GAMGI_PHYS_TETRAGONAL_P;

  case GAMGI_PHYS_CUBIC_F:
  case GAMGI_PHYS_CUBIC_I:
  return GAMGI_PHYS_CUBIC_P;

  default:
  break;
  }

return lattice;
}

void gamgi_phys_cell_parameters (gamgi_cell *cell,
double a, double b, double c, double ab, double ac, double bc)
{
/*****************************************
 * the a,b,c,ab,ac,bc values are always  *
 * the conventional cell parameters      *
 *                                       *
 * this routine takes advantage of our   *
 * knowledge of the lattice to improve   *
 * the numerical accuracy of the lattice *
 * parameters used to build the cell     *
 *****************************************/

switch (cell->lattice)
  {
  case GAMGI_PHYS_TRICLINIC_P:
  cell->a = a; cell->b = b; cell->c = c;
  cell->ab = ab; cell->ac = ac; cell->bc = bc;
  break;

  case GAMGI_PHYS_MONOCLINIC_P:
  case GAMGI_PHYS_MONOCLINIC_C:
  cell->a = a; cell->b = b; cell->c = c;
  cell->ab = cell->bc = 90.0; cell->ac = ac;
  break;

  case GAMGI_PHYS_HEXAGONAL_P:
  case GAMGI_PHYS_HEXAGONAL_R:
  cell->a = cell->b = a; cell->c = c;
  cell->ab = 120.0; cell->ac = cell->bc = 90.0;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_P:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  cell->a = a; cell->b = b; cell->c = c;
  cell->ab = cell->ac = cell->bc = 90.0;
  break;

  case GAMGI_PHYS_TETRAGONAL_P:
  case GAMGI_PHYS_TETRAGONAL_I:
  cell->a = cell->b = a; cell->c = c;
  cell->ab = cell->ac = cell->bc = 90.0;
  break;

  case GAMGI_PHYS_CUBIC_P:
  case GAMGI_PHYS_CUBIC_I:
  case GAMGI_PHYS_CUBIC_F:
  cell->a = cell->b = cell->c = a;
  cell->ab = cell->ac = cell->bc = 90.0;
  break;
  }
}

void gamgi_phys_cell_primitive_index (int *range, int *index)
{
int i, j, k;
int offset = 0;

for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      index[offset + 0] = k;
      index[offset + 1] = j;
      index[offset + 2] = i;
      offset += 3;
      }
    }
  }

}

void gamgi_phys_cell_rombohedral_conventional (double primitive_a,
double primitive_bc, double *conventional_a, double *conventional_c)
{
double aux;

aux = 2 * sin (GAMGI_MATH_DEG_RAD * primitive_bc / 2.0);

*conventional_a = primitive_a * aux;
*conventional_c = primitive_a * 3 * sqrt (1 - aux * aux / 3.0);
}

void gamgi_phys_cell_rombohedral_primitive (double conventional_a,
double conventional_c, double *primitive_a, double *primitive_bc)
{
double aux;

aux = pow (conventional_c / conventional_a, 2);

*primitive_a = conventional_a * sqrt (aux + 3) / 3.0;
*primitive_bc = GAMGI_MATH_RAD_DEG * acos ((aux - 3.0 / 2.0) / (aux + 3.0));
}

/********************* external function *******************
 *                                                         *
 *               GAMGI_PHYS_CELL_HEXAGONAL_CLEAN           *
 *                                                         *
 * 1) use the index array to redirect points to equivalent *
 * points that appear before; 2) use the index array to    *
 * redirect loops and lines arrays to these equivalent     *
 * points and create loops_new and lines_new to allocate   *
 * the real number of loops and lines; 3) use the mark     *
 * array to mark the good (1) and bad (0) points; 4) call  *
 * clean () to create a new points array, containing       *
 * only the good points.                                   *
 ***********************************************************/

void gamgi_phys_cell_hexagonal_clean (gamgi_cell *cell)
{
int *index, *mark, *loops, *loops_new, *lines, *lines_new;
int n_loops, n_lines, n_vertices;
int n1, n2, n3, i, j, k, offset;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

index = (int *) calloc (n1 * n2 * n3 * 12, sizeof (int));

offset = 0;
for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      /************
       * vertex 1 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      offset++;

      /************
       * vertex 2 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (k < n1 - 1) index[offset + 12 + 5] = index[offset];
      if (j < n2 - 1) index[offset + 12 * n1 + 3] = index[offset];
      offset++;

      /************
       * vertex 3 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 2] = index[offset];
      offset++;

      /************
       * vertex 4 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (j < n2 - 1) index[offset + 12 * n1 + 5] = index[offset];
      if (k < n1 - 1) index[offset + 12 + 7] = index[offset];
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 2] = index[offset];
      offset++;

      /************
       * vertex 5 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      offset++;

      /************
       * vertex 6 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (k < n1 - 1) index[offset + 12 - 5] = index[offset];
      offset++;

      /************
       * vertex 7 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      offset++;

      /************
       * vertex 8 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (k > 0 && j < n2 - 1) index[offset + 12 * n1 - 12 - 2] = index[offset];
      if (j < n2 - 1) index[offset + 12 * n1 - 7] = index[offset];
      offset++;

      /************
       * vertex 9 *
       ************/

      if (index[offset] == 0) index[offset] = offset;
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 4] = index[offset];
      offset++;

      /*************
       * vertex 10 *
       *************/

      if (index[offset] == 0) index[offset] = offset;
      if (k < n1 - 1) index[offset + 12 - 7] = index[offset];
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 4] = index[offset];
      offset++;

      /*************
       * vertex 11 *
       *************/

      if (index[offset] == 0) index[offset] = offset;
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 4] = index[offset];
      offset++;

      /*************
       * vertex 12 *
       *************/

      if (index[offset] == 0) index[offset] = offset;
      if (k > 0 && j < n2 - 1) index[offset + 12 * n1 - 12 - 2] = index[offset];
      if (j < n2 - 1) index[offset + 12 * n1 - 9] = index[offset];
      if (i < n3 - 1) index[offset + 12 * n1 * n2 - 4] = index[offset];
      offset++;
      }
    }
  }

/***************************************
 * redirect loops and create new loops *
 ***************************************/

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
    while (n_vertices-- > 0)
      {
      loops[i] = index[loops[i]];
      i++;
      }
    }

  cell->n_loops = i;
  loops_new = (int *) malloc (sizeof (int) * cell->n_loops);
  }

/***************************************
 * redirect lines and create new lines *
 ***************************************/

lines_new = NULL;

lines = cell->lines;
if (lines != NULL)
  {
  i = 0;
  n_lines = lines[i++];
  while (n_lines-- > 0)
    {
    if (lines[i] == -1) break;

    lines[i] = index[lines[i]];
    i++;
    lines[i] = index[lines[i]];
    i++;
    }

  cell->n_lines = i;
  lines_new = (int *) malloc (sizeof (int) * cell->n_lines);
  }

/***********************************************
 * create array marking each point:            *
 * 0: the point is bad and must be discarded.  *
 * 1: the point is good and must be preserved. *
 ***********************************************/

mark = (int *) malloc (sizeof (int) * cell->n_points);

i = 0;
offset = n1 * n2 * n3 * 12;
while (i < offset)
  {
  if (index[i] == i) mark[i] = 1;
  else mark [i] = 0;
  i++;
  }
while (i < cell->n_points) mark [i++] = 1;

free (index);

gamgi_phys_cell_clean (cell, mark, loops_new, lines_new);

cell->n_nodes = cell->n_points;
}

void gamgi_phys_cell_hexagonal_create (gamgi_cell *cell)
{
int i, j, k, n1, n2, n3, cells;
int points_offset, vertices_offset;
int loops_offset, lines_offset;
int *loops, *lines;
double translate[3], c1[3], c2[3];
double a, c, x, y, z, aux;
double *points;
gamgi_bool k0, k1, j0, j1, i0, i1;
gamgi_bool local1, local2, local3, local4, local5, local6;
gamgi_bool local7, local8, local9, local10, local11, local12;
gamgi_bool local13, local14, local15, local16, local17, local18;
gamgi_bool global_all, global_faces, global_edges;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

cells = n1 * n2 * n3;

/***********************************************
 * 12 nodes per cell (left) plus central nodes *
 * (right) (common nodes are removed later)    *
 ***********************************************/

cell->n_points = cell->n_nodes = cells * 12 + n1 * n2 * (n3 + 1);
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

/***********************************************
 * 6 faces x 4 vertices + 2 faces x 6 vertices *
 ***********************************************/

i = cell->n_loops = 1 + cells * (6 * 5 + 2 * 7);
loops = (int *) malloc (sizeof (int) * i);
while (i-- > 1) loops[i] = -1;
loops[0] = cells * 8;
cell->loops = loops;

/************************************************
 * 2 x (6 vertical lines + 12 horizontal lines) *
 ************************************************/

i = cell->n_lines = 1 + cells * 2 * (6 + 12);
lines = (int *) malloc (sizeof (int) * i);
while (i-- > 1) lines[i] = -1;
lines[0] = cells * (6 + 12);
cell->lines = lines;

/**********
 * offset *
 **********/

loops_offset = 1;
lines_offset = 1;
points_offset = 0;
vertices_offset = 0;

gamgi_math_vector_add (cell->p1, cell->p2, translate);
gamgi_math_vector_add (translate, cell->p1, c1);
gamgi_math_vector_add (translate, cell->p2, c2);

a = cell->a;
c = cell->c;
aux = a * sqrt(3)/2;

global_edges = (cell->borders != GAMGI_PHYS_EDGES);
global_faces = (cell->borders == GAMGI_PHYS_FACES);
global_all = (cell->borders == GAMGI_PHYS_ALL);

/*******************
 * prismatic nodes *
 *******************/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      x = k*c1[0] + j*c2[0] + i*cell->p3[0];
      y = k*c1[1] + j*c2[1] + i*cell->p3[1];
      z = k*c1[2] + j*c2[2] + i*cell->p3[2];

      /************
       * vertex 1 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a/2, y - aux, z);
      vertices_offset += 3;

      /************
       * vertex 2 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a/2, y + aux, z);
      vertices_offset += 3;

      /************
       * vertex 3 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a/2, y - aux, z + c);
      vertices_offset += 3;

      /************
       * vertex 4 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a/2, y + aux, z + c);
      vertices_offset += 3;

      /************
       * vertex 5 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a/2, y - aux, z);
      vertices_offset += 3;

      /************
       * vertex 6 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a, y, z);
      vertices_offset += 3;

      /************
       * vertex 7 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a, y, z);
      vertices_offset += 3;

      /************
       * vertex 8 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a/2, y + aux, z);
      vertices_offset += 3;

      /************
       * vertex 9 *
       ************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a/2, y - aux, z + c);
      vertices_offset += 3;

      /*************
       * vertex 10 *
       *************/

      gamgi_math_vector_absolute (points + vertices_offset, x + a, y, z + c);
      vertices_offset += 3;

      /*************
       * vertex 11 *
       *************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a, y, z + c);
      vertices_offset += 3;

      /*************
       * vertex 12 *
       *************/

      gamgi_math_vector_absolute (points + vertices_offset, x - a/2, y + aux, z + c);
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
        gamgi_math_polygon_loops_v12 (cell->loops, &loops_offset, points_offset,
        (i0), (i1), (j0), (k1 || j0), (k1), (j1), (k0 || j1), (k0));

      /*********
       * lines *
       *********/

      if (cell->borders != FALSE)
        {
        /********************
         * horizontal below *
         ********************/

        local1 = (i0 && (global_edges || k0));
        local2 = (i0 && k1);
        local3 = (i0 && (global_edges || (k0 || j1)));
        local4 = (i0 && (k1 || j0));
        local5 = (i0 && j1);
        local6 = (i0 && (global_edges || j0));

        /********************
         * horizontal above *
         ********************/

        local7 = (global_all || (j0 && (global_edges || i1)) || (global_faces && i1));
        local8 = (j1 && (global_edges || i1));
        local9 = (global_all || ((k1 || j0) && (global_edges || i1)) || (global_faces && i1));
        local10 = ((k0 || j1) && (global_edges || i1));
        local11 = (k1 && (global_edges || i1));
        local12 = (global_all || (k0 && (global_edges || i1)) || (global_faces && i1));

        /************
         * vertical *
         ************/

        local13 = (global_all || (k0 || j0));
        local14 = (k1 || j1);
        local15 = (global_all || j0);
        local16 = (j1);
        local17 = (k1);
        local18 = (global_all || k0);

        gamgi_math_polygon_lines_v12 (cell->lines, &lines_offset, 
        points_offset, local1, local2, local3, local4, local5, local6, 
        local7, local8, local9, local10, local11, local12, local13, 
        local14, local15, local16, local17, local18);
        }

      points_offset += 12;
      }
    }
  }

/*****************
 * central nodes *
 *****************/

gamgi_math_polygon_parallelepiped (points + vertices_offset, 
0, n1 - 1, 0, n2 - 1, 0, n3, c1, c2, cell->p3);

/*************************
 * remove repeated nodes *
 *************************/

gamgi_phys_cell_hexagonal_clean (cell);

/*********************************************
 * translate nodes, so node 1 of cell 1 (the *
 * origin) has coordinates (0.0, 0.0, 0.0)   *
 *********************************************/

i = cell->n_nodes;
while (i-- > 0)
  gamgi_math_vector_add (cell->points + 3*i, translate, cell->points + 3*i);
}

void gamgi_phys_cell_hexagonal_index (gamgi_cell *cell, int *index)
{
int *mark;
int n1, n2, n3;
int i, j, k;
int jj, kk;
int n, offset;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/*******************
 * prismatic nodes *
 *******************/

mark = (int *) calloc (n1 * n2 * n3 * 12, sizeof (int));

offset = 0;
for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      kk = 2 * k + j + 1;
      jj = 2 * j + k + 1;

      /************
       * vertex 1 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk - 1;
        index[3*n + 1] = jj - 1;
        index[3*n + 2] = i;
        n++;
        }
      offset++;

      /************
       * vertex 2 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk + 1;
        index[3*n + 1] = jj + 1;
        index[3*n + 2] = i;
        n++;
        }
      if (k < n1 - 1) mark[offset + 12 + 5] = TRUE;
      if (j < n2 - 1) mark[offset + 12 * n1 + 3] = TRUE;
      offset++;

      /************
       * vertex 3 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk - 1;
        index[3*n + 1] = jj - 1;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 2] = TRUE;
      offset++;

      /************
       * vertex 4 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk + 1;
        index[3*n + 1] = jj + 1;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (j < n2 - 1) mark[offset + 12 * n1 + 5] = TRUE;
      if (k < n1 - 1) mark[offset + 12 + 7] = TRUE;
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 2] = TRUE;
      offset++;

      /************
       * vertex 5 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk;
        index[3*n + 1] = jj - 1;
        index[3*n + 2] = i;
        n++;
        }
      offset++;

      /************
       * vertex 6 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk + 1;
        index[3*n + 1] = jj;
        index[3*n + 2] = i;
        n++;
        }
      if (k < n1 - 1) mark[offset + 12 - 5] = TRUE; 
      offset++;

      /************
       * vertex 7 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk - 1;
        index[3*n + 1] = jj;
        index[3*n + 2] = i;
        n++;
        }
      offset++;

      /************
       * vertex 8 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk;
        index[3*n + 1] = jj + 1;
        index[3*n + 2] = i;
        n++;
        }
      if (k > 0 && j < n2 - 1) mark[offset + 12 * n1 - 12 - 2] = TRUE;
      if (j < n2 - 1) mark[offset + 12 * n1 - 7] = TRUE;
      offset++;

      /************
       * vertex 9 *
       ************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk;
        index[3*n + 1] = jj - 1;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 4] = TRUE;
      offset++;

      /*************
       * vertex 10 *
       *************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk + 1;
        index[3*n + 1] = jj;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (k < n1 - 1) mark[offset + 12 - 7] = TRUE;
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 4] = TRUE;
      offset++;

      /*************
       * vertex 11 *
       *************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk - 1;
        index[3*n + 1] = jj;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 4] = TRUE;
      offset++;

      /*************
       * vertex 12 *
       *************/

      if (mark[offset] == FALSE)
        {
        index[3*n + 0] = kk;
        index[3*n + 1] = jj + 1;
        index[3*n + 2] = i + 1;
        n++;
        }
      if (k > 0 && j < n2 - 1) mark[offset + 12 * n1 - 12 - 2] = TRUE;
      if (j < n2 - 1) mark[offset + 12 * n1 - 9] = TRUE;
      if (i < n3 - 1) mark[offset + 12 * n1 * n2 - 4] = TRUE;
      offset++;
      }
    }
  }

free (mark);

/*****************
 * central nodes *
 *****************/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k + j + 1;
      index[3*n + 1] = 2 * j + k + 1;
      index[3*n + 2] = i;
      n++;
      }
    }
  }

}

/****************** external function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

void gamgi_phys_cell_parallelepiped_contour (gamgi_cell *cell)
{
double v1[3], v2[3], v3[3];

/**************
 * start data *
 **************/

gamgi_math_cell_vectors (cell->v1, cell->v2,  cell->v3,
cell->v12, cell->v13,  cell->v23, v1, v2, v3);

/******************************
 * set parallelepiped corners *
 *                            *
 *  set parallelepiped loops  *
 ******************************/

gamgi_math_polygon_parallelepiped (cell->points + 3 * cell->n_nodes, 
0, 1, 0, 1, 0, 1, v1, v2, v3);
gamgi_math_polygon_edges (cell->n_nodes, 
1, 1, 1, &cell->loops, &cell->n_loops);
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

void gamgi_phys_cell_parallelepiped_range (double *p1, double *p2, 
double *p3, double *v1, double *v2, double *v3, int *range)
{
double base[9], inverse[9];
double min[3], max[3];
double v[3], coord[3];

/**************
 * start data *
 **************/

gamgi_math_vector_zero (min);
gamgi_math_vector_zero (max);

gamgi_math_matrix_absolute (base,
p1[0], p2[0], p3[0], p1[1], p2[1], p3[1], p1[2], p2[2], p3[2]);
gamgi_math_matrix_inverse (base, inverse);

/******************
 * get box limits *
 ******************/

gamgi_math_matrix_vector (inverse, v1, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_matrix_vector (inverse, v2, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_matrix_vector (inverse, v3, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_vector_add (v1, v2, v);
gamgi_math_matrix_vector (inverse, v, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_vector_add (v1, v3, v);
gamgi_math_matrix_vector (inverse, v, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_vector_add (v2, v3, v);
gamgi_math_matrix_vector (inverse, v, coord);
gamgi_math_vector_box (coord, min, max);

gamgi_math_vector_add (v1, v, v);
gamgi_math_matrix_vector (inverse, v, coord);
gamgi_math_vector_box (coord, min, max);

/*****************************
 * convert to integer limits *
 *****************************/

/*****
 * x *
 *****/

range[0] = ceil (min[0] - GAMGI_MATH_TOLERANCE_CELL);
range[1] = floor (max[0] + GAMGI_MATH_TOLERANCE_CELL);

/*****
 * y *
 *****/

range[2] = ceil (min[1] - GAMGI_MATH_TOLERANCE_CELL);
range[3] = floor (max[1] + GAMGI_MATH_TOLERANCE_CELL);

/*****
 * z *
 *****/

range[4] = ceil (min[2] - GAMGI_MATH_TOLERANCE_CELL);
range[5] = floor (max[2] + GAMGI_MATH_TOLERANCE_CELL);
}

gamgi_bool gamgi_phys_cell_parallelepiped_inside (double *inverse, double *point)
{
double coord[3];

gamgi_math_matrix_vector (inverse, point, coord);

if (coord[0] < 0 - GAMGI_MATH_TOLERANCE_CELL) return FALSE;
if (coord[0] > 1 + GAMGI_MATH_TOLERANCE_CELL) return FALSE;

if (coord[1] < 0 - GAMGI_MATH_TOLERANCE_CELL) return FALSE;
if (coord[1] > 1 + GAMGI_MATH_TOLERANCE_CELL) return FALSE;

if (coord[2] < 0 - GAMGI_MATH_TOLERANCE_CELL) return FALSE;
if (coord[2] > 1 + GAMGI_MATH_TOLERANCE_CELL) return FALSE;

return TRUE;
}

int gamgi_phys_cell_parallelepiped_count (gamgi_cell *cell)
{
double base[9], inverse[9];
double v1[3], v2[3], v3[3];
double position[3];
int range[6];
int i, j, k;
int n_nodes;

/**************
 * start data *
 **************/

gamgi_math_cell_vectors (cell->v1, cell->v2,  cell->v3,
cell->v12, cell->v13,  cell->v23, v1, v2, v3);

gamgi_math_matrix_absolute (base,
v1[0], v2[0], v3[0], v1[1], v2[1], v3[1], v1[2], v2[2], v3[2]);
gamgi_math_matrix_inverse (base, inverse);

gamgi_phys_cell_parallelepiped_range (cell->p1, cell->p2, cell->p3, v1, v2, v3, range);

/**********************
 * count nodes inside *
 **********************/

n_nodes = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      gamgi_math_vector_translation (k, j, i,
      cell->p1, cell->p2, cell->p3, position);

      /*************************
       * increase node counter *
       *************************/

      if (gamgi_phys_cell_parallelepiped_inside (inverse, position) == TRUE) n_nodes++;
      }
    }
  }

return n_nodes;
}

void gamgi_phys_cell_parallelepiped_create (gamgi_cell *cell, int n_nodes)
{
double *points;
double base[9], inverse[9];
double v1[3], v2[3], v3[3];
double position[3];
int range[6];
int i, j, k;
int offset;

/**************
 * start data *
 **************/

gamgi_math_cell_vectors (cell->v1, cell->v2,  cell->v3,
cell->v12, cell->v13,  cell->v23, v1, v2, v3);

gamgi_math_matrix_absolute (base,
v1[0], v2[0], v3[0], v1[1], v2[1], v3[1], v1[2], v2[2], v3[2]);
gamgi_math_matrix_inverse (base, inverse);

gamgi_phys_cell_parallelepiped_range (cell->p1, cell->p2, cell->p3, v1, v2, v3, range);

/*****************************************************
 * build array containing all nodes inside the       * 
 * parallelepiped, plus, if required, the 8 vertices *
 * needed to describe the parallelepiped contour.    *
 *****************************************************/

cell->n_points = cell->n_nodes = n_nodes;
if (cell->borders == GAMGI_PHYS_EDGES) cell->n_points += 8;
points = (double *) malloc (cell->n_points * 3 * sizeof (double));
cell->points = points;

/***************************************
 * start generating the node positions *
 ***************************************/

offset = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      /**********************
       * save node position *
       **********************/

      gamgi_math_vector_translation (k, j, i,
      cell->p1, cell->p2, cell->p3, position);

      if (gamgi_phys_cell_parallelepiped_inside (inverse, position) == TRUE)
        {
        gamgi_math_vector_copy (position, points + offset);
        offset += 3;
        }

      }
    }
  }

}

void gamgi_phys_cell_parallelepiped_index (gamgi_cell *cell, int *index)
{
double base[9], inverse[9];
double v1[3], v2[3], v3[3];
double position[3];
int range[6];
int i, j, k;
int offset;

/**************
 * start data *
 **************/

gamgi_math_cell_vectors (cell->v1, cell->v2,  cell->v3,
cell->v12, cell->v13,  cell->v23, v1, v2, v3);

gamgi_math_matrix_absolute (base,
v1[0], v2[0], v3[0], v1[1], v2[1], v3[1], v1[2], v2[2], v3[2]);
gamgi_math_matrix_inverse (base, inverse);

gamgi_phys_cell_parallelepiped_range (cell->p1, cell->p2, cell->p3, v1, v2, v3, range);

offset = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      /************************
       * get node coordinates *
       ************************/

      gamgi_math_vector_translation (k, j, i, 
      cell->p1, cell->p2, cell->p3, position);

      if (gamgi_phys_cell_parallelepiped_inside (inverse, position) == TRUE)
        {
        index[offset + 0] = k;
        index[offset + 1] = j;
        index[offset + 2] = i;
        offset += 3;
        }

      }
    }
  }

}

/****************** external function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

void gamgi_phys_cell_sphere_contour (gamgi_cell *cell)
{
double r;
int *loops;
int points_offset, loops_offset;

cell->n_loops = 1 + 3 + 3 * GAMGI_MESA_CELL_SLICES;
loops = (int *) malloc (sizeof (int) * cell->n_loops);
cell->loops = loops;

loops[0] = 3;
loops_offset = 1;
points_offset = cell->n_nodes;
r = cell->v1 + GAMGI_MATH_TOLERANCE_CELL;

/******************************************
 * build three orthogonal circumpherences *
 ******************************************/

gamgi_math_polygon_circumpherence_yz (loops, cell->points, r,
GAMGI_MESA_CELL_SLICES, &loops_offset, &points_offset);

gamgi_math_polygon_circumpherence_xz (loops, cell->points, r,
GAMGI_MESA_CELL_SLICES, &loops_offset, &points_offset);

gamgi_math_polygon_circumpherence_xy (loops, cell->points, r, 
GAMGI_MESA_CELL_SLICES, &loops_offset, &points_offset);
}

void gamgi_phys_cell_sphere_range (double *p1, 
double *p2, double *p3, double r, int *range)
{
double normal[3];
double dot;

/**********************
 * get scanning range *
 **********************/

gamgi_math_vector_cross (p1, p2, normal);
gamgi_math_vector_normal (normal);
dot = gamgi_math_vector_dot (p3, normal);
range[5] = floor (r / dot);
range[4] = -range[5];

gamgi_math_vector_cross (p3, p1, normal);
gamgi_math_vector_normal (normal);
dot = gamgi_math_vector_dot (p2, normal);
range[3] = floor (r / dot);
range[2] = -range[3];

gamgi_math_vector_cross (p2, p3, normal);
gamgi_math_vector_normal (normal);
dot = gamgi_math_vector_dot (p1, normal);
range[1] = floor (r / dot);
range[0] = -range[1];
}

int gamgi_phys_cell_sphere_count (gamgi_cell *cell)
{
double position[3];
double r;
int range[6];
int i, j, k;
int n_nodes;

/**************
 * start data *
 **************/

r = cell->v1 + GAMGI_MATH_TOLERANCE_CELL;

gamgi_phys_cell_sphere_range (cell->p1, cell->p2, cell->p3, r, range);

/**********************
 * count nodes inside *
 **********************/

n_nodes = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      /*************************
       * increase node counter *
       *************************/

      gamgi_math_vector_translation (k, j, i, cell->p1, cell->p2, cell->p3, position);

      if (position[0] * position[0] + position[1] * position[1] +
      position[2] * position[2] < r * r) n_nodes++;
      }
    }
  }

return n_nodes;
}

void gamgi_phys_cell_sphere_create (gamgi_cell *cell, int n_nodes)
{
double *points;
double position[3];
double r;
int range[6];
int i, j, k;
int offset;

/**************
 * start data *
 **************/

r = cell->v1 + GAMGI_MATH_TOLERANCE_CELL;

gamgi_phys_cell_sphere_range (cell->p1, cell->p2, cell->p3, r, range);

/*******************************************************
 * build array containing all nodes inside the sphere, *
 * plus, if required, the 3 * GAMGI_MESA_CELL_SLICES   *
 * vertices needed to describe the 3 circumpherences.  *
 *******************************************************/

cell->n_points = cell->n_nodes = n_nodes;
if (cell->borders == GAMGI_PHYS_EDGES)
  cell->n_points += 3 * GAMGI_MESA_CELL_SLICES;

points = (double *) malloc (3 * cell->n_points * sizeof (double));
cell->points = points;

/***************************
 * generate node positions *
 ***************************/

offset = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      /**********************
       * save node position *
       **********************/ 

      gamgi_math_vector_translation (k, j, i, cell->p1, cell->p2, cell->p3, position);

      if (position[0] * position[0] + position[1] * position[1] + 
      position[2] * position[2] < r * r)
        {
        gamgi_math_vector_copy (position, points + offset);
        offset += 3;
        }

      }
    }
  }

}

void gamgi_phys_cell_sphere_index (gamgi_cell *cell, int *index)
{
double position[3];
double r;
int range[6];
int i, j, k;
int offset;

/**************
 * start data *
 **************/

r = cell->v1 + GAMGI_MATH_TOLERANCE_CELL;

gamgi_phys_cell_sphere_range (cell->p1, cell->p2, cell->p3, r, range);

offset = 0;
for (i = range[4]; i <= range[5]; i++)
  {
  for (j = range[2]; j <= range[3]; j++)
    {
    for (k = range[0]; k <= range[1]; k++)
      {
      /************************
       * get node coordinates *
       ************************/

      gamgi_math_vector_translation (k, j, i, cell->p1, cell->p2, cell->p3, position);

      if (position[0] * position[0] + position[1] * position[1] +
      position[2] * position[2] < r * r)
        {
        index[offset + 0] = k;
        index[offset + 1] = j;
        index[offset + 2] = i;
        offset += 3;
        }

      }
    }
  }

}

/****************** external function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

void gamgi_phys_cell_projection_contour (gamgi_cell *cell)
{
double *points;
int *loops;
int points_offset, loops_offset;

/******************************************
 * the cell center is calculated from the *
 * average of the nodes positions, so we  *
 * define one node, initialized to zero   *
 ******************************************/

cell->n_nodes = 1;
 
/********************************************
 * build points,loops arrays containing the * 
 * stereograpic projection circumpherence   *
 ********************************************/

cell->n_points = 1 + GAMGI_MESA_CELL_SLICES;
points = (double *) malloc (sizeof (double) * 3 * cell->n_points);
cell->points = points;

cell->n_loops = 1 + 1 + GAMGI_MESA_CELL_SLICES;
loops = (int *) malloc (sizeof (int) * cell->n_loops);
cell->loops = loops;

/******************************
 * set node position to zero  *
 * set number of loops to one *
 ******************************/

gamgi_math_vector_zero (points);
loops[0] = 1;

loops_offset = 1;
points_offset = cell->n_nodes;

/****************************
 * build projection contour *
 ****************************/

gamgi_math_polygon_circumpherence_xy (loops, cell->points, 1.0,
GAMGI_MESA_CELL_SLICES, &loops_offset, &points_offset);
}

/****************** external function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

void gamgi_phys_cell_create (gamgi_cell *cell)
{
int n;

/****************************************
 * obtain conventional vectors for cell *
 *                                      *
 *   obtain primitive vectors for cell  *
 ****************************************/

gamgi_math_cell_vectors (cell->a, cell->b,  cell->c,  cell->ab,
cell->ac, cell->bc, cell->a1, cell->a2, cell->a3);

gamgi_math_cell_primitive (cell->lattice,
cell->a1, cell->a2, cell->a3, cell->p1, cell->p2, cell->p3);

/********************************
 * create and position the cell *
 ********************************/

switch (cell->model)
  {
  case GAMGI_PHYS_CONVENTIONAL:
  if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
    gamgi_phys_cell_hexagonal_create (cell);
  else
    {
    static_conventional (cell);
    if (cell->borders == GAMGI_PHYS_EDGES || 
    cell->borders == GAMGI_PHYS_FACES || cell->faces == TRUE)
      gamgi_math_polygon_edges (0, cell->n1, cell->n2, 
      cell->n3, &cell->loops, &cell->n_loops);
    if (cell->borders == GAMGI_PHYS_FACES)
      gamgi_math_polygon_faces (0, cell->n1, cell->n2,
      cell->n3, &cell->lines, &cell->n_lines);
    if (cell->borders == GAMGI_PHYS_ALL)
      gamgi_math_polygon_body (0, cell->n1, cell->n2,
      cell->n3, &cell->lines, &cell->n_lines);
    }
  break;

  case GAMGI_PHYS_PRIMITIVE:
  static_primitive_P (cell);
  if (cell->borders == GAMGI_PHYS_EDGES || 
  cell->borders == GAMGI_PHYS_FACES || cell->faces == TRUE)
    gamgi_math_polygon_edges (0, cell->n1, cell->n2, 
    cell->n3, &cell->loops, &cell->n_loops);
  if (cell->borders == GAMGI_PHYS_FACES)
    gamgi_math_polygon_faces (0, cell->n1, cell->n2,
    cell->n3, &cell->lines, &cell->n_lines);
  if (cell->borders == GAMGI_PHYS_ALL)
    gamgi_math_polygon_body (0, cell->n1, cell->n2,
    cell->n3, &cell->lines, &cell->n_lines);
  break;

  case GAMGI_PHYS_WIGNER:
  gamgi_phys_wigner_create (cell);
  break;

  case GAMGI_PHYS_PARALLELEPIPED:
  n = gamgi_phys_cell_parallelepiped_count (cell);
  gamgi_phys_cell_parallelepiped_create (cell, n);
  if (cell->borders == GAMGI_PHYS_EDGES)
    gamgi_phys_cell_parallelepiped_contour (cell);
  break;

  case GAMGI_PHYS_SPHERE:
  n = gamgi_phys_cell_sphere_count (cell);
  gamgi_phys_cell_sphere_create (cell, n);
  if (cell->borders == GAMGI_PHYS_EDGES)
    gamgi_phys_cell_sphere_contour (cell);
  break;

  case GAMGI_PHYS_PROJECTION:
  gamgi_phys_cell_projection_contour (cell);
  break;
  }

/********************************************************
 * set cell center, so from now on cell can be rotated, *
 * translated to any arbitrary orientation, position    *
 ********************************************************/

gamgi_math_cell_center (cell, cell->center);
}

gamgi_cell *gamgi_phys_cell_polygons (gamgi_cell *cell_old)
{
gamgi_cell *cell;

cell = gamgi_engine_create_cell ();
gamgi_engine_start_cell (cell);

/***************************
 * copy lattice parameters *
 ***************************/

cell->lattice = cell_old->lattice;
cell->group = 0;

cell->a = cell_old->a;
cell->b = cell_old->b;
cell->c = cell_old->c;
cell->ab = cell_old->ab;
cell->ac = cell_old->ac;
cell->bc = cell_old->bc;

/**************************
 * copy volume parameters *
 **************************/

cell->model = cell_old->model;

cell->n1 = cell_old->n1;
cell->n2 = cell_old->n2;
cell->n3 = cell_old->n3;

cell->v1 = cell_old->v1;
cell->v2 = cell_old->v2;
cell->v3 = cell_old->v3;
cell->v12 = cell_old->v12;
cell->v13 = cell_old->v13;
cell->v23 = cell_old->v23;

/************************************************************************
 *                      set view parameters                             *
 *                                                                      *
 * Choose solid faces for Conventional, Primitive and Wigner-Seitz      *
 * volumes, to guarantee that the loops array is built. Solid faces     *
 * are not allowed for Parallelepiped volumes, so BORDERS must be       *
 * set to GAMGI_PHYS_FACES (or GAMGI_PHYS_EDGES) instead. In sphere     *
 * volumes, planes/directions are handled separately, and in Projection *
 * representations they are represented differently, as poles/traces.   *
 ************************************************************************/

if (cell->model == GAMGI_PHYS_PARALLELEPIPED) cell->borders = GAMGI_PHYS_FACES;
else cell->faces = TRUE;

gamgi_phys_cell_create (cell);

return cell;
}
