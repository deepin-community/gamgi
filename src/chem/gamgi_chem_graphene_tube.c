/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene_tube.c
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
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_chem_graphene.h"
#include "gamgi_io_token.h"

#define TOLERANCE_X 0.1
#define TOLERANCE_Z 0.1

static void static_vectors (double *a, double *b,
double angle, double *a_rotated, double *b_rotated)
{
/************************************
 * rotated graphene lattice vectors *
 ************************************/

a_rotated[0] = cos (angle) * a[0] - sin (angle) * a[1];
a_rotated[1] = sin (angle) * a[0] + cos (angle) * a[1];
b_rotated[0] = cos (angle) * b[0] - sin (angle) * b[1];
b_rotated[1] = sin (angle) * b[0] + cos (angle) * b[1];
}

static double static_height (double bond, int rings, int n1, int n2,
double angle)
{
/***********************************************************
 * armchair (angle = 30):                                  *
 * h = 2 * rings * bond * sin (PI / 3.0)                   *
 *                                                         *
 * zigzag (angle = 0):                                     *
 * h = (1 + rings) * bond * sin (PI / 6.0) + rings * bond  *
 ***********************************************************/

return 
bond * sin (GAMGI_MATH_PI / 6.0 - angle) +
rings * bond * sin (GAMGI_MATH_PI / 2.0 + angle) +
rings * bond * sin (GAMGI_MATH_PI / 6.0 + angle);
}

static int static_inside (double perimeter, double height, double x, double z)
{
if (x < -TOLERANCE_X || x > perimeter + TOLERANCE_X) return FALSE;
if (z < -TOLERANCE_Z || z > height + TOLERANCE_Z) return FALSE;

return TRUE;
}

static void static_fold (gamgi_atom *atom, double perimeter, double x, double z)
{
double radius, angle;

radius = perimeter / (2 * GAMGI_MATH_PI);
angle = x / radius;

atom->position[0] = radius * cos (angle);
atom->position[1] = radius * sin (angle);
atom->position[2] = z;
}

static void static_bond (gamgi_atom *atom1,
gamgi_atom **driver, int max, int i, int j, int type)
{
gamgi_atom *atom2;
gamgi_bond *bond;
int number;

if (j < 0 || j >= max) return;
if (i < -max || i >= max) return;

number = gamgi_chem_graphene_index (2 * max, max, 0, i, j, type);
atom2 = driver[number]; if (atom2 == NULL) return;

/***************
 * create bond *
 ***************/

bond = gamgi_chem_bond_create (atom1, atom2);
gamgi_chem_bond_name (bond, bond->object.name);
gamgi_mesa_start_bond (bond, gamgi->bond);
}

static void static_atom_2d (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int i, int j, int type, int n1, int n2, double perimeter,
double height, int element, double x, double y)
{
gamgi_atom *atom;
int number;

/*****************************************************
 * ignore atoms not inside the rectangle +T x height *
 *****************************************************/

if (static_inside (perimeter, height, x, y) == FALSE) return;

/***************
 * create atom *
 ***************/

atom = gamgi_chem_atom_create (element);
gamgi_engine_link_atom_molecule (atom, molecule);

atom->position[0] = x - perimeter / 2;
atom->position[1] = y - height / 2;
atom->position[2] = 0.0;

number = gamgi_chem_graphene_index (2 * max, max, 0, i, j, type);
driver[number] = atom;
}

static void static_atom_3d (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int i, int j, int type, int n1, int n2, double perimeter,
double height, int element, double x, double z)
{
gamgi_atom *atom;
int i_pbc, j_pbc, pbc, number;

/*****************************************************
 * ignore atoms not inside the rectangle +T x height *
 *****************************************************/

if (static_inside (perimeter, height, x, z) == FALSE) return;

/*****************************************************
 * ignore atoms related with periodic boundary atoms *
 * with -T vector, so boundary atoms (that would be  *
 * superimposed after folding) are not created twice *
 *                                                   *
 * with +T vector is not needed because atoms are    *
 * generated in increasing order, from -max to + max *
 *****************************************************/

i_pbc = i - n1 - n2;
j_pbc = j - n2;
pbc = TRUE;
if (j_pbc < 0 || i_pbc >= max) pbc = FALSE;
if (i_pbc < -max || i_pbc >= max) pbc = FALSE;
if (pbc == TRUE)
  { 
  number = gamgi_chem_graphene_index (2 * max, max, 0, i_pbc, j_pbc, type);
  if (driver[number] == NULL) pbc = FALSE;
  }

/***************
 * create atom *
 ***************/

if (pbc == FALSE)
  {
  atom = gamgi_chem_atom_create (element);
  gamgi_engine_link_atom_molecule (atom, molecule);

  static_fold (atom, perimeter, x, z - height / 2);
  number = gamgi_chem_graphene_index (2 * max, max, 0, i, j, type);
  driver[number] = atom;
  }
}

