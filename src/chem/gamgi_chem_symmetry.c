/************************************************************************* 
 *                                                                        
 * $GAMGI/src/chem/gamgi_chem_symmetry.c                                  
 *                                                                        
 *  Point group symmetry analyzer.                                        
 *                                                                        
 *  Copyright (C) 1996, 2003 Serguei Patchkovskii                         
 *  Serguei.Patchkovskii@nrc.ca                                           
 *                                                                        
 *  This program is free software; you can redistribute it and/or modify  
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 2 of the License, or     
 *  (at your option) any later version.                                   
 *                                                                        
 *  This program is distributed in the hope that it will be useful,       
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *  GNU General Public License for more details.                          
 *                                                                        
 *  You should have received a copy of the GNU General Public License     
 *  along with this program; if not, write to the Free Software           
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             
 *                                                                        
 */

/*********************************************
 * Code taken from the program Symmetry 1.16 *
 *                         by Carlos Pereira *
 *********************************************/

#include "gamgi_engine.h"
#include "gamgi_chem.h"
#include "gamgi_math.h"
#include "gamgi_gtk.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

#define GAMGI_CHEM_STEP_MIN 1e-7
#define GAMGI_CHEM_STEP_MAX 5e-1
#define GAMGI_CHEM_STEP_GRADIENT 1e-7
#define GAMGI_CHEM_STEP_THRESHOLD 1e-10
#define GAMGI_CHEM_STEP_CYCLES 200
#define GAMGI_CHEM_STEP_HITS 5
#define GAMGI_CHEM_FORCE_MIN 1e-3
#define GAMGI_CHEM_FORCE_MAX 1e3

/**************************************************************************
 * Planes are characterized by the surface normal direction (taken in     *
 * the direction from the coordinate origin) and distance from the        *
 * coordinate origin to the plane in the direction of the surface normal. *
 * Inversion is characterized by location of the inversion center.        *
 *                                                                        *
 * Rotation is characterized by a vector (distance+direction) from        *
 * the origin to the rotation axis, axis direction and rotation order.    *
 * Rotations are in the clockwise direction looking opposite to the       *
 * direction of the axis. Note that this definition of the rotation       *
 * axis is not unique, since an arbitrary multiple of the axis direction  *
 * can be added to the position vector without changing actual operation. *
 *                                                                        *
 * Mirror rotation is defined by the same parameters as normal rotation,  *
 * but the origin is now unambiguous since it defines the position of     *
 * the plane associated with the axis.                                    *
 *                                                                        *
 * function: symmetry transformation function                             *
 * table: symmetry transformation table                                   *
 * order: number transformations to get identity                          *
 * parameters: 4 for inversion and planes, 7 for axes                     *
 * error: largest error associated with the element                       *
 *                                                                        *
 * The order of distance, normal, direction in the element structure      *
 * cannot be changed because of static_set, static_get memory functions.  *
 **************************************************************************/

typedef struct _gamgi_symmetry {

/********************
 * atomic structure *
 ********************/

int n_atoms;
gamgi_atom **atoms;
gamgi_dlist *dlist;
double center[3];
double *distance;

/**************
 * input data *
 **************/

gamgi_bool periodic;
float initial;
float final;
GtkWidget *text;

/*****************
 * symmetry data *
 *****************/

int n_i;
struct _gamgi_element **i;
int n_m;
struct _gamgi_element **m;
int n_C;
struct _gamgi_element **C;
int n_S;
struct _gamgi_element **S;

struct _gamgi_element *plane;

int *sum_C;
int *sum_S; } gamgi_symmetry;

typedef struct _gamgi_element {
void (*function) (gamgi_symmetry *symmetry,
struct _gamgi_element *element, gamgi_atom *from, gamgi_atom *to);
int *table;
int order;
int parameters;
double error;
double distance;
double normal[3];
double direction[3]; } gamgi_element;

static void static_group (char *group, gamgi_symmetry *symmetry);

/************************************************************
 * this table shows how to convert the most common chemical * 
 * points groups (n <= 8) to the 32 crystallographic groups *
 *                                                          *
 * Crystal    <-   Chemical  Elements                       *
 *                  C1                                      *
 *                  Cs        m                             *
 *                  Ci        i                             *
 *                  C2        C2                            *
 *                  C3        C3                            *
 *                  C4        C4 C2                         *
 *   C1       -     C5        C5                            *
 *                  C6        C6 C3 C2                      *
 *   C1       -     C7        C7                            *
 *   C4       -     C8        C8 C4 C2                      *
 *                  D2        3C2                           *
 *                  D3        C3 3C2                        *
 *                  D4        C4 5C2                        *
 *   C2       -     D5        C5 5C2                        *
 *                  D6        C6 C3 7C2                     *
 *   C2       -     D7        C7 7C2                        *
 *   D4       -     D8        C8 C4 9C2                     *
 *                  C2v       C2 2m                         *
 *                  C3v       C3 3m                         *
 *                  C4v       C4 C2 4m                      *
 *   Cs       -     C5v       C5 5m                         *
 *                  C6v       C6 C3 C2 6m                   *
 *   Cs       -     C7v       C7 7m                         *
 *   C4v      -     C8v       C8 C4 C2 8m                   *
 *                  C2h       i C2 m                        *
 *                  C3h       C3 S3 m                       *
 *                  C4h       i C4 C2 S4 m                  *
 *   Cs       -     C5h       C5 S5 m                       *
 *                  C6h       i C6 C3 C2 S6 S3 m            *
 *   Cs       -     C7h       C7 S7 m                       *
 *   C4h      -     C8h       i C8 C4 C2 S8 S4 m            *
 *                  D2h       i 3C2 3m                      *
 *                  D3h       C3 3C2 S3 4m                  *
 *                  D4h       i C4 5C2 S4 5m                *
 *   C2v      -     D5h       C5 5C2 S5 6m                  *
 *                  D6h       i C6 C3 7C2 S6 S3 7m          *
 *   C2v      -     D7h       C7 7C2 S7 8m                  *
 *   D4h      -     D8h       i C8 C4 9C2 S8 S4 9m          *
 *                  D2d       3C2 S4 2m                     *
 *                  D3d       i C3 3C2 S6 3m                *
 * D4-C4v     -     D4d       C4 5C2 S8 4m                  *
 *   C2h            D5d       i C5 5C2 S10 5m               *
 * D6-C6v-S4  -     D6d       C6 C3 7C2 S12 S4 6m           *
 *   C2h   -        D7d       i C7 7C2 S14 7m               *
 * D4-C4v     -     D8d       C8 C4 9C2 S16 8m              *
 *                  S4        C2 S4                         *
 *                  S6        i C3 S6                       *
 *   C4       -     S8        C4 C2 S8                      *
 *                  T         4C3 3C2                       *
 *                  Th        i 4C3 3C2 4S6 3m              *
 *                  Td        4C3 3C2 3S4 6m                *
 *                  O         3C4 4C3 9C2                   *
 *                  Oh        i 3C4 4C3 9C2 4S6 3S4 9m      *
 * C6v-C4v    +     C0v       C0 m                          *
 * D6h-D4h    +     D0h       i C0 C2 2m S0                 *
 *   T        -     I         6C5 10C3 15C2                 *
 *   Th       -     Ih        i 6C5 10C3 15C2 6S10 10S6 15m *
 * D6h-D4h-Oh +     Kh        i C0 m S0                     *
 ************************************************************/

static double pow2 (double x)
{
return x * x;
}

static int static_divisors (int number)
{
int n, sum = 1;

n = number;
while (--n > 1) if (number % n == 0) sum++;
  
return sum;
}

static int static_redundant (int n, int p)
{
int k;

for (k = 1; k < p; k++)
  if (p % k == 0 && n * k % p == 0) return TRUE;

return FALSE;
}

static int static_multiplicity (int n) 
{
int p, sum; 
 
if (n == 2) return 1;

sum = 2;
for (p = 2; 2*p < n; p++)
  if (static_redundant (n, p) == FALSE) sum += 2;
 
return sum;
}

/****************************
 * transformation functions *
 ****************************/

static void static_invert (gamgi_symmetry *symmetry,
gamgi_element *center, gamgi_atom *from, gamgi_atom *to)
{
int i;

to->element = from->element;

/**************************************************
 * center->distance * center->normal[i] gives the *
 * inversion point, minus from->position[i] gives *
 * the vector pointing from the symmetry point to *
 * the center, summing again center->distance x   *
 * center->normal[i] gives the transformed point. *
 **************************************************/

for (i = 0; i < 3; i++)
  to->position[i] = 2 * center->distance *
  center->normal[i] - from->position[i];
}

static void static_reflect (gamgi_symmetry *symmetry, 
gamgi_element *plane, gamgi_atom *from, gamgi_atom *to)
{
int i;
double r;

to->element = from->element;

/******************************************************
 * The initial >r< gives the distance from the origin *
 * to the plane, normal to the plane. The dot product *
 * from->position[i] * plane->normal[i] gives the     *
 * distance from the origin to the point, normal to   *
 * the plane. The difference, the final >r<, gives    *
 * the distance from the plane to the point, normal   *
 * to the plane, which can be positive (point closer  *
 * to the origin than the plane) or negative (point   *
 * farther from the origin than the plane).           *
 ******************************************************/

r = plane->distance;
for (i = 0; i < 3; i++)
  r -= from->position[i] * plane->normal[i];

/*****************************************************
 * add >2*r< to the point, normal to the plane, so   *
 * the final point is on the other side of the plane *
 *****************************************************/

for (i = 0; i < 3; i++)
  to->position[i] = from->position[i] + 2 * r * plane->normal[i];
}

static void static_rotate (gamgi_symmetry *symmetry, 
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to)
{
double x[3], y[3], a[3], b[3], c[3];
double angle, sin_angle, cos_angle, length;

to->element = from->element;

/******************************************************
 * >x< is the relative position vector going from the *
 * axis point (geometric center) to the atom position *
 ******************************************************/

x[0] = from->position[0] - axis->distance * axis->normal[0];
x[1] = from->position[1] - axis->distance * axis->normal[1];
x[2] = from->position[2] - axis->distance * axis->normal[2];

/**********************************************************
 * dot is the length of the x vector (relative position), *
 * projected along the >direction< (rotation axis) vector *
 **********************************************************/

length = x[0] * axis->direction[0] +
x[1] * axis->direction[1] + x[2] * axis->direction[2];

/******************************************
 * >a< is the >x< vector projected along  *
 * the >direction< (rotation axis) vector *
 ******************************************/

a[0] = length * axis->direction[0];
a[1] = length * axis->direction[1];
a[2] = length * axis->direction[2];

/**************************************
 * >b< is the vector perpendicular to *
 * >direction< (rotation axis) going  *
 * from the axis to the atom position *
 **************************************/

b[0] = x[0] - a[0];
b[1] = x[1] - a[1];
b[2] = x[2] - a[2];

/******************************************************************
 * >c< is the third vector, perpendicular to >b< and >direction<, *
 * obtained from the cross product: >b< cross >axis->direction<   *
 ******************************************************************/

c[0] = b[1]*axis->direction[2] - b[2]*axis->direction[1];
c[1] = b[2]*axis->direction[0] - b[0]*axis->direction[2];
c[2] = b[0]*axis->direction[1] - b[1]*axis->direction[0];

/********************************************************
 * rotation parameters: for a rotation of infinite      *
 * order, any angle should work fine, so we arbitrarily *
 * choose 1 radian, which is a large rotation, enough   *
 * to check whether the atoms are indeed aligned or not *
 ********************************************************/

if (axis->order == 0) angle = 1.0;
else angle = 2 * GAMGI_MATH_PI / axis->order;
sin_angle = sin (angle);
cos_angle = cos (angle);

/**********************************
 * after rotation >x< becomes >y< *
 **********************************/

y[0] = a[0] + b[0]*cos_angle + c[0]*sin_angle;
y[1] = a[1] + b[1]*cos_angle + c[1]*sin_angle;
y[2] = a[2] + b[2]*cos_angle + c[2]*sin_angle;

/******************************************************************
 * Initially we went from absolute (>from->position<) to relative *
 * coordinates (>x<). Now we reverse that transformation, going   *
 * from relative (>y<) to absolute coordinates (>to->position<).  *
 ******************************************************************/

to->position[0] = y[0] + axis->distance * axis->normal[0];
to->position[1] = y[1] + axis->distance * axis->normal[1];
to->position[2] = y[2] + axis->distance * axis->normal[2];
}

static void static_rotate_reflect (gamgi_symmetry *symmetry, 
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to)
{
double x[3], y[3], a[3], b[3], c[3];
double angle, sin_angle, cos_angle, length;

to->element = from->element;

/******************************************************
 * >x< is the relative position vector going from the *
 * axis point (geometric center) to the atom position *
 ******************************************************/

x[0] = from->position[0] - axis->distance * axis->normal[0];
x[1] = from->position[1] - axis->distance * axis->normal[1];
x[2] = from->position[2] - axis->distance * axis->normal[2];

/**********************************************************
 * dot is the length of the x vector (relative position), *
 * projected along the >direction< (rotation axis) vector *
 **********************************************************/

length = x[0] * axis->direction[0] +
x[1] * axis->direction[1] + x[2] * axis->direction[2];

/******************************************
 * >a< is the >x< vector projected along  *
 * the >direction< (rotation axis) vector *
 ******************************************/

a[0] = length * axis->direction[0];
a[1] = length * axis->direction[1];
a[2] = length * axis->direction[2];

/**************************************
 * >b< is the vector perpendicular to *
 * >direction< (rotation axis) going  *
 * from the axis to the atom position *
 **************************************/

b[0] = x[0] - a[0];
b[1] = x[1] - a[1];
b[2] = x[2] - a[2];

/******************************************************************
 * >c< is the third vector, perpendicular to >b< and >direction<, *
 * obtained from the cross product: >b< cross >axis->direction<   *
 ******************************************************************/

c[0] = b[1]*axis->direction[2] - b[2]*axis->direction[1];
c[1] = b[2]*axis->direction[0] - b[0]*axis->direction[2];
c[2] = b[0]*axis->direction[1] - b[1]*axis->direction[0];

/********************************************************
 * rotation parameters: for a rotation of infinite      *
 * order, any angle should work fine, so we arbitrarily *
 * choose 1 radian, which is a large rotation, enough   *
 * to check whether the atoms are indeed aligned or not *
 ********************************************************/

if (axis->order == 0) angle = 1.0;
else angle = 2 * GAMGI_MATH_PI / axis->order;
sin_angle = sin (angle);
cos_angle = cos (angle);

/********************************************************************
 *      after rotation AND reflection >x< becomes >y<               *
 *                                                                  *
 * The only difference between this function and static_rotate      *
 * and is the minus signal in the vector >a<. As >a< is a vector    *
 * from the global center to the projection of the initial point    *
 * along the rotation axis, substracting >a< instead of adding it   *
 * as before effectively reflects the rotated point along the plane *
 * normal to the rotation axis that passes in the global center     *
 ********************************************************************/

y[0] = -a[0] + b[0]*cos_angle + c[0]*sin_angle;
y[1] = -a[1] + b[1]*cos_angle + c[1]*sin_angle;
y[2] = -a[2] + b[2]*cos_angle + c[2]*sin_angle;

/******************************************************************
 * Initially we went from absolute (>from->position<) to relative *
 * coordinates (>x<). Now we reverse that transformation, going   *
 * from relative (>y<) to absolute coordinates (>to->position<).  *
 ******************************************************************/

to->position[0] = y[0] + axis->distance * axis->normal[0];
to->position[1] = y[1] + axis->distance * axis->normal[1];
to->position[2] = y[2] + axis->distance * axis->normal[2];
}

static int static_apply (gamgi_symmetry *symmetry, gamgi_element *element)
{
int i, j, j_best;
double distance, distance_best;
int n_atoms = symmetry->n_atoms;
gamgi_atom **atoms = symmetry->atoms;
gamgi_atom new;
char *used;

/***********************************************
 * used is only allocated inside this function *
 ***********************************************/
 
used = (char *) malloc (sizeof (char) * n_atoms);
for (i = 0; i < n_atoms; i++) used[i] = 0;

for (i = 0; i < n_atoms; i++)
  {
  if (element->table[i] < 0)
    {
    /*********************************************
     * No symmetric atom yet: apply operation of *
     * symmetry; new coordinates: new->position  *
     *********************************************/

    element->function (symmetry, element, atoms[i], &new);

    j_best = i;

    /******************************************************
     * initialize distance_best with a distance too large *
     ******************************************************/

    distance_best = 2.0 * symmetry->initial;

    for (j = 0; j < n_atoms; j++)
      {
      if (atoms[j]->element != new.element || used[j] == 1) continue;

      distance = sqrt (pow2 (new.position[0] -  atoms[j]->position[0]) +
      pow2 (new.position[1] -  atoms[j]->position[1]) + 
      pow2 (new.position[2] -  atoms[j]->position[2]));

      if (distance < distance_best)
        {
        j_best = j;
        distance_best = distance;
        }
      }

    /*************************************************************
     * best pair for atom i is j_best with error = distance_best * 
     *************************************************************/

    if (distance_best > symmetry->initial)
      { 
      /*****************************************
       * There is no symmetric atom for atom i *
       *****************************************/

      free (used);
      return -1;
      }

    /*******************************************************************
     * save symmetric pair for atom i and flag it down as already used *
     *******************************************************************/

    element->table[i] = j_best;
    used[j_best] = 1;
    }
  }

/****************************************************
 * used is always free before leaving this function *
 ****************************************************/

free (used);
return 1;
}

static int static_order (gamgi_symmetry *symmetry, gamgi_element *element)
{
int n_atoms = symmetry->n_atoms;
int i, j, k ;

for (i = 0; i < n_atoms; i++)
  {
  /******************************************************************
   * for any operation of symmetry, some atoms may remain unchanged *
   ******************************************************************/

  if (element->table[i] == i) continue;

  /***************************************************
   * for improper axis, second-order transform is ok *
   ***************************************************/

  if (element->function == static_rotate_reflect)
    {
    j = element->table[i];

    if (element->table[j] == i) continue; 
    }

  /******************************************************
   * if an atom is changed by an operation of symmetry, *
   * then it should return to its initial position      *
   * only after exactly element->order operations.      *
   ******************************************************/

  for (j = element->order - 1, k = element->table[i]; 
  j > 0; j--, k = element->table[k])
    if (k == i) return -1;

  /**************************************
   * For improper axes, the complete    *
   * loop may also take twice the order *
   **************************************/

  if (k != i && element->function == static_rotate_reflect)
    {
   /**************************************
    * improper rotation transform looped *
    * j steps too early from atom i      *
    **************************************/

    for (j = element->order; j > 0; j--, k = element->table[k])
      if (k == i) return -1;
    }

  /************************************************
   * atom does not return to its initial position *
   * after element->order operations of symmetry. *
   ************************************************/

  if (k != i) return -1;
  }

return 1;
}

