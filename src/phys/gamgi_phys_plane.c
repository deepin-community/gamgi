/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_plane.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_filter.h"
#include "gamgi_engine_find.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_array.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_mesa_start.h"
#include "gamgi_mesa_rotate_out.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_polygon.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_math_position.h"
#include "gamgi_chem_atom.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_reciprocal.h"

static void static_create (gamgi_cell *cell,
gamgi_enum vectors, int h, int k, int l, double *origin)
{
gamgi_plane *plane;
char token[GAMGI_ENGINE_TOKEN];

plane = gamgi_engine_create_plane ();
gamgi_engine_start_plane (plane);
gamgi_mesa_start_plane (plane, gamgi->plane);

sprintf (token, "(%d.%d.%d)", h, k, l);
strcpy (plane->object.name, token);

plane->vectors = vectors;
plane->hkl[0] = h;
plane->hkl[1] = k;
plane->hkl[2] = l;

gamgi_math_vector_copy (origin, plane->origin);
gamgi_phys_reciprocal_points (plane);

/************************
 * plane representation *
 ************************/

plane->model = GAMGI_PHYS_VECTOR;

gamgi_engine_link_plane_cell (plane, cell);
}

static void static_create_all (gamgi_cell *cell, int *index,
double *transformation, double *orientation)
{
double u[3], v[3], translate[3];
int n, h, k, l;
gamgi_enum vectors;

if (cell->model == GAMGI_PHYS_CONVENTIONAL)
  vectors = GAMGI_PHYS_CONVENTIONAL;
else
  vectors = GAMGI_PHYS_PRIMITIVE;

for (n = 0; n < cell->n_nodes; n++)
  {
  h = index[3*n + 0];
  k = index[3*n + 1];
  l = index[3*n + 2];

  /*********************************************************
   * the transformation rotation is around the cell origin *
   *                                                       *
   * the orientation rotation is around the cell center    *
   *********************************************************/

  gamgi_math_matrix_vector (transformation, cell->points + 3*n, u);

  gamgi_math_vector_sub (u, cell->center, v);
  gamgi_math_matrix_vector (orientation, v, u);
  gamgi_math_vector_add (u, cell->center, v);

  /**********************************************************
   * the initial node is always at the cell physical origin *
   **********************************************************/

  if (n == 0)
    gamgi_math_vector_sub (cell->origin, v, translate);

  /******************************
   * there is no plane at (000) *
   ******************************/

  if (gamgi_phys_direction_zero (index + 3*n) == FALSE) 
    {
    gamgi_math_vector_add (v, translate, u);
    static_create (cell, vectors, h, k, l, u);
    }
  }
}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_conventional_F (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = 2 * k;
      index[3*n + 1] = 2 * j;
      index[3*n + 2] = 2 * i;
      n++;
      }
    }
  }

/***********
 * A nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = 2 * k;
      index[3*n + 1] = 2 * j + 1;
      index[3*n + 2] = 2 * i + 1;
      n++;
      }
    }
  }

/***********
 * B nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k + 1;
      index[3*n + 1] = 2 * j;
      index[3*n + 2] = 2 * i + 1;
      n++;
      }
    }
  }

/***********
 * C nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k + 1;
      index[3*n + 1] = 2 * j + 1;
      index[3*n + 2] = 2 * i;
      n++;
      }
    }
  }

}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_conventional_I (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = 2 * k;
      index[3*n + 1] = 2 * j;
      index[3*n + 2] = 2 * i;
      n++;
      }
    }
  }

/***********
 * I nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k + 1;
      index[3*n + 1] = 2 * j + 1;
      index[3*n + 2] = 2 * i + 1;
      n++;
      }
    }
  }

}

/****************** internal function ***************
 *                                                  *
 *                STATIC_CONVENTIONAL_TI            *
 *                                                  *
 * Get (hkl) plane indices in a convencional cell   *
 * volume describing a tI reciprocal lattice. The   *
 * reciprocal of a tI is a tF lattice, which is     *
 * never considered because it is equivalent to a   *
 * tI lattice rotated 45 degrees, but this rotation *
 * must be corrected back for the plane indices.    *
 ****************************************************/

static void static_conventional_tI (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = k - j;
      index[3*n + 1] = j + k;
      index[3*n + 2] = 2 * i;
      n++;
      }
    }
  }

/***********
 * I nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = k - j;
      index[3*n + 1] = j + k + 1;
      index[3*n + 2] = 2 * i + 1;
      n++;
      }
    }
  }

}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_conventional_C (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = 2 * k;
      index[3*n + 1] = 2 * j;
      index[3*n + 2] = i;
      n++;
      }
    }
  }

/***********
 * C nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k + 1;
      index[3*n + 1] = 2 * j + 1;
      index[3*n + 2] = i;
      n++;
      }
    }
  }

}

/****************** internal function ***************
 *                                                  *
 *                                                  *
 ****************************************************/

static void static_conventional_hR (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = 2 * k - j;
      index[3*n + 1] = 2 * j - k;
      index[3*n + 2] = 3 * i;
      n++;
      }
    }
  }

/***********
 * R nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  /***********************************************
   * Obverse convention for r nodes: 2/3 1/3 1/3 *
   ***********************************************/

  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k - j + 1;
      index[3*n + 1] = 2 * j - k;
      index[3*n + 2] = 3 * i + 1;
      n++;
      }
    }

  /***********************************************
   * Obverse convention for r nodes: 1/3 2/3 2/3 *
   ***********************************************/

  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = 2 * k - j;
      index[3*n + 1] = 2 * j - k + 1;
      index[3*n + 2] = 3 * i + 2;
      n++;
      }
    }

  }
}

static void static_primitive (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i <= n3; i++)
  {
  for (j = 0; j <= n2; j++)
    {
    for (k = 0; k <= n1; k++)
      {
      index[3*n + 0] = k;
      index[3*n + 1] = j;
      index[3*n + 2] = i;
      n++;
      }
    }
  }
}

