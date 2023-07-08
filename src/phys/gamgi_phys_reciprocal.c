/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_reciprocal.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_phys.h"
#include "gamgi_math.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_cell.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_plane.h"

gamgi_enum gamgi_phys_reciprocal_lattice (gamgi_enum lattice)
{
switch (lattice)
  {
  /************************
   * Direct    Reciprocal *
   * Lattice -> Lattice   *
   *  P           P       *
   *  C           C       *
   *  R           R       *
   *  I           F       *
   *  F           I       *
   ************************/

  case GAMGI_PHYS_ORTHORHOMBIC_I:
  return GAMGI_PHYS_ORTHORHOMBIC_F;

  case GAMGI_PHYS_ORTHORHOMBIC_F:
  return GAMGI_PHYS_ORTHORHOMBIC_I;

  case GAMGI_PHYS_CUBIC_I: 
  return GAMGI_PHYS_CUBIC_F;

  case GAMGI_PHYS_CUBIC_F:
  return GAMGI_PHYS_CUBIC_I;

  default:
  break;
  }

/***********************************************************
 * The reciprocal lattice of Tetragonal I is Tetragonal F, *
 * which is equivalent to Tetragonal I, after scaling the  *
 * a,b vectors by 1/sqrt(2) and rotating them 45 degrees.  *
 ***********************************************************/

return lattice;
}

void gamgi_phys_reciprocal_vectors (double factor, double *p1, 
double *p2, double *p3, double *b1, double *b2, double *b3)
{
double scale;

/******************************************************
 * transform direct lattice (cell_in) into reciprocal *
 * lattice (cell_out) applying the reciprocal lattice *
 * vector definition (the vectors must be primitive): *
 *          b1 = factor*(p2xp3)/(p1.p2xp3)            *
 *          b2 = factor*(p3xp1)/(p1.p2xp3)            *
 *          b3 = factor*(p1xp2)/(p1.p2xp3)          *
 ******************************************************/

gamgi_math_vector_cross (p2, p3, b1);
gamgi_math_vector_cross (p3, p1, b2);
gamgi_math_vector_cross (p1, p2, b3);

/***********************************************************
 * Usually the scaling factor is:                          *
 * -> 1.0 (the most common choice in Crystallography)      *
 * -> lambda (wavelength, used in Radio Crystallography)   *
 * -> lambda/2 (wavelength/2, the so-called normalized     *
 * reciprocal lattice, used in X-ray Diffraction, in       *
 * conjunction with Bragg's law)                           *
 * -> 2*PI (the most common choice in Solid State Physics) *
 * -> (volume/node)**2/3 (so the volume per node is the    *
 * same in direct and reciprocal spaces), the so-called    *
 * Bravais polar lattice).                                 *
 ***********************************************************/

scale = factor / gamgi_math_vector_dot (p1, b1);

gamgi_math_vector_scale (b1, b1, scale);
gamgi_math_vector_scale (b2, b2, scale);
gamgi_math_vector_scale (b3, b3, scale);
}

static void static_cell_copy (gamgi_cell *cell_old, gamgi_cell *cell)
{
/*****************************
 * disable group information *
 *****************************/

cell->group = 0;

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

/************************
 * copy view parameters *
 ************************/

cell->borders = cell_old->borders;
cell->faces = cell_old->faces;
cell->nodes = cell_old->nodes;

cell->red = cell_old->red;
cell->green = cell_old->green;
cell->blue = cell_old->blue;

cell->object.scale = cell_old->object.scale;
}

static void static_cell_increase (gamgi_enum lattice, gamgi_cell *cell)
{
switch (lattice)
  {
  case GAMGI_PHYS_ORTHORHOMBIC_F: case GAMGI_PHYS_CUBIC_F: 
  case GAMGI_PHYS_ORTHORHOMBIC_I: case GAMGI_PHYS_CUBIC_I:
  cell->n1 *= 2;
  cell->n2 *= 2;
  cell->n3 *= 2;
  break;

  case GAMGI_PHYS_MONOCLINIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  cell->n1 *= 2;
  cell->n2 *= 2;
  break;

  case GAMGI_PHYS_TETRAGONAL_I:
  cell->n1 = cell->n1 + cell->n2;
  cell->n2 = cell->n1;
  cell->n3 *= 2;
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  cell->n1 = cell->n1 + cell->n2;
  cell->n2 += cell->n1;
  cell->n3 *= 3;
  break;

  default:
  break;
  }
}

