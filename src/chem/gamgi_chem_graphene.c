/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene.c
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
#include "gamgi_chem_graphene_tube.h"
#include "gamgi_io_token.h"

#define TOLERANCE_X 0.1
#define TOLERANCE_Z 0.1

static void static_bond (gamgi_atom *atom1, gamgi_atom **driver,
int n1, int n2, int i, int j, int type)
{
gamgi_atom *atom2;
gamgi_bond *bond;
int number;

if (j < 0 || j >= n2) return;
if (i < 0 || i >= n1) return;

number = gamgi_chem_graphene_index (n1, 0, 0, i, j, type);
atom2 = driver[number]; if (atom2 == NULL) return;

/***************
 * create bond *
 ***************/

bond = gamgi_chem_bond_create (atom1, atom2);
gamgi_chem_bond_name (bond, bond->object.name);
gamgi_mesa_start_bond (bond, gamgi->bond);
}

static gamgi_atom *static_atom (gamgi_molecule *molecule,
int element, double x, double y, double z)
{
gamgi_atom *atom;

atom = gamgi_chem_atom_create (element);
gamgi_engine_link_atom_molecule (atom, molecule);
atom->position[0] = x;
atom->position[1] = y;
atom->position[2] = z;

return atom;
}

static void static_bond_2d (gamgi_atom **driver, int n1, int n2)
{
gamgi_atom *atom;
int i, j, number;

/**************************************
 * build bonds between atoms in the   *
 * graphene sheet that are neighbours *
 **************************************/

for (j = 0; j < n2; j++)
  {
  for (i = 0; i < n1; i++)
    {
    /*****************************************************
     * type = FALSE for atoms A, type = TRUE for atoms B *
     *****************************************************/

    number = gamgi_chem_graphene_index (n1, 0, 0, i, j, FALSE);
    atom = driver[number]; if (atom == NULL) continue;

    /***************
     * local bonds *
     ***************/

    static_bond (atom, driver, n1, n2, i, j, TRUE);
    static_bond (atom, driver, n1, n2, i + 1, j, TRUE);
    static_bond (atom, driver, n1, n2, i, j - 1, TRUE);
    }
  }
}

void static_atom_2d (gamgi_molecule *molecule, gamgi_atom **driver,
int n1, int n2, int i, int j, int type, int element, double x, double y)
{
int number;

/**************************************
 * save pointer to new atom in driver *
 **************************************/

number = gamgi_chem_graphene_index (n1, 0, 0, i, j, type);
driver[number] = static_atom (molecule, element, x, y, 0.0);
}

static gamgi_atom **static_lattice (gamgi_molecule *molecule,
int n1, int n2, int element1, int element2, double bond)
{
gamgi_atom **driver;
double a[2], b[2];
double x, y;
int i, j, number;

gamgi_chem_graphene_vectors (bond, a, b);

/*******************************************
 * driver array: each cell (i,j) stores    *
 * the id numbers of the two A, B atoms    *
 * in each (i,j) primitive graphene cell   *
 *                                         *
 * number: max x 2 max x (atom A + atom B) *
 *******************************************/

number = 2 * n1 * n2;
driver = (gamgi_atom **) malloc (sizeof (gamgi_atom *) * number);
for (i = 0; i < number; i++) driver[i] = NULL;

for (j = 0; j < n2; j++)
  {
  for (i = 0; i < n1; i++)
    {
    /*************************************
     * A atoms (type = FALSE): (0, 0, 0) *
     *************************************/

    x = i * a[0] + j * b[0];
    y = i * a[1] + j * b[1];
    static_atom_2d (molecule, driver, n1, n2, i, j, FALSE, element1, x, y);

    /*****************************************
     * B atoms (type = TRUE): (-1/3, 1/3, 0) *
     *****************************************/

    x += -1 / 3.0 * a[0] + 1 / 3.0 * b[0];
    y += -1 / 3.0 * a[1] + 1 / 3.0 * b[1];
    static_atom_2d (molecule, driver, n1, n2, i, j, TRUE, element2, x, y);
    }
  }

static_bond_2d (driver, n1, n2);

return driver;
}

