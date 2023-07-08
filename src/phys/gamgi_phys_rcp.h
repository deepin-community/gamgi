/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_rcp.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static double static_pbc_min (double box, double central, double neighbour);

static double static_pbc_out (int slices, double box, int *n);

static double static_pbc_in (double box, double position);

static double static_density (double relaxation);

static double static_diameter (int n_atoms, double density, double *box);

static double static_out (double relaxation, double in, double out, gamgi_rcp *rcp);

static double static_index (int a, int b, int c, int *slices);

static gamgi_slist *static_stock_get (gamgi_rcp *rcp);

static void static_stock_put (gamgi_slist *slist, gamgi_rcp *rcp);

static void static_accumulation (double value, 
double *min, double *max, double *average, double *deviation)

static void static_expectancy (double *average, double *deviation, int n);

static void static_nearest (gamgi_atom *central, int a, 
int b, int c, gamgi_rcp *rcp, gamgi_atom **neighbour, double *min);

static int static_cells_add (gamgi_atom *atom, int *cell, gamgi_rcp *rcp);

static void static_cells_locate (double *position, gamgi_rcp *rcp, int *cell);

static void static_cells_start (gamgi_rcp *rcp);

static void static_cells_move (gamgi_atom *atom, int *old, int *new, gamgi_rcp *rcp);

static void static_cells_end (gamgi_rcp *rcp);

static int static_ranges_add (gamgi_atom *atom1, 
gamgi_atom *atom2, double min, gamgi_rcp *rcp);

void static_ranges_pair (gamgi_rod *rod1, gamgi_rcp *rcp);

static void static_ranges_start (gamgi_rcp *rcp);

gamgi_rod *static_ranges_next (gamgi_rcp *rcp);

static void static_ranges_remove (gamgi_rod *rod, gamgi_rcp *rcp);

static void static_ranges_end (gamgi_rcp *rcp);

static gamgi_rod *static_rods_add (gamgi_atom *atom, gamgi_rcp *rcp);

static void static_rods_start (gamgi_rcp *rcp);

static void static_rods_end (gamgi_rcp *rcp);

static void static_move (gamgi_atom *atom, double *shift, 
gamgi_rcp *rcp, int *old, int *new);

void static_update_old (gamgi_atom *central, gamgi_atom *old, 
int *cell, double out, gamgi_rcp *rcp);

void static_update_new (gamgi_atom *central, gamgi_atom *old,
int *cell, double out, gamgi_rcp *rcp);

void static_separate (gamgi_rod *rod, double out, gamgi_rcp *rcp);

void static_atoms_start (gamgi_atom *atom, int seed, gamgi_rcp *rcp);

int static_atoms_number (gamgi_atom *atom, double *box, double density);

void static_atoms_end (gamgi_cell *cell, double in, gamgi_rcp *rcp);

static gamgi_bool static_scan_obstacle (gamgi_atom *central,
gamgi_atom *neighbour, double *r, double in, gamgi_rcp *rcp);

static void static_scan_neighbour (gamgi_atom *central,
int k, int j, int i, double max, double in, double *g_cn_min,
double *g_cn_max, double *g_cn_average, double *g_cn_deviation,
double *t_cn_min, double *t_cn_max, double *t_cn_average,
double *t_cn_deviation, gamgi_rcp *rcp);

static void static_scan_central (double in, double *g_cn_min,
double *g_cn_max, double *g_cn_average, double *g_cn_deviation,
double *t_cn_min, double *t_cn_max, double *t_cn_average,
double *t_cn_deviation, gamgi_rcp *rcp);

static void static_report (GtkWidget *text, double in, gamgi_rcp *rcp);

static gamgi_rcp *static_start (gamgi_cell *cell, gamgi_atom *atom, 
int n_atoms, int seed, double relaxation);

static void static_end (gamgi_cell *cell, double in, gamgi_rcp *rcp);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_phys_rcp (gamgi_cell *cell, gamgi_atom *atom, 
int n_atoms, int seed, double relaxation, GtkWidget *text);

/*************
 * Rationale *
 *************

Needs integration.
- 12 Dec 05 -

 *************
 * Rationale *
 *************/