static void static_wigner (gamgi_cell *cell, int *index)
{
int n1, n2, n3;
int i, j, k;
int n;

/**************
 * start data *
 **************/

n1 = cell->n1;
n2 = cell->n2;
n3 = cell->n3;
n = 0;

/***********
 * P nodes *
 ***********/

for (i = 0; i < n3; i++)
  {
  for (j = 0; j < n2; j++)
    {
    for (k = 0; k < n1; k++)
      {
      index[3*n + 0] = k;
      index[3*n + 1] = j;
      index[3*n + 2] = i;
      n++;
      }
    }
  }
}

static int *static_index_nodes (gamgi_cell *cell)
{
int *index;
int n;

index = (int *) malloc (cell->n_nodes * 3 * sizeof (int));

switch (cell->model)
  {
  case GAMGI_PHYS_CONVENTIONAL:
  switch (cell->lattice)
    {
    case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
    static_conventional_F (cell, index);
    break;

    case GAMGI_PHYS_ORTHORHOMBIC_I: case GAMGI_PHYS_CUBIC_I:
    static_conventional_I (cell, index);
    break;

    case GAMGI_PHYS_TETRAGONAL_I:
    static_conventional_tI (cell, index);
    break;

    case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
    static_conventional_C (cell, index);
    break;

    case GAMGI_PHYS_HEXAGONAL_R:
    static_conventional_hR (cell, index);
    break;

    case GAMGI_PHYS_HEXAGONAL_P:
    gamgi_phys_cell_hexagonal_index (cell, index);
    break;

    default:
    static_primitive (cell, index);
    break;
    }
  break;

  case GAMGI_PHYS_PRIMITIVE:
  static_primitive (cell, index);
  break;

  case GAMGI_PHYS_WIGNER:
  static_wigner (cell, index);
  break;

  case GAMGI_PHYS_PARALLELEPIPED:
  gamgi_phys_cell_parallelepiped_index (cell, index);
  break;

  case GAMGI_PHYS_SPHERE:
  gamgi_phys_cell_sphere_index (cell, index);
  break;

  default:
  break;
  }

/*****************************************************
 * The reciprocal of the hP lattice makes an angle   *
 * between b1 and b2 of 60 degrees, so to have again *
 * 120 degrees, b2 was replaced by b2-b1, but this   *
 * must be corrected back for the plane indices.     *
 *****************************************************/

if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
  for (n = 0; n < cell->n_nodes; n++)
    index[3*n + 0] -= index[3*n + 1];

return index;
}

static void static_index_origin (gamgi_cell *cell, int *offset)
{
int o1, o2, o3;
gamgi_enum o4;

/*******************************************
 * primitive lattices OR primitive volumes *
 *******************************************/

if (cell->model != GAMGI_PHYS_CONVENTIONAL ||
gamgi_phys_cell_primitive (cell->lattice) == cell->lattice)
  { 
  offset[0] = cell->o1;
  offset[1] = cell->o2;
  offset[2] = cell->o3;
  return;
  }

/**********************************************
 * centered lattices AND conventional volumes *
 **********************************************/

gamgi_math_node_conventional (cell->lattice, 
cell->o1, cell->o2, cell->o3, &o1, &o2, &o3, &o4);
  
if (cell->lattice == GAMGI_PHYS_CUBIC_I || 
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_I)
  {
  offset[0] = 2 * o1;
  offset[1] = 2 * o2;
  offset[2] = 2 * o3;
  if (o4 == GAMGI_PHYS_111)
     { offset[0] += 1; offset[1] += 1; offset[2] += 1; }
  return;
  }
  
if (cell->lattice == GAMGI_PHYS_CUBIC_F ||
cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_F)
  {
  offset[0] = 2 * o1;
  offset[1] = 2 * o2;
  offset[2] = 2 * o3;
  if (o4 == GAMGI_PHYS_110) { offset[0] += 1; offset[1] += 1; }
  if (o4 == GAMGI_PHYS_101) { offset[0] += 1; offset[2] += 1; }
  if (o4 == GAMGI_PHYS_011) { offset[1] += 1; offset[2] += 1; }
  return;
  }

if (cell->lattice == GAMGI_PHYS_ORTHORHOMBIC_C ||
cell->lattice == GAMGI_PHYS_MONOCLINIC_C)
  {
  offset[0] = 2 * o1;
  offset[1] = 2 * o2;
  offset[2] = o3;
  if (o4 == GAMGI_PHYS_110) { offset[0] += 1; offset[1] += 1; }
  return;
  }

/********************************************************
 * tI: indexes are given for the Tetragonal F lattice,  *
 * equivalent to the I lattice, when rotated 45 degrees *
 *********************************************************/

if (cell->lattice == GAMGI_PHYS_TETRAGONAL_I)
  {
  offset[0] = o1 - o2;
  offset[1] = o1 + o2;
  offset[2] = 2 * o3;
  if (o4 == GAMGI_PHYS_111) { offset[1] += 1; offset[2] += 1; }
  return;
  }

/*******************************************
 * hR: (0 0 0) (2/3 1/3 1/3) (1/3 2/3 2/3) *
 *******************************************/

if (cell->lattice == GAMGI_PHYS_HEXAGONAL_R)
  {
  offset[0] = 2 * o1 - o2;
  offset[1] = 2 * o2 - o1;
  offset[2] = 3 * o3;
  if (o4 == GAMGI_PHYS_211) { offset[0] += 1; offset[2] += 1; }
  if (o4 == GAMGI_PHYS_122) { offset[1] += 1; offset[2] += 2; }
  return;
  }

}

static void static_index_translate (gamgi_cell *cell, int *offset, int *index)
{
int n;

/********************************************
 * offset represents the new plane indices  *
 * for the physical origin, initially (000) *
 ********************************************/

for (n = 0; n < cell->n_nodes; n++)
  {
  index[3*n + 0] = index[3*n + 0] - offset[0];
  index[3*n + 1] = index[3*n + 1] - offset[1];
  index[3*n + 2] = index[3*n + 2] - offset[2];
  }
}

static void static_plane_h00 (double *r1, double *a2, double *a3, double *out2, double *out3)
{
gamgi_math_vector_add (r1, a2, out2);
gamgi_math_vector_add (r1, a3, out3);
}