void static_1p5p (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int j_start, int a, int b, int type, int element1, int element2,
double radius, double z_top, double angle_start)
{
gamgi_atom *atoms[5];
gamgi_atom *atom;
gamgi_bond *bond;
double x, y, angle;
int i, j, k, element, number;

/***************************************************
 * build atoms and bonds for a (5,0) nanotube cap, *
 * formed by 1 pentagon linked to 5 pentagons      *
 ***************************************************/

for (i = 0; i < 5; i++)
  {
  angle = angle_start + i * 2 * GAMGI_MATH_PI / 5;
  x = radius * cos (angle);
  y = radius * sin (angle);

  element = (i % 2 == 0) ? element1 : element2;
  atoms[i] = static_atom (molecule, element, x, y, z_top);
  }

j = j_start;
for (i = 0; i < 5; i++)
  {
  k = (j + 1) % 5;

  /********************
   * create cap bonds *
   ********************/

  bond = gamgi_chem_bond_create (atoms[j], atoms[k]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  /*************************
   * create tube-cap bonds *
   *************************/

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + i, b, type);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[j], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  j = k;
  }
}

void static_caps_5_0 (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int rings, double perimeter,
double height, int element1, int element2, double bond)
{
double radius, z_top, width;

/*****************************************************
 * build bottom and top caps for (5,0) nanotubes,    *
 * formed by a single pentagon linked to 5 pentagons *
 *****************************************************/

radius = 0.5 * bond / sin (GAMGI_MATH_PI/5);
width = 0.5 * perimeter / GAMGI_MATH_PI - radius;
z_top = height / 2 + sqrt (bond * bond - width * width);

/************************************
 * cap below (to build cap or cage) *
 ************************************/

static_1p5p (molecule, driver, max, 0, 0, 0,
FALSE, element1, element2, radius, -z_top, 0.0);

if (rings < 0) return;

/*****************************
 * cap above (to build cage) *
 *****************************/

if (rings % 2 == 0)
  static_1p5p (molecule, driver, max, 3, rings / 2, rings,
  TRUE, element1, element2, radius, z_top, GAMGI_MATH_PI);
else
  static_1p5p (molecule, driver, max, 0, rings / 2, rings,
  TRUE, element1, element2, radius, z_top, 0.0);
}

void static_1p5h5ph (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int j_start, int a, int b, int element1, int element2,
double radius, double z_top, double z_step, double angle_start)
{
gamgi_atom *atoms[20];
gamgi_atom *atom;
gamgi_bond *bond;
double x, y, x_new, y_new, angle, angle_new;
int i, j, k, element, number;

/***************************************************
 * build atoms and bonds for a (5,5) nanotube cap, *
 * formed by 1 pentagon surrounded by 5 hexagons,  *
 * linked to 5 alternating pentagons and hexagons  *
 ***************************************************/

/*********
 * atoms *
 *********/

for (i = 0; i < 5; i++)
  {
  angle = angle_start + i * 2 * GAMGI_MATH_PI / 5;
  x = radius * cos (angle);
  y = radius * sin (angle);

  element = (i % 2 == 0) ? element1 : element2;
  atoms[4 * i + 0] = static_atom (molecule, element, x, y, z_top);
  element = (i % 2 == 1) ? element1 : element2;
  atoms[4 * i + 1] = static_atom (molecule, element, 2 * x, 2 * y, z_top - z_step);

  angle_new = angle - 2 * GAMGI_MATH_PI / 5;
  x_new = 2 * x + radius * cos (angle_new);
  y_new = 2 * y + radius * sin (angle_new);
  atoms[4 * i + 2] = static_atom (molecule, element2, x_new, y_new, z_top - 2 * z_step);

  angle_new = angle + 2 * GAMGI_MATH_PI / 5;
  x_new = 2 * x + radius * cos (angle_new);
  y_new = 2 * y + radius * sin (angle_new);
  atoms[4 * i + 3] = static_atom (molecule, element1, x_new, y_new, z_top - 2 * z_step);
  }

/*********
 * bonds *
 *********/

j = j_start;
for (i = 0; i < 5; i++)
  {
  k = (j + 1) % 5;

  /********************
   * create cap bonds *
   ********************/

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * k + 0]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * j + 1]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 3]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 3], atoms[4 * k + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  /*************************
   * create tube-cap bonds *
   *************************/

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, FALSE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 2], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, TRUE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 3], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  j = k;
  }
}

