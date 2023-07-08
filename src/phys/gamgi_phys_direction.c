/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_direction.c
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
#include "gamgi_engine_array.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_mesa_start.h"
#include "gamgi_mesa_rotate_out.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_diophantine.h"
#include "gamgi_math_polygon.h"
#include "gamgi_math_position.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_node.h"
#include "gamgi_chem_atom.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_projection.h"

void static_closer (int *uvw, int o1, int o2, int o3, 
int signal, int *node_old)
{
int node_new[3];
int t_old[3], t_new[3];

node_new[0] = node_old[0];
node_new[1] = node_old[1];
node_new[2] = node_old[2];

t_new[0] = node_new[0] - o1;
t_new[1] = node_new[1] - o2;
t_new[2] = node_new[2] - o3;

do
  {
  node_old[0] = node_new[0];
  node_old[1] = node_new[1];
  node_old[2] = node_new[2];

  t_old[0] = t_new[0];
  t_old[1] = t_new[1];
  t_old[2] = t_new[2];

  node_new[0] = node_old[0] + signal * uvw[0];
  node_new[1] = node_old[1] + signal * uvw[1];
  node_new[2] = node_old[2] + signal * uvw[2];

  t_new[0] = node_new[0] - o1;
  t_new[1] = node_new[1] - o2;
  t_new[2] = node_new[2] - o3;
  }
while (gamgi_phys_direction_higher (t_old, t_new) == TRUE);
}

static void static_extremities (int n_neighbours, gamgi_atom **neighbours,
int *n_min, int *n_max)
{
gamgi_atom *atom, *atom_before;
double x, y, z;
double min, max;
int i, d;

/********************************************************
 * get main axis: the axis where coodinates change more *
 ********************************************************/

x = 0; y = 0; z = 0;
for (i = 1; i < n_neighbours; i++)
  {
  atom_before = neighbours[i - 1];
  atom = neighbours[i];

  x += fabs (atom->position[0] - atom_before->position[0]);
  y += fabs (atom->position[1] - atom_before->position[1]);
  z += fabs (atom->position[2] - atom_before->position[2]);
  }

max = x; d = 0;
if (y > max) { max = y; d = 1; }
if (z > max) { max = z; d = 2; }

/************************************
 * get minimum and maximum points,  *
 * the extremities, along main axis *
 ************************************/

min = DBL_MAX; max = -DBL_MAX;
for (i = 0; i < n_neighbours; i++)
  {
  atom = neighbours[i];

  if (atom->position[d] < min)
    { min = atom->position[d]; *n_min = i; }
  if (atom->position[d] > max)
    { max = atom->position[d]; *n_max = i; }
  }

}

static void static_segment (gamgi_darray *darray)
{
double *points;
double u[3], v[3];
double length, length_max;
int m, n, n_points;
int i, j;

/**************
 * start data *
 **************/

n_points = darray->offset;
points = darray->array;

/*************************************************************
 * get extreme points: the plane should intersect the        *
 * convex polygon in 2 (segment-segment), 3 (segment-vertex) * 
 * or 4 points (vertex-vertex), so the double cycle is ok    *
 *                                                           * 
 * points have 4 coordinates, so they can be easily q-sorted *
 *                                                           *
 * length_max must be initialized to a negative value, as    *
 * the array may contain equal points only, so length can    *
 * be always zero. In this case, the first pair remains and  *
 * the array is eliminated later, after a length check.      *
 *************************************************************/

m = -1; n = -1;
length_max = -DBL_MAX;
for (i = 0; i < n_points - 1; i++)
  {
  for (j = i + 1; j < n_points; j++)
    {
    gamgi_math_vector_sub (points + 4*i, points + 4*j, u);
    length = gamgi_math_vector_length (u);

    if (length > length_max) { length_max = length; m = i; n = j; }
    }
  }

/***********************
 * save extreme points *
 ***********************/

gamgi_math_vector_copy (points + 4 * m, u);
gamgi_math_vector_copy (points + 4 * n, v);

gamgi_math_vector_copy (u, points);
gamgi_math_vector_copy (v, points + 3);

/****************************************************
 * reallocate the array data, now with 3 dimensions *
 ****************************************************/

gamgi_engine_darray_recreate (darray, 2, GAMGI_MATH_PLANE_STEP, 3);
}

gamgi_bool static static_point (gamgi_cell *cell,   
double *r1, double *r2, double *r3)
{
double cross[3], u[3], v[3];
double length;

/******************************************
 * by default, use the cell center as the *
 * third point defining the plane, to     *
 * maximize the section crossing the cell *
 ******************************************/

gamgi_math_cell_center (cell, r3);

gamgi_math_vector_sub (r3, r1, u);
gamgi_math_vector_sub (r3, r2, v);
gamgi_math_vector_cross (u, v, cross);
length = gamgi_math_vector_length (cross);
if (length > GAMGI_MATH_TOLERANCE_LENGTH) return TRUE;

/************************************************************
 * if the cell center is aligned with the other two points, *
 * then the direction passes through the cell center: in    *
 * this case, choosing an arbitrary ortoghonal point        *
 * should be enough to maximize the plane section           *
 ************************************************************/

gamgi_math_vector_orthogonal (r1, r2, r3);

return FALSE;
}