static void static_plane_hk0 (double *r1, double *r2, double *a3, double *out3)
{
/*******************************************************
 * get the middle point of the two intersection points *
 * and add the third axis, to generate the third point *
 *******************************************************/

gamgi_math_vector_add (r1, r2, out3);
gamgi_math_vector_scale (out3, out3, 0.5);
gamgi_math_vector_add (out3, a3, out3);
}

static void static_plane_hkl (gamgi_cell *cell, gamgi_plane *plane, 
double *r1, double *r2, double *r3)
{
double *v1, *v2, *v3;
int h, k, l;

/**************
 * start data *
 **************/

h = plane->hkl[0];
k = plane->hkl[1];
l = plane->hkl[2];

/*******************************************
 * get conventional or primitive cell axes *
 *******************************************/

v1 = NULL; v2 = NULL; v3 = NULL;
if (plane->vectors == GAMGI_PHYS_CONVENTIONAL)
  { v1 = cell->a1; v2 = cell->a2; v3 = cell->a3; }
if (plane->vectors == GAMGI_PHYS_PRIMITIVE)
  { v1 = cell->p1; v2 = cell->p2; v3 = cell->p3; }

/*******************************************
 * get three points representing the plane *
 *******************************************/

if (h != 0) gamgi_math_vector_scale (v1, r1, 1.0/h);
if (k != 0) gamgi_math_vector_scale (v2, r2, 1.0/k);
if (l != 0) gamgi_math_vector_scale (v3, r3, 1.0/l);

/************************************************
 * plane is parallel to one axis: add this axis *
 * to the middle point of the two valid points  *
 ************************************************/

if (h == 0 && k != 0 && l != 0) static_plane_hk0 (r2, r3, v1, r1);
if (h != 0 && k == 0 && l != 0) static_plane_hk0 (r1, r3, v2, r2);
if (h != 0 && k != 0 && l == 0) static_plane_hk0 (r1, r2, v3, r3);

/*************************************
 * plane is parallel to two axes:    *
 * add these axes to the valid point *
 *************************************/

if (h != 0 && k == 0 && l == 0) static_plane_h00 (r1, v2, v3, r2, r3);
if (h == 0 && k != 0 && l == 0) static_plane_h00 (r2, v1, v3, r1, r3);
if (h == 0 && k == 0 && l != 0) static_plane_h00 (r3, v1, v2, r1, r2);
}

static void static_center (gamgi_plane *plane)
{
int n;

gamgi_math_vector_zero (plane->center);

/*********************************
 * at least one point must exist *
 *********************************/

for (n = 0; n < plane->n_points; n++)
  {
  plane->center[0] += plane->points[3*n + 0];
  plane->center[1] += plane->points[3*n + 1];
  plane->center[2] += plane->points[3*n + 2];
  }

gamgi_math_vector_scale (plane->center, plane->center, 1.0 / plane->n_points);
}

static gamgi_darray *static_polygon_dlist (gamgi_plane *plane,
double thickness, double *position, double *normal, gamgi_dlist *dlist)
{
gamgi_atom *atom;
gamgi_darray *darray;
double point[4];
double length;

/**************************************************
 * store 4-coordinate points: the 4th coordinate  *
 * will be used later to order the polygon points *
 **************************************************/

darray = gamgi_engine_darray_create (GAMGI_MATH_PLANE_SIZE, 
GAMGI_MATH_PLANE_STEP, 4);

/***************************************************
 * build plane polygon with points where are atoms *
 * in parent closer to the plane than thickness    *
 ***************************************************/

while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, position, point);
  length = gamgi_math_vector_dot (point, normal);

  if (fabs (length) < thickness)
    {
    /**********************************
     * add projected point to polygon *
     **********************************/

    gamgi_math_vector_scale (normal, point, length);
    gamgi_math_vector_sub (atom->position, point, point);
    gamgi_engine_darray_push (darray, point);
    }

  dlist = dlist->next;
  }

return darray;
}

static gamgi_darray *static_polygon_array (gamgi_plane *plane,
int n_neighbours, gamgi_atom **neighbours)
{
gamgi_atom *atom;
gamgi_darray *darray;
int i;

/**************************************************
 * store 4-coordinate points: the 4th coordinate  *
 * will be used later to order the polygon points *
 **************************************************/

darray = gamgi_engine_darray_create (GAMGI_MATH_PLANE_SIZE,
GAMGI_MATH_PLANE_STEP, 4);

/*******************************************
 * build plane polygon with all neighbours *
 *******************************************/

for (i = 0; i < n_neighbours; i++)
  {
  /*****************************
   * add atom point to polygon *
   *****************************/

  atom = neighbours[i];
  gamgi_engine_darray_push (darray, atom->position);
  }

return darray;
}

static void static_copy (gamgi_plane *plane, double thickness,
gamgi_dlist *start, double *normal)
{
gamgi_atom *atom, *atom_new;
gamgi_dlist *dlist;
double r[3];
double length;

/********************************************
 * copy and link to the plane all atoms     *
 * in parent that are closer than thickness *
 ********************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, plane->center, r);
  length = gamgi_math_vector_dot (r, normal);

  if (fabs (length) < thickness)
    {
    atom_new = gamgi_engine_copy_atom (atom);
    gamgi_engine_link_atom_plane (atom_new, plane);
    }
  }

/********************************
 * clean bonds in parent object *
 ********************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  gamgi_chem_atom_clean (GAMGI_CAST_ATOM dlist->data);
}

static void static_link (gamgi_plane *plane, double thickness,
gamgi_dlist *start, double *normal)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
double r[3];
double length;

/********************************************
 * link atoms to plane: this is used in     *
 * atomic planes and crystalographic planes *
 ********************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, plane->center, r);
  length = gamgi_math_vector_dot (r, normal);

  if (fabs (length) < thickness)
    {
    gamgi_engine_unlink_atom (atom);
    gamgi_engine_link_atom_plane (atom, plane);
    }
  }
}

static gamgi_bool static_normal (gamgi_plane *plane, double *normal)
{
double r1[3], r2[3];
double cross[3];
double length;
int i, j, k;

/**************************************************
 * go around polygon, accumulating normal vectors *
 **************************************************/

for (i = 0; i < plane->n_points - 2; i++)
  {
  for (j = i + 1; j < plane->n_points - 1; j++)
    {
    for (k = j + 1; k < plane->n_points; k++)
      {
      gamgi_math_vector_sub (plane->points + 3*j, plane->points + 3*i, r1);
      gamgi_math_vector_sub (plane->points + 3*k, plane->points + 3*j, r2);
      gamgi_math_vector_cross (r1, r2, cross);
      gamgi_math_vector_add (normal, cross, normal);
      }
    }
  }

/***************************************************
 * normalize vector perpendicular to plane polygon *
 ***************************************************/

length = gamgi_math_vector_length (normal);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
gamgi_math_vector_scale (normal, normal, 1/length);

return TRUE;
}