void static_caps_5_5 (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int rings, double perimeter,
double height, int element1, int element2, double bond)
{
double radius, z_top, z_step, angle_start;

/**************************************************
 * build bottom and top caps for (5,5) nanotubes, *
 * formed by 1 pentagon surrounded by 5 hexagons, *
 * linked to 5 alternating pentagons and hexagons *
 **************************************************/

radius = 0.5 * bond / sin (GAMGI_MATH_PI/5);
angle_start = GAMGI_MATH_PI * bond / perimeter;
z_step = bond * sqrt (1 - pow (2 * sin (GAMGI_MATH_PI / 5), -2));
z_top = height / 2 + bond * sqrt(3) / 2 + 2 * z_step;

/************************************
 * cap below (to build cap or cage) *
 ************************************/

static_1p5h5ph (molecule, driver, max, 0, 0, 0,
element1, element2, radius, -z_top, -z_step, angle_start);

if (rings < 0) return;

/*****************************
 * cap above (to build cage) *
 *****************************/

if (rings % 2 == 0)
  static_1p5h5ph (molecule, driver, max, 3, 1, rings / 2 + 1,
  element1, element2, radius, z_top, z_step, angle_start + GAMGI_MATH_PI);
else
  static_1p5h5ph (molecule, driver, max, 0, 0, rings / 2 + 1,
  element1, element2, radius, z_top, z_step, angle_start);
}

void static_1h6h6ph_simple (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int j_start, int a, int b, int element1, int element2,
double radius, double z_top, double z_step, double angle_start)
{
gamgi_atom *atoms[24];
gamgi_atom *atom;
gamgi_bond *bond;
double x, y, x_new, y_new, angle, angle_new;
int i, j, k, element, number;

/***************************************************
 * build atoms and bonds for a (6.6) nanotube cap, *
 * formed by 1 hexagon surrounded by 6 hexagons,   *
 * linked to 6 alternating pentagons and hexagons  *
 ***************************************************/

/*********
 * atoms *
 *********/

for (i = 0; i < 6; i++)
  {
  angle = angle_start + i * 2 * GAMGI_MATH_PI / 6;
  x = radius * cos (angle);
  y = radius * sin (angle);

  element = (i % 2 == 0) ? element1 : element2;
  atoms[4 * i + 0] = static_atom (molecule, element, x, y, z_top);
  element = (i % 2 == 1) ? element1 : element2;
  atoms[4 * i + 1] = static_atom (molecule, element, 2 * x, 2 * y, z_top - z_step);

  angle_new = angle - 2 * GAMGI_MATH_PI / 5;
  x_new = 2 * x + radius * cos (angle_new);
  y_new = 2 * y + radius * sin (angle_new);
  atoms[4 * i + 2] = static_atom (molecule, element2, x_new, y_new, z_top - 2 * z_step);

  angle_new = angle + 2 * GAMGI_MATH_PI / 5;
  x_new = 2 * x + radius * cos (angle_new);
  y_new = 2 * y + radius * sin (angle_new);
  atoms[4 * i + 3] = static_atom (molecule, element1, x_new, y_new, z_top - 2 * z_step);
  }

/*********
 * bonds *
 *********/

j = j_start;
for (i = 0; i < 6; i++)
  {
  k = (j + 1) % 6;

  /********************
   * create cap bonds *
   ********************/

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * k + 0]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * j + 1]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 3]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 3], atoms[4 * k + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  /*************************
   * create tube-cap bonds *
   *************************/

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, FALSE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 2], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, TRUE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 3], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  j = k;
  }
}

void static_caps_6_6_simple (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int rings, double perimeter,
double height, int element1, int element2, double bond)
{
double radius, z_top, z_step, angle_start;

radius = bond;
angle_start = GAMGI_MATH_PI * bond / perimeter;
z_step = bond * sqrt (1 - pow (2 * sin (GAMGI_MATH_PI / 5), -2));
z_top = height / 2 + bond * sqrt(3) / 2 + 2 * z_step;

/************************************
 * cap below (to build cap or cage) *
 ************************************/

static_1h6h6ph_simple (molecule, driver, max, 0, 0, 0,
element1, element2, radius, -z_top, -z_step, angle_start);

if (rings < 0) return;

/*****************************
 * cap above (to build cage) *
 *****************************/

if (rings % 2 == 0)
  static_1h6h6ph_simple (molecule, driver, max, 0, 1, rings / 2 + 1,
  element1, element2, radius, z_top, z_step, angle_start + GAMGI_MATH_PI / 6);
else
  static_1h6h6ph_simple (molecule, driver, max, 0, 0, rings / 2 + 1,
  element1, element2, radius, z_top, z_step, angle_start);
}

