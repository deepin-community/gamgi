/**************************************************
 *
 * $GAMGI/src/chem/gamgi_chem_graphene_cone.h
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

static double static_circle (double bond, int rings);

static double static_cone (double r_out, double disclination);

static int static_index (int i, int j, int max);

static int static_arc_origin (double x, double y, double r_in, double r_out,
double disclination, double *x_new, double *y_new, double *z_new);

static int static_arc (double x, double y, double o_x, double o_y,
double c_x, double c_y, double r_in, double r_out, double disclination,
double bisector, double min, double max, int negative, int positive,
int side, double *x_new, double *y_new, double *z_new);

static void static_bond (gamgi_atom *atom1,
gamgi_atom **driver, int max, int i, int j, int type);

static int static_update (int index, int update, int type);

static void static_link (gamgi_atom **driver, int max,
int i0, int j0, int k0, int i1, int j1, int k1,
int ii0, int jj0, int kk0, int ii1, int jj1, int kk1);

static int static_atom_0 (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static int static_atom_60_d (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_60_d (gamgi_atom **driver, int max);

static int static_atom_60_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new); 

static void static_bond_60_o (gamgi_atom **driver, int max);

static int static_atom_120_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_120_e (gamgi_atom **driver, int max);

static int static_atom_120_h (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_120_h (gamgi_atom **driver, int max);

static int static_atom_120_s (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_120_s (gamgi_atom **driver, int max);

static int static_atom_120_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_120_o (gamgi_atom **driver, int max);

static int static_atom_180_d (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_180_d (gamgi_atom **driver, int max);

static int static_atom_180_s (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_180_s (gamgi_atom **driver, int max);

static int static_atom_180_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_180_o (gamgi_atom **driver, int max);

static int static_atom_240_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_240_e (gamgi_atom **driver, int max);

static int static_atom_240_h (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_240_h (gamgi_atom **driver, int max);

static int static_atom_240_p (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_240_p (gamgi_atom **driver, int max);

static int static_atom_240_a (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_240_a (gamgi_atom **driver, int max);

static int static_atom_240_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_240_o (gamgi_atom **driver, int max);

static int static_atom_300_e (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_300_e (gamgi_atom **driver, int max);

static int static_atom_300_o (double *a, double *b, double r_in, double r_out,
double x, double y, double *x_new, double *y_new, double *z_new);

static void static_bond_300_o (gamgi_atom **driver, int max);

static void static_atom (gamgi_molecule *molecule, gamgi_atom **driver, int max,
int i, int j, double *a, double *b, double r_in, double r_out, int element,
double x, double y, int type, atom_function function);

static void static_cone_3d (gamgi_molecule *molecule,
atom_function atom_model, bond_function bond_model, int element1,
int element2, double bond, int rings_in, int rings_out);

static void static_optimize (gamgi_molecule *molecule, double bond_length);

static atom_function *static_atom_router (void);

static bond_function *static_bond_router (void);

 **********************
 * external functions *
 **********************/

void gamgi_chem_graphene_cone (gamgi_molecule *molecule, int mode,
int rings_in, int rings_out, int element1, int element2, double bond);

/*************
 * Rationale *
 *************

Under development, last update: August 2016.

 *************
 * Rationale *
 *************/
