/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_export_config.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_expat_export_config_help (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_text (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_orbital (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_bond_global (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_bond_local (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error,
int element1, int element2, double min, double max);

void gamgi_expat_export_config_atom_global (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_atom_local (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error,
int element, double mass, double radius, float red, float green, float blue);

void gamgi_expat_export_config_direction (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_plane (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_group (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_molecule (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_cluster (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_cell (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_arrow (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_shape (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_graph (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_assembly (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_light (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_layer (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_window (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

void gamgi_expat_export_config_gamgi (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *output, gamgi_bool *error);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/