static void static_bond_2d (gamgi_atom **driver, int max, int n1, int n2)
{
gamgi_atom *atom;
int i, j, number;

/**************************************
 * build bonds between atoms in the   *
 * graphene sheet that are neighbours *
 **************************************/

for (j = 0; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    /*****************************************************
     * type = FALSE for atoms A, type = TRUE for atoms B *
     *****************************************************/
    
    number = gamgi_chem_graphene_index (2 * max, max, 0, i, j, FALSE);
    atom = driver[number]; if (atom == NULL) continue;

    /***************
     * local bonds *
     ***************/

    static_bond (atom, driver, max, i, j, TRUE);
    static_bond (atom, driver, max, i - 1, j, TRUE);
    static_bond (atom, driver, max, i - 1, j - 1, TRUE);
    }
  }
}

static void static_bond_3d (gamgi_atom **driver, int max, int n1, int n2)
{
gamgi_atom *atom;
int i, j, number;

/***********************************************
 * build bonds between atoms that become       *
 * neighbours after folding the graphene sheet *
 ***********************************************/

for (j = 0; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    /*****************************************************
     * type = FALSE for atoms A, type = TRUE for atoms B *
     *****************************************************/

    number = gamgi_chem_graphene_index (2 * max, max, 0, i, j, FALSE);
    atom = driver[number]; if (atom == NULL) continue;

    /**************************************
     * periodic boundary bonds: -T vector *
     **************************************/

    static_bond (atom, driver, max, i - n1 - n2, j - n2, TRUE);
    static_bond (atom, driver, max, i - 1 - n1 - n2, j - n2, TRUE);
    static_bond (atom, driver, max, i - 1 - n1 - n2, j - 1- n2, TRUE);

    /**************************************
     * periodic boundary bonds: +T vector *
     **************************************/

    static_bond (atom, driver, max, i + n1 + n2, j + n2, TRUE);
    static_bond (atom, driver, max, i - 1 + n1 + n2, j + n2, TRUE);
    static_bond (atom, driver, max, i - 1 + n1 + n2, j - 1+ n2, TRUE);
    }
  }
}

gamgi_atom **gamgi_chem_graphene_tube_driver (int n1, int n2, int rings,
double bond, int *max, double *perimeter, double *angle, double *height)
{
gamgi_atom **driver;
double a[2], b[2], t[2];
double radius, p, h;
int m, number, i;

/*********************
 * make sure n1 > n2 *
 *********************/

if (n1 < n2) { i = n2; n2 = n1; n1 = i; }

/****************************
 * nanotube lattice vectors *
 ****************************/

a[0] = bond * sqrt (3.0);
a[1] = 0.0;
b[0] = 0.5 * bond * sqrt (3.0);
b[1] = 1.5 * bond;

/********************************************
 * translation vector: T is always between  *
 * 0 and PI/2, angle is set negative so T   *
 * vector is rotated down, to be horizontal *
 *                                          *
 * rectangle dimensions: perimeter x height *
 ********************************************/

t[0] = n1 * a[0] + n2 * b[0];
t[1] = n1 * a[1] + n2 * b[1];

*perimeter = p = sqrt (t[0] * t[0] + t[1] * t[1]);
*angle = -acos (t[0] / p);
*height = h = static_height (bond, rings, n1, n2, *angle);

/***********************************************
 * scanning region: a hexagonal losango with   *
 * [-max,max] x [0, max] cells, covering       *
 * completely a half-circle with radius radius *
 ***********************************************/

radius = sqrt (p * p + h * h);
*max = m = 1.0 + (1.2 * radius) / (bond * sqrt (3.0));

/*******************************************
 * driver array: each cell (i,j) stores    *
 * the id numbers of the two A, B atoms    *
 * in each (i,j) primitive graphene cell   *
 *                                         *
 * number: max x 2 max x (atom A + atom B) *
 *******************************************/

number = 4 * m * m;
driver = (gamgi_atom **) malloc (sizeof (gamgi_atom *) * number);
for (i = 0; i < number; i++) driver[i] = NULL;

return driver;
}

