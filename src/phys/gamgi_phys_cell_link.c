/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_cell_link.c
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

#include "gamgi_engine_create.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_polygon.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_space.h"

static void static_wigner_min (gamgi_cell *cell, 
double *absolute, int *equal, int *node_min)
{
double absolute_new[3];
double absolute_min[3];
double position[3];
double length_min;
double length;
int i, j, k;
int node;

/*********************************************************
 * For Wigner-Seitz cells we need to see which lattice   *
 * node is closer to the object, because that is the     *
 * center of the Wigner-Seitz cell that contains the     *
 * object and absolute should be referred to that node.  *
 *                                                       *
 * This method does not work for rombohedric, monoclinic *
 * and triclinic lattices with very small angles, as in  *
 * these cases the closer node can be in a cell which is *
 * much farther away. These are exactly the lattices for *
 * which Wigner-Seitz cells cannot be constructed yet.   *
 *********************************************************/

length_min = DBL_MAX;
for (k = 0; k <= 1; k++)
  {
  for (j = 0; j <= 1; j++)
    {
    for (i = 0; i <= 1; i++)
      {
      /*************************
       * initialize node flags *
       *************************/

      node = 4*k + 2*j + i;
      equal[node] = -1;

      gamgi_math_vector_absolute (position,
      i * cell->p1[0] + j * cell->p2[0] + k * cell->p3[0],
      i * cell->p1[1] + j * cell->p2[1] + k * cell->p3[1],
      i * cell->p1[2] + j * cell->p2[2] + k * cell->p3[2]);

      gamgi_math_vector_sub (absolute, position, absolute_new);
      length = gamgi_math_vector_length (absolute_new);

      /*****************************************************
       * when a set of nodes is closer to absolute, the    *
       * first one is the minimum, the others are replicas *
       *****************************************************/

      if (length < length_min - GAMGI_MATH_TOLERANCE_LENGTH)
        {
        length_min = length;
        *node_min = node;
        gamgi_math_vector_copy (absolute_new, absolute_min);
        }
      else if (length < length_min + GAMGI_MATH_TOLERANCE_LENGTH)
        equal[node] = *node_min;
      }
    }
  }
gamgi_math_vector_copy (absolute_min, absolute);
}

static int *static_wigner_replicas (int *replicas, 
int *equal, int position, int node_min, int *offset)
{
int n_replicas, node;
int i, j, k;
int i_min, j_min, k_min;

/*******************************************
 * count how many nodes (the replicas) are *
 * at the same distance as the chosen node *
 *******************************************/

n_replicas = 0;
for (node = 0; node < 8; node++)
  if (node > node_min && equal[node] == node_min)
    n_replicas++;

/*****************************************************************
 * allocate and write the replicas array: the first n_positions  *
 * point to the beginning of each range in the array. Each range *
 * indicates the number of replicas and its node coordinates.    *
 *                                                               *
 * initially replicas is NULL and offset is n_positions,         *
 * according to the format, then each position forces a realloc  *
 * and writes its own n_replicas and replicas coordinates        *
 *****************************************************************/

replicas = (int *) realloc (replicas,
(*offset + 1 + 3 * n_replicas) * sizeof (int));

replicas[position] = *offset;
replicas[(*offset)++] = n_replicas;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

i_min = j_min = k_min = -1;

/****************************************************
 * set (shifted) node coordinates in replicas array *
 ****************************************************/

for (k = 0; k <= 1; k++)
  {
  for (j = 0; j <= 1; j++)
    {
    for (i = 0; i <= 1; i++)
      {
      node = 4*k + 2*j + i;

      if (node == node_min)
        {
        i_min = i;
        j_min = j;
        k_min = k;
        }

      if (node > node_min && equal[node] == node_min)
        {
        replicas [(*offset)++] = i - i_min;
        replicas [(*offset)++] = j - j_min;
        replicas [(*offset)++] = k - k_min;
        }

      }
    }
  }

return replicas;
}

static void static_shift (gamgi_cell *cell, int n_positions,
double *positions, double *translation, double *rotation)
{
double position[3];
int i;

/***************************************************
 * shift coordinates: first rotate, then translate *
 *                                                 *
 * inicial coordinates are in the range ]-1, +1[,  *
 * this range does not change with rotation, so    *
 * no need to correct (which is good, because      *
 * there is no easy correction for translations)   *
 *                                                 *
 * the range does change with translation, but     *
 * in this case a easy correction exists, just     *
 * apply periodic boundary conditions, reducing    *
 * the new coordinates to the range ]-1, +1[       *
 ***************************************************/

for (i = 0; i < n_positions; i++)
  {
  gamgi_math_matrix_vector (rotation, positions, position);
  gamgi_math_vector_add (position, translation, positions);
  gamgi_math_vector_fmod (positions, positions);
  positions += 3;
  }
}

static void static_conventional (gamgi_cell *cell,
gamgi_enum vectors, double *xyz)
{
double basis[9], inverse[9];
double position[3];

/***************************************************************
 * convert coordinates in primitive vectors ([-1,+1])          *
 * or axes vectors (any) to conventional vectors ([-1,+1]):    *
 *                                                             *
 * [conv. vectors][conv. coord] = [prim. vectors][prim. coord] *
 * [conv. vectors][conv. coord] = [axes coord]                 *
 *                                                             *
 * this conversion must be done before generating positions    *
 * with space symmetry information, as wyckoff positions       *
 * are always given as [-1,+1] conventional coordinates        *
 ***************************************************************/

if (vectors != GAMGI_PHYS_CONVENTIONAL)
  {
  gamgi_math_matrix_absolute (basis,
  cell->a1[0], cell->a2[0], cell->a3[0],
  cell->a1[1], cell->a2[1], cell->a3[1],
  cell->a1[2], cell->a2[2], cell->a3[2]);
  gamgi_math_matrix_inverse (basis, inverse);

  if (vectors == GAMGI_PHYS_PRIMITIVE)
    {
    gamgi_math_matrix_absolute (basis,
    cell->p1[0], cell->p2[0], cell->p3[0],
    cell->p1[1], cell->p2[1], cell->p3[1],
    cell->p1[2], cell->p2[2], cell->p3[2]);
    }
  else gamgi_math_matrix_unit (basis);

  /***********************************
   * convert to conventional vectors *
   ***********************************/

  gamgi_math_matrix_vector (basis, xyz, position);
  gamgi_math_matrix_vector (inverse, position, xyz);

  /*****************************************
   * use coordinates in the range ]-1, +1[ *
   *****************************************/

  gamgi_math_vector_fmod (xyz, xyz);
  }
}

