/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_array.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_iarray_start (gamgi_iarray *iarray,
int size, int increment, int dimension);

static void static_darray_start (gamgi_darray *darray,
int size, int increment, int dimension);

 **********************
 * external functions *
 **********************/

gamgi_iarray *gamgi_engine_iarray_create (int total, int step, int dimensions);

gamgi_iarray *gamgi_engine_iarray_recreate (gamgi_iarray *iarray, 
int total, int step, int dimensions);

int *gamgi_engine_iarray_push (gamgi_iarray *iarray, int *vector);

void gamgi_engine_iarray_pop (gamgi_iarray *iarray, int offset);

void gamgi_engine_iarray_remove (gamgi_iarray *iarray);

void gamgi_engine_iarray_copy_remove (gamgi_iarray *iarray,
int **array, int *size);

gamgi_darray *gamgi_engine_darray_create (int total, int step, int dimensions);

gamgi_darray *gamgi_engine_darray_recreate (gamgi_darray *darray, 
int total, int step, int dimensions);

double *gamgi_engine_darray_push (gamgi_darray *darray, double *vector);

void gamgi_engine_darray_pop (gamgi_darray *darray, int offset);

void gamgi_engine_darray_remove (gamgi_darray *darray);

void gamgi_engine_darray_copy_remove (gamgi_darray *darray,
double **array, int *size);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
