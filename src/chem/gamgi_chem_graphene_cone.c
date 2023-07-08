/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene_cone.c
 *
 * Copyright (C) 2016 Carlos Pereira, Joao Coelho
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_link.h"
#include "gamgi_mesa_start.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_graphene.h"
#include "gamgi_io_token.h"

#define TOLERANCE_R 0.1
#define TOLERANCE_T 0.2

typedef int (* atom_function) (double *a, double *b,
double r_in, double r_out, double x, double y,
double *x_new, double *y_new, double *z_new);

typedef void (* bond_function) (gamgi_atom **driver, int max);

static double static_circle (double bond, int rings)
{
double h, v;

h = 0;
if (rings > 0)
  h = (rings - 1) * bond * cos (GAMGI_MATH_PI / 6.0);

v = 0;
if (rings > 0)
  v = bond / 2.0 + bond * sin (GAMGI_MATH_PI / 6.0) +
  (rings - 1) * (bond + bond * sin (GAMGI_MATH_PI / 6.0));

return sqrt (h * h + v * v);
}

static double static_cone (double r_out, double disclination)
{
return (2 * GAMGI_MATH_PI - disclination) * r_out / (2 * GAMGI_MATH_PI);
}

static int static_index (int i, int j, int max)
{
return 2 * (2 * max * (j + max) + i + max);
}

static int static_arc_origin (double x, double y, double r_in, double r_out,
double disclination, double *x_new, double *y_new, double *z_new)
{
double r_cone, f, height;
double d, angle;

/*******************************************************
 * convert angles from 0-360 degrees to 0-2 PI radians *
 *******************************************************/

disclination *= GAMGI_MATH_DEG_RAD;

/********************************
 * remove inner and outer atoms *
 ********************************/

d = sqrt (x * x + y * y);
if (d < r_in || d > r_out) return FALSE;

/************************************
 * calculate cone radius and height *
 ************************************/

r_cone = static_cone (r_out, disclination);
f = r_cone / r_out; if (f > 1.0) f = 1.0;
height = r_out * sqrt (1 - f * f);

/***************************
 * 1 ring, single rotation *
 ***************************/

angle = acos (x / d); if (y < 0) angle = 2 * GAMGI_MATH_PI - angle;
if (angle > disclination)
  {
  angle = 2 * GAMGI_MATH_PI *
  (1 - (2 * GAMGI_MATH_PI - angle) / (2 * GAMGI_MATH_PI - disclination));
  *x_new = d * f * cos (angle);
  *y_new = d * f * sin (angle);
  *z_new = height * (1.0 - d * f / r_cone) - height / 2;
  return TRUE;
  }

return FALSE;
}