static void static_primitive (gamgi_cell *cell,
int n_positions, double *relative)
{
double basis[9], inverse[9];
double position[3];
int n;

gamgi_math_matrix_absolute (basis,
cell->p1[0], cell->p2[0], cell->p3[0],
cell->p1[1], cell->p2[1], cell->p3[1],
cell->p1[2], cell->p2[2], cell->p3[2]);
gamgi_math_matrix_inverse (basis, inverse);

gamgi_math_matrix_absolute (basis,
cell->a1[0], cell->a2[0], cell->a3[0],
cell->a1[1], cell->a2[1], cell->a3[1],
cell->a1[2], cell->a2[2], cell->a3[2]);

for (n = 0; n < n_positions; n++)
  {
  /******************************
   * convert to primite vectors *
   ******************************/

  gamgi_math_matrix_vector (basis, relative, position);
  gamgi_math_matrix_vector (inverse, position, relative);

  /*****************************************
   * use coordinates in the range ]-1, +1[ *
   *****************************************/

  gamgi_math_vector_fmod (relative, relative);

  relative += 3;
  }
}

/***************** internal function *******************
 *                                                     *
 *                  STATIC_ABSOLUTE                    *
 *                                                     *
 *      Calculate the object position, in absolute     *
 *         coordinates, relative to each node          *
 *******************************************************/

static double *static_absolute (gamgi_cell *cell,
int n_positions, double *relative)
{
double basis[9];
double *absolute;
int n;

absolute = (double *) malloc (3 * n_positions * sizeof (double));

gamgi_math_matrix_absolute (basis,
cell->p1[0], cell->p2[0], cell->p3[0],
cell->p1[1], cell->p2[1], cell->p3[1],
cell->p1[2], cell->p2[2], cell->p3[2]);

for (n = 0; n < n_positions; n++)
  {
  /*****************************************************************
   * For Wigner-Seitz volumes, all coordinates must be positive,   *
   * to ensure that the position is inside a primitive cell, so    *
   * searching the 8 nodes around we will find the nearest node.   *
   * (cubic, tetragonal, orthorhombic and hexagonal lattices only) *
   *                                                               *
   * For all the other volumes, coordinates too close to -1 or +1  *
   * should be avoided, because the cell border tolerance does not *
   * apply for these coordinates: if a given point has coordinate  *
   * 1-1E-10, it is out for the most positive nodes, which is an   *
   * error. If we subtract 1, the new coordinate -1E-10 is valid   *
   * for all nodes, because it is under the border tolerance.      *
   *****************************************************************/

  if (cell->model == GAMGI_PHYS_WIGNER)
    gamgi_math_cell_range_positive (relative + 3*n, relative + 3*n);
  else
    gamgi_math_cell_range_half (relative + 3*n, relative + 3*n);

  /*********************************************
   * absolute is the vector that will be added *
   * to each node to obtain the final position *
   *********************************************/

  gamgi_math_matrix_vector (basis, relative + 3*n, absolute + 3*n);
  }

return absolute;
}

static int *static_wigner_absolute (gamgi_cell *cell, 
gamgi_enum cutoff, int n_positions, double *absolute)
{
int equal[8];
int *replicas;
int n, node_min;
int offset;

replicas = NULL;
offset = n_positions;
node_min = -1;

for (n = 0; n < n_positions; n++)
  {
  /*********************************************************
   * For Wigner-Seitz cells we need to see which lattice   *
   * node is closer to the object, because that is the     *
   * center of the Wigner-Seitz cell that contains the     *
   * object and absolute should be referred to that node.  *
   *                                                       *
   * This method does not work for rombohedric, monoclinic *
   * and triclinic lattices with very small angles, as in  *
   * these cases the closer node can be in a cell which is *
   * much farther away. These are exactly the lattices for *
   * which Wigner-Seitz cells cannot be constructed yet.   *
   *********************************************************/

  static_wigner_min (cell, absolute + 3*n, equal, &node_min);

  /***************************************************
   * when cutoff is node-based we only need to       *
   * find the absolute vector from the nearest node  *
   *                                                 *
   * when cutoff is object-based we need to find     *
   * all the other nodes that are at the same        *
   * distance, so nodes outside can still contribute *
   * to what is in the Wigner-Seitz structure, as    *
   * long as they share contents with nodes inside   *
   ***************************************************/

  if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
    replicas = static_wigner_replicas (replicas, equal, n, node_min, &offset);
  }

return replicas;
}

static void static_volume (gamgi_cell *cell, double *inverse) 
{
double volume[9];
double v1[3], v2[3], v3[3];
int n1, n2, n3;

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;

switch (cell->model)
  {
  case GAMGI_PHYS_CONVENTIONAL:
  if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
    {
    n1 = 2*cell->n1 + cell->n2 - 1;
    n2 = 2*cell->n2 + cell->n1 - 1;
    }
  gamgi_math_matrix_absolute (volume,
  n1 * cell->a1[0], n2 * cell->a2[0], n3 * cell->a3[0],
  n1 * cell->a1[1], n2 * cell->a2[1], n3 * cell->a3[1], 
  n1 * cell->a1[2], n2 * cell->a2[2], n3 * cell->a3[2]);
  break;

  case GAMGI_PHYS_PRIMITIVE:
  gamgi_math_matrix_absolute (volume,
  n1 * cell->p1[0], n2 * cell->p2[0], n3 * cell->p3[0],
  n1 * cell->p1[1], n2 * cell->p2[1], n3 * cell->p3[1],   
  n1 * cell->p1[2], n2 * cell->p2[2], n3 * cell->p3[2]);
  break;
  
  case GAMGI_PHYS_PARALLELEPIPED:
  gamgi_math_cell_vectors (cell->v1, cell->v2,  cell->v3,
  cell->v12, cell->v13, cell->v23, v1, v2, v3);
  gamgi_math_matrix_absolute (volume, v1[0], v2[0], v3[0],
  v1[1], v2[1], v3[1], v1[2], v2[2], v3[2]); 
  break;
  
  case GAMGI_PHYS_SPHERE:
  case GAMGI_PHYS_WIGNER:
  return;
  }

gamgi_math_matrix_inverse (volume, inverse);
}

static void static_node_coordinates (int node,
int n1, int n2, int n3, int *i, int *j, int *k)
{
int left;

*k = node / (n1 * n2);
left = node % (n1 * n2);
*j = left / n1;
*i = left % n1;
}