static void static_center (gamgi_direction *direction)
{
/****************************************************
 * a direction is always defined by two points only *
 ****************************************************/

gamgi_math_vector_add (direction->points,
direction->points + 3, direction->center);
gamgi_math_vector_scale (direction->center, direction->center, 0.5);
}

static gamgi_darray *static_line (gamgi_direction *direction,
double thickness, double *position, double *normal, gamgi_dlist *dlist)
{
gamgi_atom *atom;
gamgi_darray *darray;
double point[4];
double r[3];
double length;

/**************************************************
 * store 4-coordinate points: the 4th coordinate  *
 * will be used later to order the polygon points *
 **************************************************/

darray = gamgi_engine_darray_create (GAMGI_MATH_PLANE_SIZE, GAMGI_MATH_PLANE_STEP, 4);

/***********************************************************
 * build direction line with projected points, when atoms  *
 * in common parent are closer than thickness to direction *
 ***********************************************************/

while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, position, point);
  length = gamgi_math_vector_dot (point, normal);
  gamgi_math_vector_scale (normal, r, length);
  gamgi_math_vector_sub (point, r, point);
  length = gamgi_math_vector_length (point);

  if (fabs (length) < thickness)
    {
    gamgi_math_vector_sub (atom->position, point, point);
    gamgi_engine_darray_push (darray, point);
    }

  dlist = dlist->next;
  }

return darray;
}

static void static_copy (gamgi_direction *direction,
double thickness, double *normal, gamgi_dlist *start)
{
gamgi_atom *atom, *atom_new;
gamgi_dlist *dlist;
double r[3], s[3];
double length;

/********************************************
 * copy and link to the direction all atoms *
 * in parent that are closer than thickness *
 ********************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, direction->center, r);
  length = gamgi_math_vector_dot (r, normal);
  gamgi_math_vector_scale (normal, s, length);
  gamgi_math_vector_sub (r, s, r);
  length = gamgi_math_vector_length (r);

  if (fabs (length) < thickness)
    {
    atom_new = gamgi_engine_copy_atom (atom);
    gamgi_engine_link_atom_direction (atom_new, direction);
    }
  }

/********************************
 * clean bonds in parent object *
 ********************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  gamgi_chem_atom_clean (GAMGI_CAST_ATOM dlist->data);
}

static void static_link (gamgi_direction *direction,
double thickness, double *normal, gamgi_dlist *start)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
double r[3], s[3];
double length;

/****************************************************
 * link atoms to direction: this is used in         *
 * atomic directions and crystalographic directions *
 ****************************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  gamgi_math_vector_sub (atom->position, direction->center, r);
  length = gamgi_math_vector_dot (r, normal);
  gamgi_math_vector_scale (normal, s, length);
  gamgi_math_vector_sub (r, s, r);
  length = gamgi_math_vector_length (r);

  if (fabs (length) < thickness)
    {
    gamgi_engine_unlink_atom (atom);
    gamgi_engine_link_atom_direction (atom, direction);
    }
  }
}

static int static_order (int *hkl, gamgi_cell *cell, int step)
{
int order;
gamgi_plane *plane;
gamgi_bool inside;

/******************************************
 * build auxiliary plane to scan the cell *
 ******************************************/

plane = gamgi_engine_create_plane ();
gamgi_engine_start_plane (plane);

plane->hkl[0] = hkl[0];
plane->hkl[1] = hkl[1];
plane->hkl[2] = hkl[2];
plane->vectors = GAMGI_PHYS_PRIMITIVE;

order = 0;
do
  {
  order += step;
  plane->order = order;
  inside = gamgi_phys_plane_cell (plane, cell);

  /*********************************
   * free polygon data and restart *
   * plane, ready for next scan    *
   *********************************/

  free (plane->points);
  plane->points = NULL;
  free (plane->loops);
  plane->loops = NULL;
  plane->n_points = 0;
  plane->n_loops = 0;

  } while (inside == TRUE);

gamgi_engine_remove_plane (plane);

return order - step;
}

