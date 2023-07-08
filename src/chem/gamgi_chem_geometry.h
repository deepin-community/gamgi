/******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_geometry.h
 *
 * Copyright (C) 2007 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static int static_compare_length (const void *a, const void *b);

static int static_compare_angle (const void *a, const void *b);

static int static_compare_torsion (const void *a, const void *b);

static void static_report_length (gamgi_geometry *geometry,
int n, char *token, int *occurrences, int *offset);

static void static_report_angle (gamgi_geometry *geometry, 
int n, char *token, int *occurrences, int *offset);

static void static_report_torsion (gamgi_geometry *geometry, 
int n, char *token, int *occurrences, int *undefined, int *offset);

static void static_data (gamgi_geometry *geometry, int offset,
int occurrences, double *min, double *max, double *average,
double *deviation);

static void static_report (gamgi_geometry *geometry, GtkWidget *text);

static void static_measure_length (gamgi_atom *atom1, gamgi_atom *atom2, 
gamgi_geometry *geometry);

static void static_measure_angle (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_geometry *geometry);

static void static_measure_torsion (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_atom *atom4, gamgi_geometry *geometry);

static void static_level4 (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_atom *atom4, gamgi_geometry *geometry);

static void static_level3 (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_geometry *geometry);

static void static_level2 (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_geometry *geometry);

static void static_level1 (gamgi_atom *atom1, gamgi_geometry *geometry);

static void static_measure (gamgi_geometry *geometry);

static gamgi_geometry *static_start (gamgi_object *object);

static void static_end (gamgi_geometry *geometry);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_chem_geometry (gamgi_object *object, GtkWidget *text,
gamgi_layer *layer);