static int static_same (gamgi_symmetry *symmetry, gamgi_element *a, gamgi_element *b)
{
int n_atoms = symmetry->n_atoms;
int i, j;
int same;

/***********************************************
 * check if elements a,b are in the same class *
 ***********************************************/

if ((a->order != b->order) || (a->parameters != b->parameters) || 
(a->function != b->function)) return 0;

/******************************************************
 * check if all atoms are transformed in the same way *
 ******************************************************/

same = 1;
for (i = 0; i < n_atoms; i++)
  {
  if (a->table[i] != b->table[i])
    {
    same = 0;
    break;
    }
  }

if (same == 0 && a->order > 2)
  {
  /***************************************************
   * b can also be a reverse transformation for a:   *
   * C6 and C-6 rotations are different for the test *
   * above, but are the same for the test below.     *
   ***************************************************/

  for (i = 0; i < n_atoms; i++)
    {
    j = a->table[i];
    if (b->table[j] != i) return 0;
    }

  return 1;
  }

return same;
}

static gamgi_element *static_element (void)
{
return calloc (1, sizeof (gamgi_element));
}

static gamgi_element *static_create (gamgi_symmetry *symmetry)
{
gamgi_element *element;
int n_atoms = symmetry->n_atoms;
int n;

element = static_element ();

/***************************************************************
 * initialize each symmetry atom pair with an impossible value *
 ***************************************************************/

element->table = (int *) malloc (sizeof (int) * n_atoms);
for (n = 0; n < n_atoms; n++) element->table[n] = -1;

return element;
}

static gamgi_element *static_copy (gamgi_element *old)
{
gamgi_element *new;

/**********************************
 * the element is well known, so  *
 * table and function are ignored *
 **********************************/

new = static_element ();
new->function = NULL;
new->table = NULL;
new->order = old->order;
new->parameters = old->parameters;
new->error = old->error;
new->distance = old->distance;
gamgi_math_vector_copy (old->normal, new->normal);
gamgi_math_vector_copy (old->direction, new->direction);

return new;
}

static void static_insert (gamgi_element ***elements,
int *n_elements, int new)
{
int k;
 
*n_elements += 1;

*elements = (gamgi_element **) realloc (*elements,
sizeof (gamgi_element *) * (*n_elements));
 
for (k = *n_elements - 1; k > new; k--)
  (*elements)[k] = (*elements)[k - 1];

(*elements)[new] = static_element ();
}

static void static_insert_copy (gamgi_element ***elements,
int *n_elements, int old, int new)
{
static_insert (elements, n_elements, new);

if (old >= new) old++;
(*elements)[new] = static_copy ((*elements)[old]);
}

static void static_resize (gamgi_element ***elements, int *n_elements)
{
int k, empty = 0;

for (k = 0; k < *n_elements; k++)
  {
  if ((*elements)[k] == NULL) empty++;
  else (*elements)[k - empty] = (*elements)[k];
  }
*n_elements -= empty;
*elements = (gamgi_element **) realloc (*elements,
sizeof (gamgi_element *) * (*n_elements));
}

static void static_remove (gamgi_element *element)
{
/*****************************************************************
 * element is always non NULL here, unless there is a gross      *
 * mistake somewhere and in that case it is better to crash      *
 * the program (so the mistake is immediately found and removed) *
 *****************************************************************/

free (element->table);
free (element);
}

static void static_restrict (gamgi_element ***elements, int *n_elements)
{
int k, order;

for (k = 0; k < *n_elements; k++)
  {
  order = (*elements)[k]->order;
  if (order > 6 || order == 5)
    {
    static_remove ((*elements)[k]);
    (*elements)[k] = NULL;
    }
  }
static_resize (elements, n_elements);
}

static void static_delete (gamgi_element ***elements, 
int *n_elements, int offset)
{
int k;

for (k = offset; k < *n_elements; k++)
  {
  static_remove ((*elements)[k]);
  (*elements)[k] = NULL;
  }
static_resize (elements, n_elements);
}

static int static_quality (gamgi_symmetry *symmetry, gamgi_element *element)
{
int i, j;
int n_atoms = symmetry->n_atoms;
gamgi_atom **atoms = symmetry->atoms;
gamgi_atom new;
double r, error = 0.0;

for (i = 0; i < n_atoms; i++)
  {
  element->function (symmetry, element, atoms[i], &new);
  j = element->table[i];

  r = sqrt (pow2 (new.position[0] -  atoms[j]->position[0]) +
  pow2 (new.position[1] -  atoms[j]->position[1]) + 
  pow2 (new.position[2] -  atoms[j]->position[2]));

  /****************************************
   * distance between >new< (transformed) *
   * and >j< (symmetric) atoms is too big *
   ****************************************/

  if (r > symmetry->final) return -1;

  if (r > error) error = r;
  }

element->error = error;

return 1;
}

static int static_evaluate (gamgi_symmetry *symmetry,
gamgi_element *element, double *sum, double *max)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_atom new;
int i, j;
double r;

if (element->parameters >= 4)
  {
  /*****************
   * update normal *
   *****************/

  r =  sqrt (pow2 (element->normal[0]) + pow2 (element->normal[1]) +
  pow2 (element->normal[2]));

  /*********************
   * Normal collapsed! *
   *********************/

  if (r < GAMGI_MATH_TOLERANCE_LENGTH) return -1;

  element->normal[0] /= r;
  element->normal[1] /= r;
  element->normal[2] /= r;
        
  /*******************
   * update distance *
   *******************/

  if (element->distance < 0)
    {
    element->distance = -element->distance;

    element->normal[0] = -element->normal[0] ;
    element->normal[1] = -element->normal[1] ;
    element->normal[2] = -element->normal[2] ;
    }
  }

if (element->parameters >= 7)
  {
  /********************
   * update direction *
   ********************/

  r =  sqrt (pow2 (element->direction[0]) + pow2 (element->direction[1]) +
  pow2 (element->direction[2]));

  /************************
   * Direction collapsed! *
   ************************/

  if (r < GAMGI_MATH_TOLERANCE_LENGTH) return -1;

  element->direction[0] /= r;
  element->direction[1] /= r;
  element->direction[2] /= r;
  }

/***************************************************
 * evaluate quality of optimized symmetry element: *
 * max (maximum error) and sum (accumulated error) *
 ***************************************************/

*sum = *max = 0;
for (i = 0; i < symmetry->n_atoms; i++)
  {
  element->function (symmetry, element, atoms[i], &new);
  j = element->table[i];

  r = pow2 (atoms[j]->position[0] - new.position[0]) + 
  pow2 (atoms[j]->position[1] - new.position[1]) +
  pow2 (atoms[j]->position[2] - new.position[2]);

  if (r > *max) *max = r;
  *sum += r;
  }

return 1;
}

static void static_get (gamgi_element *element, double *parameters)
{
memcpy (parameters, &element->distance, element->parameters * sizeof (double));
}

static void static_set (gamgi_element *element, double *parameters)
{
memcpy (&element->distance, parameters, element->parameters * sizeof (double));
}

static int static_optimize (gamgi_symmetry *symmetry, gamgi_element *element)
{
double parameters[7];
double gradient[7];
double force[7];
double step[7];
double sum, sum_old, sum_new1, sum_new2, sum_down, sum_up, max;
double a, b, x, norm;
int n, n_parameters = element->parameters;
int cycle = 0;
int hits = 0;

sum = 0;
do
  {
  sum_old = sum;

  /*******************************************************
   * evaluate and check if maximum error is small enough *
   *******************************************************/

  if (static_evaluate (symmetry, element, &sum, &max) < 0) return -1;
  if (sqrt (max) < symmetry->final) return 1;

  /******************************************
   * check if accumulated error is changing *
   ******************************************/

  if (cycle > 0)
    {
    if (fabs (sum - sum_old) > GAMGI_CHEM_STEP_THRESHOLD) hits = 0;
    else hits++;

    /**********************************************************
     * optimization failed: accumulated error is not changing *
     **********************************************************/

    if (hits >= GAMGI_CHEM_STEP_HITS) return -1;
    }

  /************************************************************
   * evaluate function, gradient and diagonal force constants *
   ************************************************************/

  static_get (element, parameters);
  for (n = 0; n < n_parameters; n++)
    {
    /***************************
     * shift down and evaluate *
     ***************************/

    parameters[n] -= GAMGI_CHEM_STEP_GRADIENT;
    static_set (element, parameters);
    if (static_evaluate (symmetry, element, &sum_down, &max) < 0) return -1;

    /*************************
     * shift up and evaluate *
     *************************/

    parameters[n] += 2*GAMGI_CHEM_STEP_GRADIENT;
    static_set (element, parameters);
    if (static_evaluate (symmetry, element, &sum_up, &max) < 0) return -1;

    /****************************
     * reset and grab step data *
     ****************************/

    parameters[n] -= GAMGI_CHEM_STEP_GRADIENT;
    gradient[n] = (sum_up - sum_down) / (2*GAMGI_CHEM_STEP_GRADIENT);
    force[n] = (sum_up + sum_down - 2*sum) / 
    (GAMGI_CHEM_STEP_GRADIENT * GAMGI_CHEM_STEP_GRADIENT);
    }

  /***************************
   * set a quasy-Newton step *
   ***************************/

  norm = 0.0;
  for (n = 0; n < n_parameters; n++)
    {
    if (force[n] <  0) force[n] = -force[n];
    if (force[n] < GAMGI_CHEM_FORCE_MIN) force[n] = GAMGI_CHEM_FORCE_MIN;
    if (force[n] > GAMGI_CHEM_FORCE_MAX) force[n] = GAMGI_CHEM_FORCE_MAX;
    step[n] = - gradient[n] / force[n];
    norm += step[n] * step[n];
    }

  /***************************************
   * make sure the step is not too large *
   ***************************************/

  norm = sqrt (norm);
  if (norm > GAMGI_CHEM_STEP_MAX)
    {
    /******************** 
     * renormalize step *
     ********************/

    for (n = 0; n < n_parameters; n++)
      step[n] *= GAMGI_CHEM_STEP_MAX/norm;
    norm = GAMGI_CHEM_STEP_MAX;
    }

  /*********************************************************
   * get a configuration with lower accumulated error or   *
   * keep decreasing step until a minimum limit is reached *
   *********************************************************/

  do
    {
    /**********************************
     * change parameters and evaluate *
     **********************************/

    for (n = 0; n < n_parameters; n++) parameters[n] += step[n];
    static_set (element, parameters);
    if (static_evaluate (symmetry, element, &sum_new1, &max) < 0) return -1;
    if (sum_new1 < sum) break;

    /*****************************************
     * reset parameters and set smaller step *
     *****************************************/

    for (n = 0; n < n_parameters; n++)
      {
      parameters[n] -= step[n];
      step[n] /= 2;
      }

    static_set (element, parameters);
    norm /= 2;
    } while (norm > GAMGI_CHEM_STEP_MIN);

  /************************************************************
   * optimization failed: accumulated error is not decreasing *
   ************************************************************/

  if (norm <= GAMGI_CHEM_STEP_MIN) return -1;

  /****************************************************************
   * if linear optimization succeded, but only with a small step, *
   * try quadratic optimization to further improve parameters     *
   ****************************************************************/

  if (norm < GAMGI_CHEM_STEP_MAX / 2)
    {
    /********************************************************* 
     * do another linear step, evaluate and reset parameters *
     *********************************************************/

    for (n = 0; n < n_parameters; n++) parameters[n] += step[n];
    static_set (element, parameters);
    if (static_evaluate (symmetry, element, &sum_new2, &max) < 0) return -1;
    for (n = 0; n < n_parameters; n++) parameters[n] -= 2*step[n];

    /******************************************************
     * interpolation base points: sum, sum_new1, sum_new2 *
     *                                                    *
     * linear interpolation coefficients: a, b            *
     ******************************************************/

    a = (4*sum - sum_new2 - 3*sum_new1) / 2;
    b = (sum + sum_new2 - 2*sum_new1) / 2;

    /********************************************
     * try to apply interpolation to parameters *
     ********************************************/

    if (b > 0)
      {
      x = -a/(2*b);
      if (x > 0.2 && x < 1.8)
        for (n = 0; n < n_parameters; n++) parameters[n] += x*step[n];
      else b = 0;
      }

    /*********************************************************
     * interpolation failed: restore parameters for the case *
     * with smaller accumulated error: sum_new1 or sum_new2  *
     *********************************************************/

    if (b <= 0)
      {
      if (sum_new2 < sum_new1)
        for (n = 0; n < n_parameters; n++) parameters[n] += 2*step[n];
      else
        for (n = 0; n < n_parameters; n++) parameters[n] += step[n];
      }
    static_set (element, parameters);
    }
  } while (++cycle < GAMGI_CHEM_STEP_CYCLES);

/*********************************************************
 * optimization failed: maximum number of cycles reached *
 *********************************************************/

return -1;
}

static int static_refine (gamgi_symmetry *symmetry, gamgi_element *element, int apply)
{
int n;

/*******************************************************
 * no symmetry correspondence table can be constructed *
 *******************************************************/

if (apply == 1 && static_apply (symmetry, element) < 0) return -1;

/*****************************************************
 * transformation is identical to inversion center n *
 *****************************************************/

for (n = 0; n < symmetry->n_i; n++)
  if (static_same (symmetry, symmetry->i[n], element) == 1) return -1;

/******************************************
 * transformation is identical to plane n *
 ******************************************/

for (n = 0; n < symmetry->n_m; n++)
  if (static_same (symmetry, symmetry->m[n], element) == 1) return -1;

/************************************************
 * transformation is identical to normal axis n *
 ************************************************/

for (n = 0; n < symmetry->n_C; n++)
  if (static_same (symmetry, symmetry->C[n], element) == 1) return -1;

/**************************************************
 * transformation is identical to improper axis n *
 **************************************************/

for (n = 0; n < symmetry->n_S; n++)
  if (static_same (symmetry, symmetry->S[n], element) == 1) return -1;

/********************************** 
 * incorrect transformation order *
 **********************************/

if (static_order (symmetry, element) < 0) return -1;

/*************************
 * refine transformation *
 *************************/

if (static_optimize (symmetry, element) < 0) return -1;

/*******************************************
 * refined transformation not close enough *
 *******************************************/

if (static_quality (symmetry, element) < 0) return -1;

return 1;
}

/********************************
 * inversion-specific functions *
 ********************************/

static gamgi_element *static_try_i (gamgi_symmetry *symmetry)
{
gamgi_element *center;
double r;

/*******************************
 * initialize symmetry element *
 *******************************/

center = static_create (symmetry);

center->function = static_invert;
center->order = 2;
center->parameters = 4;

/************************************************
 * get distance from origin to geometric center *
 ************************************************/

r = sqrt (pow2 (symmetry->center[0]) + pow2 (symmetry->center[1]) +
pow2 (symmetry->center[2]));

/******************************************************
 * normal pointing from origin to geometric center or *
 * (1, 0, 0) when geometric center is at the origin   *
 ******************************************************/

if (r > 0)
  {
  center->normal[0] = symmetry->center[0] / r;
  center->normal[1] = symmetry->center[1] / r;
  center->normal[2] = symmetry->center[2] / r;
  }
else
  {
  center->normal[0] = 1;
  center->normal[1] = 0;
  center->normal[2] = 0;
  }

/****************************************************
 * distance from the origin to the geometric center *
 ****************************************************/

center->distance = r;

if (static_refine (symmetry, center, 1) < 0)
  {
  /************************************************
   * after refinement: inversion center not found *
   ************************************************/

  static_remove (center);
  return NULL;
  }

return center;
}

/****************************
 * plane-specific functions *
 ****************************/

static gamgi_element *static_try_m (gamgi_symmetry *symmetry, int i, int j)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_element *plane;
double dif[3], mid[3];
double r;

/****************************************
 * Try the plane half-way between atoms *
 * i and j, normal to the vector rij    *
 ****************************************/

plane = static_create (symmetry);

plane->function = static_reflect;
plane->order = 2;
plane->parameters = 4;

/**************
 * difference *
 **************/

dif[0] = atoms[i]->position[0] - atoms[j]->position[0];
dif[1] = atoms[i]->position[1] - atoms[j]->position[1];
dif[2] = atoms[i]->position[2] - atoms[j]->position[2];

/**********************************
 * distance between atoms i and j *
 **********************************/

r = sqrt (dif[0]*dif[0] + dif[1]*dif[1] + dif[2]*dif[2]);
if (r < GAMGI_MATH_TOLERANCE_LENGTH) return NULL;

/************
 * midpoint *
 ************/

mid[0] = (atoms[i]->position[0] + atoms[j]->position[0]) / 2.0;
mid[1] = (atoms[i]->position[1] + atoms[j]->position[1]) / 2.0;
mid[2] = (atoms[i]->position[2] + atoms[j]->position[2]) / 2.0;

/*********************************************
 * unitary vector perpendicular to the plane *
 *********************************************/

plane->normal[0] = dif[0] / r;
plane->normal[1] = dif[1] / r;
plane->normal[2] = dif[2] / r;

/***************************************************
 * project vector >mid< along direction >normal<,  *
 * to get the distance >r< from the origin to the  *
 * plane, along the direction normal to the plane. *
 ***************************************************/
 
r = mid[0] * plane->normal[0] + mid[1] * plane->normal[1] + 
mid[2] * plane->normal[2];

/***********************************************************
 * >r< negative: the dot product is negative, so >normal<  *
 * and >mid< vectors point in opposite directions: reverse *
 * normal direction, so distance becomes positive and the  *
 * >normal< vector points from the origin to the plane.    *
 ***********************************************************/