static gamgi_bool static_range (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, double thickness, gamgi_dlist *dlist, int *node, int *swap,
double *normal, gamgi_bool link)
{
gamgi_direction *direction_new;
int node_p[3];
gamgi_bool inside;
int uvw[3];

/************************
 * create new direction *
 ************************/

direction_new = gamgi_engine_copy_direction (direction);

if (strcmp (direction->object.name, "") == 0)
  gamgi_phys_direction_name (direction->uvw[0], direction->uvw[1],
  direction->uvw[2], direction_new->object.name);

node_p[swap[0]] = node[0];
node_p[swap[1]] = node[1];
node_p[swap[2]] = node[2];

/******************************************
 * get and set node closer to cell origin *
 ******************************************/

gamgi_phys_direction_closer (cell, direction, node_p);
if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
  {
  direction_new->node[0] = node_p[0]; 
  direction_new->node[1] = node_p[1]; 
  direction_new->node[2] = node_p[2];
  }
if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
  {
  gamgi_math_node_conventional (cell->lattice, node_p[0], node_p[1], 
  node_p[2], &direction_new->node[0], &direction_new->node[1], 
  &direction_new->node[2], &direction_new->node[3]);
  }

gamgi_phys_direction_primitive (cell->lattice, direction->uvw, uvw);

/***********************
 * build new direction *
 ***********************/

inside = gamgi_phys_direction_cell (direction_new, cell);

/***********************************
 * link options: link diretcion to *
 * cell, link atoms to direction   *
 ***********************************/

if (inside == TRUE || link == TRUE)
  {
  gamgi_engine_link_direction_cell (direction_new, cell);
  if (structure == GAMGI_CHEM_STRUCTURE_COPY)
    static_copy (direction_new, thickness, normal, dlist);
  if (structure == GAMGI_CHEM_STRUCTURE_LINK)
    static_link (direction_new, thickness, normal, dlist);
  }
else gamgi_engine_remove_direction (direction_new);

/********************************
 * return flag informing if the *
 * direction is inside the cell *
 ********************************/

return inside;
}

static void static_projection (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum model)
{
gamgi_direction *direction_new;

/******************************************************************
 * Direction Create: direction is just a template, removed in the *
 * end, used to copy data to direction_new, the real direction    *
 *                                                                *
 * Direction Modify: direction is the real direction to modify    *
 *                                                                *
 * direction->object.object is used here as a flag to distinguish *
 * between the two cases. direction->object.object can be used    *
 * because direction is always unlinked at this stage             *
 ******************************************************************/

if (direction->object.object == NULL)
  direction_new = gamgi_engine_copy_direction (direction);
else
  { direction_new = direction; direction->object.object = NULL; }

if (strcmp (direction->object.name, "") == 0)
  gamgi_phys_direction_name (direction->uvw[0], direction->uvw[1],
  direction->uvw[2], direction_new->object.name);
direction_new->model = model;

gamgi_phys_projection_create (direction_new->model, &direction_new->points,
&direction_new->n_points, &direction_new->loops, &direction_new->n_loops);

gamgi_engine_link_direction_cell (direction_new, cell);
gamgi_mesa_rotate_out_direction (direction_new, NULL, NULL, NULL);
}

gamgi_bool gamgi_phys_direction_zero (int *a)
{
/****************************************
 * at least one indice must be non-zero *
 ****************************************/

if (a[0] == 0 && a[1] == 0 && a[2] == 0) return TRUE;

return FALSE;
}

gamgi_bool gamgi_phys_direction_higher (int *a, int *b)
{
/***********************************************
 * a,b represent two sets of hkl plane indices *
 ***********************************************/

if (a[0]*a[0] + a[1]*a[1] + a[2]*a[2] > b[0]*b[0] + b[1]*b[1] + b[2]*b[2])
  return TRUE;

return FALSE;
}

gamgi_bool gamgi_phys_direction_weiss (int *a, int *b)
{
/*************************************************************
 * a,b represent two sets of hkl/uvw plane/direction indices *
 *                                                           *
 * Weiss zone law: if plane (hkl) and direction [uvw] are    *
 * perpendicular then hu+kv+lw = 0, for all lattices         *
 *************************************************************/

if (a[0]*b[0] + a[1]*b[1] + a[2]*b[2] == 0) return TRUE;

return FALSE;
}

gamgi_bool gamgi_phys_direction_decrease (int *uvw, int *uvw_min)
{
int i, min;

/*********************************
 * get smaller, non-zero, indice *
 *********************************/

min = INT_MAX;
if (uvw[0] != 0 && abs (uvw[0]) < min) min = abs (uvw[0]);
if (uvw[1] != 0 && abs (uvw[1]) < min) min = abs (uvw[1]);
if (uvw[2] != 0 && abs (uvw[2]) < min) min = abs (uvw[2]);

/****************************************************
 * get maximum common divisor of all three indices, *
 * trying from the smaller, non-zero, indice to 1   *
 ****************************************************/

for (i = min; i > 1; i--)
  if (uvw[0] % i == 0 && uvw[1] % i == 0 && uvw[2] % i == 0) break;

/****************************************************
 * get the smaller indices that are proportional to *
 * the original ones (they are the same, if i = 1)  *
 ****************************************************/

uvw_min[0] = uvw[0] / i;
uvw_min[1] = uvw[1] / i;
uvw_min[2] = uvw[2] / i;

if (i == 1) return FALSE;

return TRUE;
}