static int static_arc (double x, double y, double o_x, double o_y,
double c_x, double c_y, double r_in, double r_out, double disclination,
double bisector, double min, double max, int negative, int positive,
int side, double *x_new, double *y_new, double *z_new)
{
double x_c, y_c, d_c;
double angle, a0, a1;
double d, t0, t1;
double r_cone, f, height;

/*******************************************************
 * convert angles from 0-360 degrees to 0-2 PI radians *
 *******************************************************/

disclination *= GAMGI_MATH_DEG_RAD;
bisector *= GAMGI_MATH_DEG_RAD;
min *= GAMGI_MATH_DEG_RAD;
max *= GAMGI_MATH_DEG_RAD;

/************************
 * change global origin *
 ************************/

x += o_x;
y += o_y;

/********************************
 * remove inner and outer atoms *
 ********************************/

d = sqrt (x * x + y * y);
if (d < r_in || d > r_out) return -1;

/************************************
 * calculate cone radius and height *
 ************************************/

r_cone = static_cone (r_out, disclination);
f = r_cone / r_out; if (f > 1.0) f = 1.0;
height = r_out * sqrt (1 - f * f);

/*********************
 * set default atoms *
 *********************/

*x_new = f * x;
*y_new = f * y;
*z_new = height * (1.0 - d * f / r_cone) - height / 2;

/******************************************
 * get cartesian and cylindrical new atom *
 * coordinates, from the rotation center  *
 ******************************************/

x_c = x - c_x;
y_c = y - c_y;
d_c = sqrt (x_c * x_c + y_c * y_c);
angle = acos (x_c / d_c);
if (y_c < 0) angle = 2 * GAMGI_MATH_PI - angle;

/*****************************************
 * remove atoms in negative, positive or *
 * both arcs from bisector to min angles *
 *****************************************/

a0 = bisector - min;
a1 = bisector + min;
t0 = d_c * sin (angle - a0);
t1 = d_c * sin (angle - a1);
if (t0 > side * TOLERANCE_T && t1 < side * TOLERANCE_T) return -1;

/********************************************************
 * remove but do not rotate atoms: useful for debugging *
 ********************************************************/

if (positive == FALSE && negative == FALSE) return 0;
if (min == max) return 0;

if (positive == TRUE && negative == FALSE)
  { bisector -= min; min *= 2; }

if (positive == FALSE && negative == TRUE)
  { bisector += min; min *= 2; }

/***************************************************************
 * convert angle to local values: [-max to +max], bisector = 0 *
 ***************************************************************/

angle = angle - bisector;
if (positive == TRUE && negative == TRUE)
  {
  if (angle > GAMGI_MATH_PI) angle -= 2 * GAMGI_MATH_PI;
  if (angle < -GAMGI_MATH_PI) angle += 2 * GAMGI_MATH_PI;
  }

/********************************************
 * return atom if outside the rotation zone *
 ********************************************/

if (fabs (angle) < min / 2) return 0;
if (fabs (angle) > max) return 0;
if (negative == FALSE && angle < 0) return 0;
if (positive == FALSE && angle > 0) return 0;

/*******************************************
 * scale angle from [max, min] to [max, 0] *
 *******************************************/

if (angle < 0) {min = -min; max = -max; }
angle -= min * (max - angle)/ (max - min);

/***************************************************
 * convert angle back to absolute values: [0, 360[ *
 ***************************************************/

angle += bisector;
if (angle < 0) angle += 2 * GAMGI_MATH_PI;

/***********************************************
 * rotate atom with new angle to close arc gap *
 ***********************************************/

*x_new = f * d_c * cos (angle) + f * c_x; 
*y_new = f * d_c * sin (angle) + f * c_y;

return 1;
}

static void static_bond (gamgi_atom *atom1,
gamgi_atom **driver, int max, int i, int j, int type)
{
gamgi_atom *atom2;
gamgi_bond *bond;
int number;

if (i < -max || i >= max) return;
if (j < -max || j >= max) return;

number = static_index (i, j, max) + type;
atom2 = driver[number];
if (atom2 == NULL) return;

/***************
 * create bond *
 ***************/

bond = gamgi_chem_bond_create (atom1, atom2);
gamgi_chem_bond_name (bond, bond->object.name);
gamgi_mesa_start_bond (bond, gamgi->bond);
}

static int static_update (int index, int update, int type)
{
if (abs (update) == 1) index += update;
if (abs (update) == 2 && type == FALSE) index += update / 2;
if (abs (update) == 3 && type == TRUE) index += update / 3;

return index;
}

static void static_link (gamgi_atom **driver, int max,
int i0, int j0, int k0, int i1, int j1, int k1,
int ii0, int jj0, int kk0, int ii1, int jj1, int kk1)
{
gamgi_atom *atom;
int number;

while (i0 < max - 1 && j0 < max - 1 && i0 > -max && j0 > -max)
  {
  /*****************
   * update atom 0 *
   *****************/

  i0 = static_update (i0, ii0, k0);
  j0 = static_update (j0, jj0, k0);
  k0 = abs (kk0 - k0);

  /*****************
   * update atom 1 *
   *****************/

  i1 = static_update (i1, ii1, k1);
  j1 = static_update (j1, jj1, k1);
  k1 = abs (kk1 - k1);

  /*******************************************************
   * create bond between atoms (i0,j0,k0) and (i1,j1,k1) *
   *******************************************************/
  
  number = static_index (i0, j0, max) + k0;
  if (number >= 0 && number < 8 * max * max)
    {
    atom = driver[number]; if (atom != NULL)
    static_bond (atom, driver, max, i1, j1, k1);
    }

  }
}