if (r < 0)
  {  
  r = -r;

  plane->normal[0] = -plane->normal[0];
  plane->normal[1] = -plane->normal[1];
  plane->normal[2] = -plane->normal[2];
  }

/****************************************************
 * before refinement: plane is at r from the origin *
 ****************************************************/

plane->distance = r;

if (static_refine (symmetry, plane, 1) < 0)
  {
  /*************************************************
   * after refinement: plane of symmetry not found *
   *************************************************/

  static_remove (plane);
  return NULL;
  }

return plane;
}

static gamgi_element *static_try_m0 (gamgi_symmetry *symmetry)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_element *plane;
int n_atoms = symmetry->n_atoms;
int i, j, k;
double d0[3], d1[3], d2[3], p[3];
double r, s0, s1, s2;
double *d = NULL;

/**************************************
 * Try the plane containing all atoms *
 **************************************/

plane = static_create (symmetry);

plane->function = static_reflect;
plane->order = 1;
plane->parameters = 4;

/***************************************************
 * calculate average vector perpendicular to plane *
 ***************************************************/

/*********************************************************
 * we need to start with vectors along the 3 independent *
 * space directions xyz to guarantee that at least one   *
 * of them has a non-zero component perpendicular to the *
 * plane, otherwise the normal will colapse to zero.     *
 *********************************************************/

d0[0] = 1.0; d0[1] = 0.0; d0[2] = 0.0;
d1[0] = 0.0; d1[1] = 1.0; d1[2] = 0.0;
d2[0] = 0.0; d2[1] = 0.0; d2[2] = 1.0;

/****************************************************
 * This loop tries to eliminate contributions       *
 * of the normal, which appear within the molecular *
 * plane. Once all in-plane contributions are       *
 * eliminated, what remains must be the normal.     *
 ****************************************************/