void gamgi_chem_graphene_tube_2d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond)
{
double a[2], b[2], a_rotated[2], b_rotated[2];
double x, y;
int i, j;

/***********************************************************
 * build: 1) graphene vectors; 2) rotated graphene vectors *
 * so the rotated translation vector becomes horizontal    *
 ***********************************************************/

gamgi_chem_graphene_vectors (bond, a, b);
static_vectors (a, b, angle, a_rotated, b_rotated);

/******************************************************
 * each primitive cell (i,j), with length a and       *
 * angle 120o, is formed by two equilateral triangles *
 *                                                    *
 * atoms A are in the vertices,                       *
 * atoms B are centered in the triangles below        *
 *                                                    *
 * the distance between atoms is the distance from    *
 * a vertex to the center of a triangle: a/sqrt(3)    *
 ******************************************************/

for (j = 0; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    /*************************************
     * A atoms (type = FALSE): (0, 0, 0) *
     *************************************/

    x = i * a_rotated[0] + j * b_rotated[0];
    y = i * a_rotated[1] + j * b_rotated[1];
    static_atom_2d (molecule, driver, max, i, j, FALSE,
    n1, n2, perimeter, height, element1, x, y);

    /****************************************
     * B atoms (type = TRUE): (2/3, 1/3, 0) *
     ****************************************/

    x += 2 / 3.0 * a_rotated[0] + 1 / 3.0 * b_rotated[0];
    y += 2 / 3.0 * a_rotated[1] + 1 / 3.0 * b_rotated[1];
    static_atom_2d (molecule, driver, max, i, j, TRUE,
    n1, n2, perimeter, height, element2, x, y);
    }
  }

static_bond_2d (driver, max, n1, n2);
}

void gamgi_chem_graphene_tube_3d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond)
{
double a[2], b[2], a_rotated[2], b_rotated[2];
double x, z;
int i, j;

/***********************************************************
 * build: 1) graphene vectors; 2) rotated graphene vectors *
 * so the rotated translation vector becomes horizontal    *
 ***********************************************************/

gamgi_chem_graphene_vectors (bond, a, b);
static_vectors (a, b, angle, a_rotated, b_rotated);

/******************************************************
 * each primitive cell (i,j), with length a and       *
 * angle 120o, is formed by two equilateral triangles *
 *                                                    *
 * atoms A are in the vertices,                       *
 * atoms B are centered in the triangles below        *
 *                                                    *
 * the distance between atoms is the distance from    *
 * a vertex to the center of a triangle: a/sqrt(3)    *
 ******************************************************/

for (j = 0; j < max; j++)
  {
  for (i = -max; i < max; i++)
    {
    /*************************************
     * A atoms (type = FALSE): (0, 0, 0) *
     *************************************/

    x = i * a_rotated[0] + j * b_rotated[0];
    z = i * a_rotated[1] + j * b_rotated[1];
    static_atom_3d (molecule, driver, max, i, j, FALSE,
    n1, n2, perimeter, height, element1, x, z);

    /****************************************
     * B atoms (type = TRUE): (2/3, 0, 1/3) *
     ****************************************/

    x += 2 / 3.0 * a_rotated[0] + 1 / 3.0 * b_rotated[0];
    z += 2 / 3.0 * a_rotated[1] + 1 / 3.0 * b_rotated[1];
    static_atom_3d (molecule, driver, max, i, j, TRUE,
    n1, n2, perimeter, height, element2, x, z);
    }
  }

static_bond_2d (driver, max, n1, n2);
static_bond_3d (driver, max, n1, n2);
}

void gamgi_chem_graphene_tube (gamgi_molecule *molecule, int n1,
int n2, int rings, int element1, int element2, double bond)
{
gamgi_atom **driver = NULL;
double perimeter, angle, height;
int max;

driver = gamgi_chem_graphene_tube_driver (n1, n2,
rings, bond, &max, &perimeter, &angle, &height);

gamgi_chem_graphene_tube_3d (molecule, driver, max,
n1, n2, perimeter, angle, height, element1, element2, bond);

free (driver);
}
