/******************************************
 *
 * $GAMGI/src/math/gamgi_math_node.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_phys.h"

#include "gamgi_math_diophantine.h"

static gamgi_bool static_decrease (gamgi_enum lattice, int *in, int *out)
{
int factor;

/***************************************
 * convert primitive node contribution *
 ***************************************/

switch (lattice)
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  factor = 2;
  break;

  /*****************
   * face-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  factor = 2;
  break;

   /*****************
    * base-centered *
    *****************/

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  factor = 2;
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_HEXAGONAL_R:
  factor = 3;
  break;

  /*************
   * primitive *
   *************/

  default:
  factor = 1;
  }

out[0] = in[0] / factor;
out[1] = in[1] / factor;
out[2] = in[2] / factor;

/**************************************
 * convert centered node contribution *
 **************************************/

if (in[0] % factor == 0 && in[1] % factor == 0 && in[2] % factor == 0)
  { out[3] = FALSE; return TRUE; }

switch (lattice)
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  if (in[0] % 2 == 1 && in[1] % 2 == 1 && in[2] % 2 == 1)
    { out[3] = GAMGI_PHYS_111; return TRUE; }
  break;

  /*****************
   * face-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  if (in[0] % 2 == 0 && in[1] % 2 == 1 && in[2] % 2 == 1)
    { out[3] = GAMGI_PHYS_011; return TRUE; }
  if (in[0] % 2 == 1 && in[1] % 2 == 0 && in[2] % 2 == 1)
    { out[3] = GAMGI_PHYS_101; return TRUE; }
  if (in[0] % 2 == 1 && in[1] % 2 == 1 && in[2] % 2 == 0)
    { out[3] = GAMGI_PHYS_110; return TRUE; }
  break;

   /*****************
    * base-centered *
    *****************/

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  if (in[0] % 2 == 1 && in[1] % 2 == 1 && in[2] % 2 == 0)
    { out[3] = GAMGI_PHYS_110; return TRUE; }
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_HEXAGONAL_R:
  if (in[0] % 3 == 2 && in[1] % 3 == 1 && in[2] % 3 == 1)
    { out[3] = GAMGI_PHYS_211; return TRUE; }
  if (in[0] % 3 == 1 && in[1] % 3 == 2 && in[2] % 3 == 2)
    { out[3] = GAMGI_PHYS_122; return TRUE; }
  break;

  /*************
   * primitive *
   *************/

  default:
  break;
  }

return FALSE;
}

static int static_increase (gamgi_enum lattice, int *in, int *out)
{
int factor;

/***************************************
 * convert primitive node contribution *
 ***************************************/

switch (lattice)
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  factor = 2;
  break;

  /*****************
   * face-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  factor = 2;
  break;

  /*****************
   * base-centered *
   *****************/

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  factor = 2;
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_HEXAGONAL_R:
  factor = 3;
  break;

  /*************
   * primitive *
   *************/

  default:
  factor = 1;
  break;
  }

out[0] = factor * in[0];
out[1] = factor * in[1];
out[2] = factor * in[2];

/**************************************
 * convert centered node contribution *
 **************************************/

switch (in[3])
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_111:
  out[0] += 1; out[1] += 1; out[2] += 1;
  break;

  /********************************
   * face-centered, base-centered *
   ********************************/

  case GAMGI_PHYS_011:
  out[1] += 1; out[2] += 1;
  break;
 
  case GAMGI_PHYS_101:
  out[0] += 1; out[2] += 1;
  break;

  case GAMGI_PHYS_110:
  out[0] += 1; out[1] += 1;
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_211:
  out[0] += 2; out[1] += 1; out[2] += 1;
  break;

  case GAMGI_PHYS_122:
  out[0] += 1; out[1] += 2; out[2] += 2;
  break;

  /*************
   * primitive *
   *************/

  default:
  break;
  }

return factor;
}

