/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_bond.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_chem_bond_length (gamgi_bond_class *bond_class);

void gamgi_chem_bond_range (gamgi_bond_class *bond_class,
int element1, int element2, double radius1,
double radius2, double *min, double *max);

void gamgi_chem_bond_name (gamgi_bond *bond, char *name);

gamgi_bond *gamgi_chem_bond_get (gamgi_atom *atom1, gamgi_atom *atom2);

gamgi_bond *gamgi_chem_bond_create (gamgi_atom *atom1, gamgi_atom *atom2);

void gamgi_chem_bond_copy (gamgi_bond *bond, gamgi_bond *bond_new);

void gamgi_chem_bond_half_clean (gamgi_dlist *atom_start);

void gamgi_chem_bond_half_remove (gamgi_object *scope);

void gamgi_chem_bond_export (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *error);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
