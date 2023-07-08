/*******************************************************
 *
 * $GAMGI/src/gtk/molecule/gamgi_gtk_molecule_create.h
 *
 * Copyright (C) 2016 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_length (GtkWidget *widget, void *data);

static void static_height (GtkWidget *widget, void *data);

static void static_angle (GtkWidget *widget, void *data);

static void static_start_atoms (gamgi_window *window);

static void static_start_sheet (gamgi_window *window);

static void static_start_tube (gamgi_window *window);

static void static_start_cone (gamgi_window *window);

static void static_start_cage (gamgi_window *window);

static void static_start_cap (gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static gamgi_bool static_ok_atoms (gamgi_molecule *molecule,
gamgi_window *window, int *element1, int *element2, double *bond);

static void static_ok_object (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok_sheet (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok_tube (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok_cone (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok_cage (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok_cap (gamgi_molecule *molecule, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

**********************
 * external functions *
 **********************/

void gamgi_gtk_molecule_create (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

These functions build the interface to create either a low-level molecule
object that can subsequently be linked to other objects such as atoms, or
high-level molecules using a wide range of structural models. Currently
only high-level models to implement graphene-based structures such as
sheets, tubes, cones, cages and caps are implemented - July 2016.

 *************
 * Rationale *
 *************/
