/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_projection.h
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

void gamgi_phys_projection_wulff (double *spherical, double *planar);

void gamgi_phys_projection_schmidt (double *spherical, double *planar);

void gamgi_phys_projection_create (gamgi_enum model,            
double **points, int *n_points, int **loops, int *n_loops);

void gamgi_phys_projection_pole (gamgi_net net, 
double *direction, double *points, int *loops);

void gamgi_phys_projection_trace (gamgi_net net,
double *direction, double *points, int *loops);

/*************
 * Rationale *
 *************

Needs cleaning.
- 09 Aug 05 -

 *************
 * Rationale *
 *************/