void gamgi_phys_direction_conventional (gamgi_enum lattice,
int *uvw_p, int *uvw_c)
{
int node[3];

/*******************************************************
 * First get a direction node with integer coordinates *
 * in terms of conventional vectors, then convert to   *
 * conventional node, then minimize if necessary       *
 *******************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I: case GAMGI_PHYS_CUBIC_F: 
  case GAMGI_PHYS_ORTHORHOMBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_C: 
  case GAMGI_PHYS_MONOCLINIC_C:
  node[0] = 2 * uvw_p[0];
  node[1] = 2 * uvw_p[1];
  node[2] = 2 * uvw_p[2];
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  node[0] = 3 * uvw_p[0];
  node[1] = 3 * uvw_p[1];
  node[2] = 3 * uvw_p[2];
  break;

  default:
  node[0] = uvw_p[0];
  node[1] = uvw_p[1];
  node[2] = uvw_p[2];
  break;
  }

gamgi_math_node_conventional (lattice, node[0], node[1],
node[2], &uvw_c[0], &uvw_c[1], &uvw_c[2], &uvw_c[3]);

gamgi_phys_direction_decrease (uvw_c, uvw_c);
}

void gamgi_phys_direction_primitive (gamgi_enum lattice,
int *uvw_c, int *uvw_p)
{
int node[4];

/************************************************************
 * First get the primitive node, than make sure it is       *
 * the nearest node to the origin. Example: in a F lattice, *
 * conventional direction [110] is converted in primitive   *
 * direction [200] that is then decreased to [100].         *
 ************************************************************/

node[0] = uvw_c[0];
node[1] = uvw_c[1];
node[2] = uvw_c[2];
node[3] = 0;

gamgi_math_node_primitive (lattice, node[0], node[1], 
node[2], node[3], &uvw_p[0], &uvw_p[1], &uvw_p[2]);

gamgi_phys_direction_decrease (uvw_p, uvw_p);
}

void gamgi_phys_direction_closer (gamgi_cell *cell,
gamgi_direction *direction, int *node)
{
int uvw[3];

/***********************************************************
 * get uvw direction indices in terms of primitive vectors *
 *                                                         *
 * the initialization is only to avoid a compiler warning  *
 ***********************************************************/

uvw[0] = uvw[1] = uvw[2] = 0;
if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
  {
  uvw[0] = direction->uvw[0];
  uvw[1] = direction->uvw[1];
  uvw[2] = direction->uvw[2];
  }
if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
  {
  gamgi_phys_direction_primitive (cell->lattice, direction->uvw, uvw); 
  }

/*************************************
 * get direction node closer to cell *
 * origin, using primitive vectors:  *
 *   1) try along the - direction    *
 *   2) try along the + direction    *
 *************************************/

static_closer (uvw, cell->o1, cell->o2, cell->o3, -1, node);
static_closer (uvw, cell->o1, cell->o2, cell->o3, +1, node);
}

void gamgi_phys_direction_vector (gamgi_cell *cell,
int *uvw, gamgi_enum vectors, double *vector)
{
/********************************************************
 * get translation vector in absolute x,y,z coordinates *
 ********************************************************/

if (vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_vector_translation (uvw[0], uvw[1], uvw[2],
  cell->a1, cell->a2, cell->a3, vector);
if (vectors == GAMGI_PHYS_PRIMITIVE)
  gamgi_math_vector_translation (uvw[0], uvw[1], uvw[2],
  cell->p1, cell->p2, cell->p3, vector);
}

void gamgi_phys_direction_points (gamgi_direction *direction,
double *r1, double *r2)
{
double rotate[9];

/******************************************
 * get direction->quaternion to rotate    *
 * and direction->origin to translate     *
 * the two points defining the direction, *
 * initially in direction coordinates     *
 ******************************************/

gamgi_math_quaternion_to_matrix (direction->quaternion, rotate);

gamgi_math_matrix_vector (rotate, direction->points, r1);
gamgi_math_vector_add (r1, direction->origin, r1);

gamgi_math_matrix_vector (rotate, direction->points + 3, r2);
gamgi_math_vector_add (r2, direction->origin, r2);
}

void gamgi_phys_direction_default (double *p1, double *p2, 
gamgi_bool *inside, gamgi_darray *darray)
{
gamgi_engine_darray_pop (darray, 0);
gamgi_engine_darray_push (darray, p1);
gamgi_engine_darray_push (darray, p2);

*inside = FALSE;
}

gamgi_bool gamgi_phys_direction_cell (gamgi_direction *direction, 
gamgi_cell *cell)
{
gamgi_cell *cell_new;
gamgi_darray *darray0, *darray1;
double rotate[9];
double r1[3], r2[3], r3[3];
double normal[3], origin[3], segment[3];
double position[3];
double translate[3];
double area, length;
int *loops;
int n_loops;
int o1, o2, o3;
gamgi_bool inside = TRUE;

/****************************
 * direction representation *
 ****************************/

direction->model = GAMGI_PHYS_LINE;

/********************************************
 * get 2 points describing the direction:   *
 * we assume that they are far enough from  *
 * each other to represent a real direction *
 ********************************************/

gamgi_math_vector_zero (r1);
gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, r2);

gamgi_math_cell_offset (cell, origin);
gamgi_math_vector_add (r1, origin, r1);
gamgi_math_vector_add (r2, origin, r2);

/*****************************************************
 * get direction node o1,o2,o3 primitive coordinates *
 *****************************************************/

if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
  { o1 = direction->node[0]; o2 = direction->node[1]; o3 = direction->node[2]; }
if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice, direction->node[0], 
  direction->node[1], direction->node[2], direction->node[3], &o1, &o2, &o3);