static gamgi_bool static_range (gamgi_plane *plane, gamgi_cell *cell,
gamgi_enum structure, double thickness, gamgi_dlist *dlist, int order,
double *normal, gamgi_bool link)
{
gamgi_plane *plane_new;
gamgi_bool inside;

/**************************************************************
 * Plane Create: plane is just a template, removed in the     *
 * end, used to copy data to plane_new, the real plane        *
 *                                                            *
 * Plane Modify: plane is the real plane, the plane to modify *
 *                                                            *
 * plane->object.object is used here as a flag to distinguish *
 * between the two cases. plane->object.object can be used    *
 * because plane is always unlinked at this stage             *
 **************************************************************/

if (plane->object.object == NULL)
  plane_new = gamgi_engine_copy_plane (plane);
else
  { plane_new = plane; plane->object.object = NULL; }

if (strcmp (plane->object.name, "") == 0) gamgi_phys_plane_name
(plane->hkl[0], plane->hkl[1], plane->hkl[2], plane_new->object.name);
plane_new->order = order;

/*******************
 * build new plane *
 *******************/

inside = gamgi_phys_plane_cell (plane_new, cell);

/*****************************************************
 * link plane to cell, copy,link,none atoms to plane *
 *****************************************************/

if (inside == TRUE || link == TRUE)
  {
  gamgi_engine_link_plane_cell (plane_new, cell);
  if (structure == GAMGI_CHEM_STRUCTURE_COPY)
    static_copy (plane_new, thickness, dlist, normal);
  if (structure == GAMGI_CHEM_STRUCTURE_LINK)
    static_link (plane_new, thickness, dlist, normal);
  }
else gamgi_engine_remove_plane (plane_new);

/**********************************
 * return flag informing if the   *
 * plane is still inside the cell *
 **********************************/

return inside;
}

static void static_projection (gamgi_plane *plane,
gamgi_cell *cell, gamgi_enum model)
{
gamgi_plane *plane_new;

/**************************************************************
 * Plane Create: plane is just a template, removed in the     *
 * end, used to copy data to plane_new, the real plane        *
 *                                                            *
 * Plane Modify: plane is the real plane to modify            *
 *                                                            *
 * plane->object.object is used here as a flag to distinguish *
 * between the two cases. plane->object.object can be used    *
 * because plane is always unlinked at this stage             *
 **************************************************************/

if (plane->object.object == NULL)
  plane_new = gamgi_engine_copy_plane (plane);
else
  { plane_new = plane; plane->object.object = NULL; }

if (strcmp (plane->object.name, "") == 0)
  gamgi_phys_plane_name (plane->hkl[0], plane->hkl[1],
  plane->hkl[2], plane_new->object.name);
plane_new->model = model;

gamgi_phys_projection_create (plane_new->model, &plane_new->points,
&plane_new->n_points, &plane_new->loops, &plane_new->n_loops);

gamgi_engine_link_plane_cell (plane_new, cell);
gamgi_mesa_rotate_out_plane (plane_new, NULL, NULL, NULL);
}

int gamgi_phys_plane_order (gamgi_cell *cell, 
int *hkl, gamgi_enum vectors, int o1, int o2, int o3)
{
double normal[3], position[3];
double shift[3];
double length, order;
int n;

/*************************
 * get reciprocal vector *
 * get normal vector     *
 *************************/

gamgi_phys_plane_vector (cell, hkl, vectors, shift);
length = gamgi_math_vector_length (shift);
gamgi_math_vector_scale (shift, normal, 1.0 / length);

/**************************************************************
 * Get position for primitive node o1,o2,o3 and project on    *
 * plane normal to get the total distance to node 000. Then   *
 * divide by distance between planes to get the plane order,  *
 * the number of planes between the two nodes. Opposite       *
 * planes such as (111) and (-1-1-1) have symmetrical orders. *
 **************************************************************/

gamgi_math_vector_translation (o1, o2, o3,
cell->p1, cell->p2, cell->p3, position);
order = length * gamgi_math_vector_dot (position, normal);

/************************************
 * convert float (order) to int (n) *
 ************************************/

n = 0;
if (order > 0.0) n = floor (order + 0.5);
if (order < 0.0) n = ceil (order - 0.5);

return n;
}