/********************* internal function *****************
 *                                                       *
 *                       STATIC_VECTORS                  *
 *                                                       *
 * Receives the primitive vectors of the direct lattice  *
 * and gives the primitive vectors of the reciprocal     *
 * lattice. hP and tI lattices must be tuned in the end. *
 *********************************************************/

static void static_vectors (double factor, gamgi_enum lattice,
double *p1, double *p2, double *p3, double *b1, double *b2, double *b3)
{
double f1[3], f2[3], f3[3];
double i1[3], i2[3], i3[3];

/******************************************************
 *                get reciprocal vectors              *
 *                                                    *
 * The reciprocal lattice of the Hexagonal P lattice  *
 * is a Hexagonal P lattice, rotated 30 degrees       *
 *                                                    *
 * The reciprocal lattice of the Tetragonal I lattice *
 * is the Tetragonal F lattice, which is equivalent   *
 * to a Tetragonal I lattice, rotated 45 degrees      *
 *                                                    *
 * The reciprocal lattice of Triclinic and Monoclinic *
 * lattices have a different orientation. Applying    *
 * a second reciprocal lattice transformation gives   *
 * back exactly the original lattice orientation.     *
 *                                                    *
 * All the other lattices are transformed into        *
 * reciprocal lattices with the same orientation.     *
 ******************************************************/

gamgi_phys_reciprocal_vectors (factor, p1, p2, p3, b1, b2, b3);

/***********************************************
 * replace b2 by b2-b1, to change the angle    *
 * (b1,b2) from 60 to 120 degrees, the value   *
 * we use to represent the hexagonal P lattice *
 ***********************************************/

if (lattice == GAMGI_PHYS_HEXAGONAL_P)
  gamgi_math_vector_sub (b2, b1, b2);

/*******************************************************
 * The reciprocal lattice of GAMGI_PHYS_TETRAGONAL_I   *
 * is GAMGI_PHYS_TETRAGONAL_F, which is not defined,   *
 * as it is equivalent to GAMGI_PHYS_TETRAGONAL_I      *
 * rotated 45 degrees around the conventional vectors. *
 *                                                     *
 * 1) Convert primitive to conventional tF vectors     *
 *    (reusing the GAMGI_PHYS_ORTHORHOMBIC_F code)     *
 * 2) Convert conventional tF (f) to tI (i) vectors    *
 *    (reusing the GAMGI_PHYS_ORTHORHOMBIC_C code)     *
 * 3) Convert conventional to primitive tI vectors     *
 *******************************************************/

if (lattice == GAMGI_PHYS_TETRAGONAL_I)
  {
  gamgi_math_cell_conventional (GAMGI_PHYS_ORTHORHOMBIC_F,
  b1, b2, b3, f1, f2, f3);
  gamgi_math_cell_primitive (GAMGI_PHYS_ORTHORHOMBIC_C,
  f2, f1, f3, i2, i1, i3);
  gamgi_math_cell_primitive (lattice, i1, i2, i3, b1, b2, b3);
  }
}

static void static_lattice (gamgi_cell *cell, double *rotate)
{
double base[9], inverse[9];
double quaternion[4];
double a1[3], a2[3], a3[3];
double a, b, c, ab, ac, bc;

/****************************************
 * get parameters and build new lattice *
 ****************************************/

gamgi_math_cell_conventional (cell->lattice, 
cell->p1, cell->p2, cell->p3, a1, a2, a3);
gamgi_math_cell_parameters (a1, a2, a3, &a, &b, &c, &ab, &ac, &bc);
gamgi_phys_cell_parameters (cell, a, b, c, ab, ac, bc);
gamgi_phys_cell_create (cell);

/*********************************************************************
 *       Get rotation for reciprocal lattice transformation          *
 *                                                                   *
 * the base matrix built from cell->a1, cell->a2, cell->a3 describes *
 * the reference orientation for the conventional vectors, while the *
 * base matrix built from a1, a2, a3 describes the orientation after *
 * the reciprocal lattice transformation. Initially each lattice is  *
 * built with the reference orientation, so the lattice must rotate  *
 * to obtain the real orientation.                                   *
 *********************************************************************/

gamgi_math_matrix_absolute (base,
cell->a1[0], cell->a2[0], cell->a3[0],
cell->a1[1], cell->a2[1], cell->a3[1],
cell->a1[2], cell->a2[2], cell->a3[2]);
gamgi_math_matrix_inverse (base, inverse);

gamgi_math_matrix_absolute (base,
a1[0], a2[0], a3[0], a1[1], a2[1], a3[1], a1[2], a2[2], a3[2]);
gamgi_math_matrix_matrix (base, inverse, rotate);

/***********************************************
 * apply reciprocal lattice matrix rotation    *
 * to cell (the rotation center is irrelevant, *
 * as after the second rotation, the           *
 * translation corrects these effects)         *
 ***********************************************/

gamgi_math_quaternion_from_matrix (rotate, quaternion);
gamgi_mesa_rotate_cell (cell, quaternion, rotate, cell->origin);
}