static gamgi_bool static_centered (int *hkl, int order, gamgi_enum value)
{
int a, b, c, d;

/*********************************
 * find centered node coordinate *
 *********************************/

switch (value)
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_111:
  a = 2 * hkl[0];
  b = 2 * hkl[1];
  c = 2 * hkl[2];
  d = 2 * order - hkl[0] - hkl[1] - hkl[2];
  break;

  /********************************
   * face-centered, base-centered *
   ********************************/

  case GAMGI_PHYS_011:
  a = 2 * hkl[0];
  b = 2 * hkl[1];
  c = 2 * hkl[2];
  d = 2 * order - hkl[1] - hkl[2];
  break;

  case GAMGI_PHYS_101:
  a = 2 * hkl[0];
  b = 2 * hkl[1];
  c = 2 * hkl[2];
  d = 2 * order - hkl[0] - hkl[2];
  break;

  case GAMGI_PHYS_110:
  a = 2 * hkl[0];
  b = 2 * hkl[1];
  c = 2 * hkl[2];
  d = 2 * order - hkl[0] - hkl[1];
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_211:
  a = 3 * hkl[0];
  b = 3 * hkl[1];
  c = 3 * hkl[2];
  d = 3 * order - 2 * hkl[0] - hkl[1] - hkl[2];
  break;

  case GAMGI_PHYS_122:
  a = 3 * hkl[0];
  b = 3 * hkl[1];
  c = 3 * hkl[2];
  d = 3 * order - hkl[0] - 2 * hkl[1] - 2 * hkl[2];
  break;

  /*************
   * primitive *
   *************/

  default:
  a = hkl[0];
  b = hkl[1];
  c = hkl[2];
  d = order;
  break;
  }

return gamgi_math_diophantine_soluble (a, b, c, d);
}

static gamgi_bool static_primitive (gamgi_enum lattice,
int *hkl, int order, int o, gamgi_enum value)
{
int a, b, c;

/********************************************
 * try to find a node in the plane with     *
 * given (o1|o2|o3) integer node coordinate *
 ********************************************/

/*********************************************
 * first look for primitive nodes (o4 = 000) *
 *********************************************/

if (o == 0) a = hkl[1]; else a = hkl[0];
if (o == 2) b = hkl[1]; else b = hkl[2];
c = order - hkl[o] * value;

if (gamgi_math_diophantine_soluble (0, a, b, c) == TRUE)
  return TRUE;

/***************************************************************
 * then look for centered nodes (o4 = 111|011|101|110|211|122) *
 ***************************************************************/

switch (lattice)
  {
  /**********************
   * body-centered: 111 *
   **********************/

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  c = 2 * order - (2 * value + 1) * hkl[o] - a - b;
  if (gamgi_math_diophantine_soluble (0, 2 * a, 2 * b, c) == TRUE) return TRUE;
  break;

  /********************************
   * face-centered: 011, 101, 110 *
   ********************************/

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  c = 2 * order - (2 * value) * hkl[o] - a - b;
  if (gamgi_math_diophantine_soluble (0, 2 * a, 2 * b, c) == TRUE) return TRUE;
  c = 2 * order - (2 * value + 1) * hkl[o] - b;
  if (gamgi_math_diophantine_soluble (0, 2 * a, 2 * b, c) == TRUE) return TRUE;
  c = 2 * order - (2 * value + 1) * hkl[o] - a;
  if (gamgi_math_diophantine_soluble (0, 2 * a, 2 * b, c) == TRUE) return TRUE;
  break;

  /**********************
   * base-centered: 110 *
   **********************/

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  if (o == 2) c = 2 * order - (2 * value) * hkl[o] - a - b;
  else c = 2 * order - (2 * value + 1) * hkl[o] - a;
  if (gamgi_math_diophantine_soluble (0, 2 * a, 2 * b, c) == TRUE) return TRUE;
  break;

  /*******************************************
   * rombohedral, obverse-centered: 211, 122 *
   *******************************************/

  case GAMGI_PHYS_HEXAGONAL_R:
  if (o == 0) c = 3 * order - (3 * value + 2) * hkl[o] - a - b;
  else c = 3 * order - (3 * value + 1) * hkl[o] - 2 * a - b;
  if (gamgi_math_diophantine_soluble (0, 3 * a, 3 * b, c) == TRUE) return TRUE;

  if (o == 0) c = 3 * order - (3 * value + 1) * hkl[o] - 2 * a - 2 * b;
  else c = 3 * order - (3 * value + 2) * hkl[o] - a - 2 * b;
  if (gamgi_math_diophantine_soluble (0, 3 * a, 3 * b, c) == TRUE) return TRUE;
  break;

  /*************
   * primitive *
   *************/

  default:
  break;
  }

return FALSE;
}

