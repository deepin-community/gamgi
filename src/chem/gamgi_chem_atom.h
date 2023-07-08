/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_atom.h
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

void gamgi_chem_atom_name (int element, char *name);

gamgi_bool gamgi_chem_atom_number (const char *name, int *number);

gamgi_atom *gamgi_chem_atom_create (int element);

gamgi_slist **gamgi_chem_atom_element (void);

void gamgi_chem_atom_mass (gamgi_atom_class *atom_class);

void gamgi_chem_atom_radius (gamgi_atom_class *atom_class);

void gamgi_chem_atom_color (gamgi_atom_class *atom_class);

void gamgi_chem_atom_export (gamgi_window *window, FILE *fp, 
int depth_parent, int *depth_last, gamgi_bool *error);

void gamgi_chem_atom_mass_list (GtkListStore *store, int element);

void gamgi_chem_atom_radius_list (GtkListStore *store, int element);

void gamgi_chem_atom_ionic_list (GtkListStore *store, int element);

void gamgi_chem_atom_clean (gamgi_atom *atom);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