/***********************************************
 * Translate direction points so the direction *
 * passes through primitive node o1,o2,o3. By  *
 * default, directions pass through node 000.  *
 ***********************************************/

gamgi_math_vector_translation (o1, o2, o3, 
cell->p1, cell->p2, cell->p3, translate);
gamgi_math_vector_add (r1, translate, r1);
gamgi_math_vector_add (r2, translate, r2);

if (cell->model == GAMGI_PHYS_SPHERE)
  {
  /************************************************
   * get an array of points describing the        *
   * direction intersection with the cell sphere  *
   *                                              *
   * build default segment line when the          *
   * direction does not intersect the cell volume *
   ************************************************/

  darray1 = gamgi_math_polygon_diameter (cell, direction, r1, r2);
  if (darray1->offset < 2)
    gamgi_phys_direction_default (r1, r2, &inside, darray1);
  }
else
  {
  /*******************************************************
   * build a plane defined by direction points r1,r2     *
   * plus a third point r3, chosen in order to maximize  *
   * the section inside the cell that is crossed by the  *
   * plane, to avoid chosing planes tangent to the cell, *
   * which may result in directions inside recognized    *
   * as outside the cell, due to numerical errors        *
   *******************************************************/

  static_point (cell, r1, r2, r3);

  /*******************************************
   * get an array of points describing the   *
   * plane intersection with the cell loops  *
   *                                         *
   *  build a new equivalent cell with all   *
   *  the loops needed to test intersection  *
   *                                         *
   * orientate polygon counterclockwise when *
   * looking with origin on the other side   *
   *                                         *
   *    expand polygon to make it convex     *
   *                                         *
   * build a new plane, normal to the first  *
   * one, containing the direction           *
   *                                         *
   * get an array of points describing the   *
   * plane intersection with the polygon     *
   *                                         *
   *    determine the two farthest points    *
   *******************************************/

  cell_new = gamgi_phys_cell_polygons (cell);
  darray0 = gamgi_math_polygon_scan (cell_new->loops, cell_new->points, r1, r2, r3);
  gamgi_engine_remove_cell (cell_new);

  /*******************************************
   * build default line when the direction   *
   * line does not intersect the cell volume *
   *******************************************/

  if (darray0->offset < 2)
    gamgi_phys_direction_default (r1, r2, &inside, darray0);

  /**********************************************
   *           get normal vector                *
   *                                            *
   * orientate polygon counterclockwise when    *
   * looking with origin on the other side: the *
   * normal vector points away from the origin  *
   *                                            *
   *     expand polygon to make it convex       *
   **********************************************/

  gamgi_math_polygon_normal (r1, r2, r3, normal);
  gamgi_math_polygon_orientate (darray0, normal);
  gamgi_math_polygon_convex (darray0, normal);

  /********************************************
   * build default polygon when the direction *
   * plane barely intersects the cell volume  *
   ********************************************/

  area = gamgi_math_polygon_area (darray0->offset, darray0->array);
  if (area < GAMGI_MATH_DIRECTION_AREA)
    gamgi_phys_plane_default (r1, r2, r3, &inside, darray0);

  /**********************************************
   * build a new plane, normal to the first one *
   **********************************************/

  gamgi_math_vector_add (r1, normal, r3);

  gamgi_math_polygon_loops (darray0->offset, &loops, &n_loops);
  darray1 = gamgi_math_polygon_scan (loops, darray0->array, r1, r2, r3);
  gamgi_engine_darray_remove (darray0);
  free (loops);

  /**********************************************
   * build default direction when the direction *
   * plane does not intersect the cell polygon  *
   **********************************************/

  if (darray1->offset < 2)
    gamgi_phys_direction_default (r1, r2, &inside, darray1);

  /**********************************************
   * build default direction when the direction *
   * plane barely intersects the cell polygon   *
   **********************************************/

  static_segment (darray1);
  gamgi_math_vector_sub (darray1->array + 3, darray1->array, segment);
  length = gamgi_math_vector_length (segment);
  if (length < GAMGI_MATH_DIRECTION_LENGTH)
    gamgi_phys_direction_default (r1, r2, &inside, darray1);
  }

/***************************************************
 * set points,loops arrays and remove array object *
 ***************************************************/

gamgi_engine_darray_copy_remove (darray1, &direction->points, &direction->n_points);
gamgi_math_polygon_lines (direction->n_points, &direction->loops, &direction->n_loops);

/********************************************
 * set direction center, origin, quaternion *
 ********************************************/