gamgi_bool gamgi_math_node_check (gamgi_enum lattice, gamgi_enum node)
{
/********************************************
 * corner node: acceptable for all lattices *
 ********************************************/

if (node == FALSE) return TRUE;

/***************************************************
 * centered node: acceptable for specific lattices *
 ***************************************************/

switch (lattice)
  {
  /*****************
   * body-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  if (node == GAMGI_PHYS_111) return TRUE;
  break;

  /*****************
   * face-centered *
   *****************/

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  if (node == GAMGI_PHYS_110) return TRUE;
  if (node == GAMGI_PHYS_101) return TRUE;
  if (node == GAMGI_PHYS_011) return TRUE;
  break;

  /*****************
   * base-centered *
   *****************/

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  if (node == GAMGI_PHYS_110) return TRUE;
  break;

  /*********************************
   * rombohedral, obverse-centered *
   *********************************/

  case GAMGI_PHYS_HEXAGONAL_R:
  if (node == GAMGI_PHYS_211) return TRUE;
  if (node == GAMGI_PHYS_122) return TRUE;
  break;

  /****************
   * non-centered *
   ****************/

  default:
  break;
  }

/*******************************
 * Error: lattice does no have *
 * centered nodes of this type *
 *******************************/

return FALSE;
}

void gamgi_math_node_primitive (gamgi_enum lattice,
int c1, int c2, int c3, int c4, int *p1, int *p2, int *p3)
{
int a, b, c;

/******************************************************
 * convert conventional to primitive node coordinates *
 ******************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  a = c1 + c3;
  if (c4 == GAMGI_PHYS_111) a++;
  b = c1 + c2;
  if (c4 == GAMGI_PHYS_111) b++;
  c = c2 + c3;
  if (c4 == GAMGI_PHYS_111) c++;
  break;

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  a = c1 - c2 + c3;
  if (c4 == GAMGI_PHYS_101) a++;
  b = c1 + c2 - c3;
  if (c4 == GAMGI_PHYS_110) b++;
  c = -c1 + c2 + c3;
  if (c4 == GAMGI_PHYS_011) c++;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  a = c1 - c2;
  b = c1 + c2;
  if (c4 == GAMGI_PHYS_110) b++;
  c = c3;
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  a = c1 + c3;
  if (c4 == GAMGI_PHYS_122 || c4 == GAMGI_PHYS_211) a++;
  b = -c1 + c2 + c3;
  if (c4 == GAMGI_PHYS_122) b++;
  c = -c2 + c3;
  break;

  default:
  a = c1;
  b = c2;
  c = c3;
  break;
  }

*p1 = a;
*p2 = b;
*p3 = c;
}

void gamgi_math_node_conventional (gamgi_enum lattice,
int p1, int p2, int p3, int *c1, int *c2, int *c3, int *c4)
{
int t, a, b, c, d;

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

a = b = c = d = 0;

/******************************************************
 * convert primitive to conventional node coordinates *
 ******************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I: case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  t = p1 + p2 - p3;
  if (t % 2 == 0) a = t;
  if (t % 2 == 1 || t % 2 == -1) a = t - 1;
  a /= 2;

  t = -p1 + p2 + p3;
  if (t % 2 == 0) b = t;
  if (t % 2 == 1 || t % 2 == -1) b = t - 1;
  b /= 2;

  t = p1 - p2 + p3;
  if (t % 2 == 0) c = t; 
  if (t % 2 == 1 || t % 2 == -1) c = t - 1;
  c /= 2;

  t = p1 + p2 + p3;
  if (t % 2 == 0) d = FALSE;
  if (t % 2 == 1 || t % 2 == -1) d = GAMGI_PHYS_111;
  break;

  case GAMGI_PHYS_CUBIC_F: case GAMGI_PHYS_ORTHORHOMBIC_F:
  t = p1 + p2;
  if (t % 2 == 0) a = t; 
  if (t % 2 == 1 || t % 2 == -1) a = t - 1;
  a /= 2;

  t = p2 + p3;
  if (t % 2 == 0) b = t;
  if (t % 2 == 1 || t % 2 == -1) b = t - 1; 
  b /= 2;

  t = p1 + p3;
  if (t % 2 == 0) c = t;
  if (t % 2 == 1 || t % 2 == -1) c = t - 1;
  c /= 2;

  if (p1 % 2 == 0 && p2 % 2 == 0 && p3 % 2 == 0) d = FALSE;
  if (p1 % 2 != 0 && p2 % 2 != 0 && p3 % 2 != 0) d = FALSE;
  if (p1 % 2 != 0 && p2 % 2 == 0 && p3 % 2 != 0) d = GAMGI_PHYS_110;
  if (p1 % 2 == 0 && p2 % 2 != 0 && p3 % 2 == 0) d = GAMGI_PHYS_110;
  if (p1 % 2 == 0 && p2 % 2 != 0 && p3 % 2 != 0) d = GAMGI_PHYS_101;
  if (p1 % 2 != 0 && p2 % 2 == 0 && p3 % 2 == 0) d = GAMGI_PHYS_101;
  if (p1 % 2 != 0 && p2 % 2 != 0 && p3 % 2 == 0) d = GAMGI_PHYS_011;
  if (p1 % 2 == 0 && p2 % 2 == 0 && p3 % 2 != 0) d = GAMGI_PHYS_011;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C: case GAMGI_PHYS_MONOCLINIC_C:
  t = p1 + p2;
  if (t % 2 == 0) a = t;
  if (t % 2 == 1 || t % 2 == -1) a = t - 1; 
  a /= 2;
  
  t = -p1 + p2;
  if (t % 2 == 0) b = t;
  if (t % 2 == 1 || t % 2 == -1) b = t - 1;
  b /= 2;

  c = p3;

  t = p1 + p2;
  if (t % 2 == 0) d = FALSE;
  if (t % 2 == 1 || t % 2 == -1) d = GAMGI_PHYS_110;
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  t = 2 * p1 - p2 - p3;
  if (t % 3 == 0) a = t;
  if (t % 3 == 1 || t % 3 == -2) a = t - 1;
  if (t % 3 == 2 || t % 3 == -1) a = t - 2;
  a /= 3;

  t = p1 + p2 - 2 * p3;
  if (t % 3 == 0) b = t;
  if (t % 3 == 1 || t % 3 == -2) b = t - 1;
  if (t % 3 == 2 || t % 3 == -1) b = t - 2;
  b /= 3;

  t = p1 + p2 + p3;
  if (t % 3 == 0) c = t;
  if (t % 3 == 1 || t % 3 == -2) c = t - 1;
  if (t % 3 == 2 || t % 3 == -1) c = t - 2;
  c /= 3;

  t = 2 * p1 - p2 - p3;
  if (t % 3 == 0) d = FALSE;
  if (t % 3 == 1 || t % 3 == -2) d = GAMGI_PHYS_122;
  if (t % 3 == 2 || t % 3 == -1) d = GAMGI_PHYS_211;
  break;

  default:
  a = p1;
  b = p2;
  c = p3;
  d = FALSE;
  break;
  }

*c1 = a;
*c2 = b;
*c3 = c;
*c4 = d;
}

gamgi_bool gamgi_math_node_direction (gamgi_direction *direction, int o, int value)
{
gamgi_cell *cell;
gamgi_enum lattice;
int node_new[4];
int node2[3], node2_new[3];
int *uvw, *node;
int remainder, factor, signal, quotient;

uvw = direction->uvw;
node = direction->node;
cell = GAMGI_CAST_CELL direction->object.object;
lattice = cell->lattice;

if (direction->vectors == GAMGI_PHYS_PRIMITIVE)
  {
  if (o == 3)
    { if (value == 0) return TRUE; else return FALSE; }

  if (node[o] == value) return TRUE;
  if (uvw[o] == 0) return FALSE;
  remainder = (value - node[o]) % uvw[o];
  if (remainder == 0) return TRUE; else return FALSE;
  }

if (o == 3)
  {
  if (node[3] == value) return TRUE;
  if (gamgi_math_node_check (lattice, value) == FALSE) return FALSE;

  static_increase (lattice, node, node2);
  while (1 > 0)
    {
    node2_new[0] = node2[0] + uvw[0];
    node2_new[1] = node2[1] + uvw[1];
    node2_new[2] = node2[2] + uvw[2];

    node2[0] = node2_new[0];
    node2[1] = node2_new[1];
    node2[2] = node2_new[2];

    /*********************************************************************
     * moving successively along uvw direction, one integer segment      *
     * each time, eventually a node is reached with the same o4          *
     * coordinate as initialy. If the searched o4 is not found before    *
     * that, then no o4 will be found ever for this direction.           *
     *                                                                   *
     * 1) if no node was found, just move to next segment                *
     * 2) if new node has searched o4 coordinate, search suceeded        *
     * 3) if new node has again the initial o4 coordinate, search failed *
     *********************************************************************/

    if (static_decrease (lattice,
    node2_new, node_new) == FALSE) continue;

    if (node_new[3] == value) return TRUE;
    if (node_new[3] == node[3]) return FALSE;
    }
  }