for (i = 1; i < n_atoms; i++)
  {
  for (j = 0; j < i; j++)
    {
    /***********************************
     * normal vector between i,j atoms *
     ***********************************/

    p[0] = atoms[i]->position[0] - atoms[j]->position[0];
    p[1] = atoms[i]->position[1] - atoms[j]->position[1];
    p[2] = atoms[i]->position[2] - atoms[j]->position[2];

    r = sqrt (p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
    if (r < GAMGI_MATH_TOLERANCE_LENGTH) continue;

    p[0] /= r;
    p[1] /= r;
    p[2] /= r;

    /**************************************************
     * sn is the length of vector dn projected along  *
     * vector p (p is a unitary vector but dn is not) *
     *                                                *
     *   (sn * p) is a vector along p of length sn    *
     *                                                *
     *  dn - (sn * p) is a vector perpendicular to p  *
     **************************************************/

    s0 = p[0] * d0[0] + p[1] * d0[1] + p[2] * d0[2];
    s1 = p[0] * d1[0] + p[1] * d1[1] + p[2] * d1[2];
    s2 = p[0] * d2[0] + p[1] * d2[1] + p[2] * d2[2];

    for (k = 0; k < 3; k++)
      {
      d0[k] -= s0 * p[k];
      d1[k] -= s1 * p[k];
      d2[k] -= s2 * p[k];
      }
    }
  }

s0 = fabs (d0[0]) + fabs (d0[1]) + fabs (d0[2]);
s1 = fabs (d1[0]) + fabs (d1[1]) + fabs (d1[2]);
s2 = fabs (d2[0]) + fabs (d2[1]) + fabs (d2[2]);

if (s0 >= s1 && s0 >= s2) d = d0;
if (s1 >= s0 && s1 >= s2) d = d1;
if (s2 >= s0 && s2 >= s1) d = d2;

r = sqrt (d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

/*********************************************
 * unitary vector perpendicular to the plane *
 *********************************************/

if (r > 0)
  {
  plane->normal[0] = d[0] / r;
  plane->normal[1] = d[1] / r;
  plane->normal[2] = d[2] / r;
  }
else
  {
  plane->normal[0] = 1;
  plane->normal[1] = 0;
  plane->normal[2] = 0;
  }

/********************************************************
 * project vector >the global center< along direction   *
 * >normal<, to get the distance >r< from the origin to * 
 * the plane, along the direction normal to the plane.  *
 ********************************************************/

r = symmetry->center[0] * plane->normal[0] +
symmetry->center[1] * plane->normal[1] +
symmetry->center[2] * plane->normal[2];

plane->distance = r;

/******************************************************
 * all atoms are supposed to be in the plane of       *
 * symmetry, thus plane->order = 1 and each atom      *
 * must transfom into itself. As the transform        *
 * array is already known, the second argument of     *
 * static_refine is 0, so static_apply is not called. *
 ******************************************************/

for (k = 0; k < n_atoms; k++) plane->table[k] = k;

if (static_refine (symmetry, plane, 0) < 0)
  {
  /**********************************************************
   * after refinement: plane containing all atoms not found *
   **********************************************************/

  static_remove (plane);
  return NULL;
  }

return plane;
}

/**********************************
 * normal axes-specific functions *
 **********************************/

static gamgi_element *static_try_C0 (gamgi_symmetry *symmetry)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_element *axis;
int n_atoms = symmetry->n_atoms;
double *center = symmetry->center;
double sum[3], rel[3];
double r;
int i, k;

/*************************************
 * Try the axis containing all atoms *
 *************************************/

axis = static_create (symmetry);

axis->function = static_rotate;
axis->order = 0;
axis->parameters = 7;

/********************************************
 * calculate average vector paralel to axis *
 ********************************************/

sum[0] = 0; sum[1] = 0; sum[2] = 0;

for (i = 0; i < n_atoms; i++)
  {
  rel[0] = atoms[i]->position[0] - center[0];
  rel[1] = atoms[i]->position[1] - center[1];
  rel[2] = atoms[i]->position[2] - center[2];

  /**********************************************************
   * add vectors from origin to all atoms. We need to check *
   * the dot product to guarantee that the new >rel< vector *
   * and the sum >sum< of all previous vectors are pointing *
   * to the same side, thus summing up and not subtracting  *
   **********************************************************/

  r = rel[0]*sum[0] + rel[1]*sum[1] + rel[2]*sum[2];

  if (r >= 0)
    for (k = 0; k < 3; k++) sum[k] += rel[k];
  else
    for (k = 0; k < 3; k++) sum[k] -= rel[k];
  }

/*************************
 * normalize axis vector *
 *************************/

r = sqrt (sum[0]*sum[0] + sum[1]*sum[1] + sum[2]*sum[2]);

if (r > 0)
  {
  sum[0] /= r;
  sum[1] /= r;
  sum[2] /= r;
  }
else sum[0] = 1;

axis->direction[0] = sum[0];
axis->direction[1] = sum[1];
axis->direction[2] = sum[2];

/****************************************************
 * distance from the origin to the geometric center *
 ****************************************************/

r = sqrt (pow2 (center[0]) + pow2 (center[1]) + pow2 (center[2]));

axis->distance = r;

/************************************
 * unitary vector pointing from the *
 * origin to the geometric center   *
 ************************************/

if (r > 0)
  {
  axis->normal[0] = center[0] / r;
  axis->normal[1] = center[1] / r;
  axis->normal[2] = center[2] / r;
  }
else 
  {
  axis->normal[0] = 1;
  axis->normal[1] = 0;
  axis->normal[2] = 0;
  }

/******************************************************
 * all atoms are supposed to be in the axis of        *
 * symmetry, thus axis->order = 0 and each atom       *
 * must transfom into itself. As the transform        *
 * array is already known, the second argument of     *
 * static_refine is 0, so static_apply is not called. *
 ******************************************************/

for (k = 0; k < n_atoms; k++)
  axis->table[k] = k;

if (static_refine (symmetry, axis, 0) < 0)
  {
  /*********************************************************
   * after refinement: axis containing all atoms not found *
   *********************************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

static gamgi_element *static_try_Cm (gamgi_symmetry *symmetry)
{
gamgi_element *axis;
double *center = symmetry->center;
double r;

axis = static_create (symmetry);
axis->function = static_rotate;
axis->order = 2;
axis->parameters = 7;

r = sqrt (pow2 (center[0]) + pow2 (center[1]) + pow2 (center[2]));

if (r > 0)
  {
  axis->normal[0] = center[0] / r;
  axis->normal[1] = center[1] / r;
  axis->normal[2] = center[2] / r;
  }
else
  {
  axis->normal[0] = 1.0;
  axis->normal[1] = 0.0;
  axis->normal[2] = 0.0;
  }

axis->distance = r;

/*******************************************
 * Try C2 perpendicular to molecular plane *
 *******************************************/

axis->direction[0] = symmetry->plane->normal[0];
axis->direction[1] = symmetry->plane->normal[1];
axis->direction[2] = symmetry->plane->normal[2];

if (static_refine (symmetry, axis, 1) < 0)
  {
  /***************************************
   * after refinement: C2 axis not found *
   ***************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

static gamgi_element *static_try_C2 (gamgi_symmetry *symmetry, 
int i, int j, double *support)
{
gamgi_atom **atoms = symmetry->atoms;
double *center = symmetry->center;
gamgi_element *axis;
double ris, rjs, r;
double normal[3];

/**************************************************************
 * make sure atoms i,j are at the same distance from support, *
 * otherwise support can't actually define a rotation axis    *
 **************************************************************/

ris = sqrt (pow2 (atoms[i]->position[0] - support[0]) +
pow2 (atoms[i]->position[1] - support[1]) + 
pow2 (atoms[i]->position[2] - support[2]));

rjs = sqrt (pow2 (atoms[j]->position[0] - support[0]) +
pow2 (atoms[j]->position[1] - support[1]) + 
pow2 (atoms[j]->position[2] - support[2]));

if (fabs (ris - rjs) > symmetry->initial) return NULL;

/***************************************
 * Trying c2 axis for the pair (i, j)  *
 * with the help of the vector support *
 ***************************************/

axis = static_create (symmetry);

axis->function = static_rotate;
axis->order = 2;
axis->parameters = 7;

r = sqrt (pow2 (center[0]) + pow2 (center[1]) + pow2 (center[2]));

if (r > 0)
  {
  axis->normal[0] = center[0] / r;
  axis->normal[1] = center[1] / r;
  axis->normal[2] = center[2] / r;
  }
else
  {
  axis->normal[0] = 1;
  axis->normal[1] = 0;
  axis->normal[2] = 0;
  }

axis->distance = r;

/**********************************************************
 * i,j,support form a "T", so the center vector is normal *
 * to the i-j direction, defining a C2 axis for atoms i,j *
 **********************************************************/

normal[0] = (atoms[i]->position[0] + atoms[j]->position[0])/2 - support[0];
normal[1] = (atoms[i]->position[1] + atoms[j]->position[1])/2 - support[1];
normal[2] = (atoms[i]->position[2] + atoms[j]->position[2])/2 - support[2];

r = sqrt (normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

if (r < symmetry->initial)
  {
  /*******************************
   * c2 is undefined, try random *
   * direction perpendicular to  *
   * line defined by i,j atoms   *
   *******************************/

  normal[0] = atoms[i]->position[0] - atoms[j]->position[0];
  normal[1] = atoms[i]->position[1] - atoms[j]->position[1];
  normal[2] = atoms[i]->position[2] - atoms[j]->position[2];

  if (fabs (normal[2]) + fabs (normal[1]) > GAMGI_MATH_TOLERANCE_LENGTH)
    {
    axis->direction[0] =  0.0;
    axis->direction[1] =  normal[2];
    axis->direction[2] = -normal[1];
    }
  else
    {
    axis->direction[0] = -normal[2];
    axis->direction[1] =  0.0;
    axis->direction[2] =  normal[0];
    }

  r = sqrt (pow2 (axis->direction[0]) +
  pow2 (axis->direction[1]) + pow2 (axis->direction[2]));
  if (r < GAMGI_MATH_TOLERANCE_LENGTH)
    {
    static_remove (axis);
    return NULL ;
    }

  axis->direction[0] /= r;
  axis->direction[1] /= r;
  axis->direction[2] /= r;
  }
else
  {
  /***********************************
   * direction is ok, renormalize it *
   ***********************************/

  axis->direction[0] = normal[0] / r;
  axis->direction[1] = normal[1] / r;
  axis->direction[2] = normal[2] / r;
  }

if (static_refine (symmetry, axis, 1) < 0)
  {
  /***************************************
   * after refinement: c2 axis not found *
   ***************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

static gamgi_element *static_try_axis (gamgi_symmetry *symmetry, 
double a[3], double b[3], double c[3])
{
gamgi_element *axis;
double *center = symmetry->center;
int order, sign;
double ra, rb, rc, rab, rbc, rac, r;
double direction[3];
double angle;

/********************************
 * distance to geometric center *
 ********************************/

ra = sqrt (a[0]*a[0] + a[1]*a[1] + a[2]*a[2]); 
rb = sqrt (b[0]*b[0] + b[1]*b[1] + b[2]*b[2]); 
rc = sqrt (c[0]*c[0] + c[1]*c[1] + c[2]*c[2]); 

/****************************** 
 * points are not on a sphere *
 ******************************/

if (fabs (ra - rb) > symmetry->initial || 
fabs (ra - rc) > symmetry->initial || 
fabs (rb - rc) > symmetry->initial) return NULL;

/***************************
 * distance between points *
 ***************************/

rab = sqrt (pow2 (a[0]-b[0]) + pow2 (a[1]-b[1]) + pow2 (a[2]-b[2]));
rac = sqrt (pow2 (a[0]-c[0]) + pow2 (a[1]-c[1]) + pow2 (a[2]-c[2]));
rbc = sqrt (pow2 (b[0]-c[0]) + pow2 (b[1]-c[1]) + pow2 (b[2]-c[2]));

/************************************** 
 * points are not rotation-equivalent *
 **************************************/

if (fabs (rab - rbc) > symmetry->initial) return NULL;

/********************************************
 * rotation is underdefined by these points *
 ********************************************/

if (rab <= GAMGI_MATH_TOLERANCE_LENGTH || 
rbc <= GAMGI_MATH_TOLERANCE_LENGTH || 
rac <= GAMGI_MATH_TOLERANCE_LENGTH) return NULL;

/*****************************************
 * The internal angle is calculated from *
 * the triangle formed by adjacent atoms *
 * abc, in this order, so ra = rb = rc   *
 * and rab = rbc != rac.                 *
 *                                       *
 * divide the triangle a,c, (horizontal) *
 * b (below) in two right triangles:     *
 * rac/2 is the opposite side            *
 * (rab+rbc)/2 is the average diagonal   *
 *                                       *
 * get the half-angle, then multiply by  *
 * 2, to get the full internal angle.    *
 * For a polygon of order n, the angle   *
 * of rotation is 360/n = 180 - internal *
 * angle.                                *
 *****************************************/

/************************************************
 * atoms are too close to a straight line, so   *
 * order goes to infinit and angle goes to zero *
 ************************************************/

angle = GAMGI_MATH_PI - 2*asin (rac/(rab+rbc));
if (fabs(angle) <= GAMGI_MATH_TOLERANCE_ANGLE) return NULL;

/*********************************************************
 * the rotation axis order must be above 2: C2 axes are  *
 * handled elsewhere and S2 axes are handled differently *
 *********************************************************/

order = floor ((2*GAMGI_MATH_PI)/angle + 0.5);
if (order <= 2) return NULL;

axis = static_create (symmetry);
axis->order = order;
axis->parameters = 7;

/****************************************************
 * distance from the origin to the geometric center *
 ****************************************************/

r = sqrt (pow2 (center[0]) + pow2 (center[1]) + pow2 (center[2]));

axis->distance = r;

/************************************
 * unitary vector pointing from the *
 * origin to the geometric center   *
 ************************************/

if (r > 0)
  {
  axis->normal[0] = center[0] / r;
  axis->normal[1] = center[1] / r;
  axis->normal[2] = center[2] / r;
  }
else
  {
  axis->normal[0] = 1.0;
  axis->normal[1] = 0.0;
  axis->normal[2] = 0.0;
  }

/*****************************************************
 * arbitrarily select axis direction so that first   *
 * non-zero component of axis->direction is positive *
 *****************************************************/

direction[0] = (b[1]-a[1])*(c[2]-b[2]) - (b[2]-a[2])*(c[1]-b[1]);
direction[1] = (b[2]-a[2])*(c[0]-b[0]) - (b[0]-a[0])*(c[2]-b[2]);
direction[2] = (b[0]-a[0])*(c[1]-b[1]) - (b[1]-a[1])*(c[0]-b[0]);

/*****************************************************
 * arbitrarily select axis direction so that first   *
 * non-zero component of axis->direction is positive *
 *****************************************************/

sign = 0 ;
if (direction[0] <= 0)
  {
  if (direction[0] < 0) sign = 1;
  else
    {
    if (direction[1] <= 0)
      {
      if (direction[1] < 0) sign = 1;

      else if (direction[2] < 0) sign = 1;
      }
    }
  }

if (sign == 1)
  {
  /**************************
   * reverse axis direction *
   **************************/

  direction[0] = -direction[0];
  direction[1] = -direction[1];
  direction[2] = -direction[2];
  }

/******************************************
 * normalize axis vector: >r< cannot be 0 *
 * because of the tests applied above on  *
 * >ra<, >rb<, >rc<, >rab<, >rbc<, >rac<  *
 ******************************************/

r = sqrt (pow2 (direction[0]) + pow2 (direction[1]) + pow2 (direction[2]));

axis->direction[0] = direction[0] / r;
axis->direction[1] = direction[1] / r;
axis->direction[2] = direction[2] / r;

/****************************************************
 * axis origin is at: axis->distance * axis->normal *
 * axis is in the direction: axis->direction        *
*****************************************************/

return axis;
}

static gamgi_element *static_try_Cn (gamgi_symmetry *symmetry, 
int na, int nb, int nc)
{
gamgi_element *axis;
double a[3], b[3], c[3];
int i;

for (i = 0; i < 3; i++)
  {
  a[i] = symmetry->atoms[na]->position[i] - symmetry->center[i];
  b[i] = symmetry->atoms[nb]->position[i] - symmetry->center[i];
  c[i] = symmetry->atoms[nc]->position[i] - symmetry->center[i];
  }

/*******************************************************
 * Do a quick check of geometry validity, to see if it *
 * is possible a coherent axis defined by these points *
 ********************************************************/

if ((axis = static_try_axis (symmetry, a, b, c)) == NULL) return NULL;

/***********************************************
 * Trying Cn axis for the triplet (na, nb, nc) *
 ***********************************************/

axis->function = static_rotate;

if (static_refine (symmetry, axis, 1) < 0)
  {
  /***************************************************
   * after refinement: C(axis->order) axis not found *
   ***************************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

/************************************
 * improper axes-specific functions *
 ************************************/

static gamgi_element *static_try_S0 (gamgi_symmetry *symmetry)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_element *axis;
int n_atoms = symmetry->n_atoms;
double *center = symmetry->center;
double sum[3], rel[3];
double r;
int i, k;

/*************************************
 * Try the axis containing all atoms *
 *************************************/

axis = static_create (symmetry);

axis->function = static_rotate_reflect;
axis->order = 0;
axis->parameters = 7;

/********************************************
 * calculate average vector paralel to axis *
 ********************************************/

sum[0] = 0; sum[1] = 0; sum[2] = 0;

for (i = 0; i < n_atoms; i++)
  {
  rel[0] = atoms[i]->position[0] - center[0];
  rel[1] = atoms[i]->position[1] - center[1];
  rel[2] = atoms[i]->position[2] - center[2];

  /**********************************************************
   * add vectors from origin to all atoms. We need to check *
   * the dot product to guarantee that the new >rel< vector *
   * and the sum >sum< of all previous vectors are pointing *
   * to the same side, thus summing up and not subtracting  *
   **********************************************************/

  r = rel[0]*sum[0] + rel[1]*sum[1] + rel[2]*sum[2];

  if (r >= 0)
    for (k = 0; k < 3; k++) sum[k] += rel[k];
  else
    for (k = 0; k < 3; k++) sum[k] -= rel[k];
  }

/*************************
 * normalize axis vector *
 *************************/

r = sqrt (sum[0]*sum[0] + sum[1]*sum[1] + sum[2]*sum[2]);

if (r > 0)
  {
  sum[0] /= r;
  sum[1] /= r;
  sum[2] /= r;
  }
else sum[0] = 1;

axis->direction[0] = sum[0];
axis->direction[1] = sum[1];
axis->direction[2] = sum[2];

/****************************************************
 * distance from the origin to the geometric center *
 ****************************************************/

r = sqrt (pow2 (center[0]) + pow2 (center[1]) + pow2 (center[2]));

axis->distance = r;

/************************************
 * unitary vector pointing from the *
 * origin to the geometric center   *
 ************************************/

if (r > 0)
  {
  axis->normal[0] = center[0] / r;
  axis->normal[1] = center[1] / r;
  axis->normal[2] = center[2] / r;
  }
else 
  {
  axis->normal[0] = 1;
  axis->normal[1] = 0;
  axis->normal[2] = 0;
  }

if (static_refine (symmetry, axis, 1) < 0)
  {
  /*********************************************************
   * after refinement: axis containing all atoms not found *
   *********************************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

static gamgi_element *static_try_Sn (gamgi_symmetry *symmetry, 
int na, int nb, int nc)
{
gamgi_element * axis;
double a[3], b[3], c[3];
double centerpoint[3];
double r;
int i;

/**********************************************************************
 * we are looking for this geometry: a,c are below the mirror plane   *
 * and b is above, so their height is - + - as is always the case     *
 * in improper rotations. Summing b+a, with b+c gives a vector that   *
 * lives in the mirror plane. Then we define the vector equivalent    *
 * to b that is below the mirror, so a,c and this new vector define   *
 * a proper rotation which has the same axis as the improper rotation *
 **********************************************************************/

/**************************************** 
 * first, reduce the problem to Cn case *
 ****************************************/

for (i = 0; i < 3; i++)
  {
  a[i] = symmetry->atoms[na]->position[i] - symmetry->center[i];
  b[i] = symmetry->atoms[nb]->position[i] - symmetry->center[i];
  c[i] = symmetry->atoms[nc]->position[i] - symmetry->center[i];
  }

centerpoint[0] = a[0] + c[0] + 2*b[0];
centerpoint[1] = a[1] + c[1] + 2*b[1];
centerpoint[2] = a[2] + c[2] + 2*b[2];

r = sqrt (pow2 (centerpoint[0]) + pow2 (centerpoint[1]) + 
pow2 (centerpoint[2]));

if (r <= GAMGI_MATH_TOLERANCE_LENGTH) return NULL;

centerpoint[0] /= r;
centerpoint[1] /= r;
centerpoint[2] /= r;

r = centerpoint[0] * b[0] + centerpoint[1] * b[1] + centerpoint[2] * b[2];

/************************************************************
 * The new b is a point which, together with a and c, would *
 * define a proper axis of the same order, and the same     *
 * direction, as the improper axis we are looking for.      *
 ************************************************************/

b[0] = 2*r*centerpoint[0] - b[0];
b[1] = 2*r*centerpoint[1] - b[1];
b[2] = 2*r*centerpoint[2] - b[2];

/*************************************************************
 * Do a quick check of geometry validity, to see if it is    *
 * possible a coherent improper axis defined by these points *
 *************************************************************/

if ((axis = static_try_axis (symmetry, a, b, c)) == NULL) return NULL;

/***********************************************
 * Trying Sn axis for the triplet (na, nb, nc) *
 ***********************************************/

axis->function = static_rotate_reflect;

if (static_refine (symmetry, axis, 1) < 0)
  {
  /***************************************************
   * after refinement: S(axis->order) axis not found *
   ***************************************************/

  static_remove (axis);
  return NULL ;
  }

return axis;
}

/********************
 * Control routines *
 ********************/

static void static_center (gamgi_symmetry *symmetry)
{
int i;
int n_atoms = symmetry->n_atoms;
gamgi_atom **atoms = symmetry->atoms;
double center[3];

/******************************
 * calculate geometric center *
 ******************************/

center[0] = 0.0;
center[1] = 0.0;
center[2] = 0.0;

for (i = 0; i < n_atoms; i++)
  {
  center[0] += atoms[i]->position[0];
  center[1] += atoms[i]->position[1];
  center[2] += atoms[i]->position[2];
  }

symmetry->center[0] = center[0] / n_atoms;
symmetry->center[1] = center[1] / n_atoms;
symmetry->center[2] = center[2] / n_atoms;

/**************************************************************
 * calculate atomic distances (squared) from geometric center *
 **************************************************************/

symmetry->distance = (double *) malloc (n_atoms * sizeof (double));

for (i = 0; i < n_atoms; i++)
  symmetry->distance[i] = pow2 (atoms[i]->position[0] - symmetry->center[0]) +
  pow2 (atoms[i]->position[1] - symmetry->center[1]) +
  pow2 (atoms[i]->position[2] - symmetry->center[2]);
}

static void static_find_i (gamgi_symmetry *symmetry)
{
gamgi_element *center;

/************************
 * get inversion center *
 ************************/

center = static_try_i (symmetry);
if (center != NULL)
  {
  symmetry->i = (gamgi_element **) calloc (1, sizeof (gamgi_element *));
  symmetry->i[0] = center;
  symmetry->n_i = 1;
  }
}

static void static_find_m (gamgi_symmetry *symmetry)
{
gamgi_element *plane;
int n_atoms = symmetry->n_atoms;
gamgi_atom **atoms = symmetry->atoms;
int i, j;

/**********************************
 * find plane including all atoms *
 **********************************/

plane = static_try_m0 (symmetry);
if (plane != NULL)
  {
  symmetry->plane = plane;

  /***********************************
   * update array of symmetry planes *
   ***********************************/

  symmetry->n_m++;
  symmetry->m = (gamgi_element **) realloc (symmetry->m,
  sizeof (gamgi_element *) * symmetry->n_m);
  symmetry->m[symmetry->n_m - 1] = plane;
  }

/*********************************
 * find planes between atoms i,j *
 *********************************/

for (i = 1; i < n_atoms; i++)
  {
  for (j = 0; j < i; j++)
    {
    if (atoms[i]->element != atoms[j]->element) continue;

    plane = static_try_m (symmetry, i, j);
    if (plane != NULL)
      {
      /***********************************
       * update array of symmetry planes *
       ***********************************/

      symmetry->n_m++;
      symmetry->m = (gamgi_element **) realloc (symmetry->m,
      sizeof (gamgi_element *) * symmetry->n_m);
      symmetry->m[symmetry->n_m - 1] = plane;
      }
    }
  }
}

static void static_find_C0 (gamgi_symmetry *symmetry)
{
gamgi_element *axis;

/*********************************
 * find axis including all atoms *
 *********************************/

axis = static_try_C0 (symmetry);
if (axis != NULL)
  {
  symmetry->n_C++;
  symmetry->C = (gamgi_element **) realloc (symmetry->C,
  sizeof (gamgi_element *) * symmetry->n_C);
  symmetry->C[symmetry->n_C - 1] = axis;
  }
}

static void static_find_C2 (gamgi_symmetry *symmetry)
{
int i, j, k, l;
int n_atoms = symmetry->n_atoms;
gamgi_atom **atoms = symmetry->atoms;
double *distances;
double r, center[3];
gamgi_element *axis;

/*********************************************
 * try C2 axis perpendicular to global plane *
 *********************************************/

if (symmetry->plane != NULL && n_atoms > 1 &&
(axis = static_try_Cm (symmetry)) != NULL)
  {
  symmetry->n_C++;
  symmetry->C = (gamgi_element **) realloc (symmetry->C,
  sizeof (gamgi_element *) * symmetry->n_C);
  symmetry->C[symmetry->n_C - 1] = axis;
  }

/****************************************************
 * distances is allocated only inside this function *
 ****************************************************/

distances = malloc (sizeof (double) * n_atoms);
for (i = 0; i < n_atoms; i++) distances[i] = 0.0;

for (i = 1; i < n_atoms; i++)
  {
  for (j = 0; j < i; j++)
    {
    if (atoms[i]->element != atoms[j]->element) continue;

    /******************************************* 
     * A very cheap, but quite effective check *
     *******************************************/

    if (fabs (symmetry->distance[i] - symmetry->distance[j]) > 
    symmetry->initial) continue;

    /**************************************************************
     * First, let's try to get it cheap and use the global center *
     **************************************************************/

    center[0] = (atoms[i]->position[0] + atoms[j]->position[0]) / 2;
    center[1] = (atoms[i]->position[1] + atoms[j]->position[1]) / 2;
    center[2] = (atoms[i]->position[2] + atoms[j]->position[2]) / 2;

    r = sqrt (pow2 (center[0] - symmetry->center[0]) + 
    pow2 (center[1] - symmetry->center[1]) + 
    pow2 (center[2] - symmetry->center[2]));

    /*************************************************
     * the global center can be used as the support  *
     * point as long as its distance to the midpoint *
     * is big enought to accurately define a vector  *
     *************************************************/

    if (r > GAMGI_MATH_TOLERANCE_LENGTH)
      {
      /********************************************
       * the global center is far enough from i,j *
       * midpoint to be used as the support point *
       ********************************************/

      axis = static_try_C2 (symmetry, i, j, symmetry->center);
      if (axis != NULL)
        {
        symmetry->n_C++;
        symmetry->C = (gamgi_element **) realloc (symmetry->C, 
        sizeof (gamgi_element *) * symmetry->n_C);
        symmetry->C[symmetry->n_C - 1] = axis;
        }

      /***************************************************
       * we can now leave this i,j pair because symmetry *
       * axes must pass through the global center, so    *
       * other C2 axis for this i,j pair is not possible *
       ***************************************************/

      continue;
      }

    /*******************************
     * C2 can pass through some    *
     * other atom, used as support *
     *******************************/

    for (k = 0; k < n_atoms; k++)
      {
      /*************************************************************
       * the i,j midpoint is in (or fairly close to) the geometric *
       * center, so it is possible to have an unlimited number of  *
       * c2 axes passing in this i,j midpoint and any atom k       *
       *************************************************************/

      if ((axis = static_try_C2 (symmetry, i, j, atoms[k]->position)) != NULL)
        {
        symmetry->n_C++;
        symmetry->C = (gamgi_element **) realloc (symmetry->C, 
        sizeof (gamgi_element *) * symmetry->n_C);
        symmetry->C[symmetry->n_C - 1] = axis;
        }

      /******************************************************
       * we cannot leave this i,j pair yet because other    *
       * C2 symmetry axis are possible for this i,j pair,   *
       * as long as they all pass through the global center *
       ******************************************************/
      }

    /******************************************************
     * Prepare data for an additional pre-screening check *
     ******************************************************/

    for (k = 0; k < n_atoms; k++)
      distances[k] = sqrt (pow2 (atoms[k]->position[0] - center[0]) + 
      pow2 (atoms[k]->position[1] - center[1]) + 
      pow2 (atoms[k]->position[2] - center[2]));

    /*************************************
     * C2 can pass through the middle of *
     * some other pair, used as support  *
     *************************************/

    for (k = 0; k < n_atoms; k++)
      {
      for (l = 0 ; l < n_atoms; l++)
        {
        if (atoms[k]->element != atoms[l]->element) continue;

        /***************************************************
         * We really need this one to run reasonably fast! *
         ***************************************************/

        if (fabs (symmetry->distance[k] - symmetry->distance[l]) > 
        symmetry->initial || fabs (distances[k] - 
        distances[l]) > symmetry->initial) continue;

        /************
         * midpoint *
         ************/

        center[0] = (atoms[k]->position[0] + atoms[l]->position[0]) / 2;
        center[1] = (atoms[k]->position[1] + atoms[l]->position[1]) / 2;
        center[2] = (atoms[k]->position[2] + atoms[l]->position[2]) / 2;

        /*************************************************************
         * the i,j midpoint is in (or fairly close to) the geometric *
         * center, so it is possible to have an unlimited number of  *
         * c2 axes passing in this i,j midpoint and any k,l midpoint *
         *************************************************************/

        axis = static_try_C2 (symmetry, i, j, center);
        if (axis != NULL)
          {
          symmetry->n_C++ ;
          symmetry->C = (gamgi_element **) realloc (symmetry->C, 
          sizeof (gamgi_element *) * symmetry->n_C);
          symmetry->C[symmetry->n_C - 1] = axis;
          }
        }
      }
    }
  }

free (distances);
}

static void static_find_Cn (gamgi_symmetry *symmetry)
{
gamgi_atom **atoms = symmetry->atoms;
gamgi_element *axis;
int n_atoms = symmetry->n_atoms;
int i, j, k ;

for (i = 0; i < n_atoms; i++)
  {
  for (j = i + 1; j < n_atoms; j++)
    {
    if (atoms[i]->element != atoms[j]->element) continue;

    /*******************************************
     * A very cheap, but quite effective check *
     *******************************************/

    if (fabs (symmetry->distance[i] - symmetry->distance[j]) > 
    symmetry->initial) continue;

    for (k = 0; k < symmetry->n_atoms; k++)
      {
      if (atoms[i]->element != atoms[k]->element) continue;

      /**********************************************************
       * distance between atoms i,j,k and the geometric center  *
       * must be equal, distances i-j and j-k must be equal but *
       * distance i-k can be different (j is the central atom)  *
       **********************************************************/

      if ((fabs (symmetry->distance[i] - symmetry->distance[k]) > 
      symmetry->initial) || (fabs (symmetry->distance[j] - 
      symmetry->distance[k]) > symmetry->initial)) continue;

      axis = static_try_Cn (symmetry, i, j, k);
      if (axis != NULL)
        {
        symmetry->n_C++;
        symmetry->C = (gamgi_element **) realloc (symmetry->C,
        sizeof (gamgi_element *) * symmetry->n_C);
        symmetry->C[symmetry->n_C - 1] = axis;
        }
      }
    }
  }
}

static void static_find_S0 (gamgi_symmetry *symmetry)
{
gamgi_element *axis;

/*************************************
 * Try the axis containing all atoms *
 *************************************/

axis = static_try_S0 (symmetry);
if (axis != NULL)
  {
  symmetry->n_S++;
  symmetry->S = (gamgi_element **) realloc (symmetry->S,
  sizeof (gamgi_element *) * symmetry->n_S);
  symmetry->S[symmetry->n_S - 1] = axis;
  }
}

static void static_find_Sn (gamgi_symmetry *symmetry)
{
int i, j, k;
int n_atoms = symmetry->n_atoms;
gamgi_element *axis;

for (i = 0; i < n_atoms; i++)
  {
  for (j = i + 1; j < n_atoms; j++)
    {
    for (k = 0; k < n_atoms; k++)
      {
      if ((axis = static_try_Sn (symmetry, i, j, k)) != NULL)
        {
        symmetry->n_S++;
        symmetry->S = (gamgi_element **) realloc (symmetry->S,
        sizeof (gamgi_element *) * symmetry->n_S);
        symmetry->S[symmetry->n_S - 1] = axis;
        }
      }
    }
  }
}

static void static_find (gamgi_symmetry *symmetry)
{
static_center (symmetry);
static_find_i (symmetry);
static_find_m (symmetry);
static_find_C0 (symmetry);
static_find_C2 (symmetry);
static_find_Cn (symmetry);
static_find_S0 (symmetry);
static_find_Sn (symmetry);
}

static int static_compare (const void *a, const void *b)
{
gamgi_element *axis_a = *(gamgi_element **) a;
gamgi_element *axis_b = *(gamgi_element **) b;
int i, order_a, order_b;

/*******************************************************
 * elements are swaped if the return value is positive *
 *******************************************************/

/*****************************************
 * align first element with higher order *
 * the highest order is infinite (0)     *
 * order_a and order_b cannot be both 0  *
 *****************************************/

order_a = axis_a->order;
order_b = axis_b->order;
if (order_a == 0) order_a = order_b + 1;
if (order_b == 0) order_b = order_a + 1;

if ((i = order_b - order_a) != 0) return i;

/******************************************************
 * same order: align first element with smaller error *
 ******************************************************/

if (axis_a->error > axis_b->error) return 1;

return -1;
}

static void static_sort (gamgi_element **elements, int n_elements)
{
if (n_elements > 1)
  qsort (elements, n_elements, sizeof (gamgi_element *), static_compare);
}

static void static_max_m (double *direction, gamgi_element **m, int n_m)
{
gamgi_element *element;
double dot, dot_max;
int n, n_max;

n_max = -1;
dot_max = -DBL_MAX;
for (n = 0; n < n_m; n++)
  {
  dot = gamgi_math_vector_dot (direction, m[n]->normal);
  dot = fabs (dot);
  if (dot > dot_max ) { dot_max = dot; n_max = n; }
  }

element = m[n_max];
for (n = 0; n < n_max; n++) m[n_max - n] = m[n_max - n - 1];
m[0] = element;
}

static void static_max_C2 (double *direction, gamgi_symmetry *symmetry)
{
gamgi_element *element;
double dot, dot_max;
int n, n_max, start;

for (n = 0; n < symmetry->n_C; n++)
  if (symmetry->C[n]->order == 2) break;
start = n;

n_max = -1;
dot_max = -DBL_MAX;
for (n = start; n < symmetry->n_C; n++)
  {
  if (symmetry->C[n]->order > 2) continue;
  dot = gamgi_math_vector_dot (direction, symmetry->C[n]->direction);
  dot = fabs (dot);
  if (dot > dot_max ) { dot_max = dot; n_max = n; }
  }

element = symmetry->C[n_max];
for (n = start; n < n_max; n++)
  symmetry->C[start + n_max - n] = symmetry->C[start + n_max - n - 1];
symmetry->C[start] = element;
}

static void static_min_C2 (double *direction, gamgi_element **C, int n_C)
{
gamgi_element *element;
double dot, dot_min;
int n, n_min;

n_min = -1;
dot_min = DBL_MAX;
for (n = 0;  n < n_C; n++)
  {
  dot = gamgi_math_vector_dot (direction, C[n]->direction);
  dot = fabs (dot);
  if (dot < dot_min ) { dot_min = dot; n_min = n; }
  }

element = C[n_min];
for (n = 0; n < n_min; n++) C[n_min - n] = C[n_min - n - 1];
C[0] = element;
}

static void static_sum (gamgi_element **elements, int n_elements, int **sum)
{
int n, max = 0;

/*********************************************
 * in the D_0h group, elements[0]->order = 0 *
 * and elements[1]->order = 2, so the first  *
 * element might not have the highest order  *
 *********************************************/

for (n = 0; n < n_elements; n++)
  if (max < elements[n]->order) max = elements[n]->order;

if (elements != NULL)
  {
  /*************************************************
   * for each order, count number of rotation axes *
   *************************************************/

  max++;
  free (*sum);
  *sum = (int *) malloc (sizeof (int)  * max);
  for (n = 0; n < max; n++) (*sum)[n] = 0;

  for (n = 0; n < n_elements; n++) (*sum)[elements[n]->order]++;
  }
}

static void static_operations (GtkTextBuffer *buffer, 
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
int n, operations;
int *sum_C = symmetry->sum_C;
int *sum_S = symmetry->sum_S;

/************
 * Identity *
 ************/

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "\nE ");

/********************
 * inversion center *
 ********************/

if (symmetry->n_i == 1)
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "i ");

/**************************************
 * normal axes: show in reverse order *
 **************************************/

if (symmetry->n_C != 0)
  {
  if (sum_C[0] == 1)
    {
    gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "2C0 ");

    if (symmetry->n_C > 1)
      gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "C2 ");
    }
  else
    {
    for (n = symmetry->C[0]->order; n > 1; n--)
      {
      operations = sum_C[n] * static_multiplicity (n);
      if (operations == 1)
        gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "C%d ", n);
      if (operations > 1)
        gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%dC%d ", 
        operations, n);
      }
    }
  }

/****************************************
 * improper axes: show in reverse order *
 ****************************************/

if (symmetry->n_S != 0)
  {
  if (sum_S[0] == 1)
    gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "2S0 ");
  else
    {
    for (n = symmetry->S[0]->order; n > 1; n--)
      {
      operations = sum_S[n] * static_multiplicity (n);
      if (operations == 1)
        gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "S%d ", n);
      if (operations > 1)
        gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%dS%d ", 
        operations, n);
      }
    }
  }

