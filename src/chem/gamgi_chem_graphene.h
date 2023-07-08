/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene.h
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

 **********************
 * external functions *
 **********************/

int gamgi_chem_graphene_index (int size_h,
int center_h, int center_v, int h, int v, int offset);

void gamgi_chem_graphene_vectors (double bond, double *a, double *b);

void gamgi_chem_graphene_hydrogen (gamgi_molecule *molecule);

void gamgi_chem_graphene_sheet (gamgi_molecule *molecule, int n1,
int n2, int rings, int element1, int element2, double bond);

void gamgi_chem_graphene_cage (gamgi_molecule *molecule,
int mode, int rings, int element1, int element2, double bond);

void gamgi_chem_graphene_cap (gamgi_molecule *molecule,
int mode, int element1, int element2, double bond);

/*************
 * Rationale *
 *************

Implemented, needs cleaning.

 *************
 * Rationale *
 *************/