void static_1h6h6ph_complex (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int j_start, int a, int b, int element1, int element2,
double x0, double z0, double x1, double z1, double x2, double y2, double z2,
double angle_start)
{
gamgi_atom *atoms[24];
gamgi_atom *atom;
gamgi_bond *bond;
double x, y, angle;
int i, j, k, element, number;

/***************************************************
 * build atoms and bonds for a (6.6) nanotube cap, *
 * formed by 1 hexagon surrounded by 6 hexagons,   *
 * linked to 6 alternating pentagons and hexagons  *
 ***************************************************/

/*********
 * atoms *
 *********/

for (i = 0; i < 6; i++)
  {
  angle = angle_start + i * 2 * GAMGI_MATH_PI / 6;

  x = x0 * cos (angle);
  y = x0 * sin (angle);
  element = (i % 2 == 0) ? element1 : element2;
  atoms[4 * i + 0] = static_atom (molecule, element, x, y, z0);

  x = (x0 + x1) * cos (angle);
  y = (x0 + x1) * sin (angle);
  element = (i % 2 == 1) ? element1 : element2;
  atoms[4 * i + 1] = static_atom (molecule, element, x, y, z0 - z1);

  x = (x0 + x1 + x2) * cos (angle) + y2 * sin (angle);
  y = (x0 + x1 + x2) * sin (angle) - y2 * cos (angle);
  atoms[4 * i + 2] = static_atom (molecule, element2, x, y, z0 - z1 - z2);

  x = (x0 + x1 + x2) * cos (angle) - y2 * sin (angle);
  y = (x0 + x1 + x2) * sin (angle) + y2 * cos (angle);
  atoms[4 * i + 3] = static_atom (molecule, element1, x, y, z0 - z1 - z2);
  }

/*********
 * bonds *
 *********/

j = j_start;
for (i = 0; i < 6; i++)
  {
  k = (j + 1) % 6;

  /********************
   * create cap bonds *
   ********************/

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * k + 0]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 0], atoms[4 * j + 1]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 1], atoms[4 * j + 3]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  bond = gamgi_chem_bond_create (atoms[4 * j + 3], atoms[4 * k + 2]);
  gamgi_chem_bond_name (bond, bond->object.name);
  gamgi_mesa_start_bond (bond, gamgi->bond);

  /*************************
   * create tube-cap bonds *
   *************************/

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, FALSE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 2], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  number = gamgi_chem_graphene_index (2 * max, max, 0, a + 2 * i, b + i, TRUE);
  atom = driver[number];
  if (atom != NULL)
    {
    bond = gamgi_chem_bond_create (atoms[4 * j + 3], atom);
    gamgi_chem_bond_name (bond, bond->object.name);
    gamgi_mesa_start_bond (bond, gamgi->bond);
    }

  j = k;
  }
}

void static_caps_6_6_complex (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int rings, double perimeter,
double height, int element1, int element2, double bond)
{
double x0, z0, x1, z1, x2, y2, z2, z3;
double angle_base, angle_shift, angle_start;;

angle_start = GAMGI_MATH_PI * bond / perimeter;
angle_base = asin (1 / (sqrt (3) * tan (GAMGI_MATH_PI / 5)));
angle_shift = atan (sqrt (0.5 * (3 - sqrt (5))));

z3 = 2 * bond * pow (cos (GAMGI_MATH_PI / 5), 2) / sqrt (3);
z2 = bond * cos (GAMGI_MATH_PI / 5) / sqrt (3);
z1 = bond * sin (angle_base - angle_shift);
z0 = height / 2 + z1 + z2 + z3;

x2 = bond / (2 * sqrt (3) * cos (GAMGI_MATH_PI / 5));
x1 = bond * cos (angle_base - angle_shift);
x0 = bond;

y2 = bond * cos (GAMGI_MATH_PI / 5);

/************************************
 * cap below (to build cap or cage) *
 ************************************/

static_1h6h6ph_complex (molecule, driver, max, 0, 0, 0,
element1, element2, x0, -z0, x1, -z1, x2, y2, -z2, angle_start);

if (rings < 0) return;

/*****************************
 * cap above (to build cage) *
 *****************************/

if (rings % 2 == 0)
  static_1h6h6ph_complex (molecule, driver, max, 0, 1, rings / 2 + 1,
  element1, element2, x0, z0, x1, z1, x2, y2, z2, angle_start + GAMGI_MATH_PI / 6);
else
  static_1h6h6ph_complex (molecule, driver, max, 0, 0, rings / 2 + 1,
  element1, element2, x0, z0, x1, z1, x2, y2, z2, angle_start);
}