void gamgi_phys_plane_vector (gamgi_cell *cell,
int *hkl, gamgi_enum vectors, double *vector)
{
double b1[3], b2[3], b3[3];

/*************************************************
 * build reciprocal lattice vectors, to get the  *
 * directon vector that is normal to the plane   *
 *                                               *
 * the reciprocal lattice constant is set to 1.0 *
 *************************************************/

if (vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_phys_reciprocal_vectors (1.0, cell->a1, cell->a2, cell->a3, b1, b2, b3);
if (vectors == GAMGI_PHYS_PRIMITIVE)
  gamgi_phys_reciprocal_vectors (1.0, cell->p1, cell->p2, cell->p3, b1, b2, b3);

/********************************************************
 * get translation vector in absolute x,y,z coordinates *
 ********************************************************/

gamgi_math_vector_translation (hkl[0], hkl[1], hkl[2], b1, b2, b3, vector);
}

int gamgi_phys_plane_increase (gamgi_enum lattice,
int *hkl, gamgi_enum vectors, int *hkl_multiple)
{
hkl_multiple[0] = hkl[0];
hkl_multiple[1] = hkl[1];
hkl_multiple[2] = hkl[2];

if (vectors == GAMGI_PHYS_PRIMITIVE) return 1;

/******************************************************
 * In centered cells, some conventional indices       *
 * represent planes which are a subset of other       *
 * planes closer to the origin, with the same         *
 * orientation. These planes cannot be described      *
 * in terms of primitive vectors and are not present  *
 * in the reciprocal lattice, so strictly speaking,   *
 * they do not exist, in crystallographic terms.      *
 *                                                    *
 * This routine converts these planes to those with   *
 * the same orientation that are closer to the origin *
 ******************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  case GAMGI_PHYS_CUBIC_F:
  if (hkl[0] % 2 == 0 && hkl[1] % 2 == 0 && hkl[2] % 2 == 0) return 1;
  if (hkl[0] % 2 != 0 && hkl[1] % 2 != 0 && hkl[2] % 2 != 0) return 1;
  hkl_multiple[0] *= 2; hkl_multiple[1] *= 2; hkl_multiple[2] *= 2;
  return 2;

  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_CUBIC_I:
  if ((hkl[0] + hkl[1] + hkl[2]) % 2 == 0) return 1;
  hkl_multiple[0] *= 2; hkl_multiple[1] *= 2; hkl_multiple[2] *= 2;
  return 2;

  case GAMGI_PHYS_MONOCLINIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  if ((hkl[0] + hkl[1]) % 2 == 0) return 1;
  hkl_multiple[0] *= 2; hkl_multiple[1] *= 2; hkl_multiple[2] *= 2;
  return 2;

  case GAMGI_PHYS_HEXAGONAL_R:
  if ((-hkl[0] + hkl[1] + hkl[2]) % 3 == 0) return 1;
  hkl_multiple[0] *= 3; hkl_multiple[1] *= 3; hkl_multiple[2] *= 3;
  return 3;

  default:
  break;
  }

return 1;
}

gamgi_bool gamgi_phys_plane_miller (gamgi_enum lattice, 
gamgi_enum vectors, int *hkl)
{
int hkl_min[3];
int hkl_max[3];

gamgi_phys_direction_decrease (hkl, hkl_min);
gamgi_phys_plane_increase (lattice, hkl_min, vectors, hkl_max);

if (hkl_max[0] != hkl[0] || hkl_max[1] != hkl[1] || 
hkl_max[2] != hkl[2]) return FALSE;

return TRUE;
}

gamgi_bool gamgi_phys_plane_laue (gamgi_enum lattice,
gamgi_enum vectors, int *hkl)
{
int hkl_min[3];
int hkl_max[3];

gamgi_phys_direction_decrease (hkl, hkl_min);
gamgi_phys_plane_increase (lattice, hkl_min, vectors, hkl_max);

if (gamgi_phys_direction_higher (hkl, hkl_max) == TRUE) return TRUE;

return FALSE;
}

void gamgi_phys_plane_conventional (gamgi_enum lattice,
int *hkl_p, int *hkl_c)
{
/***********************************************************
 * convert planes indices from primitive to conventional   *
 * vectors: a valid solution (integer numbers) always      *
 * exists. The plane order remains the same.               *
 *                                                         *
 * The reciprocal lattice vector describing a given plane  *
 * must be the same in primitive and conventional vectors, *
 * so equalizing the two vectors, the relation between     *
 * the two sets of Miller indices can be obtained.         *
 ***********************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  case GAMGI_PHYS_CUBIC_F:
  hkl_c[0] = + hkl_p[0] + hkl_p[1] - hkl_p[2];
  hkl_c[1] = - hkl_p[0] + hkl_p[1] + hkl_p[2];
  hkl_c[2] = + hkl_p[0] - hkl_p[1] + hkl_p[2];
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_CUBIC_I:
  hkl_c[0] = + hkl_p[0] + hkl_p[1];
  hkl_c[1] = + hkl_p[1] + hkl_p[2];
  hkl_c[2] = + hkl_p[0] + hkl_p[2];
  break;

  case GAMGI_PHYS_MONOCLINIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  hkl_c[0] = + hkl_p[0] + hkl_p[1];
  hkl_c[1] = - hkl_p[0] + hkl_p[1];
  hkl_c[2] = + hkl_p[2];
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  hkl_c[0] = + hkl_p[0] - hkl_p[1];
  hkl_c[1] = + hkl_p[1] - hkl_p[2];
  hkl_c[2] = + hkl_p[0] + hkl_p[1] + hkl_p[2];
  break;

  default:
  hkl_c[0] = hkl_p[0];
  hkl_c[1] = hkl_p[1];
  hkl_c[2] = hkl_p[2];
  }

}

void gamgi_phys_plane_primitive (gamgi_enum lattice,
int *hkl_c, int *hkl_p)
{
/***********************************************************
 * convert planes indices from conventional to primitive   *
 * vectors: a valid solution (integer numbers) only exists *
 * when the plane is not a multiple of other planes, i.e.  *
 * when there is no other plane closer to the origin. The  *
 * plane order remains the same.                           *
 *                                                         *
 * The reciprocal lattice vector describing a given plane  *
 * must be the same in primitive and conventional vectors, *
 * so equalizing the two vectors, the relation between     *
 * the two sets of Miller indices can be obtained.         *
 ***********************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  case GAMGI_PHYS_CUBIC_F:
  hkl_p[0] = (+ hkl_c[0] + hkl_c[2]) / 2;
  hkl_p[1] = (+ hkl_c[0] + hkl_c[1]) / 2;
  hkl_p[2] = (+ hkl_c[1] + hkl_c[2]) / 2;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_CUBIC_I:
  hkl_p[0] = (+ hkl_c[0] - hkl_c[1] + hkl_c[2]) / 2;
  hkl_p[1] = (+ hkl_c[0] + hkl_c[1] - hkl_c[2]) / 2;
  hkl_p[2] = (- hkl_c[0] + hkl_c[1] + hkl_c[2]) / 2;
  break;

  case GAMGI_PHYS_MONOCLINIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  hkl_p[0] = (+ hkl_c[0] - hkl_c[1]) / 2;
  hkl_p[1] = (+ hkl_c[0] + hkl_c[1]) / 2;
  hkl_p[2] = hkl_c[2];
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  hkl_p[0] = (2 * hkl_c[0] + hkl_c[1] + hkl_c[2]) / 3;
  hkl_p[1] = (- hkl_c[0] + hkl_c[1] + hkl_c[2]) / 3;
  hkl_p[2] = (- hkl_c[0] - 2 * hkl_c[1] + hkl_c[2]) / 3;
  break;

  default:
  hkl_p[0] = hkl_c[0];
  hkl_p[1] = hkl_c[1];
  hkl_p[2] = hkl_c[2];
  }

}

void gamgi_phys_plane_reciprocal (gamgi_cell *cell,
double *transformation, double *orientation)
{
int *index;
int offset[3] = {0};

/*****************************************************
 * initialize offset, to avoid a compilation warning *
 *****************************************************/

index = static_index_nodes (cell);
static_index_origin (cell, offset);
static_index_translate (cell, offset, index);

static_create_all (cell, index, transformation, orientation);

free (index);
}

void gamgi_phys_plane_point (gamgi_plane *plane, 
gamgi_cell *cell, double *point)
{
double rotate[9];

/***********************************
 * get plane->quaternion to rotate *
 * and plane->origin to translate  *
 * the point defining the plane,   *
 * initially in plane coordinates  *
 ***********************************/

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_math_matrix_vector (rotate, plane->points, point);
gamgi_math_vector_add (point, cell->origin, point);
}

gamgi_bool gamgi_phys_plane_normal (gamgi_plane *plane, double *normal)
{
double rotate[9];
double vector[3];

if (static_normal (plane, vector) == FALSE) return FALSE;

gamgi_math_quaternion_to_matrix (plane->quaternion, rotate);
gamgi_math_matrix_vector (rotate, vector, normal);

return TRUE;
}

void gamgi_phys_plane_default (double *p1, double *p2, 
double *p3, gamgi_bool *inside, gamgi_darray *darray)
{
gamgi_engine_darray_pop (darray, 0);
gamgi_engine_darray_push (darray, p1);
gamgi_engine_darray_push (darray, p2);
gamgi_engine_darray_push (darray, p3);

*inside = FALSE;
}

gamgi_bool gamgi_phys_plane_cell (gamgi_plane *plane, gamgi_cell *cell)
{
gamgi_cell *cell_new;
gamgi_darray *darray;
double rotate[9];
double r1[3], r2[3], r3[3];
double normal[3], origin[3];
double translate[3], shift[3];
double position[3];
double length, area;
gamgi_bool inside = TRUE;

/************************
 * plane representation *
 ************************/

plane->model = GAMGI_PHYS_POLYGON;

/*******************************************
 * get 3 points describing the plane: they *
 * are far enough to describe a real plane *
 *******************************************/

static_plane_hkl (cell, plane, r1, r2, r3);
gamgi_math_cell_offset (cell, origin);
gamgi_math_vector_add (r1, origin, r1);
gamgi_math_vector_add (r2, origin, r2);
gamgi_math_vector_add (r3, origin, r3);

/****************************************
 * get normal and shift vectors: shift  *
 * is a normal vector with a length     *
 * equal to the distance between planes *
 ****************************************/

gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, shift);
length = gamgi_math_vector_length (shift);
gamgi_math_vector_scale (shift, normal, 1.0 / length);
gamgi_math_vector_scale (normal, shift, 1.0 / length);

/*********************************************************************
 * Planes of the same family are equivalent and equally spaced.      *
 * The default plane coordinates are for the plane closer to the     *
 * origin, with intersections 1/h 1/k 1/l. Planes with intersections *
 * n/h n/k n/l are obtained when order (n) is different from 1.      *
 *********************************************************************/

gamgi_math_vector_scale (shift, translate, plane->order - 1);
gamgi_math_vector_add (r1, translate, r1);
gamgi_math_vector_add (r2, translate, r2);
gamgi_math_vector_add (r3, translate, r3);

if (cell->model == GAMGI_PHYS_SPHERE)
  {
  /********************************************
   * get an array of points describing the    *
   * plane intersection with the cell sphere  *
   *                                          *
   * build default triangle polygon when the  *
   * plane does not intersect the cell volume *
   ********************************************/

  darray = gamgi_math_polygon_circle (cell, plane, 
  normal, GAMGI_MESA_PLANE_SLICES, r1, r2, r3);
  if (darray->offset < 3)
    gamgi_phys_plane_default (r1, r2, r3, &inside, darray);
  }
else
  {
  /********************************************
   * get an array of points describing the    *
   * plane intersection with the cell loops   *
   *                                          *
   *  build a new equivalent cell with all    *
   *  the loops needed to test intersection   *
   *                                          *
   * build default triangle polygon when the  *
   * plane does not intersect the cell volume *
   ********************************************/

  cell_new = gamgi_phys_cell_polygons (cell);
  darray = gamgi_math_polygon_scan (cell_new->loops, cell_new->points, r1, r2, r3);
  gamgi_engine_remove_cell (cell_new);
  if (darray->offset < 3)
    gamgi_phys_plane_default (r1, r2, r3, &inside, darray);

  /**********************************************
   * orientate polygon counterclockwise when    *
   * looking with origin on the other side: the *
   * normal vector points away from the origin  *
   *                                            *
   *     expand polygon to make it convex       *
   **********************************************/

  gamgi_math_polygon_orientate (darray, normal);
  gamgi_math_polygon_convex (darray, normal);

  /*******************************************
   * build default triangle polygon when the *
   * plane barely intersects the cell volume *
   *******************************************/

  area = gamgi_math_polygon_area (darray->offset, darray->array);
  if (area < GAMGI_MATH_PLANE_AREA)
    gamgi_phys_plane_default (r1, r2, r3, &inside, darray);
  }

/***************************************************
 * set points,loops arrays and remove array object *
 ***************************************************/

gamgi_engine_darray_copy_remove (darray, &plane->points, &plane->n_points);
gamgi_math_polygon_loops (plane->n_points, &plane->loops, &plane->n_loops);

/****************************************
 * set plane center, origin, quaternion *
 ****************************************/

static_center (plane);
gamgi_math_position_rotation (GAMGI_CAST_OBJECT cell, rotate);
gamgi_math_position_translation (GAMGI_CAST_OBJECT cell, translate);
gamgi_math_matrix_vector (rotate, plane->center, position);
gamgi_math_vector_add (position, translate, plane->center);

gamgi_math_vector_copy (cell->origin, plane->origin);
plane->quaternion[0] = cell->quaternion[0];
plane->quaternion[1] = cell->quaternion[1];
plane->quaternion[2] = cell->quaternion[2];
plane->quaternion[3] = cell->quaternion[3];

/****************************************************
 * return FALSE if plane is outside the cell volume *
 ****************************************************/
 
return inside;
}