static gamgi_bool static_node_inside (int n1, int n2, int n3, 
int i, int j, int k)
{
if (i > 0 && i < n1 - 1 && j > 0 && j < n2 - 1 && k > 0 && k < n3 - 1)
  return TRUE;

return FALSE;
}

static gamgi_bool static_wigner_inside (gamgi_cell *cell,
double *absolute, int node, int position, int *replicas)
{
int n1, n2, n3;
int i, j, k, n;
int i_new, j_new, k_new;
int offset, n_replicas;

n1 = cell->n1 + 2;
n2 = cell->n2 + 2;
n3 = cell->n3 + 2;

/*********************************************
 * get i,j,k node coordinates, so we can see *
 * if this is a real or a surrounding node   *
 *********************************************/

static_node_coordinates (node, n1, n2, n3, &i, &j, &k);

/****************************
 * link when a node is real *
 ****************************/

if (static_node_inside (n1, n2, n3, i, j, k) == TRUE)
  return TRUE;

/**************************************************
 * The format of the replicas array is: the first *
 * n_positions give the offset for each position  *
 * zone, then each position zone starts with the  *
 * number of replicas, followed immediately by    *
 * the replicas i,j,k node coordinates.           *
 **************************************************/

offset = replicas[position];

n_replicas = replicas[offset];
for (n = 0; n < n_replicas; n++)
  {
  i_new = i + replicas[offset + 3*n + 1];
  j_new = j + replicas[offset + 3*n + 2];
  k_new = k + replicas[offset + 3*n + 3];

  /***************************************************************
   * link when a non-real node has a replica that is a real node *
   ***************************************************************/

  if (static_node_inside (n1, n2, n3, i_new, j_new, k_new) == TRUE)
    return TRUE;
  }

return FALSE;
}

static gamgi_bool static_object_inside (gamgi_cell *cell, 
double *absolute, double *inverse, int node, int position, 
int *replicas)
{
double relative[3];

switch (cell->model)
  {
  case GAMGI_PHYS_CONVENTIONAL: 
  case GAMGI_PHYS_PRIMITIVE: 
  case GAMGI_PHYS_PARALLELEPIPED:
  gamgi_math_matrix_vector (inverse, absolute, relative);
  if (relative[0] < 1 + GAMGI_MATH_TOLERANCE_CELL 
  && relative[1] < 1 + GAMGI_MATH_TOLERANCE_CELL 
  && relative[2] < 1 + GAMGI_MATH_TOLERANCE_CELL 
  && relative[0] > 0 - GAMGI_MATH_TOLERANCE_CELL 
  && relative[1] > 0 - GAMGI_MATH_TOLERANCE_CELL 
  && relative[2] > 0 - GAMGI_MATH_TOLERANCE_CELL) return TRUE;
  break;

  case GAMGI_PHYS_SPHERE:
  if (pow (absolute[0], 2) + pow (absolute[1], 2) + pow (absolute[2], 2) 
  < pow (cell->v1 + GAMGI_MATH_TOLERANCE_CELL, 2)) return TRUE;
  break;
  
  case GAMGI_PHYS_WIGNER:
  return static_wigner_inside (cell, absolute, node, position, replicas);
  break;
  }

return FALSE;
}

static gamgi_bool static_point_inside (gamgi_cell *cell, int x, 
int y, double coord_x, double coord_y, int *triangles)
{
/******************************************************
 * convert x,y node to x,y triangle coordinates:      *
 * x triangle cell coordinate = x node coordinate X 2 *
 * y triangle cell coordinate = y node coordinate     *
 *                                                    *
 *       x,y node coordinates start at 0,0            *
 ******************************************************/

x *= 2;

/****************************************
 * node (0,0) starts at cell (0,0),     *
 * which corresponds to the triangle    *
 * below left of the hexagon, because   *
 * of the 1-cell frame around the nodes *
 ****************************************/

if (coord_y < 0)
  {
  /*******************
   * triangles below *
   * left x + 0      *
   * center x + 1    *
   * right x + 2     *
   *******************/

  if (coord_x > 0) x += 2;
  else if (coord_x > coord_y) x += 1;
  }
else
  {
  x++;
  y++;

  /*******************
   * triangles above *
   * left x + 0      *
   * center x + 1    *
   * right x + 2     *
   *******************/

  if (coord_x > coord_y) x += 2;
  else if (coord_x > 0) x += 1;
  }

/**************************************
 * taking the frame into account, the *
 * number of triangles in a row is:   *
 * 2 * (2 * cell->n1 + cell->n2 + 1)  *
 **************************************/

if (triangles[y * 2 * (2 * cell->n1 + cell->n2 + 1) + x] == 1)
  return TRUE;

return FALSE;
}

static gamgi_bool static_inside_triangles (gamgi_cell *cell, 
int node, double *coord, int *index, int *triangles)
{
int x, y;

/********************
 * node coordinates *
 ********************/

x = index[3*node + 0];
y = index[3*node + 1];

/*********************************************
 * instead of testing a single point, define *
 * a square with borders at a distance equal *
 * to the tolerance from the center and then *
 * test if at least one the four corners are *
 * inside a valid triangle.                  *
 *********************************************/

if (static_point_inside (cell, x, y, coord[0] - GAMGI_MATH_TOLERANCE_CELL, 
coord[1] - GAMGI_MATH_TOLERANCE_CELL, triangles) == TRUE) return TRUE;

if (static_point_inside (cell, x, y, coord[0] + GAMGI_MATH_TOLERANCE_CELL,
coord[1] - GAMGI_MATH_TOLERANCE_CELL, triangles) == TRUE) return TRUE;

if (static_point_inside (cell, x, y, coord[0] - GAMGI_MATH_TOLERANCE_CELL, 
coord[1] + GAMGI_MATH_TOLERANCE_CELL, triangles) == TRUE) return TRUE;

if (static_point_inside (cell, x, y, coord[0] + GAMGI_MATH_TOLERANCE_CELL, 
coord[1] + GAMGI_MATH_TOLERANCE_CELL, triangles) == TRUE) return TRUE;

return FALSE;
}