int gamgi_chem_graphene_index (int size_h,
int center_h, int center_v, int h, int v, int offset)
{
/*********************************************
 * h, v = horizontal, vertical coordinates   *
 *                                           *
 * each cell has two atoms (offset = 0 or 1) *
 *********************************************/

return 2 * (size_h * (v + center_v) + h + center_h) + offset;
}

void gamgi_chem_graphene_vectors (double bond, double *a, double *b)
{
/****************************
 * graphene lattice vectors *
 ****************************/

a[0] = bond * sqrt (3.0);
a[1] = 0.0;
b[0] = -0.5 * bond * sqrt (3.0);
b[1] = 1.5 * bond;
}

void gamgi_chem_graphene_hydrogen (gamgi_molecule *molecule)
{
}

void gamgi_chem_graphene_sheet (gamgi_molecule *molecule, int n1,
int n2, int rings, int element1, int element2, double bond)
{
gamgi_atom **driver = NULL;
double perimeter, angle, height;
int max;

/*************************************************************************
 * build a graphene sheet, as an hexagonal lattice or a rectangular tube *
 *************************************************************************/

if (rings < 0)
  {
  driver = static_lattice (molecule,
  n1, n2, element1, element2, bond);
  }
else
  {
  driver = gamgi_chem_graphene_tube_driver (n1, n2,
  rings, bond, &max, &perimeter, &angle, &height);

  gamgi_chem_graphene_tube_2d (molecule, driver, max,
  n1, n2, perimeter, angle, height, element1, element2, bond);
  }

/***********************
 * bond isolated atoms *
 ***********************/

gamgi_chem_graphene_hydrogen (molecule);

free (driver);
}

void gamgi_chem_graphene_cage (gamgi_molecule *molecule,
int mode, int rings, int element1, int element2, double bond)
{
gamgi_atom **driver = NULL;
double perimeter, angle, height;
int max;

/*********************************************************
 * build a fullerene cage formed by a tube with two caps *
 *********************************************************/

switch (mode)
  {
  case GAMGI_CHEM_GRAPHENE_CAGE_5_0:
  driver = gamgi_chem_graphene_tube_driver (5, 0,
  rings, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  5, 0, perimeter, angle, height, element1, element2, bond);
  static_caps_5_0 (molecule, driver, max, rings,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAGE_5_5:
  driver = gamgi_chem_graphene_tube_driver (5, 5,
  rings, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  5, 5, perimeter, angle, height, element1, element2, bond);
  static_caps_5_5 (molecule, driver, max, rings,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAGE_6_6_S:
  driver = gamgi_chem_graphene_tube_driver (6, 6,
  rings, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  6, 6, perimeter, angle, height, element1, element2, bond);
  static_caps_6_6_simple (molecule, driver, max, rings,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAGE_6_6_C:
  driver = gamgi_chem_graphene_tube_driver (6, 6,
  rings, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  6, 6, perimeter, angle, height, element1, element2, bond);
  static_caps_6_6_complex (molecule, driver, max, rings,
  perimeter, height, element1, element2, bond);
  break;
  }

free (driver);
}

void gamgi_chem_graphene_cap (gamgi_molecule *molecule,
int mode, int element1, int element2, double bond)
{
gamgi_atom **driver = NULL;
double perimeter, angle, height;
int max;

/*************************
 * build a fullerene cap *
 *************************/

switch (mode)
  {
  case GAMGI_CHEM_GRAPHENE_CAP_5_0:
  driver = gamgi_chem_graphene_tube_driver (5, 0,
  0, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  5, 0, perimeter, angle, height, element1, element2, bond);
  static_caps_5_0 (molecule, driver, max, -1,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAP_5_5:
  driver = gamgi_chem_graphene_tube_driver (5, 5,
  0, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  5, 5, perimeter, angle, height, element1, element2, bond);
  static_caps_5_5 (molecule, driver, max, -1,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAP_6_6_S:
  driver = gamgi_chem_graphene_tube_driver (6, 6,
  0, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  6, 6, perimeter, angle, height, element1, element2, bond);
  static_caps_6_6_simple (molecule, driver, max, -1,
  perimeter, height, element1, element2, bond);
  break;

  case GAMGI_CHEM_GRAPHENE_CAP_6_6_C:
  driver = gamgi_chem_graphene_tube_driver (6, 6,
  0, bond, &max, &perimeter, &angle, &height);
  gamgi_chem_graphene_tube_3d (molecule, driver, max,
  6, 6, perimeter, angle, height, element1, element2, bond);
  static_caps_6_6_complex (molecule, driver, max, -1,
  perimeter, height, element1, element2, bond);
  break;
  }

free (driver);
}
