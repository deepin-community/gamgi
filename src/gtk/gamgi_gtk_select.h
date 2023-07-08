/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_select.h
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

void gamgi_gtk_arrow_select (GtkWidget *widget, void *data);

void gamgi_gtk_shape_select (GtkWidget *widget, void *data);

void gamgi_gtk_graph_select (GtkWidget *widget, void *data);

void gamgi_gtk_select_mode (char *name, gamgi_window *window);

void gamgi_gtk_select_method_bool (char *label, gamgi_window *window);

void gamgi_gtk_select_method_direct (char *label, gamgi_window *window);

void gamgi_gtk_select_method_default (char *label, gamgi_window *window);

gamgi_dlist *gamgi_gtk_select_ok_pick (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

gamgi_dlist *gamgi_gtk_select_ok_region (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

gamgi_bool gamgi_gtk_select_entry_alpha (char *valid,
int length, char *min, char *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_entry_double (double lower,
double upper, double *min, double *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_entry_float (float lower,
float upper, float *min, float *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_entry_int (int lower,
int upper, int *min, int *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_combo_entry_alpha (char *valid,
int length, char *min, char *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_combo_entry_float (float lower,
float upper, float *min, float *max, gamgi_window *window);

gamgi_bool gamgi_gtk_select_combo_entry_int (int lower,
int upper, int *min, int *max, gamgi_window *window);

void gamgi_gtk_select_combo_int (int *min, int *max, gamgi_window *window);

void gamgi_gtk_select_merge (gamgi_dlist **current, gamgi_dlist *start);

GtkTreeIter gamgi_gtk_select_row (GtkTreeStore *store, 
GtkTreeIter *parent, gamgi_object *object);

GtkWidget *gamgi_gtk_select_dialog (gamgi_window *window, 
gamgi_callback2 function_ok);

void gamgi_gtk_select_text (GtkTreeStore *store, 
GtkTreeIter *iter_parent, gamgi_text *text);

void gamgi_gtk_select_orbital (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_orbital *orbital);

void gamgi_gtk_select_bond (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_bond *bond);

void gamgi_gtk_select_atom (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_atom *atom);

void gamgi_gtk_select_direction (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_direction *direction);

void gamgi_gtk_select_plane (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_plane *plane);

void gamgi_gtk_select_group (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_group *group);

void gamgi_gtk_select_molecule (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_molecule *molecule);

void gamgi_gtk_select_cluster (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_cluster *cluster);

void gamgi_gtk_select_cell (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_cell *cell);

void gamgi_gtk_select_arrow (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_arrow *arrow);

void gamgi_gtk_select_shape (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_shape *shape);

void gamgi_gtk_select_graph (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_graph *graph);

void gamgi_gtk_select_assembly (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_assembly *assembly);

void gamgi_gtk_select_light (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_light *light);

void gamgi_gtk_select_layer (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_layer *layer);

void gamgi_gtk_select_window (GtkTreeStore *store,
GtkTreeIter *iter_parent, gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented, needs cleaning.

 *************
 * Rationale *
 *************/
