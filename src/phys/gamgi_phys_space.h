/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_space.h
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

static void static_range (const int *driver, int site, int *start, int *end);

static double static_parser (char *token, double *input);

static void static_generate (double *input, const char **wyckoff,
int start, int n_positions, double *positions);

static static_positive (double *in, double *out);

static gamgi_bool static_equal (double *position_i, double *position_j);

static void static_compare_one (int n_positions,
double *positions, gamgi_bool *equal);

static void static_compare_two (int n_positions,
double *positions, double *positions_new, gamgi_bool *equal);

static int static_count (int n_positions, double *positions, gamgi_bool *equal);

static int static_site (double *input, const char **wyckoff,
const int *driver, int n_positions, double *positions, gamgi_bool *equal);

 **********************
 * external functions *
 **********************/

void gamgi_phys_space_get (int group, const char ***positions,
const char ***symmetry, const int **driver);

int gamgi_phys_space_multiplicity (const int *driver, int site);

char gamgi_phys_space_letter (int n_sites, int site);

void gamgi_phys_space_used (const char **wyckoff,
const int *driver, int site, gamgi_bool *used);

double *gamgi_phys_space_positions (int site, double *xyz, 
const char **wyckoff, const int *driver, int *n_positions, 
gamgi_window *window);

/*************
 * Rationale *
 *************

Needs cleaning.
- 09 Aug 05 -

 *************
 * Rationale *
 *************/