static int static_atom_0 (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
double number;

/*************************************
 * remove inner and outer atoms only *
 *************************************/

number = static_arc (x, y, 0, 0, 0, 0, r_in, r_out,
0, 0, 0, 0, FALSE, FALSE, +1, x_new, y_new, z_new);
if (number < 0) return FALSE;

return TRUE;
}

static int static_atom_60_d (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/***************************
 * 1 ring, single rotation *
 ***************************/

number = static_arc (x, y, 0, 0, 0, 0, r_in, r_out,
60, 0, 30, 180, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_60_d (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, -1, 0, 0, -1, 0, 0, 2, -2, 1, 1, 3, 1);
}

static int static_atom_60_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new) 
{
return static_arc_origin (x, y, r_in, r_out, 60, x_new, y_new, z_new);
}

static void static_bond_60_o (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, -1, -1, 1, -1, -1, 1, 1, 0, 0, 1, 1, 0);
}

static int static_atom_120_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/*******************************
 * 2 rings, connected by edges *
 *******************************/

number = static_arc (x, y, a[0]/2, 0, 0, b[1], r_in, r_out,
120, 90, 30, 90, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0, 0, -b[1], r_in, r_out,
120, 270, 30, 90, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_120_e (gamgi_atom **driver, int max)
{
/****************************
 * build bonds for arcs 1,2 *
 ****************************/

static_link (driver, max, -1, 0, 0, -1, 0, 0, 1, 1, 0, 0, 1, 0);
static_link (driver, max, -1, -1, 1, -1, -1, 1, -1, -1, 0, 0, -1, 0);
}

static int static_atom_120_h (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/**********************************
 * 2 rings, connected by hexagons *
 **********************************/

number = static_arc (x, y, 0, 0, a[0]+b[0], a[1]+b[1], r_in, r_out,
120, 60, 30, 90, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -a[0]-b[0], -a[1]-b[1], r_in, r_out,
120, 240, 30, 90, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_120_h (gamgi_atom **driver, int max)
{
/****************************
 * build bonds for arcs 1,2 *
 ****************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, -1, 0, -1, -1, 0, -1, -2, 1, -3, -1, 1);

}

static int static_atom_120_s (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/****************************
 * 2 rings, single rotation *
 ****************************/

number = static_arc (x, y, 0, 0, a[0]+b[0], a[1]+b[1], r_in, r_out,
120, 30, 30, 180, FALSE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -a[0]-b[0], -a[1]-b[1], r_in, r_out,
120, 210, 30, 180, FALSE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_120_s (gamgi_atom **driver, int max)
{
/****************************
 * build bonds for arcs 1,2 *
 ****************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0);
static_link (driver, max, -1, -1, 0, -1, -1, 0, -1, 0, 0, -1, -1, 0);
}

static int static_atom_120_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
return static_arc_origin (x, y, r_in, r_out, 120, x_new, y_new, z_new);
}

static void static_bond_120_o (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, -1, -1, 1, -1, -1, 0, 1, 0, 0, 0, 1, 0);
}

static int static_atom_180_d (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/****************************
 * 3 rings, double rotation *
 ****************************/

number = static_arc (x, y, 0, 0, a[0]+b[0], a[1]+b[1], r_in, r_out,
180, 60, 30, 60, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -a[0], -a[1], r_in, r_out,
180, 180, 30, 60, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -b[0], -b[1], r_in, r_out,
180, 300, 30, 60, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_180_d (gamgi_atom **driver, int max)
{
/******************************
 * build bonds for arcs 1,2,3 *
 ******************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, -1, 1, -1, -1, 1, -3, 3, 1, -1, -2, 1);
static_link (driver, max, 0, -1, 1, 0, -1, 1, -3, -1, 1, 2, -2, 1);
}

static int static_atom_180_s (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/***************************************
 * 3 rings, single rotation (bonds=no) *
 ***************************************/

number = static_arc (x, y, 0, 0, a[0]+b[0], a[1]+b[1], r_in, r_out,
180, 30, 30, 120, FALSE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -a[0], -a[1], r_in, r_out,
180, 150, 30, 120, FALSE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, 0, 0, -b[0], -b[1], r_in, r_out,
180, 270, 30, 120, FALSE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_180_s (gamgi_atom **driver, int max)
{
/******************************
 * build bonds for arcs 1,2,3 *
 ******************************/

static_link (driver, max, -1, -1, 1, -1, -1, 1, 0, 1, 0, -1, 0, 0);
static_link (driver, max, 0, -1, 1, 0, -1, 1, -1, -1, 0, 0, -1, 0);
static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0);
}

static int static_atom_180_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
return static_arc_origin (x, y, r_in, r_out, 180, x_new, y_new, z_new);
}

static void static_bond_180_o (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, -1, -1, 1, 0, -1, 1, 1, 0, 0, -1, 0, 0);
}

static int static_atom_240_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/****************************
 * 4 rings, edges connected *
 ****************************/

number = static_arc (x, y, a[0]/2, 0, 3 * a[0]/2, 0, r_in, r_out,
240, 0, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0, 0, b[1], r_in, r_out,
240, 90, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0, -3 * a[0]/2, 0, r_in, r_out,
240, 180, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0, 0, -b[1], r_in, r_out,
240, 270, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_240_e (gamgi_atom **driver, int max)
{
/*********************************
 * build bonds for arcs 1,2,3,4 *
 *********************************/

static_link (driver, max, -1, 0, 0, -1, 0, 0, 1, 1, 0, 0, 1, 0);
static_link (driver, max, -1, -1, 1, -1, -1, 1, -1, -1, 0, 0, -1, 0);
static_link (driver, max, 0, 0, 0, 0, 0, 0, 2, -2, 1, 1, 3, 1);
static_link (driver, max, -2, -1, 1, -2, -1, 1, -3, 3, 1, -1, -2, 1);
}

static int static_atom_240_h (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/*******************************
 * 4 rings, hexagons connected *
 *******************************/

number = static_arc (x, y, a[0]/2, 0.0, +a[0], +b[1], r_in, r_out,
240, 60, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], +b[1], r_in, r_out,
240, 150, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], -b[1], r_in, r_out,
240, 240, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, +a[0], -b[1], r_in, r_out,
240, 330, 30, 45, TRUE, TRUE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_240_h (gamgi_atom **driver, int max)
{
/*********************************
 * build bonds for arcs 1,2,3,4 *
 *********************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, 0, 1, -1, 0, 1, 0, 1, 0, -1, 0, 0);
static_link (driver, max, -2, -1, 0, -2, -1, 0, -1, -2, 1, -3, -1, 1);
static_link (driver, max, -1, -1, 0, -1, -1, 0, 1, 0, 0, 0, -1, 0);
}

static int static_atom_240_p (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/***************************
 * 4 rings, planes aligned *
 ***************************/

number = static_arc (x, y, a[0]/2, 0.0, +a[0], +b[1], r_in, r_out,
240, 60, 30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], +b[1], r_in, r_out,
240, 120, 30, 90, FALSE, TRUE, -1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], -b[1], r_in, r_out,
240, 240,  30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, +a[0], -b[1], r_in, r_out,
240, 300, 30, 90, FALSE, TRUE, -1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_240_p (gamgi_atom **driver, int max)
{
/*********************************
 * build bonds for arcs 1,2,3,4 *
 *********************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, 0, 1, -1, 0, 1, 2, 1, 1, -3, 3, 1);
static_link (driver, max, -2, -1, 0, -2, -1, 0, -1, -2, 1, -3, -1, 1);
static_link (driver, max, -1, -1, 0, -1, -1, 0, -3, -1, 1, 2, -2, 1);
}

static int static_atom_240_a (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
int number;

/*************************
 * 4 rings, axes aligned *
 *************************/

number = static_arc (x, y, a[0]/2, 0.0, +a[0], +b[1], r_in, r_out,
240, 60, 30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], +b[1], r_in, r_out,
240, 150, 30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, -a[0], -b[1], r_in, r_out,
240, 240, 30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

number = static_arc (x, y, a[0]/2, 0.0, +a[0], -b[1], r_in, r_out,
240, 330, 30, 90, TRUE, FALSE, +1, x_new, y_new, z_new);
if (number > 0) return TRUE;
if (number < 0) return FALSE;

return TRUE;
}

static void static_bond_240_a (gamgi_atom **driver, int max)
{
/********************************
 * build bonds for arcs 1,2,3,4 *
 ********************************/

static_link (driver, max, 0, 0, 1, 0, 0, 1, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, 0, 1, -1, 0, 1, 0, 1, 0, -1, 0, 0);
static_link (driver, max, -2, -1, 0, -2, -1, 0, -1, -2, 1, -3, -1, 1);
static_link (driver, max, -1, -1, 0, -1, -1, 0, 0, -1, 0, 1, 0, 0);
}

static int static_atom_240_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
return static_arc_origin (x, y, r_in, r_out, 240, x_new, y_new, z_new);
}

static void static_bond_240_o (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, -1, -1, 1, 0, 0, 0, 1, 0, 0, -1, -1, 0);
}

static int static_atom_300_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
double d, angle, min, max;
int number;

/****************************
 * 5 rings, edges connected *
 ****************************/

number = static_arc (x, y, 0, 0, 0, 0, r_in, r_out,
300, 0, 30, 45, FALSE, FALSE, +1, x_new, y_new, z_new);

if (number == 0)
  number = static_arc (x, y, 0, 0, 0, 2 * b[1], r_in, r_out,
  300, 60, 30, 75, FALSE, TRUE, -1, x_new, y_new, z_new);

if (number == 0)
  number = static_arc (x, y, 0, 0, -a[0] + b[0], b[1], r_in, r_out,
  300, 150, 30, 75, TRUE, FALSE, +1, x_new, y_new, z_new);

if (number == 0)
  number = static_arc (x, y, 0, 0, -a[0] + b[0], -b[1], r_in, r_out,
  300, 210, 30, 75, FALSE, TRUE, -1, x_new, y_new, z_new);

if (number == 0)
  number = static_arc (x, y, 0, 0, 0, -2 * b[1], r_in, r_out,
  300, 300, 30, 75, TRUE, FALSE, +1, x_new, y_new, z_new);

if (number >= 0)
  {
  d = sqrt ((*x_new) * (*x_new) + (*y_new) * (*y_new));
  angle = acos (*x_new / d);
  if (*y_new < 0) angle = 2 * GAMGI_MATH_PI - angle;

  min = 30 * GAMGI_MATH_DEG_RAD;
  max = 180 * GAMGI_MATH_DEG_RAD;
  if (angle < 0) {min = -min; max = -max; }
  angle -= min * (max - angle)/ (max - min);

  *x_new = d * cos (angle);
  *y_new = d * sin (angle);

  return TRUE;
  }

return FALSE;
}

static void static_bond_300_e (gamgi_atom **driver, int max)
{
/**********************************
 * build bonds for arcs 1,2,3,4,5 *
 **********************************/

static_link (driver, max, 0, 1, 0, 0, 1, 0, 1, 3, 1, 2, 1, 1);
static_link (driver, max, -1, 0, 1, -1, 0, 1, 0, 1, 0, -1, 0, 0);
static_link (driver, max, -2, -1, 0, -2, -1, 0, -1, 0, 0, -1, -1, 0);
static_link (driver, max, -1, -2, 1, -1, -2, 1, -3, -1, 1, 2, -2, 1);
static_link (driver, max, -1, 0, 0, -1, 0, 0, 2, -2, 1, 1, 3, 1);
}

static int static_atom_300_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new)
{
return static_arc_origin (x, y, r_in, r_out, 300, x_new, y_new, z_new);
}

static void static_bond_300_o (gamgi_atom **driver, int max)
{
/*************************
 * build bonds for arc 1 *
 *************************/

static_link (driver, max, 0, -1, 1, 0, -1, 1, 1, 0, 0, 0, -1, 0);
}

static void static_atom (gamgi_molecule *molecule, gamgi_atom **driver, int max,
int i, int j, double *a, double *b, double r_in, double r_out, int element,
double x, double y, int type, atom_function function)
{
gamgi_atom *atom;
double x_new, y_new, z_new;
int number;

/*************************************
 * remove atoms in arcs and replace  *
 * with atoms rotated around centers *
 *************************************/

if (function != NULL && (*function) (a, b, r_in,
r_out, x, y, &x_new, &y_new, &z_new) == TRUE)
  {
  /************
   * add atom *
   ************/

  atom = gamgi_chem_atom_create (element);
  gamgi_engine_link_atom_molecule (atom, molecule);
  atom->position[0] = x_new;
  atom->position[1] = y_new;
  atom->position[2] = z_new;

  number = static_index (i, j, max) + type;
  driver[number] = atom;
  }
}

static void static_cone_3d (gamgi_molecule *molecule,
atom_function atom_model, bond_function bond_model, int element1,
int element2, double bond, int rings_in, int rings_out)
{
gamgi_atom *atom;
gamgi_atom **driver;
double a[2], b[2];
double r_in, r_out, x, y;
int i, j, max, number;

/****************************
 * graphene lattice vectors *
 ****************************/

gamgi_chem_graphene_vectors (bond, a, b);

/*******************************************************
  inner radius, outer radius, cone radius, cone height *
 *******************************************************/

r_in = static_circle (bond, rings_in) + TOLERANCE_R;
r_out = static_circle (bond, rings_out) + TOLERANCE_R;

/*********************************************
 * scanning region: a hexagonal losango with *
 * [-max,max] x [-max, max] cells, covering  *
 * completely a circle with radius r_out     *
 *********************************************/

max = 1 + (r_out * 1.2) / a[0];

/**************************************************
 * bond driver: 2 max x 2 max x (atom A + atom B) *
 **************************************************/

number = 8 * max * max;
driver = (gamgi_atom **) malloc (sizeof (gamgi_atom *) * number);
for (i = 0; i < number; i++) driver[i] = NULL;

/***************
 * build atoms *
 ***************/

for (j = -max; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    /*****************************************
     * A atoms (type = FALSE): (2/3, 1/3, 0) *
     *****************************************/

    x = (i + 2/3.0) * a[0] + (j + 1/3.0) * b[0];
    y = (i + 2/3.0) * a[1] + (j + 1/3.0) * b[1];
    static_atom (molecule, driver, max, i, j, a, b,
    r_in, r_out, element1, x, y, FALSE, atom_model);

    /****************************************
     * B atoms (type = TRUE): (1/3, 2/3, 0) *
     ****************************************/

    x = (i + 1/3.0) * a[0] + (j + 2/3.0) * b[0];
    y = (i + 1/3.0) * a[1] + (j + 2/3.0) * b[1];
    static_atom (molecule, driver, max, i, j, a, b,
    r_in, r_out, element2, x, y, TRUE, atom_model);
    }
  }

/****************************************************************************
 * build graphene bonds (type = FALSE for atoms A, type = TRUE for atoms B) *
 ****************************************************************************/

for (j = -max; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    number = static_index (i, j, max) + FALSE;
    atom = driver[number]; if (atom == 0) continue;

    static_bond (atom, driver, max, i, j, TRUE);
    static_bond (atom, driver, max, i + 1, j, TRUE);
    static_bond (atom, driver, max, i, j - 1, TRUE);
    }
  }

/********************
 * build cone bonds *
 ********************/

if (bond_model != NULL)
  (* bond_model) (driver, max);

free (driver);
}

static void static_optimize (gamgi_molecule *molecule, double bond_length)
{
gamgi_atom *central, *neighbour;
gamgi_bond *bond;
gamgi_dlist *dlist_a, *dlist_b;
double vector[3];
double length, weight = 0.1;
int i, iterations = 10;

return;

for (i = 0; i < iterations; i++)
  {

  for (dlist_a = molecule->atom_start; dlist_a != NULL; dlist_a = dlist_a->next)
    {
    central = GAMGI_CAST_ATOM dlist_a->data;

    for (dlist_b = central->bond_start; dlist_b != NULL; dlist_b = dlist_b->next)
      {
      bond = GAMGI_CAST_BOND dlist_b->data;
      if (bond->atom1 == central)
        neighbour = bond->atom2;
      else
        neighbour = bond->atom1;

      gamgi_math_vector_sub (neighbour->position, central->position, vector);
      length = gamgi_math_vector_length (vector);

      gamgi_math_vector_scale (vector, vector, weight * (length - bond_length) / length);
      gamgi_math_vector_add (central->position, vector, central->position);
      }
    }

  }

}

static atom_function *static_atom_router (void)
{
atom_function *router;

router = (atom_function *) malloc (sizeof (atom_function) * GAMGI_CHEM_CONE_300_O);

router[GAMGI_CHEM_CONE_0] = static_atom_0;

router[GAMGI_CHEM_CONE_60_D] = static_atom_60_d;
router[GAMGI_CHEM_CONE_60_O] = static_atom_60_o;

router[GAMGI_CHEM_CONE_120_E] = static_atom_120_e;
router[GAMGI_CHEM_CONE_120_H] = static_atom_120_h;
router[GAMGI_CHEM_CONE_120_S] = static_atom_120_s;
router[GAMGI_CHEM_CONE_120_O] = static_atom_120_o;

router[GAMGI_CHEM_CONE_180_D] = static_atom_180_d;
router[GAMGI_CHEM_CONE_180_S] = static_atom_180_s;
router[GAMGI_CHEM_CONE_180_O] = static_atom_180_o;

router[GAMGI_CHEM_CONE_240_E] = static_atom_240_e;
router[GAMGI_CHEM_CONE_240_H] = static_atom_240_h;
router[GAMGI_CHEM_CONE_240_P] = static_atom_240_p;
router[GAMGI_CHEM_CONE_240_A] = static_atom_240_a;
router[GAMGI_CHEM_CONE_240_O] = static_atom_240_o;

router[GAMGI_CHEM_CONE_300_E] = static_atom_300_e;
router[GAMGI_CHEM_CONE_300_O] = static_atom_300_o;

return router;
}

static bond_function *static_bond_router (void)
{
bond_function *router;

router = (bond_function *) malloc (sizeof (bond_function) * GAMGI_CHEM_CONE_300_O);

router[GAMGI_CHEM_CONE_0] = NULL;

router[GAMGI_CHEM_CONE_60_D] = static_bond_60_d;
router[GAMGI_CHEM_CONE_60_O] = static_bond_60_o;

router[GAMGI_CHEM_CONE_120_E] = static_bond_120_e;
router[GAMGI_CHEM_CONE_120_H] = static_bond_120_h;
router[GAMGI_CHEM_CONE_120_S] = static_bond_120_s;
router[GAMGI_CHEM_CONE_120_O] = static_bond_120_o;

router[GAMGI_CHEM_CONE_180_D] = static_bond_180_d;
router[GAMGI_CHEM_CONE_180_S] = static_bond_180_s;
router[GAMGI_CHEM_CONE_180_O] = static_bond_180_o;

router[GAMGI_CHEM_CONE_240_E] = static_bond_240_e;
router[GAMGI_CHEM_CONE_240_H] = static_bond_240_h;
router[GAMGI_CHEM_CONE_240_P] = static_bond_240_p;
router[GAMGI_CHEM_CONE_240_A] = static_bond_240_a;
router[GAMGI_CHEM_CONE_240_O] = static_bond_240_o;

router[GAMGI_CHEM_CONE_300_E] = static_bond_300_e;
router[GAMGI_CHEM_CONE_300_O] = static_bond_300_o;

return router;
}

void gamgi_chem_graphene_cone (gamgi_molecule *molecule, int mode,
int rings_in, int rings_out, int element1, int element2, double bond)
{
atom_function *atom_router;
bond_function *bond_router;
atom_function atom_model;
bond_function bond_model;

/******************************************
 * build routers to point to the selected *
 * model to handle atoms and bonds        *
 *                                        *
 * get model to handle atoms and bonds    *
 *                                        *
 *         apply selected model           *
 *                                        *
 *          optimize structure            *
 ******************************************/

atom_router = static_atom_router ();
bond_router = static_bond_router ();

atom_model = atom_router[mode];
bond_model = bond_router[mode];

static_cone_3d (molecule, atom_model, bond_model,
element1, element2, bond, rings_in, rings_out);

static_optimize (molecule, bond);

/********************
 * free router data *
 ********************/

free (atom_router);
free (bond_router);
}
