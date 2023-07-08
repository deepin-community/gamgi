/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_bond.h
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

void gamgi_mesa_bond_draw_line_1 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_line_2 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_line_3 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_line (gamgi_bond *bond);

void gamgi_mesa_bond_draw_cylinder_1 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_cylinder_3 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_cylinder_5 (gamgi_bond *bond);

void gamgi_mesa_bond_draw_cylinder (gamgi_bond *bond);

/*************
 * Rationale *
 *************

These are the functions that actually draw the bonds,
a function for each style. More styles might be added 
in the future.

 *************
 * Rationale *
 *************/