static void static_position (gamgi_cell *cell_old, 
gamgi_cell *cell, double *rotate)
{
double translate[3];

/*************************************************
 *    apply old cell accumulated rotation to     *
 *    cell (the rotation center is irrelevant)   *
 *                                               *
 * apply translation so old cell and cell have   *
 * the same origin. The rotation changes the     *
 * origin so the translate vector must subtract  *
 * this effect before being applied to the cell. *
 *************************************************/

gamgi_math_quaternion_to_matrix (cell_old->quaternion, rotate);
gamgi_mesa_rotate_cell (cell, cell_old->quaternion, rotate, cell->center);

gamgi_math_vector_sub (cell_old->origin, cell->origin, translate);
gamgi_mesa_translate_cell (cell, translate);
}

void gamgi_phys_reciprocal_points (gamgi_plane *plane)
{
plane->points = (double *) malloc (3 * sizeof (double));
gamgi_math_vector_copy (plane->origin, plane->points);
plane->n_points = 1;

plane->loops = (int *) malloc (2 * sizeof (int));
plane->loops[0] = 1;
plane->loops[1] = 0;
plane->n_loops = 2;
}

gamgi_cell *gamgi_phys_reciprocal_primitive (gamgi_cell *cell_old,
double factor, gamgi_window *window)
{
gamgi_cell *cell;
double transformation[9], orientation[9], rotate[9];
double translate[3], origin[3];

/**********************************************************
 * create cell: use darker colors to show the primitive   *
 * lattice, to make it less visible than the real lattice *
 **********************************************************/

cell = gamgi_engine_create_cell ();
gamgi_engine_start_cell (cell);
static_cell_copy (cell_old, cell);
strcpy (cell->object.name, "Primitive");
cell->red = GAMGI_MESA_CELL_RECIPROCAL_R; 
cell->green = GAMGI_MESA_CELL_RECIPROCAL_G; 
cell->blue = GAMGI_MESA_CELL_RECIPROCAL_B;

/****************************************************
 * Create lattice: 1) for hR cells, the primitive   *
 * lattice must be represented with a primitive     *
 * volume, not conventional; 2) increase the number *
 * of primitive cells, to cover the real cell; 3)   *
 * for hR and tI cells, translate the cell origin.  *
 ****************************************************/

cell->lattice = gamgi_phys_cell_primitive (cell_old->lattice);
if (cell_old->lattice == GAMGI_PHYS_HEXAGONAL_R)
  cell->model = GAMGI_PHYS_PRIMITIVE;

cell->reciprocal = factor;
static_vectors (factor, cell->lattice, cell_old->a1,
cell_old->a2, cell_old->a3, cell->p1, cell->p2, cell->p3);
static_cell_increase (cell_old->lattice, cell);

/*****************************************************************
 * the transformation matrix describes the rotation needed       *
 * to take cell vectors from the standard orientation to the     *
 * orientation produced by the reciprocal lattice transformation *
 *                                                               *
 * the orientation matrix describes the orientation of the       *
 * initial cell, compared with standard orientation, when        *
 * the reciprocal lattice transformation was applied             *
 *****************************************************************/

static_lattice (cell, transformation);
static_position (cell_old, cell, orientation);

/*************************************************************
 * Translate cell back along rotated a1,a2 vectors, so the   *
 * previously increased reciprocal primitive lattice covers  *
 * exactly the centered reciprocal lattice.                  *
 *                                                           *
 * The two rotations can be combined, even if transformation *
 * is applied to the cell origin and orientation is applied  *
 * to the cell center, because the new a1,a2 orientation     *
 * is all that is needed (a1,a2 always start from origin).   *
 *************************************************************/

if (cell->lattice == GAMGI_PHYS_TETRAGONAL_P)
  {
  gamgi_math_matrix_matrix (orientation, transformation, rotate);
  gamgi_math_matrix_vector (rotate, cell->a1, translate);
  gamgi_math_vector_scale (translate, translate, -cell_old->n2);
  gamgi_mesa_translate_cell (cell, translate);
  }
if (cell->lattice == GAMGI_PHYS_HEXAGONAL_P)
  {
  gamgi_math_matrix_matrix (orientation, transformation, rotate);
  gamgi_math_matrix_vector (rotate, cell->a2, translate);
  gamgi_math_vector_scale (translate, translate, -cell_old->n1);
  gamgi_mesa_translate_cell (cell, translate);
  }

/*****************************************************
 * To avoid complex issues arising in I->F, F->I     *
 * transformations, and in parallelepiped and sphere *
 * volumes, the reciprocal lattice is always created *
 * with the default origin: o1=0, o2=0, o3=0 (after  *
 * the transformation, users can then easily move    *
 * the node (000), to any valid position they like). *
 *                                                   *
 *  this is an auxiliary lattice only, so no axes    *
 *  are shown (users can then change this default).  *
 *                                                   *
 * The reciprocal lattice must be translated so the  *
 * node (000) is in the same position for direct     *
 * and reciprocal lattices. As the transformation    *
 * rotation is applied around the origin, which is   *
 * unchanged for the reciprocal lattice (see above), *
 * only the orientation matrix should be considered. *
 *****************************************************/

cell->o1 = 0; cell->o2 = 0; cell->o3 = 0;
if (cell_old->lattice == GAMGI_PHYS_TETRAGONAL_I) cell->o1 = cell_old->n2;
if (cell_old->lattice == GAMGI_PHYS_HEXAGONAL_R) cell->o2 = cell_old->n1;
cell->origin_vectors = GAMGI_PHYS_PRIMITIVE;

cell->axes = FALSE;
cell->axes_vectors = GAMGI_PHYS_PRIMITIVE;

gamgi_math_cell_offset (cell_old, origin);
gamgi_math_matrix_vector (orientation, origin, translate);
gamgi_mesa_translate_cell (cell, translate);

return cell;
}