if (node[o] == value) return TRUE;
if (uvw[o] == 0) return FALSE;

factor = static_increase (lattice, node, node2);
quotient = (factor * value - node2[o]) / uvw[o];
signal = quotient / abs (quotient);
quotient = abs (quotient);

while (1 > 0)
  {
  node2_new[0] = node2[0] + signal * quotient * uvw[0];
  node2_new[1] = node2[1] + signal * quotient * uvw[1];
  node2_new[2] = node2[2] + signal * quotient * uvw[2];

  node2[0] = node2_new[0];
  node2[1] = node2_new[1];
  node2[2] = node2_new[2];

  quotient = 1;

  if (static_decrease (lattice,
  node2_new, node_new) == FALSE) continue;
  if (node_new[o] == value) return TRUE;
  if (signal * (node_new[o] - value) > 0) return FALSE;
  }

return FALSE;
}

gamgi_bool gamgi_math_node_plane (gamgi_plane *plane, int o, int value)
{
gamgi_cell *cell;
gamgi_enum lattice;
int *hkl;
int a, b, c;
int order, gcd;

hkl = plane->hkl;
order = plane->order;
cell = GAMGI_CAST_CELL plane->object.object;
lattice = cell->lattice;

if (plane->vectors == GAMGI_PHYS_PRIMITIVE)
  {
  if (o == 3)
    { if (value == 0) return TRUE; else return FALSE; }

  if (o == 0) a = hkl[1]; else a = hkl[0];
  if (o == 2) b = hkl[1]; else b = hkl[2];
  c = order - value * hkl[o];

  gcd = gamgi_math_diophantine_gcd (a, b);
  if (c % gcd == 0) return TRUE; else return FALSE;
  }

/*********************************
 * find centered node coordinate *
 *********************************/

if (o == 3)
  {
  if (gamgi_math_node_check (lattice, value) == FALSE) return FALSE;
  return static_centered (hkl, order, value);
  }

/**********************************
 * find primitive node coordinate *
 **********************************/

return static_primitive (lattice, hkl, order, o, value);
}