gamgi_object *gamgi_phys_plane_parent (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_window *window)
{
gamgi_object *parent;

/**********************************************************
 * return first parent common to the three defining atoms *
 **********************************************************/

parent = gamgi_engine_find_root (GAMGI_CAST_OBJECT atom1,
GAMGI_CAST_OBJECT atom2, GAMGI_CAST_OBJECT window->layer);

parent = gamgi_engine_find_root (parent,
GAMGI_CAST_OBJECT atom3, GAMGI_CAST_OBJECT window->layer);

return parent;
}

gamgi_bool gamgi_phys_plane_polygon (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3)
{
double u[3], v[3], cross[3];
double length;

gamgi_math_vector_sub (atom1->position, atom2->position, u);
gamgi_math_vector_sub (atom3->position, atom2->position, v);
gamgi_math_vector_cross (u, v, cross);
length = gamgi_math_vector_length (cross);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

return TRUE;
}

gamgi_dlist *gamgi_phys_plane_reorder (gamgi_dlist *start,
gamgi_atom *atom1, gamgi_atom *atom2, gamgi_atom *atom3)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
gamgi_bool move = FALSE;

/***************************************
 * reorder dlist so the three defining *
 * atoms are always in the beginning   *
 ***************************************/

dlist = start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  if (atom == atom1 || atom == atom2 || atom == atom3)
    { 
    if (move == TRUE)
      {
      dlist = gamgi_engine_dlist_remove_start (dlist);
      start = gamgi_engine_dlist_add_start (start);
      start->data = atom;
      }
    else dlist = dlist->next;
    }
  else { move = TRUE; dlist = dlist->next; }
  }