static int *static_triangles_inside (gamgi_cell *cell)
{
int *triangles;
int n1, n2, n;
int offset, i, j, x, y;

n1 = cell->n1;
n2 = cell->n2;

/****************************************************
 * 2D hexagonal lattice: (m,n) conventional cells   *
 * are completely covered by a 4 sided polygon      *
 * formed by (2m+n-1,2n+m-1) primitive cells or     *
 * 2(2m+n-1,2n+m-1) equilateral triangles, as each  *
 * primitive cell contains 2 triangles.             *
 *                                                  *
 *  Some of these triangles are inside the area     * 
 *  of the conventional cells, some are outside.    *
 *                                                  *
 * We add a frame of 1 primitive cell all around    *
 * the (2m+n-1,2n+m-1) cells, to make it easier to  *
 * check the volume tolerance around the hexagonal  *
 * cells, so the framed polygon has (2m+n+1,2n+m+1) *
 * primitive cells and 2(2m+n+1,2n+m+1) triangles.  *
 ****************************************************/

n = 2 * (2*n1 + n2 + 1) * (2*n2 + n1 + 1);
triangles = (int *) malloc (sizeof (int) * n);

/*****************************************
 * by default, all triangles are outside *
 *****************************************/

while (n-- > 0) triangles[n] = 0;

/***************************************
 * indicate which triangles are inside *
 ***************************************/

for (j = 0; j < n2; j++)
  {
  for (i = 0; i < n1; i++)
    {
    /*******************************************
     * x,y represent the number of primitive   *
     * cells in the x,y directions that are    *
     * necessary to cover the i,j conventional *
     * cell, without considering the frame.    *
     *******************************************/
      
    x = 2*i + j + 2;
    y = 2*j + i + 2;

    /*******************
     * triangles below *
     *******************/

    offset = (y - 1) * (2*n1 + n2 + 1) * 2 + (x - 1) * 2;

    triangles[offset + 0] = 1;
    triangles[offset + 1] = 1;
    triangles[offset + 2] = 1;

    /*******************
     * triangles above *
     *******************/

    offset += 2*(2*n1 + n2 + 1) + 1;

    triangles[offset + 0] = 1;
    triangles[offset + 1] = 1;
    triangles[offset + 2] = 1;
    }
  }

return triangles;
}

static void static_link (gamgi_cell *cell, double *points, gamgi_atom *atom, 
int *nodes, int n_nodes, int n_positions, double *relative, double *absolute, 
gamgi_enum cutoff, double *rotate, double *translate, double *inverse, 
int *index, int *triangles, int *replicas)
{
gamgi_atom *atom_new;
double position[3];
int i, j;

/**************
 * scan nodes *
 **************/

for (i = 0; i < n_nodes; i++)
  {
  /*********************************************************
   * check if the pattern rules give this node as occupied *
   *********************************************************/

  if (nodes[i] < 2) continue;

  /***************************
   * add generated positions *
   ***************************/

  for (j = 0; j < n_positions; j++)
    {
    position[0] = points[3*i + 0] + absolute[3*j + 0];
    position[1] = points[3*i + 1] + absolute[3*j + 1];
    position[2] = points[3*i + 2] + absolute[3*j + 2];

    /*************************************************
     * check if the object is inside the cell volume *
     *************************************************/

    if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
      {
      if (static_object_inside (cell, position, 
      inverse, i, j, replicas) == FALSE) continue;
  
      if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P &&
      cell->model == GAMGI_PHYS_CONVENTIONAL && 
      static_inside_triangles (cell, i, relative + 3*j, 
      index, triangles) == FALSE) continue;
      }

    /*********************************************************
     * create new object using the supplied reference object *
     *********************************************************/

    atom_new = gamgi_engine_copy_atom (atom);

    /*****************************
     * rotate and translate atom *
     *****************************/

    gamgi_math_matrix_vector (rotate, position, atom_new->position);
    gamgi_math_vector_add (atom_new->position, translate, atom_new->position);
  
    /*******************
     * link new object *
     *******************/

    gamgi_engine_link_atom_cell (atom_new, cell);
    }
  }
}

static int static_sequence (char *pattern, int *sequence)
{
int length, i;

if (strchr (pattern, '*') == NULL) return 0;

length = strlen (pattern);

for (i = 0; i < length; i++)
  {
  if (pattern[i] == '_') sequence[i] = 1;
  if (pattern[i] == '*') sequence[i] = 2;
  }

return length;
}

static void static_direction_a (int *nodes, int *sequence, int length, 
int direction, int plane, int k0, int k1, int j, int i)
{
int n, pos, k;

n = k0 + j * direction + i * plane;
if (nodes[n] == 0) return;

/**********************************************
 * The sequence is applied in a cyclic way,   *
 * the starting offset is determined here.    *
 *                                            *
 * the offset MUST be 0 when:                 *
 * 1) all nodes will be flagged 0 (len = 0),  *
 * otherwise infinite loops could occur;      *
 * 2) all nodes are flagged 3 (nodes[n] = 3), *
 * otherwise sequence ordering would no work. *
 **********************************************/

pos = 0;
if (length > 0 && nodes[n] < 3)
  while (sequence[pos] != nodes[n]) pos++;

for (k = k0; k <= k1; k++)
  {
  n = k + j * direction + i * plane;

  nodes[n] = sequence[pos++];

  if (length == pos || length == 0) pos = 0;
  }

}

static void static_direction_b (int *nodes, int *sequence, int length, 
int direction, int plane, int k, int j0, int j1, int i)
{
int n, pos, j;

n = k + j0 * direction + i * plane;
if (nodes[n] == 0) return;

/**********************************************
 * The sequence is applied in a cyclic way,   *
 * the starting offset is determined here.    *
 *                                            *
 * the offset MUST be 0 when:                 *
 * 1) all nodes will be flagged 0 (len = 0),  *
 * otherwise infinite loops could occur;      *
 * 2) all nodes are flagged 3 (nodes[n] = 3), *
 * otherwise sequence ordering would no work. *
 **********************************************/

pos = 0;
if (length > 0 && nodes[n] < 3)
  while (sequence[pos] != nodes[n]) pos++;

for (j = j0; j <= j1; j++)
  {
  n = k + j * direction + i * plane;

  nodes[n] = sequence[pos++];

  if (length == pos || length == 0) pos = 0;
  }

}

static void static_direction_c (int *nodes, int *sequence, int length, 
int direction, int plane, int k, int j, int i0, int i1)
{
int n, pos, i;

n = k + j * direction + i0 * plane;
if (nodes[n] == 0) return;

/**********************************************
 * The sequence is applied in a cyclic way,   *
 * the starting offset is determined here.    *
 *                                            *
 * the offset MUST be 0 when:                 *
 * 1) all nodes will be flagged 0 (len = 0),  *
 * otherwise infinite loops could occur;      *
 * 2) all nodes are flagged 3 (nodes[n] = 3), *
 * otherwise sequence ordering would no work. *
 **********************************************/

pos = 0;
if (length > 0 && nodes[n] < 3)
  while (sequence[pos] != nodes[n]) pos++;

for (i = i0; i <= i1; i++)
  {
  n = k + j * direction + i * plane;

  nodes[n] = sequence[pos++];

  if (length == pos || length == 0) pos = 0;
  }

}

