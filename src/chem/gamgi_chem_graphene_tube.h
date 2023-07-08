/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene_tube.h
 *
 * Copyright (C) 2016 Carlos Pereira, Joao Coelho
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_vectors (double *a, double *b,
double angle, double *a_rotated, double *b_rotated);

static double static_height (double bond, int rings, int n1, int n2,
double angle);

static int static_inside (double perimeter, double height, double x, double z);

static void static_fold (gamgi_atom *atom, double perimeter, double x, double z);

static void static_bond (gamgi_atom *atom1,
gamgi_atom **driver, int max, int i, int j, int type);

static void static_atom_2d (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int i, int j, int type, int n1, int n2, double perimeter,
double height, int element, double x, double y);

static void static_atom_3d (gamgi_molecule *molecule, gamgi_atom **driver,
int max, int i, int j, int type, int n1, int n2, double perimeter,
double height, int element, double x, double z);

static void static_bond_2d (gamgi_atom **driver, int max, int n1, int n2);

static void static_bond_3d (gamgi_atom **driver, int max, int n1, int n2);

gamgi_atom **gamgi_chem_graphene_tube_driver (int n1, int n2, int rings,
double bond, int *max, double *perimeter, double *angle, double *height);

void gamgi_chem_graphene_tube_2d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond);

void gamgi_chem_graphene_tube_3d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond);

void gamgi_chem_graphene_tube (gamgi_molecule *molecule, int n1,
int n2, int rings, int element1, int element2, double bond);

 **********************
 * external functions *
 **********************/

gamgi_atom **gamgi_chem_graphene_tube_driver (int n1, int n2, int rings,
double bond, int *max, double *perimeter, double *angle, double *height);

void gamgi_chem_graphene_tube_2d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond);

void gamgi_chem_graphene_tube_3d (gamgi_molecule *molecule,
gamgi_atom **driver, int max, int n1, int n2, double perimeter,
double angle, double height, int element1, int element2, double bond);

void gamgi_chem_graphene_tube (gamgi_molecule *molecule, int n1,
int n2, int rings, int element1, int element2, double bond);

/*************
 * Rationale *
 *************

Under development, last update: August 2016.

 *************
 * Rationale *
 *************/