/*****************
 * mirror planes *
 *****************/

if (symmetry->n_m == 1)
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "m ");
if (symmetry->n_m > 1)
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%dm ", 
  symmetry->n_m);

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "\n");
}

static void static_elements_i (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
int n, n_i = symmetry->n_i;
gamgi_element **i = symmetry->i;

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_link", "\nInversion center: ");
gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d\n", n_i);

for (n = 0; n < n_i; n++)
  {
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", "Position: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text",
  "(%.*f, %.*f, %.*f)", gamgi->gamgi->length, symmetry->center[0],
  gamgi->gamgi->length, symmetry->center[1],
  gamgi->gamgi->length, symmetry->center[2]);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Error: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text",
  "%.*e\n", gamgi->gamgi->length, i[n]->error);
  }
}

static void static_elements_m (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
int n, n_m = symmetry->n_m;
gamgi_element **m = symmetry->m;

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_link", "\nMirror planes: ");
gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d\n", n_m);

for (n = 0; n < n_m; n++)
  {
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", "Normal: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", 
  "(%.*f, %.*f, %.*f)", gamgi->gamgi->length, m[n]->normal[0], 
  gamgi->gamgi->length, m[n]->normal[1], 
  gamgi->gamgi->length, m[n]->normal[2]);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Error: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", 
  "%.*e\n", gamgi->gamgi->length, m[n]->error);
  }
}

static void static_elements_C (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
int n, n_C = symmetry->n_C;
gamgi_element **C = symmetry->C;

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_link", "\nNormal axes: ");
gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d\n", n_C);

for (n = 0; n < n_C; n++)
  {
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", "Order: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d", C[n]->order);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Direction: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", 
  "(%.*f, %.*f, %.*f)", gamgi->gamgi->length, C[n]->direction[0], 
  gamgi->gamgi->length, C[n]->direction[1], 
  gamgi->gamgi->length, C[n]->direction[2]);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Error: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text",
  "%.*e\n", gamgi->gamgi->length, C[n]->error);
  }
}

static void static_elements_S (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
int n, n_S = symmetry->n_S;
gamgi_element **S = symmetry->S;

gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_link", "\nImproper axes: ");
gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d\n", n_S);

for (n = 0; n < n_S; n++)
  {
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", "Order: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text", "%d", S[n]->order);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Direction: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text",
  "(%.*f, %.*f, %.*f)", gamgi->gamgi->length, S[n]->direction[0],
  gamgi->gamgi->length, S[n]->direction[1],
  gamgi->gamgi->length, S[n]->direction[2]);

  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_bold", " Error: ");
  gamgi_gtk_dialog_text_insert (buffer, iter, "size_normal", "color_text",
  "%.*e\n", gamgi->gamgi->length, S[n]->error);
  }
}

static void static_elements (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry)
{
static_elements_i (buffer, iter, symmetry);
static_elements_m (buffer, iter, symmetry);
static_elements_C (buffer, iter, symmetry);
static_elements_S (buffer, iter, symmetry);
}

static void static_report (char *group, gamgi_symmetry *symmetry)
{
GtkTextBuffer *buffer;
GtkTextIter iter;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (symmetry->text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

/***************
 * report data *
 ***************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPoint Group\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "\n%s\n", group);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nSymmetry Operations\n");
static_operations (buffer, &iter, symmetry);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nSymmetry Elements\n");
static_elements (buffer, &iter, symmetry);
}

/*******************************************
 * Chemical to Crystalographic conversions *
 *******************************************/

static void static_Cs (double *d_m, double e_m, gamgi_symmetry *symmetry)
{
static_delete (&symmetry->i, &symmetry->n_i, 0);
static_delete (&symmetry->m, &symmetry->n_m, 0);
static_delete (&symmetry->C, &symmetry->n_C, 0);
static_delete (&symmetry->S, &symmetry->n_S, 0);

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_m, symmetry->m[0]->normal);
symmetry->m[0]->error = e_m;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_C2 (double *d_C2, double e_C2, gamgi_symmetry *symmetry)
{
static_delete (&symmetry->i, &symmetry->n_i, 0);
static_delete (&symmetry->m, &symmetry->n_m, 0);
static_delete (&symmetry->C, &symmetry->n_C, 0);
static_delete (&symmetry->S, &symmetry->n_S, 0);

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_copy (d_C2, symmetry->C[0]->direction);
gamgi_math_vector_normal (symmetry->C[0]->direction);
symmetry->C[0]->order = 2;
symmetry->C[0]->error = e_C2;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_C3 (double *d_C3, double e_C3, gamgi_symmetry *symmetry)
{
static_delete (&symmetry->i, &symmetry->n_i, 0);
static_delete (&symmetry->m, &symmetry->n_m, 0);
static_delete (&symmetry->C, &symmetry->n_C, 0);
static_delete (&symmetry->S, &symmetry->n_S, 0);

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_copy (d_C3, symmetry->C[0]->direction);
gamgi_math_vector_normal (symmetry->C[0]->direction);
symmetry->C[0]->order = 3;
symmetry->C[0]->error = e_C3;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_C4 (double *d_C4, double *d_C2, double e_C4, double e_C2, 
gamgi_symmetry *symmetry)
{
static_C2 (d_C2, e_C2, symmetry);

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_copy (d_C4, symmetry->C[0]->direction);
gamgi_math_vector_normal (symmetry->C[0]->direction);
symmetry->C[0]->order = 4;
symmetry->C[0]->error = e_C4;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
}

static void static_C6 (double *d_C6, double *d_C3, double *d_C2, double e_C6, 
double e_C3, double e_C2, gamgi_symmetry *symmetry)
{
static_C3 (d_C3, e_C3, symmetry);

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_copy (d_C6, symmetry->C[0]->direction);
gamgi_math_vector_normal (symmetry->C[0]->direction);
symmetry->C[0]->order = 6;
symmetry->C[0]->error = e_C6;

static_insert (&symmetry->C, &symmetry->n_C, 2);
gamgi_math_vector_copy (d_C2, symmetry->C[2]->direction);
gamgi_math_vector_normal (symmetry->C[2]->direction);
symmetry->C[2]->order = 2;
symmetry->C[2]->error = e_C2;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
}

static void static_S4 (double *d_S4, double *d_C2, double e_S4, double e_C2, 
gamgi_symmetry *symmetry)
{
static_C2 (d_C2, e_C2, symmetry);

static_insert (&symmetry->S, &symmetry->n_S, 0);
gamgi_math_vector_copy (d_S4, symmetry->S[0]->direction);
gamgi_math_vector_normal (symmetry->S[0]->direction);
symmetry->S[0]->order = 4;
symmetry->S[0]->error = e_S4;

static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_S6 (double *d_S6, double *d_C3, double e_S6, double e_C3, 
double e_i, gamgi_symmetry *symmetry)
{
static_C3 (d_C3, e_C3, symmetry);

static_insert (&symmetry->S, &symmetry->n_S, 0);
gamgi_math_vector_copy (d_S6, symmetry->S[0]->direction);
gamgi_math_vector_normal (symmetry->S[0]->direction);
symmetry->S[0]->order = 6;
symmetry->S[0]->error = e_S6;

static_insert (&symmetry->i, &symmetry->n_i, 0);
symmetry->i[0]->order = 0;
symmetry->i[0]->error = e_i;

static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_D2 (double *d_C2, double *d_Ch, double e_C2, double e_Ch, 
gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C2 (d_C2, e_C2, symmetry);

static_insert (&symmetry->C, &symmetry->n_C, 1);
gamgi_math_vector_copy (d_Ch, symmetry->C[1]->direction);
gamgi_math_vector_normal (symmetry->C[1]->direction);
symmetry->C[1]->order = 2;
symmetry->C[1]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 1, 2);

direction = symmetry->C[0]->direction;
normal = symmetry->C[1]->direction;
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[2]->direction);

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
}

static void static_D3 (double *d_C3, double *d_Ch, double e_C3, double e_Ch, 
gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C3 (d_C3, e_C3, symmetry);

static_insert (&symmetry->C, &symmetry->n_C, 1);
gamgi_math_vector_copy (d_Ch, symmetry->C[1]->direction);
gamgi_math_vector_normal (symmetry->C[1]->direction);
symmetry->C[1]->order = 2;
symmetry->C[1]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 1, 2);
static_insert_copy (&symmetry->C, &symmetry->n_C, 1, 3);

direction = symmetry->C[0]->direction;
normal = symmetry->C[1]->direction;
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[2]->direction);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[3]->direction);

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
}

static void static_D4 (double *d_C4, double *d_C2, double *d_Ch, double e_C4, 
double e_C2, double e_Ch, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C4 (d_C4, d_C2, e_C4, e_C2, symmetry);
 
static_insert (&symmetry->C, &symmetry->n_C, 2);
gamgi_math_vector_copy (d_Ch, symmetry->C[2]->direction);
gamgi_math_vector_normal (symmetry->C[2]->direction);
symmetry->C[2]->order = 2;
symmetry->C[2]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 3);
static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 4);
static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 5);

direction = symmetry->C[0]->direction;
normal = symmetry->C[2]->direction;
gamgi_math_matrix_rotation (45.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[3]->direction);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[4]->direction);
gamgi_math_matrix_rotation (135.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[5]->direction);

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
}
 
static void static_D6 (double *d_C6, double *d_C3, double *d_C2, double *d_Ch,
double e_C6, double e_C3, double e_C2, double e_Ch, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);

static_insert (&symmetry->C, &symmetry->n_C, 3);
gamgi_math_vector_copy (d_Ch, symmetry->C[3]->direction);
gamgi_math_vector_normal (symmetry->C[3]->direction);
symmetry->C[3]->order = 2;
symmetry->C[3]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 4);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 5);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 6);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 7);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 8);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

direction = symmetry->C[0]->direction;
normal = symmetry->C[3]->direction;
gamgi_math_matrix_rotation (30.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[4]->direction);
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[5]->direction);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[6]->direction);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[7]->direction);
gamgi_math_matrix_rotation (150.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[8]->direction);
}

static void static_C3v (double *d_C3, double *d_m, double e_C3, double e_m, 
gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C3 (d_C3, e_C3, symmetry);
 
static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_m, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_m;

static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 1);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 2);

direction = symmetry->C[0]->direction;
normal = symmetry->m[0]->normal;
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[1]->normal);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[2]->normal);
}

static void static_C4v (double *d_C4, double *d_C2, double *d_m, double e_C4, 
double e_C2, double e_m, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C4 (d_C4, d_C2, e_C4, e_C2, symmetry);

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_m, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_m;

static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 1);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 2);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 3);

direction = symmetry->C[0]->direction;
normal = symmetry->m[0]->normal;
gamgi_math_matrix_rotation (45.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[1]->normal);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[2]->normal);
gamgi_math_matrix_rotation (1350.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[3]->normal);
}

static void static_C6v (double *d_C6, double *d_C3, double *d_C2, double *d_m,
double e_C6, double e_C3, double e_C2, double e_m, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_m, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_m;

static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 1);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 2);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 3);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 4);
static_insert_copy (&symmetry->m, &symmetry->n_m, 0, 5);

direction = symmetry->C[0]->direction;
normal = symmetry->m[0]->normal;
gamgi_math_matrix_rotation (30.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[1]->normal);
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[2]->normal);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[3]->normal);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[4]->normal);
gamgi_math_matrix_rotation (150.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[5]->normal);
}

static void static_C2h (double *d_C2, double *d_mh, double e_C2, double e_i, 
double e_mh, gamgi_symmetry *symmetry)
{
static_C2 (d_C2, e_C2, symmetry);

static_insert (&symmetry->i, &symmetry->n_i, 0);
symmetry->i[0]->order = 0;
symmetry->i[0]->error = e_i;

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_mh, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_mh;
}

static void static_C4h (double *d_C4, double *d_C2, double *d_S4, double *d_mh, 
double e_C4, double e_C2, double e_S4, double e_i, double e_mh, 
gamgi_symmetry *symmetry)
{
static_C4 (d_C4, d_C2, e_C4, e_C2, symmetry);

static_insert (&symmetry->S, &symmetry->n_S, 0);
gamgi_math_vector_copy (d_S4, symmetry->S[0]->direction);
gamgi_math_vector_normal (symmetry->S[0]->direction);
symmetry->S[0]->order = 4;
symmetry->S[0]->error = e_S4;

static_insert (&symmetry->i, &symmetry->n_i, 0);
symmetry->i[0]->order = 0;
symmetry->i[0]->error = e_i;

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_mh, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_mh;

static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_C6h (double *d_C6, double *d_C3, double *d_C2, double *d_S6,
double *d_S3, double *d_mh, double e_C6, double e_C3, double e_C2, double e_S6, 
double e_S3, double e_i, double e_mh, gamgi_symmetry *symmetry)
{
static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);

static_insert (&symmetry->S, &symmetry->n_S, 0);
gamgi_math_vector_copy (d_S6, symmetry->S[0]->direction);
gamgi_math_vector_normal (symmetry->S[0]->direction);
symmetry->S[0]->order = 6;
symmetry->S[0]->error = e_S6;

static_insert (&symmetry->S, &symmetry->n_S, 1);
gamgi_math_vector_copy (d_S3, symmetry->S[1]->direction);
gamgi_math_vector_normal (symmetry->S[1]->direction);
symmetry->S[1]->order = 3;
symmetry->S[1]->error = e_S3;

static_insert (&symmetry->i, &symmetry->n_i, 0);
symmetry->i[0]->order = 0;
symmetry->i[0]->error = e_i;

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_copy (d_mh, symmetry->m[0]->normal);
gamgi_math_vector_normal (symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_mh;

static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_D4h (double *d_C4, double *d_C2, double *d_Ch, double *d_S4,
double *d_mh, double *d_m, double e_C4, double e_C2, double e_Ch, double e_S4, 
double e_i, double e_mh, double e_m, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C4h (d_C4, d_C2, d_S4, d_mh, e_C4, e_C2, e_S4, e_i, e_mh, symmetry);

/***********************
 * add horizontal axes *
 ***********************/

static_insert (&symmetry->C, &symmetry->n_C, 2);
gamgi_math_vector_copy (d_Ch, symmetry->C[2]->direction);
gamgi_math_vector_normal (symmetry->C[2]->direction);
symmetry->C[2]->order = 2;
symmetry->C[2]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 3);
static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 4);
static_insert_copy (&symmetry->C, &symmetry->n_C, 2, 5);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

direction = symmetry->C[0]->direction;
normal = symmetry->C[2]->direction;
gamgi_math_matrix_rotation (45.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[3]->direction);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[4]->direction);
gamgi_math_matrix_rotation (135.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[5]->direction);