static int static_plane_a (int *nodes, char *pattern, 
int start, int end, int n1, int n2, int n3)
{
int sequence[GAMGI_ENGINE_TOKEN] = {0};
int i, j, length;

if (start > n1) return start;

if (end > n1) end = n1;

if (strchr (pattern, '_') != NULL)
  {
  length = static_sequence (pattern, sequence);

  for (i = 0; i <= n3; i++)
    {
    for (j = 0; j <= n2; j++)
      {
      static_direction_a (nodes, sequence, length, 
      (n1 + 1), (n1 + 1) * (n2 + 1), start, end, j, i);
      }
    }
  }

return end + 1;
}

static int static_plane_b (int *nodes, char *pattern, 
int start, int end, int n1, int n2, int n3)
{
int sequence[GAMGI_ENGINE_TOKEN] = {0};
int i, j, length;

if (start > n2) return start;

if (end > n2) end = n2;

if (strchr (pattern, '_') != NULL)
  {
  length = static_sequence (pattern, sequence);

  for (i = 0; i <= n3; i++)
    {
    for (j = 0; j <= n1; j++)
      {
      static_direction_b (nodes, sequence, length, 
      (n1 + 1), (n1 + 1) * (n2 + 1), j, start, end, i);
      }
    }
  }

return end + 1;
}