static_center (direction);
gamgi_math_position_rotation (GAMGI_CAST_OBJECT cell, rotate);
gamgi_math_position_translation (GAMGI_CAST_OBJECT cell, translate);
gamgi_math_matrix_vector (rotate, direction->center, position);
gamgi_math_vector_add (position, translate, direction->center);

gamgi_math_vector_copy (cell->origin, direction->origin);
direction->quaternion[0] = cell->quaternion[0];
direction->quaternion[1] = cell->quaternion[1];
direction->quaternion[2] = cell->quaternion[2];
direction->quaternion[3] = cell->quaternion[3];

/********************************************************
 * return FALSE if direction is outside the cell volume *
 ********************************************************/

return inside;
}

gamgi_object *gamgi_phys_direction_parent (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_window *window)
{
gamgi_object *parent;

/********************************************************
 * return first parent common to the two defining atoms *
 ********************************************************/

parent = gamgi_engine_find_root (GAMGI_CAST_OBJECT atom1,
GAMGI_CAST_OBJECT atom2, GAMGI_CAST_OBJECT window->layer);

return parent;
}

void gamgi_phys_direction_line (gamgi_direction *direction, double *points)
{
int i;

/********************************
 * build points array: 2 points *
 ********************************/

direction->points = (double *) malloc (sizeof (double) * 6);
for (i = 0; i < 6; i++) direction->points[i] = points[i];
direction->n_points = 2;

/*****************************
 * build loops array: 1 line *
 *****************************/

direction->loops = (int *) malloc (sizeof (int) * 3);
direction->loops[0] = 1;
direction->loops[1] = 0;
direction->loops[2] = 1;

direction->n_loops = 3;
}

gamgi_bool gamgi_phys_direction_reference (gamgi_direction *direction,
gamgi_atom **atom1, gamgi_atom **atom2)
{
gamgi_dlist *dlist = direction->atom_start;

*atom1 = NULL;
*atom2 = NULL;

if (dlist != NULL)
  {
  *atom1 = GAMGI_CAST_ATOM dlist->data;
  dlist = dlist->next;
  if (dlist != NULL)
    {
    *atom2 = GAMGI_CAST_ATOM dlist->data;
    return TRUE;
    }
  }

return FALSE;
}