return start;
}

void gamgi_phys_plane_atoms (gamgi_plane *plane, gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_dlist *dlist,
gamgi_enum structure, double thickness)
{
gamgi_darray *darray;
double unit[9], rotation[9];
double zero[3], normal[3], translation[3];

/************************
 * plane representation *
 ************************/

plane->model = GAMGI_PHYS_POLYGON;

/********************************************************************
 * initially the parent object is rotated and translated backward   *
 * so the plane is built with zero translation and rotation. In the *
 * end, the parent object is rotated and translated forward to its  *
 * initial position, this time already including the plane, which   *
 * will acquire the same origin and rotation as the parent object.  *
 ********************************************************************/

gamgi_math_position_rotation (plane->object.object, rotation);
gamgi_math_position_translation (plane->object.object, translation);

gamgi_math_vector_zero (zero);
gamgi_math_matrix_unit (unit);
gamgi_math_position_object_rotation (plane->object.object, unit);
gamgi_math_position_object_translation (plane->object.object, zero);

/***********************************************
 *  get normal vector: we tested before if     *
 *  the three atoms can define a real plane    *
 *                                             *
 *    use atom1 for a point in the plane       *
 *                                             *
 *    get darray with all polygon points       *
 *                                             *
 * orient polygon points, remove the redundant *
 * ones and expand polygon to make it convex   *
 ***********************************************/

gamgi_math_polygon_normal (atom1->position,
atom2->position, atom3->position, normal);

darray = static_polygon_dlist (plane,
thickness, atom1->position, normal, dlist);

gamgi_math_polygon_orientate (darray, normal);
gamgi_math_polygon_convex (darray, normal);

/***************************
 * save points array data  *
 * and remove array object *
 *                         *
 *   create loops array    *
 *                         *
 *   get polygon center    *
 ***************************/

gamgi_engine_darray_copy_remove (darray, &plane->points, &plane->n_points);
gamgi_math_polygon_loops (plane->n_points, &plane->loops, &plane->n_loops);
static_center (plane);

/*********************************
 * link,copy,none atoms to plane *
 *********************************/

if (structure == GAMGI_CHEM_STRUCTURE_COPY)
  static_copy (plane, thickness, dlist, normal);
if (structure == GAMGI_CHEM_STRUCTURE_LINK)
  static_link (plane, thickness, dlist, normal);

/***************************************
 * rotate the parent object, including *
 * the plane, to its original position *
 ***************************************/

gamgi_math_position_object_rotation (plane->object.object, rotation);
gamgi_math_position_object_translation (plane->object.object, translation);

/**********************
 * free list of atoms *
 **********************/

while (dlist != NULL)
  dlist = gamgi_engine_dlist_remove_start (dlist);
}

