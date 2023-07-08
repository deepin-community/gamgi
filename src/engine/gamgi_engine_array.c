/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_array.c 
 *                                         
 * Copyright (C) 2001, 2004 Carlos Pereira  
 *                                    
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

/***************** internal function ********************
 *                                                      *
 ********************************************************/

static void static_iarray_start (gamgi_iarray *iarray,
int size, int increment, int dimension)
{
iarray->size = size;
iarray->increment = increment;
iarray->dimension = dimension;
iarray->offset = 0;
iarray->last = NULL;
}

/***************** internal function ********************
 *                                                      *
 ********************************************************/

static void static_darray_start (gamgi_darray *darray,
int size, int increment, int dimension)
{
darray->size = size;
darray->increment = increment;
darray->dimension = dimension;
darray->offset = 0;
darray->last = NULL;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

gamgi_iarray *gamgi_engine_iarray_create (int size, 
int increment, int dimension)
{
gamgi_iarray *iarray = (gamgi_iarray *) malloc (sizeof (gamgi_iarray));

iarray->array = (int *) malloc (size * dimension * sizeof (int));
static_iarray_start (iarray, size, increment, dimension);

return iarray;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

gamgi_iarray *gamgi_engine_iarray_recreate (gamgi_iarray *iarray,
int size, int increment, int dimension)
{
iarray->array = (int *) realloc (iarray->array, size * dimension * sizeof (int));

iarray->size = size;
iarray->increment = increment;
iarray->dimension = dimension;
iarray->offset = size;

/*******************************
 * set pointer to last element *
 *******************************/

iarray->last = iarray->array + iarray->offset * iarray->dimension;

return iarray;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

int *gamgi_engine_iarray_push (gamgi_iarray *iarray, int *vector)
{
int i;

/***********************************
 * enlarge full array, adding more *
 * (user-controled) step elements  *
 ***********************************/

if (iarray->offset == iarray->size)
  {
  iarray->size += iarray->increment;
  iarray->array = (int *) realloc (iarray->array,
  iarray->size * iarray->dimension * sizeof (int));
  }

/*******************************
 * set pointer to last element *
 *******************************/

iarray->last = iarray->array + iarray->offset * iarray->dimension;

/*****************************
 * copy data to last element *
 *****************************/

if (vector != NULL)
  for (i = 0; i < iarray->dimension; i++)
    iarray->last[i] = vector[i];

/***********************
 * update array offset *
 ***********************/

iarray->offset++;

/**********************************
 * return pointer to last element *
 **********************************/

return iarray->last;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_iarray_pop (gamgi_iarray *iarray, int offset)
{
/************************************************************
 * memory is not deallocated, to avoid expensive reallocs   *
 * when the array is reused, only offset and last are reset *
 ************************************************************/

iarray->offset = offset;

if (offset == 0) iarray->last = NULL;
else iarray->last = iarray->array + (offset - 1) * iarray->dimension;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_iarray_remove (gamgi_iarray *iarray)
{
free (iarray->array);
free (iarray);
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_iarray_copy_remove (gamgi_iarray *iarray,
int **array, int *size)
{
*array = iarray->array;
*size = iarray->offset;

free (iarray);
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

gamgi_darray *gamgi_engine_darray_create (int size, 
int increment, int dimension)
{
gamgi_darray *darray = (gamgi_darray *) malloc (sizeof (gamgi_darray));

darray->array = (double *) malloc (size * dimension * sizeof (double));
static_darray_start (darray, size, increment, dimension);

return darray;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

gamgi_darray *gamgi_engine_darray_recreate (gamgi_darray *darray,
int size, int increment, int dimension)
{
darray->array = (double *) realloc (darray->array, size * dimension * sizeof (double));

darray->size = size;
darray->increment = increment;
darray->dimension = dimension;
darray->offset = size;

/*******************************
 * set pointer to last element *
 *******************************/

darray->last = darray->array + darray->offset * darray->dimension;

return darray;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

double *gamgi_engine_darray_push (gamgi_darray *darray, double *vector)
{
int i;

/***********************************
 * enlarge full array, adding more *
 * (user-controled) step elements  *
 ***********************************/

if (darray->offset == darray->size)
  {
  darray->size += darray->increment;
  darray->array = (double *) realloc (darray->array,
  darray->size * darray->dimension * sizeof (double));
  }

/*******************************
 * set pointer to last element *
 *******************************/

darray->last = darray->array + darray->offset * darray->dimension;

/*****************************
 * copy data to last element *
 *****************************/

if (vector != NULL)
  for (i = 0; i < darray->dimension; i++)
    darray->last[i] = vector[i];

/***********************
 * update array offset *
 ***********************/

darray->offset++;

/**********************************
 * return pointer to last element *
 **********************************/

return darray->last;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_darray_pop (gamgi_darray *darray, int offset)
{
/************************************************************
 * memory is not deallocated, to avoid expensive reallocs   *
 * when the array is reused, only offset and last are reset *
 ************************************************************/

darray->offset = offset;

if (offset == 0) darray->last = NULL;
else darray->last = darray->array + (offset - 1) * darray->dimension;
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_darray_remove (gamgi_darray *darray)
{
free (darray->array);
free (darray);
}

/***************** external function ********************
 *                                                      *
 ********************************************************/

void gamgi_engine_darray_copy_remove (gamgi_darray *darray,
double **array, int *size)
{
*array = darray->array;
*size = darray->offset;

free (darray);
}
