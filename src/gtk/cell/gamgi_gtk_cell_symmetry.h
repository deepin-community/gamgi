/*************************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_symmetry.h
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

gamgi_bool static_parameters_check (gamgi_enum lattice, 
double a, double b, double c, double ab, double ac, double bc);

void static_parameters_complete (gamgi_enum lattice,
double *a, double *b, double *c,
double *ab, double *ac, double *bc);

void static_parameters_old (gamgi_cell *cell, 
double *a, double *b, double *c, 
double *ab, double *ac, double *bc);

gamgi_bool static_parameters_valid (double a, double b, 
double c, double ab, double ac, double bc);

gamgi_bool static_angles_valid (double ab, double ac, double bc);

static gamgi_slist *static_list_system (gamgi_window *window);

static gamgi_slist *static_list_lattice (gamgi_window *window);

static gamgi_slist *static_list_group (gamgi_window *window);

static void static_parameters (gamgi_window *window);

static void static_nodes (gamgi_window *window);

static void static_changed_system (GtkWidget *widget, void *data);

static void static_changed_lattice (GtkWidget *widget, void *data);

static void static_changed_group (GtkWidget *widget, void *data);

static void static_row (GtkListStore *store, GtkTreeIter *iter,
char *label0, char *label1, char *label2, char *label3);

static void static_select_system (GtkWidget *widget, void *data);

static void static_select_lattice (GtkWidget *widget, void *data);

static void static_select_group (GtkWidget *widget, void *data);

static void static_remove (GtkWidget *widget, void *data);

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

static void static_system (GtkWidget *widget, void *data);

static void static_lattice (GtkWidget *widget, void *data);

static void static_group (GtkWidget *widget, void *data);

static void static_clicked_system (GtkWidget *widget, void *data);

static void static_clicked_lattice (GtkWidget *widget, void *data);

static void static_clicked_group (GtkWidget *widget, void *data);

gamgi_bool static_get_bravais (char *system,
char *lattice, int group, int *bravais);

static void static_complete (char *system, char *lattice, int group);

gamgi_bool static_get_symmetry (gamgi_window *window,
char *system, char *lattice, int *group);

static void static_set_group (int number, char *group);

static void static_set_system_lattice (int bravais,
char *system, char *lattice);

 **********************
 * external functions *
 **********************/

gamgi_enum gamgi_gtk_cell_symmetry_lattice (GtkWidget *dialog);

void gamgi_gtk_cell_symmetry_nodes (int lattice,
gamgi_enum vectors, GtkWidget *dialog);

gamgi_bool gamgi_gtk_cell_symmetry_parameters (gamgi_cell *cell,
gamgi_enum lattice, double *a, double *b, double *c,
double *ab, double *ac, double *bc);

gamgi_bool gamgi_gtk_cell_symmetry_import (char *system, 
char *lattice, int *group, int *bravais);

void gamgi_gtk_cell_symmetry_export (gamgi_cell *cell,
char *system, char *lattice);

gamgi_bool gamgi_gtk_cell_symmetry_create (int *bravais,
int *group, gamgi_window *window);

void gamgi_gtk_cell_symmetry_press (gamgi_cell *cell, gamgi_window *window);

void gamgi_gtk_cell_symmetry (GtkWidget *notebook, gamgi_window *window);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