gamgi_bool gamgi_phys_direction_length (double *begin, double *end)
{
double u[3];
double length;

gamgi_math_vector_sub (begin, end, u);
length = gamgi_math_vector_length (u);
if (length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

return TRUE;
}

gamgi_dlist *gamgi_phys_direction_reorder (gamgi_dlist *start,
gamgi_atom *atom1, gamgi_atom *atom2)
{
gamgi_atom *atom;
gamgi_dlist *dlist;
gamgi_bool move = FALSE;

/*************************************
 * reorder dlist so the two defining *
 * atoms are always in the beginning *
 *************************************/

dlist = start;
while (dlist != NULL)
  {
  atom = GAMGI_CAST_ATOM dlist->data;

  if (atom == atom1 || atom == atom2)
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

void gamgi_phys_direction_atoms (gamgi_direction *direction, gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_dlist *dlist, gamgi_enum structure, double thickness,
gamgi_bool copy)
{
gamgi_darray *darray;
double unit[9], rotation[9];
double zero[3], normal[3], translation[3];

/****************************
 * direction representation *
 ****************************/

direction->model = GAMGI_PHYS_LINE;

/**********************************************************************
 * initially the parent object is rotated and translated backward so  *
 * the direction is built with zero translation and rotation. In the  *
 * end, the parent object is rotated and translated forward to its    *
 * initial position, this time already including the direction, which *
 * will acquire the same origin and rotation as the parent object.    *
 **********************************************************************/

gamgi_math_position_rotation (direction->object.object, rotation);
gamgi_math_position_translation (direction->object.object, translation);

gamgi_math_vector_zero (zero);
gamgi_math_matrix_unit (unit);
gamgi_math_position_object_rotation (direction->object.object, unit);
gamgi_math_position_object_translation (direction->object.object, zero);

/***********************************************
 *  get direction vector: we tested before if  *
 *  the two atoms can define a real direction  *
 *                                             *
 * use atom1->position as a point in the line  *
 *                                             *
 *      get darray with all line points        *
 *                                             *
 *     keep only the two extreme points        *
 ***********************************************/

gamgi_math_vector_sub (atom2->position, atom1->position, normal);
gamgi_math_vector_normal (normal);

darray = static_line (direction, thickness, atom1->position, normal, dlist);
static_segment (darray);

/***************************
 * save points array data  *
 * and remove array object *
 *                         *
 *   create loops array    *
 ***************************/

gamgi_engine_darray_copy_remove (darray, &direction->points, &direction->n_points);
gamgi_math_polygon_lines (direction->n_points, &direction->loops, &direction->n_loops);
static_center (direction);

/***********************************
 * link or copy atoms to direction *
 ***********************************/

if (structure == GAMGI_CHEM_STRUCTURE_COPY)
  static_copy (direction, thickness, normal, dlist);
if (structure == GAMGI_CHEM_STRUCTURE_LINK)
  static_link (direction, thickness, normal, dlist);

/*******************************************
 * rotate the parent object, including     *
 * the direction, to its original position *
 *******************************************/

gamgi_math_position_object_rotation (direction->object.object, rotation);
gamgi_math_position_object_translation (direction->object.object, translation);

/**********************
 * free list of atoms *
 **********************/

while (dlist != NULL)
  dlist = gamgi_engine_dlist_remove_start (dlist);
}

void gamgi_phys_direction_polytope (int n_neighbours, gamgi_atom **neighbours,
gamgi_enum structure, float red, float green, float blue, gamgi_object *parent,
gamgi_atom *central)
{
gamgi_direction *direction;
gamgi_atom *atom;
double unit[9], rotation[9];
double zero[3], translation[3];
int i, min, max;

/********************************************************
 * link direction and set type before handling rotation *
 ********************************************************/

direction = gamgi_engine_create_direction ();
gamgi_engine_start_direction (direction);
gamgi_engine_link_object_object (GAMGI_CAST_OBJECT direction, parent);
strcpy (direction->object.name, "Direction");

direction->reference = GAMGI_ENGINE_ATOM;
direction->model = GAMGI_PHYS_LINE;

direction->red = red;
direction->green = green;
direction->blue = blue;

/**********************************************************************
 * initially the parent object is rotated and translated backward so  *
 * the direction is built with zero translation and rotation. In the  *
 * end, the parent object is rotated and translated forward to its    *
 * initial position, this time already including the direction, which *
 * will acquire the same origin and rotation as the parent object.    *
 **********************************************************************/

gamgi_math_vector_zero (zero);
gamgi_math_matrix_unit (unit);

gamgi_math_position_direction_rotation_set (direction, zero);
gamgi_math_position_direction_translation_set (direction, zero);

gamgi_math_position_rotation (parent, rotation);
gamgi_math_position_translation (parent, translation);

gamgi_math_position_object_rotation (parent, unit);
gamgi_math_position_object_translation (parent, zero);

/***************************************************
 * this is not necessary but prevents a O3 warning *
 ***************************************************/

min = -1; max = -1;

/************************************
 * get atoms defining segment with  *
 * maximum length (the extremities) *
 ************************************/

static_extremities (n_neighbours, neighbours, &min, &max);

direction->n_points = 2;
direction->points = (double *) malloc (3 * direction->n_points * sizeof (double));

atom = neighbours[min];
gamgi_math_vector_copy (atom->position, direction->points);
atom = neighbours[max];
gamgi_math_vector_copy (atom->position, direction->points + 3);

gamgi_math_polygon_lines (direction->n_points, &direction->loops, 
&direction->n_loops);
static_center (direction);

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
  gamgi_engine_link_atom_direction (atom, direction);
  for (i = 0; i < n_neighbours; i++)
    {
    atom = gamgi_engine_copy_atom (neighbours[i]);
    gamgi_engine_link_atom_direction (atom, direction);
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
  gamgi_engine_link_atom_direction (central, direction);
  for (i = 0; i < n_neighbours; i++)
    {
    gamgi_engine_unlink_atom (neighbours[i]);
    gamgi_engine_link_atom_direction (neighbours[i], direction);
    }
  }

/************************************************
 * copy all neighbours from parent to direction *
 ************************************************/

atom = gamgi_engine_copy_atom (central);
gamgi_engine_link_atom_direction (atom, direction);

for (i = 0; i < n_neighbours; i++)
  {
  atom = gamgi_engine_copy_atom (neighbours[i]);
  gamgi_engine_link_atom_direction (atom, direction);
  }

/*******************************************
 * rotate the parent object, including     *
 * the direction, to its original position *
 *******************************************/

gamgi_math_position_object_rotation (direction->object.object, rotation);
gamgi_math_position_object_translation (direction->object.object, translation);
}

void gamgi_phys_direction_range (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness, gamgi_dlist *dlist,
int *hkl1, int start1, int end1, int *hkl2, int start2, int end2)
{
double rotate[9];
double vector[3], normal[3];
int swap[3];
int hkl1_p[3], hkl2_p[3];
int hkl1_s[3], hkl2_s[3];
int node[3];
int i, j;
gamgi_bool link;

/*****************************************************
 * link atoms to direction: 1) get direction normal, *
 * rotated according to the cell orientation         *
 * 2) get list of atoms belonging to the cell        *
 *****************************************************/

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, vector);
gamgi_math_matrix_vector (rotate, vector, normal);
gamgi_math_vector_normal (normal);

/*******************************************
 * convert plane indices to primitive axes *
 *******************************************/

if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
  {
  hkl1_p[0] = hkl1[0]; hkl1_p[1] = hkl1[1]; hkl1_p[2] = hkl1[2];
  hkl2_p[0] = hkl2[0]; hkl2_p[1] = hkl2[1]; hkl2_p[2] = hkl2[2];
  }
if (direction->vectors == GAMGI_PHYS_CONVENTIONAL)
  {
  gamgi_phys_plane_primitive (cell->lattice, hkl1, hkl1_p);
  gamgi_phys_plane_primitive (cell->lattice, hkl2, hkl2_p);
  }

/************************************
 * get all the boundary planes that *
 * are not explicitely indicated    *
 ************************************/

link = TRUE;
if (start1 == -INT_MAX)
  { link = FALSE; start1 = static_order (hkl1_p, cell, -1); }
if (end1 == INT_MAX)
  { link = FALSE; end1 = static_order (hkl1_p, cell, +1); }
if (start2 == -INT_MAX)
  { link = FALSE; start2 = static_order (hkl2_p, cell, -1); }
if (end2 == INT_MAX)
  { link = FALSE; end2 = static_order (hkl2_p, cell, +1); }

/***********************************************************
 * this equation relates a plane (h k l) with order        *
 * m with all the nodes (a,b,c) belonging to that plane:   *
 *                                                         *
 *              h.a + k.b + l.c = m                        *
 *                                                         *
 * To get this equation, consider the dot product of a     *
 * generic translation vector (describing a generic node   *
 * (a,b,c) with a normal vector describing the plane       *
 * (h k l) given by its reciprocal vector G divided by     *
 * its modulus |G|. This must be equal to the order of     *
 * the plane where the node lives, times the difference    *
 * between adjacent planes, which is 1/|G|, thus leading   *
 * to the equation above.                                  *
 *                                                         *
 * Two intersecting planes have nodes in the intersection  *
 * only if the system defined by both planes has valid     *
 * solutions for (a,b,c) node coordinates. If there is     *
 * one solution then there are infinite of them.           *
 *                                                         *
 *               h1.a + k1.b + l1.c = m                    *
 *               h2.a + k2.b + l2.c = n                    *
 *                                                         *
 * to solve the diophantine linear system of two equations *
 * is useful to swap first the system coeffecients         *
 ***********************************************************/

gamgi_math_diophantine_swap (hkl1_p, hkl2_p, hkl1_s, hkl2_s, swap);

for (i = start1; i <= end1; i++)
  {
  for (j = start2; j <= end2; j++)
    {
    if (gamgi_math_diophantine_intersect (hkl1_s, hkl2_s, i, j, node) == TRUE)
      static_range (direction, cell, structure,
      thickness, dlist, node, swap, normal, link);
    }
  }
}

void gamgi_phys_direction_node (gamgi_direction *direction, gamgi_cell *cell,
gamgi_enum structure, gamgi_enum filter, double thickness, gamgi_dlist *dlist)
{
gamgi_direction *direction_new;
double rotate[9];
double vector[3], normal[3];

/******************************************************************
 * Direction Create: direction is just a template, removed in the *
 * end, used to copy data to direction_new, the real direction    *
 *                                                                *
 * Direction Modify: direction is the real direction to modify    *
 *                                                                *
 * direction->object.object is used here as a flag to distinguish *
 * between the two cases. direction->object.object can be used    *
 * because direction is always unlinked at this stage             *
 ******************************************************************/

if (direction->object.object == NULL)
  direction_new = gamgi_engine_copy_direction (direction);
else
  { direction_new = direction; direction->object.object = NULL; }

if (strcmp (direction->object.name, "") == 0)
  gamgi_phys_direction_name (direction->uvw[0], direction->uvw[1],
  direction->uvw[2], direction_new->object.name);

/*********************************************
 * 1) build new direction and link to cell   *
 * 2) get normalized direction vector,       *
 * rotated according to the cell orientation *
 *********************************************/

gamgi_phys_direction_cell (direction_new, cell);
gamgi_engine_link_direction_cell (direction_new, cell);

gamgi_math_quaternion_to_matrix (cell->quaternion, rotate);
gamgi_phys_direction_vector (cell, direction_new->uvw, direction_new->vectors, vector);
gamgi_math_matrix_vector (rotate, vector, normal);
gamgi_math_vector_normal (normal);

/*************************************************
 * 1) get list of atoms belonging to the cell;   *
 * 2) apply structure policy; 3) free atom list. *
 *************************************************/

if (structure == GAMGI_CHEM_STRUCTURE_COPY)
  static_copy (direction_new, thickness, normal, dlist);
if (structure == GAMGI_CHEM_STRUCTURE_LINK)
  static_link (direction_new, thickness, normal, dlist);
}

void gamgi_phys_direction_projection (gamgi_direction *direction,
gamgi_cell *cell, gamgi_enum pole, gamgi_enum trace)
{
if (pole == TRUE) static_projection (direction, cell, GAMGI_PHYS_POLE);
if (trace == TRUE) static_projection (direction, cell, GAMGI_PHYS_TRACE);
}

void gamgi_phys_direction_name (int u, int v, int w, char *token)
{
sprintf (token, "[%d.%d.%d]", u, v, w);
}