gamgi_cell *gamgi_phys_reciprocal (gamgi_cell *cell_old, 
double factor, gamgi_enum planes, gamgi_window *window)
{
gamgi_cell *cell;
double transformation[9], orientation[9];
double translate[3], origin[3];

/****************************************
 * create cell: use the same parameters *
 * for direct and reciprocal lattices   *
 ****************************************/

cell = gamgi_engine_create_cell ();
gamgi_engine_start_cell (cell);
static_cell_copy (cell_old, cell);
strcpy (cell->object.name, "Reciprocal");

/***********************************************
 * create lattice: for hP and tI lattices, the *
 * reciprocal lattice must be further rotated  *
 ***********************************************/

cell->lattice = gamgi_phys_reciprocal_lattice (cell_old->lattice);
cell->reciprocal = factor;
static_vectors (factor, cell->lattice, cell_old->p1,
cell_old->p2, cell_old->p3, cell->p1, cell->p2, cell->p3);
static_lattice (cell, transformation);
static_position (cell_old, cell, orientation);

/*****************************************************
 * To avoid complex issues arising in I->F, F->I     *
 * transformations, and in parallelepiped and sphere *
 * volumes, the reciprocal lattice is always created *
 * with the default origin: o1=0, o2=0, o3=0 (AFTER  *
 * the transformation, users can then easily move    *
 * the node (000), to any valid position they like). *
 *                                                   *
 * direct and reciprocal lattices: use the same axes *
 *                                                   *
 * The reciprocal lattice must be translated so the  *
 * node (000) is in the same position for direct     *
 * and reciprocal lattices. As the transformation    *
 * rotation is applied around the origin, which is   *
 * unchanged for the reciprocal lattice (see above), *
 * only the orientation matrix should be considered. *
 *****************************************************/

cell->o1 = 0; cell->o2 = 0; cell->o3 = 0;
cell->origin_vectors = GAMGI_PHYS_PRIMITIVE;

cell->axes = cell_old->axes;
cell->axes_vectors = cell_old->axes_vectors;

gamgi_math_cell_offset (cell_old, origin);
gamgi_math_matrix_vector (orientation, origin, translate);
gamgi_mesa_translate_cell (cell, translate);

if (planes == TRUE)
  gamgi_phys_plane_reciprocal (cell, transformation, orientation);

return cell;
}