/***********************
 * add vertical planes *
 ***********************/

static_insert (&symmetry->m, &symmetry->n_m, 1);
gamgi_math_vector_copy (d_m, symmetry->m[1]->normal);
gamgi_math_vector_normal (symmetry->m[1]->normal);
symmetry->m[1]->order = 0;
symmetry->m[1]->error = e_m;

static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 2);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 3);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 4);

direction = symmetry->C[0]->direction;
normal = symmetry->m[1]->normal;
gamgi_math_matrix_rotation (45.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[2]->normal);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[3]->normal);
gamgi_math_matrix_rotation (1350.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[4]->normal);
}

static void static_D6h (double *d_C6, double *d_C3, double *d_C2, double *d_Ch, 
double *d_S6, double *d_S3, double *d_mh, double *d_m, double e_C6, double e_C3, 
double e_C2, double e_Ch, double e_S6, double e_S3, double e_i, double e_mh, 
double e_m, gamgi_symmetry *symmetry)
{
double rotate[9];
double *direction, *normal;

static_C6h (d_C6, d_C3, d_C2, d_S6, d_S3, d_mh, 
e_C6, e_C3, e_C2, e_S6, e_S3, e_i, e_mh, symmetry);

/***********************
 * add horizontal axes *
 ***********************/

static_insert (&symmetry->C, &symmetry->n_C, 3);
gamgi_math_vector_copy (d_Ch, symmetry->C[3]->direction);
gamgi_math_vector_normal (symmetry->C[3]->direction);
symmetry->C[3]->order = 2;
symmetry->C[3]->error = e_Ch;

static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 4);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 5);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 6);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 7);
static_insert_copy (&symmetry->C, &symmetry->n_C, 3, 8);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

direction = symmetry->C[0]->direction;
normal = symmetry->C[3]->direction;
gamgi_math_matrix_rotation (30.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[4]->direction);
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[5]->direction);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[6]->direction);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[7]->direction);
gamgi_math_matrix_rotation (150.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->C[8]->direction);

/***********************
 * add vertical planes *
 ***********************/

static_insert (&symmetry->m, &symmetry->n_m, 1);
gamgi_math_vector_copy (d_m, symmetry->m[1]->normal);
gamgi_math_vector_normal (symmetry->m[1]->normal);
symmetry->m[1]->order = 0;
symmetry->m[1]->error = e_m;

static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 2);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 3);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 4);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 5);
static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 6);

direction = symmetry->C[0]->direction;
normal = symmetry->m[1]->normal;
gamgi_math_matrix_rotation (30.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[2]->normal);
gamgi_math_matrix_rotation (60.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[3]->normal);
gamgi_math_matrix_rotation (90.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[4]->normal);
gamgi_math_matrix_rotation (120.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[5]->normal);
gamgi_math_matrix_rotation (150.0, direction, rotate);
gamgi_math_matrix_vector (rotate, normal, symmetry->m[6]->normal);
}