void gamgi_phys_plane_polytope (int n_neighbours, gamgi_atom **neighbours,
gamgi_enum structure, float red, float green, float blue, gamgi_object *parent,
gamgi_atom *central)
{
gamgi_plane *plane;
gamgi_atom *atom;
gamgi_darray *darray;
double unit[9], rotation[9];
double zero[3], normal[3], translation[3];
int i;

/****************************************************
 * link plane and set type before handling rotation *
 ****************************************************/

plane = gamgi_engine_create_plane ();
gamgi_engine_start_plane (plane);
gamgi_engine_link_object_object (GAMGI_CAST_OBJECT plane, parent);
strcpy (plane->object.name, "Plane");

plane->reference = GAMGI_ENGINE_ATOM;
plane->model = GAMGI_PHYS_POLYGON;

plane->red = red;
plane->green = green;
plane->blue = blue;

/**********************************************************************
 * initially the parent object is rotated and translated backward so  *
 * the direction is built with zero translation and rotation. In the  *
 * end, the parent object is rotated and translated forward to its    *
 * initial position, this time already including the direction, which *
 * will acquire the same origin and rotation as the parent object.    *
 **********************************************************************/

gamgi_math_vector_zero (zero);
gamgi_math_matrix_unit (unit);

gamgi_math_position_plane_rotation_set (plane, zero);
gamgi_math_position_plane_translation_set (plane, zero);

gamgi_math_position_rotation (parent, rotation);
gamgi_math_position_translation (parent, translation);

gamgi_math_position_object_rotation (parent, unit);
gamgi_math_position_object_translation (parent, zero);

/***********************************************
 *  get normal vector: by construction, the    *
 *  first three atoms can define a real plane  *
 *                                             *
 * use neighbours[0] for a point in the plane  *
 *                                             *
 *    get darray with all polygon points       *
 *                                             *
 * orient polygon points, remove the redundant *
 * ones and expand polygon to make it convex   *
 ***********************************************/

gamgi_math_polygon_normal (neighbours[0]->position,
neighbours[1]->position, neighbours[2]->position, normal);

darray = static_polygon_array (plane, n_neighbours, neighbours);

gamgi_math_polygon_orientate (darray, normal);
gamgi_math_polygon_convex (darray, normal);

/***************************
 * save points array data  *
 * and remove array object *
 *                         *
 *   create loops array    *
 *                         *
 *   get polygon center    *
 ***************************/

gamgi_engine_darray_copy_remove (darray, &plane->points, &plane->n_points);
gamgi_math_polygon_loops (plane->n_points, &plane->loops, &plane->n_loops);
static_center (plane);

/*********************
 * include structure *
 *********************/

if (structure == GAMGI_CHEM_STRUCTURE_COPY)
  {
  /**********************************************
   * copy central, neighbour atoms to group     *
   *                                            *
   * clean bonds in central and neighbour atoms *
   **********************************************/

  atom = gamgi_engine_copy_atom (central);
  gamgi_engine_link_atom_plane (atom, plane);
  for (i = 0; i < n_neighbours; i++)
    {
    atom = gamgi_engine_copy_atom (neighbours[i]);
    gamgi_engine_link_atom_plane (atom, plane);
    }

  gamgi_chem_atom_clean (central);
  for (i = 0; i < n_neighbours; i++)
    gamgi_chem_atom_clean (neighbours[i]);
  }

if (structure == GAMGI_CHEM_STRUCTURE_LINK)
  {
  /******************************************
   * link central, neighbour atoms to group *
   ******************************************/

  gamgi_engine_unlink_atom (central);
  gamgi_engine_link_atom_plane (central, plane);
  for (i = 0; i < n_neighbours; i++)
    {
    gamgi_engine_unlink_atom (neighbours[i]);
    gamgi_engine_link_atom_plane (neighbours[i], plane);
    }
  }

/***************************************
 * rotate the parent object, including *
 * the plane, to its original position *
 ***************************************/

gamgi_math_position_object_rotation (plane->object.object, rotation);
gamgi_math_position_object_translation (plane->object.object, translation);
}

void gamgi_phys_plane_range (gamgi_plane *plane, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness,
gamgi_dlist *dlist, int start, int end)
{
double rotate[9];
double vector[3], normal[3];
int order, n;

/**************************************************************
 * get plane normal,rotated according to the cell orientation *
 **************************************************************/

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, vector);
gamgi_math_matrix_vector (rotate, vector, normal);
gamgi_math_vector_normal (normal);

/**********************************************************
 * build new planes, link to cell, apply structure policy *
 **********************************************************/

/**************************************************************
 * show all planes inside: 1) get order of plane passing      *
 * through the cell origin (0, if the origin did not change); *
 * 2) scan all planes in both directions, from the origin     *
 **************************************************************/

if (start == -INT_MAX && end == INT_MAX)
  {
  order = -gamgi_phys_plane_order (cell, plane->hkl, 
  plane->vectors, cell->o1, cell->o2, cell->o3);
  static_range (plane, cell, structure,
  thickness, dlist, order, normal, FALSE);

  for (n = order + 1; n < INT_MAX; n++)
    if (static_range (plane, cell, structure, thickness,
    dlist, n, normal, FALSE) == FALSE) break;

  for (n = order - 1; n > -INT_MAX; n--)
    if (static_range (plane, cell, structure, thickness,
    dlist, n, normal, FALSE) == FALSE) break;
  }

/**************************************
 * show start, even if it is outside, *
 * plus all planes inside from start  *
 **************************************/

else if (end == INT_MAX)
  {
  static_range (plane, cell, structure,
  thickness, dlist, start, normal, TRUE);

  for (n = start + 1; n < INT_MAX; n++)
    if (static_range (plane, cell, structure, thickness,
    dlist, n, normal, FALSE) == FALSE) break;
  }

/*************************************
 * show end, even if it is outside,  *
 * plus all planes inside until end  *
 *************************************/

else if (start == -INT_MAX)
  {
  static_range (plane, cell, structure,
  thickness, dlist, end, normal, TRUE);

  for (n = end - 1; n > -INT_MAX; n--)
    if (static_range (plane, cell, structure, thickness,
    dlist, n, normal, FALSE) == FALSE) break;
  }

/*********************************
 * show all planes from start to *
 * end, even if they are outside *
 *********************************/

else
  {
  for (n = start; n <= end; n++)
    static_range (plane, cell, structure,
    thickness, dlist, n, normal, TRUE);
  }
}

void gamgi_phys_plane_projection (gamgi_plane *plane,
gamgi_cell *cell, gamgi_enum pole, gamgi_enum trace)
{
if (pole == TRUE) static_projection (plane, cell, GAMGI_PHYS_POLE);
if (trace == TRUE) static_projection (plane, cell, GAMGI_PHYS_TRACE);
}

void gamgi_phys_plane_name (int h, int k, int l, char *token)
{
sprintf (token, "(%d.%d.%d)", h, k, l);
}