static int static_plane_c (int *nodes, char *pattern, 
int start, int end, int n1, int n2, int n3)
{
int sequence[GAMGI_ENGINE_TOKEN] = {0};
int i, j, length;

if (start > n3) return start;

if (end > n3) end = n3;

if (strchr (pattern, '_') != NULL)
  {
  length = static_sequence (pattern, sequence);

  for (i = 0; i <= n2; i++)
    {
    for (j = 0; j <= n1; j++)
      {
      static_direction_c (nodes, sequence, length, 
      (n1 + 1), (n1 + 1) * (n2 + 1), j, i, start, end);
      }
    }
  }

return end + 1;
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_volume_a (int *nodes, char *pattern, int n1, int n2, int n3)
{
char *pattern_int;
char *end;
int number;
int start = 0;

while (start <= n1)
  {
  pattern_int = strpbrk (pattern, "0123456789");
  if (pattern_int == NULL)
    {
    /*************************************
     * no number: apply pattern till end *
     *************************************/

    start = static_plane_a (nodes, pattern, start, n1, n1, n2, n3);
    return;
    }

  number = strtol (pattern_int, &end, 10);

  if (pattern_int == pattern)
    {
    /******************************
     * no pattern before number:  *
     * apply void till number - 1 *
     * apply occupied for number  *
     ******************************/

    start = static_plane_a (nodes, "_", start, number - 1, n1, n2, n3);
    start = static_plane_a (nodes, "*", start, number, n1, n2, n3);
    }
  else
    {
    /****************************************************
     * pattern before number: apply pattern till number *
     ****************************************************/

    pattern = strtok (pattern, "0123456789");
    start = static_plane_a (nodes, pattern, start, number, n1, n2, n3);
    }

  if (strcmp (end, "") == 0)
    {
    /************************************************
     * no pattern after number: apply void till end *
     ************************************************/

    start = static_plane_a (nodes, "_", start, n1, n1, n2, n3);
    return;
    }

  pattern = end;
  }
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_volume_b (int *nodes, char *pattern, int n1, int n2, int n3)
{
char *pattern_int;
char *end;
int number;
int start = 0;

while (start <= n2)
  {
  pattern_int = strpbrk (pattern, "0123456789");
  if (pattern_int == NULL)
    {
    /*************************************
     * no number: apply pattern till end *
     *************************************/

    start = static_plane_b (nodes, pattern, start, n2, n1, n2, n3);
    return;
    }

  number = strtol (pattern_int, &end, 10);

  if (pattern_int == pattern)
    {
    /******************************
     * no pattern before number:  *
     * apply void till number - 1 *
     * apply occupied for number  *
     ******************************/

    start = static_plane_b (nodes, "_", start, number - 1, n1, n2, n3);
    start = static_plane_b (nodes, "*", start, number, n1, n2, n3);
    }
  else
    {
    /****************************************************
     * pattern before number: apply pattern till number *
     ****************************************************/

    pattern = strtok (pattern, "0123456789");
    start = static_plane_b (nodes, pattern, start, number, n1, n2, n3);
    }

  if (strcmp (end, "") == 0)
    {
    /************************************************
     * no pattern after number: apply void till end *
     ************************************************/

    start = static_plane_b (nodes, "_", start, n2, n1, n2, n3);
    return;
    }

  pattern = end;
  }
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_volume_c (int *nodes, char *pattern, int n1, int n2, int n3)
{
char *pattern_int;
char *end;
int number;
int start = 0;

while (start <= n3)
  {
  pattern_int = strpbrk (pattern, "0123456789");
  if (pattern_int == NULL)
    {
    /*************************************
     * no number: apply pattern till end *
     *************************************/

    start = static_plane_c (nodes, pattern, start, n3, n1, n2, n3);
    return;
    }

  number = strtol (pattern_int, &end, 10);

  if (pattern_int == pattern)
    {
    /******************************
     * no pattern before number:  *
     * apply void till number - 1 *
     * apply occupied for number  *
     ******************************/

    start = static_plane_c (nodes, "_", start, number - 1, n1, n2, n3);
    start = static_plane_c (nodes, "*", start, number, n1, n2, n3);
    }
  else
    {
    /****************************************************
     * pattern before number: apply pattern till number *
     ****************************************************/

    pattern = strtok (pattern, "0123456789");
    start = static_plane_c (nodes, pattern, start, number, n1, n2, n3);
    }

  if (strcmp (end, "") == 0)
    {
    /************************************************
     * no pattern after number: apply void till end *
     ************************************************/

    start = static_plane_c (nodes, "_", start, n3, n1, n2, n3);
    return;
    }

  pattern = end;
  }
}

static int *static_mark (int n1, int n2, int n3, 
char *pattern_a, char *pattern_b, char *pattern_c)
{
int *nodes;
int i;

i = (n1 + 1) * (n2 + 1) * (n3 + 1);
nodes = (int *) malloc (sizeof (int) * i);
while (i-- > 0) nodes[i] = 3;

static_volume_a (nodes, pattern_a, n1, n2, n3);
static_volume_b (nodes, pattern_b, n1, n2, n3);
static_volume_c (nodes, pattern_c, n1, n2, n3);

return nodes;
}

static int *static_replicate (int n1, int n2, int n3, int *nodes_in)
{
int *nodes_out;
int in, out, i, j, k;

nodes_out = (int *) malloc (sizeof (int) * (2*n1 + 1) * (2*n2 + 1) * (2*n3 + 1));

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
   {
    for (k = 0; k <= n1; k++)
      {
      in = k + j * (n1 + 1) + i * (n1 + 1) * (n2 + 1);

      out = -k + n1 + (-j + n2) * (2*n1 + 1) + (-i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = k + n1 + (-j + n2) * (2*n1 + 1) + (-i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = -k + n1 + (j + n2) * (2*n1 + 1) + (-i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = k + n1 + (j + n2) * (2*n1 + 1) + (-i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = -k + n1 + (-j + n2) * (2*n1 + 1) + (i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = k + n1 + (-j + n2) * (2*n1 + 1) + (i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = -k + n1 + (j + n2) * (2*n1 + 1) + (i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];

      out = k + n1 + (j + n2) * (2*n1 + 1) + (i + n3) * (2*n1 + 1) * (2*n2 + 1);
      nodes_out[out] = nodes_in[in];
      }
    }
  }

free (nodes_in);

return nodes_out;
}

static int *static_copy (int *index, int o1, int o2, int o3,
int n_nodes, int *nodes_in, int n1, int n2, int n3)
{
int *nodes_out;
int direction, plane;
int i, j, k;
int n;

direction = (2 * n1 + 1);
plane = (2 * n1 + 1) * (2 * n2 + 1);

nodes_out = (int *) malloc (n_nodes * sizeof (int));

/**********************************************
 * initially node coordinates can be positive *
 * or negative, so they must be translated    *
 * before being used as coordinates           *
 **********************************************/

for (n = 0; n < n_nodes; n++)
  {
  k = index[3*n + 0] - o1 + n1;
  j = index[3*n + 1] - o2 + n2;
  i = index[3*n + 2] - o3 + n3;

  nodes_out[n] = nodes_in[k + j * direction + i * plane];
  }

free (nodes_in);

return nodes_out;
}

static int static_index_max (int min, int max, int origin)
{
if (origin - min > max - origin) return origin - min;

return max - origin;
}

static int *static_nodes (int *index, int *range, int n_nodes,            
int o1, int o2, int o3, char *pattern_a, char *pattern_b, char *pattern_c)
{
int *nodes;
int n1, n2, n3;

n1 = static_index_max (range[0], range[1], o1);
n2 = static_index_max (range[2], range[3], o2);
n3 = static_index_max (range[4], range[5], o3);

nodes = static_mark (n1, n2, n3, pattern_a, pattern_b, pattern_c);
nodes = static_replicate (n1, n2, n3, nodes);
nodes = static_copy (index, o1, o2, o3, n_nodes, nodes, n1, n2, n3);

return nodes;
}

static void static_hexagonal_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c,
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse)
{
int *index, *nodes, *triangles;
int range[6];
int n1, n2, n3;
int n_nodes;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = 2 * n1 + n2 - 1;
range[2] = 0; range[3] = 2 * n2 + n1 - 1;
range[4] = 0; range[5] = n3;

n_nodes = cell->n_nodes;
index = (int *) malloc (n_nodes * 3 * sizeof (int));
gamgi_phys_cell_hexagonal_index (cell, index);

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes, 
cell->o1, cell->o2, cell->o3, pattern_a, pattern_b, pattern_c);

/******************************
 * link objects to cell nodes *
 ******************************/

triangles = static_triangles_inside (cell);
static_link (cell, cell->points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate,
translate, inverse, index, triangles, NULL);

/******************
 * free resources *
 ******************/

free (index); free (nodes); free (triangles);
}

static void static_conventional_A_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes, offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1;
range[2] = 0; range[3] = n2 - 1;
range[4] = 0; range[5] = n3 - 1;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * list position and index for all nodes inside *
   ************************************************/

  offset = (n1 + 1) * (n2 + 1) * (n3 + 1);
  points = cell->points + 3 * offset;

  n_nodes = (n1 + 1) * n2 * n3;
  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /******************************************
   * build position and index for all nodes *
   * that can be linked to objects inside   *
   ******************************************/

  range[2] -= 1; range[3] += 1;
  range[4] -= 1; range[5] += 1;

  n_nodes = (n1 + 1) * (n2 + 2) * (n3 + 2);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_phys_cell_conventional_A (cell, 0, range[0],
  range[1], range[2], range[3], range[4], range[5], points);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_conventional_B_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes, offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1 - 1;
range[2] = 0; range[3] = n2;
range[4] = 0; range[5] = n3 - 1;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * list position and index for all nodes inside *
   ************************************************/

  offset = (n1 + 1) * (n2 + 1) * (n3 + 1) + (n1 + 1) * n2 * n3;
  points = cell->points + 3 * offset;

  n_nodes = n1 * (n2 + 1) * n3;
  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /******************************************
   * build position and index for all nodes *
   * that can be linked to objects inside   *
   ******************************************/

  range[0] -= 1; range[1] += 1;
  range[4] -= 1; range[5] += 1;

  n_nodes = (n1 + 2) * (n2 + 1) * (n3 + 2);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_phys_cell_conventional_B (cell, 0, range[0],
  range[1], range[2], range[3], range[4], range[5], points);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_conventional_C_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes, offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1 - 1;
range[2] = 0; range[3] = n2 - 1;
range[4] = 0; range[5] = n3;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * list position and index for all nodes inside *
   ************************************************/

  offset = (n1 + 1) * (n2 + 1) * (n3 + 1) + 
  (n1 + 1) * n2 * n3 + n1 * (n2 + 1) * n3;
  points = cell->points + 3 * offset;

  n_nodes = n1 * n2 * (n3 + 1);
  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /******************************************
   * build position and index for all nodes *
   * that can be linked to objects inside   *
   ******************************************/

  range[0] -= 1; range[1] += 1;
  range[2] -= 1; range[3] += 1;

  n_nodes = (n1 + 2) * (n2 + 2) * (n3 + 1);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_phys_cell_conventional_C (cell, 0, range[0], 
  range[1], range[2], range[3], range[4], range[5], points);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_conventional_I_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes, offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1 - 1;
range[2] = 0; range[3] = n2 - 1;
range[4] = 0; range[5] = n3 - 1;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * list position and index for all nodes inside *
   ************************************************/

  offset = (n1 + 1) * (n2 + 1) * (n3 + 1);
  points = cell->points + 3 * offset;

  n_nodes = n1 * n2 * n3;
  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /******************************************
   * build position and index for all nodes *
   * that can be linked to objects inside   *
   ******************************************/

  range[0] -= 1; range[1] += 1;
  range[2] -= 1; range[3] += 1;
  range[4] -= 1; range[5] += 1;

  n_nodes = (n1 + 2) * (n2 + 2) * (n3 + 2);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));

  gamgi_phys_cell_conventional_I (cell, 0, range[0], range[1],
  range[2], range[3], range[4], range[5], points);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_conventional_R_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse)
{
double *points;
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes, offset;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1 - 1;
range[2] = 0; range[3] = n2 - 1;
range[4] = 0; range[5] = 2 * n3 - 1;
if (o4 == GAMGI_PHYS_122) { range[4] -= 1; range[5] -= 1; }

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * list position and index for all nodes inside *
   ************************************************/

  offset = (n1 + 1) * (n2 + 1) * (n3 + 1);
  points = cell->points + 3 * offset;

  n_nodes = 2 * n1 * n2 * n3;
  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /******************************************
   * build position and index for all nodes *
   * that can be linked to objects inside   *
   ******************************************/

  range[0] -= 1; range[1] += 1;
  range[2] -= 1; range[3] += 1;
  range[4] -= 2; range[5] += 2;

  n_nodes = 2 * (n1 + 2) * (n2 + 2) * (n3 + 2);

  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_phys_cell_conventional_R (cell, 0, range[0], range[1], 
  range[2], range[3], range[4], range[5], points);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_wigner_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse)
{
double *points;
int *index, *nodes, *replicas;
int range[6];
int n1, n2, n3;
int n_nodes;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

/***********************************************************
 * determine nodes in adjacent cells that still contribute *
 * to build the correct structure for Wigner-Seitz cells   *
 ***********************************************************/

replicas = static_wigner_absolute (cell, cutoff, n_positions, absolute);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1 - 1;
range[2] = 0; range[3] = n2 - 1;
range[4] = 0; range[5] = n3 - 1;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /************************************************
   * get number and position for all nodes inside *
   ************************************************/

  points = cell->points;
  n_nodes = n1 * n2 * n3;
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /*****************************************
   * get number and position for all nodes *
   * that can be linked to objects inside  *
   *****************************************/

  range[0] -= 1; range[1] += 1;
  range[2] -= 1; range[3] += 1;
  range[4] -= 1; range[5] += 1;

  n_nodes = (n1 + 2) * (n2 + 2) * (n3 + 2);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_math_polygon_parallelepiped (points, range[0], range[1],
  range[2], range[3], range[4], range[5], cell->p1, cell->p2, cell->p3);
  }

/**************************************
 * build index for all relevant nodes *
 **************************************/

index = (int *) malloc (n_nodes * 3 * sizeof (int));
gamgi_phys_cell_primitive_index (range, index);

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
cell->o1, cell->o2, cell->o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, replicas);

/******************
 * free resources *
 ******************/

free (nodes); free (replicas);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_primitive_link (gamgi_cell *cell, gamgi_atom *atom,
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate,
double *translate, double *inverse)
{
int *index, *nodes;
int range[6];
int n1, n2, n3;
int o1, o2, o3, o4;
int n_nodes;

n1 = cell->n1; n2 = cell->n2; n3 = cell->n3;

if (cell->model == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_conventional (cell->lattice, 
  cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);
else
  { o1 = cell->o1; o2 = cell->o2; o3 = cell->o3; }

n_nodes = (n1 + 1) * (n2 + 1) * (n3 + 1);

/******************************
 * set range of nodes scanned *
 ******************************/

range[0] = 0; range[1] = n1;
range[2] = 0; range[3] = n2;
range[4] = 0; range[5] = n3;

index = (int *) malloc (n_nodes * 3 * sizeof (int));
gamgi_phys_cell_primitive_index (range, index);

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
o1, o2, o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, cell->points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
}

static void static_parallelepiped_link (gamgi_cell *cell, gamgi_atom *atom, 
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
double v1[3], v2[3], v3[3];
int *nodes, *index;
int range[6];
int n_nodes;

/********************************************************
 * get node range needed to cover the whole cell volume *
 ********************************************************/

gamgi_math_cell_vectors (cell->v1, cell->v2, cell->v3,
cell->v12, cell->v13, cell->v23, v1, v2, v3);
gamgi_phys_cell_parallelepiped_range (cell->p1, 
cell->p2, cell->p3, v1, v2, v3, range);

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /********************************************************
   * build array with the coordinates of all nodes inside *
   ********************************************************/

  points = cell->points;
  n_nodes = cell->n_nodes;

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_parallelepiped_index (cell, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /*********************************************
   * add a layer of nodes around: these nodes  *
   * might still add objects inside the volume *
   *                                           *
   * build array with the positions of all     *
   * nodes that might affect the volume inside *
   *                                           *
   * build array with the coordinates of all   *
   * nodes that might affect the volume inside *
   *********************************************/

  range[0] -= 1; range[2] -= 1; range[4] -= 1;
  range[1] += 1; range[3] += 1; range[5] += 1;

  n_nodes = (range[1] - range[0] + 1) *
  (range[3] - range[2] + 1) * (range[5] - range[4] + 1);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_math_polygon_parallelepiped (points, range[0], range[1],
  range[2], range[3], range[4], range[5], cell->p1, cell->p2, cell->p3);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
cell->o1, cell->o2, cell->o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

static void static_sphere_link (gamgi_cell *cell, gamgi_atom *atom, 
gamgi_enum cutoff, char *pattern_a, char *pattern_b, char *pattern_c, 
int n_positions, double *relative, double *absolute, double *rotate, 
double *translate, double *inverse)
{
double *points;
double r;
int *nodes, *index;
int range[6];
int n_nodes;

/********************************************************
 * get node range needed to cover the whole cell volume *
 ********************************************************/

r = cell->v1 + GAMGI_MATH_TOLERANCE_CELL;
gamgi_phys_cell_sphere_range (cell->p1, cell->p2, cell->p3, r, range);

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

points = NULL;
index = NULL;
n_nodes = -1;

if (cutoff == GAMGI_PHYS_CUTOFF_NODES)
  {
  /********************************************************
   * build array with the coordinates of all nodes inside *
   ********************************************************/

  points = cell->points;
  n_nodes = cell->n_nodes;

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_sphere_index (cell, index);
  }

if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS)
  {
  /*********************************************
   * add a layer of nodes around: these nodes  *
   * might still add objects inside the volume *
   *                                           *
   * build array with the positions of all     *
   * nodes that might affect the volume inside *
   *                                           *
   * build array with the coordinates of all   *
   * nodes that might affect the volume inside *
   *********************************************/

  range[0] -= 1; range[2] -= 1; range[4] -= 1;
  range[1] += 1; range[3] += 1; range[5] += 1;

  n_nodes = (range[1] - range[0] + 1) *
  (range[3] - range[2] + 1) * (range[5] - range[4] + 1);
  points = (double *) malloc (n_nodes * 3 * sizeof (double));
  gamgi_math_polygon_parallelepiped (points, range[0], range[1],
  range[2], range[3], range[4], range[5], cell->p1, cell->p2, cell->p3);

  index = (int *) malloc (n_nodes * 3 * sizeof (int));
  gamgi_phys_cell_primitive_index (range, index);
  }

/*********************************************
 * build nodes array, indicating which nodes *
 * should be used in the link operation      *
 *********************************************/

nodes = static_nodes (index, range, n_nodes,
cell->o1, cell->o2, cell->o3, pattern_a, pattern_b, pattern_c);
free (index);

/******************************
 * link objects to cell nodes *
 ******************************/

static_link (cell, points, atom, nodes, n_nodes, 
n_positions, relative, absolute, cutoff, rotate, 
translate, inverse, NULL, NULL, NULL);

/******************
 * free resources *
 ******************/

free (nodes);
if (cutoff == GAMGI_PHYS_CUTOFF_OBJECTS) free (points);
}

void gamgi_phys_cell_link (gamgi_cell *cell, gamgi_atom *atom, 
gamgi_enum vectors, const char **wyckoff, const int *driver, int site,
double *xyz, double *translation, double *rotation, gamgi_enum cutoff,
int nodes_P, int nodes_I, int nodes_A, int nodes_B, int nodes_C,
int nodes_R, const char *rule_a, const char *rule_b, const char *rule_c,
gamgi_window *window)
{
char pattern_a[GAMGI_ENGINE_STRING];
char pattern_b[GAMGI_ENGINE_STRING];
char pattern_c[GAMGI_ENGINE_STRING];
double *relative, *absolute;
double rotate[9], inverse[9];
double translate[3];
int n_positions;

/*********************************************
 * Copy constant rules to variable patterns. *
 * Patterns will be later modified by strtok *
 * calls, so they cannot be const.           *
 *********************************************/

strcpy (pattern_a, rule_a);
strcpy (pattern_b, rule_b);
strcpy (pattern_c, rule_c);

/**************************************************************
 * convert to conventional coordinates, in the range ]-1, +1[ *
 *                                                            *
 * generate Wyckoff positions in conventional coordinates, in *
 * the range ]-1, +1[ (if the selected option is Basis, the   *
 * array relative contains only one position, equal to xyz)   *
 *                                                            *
 *   apply rotation,translation shift to Wyckoff positions    *
 *                                                            *
 *  convert to primitive coordinates, in the range ]-1, +1[   *
 *                                                            *
 *  convert to absolute coordinates, relative to each node    *
 **************************************************************/

static_conventional (cell, vectors, xyz);

relative = gamgi_phys_space_positions (site, 
xyz, wyckoff, driver, &n_positions, window);

static_shift (cell, n_positions, relative, translation, rotation);

static_primitive (cell, n_positions, relative);
absolute = static_absolute (cell, n_positions, relative);

/*******************************************************
 * Until the last moment, when the object is linked    *
 * to the cell, all the calculations are done in the   *
 * laboratory coordinates. To transform these to cell  *
 * coordinates, in the end, we need to get the cell    *
 * rotation matrix, the initial center (around which   *
 * the laboratory coordinates are rotated) and the     *
 * translate vector (corrected to translate the        *
 * rotated initial origin to the current cell origin). * 
 *                                                     *
 * build inverse matrix, used with Conventional,       *
 * Primitive and Parallelepiped volumes, to decide     *
 * if after adding position to each node the result    *
 * fits inside the cell volume.                        *
 *******************************************************/

gamgi_math_position_rotation (GAMGI_CAST_OBJECT cell, rotate);
gamgi_math_position_translation (GAMGI_CAST_OBJECT cell, translate);
static_volume (cell, inverse);

/************************************************************
 * nodes outside the cell can contribute to what is inside, *
 * in the following cases:                                  *
 * 1) conventional volume with hexagonal primitive lattice  *
 * 2) object-cutoff and parallelepiped, sphere volumes      *
 * 3) object-cutoff, conventional volume, centered lattices *
 * 4) object-cutoff and Wigner-Seitz volumes                *
 ************************************************************/

switch (cell->model)
  {
  case GAMGI_PHYS_CONVENTIONAL: 

  if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
    {
    static_hexagonal_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    break;
    }

  if (nodes_P == TRUE)
    {
    static_primitive_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }

  if (nodes_I == TRUE)
    {
    static_conventional_I_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }

  if (nodes_A == TRUE)
    {
    static_conventional_A_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }

  if (nodes_B == TRUE)
    {
    static_conventional_B_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }

  if (nodes_C == TRUE)
    {
    static_conventional_C_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }

  if (nodes_R == TRUE)
    {
    static_conventional_R_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
    n_positions, relative, absolute, rotate, translate, inverse);
    }
  break;

  case GAMGI_PHYS_PRIMITIVE: 
  static_primitive_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
  n_positions, relative, absolute, rotate, translate, inverse);
  break;

  case GAMGI_PHYS_WIGNER:
  static_wigner_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c,
  n_positions, relative, absolute, rotate, translate, inverse);
  break;

  case GAMGI_PHYS_PARALLELEPIPED:
  static_parallelepiped_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c, 
  n_positions, relative, absolute, rotate, translate, inverse);
  break;

  case GAMGI_PHYS_SPHERE:
  static_sphere_link (cell, atom, cutoff, pattern_a, pattern_b, pattern_c, 
  n_positions, relative, absolute, rotate, translate, inverse);
  break;
  }

free (relative);
free (absolute);
}