static void static_T (double *rotate, double e_C3, double e_C2, 
gamgi_symmetry *symmetry)
{
double v[3];

static_delete (&symmetry->i, &symmetry->n_i, 0);
static_delete (&symmetry->m, &symmetry->n_m, 0);
static_delete (&symmetry->C, &symmetry->n_C, 0);
static_delete (&symmetry->S, &symmetry->n_S, 0);

/******
 * C3 *
 ******/

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_absolute (v, 1.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[0]->direction);
symmetry->C[0]->order = 3;
symmetry->C[0]->error = e_C3;

static_insert (&symmetry->C, &symmetry->n_C, 1);
gamgi_math_vector_absolute (v, -1.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[1]->direction);
symmetry->C[1]->order = 3;
symmetry->C[1]->error = e_C3;

static_insert (&symmetry->C, &symmetry->n_C, 2);
gamgi_math_vector_absolute (v, 1.0, -1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[2]->direction);
symmetry->C[2]->order = 3;
symmetry->C[2]->error = e_C3;

static_insert (&symmetry->C, &symmetry->n_C, 3);
gamgi_math_vector_absolute (v, -1.0, -1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[3]->direction);
symmetry->C[3]->order = 3;
symmetry->C[3]->error = e_C3;

/******
 * C2 *
 ******/

static_insert (&symmetry->C, &symmetry->n_C, 4);
gamgi_math_vector_absolute (v, 1.0, 0.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[4]->direction);
symmetry->C[4]->order = 2;
symmetry->C[4]->error = e_C2;

static_insert (&symmetry->C, &symmetry->n_C, 5);
gamgi_math_vector_absolute (v, 0.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[5]->direction);
symmetry->C[5]->order = 2;
symmetry->C[5]->error = e_C2;

static_insert (&symmetry->C, &symmetry->n_C, 6);
gamgi_math_vector_absolute (v, 0.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[6]->direction);
symmetry->C[6]->order = 2;
symmetry->C[6]->error = e_C2;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_Th (double *rotate, double e_C3, double e_C2, double e_S6, 
double e_i, double e_m, gamgi_symmetry *symmetry)
{
double v[3];

static_T (rotate, e_C3, e_C2, symmetry);

/*****
 * m *
 *****/

static_insert (&symmetry->m, &symmetry->n_m, 0);
gamgi_math_vector_absolute (v, 1.0, 0.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[0]->normal);
symmetry->m[0]->order = 0;
symmetry->m[0]->error = e_m;

static_insert (&symmetry->m, &symmetry->n_m, 1);
gamgi_math_vector_absolute (v, 0.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[1]->normal);
symmetry->m[1]->order = 0;
symmetry->m[1]->error = e_m;

static_insert (&symmetry->m, &symmetry->n_m, 2);
gamgi_math_vector_absolute (v, 0.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[2]->normal);
symmetry->m[2]->order = 0;
symmetry->m[2]->error = e_m;

/*****
 * i *
 *****/

static_insert (&symmetry->i, &symmetry->n_i, 0);
symmetry->i[0]->order = 0;
symmetry->i[0]->error = e_i;

/******
 * S6 *
 ******/

static_insert (&symmetry->S, &symmetry->n_S, 0);
gamgi_math_vector_absolute (v, 1.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[0]->direction);
symmetry->S[0]->order = 6;
symmetry->S[0]->error = e_S6;

static_insert (&symmetry->S, &symmetry->n_S, 1);
gamgi_math_vector_absolute (v, -1.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[1]->direction);
symmetry->S[1]->order = 6;
symmetry->S[1]->error = e_S6;

static_insert (&symmetry->S, &symmetry->n_S, 2);
gamgi_math_vector_absolute (v, 1.0, -1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[2]->direction);
symmetry->S[2]->order = 6;
symmetry->S[2]->error = e_S6;

static_insert (&symmetry->S, &symmetry->n_S, 3);
gamgi_math_vector_absolute (v, -1.0, -1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[3]->direction);
symmetry->S[3]->order = 6;
symmetry->S[3]->error = e_S6;

static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_Oh (double *rotate, double e_C4, double e_C3, double e_C2, 
double e_Cd, double e_S6, double e_S4, double e_i, double e_m, double e_md, 
gamgi_symmetry *symmetry)
{
double v[3];

static_Th (rotate, e_C3, e_C2, e_S6, e_i, e_m, symmetry);

/******
 * C4 *
 ******/

static_insert (&symmetry->C, &symmetry->n_C, 0);
gamgi_math_vector_absolute (v, 1.0, 0.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[0]->direction);
symmetry->C[0]->order = 4;
symmetry->C[0]->error = e_C4;

static_insert (&symmetry->C, &symmetry->n_C, 1);
gamgi_math_vector_absolute (v, 0.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[1]->direction);
symmetry->C[1]->order = 4;
symmetry->C[1]->error = e_C4;

static_insert (&symmetry->C, &symmetry->n_C, 2);
gamgi_math_vector_absolute (v, 0.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[2]->direction);
symmetry->C[2]->order = 4;
symmetry->C[2]->error = e_C4;

/******
 * C2 *
 ******/

static_insert (&symmetry->C, &symmetry->n_C, 10);
gamgi_math_vector_absolute (v, 1.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[10]->normal);
symmetry->C[10]->order = 2;
symmetry->C[10]->error = e_Cd;

static_insert (&symmetry->C, &symmetry->n_C, 11);
gamgi_math_vector_absolute (v, 1.0, -1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[11]->normal);
symmetry->C[11]->order = 2;
symmetry->C[11]->error = e_Cd;

static_insert (&symmetry->C, &symmetry->n_C, 12);
gamgi_math_vector_absolute (v, 1.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[12]->normal);
symmetry->C[12]->order = 2;
symmetry->C[12]->error = e_Cd;

static_insert (&symmetry->C, &symmetry->n_C, 13);
gamgi_math_vector_absolute (v, 1.0, 0.0, -1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[13]->normal);
symmetry->C[13]->order = 2;
symmetry->C[13]->error = e_Cd;

static_insert (&symmetry->C, &symmetry->n_C, 14);
gamgi_math_vector_absolute (v, 0.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[14]->normal);
symmetry->C[14]->order = 2;
symmetry->C[14]->error = e_Cd;

static_insert (&symmetry->C, &symmetry->n_C, 15);
gamgi_math_vector_absolute (v, 0.0, 1.0, -1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->C[15]->normal);
symmetry->C[15]->order = 2;
symmetry->C[15]->error = e_Cd;

/*****
 * m *
 *****/

static_insert (&symmetry->m, &symmetry->n_m, 3);
gamgi_math_vector_absolute (v, 1.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[3]->normal);
symmetry->m[3]->order = 0;
symmetry->m[3]->error = e_md;

static_insert (&symmetry->m, &symmetry->n_m, 4);
gamgi_math_vector_absolute (v, 1.0, -1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[4]->normal);
symmetry->m[4]->order = 0;
symmetry->m[4]->error = e_md;

static_insert (&symmetry->m, &symmetry->n_m, 5);
gamgi_math_vector_absolute (v, 1.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[5]->normal);
symmetry->m[5]->order = 0;
symmetry->m[5]->error = e_md;

static_insert (&symmetry->m, &symmetry->n_m, 6);
gamgi_math_vector_absolute (v, 1.0, 0.0, -1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[6]->normal);
symmetry->m[6]->order = 0;
symmetry->m[6]->error = e_md;

static_insert (&symmetry->m, &symmetry->n_m, 7);
gamgi_math_vector_absolute (v, 0.0, 1.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[7]->normal);
symmetry->m[7]->order = 0;
symmetry->m[7]->error = e_md;

static_insert (&symmetry->m, &symmetry->n_m, 8);
gamgi_math_vector_absolute (v, 0.0, 1.0, -1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->m[8]->normal);
symmetry->m[8]->order = 0;
symmetry->m[8]->error = e_md;

/******
 * S4 *
 ******/

static_insert (&symmetry->S, &symmetry->n_S, 4);
gamgi_math_vector_absolute (v, 1.0, 0.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[4]->direction);
symmetry->S[4]->order = 4;
symmetry->S[4]->error = e_S4;

static_insert (&symmetry->S, &symmetry->n_S, 5);
gamgi_math_vector_absolute (v, 0.0, 1.0, 0.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[5]->direction);
symmetry->S[5]->order = 4;
symmetry->S[5]->error = e_S4;

static_insert (&symmetry->S, &symmetry->n_S, 6);
gamgi_math_vector_absolute (v, 0.0, 0.0, 1.0);
gamgi_math_vector_normal (v);
gamgi_math_matrix_vector (rotate, v, symmetry->S[6]->direction);
symmetry->S[6]->order = 4;
symmetry->S[6]->error = e_S4;

static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
}

static void static_space_C0v (char *group, gamgi_symmetry *symmetry)
{
double e_C0, e_m;
double d_C0[3], d_m[3];

gamgi_math_vector_copy (symmetry->C[0]->direction, d_C0);
gamgi_math_vector_copy (symmetry->m[0]->normal, d_m);
e_C0 = symmetry->C[0]->error;
e_m = symmetry->m[0]->error;

/*******
 * C4v *
 *******/

static_C4v (d_C0, d_C0, d_m, e_C0, e_C0, e_m, symmetry);
static_group (group, symmetry);
static_report (group, symmetry);

/*******
 * C6v *
 *******/

static_C6v (d_C0, d_C0, d_C0, d_m, e_C0, e_C0, e_C0, e_m, symmetry);
static_group (group, symmetry);
}

static void static_space_D0h (char *group, gamgi_symmetry *symmetry)
{
double d_C0[3], d_Ch[3];
double d_S0[3];
double d_mh[3], d_m[3];
double e_C0, e_Ch;
double e_S0;
double e_mh, e_m;
double e_i;

gamgi_math_vector_copy (symmetry->C[0]->direction, d_C0);
gamgi_math_vector_copy (symmetry->C[1]->direction, d_Ch);
gamgi_math_vector_copy (symmetry->S[0]->direction, d_S0);
gamgi_math_vector_copy (symmetry->m[0]->normal, d_mh);
gamgi_math_vector_copy (symmetry->m[1]->normal, d_m);
e_C0 = symmetry->C[0]->error;
e_Ch = symmetry->C[1]->error;
e_S0 = symmetry->S[0]->error;
e_mh = symmetry->m[0]->error;
e_m = symmetry->m[1]->error;
e_i = symmetry->i[0]->error;

/*******
 * D4h *
 *******/

static_D4h (d_C0, d_C0, d_Ch, d_S0, d_mh, d_m, 
e_C0, e_C0, e_Ch, e_S0, e_i, e_mh, e_m, symmetry);
static_group (group, symmetry);
static_report (group, symmetry);

/*******
 * D6h *
 *******/

static_D6h (d_C0, d_C0, d_C0, d_Ch, d_S0, d_S0, d_mh, d_m, 
e_C0, e_C0, e_C0, e_Ch, e_S0, e_S0, e_i, e_mh, e_m, symmetry);
static_group (group, symmetry);
}

static void static_space_Kh (char *group, gamgi_symmetry *symmetry)
{
double rotate[9];
double d_C0[3];
double d_S0[3];
double d_m[3];

gamgi_math_vector_copy (symmetry->C[0]->direction, d_C0);
gamgi_math_vector_copy (symmetry->S[0]->direction, d_S0);
gamgi_math_vector_copy (symmetry->m[0]->normal, d_m);

/*******
 * D4h *
 *******/

static_D4h (d_C0, d_C0, d_m, d_S0, d_C0, d_m,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, symmetry);
static_group (group, symmetry);
static_report (group, symmetry);

/*******
 * D6h *
 *******/

static_D6h (d_C0, d_C0, d_C0, d_m, d_S0, d_S0, d_C0, d_m,
0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, symmetry);
static_group (group, symmetry);
static_report (group, symmetry);

/*******
 * Oh *
 *******/

gamgi_math_matrix_unit (rotate);
static_Oh (rotate, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, symmetry);
static_group (group, symmetry);
}

static void static_space_Cn (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3];
double e_C6, e_C4, e_C3, e_C2;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

if (symmetry->n_C == 4)
  {
  /******************************
   * C6, C4, C3, C2 axes exist: *
   * the group can be C4 or C6  *
   ******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;

  /******
   * C4 *
   ******/

  static_C4 (d_C4, d_C2, e_C4, e_C2, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);
  
  /******
   * C6 *
   ******/

  static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);
  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Dn (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3], d_Ch[3];
double e_C6, e_C4, e_C3, e_C2, e_Ch;
double rotate[9];
double *normal, *direction;
int n, h, order_C;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

order_C = symmetry->C[0]->order;
static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

/**************************
 * remove horizontal axes *
 **************************/

for (n = 0; n < symmetry->n_C; n++)
  if (symmetry->C[n]->order == 2) break;
h = n;
if (order_C % 2 == 0) h++;

symmetry->C[h]->error = symmetry->C[symmetry->n_C - 1]->error;
static_delete (&symmetry->C, &symmetry->n_C, h + 1);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

gamgi_math_vector_copy (symmetry->C[h]->direction, d_Ch);
e_Ch = symmetry->C[h]->error;

if (symmetry->n_C == 5)
  {
  /******************************
   * C6, C4, C3, C2 axes exist: *
   * the group can be D4 or D6  *
   ******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;

  /******
   * D4 *
   ******/

  static_D4 (d_C4, d_C2, d_Ch, e_C4, e_C2, e_Ch, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D6 *
   ******/

  static_D6 (d_C6, d_C3, d_C2, d_Ch, e_C6, e_C3, e_C2, e_Ch, symmetry);
  }
else if (h > 0)
  {
  /*******************
   * horizontal axes *
   *******************/

  order_C = symmetry->C[0]->order;
  for (n = 1; n < order_C; n++)
    static_insert_copy (&symmetry->C, &symmetry->n_C, h, h + n);
  static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
  direction = symmetry->C[0]->direction;
  normal = symmetry->C[h]->direction;
  for (n = 1; n < order_C; n++)
    {
    gamgi_math_matrix_rotation (n * 180.0 / order_C, direction, rotate);
    gamgi_math_matrix_vector (rotate, normal, 
    symmetry->C[h + n]->direction);
    }
  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Sn (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3];
double d_S4[3];
double e_C6, e_C4, e_C3, e_C2;
double e_S4;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_restrict (&symmetry->S, &symmetry->n_S);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

if (symmetry->n_C == 4)
  {
  /******************************
   * C6, C4, C3, C2 axes exist: *
   * the group can be C4 or C6  *
   ******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;

  /******
   * C4 *
   ******/

  static_C4 (d_C4, d_C2, e_C4, e_C2, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * C6 *
   ******/

  static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);
  }
else if (symmetry->n_C + symmetry->n_S == 4)
  {
  /******************************
   * C6, C3, C2, S4 axes exist: *
   * the group can be S4 or C6  *
   ******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C2);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S4);
  e_C6 = symmetry->C[0]->error;
  e_C3 = symmetry->C[1]->error;
  e_C2 = symmetry->C[2]->error;
  e_S4 = symmetry->S[0]->error;

  /******
   * S4 *
   ******/

  static_S4 (d_S4, d_C2, e_S4, e_C2, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * C6 *
   ******/

  static_C6 (d_C6, d_C3, d_C2, e_C6, e_C3, e_C2, symmetry);
  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Cnv (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3];
double d_m[3];
double e_C6, e_C4, e_C3, e_C2;
double e_m;
double rotate[9];
double *normal, *direction;
int n, order_C;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

/*********************************************************
 * horizontal planes and axes were previously sorted out *
 * according to its error, so the element of symmetry    *
 * identified with a highest degree of confidence comes  *
 * first. When recreating automatically a subset of the  *
 * horizontal elements, give them the geometry of the    *
 * first element and the error of the last element.      *
 *********************************************************/

symmetry->m[0]->error = symmetry->m[symmetry->n_m - 1]->error;
static_delete (&symmetry->m, &symmetry->n_m, 1);

gamgi_math_vector_copy (symmetry->m[0]->normal, d_m);
e_m = symmetry->m[0]->error;

if (symmetry->n_C == 4)
  {
  /*******************************
   * C6, C4, C3, C2 axes exist:  *
   * the group can be C4v or C6v *
   *******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;

  /*******
   * C4v *
   *******/

  static_C4v (d_C4, d_C2, d_m, e_C4, e_C2, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /*******
   * C6v *
   *******/

  static_C6v (d_C6, d_C3, d_C2, d_m, e_C6, e_C3, e_C2, e_m, symmetry);
  }
else
  {
  /*******************
   * vertical planes *
   *******************/

  if (symmetry->n_C > 0)
    { 
    order_C = symmetry->C[0]->order;
    for (n = 1; n < order_C; n++)
      static_insert_copy (&symmetry->m, &symmetry->n_m, 0, n);
    direction = symmetry->C[0]->direction;
    normal = symmetry->m[0]->normal;
    for (n = 1; n < order_C; n++)
      {
      gamgi_math_matrix_rotation (n * 180.0 / order_C, direction, rotate);
      gamgi_math_matrix_vector (rotate, normal, symmetry->m[n]->normal);
      }
    }

  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Cnh (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3];
double d_S6[3], d_S4[3], d_S3[3];
double d_mh[3];
double e_C6, e_C4, e_C3, e_C2;
double e_S6, e_S4, e_S3;
double e_mh;
double e_i;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_restrict (&symmetry->S, &symmetry->n_S);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

gamgi_math_vector_copy (symmetry->m[0]->normal, d_mh);
e_mh = symmetry->m[0]->error;

if (symmetry->n_C == 4)
  {
  /*******************************
   * C6, C4, C3, C2 axes exist:  *
   * the group can be C4h or C6h *
   *******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S6);
  gamgi_math_vector_copy (symmetry->S[1]->direction, d_S4);
  gamgi_math_vector_copy (symmetry->S[2]->direction, d_S3);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;
  e_S6 = symmetry->S[0]->error;
  e_S4 = symmetry->S[1]->error;
  e_S3 = symmetry->S[2]->error;
  e_i = symmetry->i[0]->error;

  /*******
   * C4h *
   *******/

  static_C4h (d_C4, d_C2, d_S4, d_mh, e_C4, e_C2, e_S4, e_i, e_mh, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /*******
   * C6h *
   *******/

  static_C6h (d_C6, d_C3, d_C2, d_S6, d_S3, d_mh,
  e_C6, e_C3, e_C2, e_S6, e_S3, e_i, e_mh, symmetry);
  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Dnh (char *group, gamgi_symmetry *symmetry)
{
double rotate[9];
double *normal, *direction;
double d_C6[3], d_C4[3], d_C3[3], d_C2[3], d_Ch[3];
double d_S6[3], d_S4[3], d_S3[3];
double d_mh[3], d_m[3];
double e_C6, e_C4, e_C3, e_C2, e_Ch;
double e_S6, e_S4, e_S3;
double e_mh, e_m;
double e_i;
int n, h, order_C;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

order_C = symmetry->C[0]->order;
static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_restrict (&symmetry->S, &symmetry->n_S);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

/*********************************************************
 * horizontal planes and axes were previously sorted out *
 * according to its error, so the element of symmetry    *
 * identified with a highest degree of confidence comes  *
 * first. When recreating automatically a subset of the  *
 * horizontal elements, give them the geometry of the    *
 * first element and the error of the last element.      *
 *********************************************************/

/**************************
 * remove horizontal axes *
 **************************/

h = symmetry->n_C - symmetry->n_m + 1;
symmetry->C[h]->error = symmetry->C[symmetry->n_C - 1]->error;
static_delete (&symmetry->C, &symmetry->n_C, h + 1);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

/**************************
 * remove vertical planes *
 **************************/

symmetry->m[1]->error = symmetry->m[symmetry->n_m - 1]->error;
static_delete (&symmetry->m, &symmetry->n_m, 2);

gamgi_math_vector_copy (symmetry->C[h]->direction, d_Ch);
gamgi_math_vector_copy (symmetry->m[0]->normal, d_mh);
gamgi_math_vector_copy (symmetry->m[1]->normal, d_m);
e_Ch = symmetry->C[h]->error;
e_mh = symmetry->m[0]->error;
e_m = symmetry->m[1]->error;

if (symmetry->n_C == 5)
  {
  /*******************************
   * C6, C4, C3, C2 axes exist:  *
   * the group can be D4h or D6h *
   *******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S6);
  gamgi_math_vector_copy (symmetry->S[1]->direction, d_S4);
  gamgi_math_vector_copy (symmetry->S[2]->direction, d_S3);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;
  e_S6 = symmetry->S[0]->error;
  e_S4 = symmetry->S[1]->error;
  e_S3 = symmetry->S[2]->error;
  e_i = symmetry->i[0]->error;

  /*******
   * D4h *
   *******/

  static_D4h (d_C4, d_C2, d_Ch, d_S4, d_mh, d_m,
  e_C4, e_C2, e_Ch, e_S4, e_i, e_mh, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /*******
   * D6h *
   *******/

  static_D6h (d_C6, d_C3, d_C2, d_Ch, d_S6, d_S3, d_mh, d_m,
  e_C6, e_C3, e_C2, e_Ch, e_S6, e_S3, e_i, e_mh, e_m, symmetry);
  }

else if (h > 0)
  {
  /*******************
   * horizontal axes *
   *******************/

  order_C = symmetry->C[0]->order;
  for (n = 1; n < order_C; n++)
    static_insert_copy (&symmetry->C, &symmetry->n_C, h, h + n);
  static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
  direction = symmetry->C[0]->direction;
  normal = symmetry->C[h]->direction;
  for (n = 1; n < order_C; n++)
    {
    gamgi_math_matrix_rotation (n * 180.0 / order_C, direction, rotate);
    gamgi_math_matrix_vector (rotate, normal,
    symmetry->C[h + n]->direction);
    }

    /*******************
     * vertical planes *
     *******************/

  for (n = 1; n < order_C; n++)
    static_insert_copy (&symmetry->m, &symmetry->n_m, 1, 1 + n);
  normal = symmetry->m[1]->normal;
  for (n = 1; n < order_C; n++)
    {
    gamgi_math_matrix_rotation (n * 180.0 / order_C, direction, rotate);
    gamgi_math_matrix_vector (rotate, normal, symmetry->m[1 + n]->normal);
    }

  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_Dnd (char *group, gamgi_symmetry *symmetry)
{
double d_C6[3], d_C4[3], d_C3[3], d_C2[3], d_Ch[3];
double d_S6[3], d_S4[3];
double d_m[3];
double e_C6, e_C4, e_C3, e_C2, e_Ch;
double e_S6, e_S4;
double e_m, e_i;
int h;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_restrict (&symmetry->S, &symmetry->n_S);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

/*********************************************************
 *         h signals the first horizontal axis           *
 *                                                       *
 * horizontal planes and axes were previously sorted out *
 * according to its error, so the element of symmetry    *
 * identified with a highest degree of confidence comes  *
 * first. When recreating automatically a subset of the  *
 * horizontal elements, give them the geometry of the    *
 * first element and the error of the last element.      *
 *********************************************************/

h = symmetry->n_C - symmetry->n_m;
gamgi_math_vector_copy (symmetry->C[h]->direction, d_Ch);
gamgi_math_vector_copy (symmetry->m[0]->normal, d_m);
e_m = symmetry->m[symmetry->n_m - 1]->error;
e_Ch = symmetry->C[symmetry->n_C - 1]->error;

/**********************************************
 * C2h is always a subgroup of odd Dnd groups *
 * D2 is always a subgroup of even Dnd groups *
 **********************************************/

if (symmetry->n_m % 6 == 0 && symmetry->n_m % 4 == 0)
  {
  /***************************************
   * C6, C4, C3, C2 axes exist:          *
   * the group can be C4v, C6v, D4 or D6 *
   ***************************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[3]->direction, d_C2);
  e_C6 = symmetry->C[0]->error;
  e_C4 = symmetry->C[1]->error;
  e_C3 = symmetry->C[2]->error;
  e_C2 = symmetry->C[3]->error;

  /*******
   * C4v *
   *******/

  static_C4v (d_C4, d_C2, d_m, e_C4, e_C2, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /*******
   * C6v *
   *******/

  static_C6v (d_C6, d_C3, d_C2, d_m, e_C6, e_C3, e_C2, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D4 *
   ******/

  static_D4 (d_C4, d_C2, d_Ch, e_C4, e_C2, e_Ch, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D6 *
   ******/

  static_D6 (d_C6, d_C3, d_C2, d_Ch, e_C6, e_C3, e_C2, e_Ch, symmetry);
  static_group (group, symmetry);
  }
else if (symmetry->n_m % 6 == 0 && symmetry->n_m % 4 != 0)
  {
  /**********************************
   * C6, S4, C3, C2 axes exist:     *
   * the group can be C6v, D6 or S4 *
   **********************************/
  
  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C6);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->C[2]->direction, d_C2);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S4);
  e_C6 = symmetry->C[0]->error;
  e_C3 = symmetry->C[1]->error;
  e_C2 = symmetry->C[2]->error;
  e_S4 = symmetry->S[0]->error;

  /*******
   * C6v *
   *******/

  static_C6v (d_C6, d_C3, d_C2, d_m, e_C6, e_C3, e_C2, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D6 *
   ******/

  static_D6 (d_C6, d_C3, d_C2, d_Ch, e_C6, e_C3, e_C2, e_Ch, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * S4 *
   ******/

  static_S4 (d_S4, d_C2, e_S4, e_C2, symmetry);
  }
else if (symmetry->n_m % 6 != 0 && symmetry->n_m % 4 == 0)
  {
  /******************************
   * C4, C2 axes exist:         *
   * the group can be C4v or D4 *
   ******************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C4);
  gamgi_math_vector_copy (symmetry->C[1]->direction, d_C2);
  e_C4 = symmetry->C[0]->error;
  e_C2 = symmetry->C[1]->error;

  /*******
   * C4v *
   *******/

  static_C4v (d_C4, d_C2, d_m, e_C4, e_C2, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D4 *
   ******/

  static_D4 (d_C4, d_C2, d_Ch, e_C4, e_C2, e_Ch, symmetry);
  }
else if (symmetry->n_m % 3 == 0 && symmetry->n_m % 2 != 0)
  {
  /***************************************
   * C3, S6 axes exist:                  *
   * the group can be C2h, C3v, D3 or S6 *
   ***************************************/
  
  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C3);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S6);
  e_C3 = symmetry->C[0]->error;
  e_S6 = symmetry->S[0]->error;
  e_i = symmetry->i[0]->error;

  /*******
   * C2h *
   *******/

  static_C2h (d_Ch, d_Ch, e_Ch, e_i, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /*******
   * C3v *
   *******/

  static_C3v (d_C3, d_m, e_C3, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D3 *
   ******/

  static_D3 (d_C3, d_Ch, e_C3, e_Ch, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * S6 *
   ******/

  static_S6 (d_S6, d_C3, e_S6, e_C3, e_i, symmetry);
  }
else if (symmetry->n_m % 3 != 0 && symmetry->n_m % 2 == 0)
  {
  /*********************************
   * C2, S4 axes exist:            *
   * the group can be Cs, D2 or S4 *
   *********************************/

  gamgi_math_vector_copy (symmetry->C[0]->direction, d_C2);
  gamgi_math_vector_copy (symmetry->S[0]->direction, d_S4);
  e_C2 = symmetry->C[0]->error;
  e_S4 = symmetry->S[0]->error;

  /******
   * Cs *
   ******/

  static_Cs (d_m, e_m, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * D2 *
   ******/

  static_D2 (d_C2, d_Ch, e_C2, e_Ch, symmetry);
  static_group (group, symmetry);
  static_report (group, symmetry);

  /******
   * S4 *
   ******/

  static_S4 (d_S4, d_C2, e_S4, e_C2, symmetry);
  }
else if (symmetry->n_m % 3 != 0 && symmetry->n_m % 2 != 0)
  {
  /************************
   * C2 axis exist:       *
   * the group can be C2h *
   ************************/

  e_i = symmetry->i[0]->error;

  /*******
   * C2h *
   *******/

  static_C2h (d_Ch, d_Ch, e_Ch, e_i, e_m, symmetry);
  }

/**********************
 * show (recursively) *
 **********************/

static_group (group, symmetry);
}

static void static_space_I (char *group, gamgi_symmetry *symmetry)
{
double rotate[9];
double *col1, *col2, *col3;
double e_C3, e_C2;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);

/*********************************************************
 * horizontal planes and axes were previously sorted out *
 * according to its error, so the element of symmetry    *
 * identified with a highest degree of confidence comes  *
 * first. When recreating automatically a subset of the  *
 * element of symmetry, give them the geometry of the    *
 * first element and the error of the last element.      *
 *********************************************************/

e_C3 = symmetry->C[9]->error;
e_C2 = symmetry->C[24]->error;

/*********************************************************
 * set first the best 3 normal C2 axes: these axes       *
 * form the real referential used to rotate the symmetry *
 * elements from the standard to the real orientation    *
 *********************************************************/ 
 
static_min_C2 (symmetry->C[10]->direction, symmetry->C + 11, symmetry->n_C - 11);
static_min_C2 (symmetry->C[10]->direction, symmetry->C + 12, symmetry->n_C - 12);

col1 = symmetry->C[10]->direction;
col2 = symmetry->C[11]->direction;
col3 = symmetry->C[12]->direction;

gamgi_math_matrix_absolute (rotate, 
col1[0], col2[0], col3[0], col1[1], col2[1], col3[1], col1[2], col2[2], col3[2]);

/*****
 * T *
 *****/

static_T (rotate, e_C3, e_C2, symmetry);
static_group (group, symmetry);
}

static void static_space_Ih (char *group, gamgi_symmetry *symmetry)
{
double rotate[9];
double *col1, *col2, *col3;
double e_C3, e_C2;
double e_S6;
double e_i;
double e_m;

/********************************************
 * remove axes when order is not 6, 4, 3, 2 *
 ********************************************/

static_restrict (&symmetry->C, &symmetry->n_C);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_restrict (&symmetry->S, &symmetry->n_S);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

/*********************************************************
 * horizontal planes and axes were previously sorted out *
 * according to its error, so the element of symmetry    *
 * identified with a highest degree of confidence comes  *
 * first. When recreating automatically a subset of the  *
 * element of symmetry, give them the geometry of the    *
 * first element and the error of the last element.      *
 *********************************************************/

e_C3 = symmetry->C[9]->error;
e_C2 = symmetry->C[24]->error;
e_S6 = symmetry->S[9]->error;
e_i = symmetry->i[0]->error;
e_m = symmetry->m[14]->error;

/*********************************************************
 * set first the best 3 normal C2 axes: these axes       *
 * form the real referential used to rotate the symmetry *
 * elements from the standard to the real orientation    *
 *********************************************************/

static_min_C2 (symmetry->C[10]->direction, symmetry->C + 11, symmetry->n_C - 11);
static_min_C2 (symmetry->C[10]->direction, symmetry->C + 12, symmetry->n_C - 12);

col1 = symmetry->C[10]->direction;
col2 = symmetry->C[11]->direction;
col3 = symmetry->C[12]->direction;

gamgi_math_matrix_absolute (rotate,
col1[0], col2[0], col3[0], col1[1], col2[1], col3[1], col1[2], col2[2], col3[2]);

/*******
 * Th *
 *******/

static_Th (rotate, e_C3, e_C2, e_S6, e_i, e_m, symmetry);
static_group (group, symmetry);
}

static void static_chemical_C1 (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_m != 0) return;
if (symmetry->n_S != 0) return;

sprintf (group, "C1");
if (symmetry->periodic == FALSE) return;

sprintf (group, "1 C1");
}

static void static_chemical_Cs (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_S != 0) return;

sprintf (group, "Cs");
if (symmetry->periodic == FALSE) return;

sprintf (group, "m Cs");
}

static void static_chemical_Ci (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_m != 0) return;
if (symmetry->n_S != 0) return;

sprintf (group, "Ci");
if (symmetry->periodic == FALSE) return;

sprintf (group, "-1 Ci");
}

static void static_chemical_C0v (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_m != 1) return;
if (symmetry->n_C != 1) return;
if (symmetry->n_S != 0) return;

sprintf (group, "C0v");
if (symmetry->periodic == FALSE) return;

static_space_C0v (group, symmetry);
}

static void static_chemical_D0h (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 1) return;
if (symmetry->sum_C[0] != 1) return;
if (symmetry->n_C != 2) return;
if (symmetry->C[1]->order != 2) return;
if (symmetry->n_S != 1) return;
if (symmetry->sum_S[0] != 1) return;

sprintf (group, "D0h"); 
if (symmetry->periodic == FALSE) return;

static_space_D0h (group, symmetry);
}

static void static_chemical_Kh (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_m != 1) return;
if (symmetry->n_C != 1) return;
if (symmetry->n_S != 1) return;
if (symmetry->sum_S[0] != 1) return;

sprintf (group, "Kh"); 
if (symmetry->periodic == FALSE) return;

static_space_Kh (group, symmetry);
}

static void static_chemical_Cn (char *group, gamgi_symmetry *symmetry)
{
int n, order_C;

if (symmetry->n_i != 0) return;
n = order_C = symmetry->C[0]->order;
while (n > 1)
  {
  if (order_C % n == 0 && symmetry->sum_C[n] != 1) return;
  if (order_C % n != 0 && symmetry->sum_C[n] != 0) return;
  n--;
  }

sprintf (group, "C%d", order_C);
if (symmetry->periodic == FALSE) return;

sprintf (group, "%d C%d", order_C, order_C);

if (order_C > 6 || order_C == 5) static_space_Cn  (group, symmetry);
}

static void static_chemical_Dn (char *group, gamgi_symmetry *symmetry)
{
int n, order_C;
int *sum_C = symmetry->sum_C;

if (symmetry->n_i != 0) return;
n = order_C = symmetry->C[0]->order;
while (n > 2)
  {
  if (order_C % n == 0 && sum_C[n] != 1) return;
  if (order_C % n != 0 && sum_C[n] != 0) return;
  n--;
  } 
if (order_C / 2 * 2 + 1 != sum_C[2]) return;

static_max_C2 (symmetry->C[0]->direction, symmetry);
sprintf (group, "D%d", order_C);
if (symmetry->periodic == FALSE) return;

if (order_C == 6)
  sprintf (group, "622 D6");
else if (order_C == 4)
  sprintf (group, "422 D4");
else if (order_C == 3)
  sprintf (group, "32 D3");
else
  sprintf (group, "222 D2");

if (order_C > 6 || order_C == 5) static_space_Dn  (group, symmetry);
}

static void static_chemical_Sn (char *group, gamgi_symmetry *symmetry)
{
int n, order_C, order_S;
int *sum_C = symmetry->sum_C;
int *sum_S = symmetry->sum_S;

n = order_C = symmetry->C[0]->order;
if (order_C % 2 == 0 && (sum_C[2] != 1 || symmetry->n_i != 0)) return;
if (order_C % 2 != 0 && (sum_C[2] != 0 || symmetry->n_i != 1)) return;
while (n > 2)
  {
  if (order_C % n == 0 && sum_C[n] != 1) return;
  if (order_C % n != 0 && sum_C[n] != 0) return;
  n--;
  }
n = order_S = symmetry->S[0]->order;
if (order_S != 2 * order_C) return;
if (sum_S[2] != 0) return;
while (n > 2)
  {
  if (order_S % n != 0 && sum_S[n] != 0) return;
  if (order_S % n == 0)
    {
    if (order_S / n % 2 == 0 && sum_S[n] != 0) return;
    if (order_S / n % 2 != 0 && sum_S[n] != 1) return;
    }
  n--;
  }

sprintf (group, "S%d", order_S);
if (symmetry->periodic == FALSE) return;

if (order_S == 6)
  sprintf (group, "-3 S6");
else if (order_S == 4)
  sprintf (group, "-4 S4");

if (order_S > 6) static_space_Sn  (group, symmetry);
}

static void static_chemical_Cnv (char *group, gamgi_symmetry *symmetry)
{
int n, order_C;

if (symmetry->n_i != 0) return;
n = order_C = symmetry->C[0]->order;
if (symmetry->n_m != order_C) return;
while (n > 1)
  {
  if (order_C % n == 0 && symmetry->sum_C[n] != 1) return;
  if (order_C % n != 0 && symmetry->sum_C[n] != 0) return;
  n--;
  }

sprintf (group, "C%dv", order_C);
if (symmetry->periodic == FALSE) return;

if (order_C == 6)
  sprintf (group, "6mm C6v");
else if (order_C == 4)
  sprintf (group, "4mm C4v");
else if (order_C == 3)
  sprintf (group, "3m C3v");
else
  sprintf (group, "mm2 C2v");

if (order_C > 6 || order_C == 5) static_space_Cnv  (group, symmetry);
}

static void static_chemical_Cnh (char *group, gamgi_symmetry *symmetry)
{
int n, order_C, order_S;
int *sum_C = symmetry->sum_C;
int *sum_S = symmetry->sum_S;

n = order_C = symmetry->C[0]->order;
if (order_C % 2 == 0 && symmetry->n_i != 1) return;
if (order_C % 2 != 0 && symmetry->n_i != 0) return;
while (n > 1)
  {
  if (order_C % n == 0 && sum_C[n] != 1) return;
  if (order_C % n != 0 && sum_C[n] != 0) return;
  n--;
  }
if (order_C == 2 && symmetry->n_S != 0) return;
if (order_C > 2)
  {
  if (symmetry->n_S == 0) return;
  n = order_S = symmetry->S[0]->order;
  if (order_S != order_C) return;
  while (n > 2)
    {
    if (order_S % n == 0 && sum_S[n] != 1) return;
    if (order_S % n != 0 && sum_S[n] != 0) return;
    n--;
    }
  }

sprintf (group, "C%dh", order_C);
if (symmetry->periodic == FALSE) return;

if (order_C == 6)
  sprintf (group, "6/m C6h");
else if (order_C == 4)
  sprintf (group, "4/m C4h");
else if (order_C == 3)
  sprintf (group, "-6 C3h");
else
  sprintf (group, "2/m C2h");

if (order_C > 6 || order_C == 5) static_space_Cnh  (group, symmetry);
}

static void static_chemical_Dnh (char *group, gamgi_symmetry *symmetry)
{
int n, order_C, order_S;
int *sum_C = symmetry->sum_C;
int *sum_S = symmetry->sum_S;

n = order_C = symmetry->C[0]->order;
if (order_C % 2 == 0 && symmetry->n_i != 1) return;
if (order_C % 2 != 0 && symmetry->n_i != 0) return;
while (n > 2)
  {
  if (order_C % n == 0 && sum_C[n] != 1) return;
  if (order_C % n != 0 && sum_C[n] != 0) return;
  n--;
  }
if (order_C / 2 * 2 + 1 != sum_C[2]) return;
if (order_C == 2 && symmetry->n_S != 0) return;
if (order_C > 2)
  {
  if (symmetry->n_S == 0) return;
  n = order_S = symmetry->S[0]->order;
  if (order_S != order_C) return;
  while (n > 2)
    {
    if (order_S % n == 0 && sum_S[n] != 1) return;
    if (order_S % n != 0 && sum_S[n] != 0) return;
    n--;
    }
  }

static_max_C2 (symmetry->C[0]->direction, symmetry);
static_max_m (symmetry->C[0]->direction, symmetry->m, symmetry->n_m);
sprintf (group, "D%dh", order_C);
if (symmetry->periodic == FALSE) return;

if (order_C == 6)
  sprintf (group, "6/mmm D6h");
else if (order_C == 4)
  sprintf (group, "4/mmm D4h");
else if (order_C == 3)
  sprintf (group, "-6m2 D3h");
else
  sprintf (group, "mmm D2h");

if (order_C > 6 || order_C == 5) static_space_Dnh  (group, symmetry);
}

static void static_chemical_Dnd (char *group, gamgi_symmetry *symmetry)
{
int order_C, order_S;
int n;
int *sum_C = symmetry->sum_C;
int *sum_S = symmetry->sum_S;

n = order_C = symmetry->C[0]->order;
if (order_C % 2 == 0 && symmetry->n_i != 0) return;
if (order_C % 2 != 0 && symmetry->n_i != 1) return;
if (symmetry->n_m != order_C) return;
if (sum_C[2] != order_C / 2 * 2 + 1) return;
while (n > 2)
  {
  if (order_C % n == 0 && sum_C[n] != 1) return;
  if (order_C % n != 0 && sum_C[n] != 0) return;
  n--;
  }
if (symmetry->n_S == 0) return;
n = order_S = symmetry->S[0]->order;
if (order_S != 2 * order_C) return;
if (sum_S[2] != 0) return;
while (n > 2)
  {
  if (order_S % n != 0 && sum_S[n] != 0) return;
  if (order_S % n == 0)
    {
    if (order_S / n % 2 == 0 && sum_S[n] != 0) return;
    if (order_S / n % 2 != 0 && sum_S[n] != 1) return;
    }
  n--;
  }

static_max_C2 (symmetry->C[0]->direction, symmetry);
sprintf (group, "D%dd", order_C);
if (symmetry->periodic == FALSE) return;

if (order_C == 3)
  sprintf (group, "-3m D3d");
else
  sprintf (group, "-42m D2d");

if (order_C > 3) static_space_Dnd  (group, symmetry);
}

static void static_chemical_T (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_C != 7) return;
if (symmetry->sum_C[2] != 3) return;
if (symmetry->n_S != 0) return;

sprintf (group, "T");
if (symmetry->periodic == FALSE) return;

sprintf (group, "23 T");
}

static void static_chemical_Td (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_m != 6) return;
if (symmetry->n_C != 7) return;
if (symmetry->sum_C[2] != 3) return;
if (symmetry->n_S != 3) return;
if (symmetry->S[0]->order != 4) return;
if (symmetry->sum_S[4] != 3) return;

sprintf (group, "Td");
if (symmetry->periodic == FALSE) return;

sprintf (group, "-43m Td");
}

static void static_chemical_Th (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 1) return;
if (symmetry->n_m != 3) return;
if (symmetry->n_C != 7) return;
if (symmetry->sum_C[2] != 3) return;
if (symmetry->n_S != 4) return;
if (symmetry->S[0]->order != 6) return;
if (symmetry->sum_S[6] != 4) return;

sprintf (group, "Th");
if (symmetry->periodic == FALSE) return;

sprintf (group, "m-3 Th");
}

static void static_chemical_O (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_C != 16) return;
if (symmetry->sum_C[3] != 4) return;
if (symmetry->sum_C[2] != 9) return;
if (symmetry->n_S != 0) return;

sprintf (group, "O");
if (symmetry->periodic == FALSE) return;

sprintf (group, "432 O");
}

static void static_chemical_Oh (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 1) return;
if (symmetry->n_m != 9) return;
if (symmetry->n_C != 16) return;
if (symmetry->sum_C[3] != 4) return;
if (symmetry->sum_C[2] != 9) return;
if (symmetry->n_S != 7) return;
if (symmetry->S[0]->order != 6) return;
if (symmetry->sum_S[6] != 4) return;
if (symmetry->sum_S[4] != 3) return;

sprintf (group, "Oh");
if (symmetry->periodic == FALSE) return;

sprintf (group, "m-3m Oh");
}

static void static_chemical_I (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 0) return;
if (symmetry->n_C != 31) return;
if (symmetry->sum_C[3] != 10) return;
if (symmetry->sum_C[2] != 15) return;
if (symmetry->n_S != 0) return;

sprintf (group, "I");
if (symmetry->periodic == FALSE) return;

static_space_I  (group, symmetry);
}

static void static_chemical_Ih (char *group, gamgi_symmetry *symmetry)
{
if (symmetry->n_i != 1) return;
if (symmetry->n_m != 15) return;
if (symmetry->n_C != 31) return;
if (symmetry->sum_C[3] != 10) return;
if (symmetry->sum_C[2] != 15) return;
if (symmetry->n_S != 16) return;
if (symmetry->S[0]->order != 10) return;
if (symmetry->sum_S[10] != 6) return;
if (symmetry->sum_S[6] != 10) return;

sprintf (group, "Ih");
if (symmetry->periodic == FALSE) return;

static_space_Ih  (group, symmetry);
}

static void static_group (char *group, gamgi_symmetry *symmetry)
{
/***************
 * the default *
 ***************/

sprintf (group, "Undefined");

/************************************************************************
 * Always check that an array position indeed exists before calling it! *
 * We can only take for granted that: 1) if symmetry->n_C != 0 then at  *
 * least one C axis exists; 2) if one C axis exists then symmetry->C[0] *
 * must exist and symmetry->C[0]->order gives the highest order axis    *
 * found; 3) the sum_C[] array must describe all the orders found, so   *
 * all the array positions smaller or equal to the highest order exist. *
 ************************************************************************/

if (symmetry->n_C == 0)
  {
  /************
   * axis: no *
   ************/

  /***********************************
   * low symmetry groups: Cs, Ci, C1 *
   ***********************************/

  if (symmetry->n_m == 1)
    static_chemical_Cs (group, symmetry);
  else if (symmetry->n_i == 1)
    static_chemical_Ci (group, symmetry);
  else
    static_chemical_C1 (group, symmetry);
  }

else if (symmetry->C[0]->order == 0)
  {
  /**********************
   * infinite axis: yes *
   **********************/

  /****************************************************
   * infinite rotation order groups: Dinfh, Kh, Cinfv *
   ****************************************************/
  
  if (symmetry->n_m == 2)
    static_chemical_D0h (group, symmetry);
  else if (symmetry->n_i == 1) 
    static_chemical_Kh (group, symmetry);
  else
    static_chemical_C0v (group, symmetry);
  }

else if (symmetry->n_m == 0)
  {
  /**************
   * planes: no *
   **************/

  /***************************
   * special groups: I, O, T *
   ***************************/

  if (symmetry->C[0]->order == 5 && symmetry->sum_C[5] == 6)
    static_chemical_I (group, symmetry);
  else if (symmetry->C[0]->order == 4 && symmetry->sum_C[4] == 3)
    static_chemical_O (group, symmetry);
  else if (symmetry->C[0]->order == 3 && symmetry->sum_C[3] == 4)
    static_chemical_T (group, symmetry);

  /****************************
   * other groups: Sn, Cn, Dn *
   ****************************/

  else if (symmetry->n_S != 0)
    static_chemical_Sn (group, symmetry);
  else if (static_divisors (symmetry->C[0]->order) == symmetry->n_C)
    static_chemical_Cn (group, symmetry);
  else
    static_chemical_Dn (group, symmetry);
  }
else
  {
  /***************
   * planes: yes *
   ***************/
   
  /**********************************
   * special groups: Ih, Oh, Td, Th *
   **********************************/

  if (symmetry->C[0]->order == 5 && symmetry->sum_C[5] == 6)
    static_chemical_Ih (group, symmetry);
  else if (symmetry->C[0]->order == 4 && symmetry->sum_C[4] == 3)
    static_chemical_Oh (group, symmetry);
  else if (symmetry->C[0]->order == 3 && symmetry->sum_C[3] == 4)
    {
    if (symmetry->n_i == 0)
      static_chemical_Td (group, symmetry);
    else
      static_chemical_Th (group, symmetry);
    }

  /************************************
   * other groups: Cnh, Dnh, Cnv, Dnd *
   ************************************/

  else if (symmetry->n_m == 1)
    static_chemical_Cnh (group, symmetry);
  else if (symmetry->n_m == symmetry->C[0]->order + 1)
    static_chemical_Dnh (group, symmetry);
  else if (symmetry->n_S == 0)
    static_chemical_Cnv (group, symmetry);
  else
    static_chemical_Dnd (group, symmetry);
  }

if (symmetry->periodic == TRUE && strcmp (group, "Undefined") == 0)
  {
  /********************************************
   * remove axes when order is not 6, 4, 3, 2 *
   ********************************************/

  static_restrict (&symmetry->C, &symmetry->n_C);
  static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
  static_restrict (&symmetry->S, &symmetry->n_S);
  static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);
  }
}

static gamgi_symmetry *static_start (gamgi_object *object,
gamgi_bool periodic, float initial, float final, GtkWidget *text)
{
gamgi_symmetry *symmetry;
gamgi_dlist *dlist, *atom_list;
gamgi_atom **atoms;
int n;

/**************************************
 * calloc initializes everything to 0 *
 **************************************/

symmetry = (gamgi_symmetry *) calloc (1, sizeof (gamgi_symmetry));

/*******************************************************
 * set type of point symmetry, text widget, tolerances *
 *******************************************************/

symmetry->periodic = periodic;
symmetry->initial = initial;
symmetry->final = final;
symmetry->text = text;

/*********************
 * create atom array *
 *********************/

atom_list = gamgi_engine_dlist_atom_object (object, NULL);

n = 0;
for (dlist = atom_list; dlist != NULL; dlist = dlist->next) n++;
symmetry->n_atoms = n;

atoms = (gamgi_atom **) malloc (n * sizeof (gamgi_atom *));
symmetry->atoms = atoms;

/*******************
 * fill atom array *
 *******************/

n = 0;
for (dlist = atom_list; dlist != NULL; dlist = dlist->next)
  atoms[n++] = GAMGI_CAST_ATOM dlist->data;

gamgi_engine_dlist_remove_all (atom_list);

return symmetry;
}

static void static_end (gamgi_symmetry *symmetry)
{
int n;

free (symmetry->atoms);
free (symmetry->distance);

n = symmetry->n_i;
while (n-- > 0)
  {
  free (symmetry->i[n]->table);
  free (symmetry->i[n]);
  }
free (symmetry->i);

n = symmetry->n_m;
while (n-- > 0)
  {
  free (symmetry->m[n]->table);
  free (symmetry->m[n]);
  }
free (symmetry->m);

n = symmetry->n_C;
while (n-- > 0)
  {
  free (symmetry->C[n]->table);
  free (symmetry->C[n]);
  }
free (symmetry->C);

n = symmetry->n_S;
while (n-- > 0)
  {
  free (symmetry->S[n]->table);
  free (symmetry->S[n]);
  }
free (symmetry->S);

free (symmetry->sum_C);
free (symmetry->sum_S);

free (symmetry);
}

gamgi_bool gamgi_chem_symmetry (gamgi_object *object, 
gamgi_bool all, float initial, float final, GtkWidget *text)
{
gamgi_symmetry *symmetry;
char token[GAMGI_ENGINE_TOKEN];

/**************************************************
 * start: allocate resources, including atom list *
 **************************************************/

symmetry = static_start (object, all, initial, final, text);

/*****************
 * find elements *
 *****************/

static_find (symmetry);

/*********************
 * organize elements *
 *********************/

static_sort (symmetry->m, symmetry->n_m);
static_sort (symmetry->C, symmetry->n_C);
static_sort (symmetry->S, symmetry->n_S);
static_sum (symmetry->C, symmetry->n_C, &symmetry->sum_C);
static_sum (symmetry->S, symmetry->n_S, &symmetry->sum_S);

/*************
 * get group *
 *************/

static_group (token, symmetry);
static_report (token, symmetry);

/********************************************
 * end: free resources, including atom list *
 ********************************************/

static_end (symmetry);

return TRUE;
}
