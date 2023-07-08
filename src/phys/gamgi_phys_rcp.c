/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_rcp.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/****************************************************
 *                     Bibliography                 *
 *                                                  *
 * W. S. Jodrey and E. M. Tory, Computer simulation *
 * of close random packing of equal spheres,        *
 * Phys. Rev. A, 32-4, 2347 (1985)                  *
 *                                                  *
 * M. Bargiel and J. Moscinski, C-language program  *
 * for the irregular close packing of hard spheres, *
 * Comp. Phys. Comm., 64, 183 (1991)                *
 ****************************************************/

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_link.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_position.h"

int check[1000 * 20];
 
typedef struct _gamgi_rod {
gamgi_atom *central;
gamgi_atom *neighbour;
int range;
double min; } gamgi_rod;

typedef struct _gamgi_rcp {

/***********************************************************
 * array describing the closest interaction for each atom  *
 * array of cells each pointing to a dynamic list of atoms *
 * array describing interactions as a function of distance *
 ***********************************************************/

gamgi_rod **rods;
gamgi_slist **cells;
gamgi_slist **ranges;

/**********************************************
 * pointer to list of nodes used to describe  *
 * old interactions, saved in stock to be     *
 * reused later, to describe new interactions *
 **********************************************/

gamgi_slist *stock;

/************************************
 * total size of provided box       *
 * cell size, at least equal to max *
 * number of cells in box           *
 ************************************/

double box[3];
double width[3];
int slices[3];

/********************************************
 * diameter for 1.0 density                 *
 * squared diameter for 0.74 density        *
 * total number of spheres                  *
 * range index for current smaller distance *
 ********************************************/

double full;
double max;
int n_atoms;
int index; } gamgi_rcp;

static double static_pbc_min (double box, double central, double neighbour)
{
double d;

d = neighbour - central;
if (d > 0 && fabs (d - box) < fabs (d)) d -= box;
if (d < 0 && fabs (d + box) < fabs (d)) d += box;

return d;
}

static double static_pbc_out (int slices, double box, int *n)
{
double s = 0.0;

if (*n < 0) { *n += slices; s = -box; }
if (*n > slices - 1) { *n -= slices; s = box; }

return s;
}

static double static_pbc_in (double box, double position)
{
if (position > box) position -= box;
if (position < 0) position += box;

return position;
}

static double static_density (double relaxation)
{
double density;

/************************
 * linear interpolation *
 *                      *
 *  relaxation density  *
 *    1.0E3 -> 0.625    *
 *    1.0E5 -> 0.645    *
 ************************/

density = 0.625 + (relaxation - 1.0E3) * (0.645 - 0.625) / ( 1.0E5 - 1.0E3);

return density;
}

static double static_diameter (int n_atoms, double density, double *box)
{
double volume;
double diameter;

volume = box[0] * box[1] * box[2];
diameter = 2 * pow (3 * volume * density / (4 * GAMGI_MATH_PI * n_atoms), 1/3.0);

return diameter;
}

static double static_out (double relaxation, double in, double out, gamgi_rcp *rcp)
{
double *box;
double in3, out3;
double volume, density, delta;
int n_atoms;

box = rcp->box;
n_atoms = rcp->n_atoms;

in3 = in * in * in;
out3 = out * out * out;
volume = box[0] * box[1] * box[2];
density = GAMGI_MATH_PI * n_atoms * (out3 - in3) / (6 * volume);
delta = floor (-log10 (density));
out -= pow (1/2.0, delta) * rcp->full / (n_atoms * relaxation);

return out;
}

static double static_index (int a, int b, int c, int *slices)
{
int index;

index = c * slices[0] * slices[1] + b * slices[0] + a;

return index;
}

static gamgi_slist *static_stock_get (gamgi_rcp *rcp)
{
gamgi_slist *new;

/***************************************************
 * to minimize time consuming memory allocation,   *
 * slist nodes are not deallocated when removed    *
 * from the array, they are saved to a stock slist *
 * instead, to be reused when a new node is needed *
 ***************************************************/

if (rcp->stock == NULL)
  new = (gamgi_slist *) malloc (sizeof (gamgi_slist));
else
  { new = rcp->stock; rcp->stock = rcp->stock->next; }

return new;
}

static void static_stock_put (gamgi_slist *slist, gamgi_rcp *rcp)
{
slist->next = rcp->stock;
rcp->stock = slist;
}

static void static_accumulation (double value, 
double *min, double *max, double *average, double *deviation)
{
/*******************************
 * accumulate statistical data *
 *******************************/

if (value < *min) *min = value;
if (value > *max) *max = value;
*average += value;
*deviation += value * value;
}

static void static_expectancy (double *average, double *deviation, int n)
{
/*************************************************************************
 * taking the absolute value is needed to avoid sqrt negative arguments, *
 * which can occur when deviation is 0, due to computer lack of precison *
 *************************************************************************/

*average /= n;
*deviation = sqrt (fabs (*deviation / n - (*average) * (*average)));
}

static void static_nearest (gamgi_atom *central, int a, 
int b, int c, gamgi_rcp *rcp, gamgi_atom **neighbour, double *min)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
double *box;
double shift[3], position[3], r[3];
double length;
int *slices;
int i, j, k;
int ii, jj, kk;
int index;

cells = rcp->cells;
box = rcp->box;
slices = rcp->slices;

/****************************
 * scan all 27 cells around *
 ****************************/

*neighbour = NULL;
*min = DBL_MAX;
for (i = -1; i <= 1; i++)
  {
  ii = c + i;
  shift[2] = static_pbc_out (slices[2], box[2], &ii);

  for (j = -1; j <= 1; j++)
    {
    jj = b + j;
    shift[1] = static_pbc_out (slices[1], box[1], &jj);

    for (k = -1; k <= 1; k++)
      {
      kk = a + k;
      shift[0] = static_pbc_out (slices[0], box[0], &kk);

      gamgi_math_vector_sub (central->position, shift, position);

      index = static_index (kk, jj, ii, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        if (atom == central) continue;

        gamgi_math_vector_sub (atom->position, position, r);
        length = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
        if (length < *min) { *min = length; *neighbour = atom; }
        }

      }
    }
  }
}

static int static_cells_add (gamgi_atom *atom, int *cell, gamgi_rcp *rcp)
{
gamgi_slist **cells;
int index;

cells = rcp->cells;

/********************************
 * add atom to cell linked list *
 ********************************/

index = static_index (cell[0], cell[1], cell[2], rcp->slices);
cells[index] = gamgi_engine_slist_add_start (cells[index]);
cells[index]->data = atom;

return index;
}

static void static_cells_locate (double *position, gamgi_rcp *rcp, int *cell)
{
double *width;
int *slices;

width = rcp->width;
slices = rcp->slices;

/********************************
 * find cell where atom belongs *
 ********************************/

cell[0] = floor (position[0] / width[0]);
if (cell[0] == slices[0]) cell[0]--;
cell[1] = floor (position[1] / width[1]);
if (cell[1] == slices[1]) cell[1]--;
cell[2] = floor (position[2] / width[2]);
if (cell[2] == slices[2]) cell[2]--;
}

static void static_cells_start (gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_atom *atom;
int cell[3];
int n_atoms;
int i;

rods = rcp->rods;
n_atoms = rcp->n_atoms;

for (i = 0; i < n_atoms; i++)
  {
  atom = rods[i]->central;
  static_cells_locate (atom->position, rcp, cell);
  static_cells_add (atom, cell, rcp);
  }

}

static void static_cells_move (gamgi_atom *atom, int *old, int *new, gamgi_rcp *rcp)
{
gamgi_slist **cells;
gamgi_slist *slist, *before;
int index;

/********************************************
 *   check if old, new cells are the same   *
 *                                          *
 * unlist from old cell, list into new cell *
 ********************************************/

if (old[0] == new[0] && old[1] == new[1] && old[2] == new[2]) return;

cells = rcp->cells;

/************************
 * unlist from old cell *
 ************************/

before = NULL;
index = static_index (old[0], old[1], old[2], rcp->slices);
for (slist = cells[index]; slist != NULL; slist = slist->next)
  {
  if (slist->data == atom) break;
  else before = slist;
  }
if (before == NULL) cells[index] = slist->next;
else before->next = slist->next;

/**********************
 * list into new cell *
 **********************/

index = static_index (new[0], new[1], new[2], rcp->slices);
slist->next = cells[index];
cells[index] = slist;
}

static void static_cells_end (gamgi_rcp *rcp)
{
gamgi_slist **cells;
gamgi_slist *slist;
int *slices;
int n_cells;
int i;

cells = rcp->cells;
slices = rcp->slices;
n_cells = slices[0] * slices[1] * slices[2];

for (i = 0; i < n_cells; i++)
  {
  slist = cells[i];
  while (slist != NULL)
    slist = gamgi_engine_slist_remove_start (slist);
  }

free (cells);
}

static int static_ranges_add (gamgi_atom *atom1, 
gamgi_atom *atom2, double min, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_rod *rod;
gamgi_slist **ranges;
gamgi_slist *slist, *before, *new;
int index;

rods = rcp->rods;
ranges = rcp->ranges;

/*********************************************************
 * ranges is an array of slists describing the smaller   *
 * distances between atoms. Each position in the array   *
 * describes an interval of distances and all atom pairs *
 * in that interval are linked to the slist starting at  *
 * that position. Distances between atoms are ordered    *
 * according to increasing positions in the array and    *
 * increasing nodes in each slist.                       *
 *********************************************************/

/**********************
 * find node position *
 **********************/

before = NULL;
index = floor (rcp->n_atoms * min / rcp->max);
for (slist = ranges[index]; slist != NULL; slist = slist->next)
  {
  rod = (gamgi_rod *) slist->data;
  if (min < rod->min) break;
  before = slist;
  }

/************************
 * update minimum index *
 ************************/

if (index < rcp->index) rcp->index = index;

/*******************************
 * get node: from stock or new *
 *******************************/

new = static_stock_get (rcp);

/*************
 * link node *
 *************/

new->next = slist;
if (before == NULL) ranges[index] = new;
else before->next = new;

/************
 * set rods *
 ************/

rod = rods[atom1->mark];
rod->neighbour = atom2;
rod->range = index;

rod = rods[atom2->mark];
rod->neighbour = atom1;
rod->range = index;

/*****************************
 * link node to rod1 or rod2 *
 *****************************/

new->data = (void *) rod;

return index;
}

void static_ranges_pair (gamgi_rod *rod1, gamgi_rcp *rcp)
{
gamgi_rod *rod2;
gamgi_atom *central1, *central2;
gamgi_atom *neighbour1, *neighbour2;

neighbour1 = rod1->neighbour;
if (neighbour1 == NULL) return;
central1 = rod1->central;

rod2 = rcp->rods[neighbour1->mark];

neighbour2 = rod2->neighbour;
if (neighbour2 == NULL) return;
central2 = rod2->central;

if (central1 == neighbour2 && rod1->min < rcp->max)
  static_ranges_add (central1, central2, rod1->min, rcp);
}

static void static_ranges_start (gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_rod *rod;
int n_atoms;
int i;

rods = rcp->rods;
n_atoms = rcp->n_atoms;

/****************************
 * initialize minimum index *
 ****************************/

rcp->index = INT_MAX;

for (i = 0; i < n_atoms; i++)
  {
  rod = rods[i];
  if (rod->range < 0) static_ranges_pair (rods[i], rcp);
  }
  
}

gamgi_rod *static_ranges_next (gamgi_rcp *rcp)
{
gamgi_slist **ranges;
gamgi_rod *rod;
gamgi_slist *slist;
int index;

ranges = rcp->ranges;
index = rcp->index;

if (index == INT_MAX) return NULL;

slist = ranges[index];
rod = (gamgi_rod *) slist->data;

return rod;
}

static void static_ranges_remove (gamgi_rod *rod, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_atom *atom1, *atom2;
gamgi_slist **ranges;
gamgi_slist *slist, *before;
int n_atoms;
int index;

rods = rcp->rods;
ranges = rcp->ranges;
n_atoms = rcp->n_atoms;

atom1= rod->central;
atom2= rod->neighbour;

/**********************
 * find node position *
 **********************/

before = NULL;
index = rod->range;
for (slist = ranges[index]; slist != NULL; slist = slist->next)
  {
  rod = (gamgi_rod *) slist->data;
  if (rod->central == atom1 || rod->central == atom2) break;
  before = slist;
  }

/*************************
 * unlink and stock node *
 *************************/

if (before == NULL) ranges[index] = slist->next;
else before->next = slist->next;

static_stock_put (slist, rcp);

/*******************
 * reset both rods *
 *******************/

rod = rods[atom1->mark];
rod->range = -1;

rod = rods[atom2->mark];
rod->range = -1;

/************************
 * update minimum index *
 ************************/

for (index = rcp->index; index < n_atoms; index++)
  if (ranges[index] != NULL) { rcp->index = index; return; }
rcp->index = INT_MAX;
}

static void static_ranges_end (gamgi_rcp *rcp)
{
gamgi_slist **ranges;
gamgi_slist *slist;
int n_ranges;
int i;

ranges = rcp->ranges;
n_ranges = rcp->n_atoms;

for (i = 0; i < n_ranges; i++)
  {
  slist = ranges[i];
  while (slist != NULL)
    slist = gamgi_engine_slist_remove_start (slist);
  }

free (ranges);
}

static gamgi_rod *static_rods_add (gamgi_atom *atom, gamgi_rcp *rcp)
{
gamgi_rod *rod;

rod = (gamgi_rod *) malloc (sizeof (gamgi_rod));
rod->central = atom;
rod->neighbour = NULL;
rod->min = 0;
rod->range = -1;

return rod;
}

static void static_rods_start (gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_rod *rod;
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
int *slices;
int i, j, k;
int index;

rods = rcp->rods;
cells = rcp->cells;
slices = rcp->slices;

/*********************************************
 * for each atom, find its closest neighbour *
 *********************************************/

for (i = 0; i < slices[2]; i++)
  {
  for (j = 0; j < slices[1]; j++)
    {
    for (k = 0; k < slices[0]; k++)
      {
      index = static_index (k, j, i, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        rod = rods[atom->mark];
        static_nearest (atom, k, j, i, rcp, &rod->neighbour, &rod->min);
        }

      }
    }
  }

}

static void static_rods_end (gamgi_rcp *rcp)
{
gamgi_rod **rods;
int n_rods;
int i;

rods = rcp->rods;
n_rods = rcp->n_atoms;

for (i = 0; i < n_rods; i++) free (rods[i]);

free (rods);
}

static void static_move (gamgi_atom *atom, double *shift, 
gamgi_rcp *rcp, int *old, int *new)
{
double *box, *position;

box = rcp->box;
position = atom->position;

static_cells_locate (position, rcp, old);
gamgi_math_vector_add (position, shift, position);
position[0] = static_pbc_in (box[0], position[0]);
position[1] = static_pbc_in (box[1], position[1]);
position[2] = static_pbc_in (box[2], position[2]);
static_cells_locate (position, rcp, new);
static_cells_move (atom, old, new, rcp);
}

void static_update_old (gamgi_atom *central, 
gamgi_atom *old, int *cell, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_rod *rod;
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
double *box;
double shift[3], position[3], r[3];
double length;
int *slices;
int i, j, k;
int ii, jj, kk;
int index;

rods = rcp->rods;
cells = rcp->cells;
box = rcp->box;
slices = rcp->slices;

/*****************************************
 * scan old cell and the 26 cells around *
 *****************************************/

for (i = -1; i <= 1; i++)
  {
  ii = cell[2] + i;
  shift[2] = static_pbc_out (slices[2], box[2], &ii);

  for (j = -1; j <= 1; j++)
    {
    jj = cell[1] + j;
    shift[1] = static_pbc_out (slices[1], box[1], &jj);

    for (k = -1; k <= 1; k++)
      {
      kk = cell[0] + k;
      shift[0] = static_pbc_out (slices[0], box[0], &kk);

      gamgi_math_vector_sub (central->position, shift, position);

      index = static_index (kk, jj, ii, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        if (atom == central || atom == old) continue;

        rod = rods[atom->mark];
        if (rod->neighbour != central) continue;

        gamgi_math_vector_sub (atom->position, position, r);
        length = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
        if (length <= rod->min) continue;

        /*************************************************************
         * atom is now more far away from central, its neighbour, so *
         * a new neighbour might exist, and new rods might be formed *
         *************************************************************/

        static_nearest (atom, kk, jj, ii, rcp, &rod->neighbour, &rod->min);
        if (rod->neighbour == central) continue;

        /**********************************
         * add range node, if pair exists *
         **********************************/

        static_ranges_pair (rod, rcp);
        }

      }
    }
  }

}

void static_update_new (gamgi_atom *central, 
gamgi_atom *old, int *cell, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_rod *rod;
gamgi_atom *atom;
gamgi_atom *neighbour;
gamgi_slist **cells;
gamgi_slist *slist;
double *box;
double shift[3], position[3], r[3];
double min, length;
int *slices;
int i, j, k;
int ii, jj, kk;
int index;

/**************************************************
 * after moving the atom called central:          *
 * 1) find the new neighbour for the central atom *
 * 2) update the atoms in the new cell that might *
 * be closer now to the central atom than to its  *
 * previous neighbours.                           *
 **************************************************/

rods = rcp->rods;
cells = rcp->cells;
box = rcp->box;
slices = rcp->slices;

/*****************************************
 * scan new cell and the 26 cells around *
 *****************************************/

neighbour = NULL;
min = DBL_MAX;
for (i = -1; i <= 1; i++)
  {
  ii = cell[2] + i;
  shift[2] = static_pbc_out (slices[2], box[2], &ii);

  for (j = -1; j <= 1; j++)
    {
    jj = cell[1] + j;
    shift[1] = static_pbc_out (slices[1], box[1], &jj);

    for (k = -1; k <= 1; k++)
      {
      kk = cell[0] + k;
      shift[0] = static_pbc_out (slices[0], box[0], &kk);

      gamgi_math_vector_sub (central->position, shift, position);

      index = static_index (kk, jj, ii, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        if (atom == central) continue;

        /**************************************
         * find the new neighbour for central *
         **************************************/

        gamgi_math_vector_sub (atom->position, position, r);
        length = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
        if (length < min) { min = length; neighbour = atom; }
        if (atom == old) continue;

        rod = rods[atom->mark];
        if (length < rod->min)
          {
          /*************************************************************
           * atom is closer to central than to its previous neighbour: * 
           * remove ranges node, if it exists, and point to central    *
           *************************************************************/

          if (rod->range >= 0) static_ranges_remove (rod, rcp);
          rod->neighbour = central;
          rod->min = length;
          }
        }

      }
    }
  }

rod = rods[central->mark];
rod->neighbour = neighbour;
rod->min = min;

/**********************************
 * add range node, if pair exists *
 **********************************/

static_ranges_pair (rod, rcp);
}

void static_separate (gamgi_rod *rod, double out, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_atom *atom1, *atom2;
double *r1, *r2;
double *box;
double shift[3];
double length;
int old1[3], old2[3];
int new1[3], new2[3];

rods = rcp->rods;
box = rcp->box;

atom1 = rod->central;
atom2 = rod->neighbour;
static_ranges_remove (rod, rcp);
rods[atom1->mark]->neighbour = NULL;
rods[atom2->mark]->neighbour = NULL;

r1 = atom1->position;
r2 = atom2->position;
shift[0] = static_pbc_min (box[0], r2[0], r1[0]);
shift[1] = static_pbc_min (box[1], r2[1], r1[1]);
shift[2] = static_pbc_min (box[2], r2[2], r1[2]);

length = gamgi_math_vector_length (shift);
gamgi_math_vector_scale (shift, shift, (out / length - 1) / 2);

static_move (atom1, shift, rcp, old1, new1);
gamgi_math_vector_scale (shift, shift, -1.0);
static_move (atom2, shift, rcp, old2, new2);

static_update_old (atom1, atom2, old1, rcp);
static_update_new (atom1, atom2, new1, rcp);

static_update_old (atom2, atom1, old2, rcp);
static_update_new (atom2, atom1, new2, rcp);
}

void static_atoms_start (gamgi_atom *atom, int seed, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_atom *atom_new;
double max = RAND_MAX;
double *box;
int i, n_atoms;

n_atoms = rcp->n_atoms;
rods = rcp->rods;
box = rcp->box;

/****************************************
 * start pseudo-random number generator *
 ****************************************/

srand (seed);

/*************************************
 * generate atoms randomly allocated *
 * 1) create atom 2) number atom     *
 * 3) set position 4) create rod     *
 *************************************/

for (i = 0; i < n_atoms; i++)
  {
  atom_new = gamgi_engine_copy_atom (atom);
  atom_new->mark = i;

  atom_new->position[0] = rand () / max * box[0];
  atom_new->position[1] = rand () / max * box[1];
  atom_new->position[2] = rand () / max * box[2];

  rods[i] = static_rods_add (atom_new, rcp);
  }

}

int static_atoms_number (gamgi_atom *atom, double *box, double density)
{
double volume_box;
double volume_sphere;
double radius;
int n_atoms;

/***********************************************
 * All atoms in GAMGI must have a valid radius *
 ***********************************************/

radius = atom->radius;

volume_box = box[0] * box[1] * box[2];
volume_sphere = 4 * GAMGI_MATH_PI * pow (radius, 3) / 3;
n_atoms = floor (volume_box * density / volume_sphere);

if (n_atoms < GAMGI_PHYS_RCP_ATOMS) return 0;

return n_atoms;
}

void static_atoms_end (gamgi_cell *cell, double in, gamgi_rcp *rcp)
{
gamgi_rod **rods;
gamgi_atom *atom;
double rotate[9];
double translate[3];
double position[3];
int i, n_atoms;

n_atoms = rcp->n_atoms;
rods = rcp->rods;

/*********************************
 * get cell rotation,translation *
 *********************************/

gamgi_math_position_rotation (GAMGI_CAST_OBJECT cell, rotate);
gamgi_math_position_translation (GAMGI_CAST_OBJECT cell, translate);

for (i = 0; i < n_atoms; i++)
  {
  atom = rods[i]->central;

  /************************
   *  remove atom mark    *
   *                      *
   * set new atom radius  *
   ************************/

  atom->mark = 0;
  atom->radius = in / 2.0;

  /*****************************
   * rotate and translate atom *
   *****************************/

  gamgi_math_matrix_vector (rotate, atom->position, position);
  gamgi_math_vector_add (position, translate, atom->position);

  /*********************
   * link atom to cell *
   *********************/

  gamgi_engine_link_atom_cell (atom, cell);
  }
}

static gamgi_bool static_scan_obstacle (gamgi_atom *central, 
gamgi_atom *neighbour, double *r, double in, gamgi_rcp *rcp)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
double *box;
double shift[3], position[3];
double d[3];
double length;
int *slices;
int cell[3];
int i, j, k;
int ii, jj, kk;
int index;

cells = rcp->cells;
box = rcp->box;
slices = rcp->slices;

/*********************************
 * get new cell after atom shift *
 *********************************/

r[0] = static_pbc_in (box[0], r[0]);
r[1] = static_pbc_in (box[1], r[1]);
r[2] = static_pbc_in (box[2], r[2]);
static_cells_locate (r, rcp, cell);

/****************************
 * scan all 27 cells around *
 ****************************/

for (i = -1; i <= 1; i++)
  {
  ii = cell[2] + i;
  shift[2] = static_pbc_out (slices[2], box[2], &ii);

  for (j = -1; j <= 1; j++)
    {
    jj = cell[1] + j;
    shift[1] = static_pbc_out (slices[1], box[1], &jj);

    for (k = -1; k <= 1; k++)
      {
      kk = cell[0] + k;
      shift[0] = static_pbc_out (slices[0], box[0], &kk);

      index = static_index (kk, jj, ii, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        if (atom == central || atom == neighbour) continue;

        gamgi_math_vector_add (atom->position, shift, position);

        gamgi_math_vector_sub (position, r, d);
        length = gamgi_math_vector_length (d);
        if (length < in - GAMGI_PHYS_RCP_TOLERANCE) return FALSE;
        }

      }
    }
  }

return TRUE;
}

static void static_scan_neighbour (gamgi_atom *central, 
int k, int j, int i, double max, double in, double *g_cn_min, 
double *g_cn_max, double *g_cn_average, double *g_cn_deviation,
double *t_cn_min, double *t_cn_max, double *t_cn_average, 
double *t_cn_deviation, gamgi_rcp *rcp)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
double *box;
double shift[3], position[3];
double r[3], r1[3], r2[3];
double length;
int *slices;
int a, b, c;
int ii, jj, kk;
int index;
int g_cn, t_cn;

cells = rcp->cells;
box = rcp->box;
slices = rcp->slices;

/********************
 * start statistics *
 ********************/

g_cn = 0;
t_cn = 0;

/****************************
 * scan all 27 cells around *
 ****************************/

for (c = -1; c <= 1; c++)
  {
  ii = i + c;
  shift[2] = static_pbc_out (slices[2], box[2], &ii);

  for (b = -1; b <= 1; b++)
    {
    jj = j + b;
    shift[1] = static_pbc_out (slices[1], box[1], &jj);

    for (a = -1; a <= 1; a++)
      {
      kk = k + a;
      shift[0] = static_pbc_out (slices[0], box[0], &kk);

      index = static_index (kk, jj, ii, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;
        if (atom == central) continue;

        gamgi_math_vector_add (atom->position, shift, position);

        /*****************************
         * check geometric neighbour *
         *****************************/

        gamgi_math_vector_sub (position, central->position, r);
        length = gamgi_math_vector_length (r);

        if (length < max) g_cn++;
        
        /*******************************
         * check topological neighbour *
         *******************************/

        gamgi_math_vector_scale (r, r, (length - in) / length / 2);
        length = gamgi_math_vector_length (r);
       
        gamgi_math_vector_add (central->position, r, r1);
        if (static_scan_obstacle (central, atom, r1, in, rcp) == FALSE)
          continue;

        gamgi_math_vector_sub (position, r, r2);
        if (static_scan_obstacle (central, atom, r2, in, rcp) == FALSE)
          continue;

        t_cn++;
        }

      }
    }
  }

/***********************************
 * accumulate statistics:          *
 * geometric coordination number   *
 * topological coordination number *
 ***********************************/

static_accumulation (g_cn, g_cn_min, g_cn_max, g_cn_average, g_cn_deviation);
static_accumulation (t_cn, t_cn_min, t_cn_max, t_cn_average, t_cn_deviation);
}

static void static_scan_central (double in, double *g_cn_min,
double *g_cn_max, double *g_cn_average, double *g_cn_deviation,
double *t_cn_min, double *t_cn_max, double *t_cn_average,
double *t_cn_deviation, gamgi_rcp *rcp)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist;
double max;
int *slices;
int i, j, k;
int index;

cells = rcp->cells;
slices = rcp->slices;
max = sqrt (rcp->max);

/********************
 * start statistics *
 ********************/

*g_cn_min = DBL_MAX;
*g_cn_max = -DBL_MAX;
*g_cn_average = 0.0;
*g_cn_deviation = 0.0;

*t_cn_min = DBL_MAX;
*t_cn_max = -DBL_MAX;
*t_cn_average = 0.0;
*t_cn_deviation = 0.0;

/***************************************
 * for each atom, find its closest     *
 * topological and geometric neighbour *
 ***************************************/

for (i = 0; i < slices[2]; i++)
  {
  for (j = 0; j < slices[1]; j++)
    {
    for (k = 0; k < slices[0]; k++)
      {
      index = static_index (k, j, i, slices);
      for (slist = cells[index]; slist != NULL; slist = slist->next)
        {
        atom = (gamgi_atom *) slist->data;

        static_scan_neighbour (atom, k, j, i, max, in, 
        g_cn_min, g_cn_max, g_cn_average, g_cn_deviation,
        t_cn_min, t_cn_max, t_cn_average, t_cn_deviation, rcp);
        }

      }
    }
  }

/***********************************
 * calculate statistics:           *
 * geometric coordination number   *
 * topological coordination number *
 ***********************************/

static_expectancy (g_cn_average, g_cn_deviation, rcp->n_atoms);
static_expectancy (t_cn_average, t_cn_deviation, rcp->n_atoms);
}

static void static_report (GtkWidget *text, double in, gamgi_rcp *rcp)
{
GtkTextBuffer *buffer;
GtkTextIter iter;
double *box;
double volume_box, volume_sphere;
double density;
double radius_min, radius_max;
double g_cn_min, g_cn_max;
double g_cn_average, g_cn_deviation;
double t_cn_min, t_cn_max;
double t_cn_average, t_cn_deviation;

/**************************************
 * no report is produced in List mode *
 **************************************/

if (text == NULL) return;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

/****************
 * measure data *
 ****************/

box = rcp->box;
volume_box = box[0] * box[1] * box[2];
volume_sphere = 4 * GAMGI_MATH_PI * pow (in/2, 3) / 3;
density = volume_sphere * rcp->n_atoms / volume_box;
radius_min = in / 2;
radius_max = sqrt (rcp->max) / 2;

/*************
 * show data *
 *************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "Structure\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Atoms: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", rcp->n_atoms);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Calculated Radius: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", radius_min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum Radius: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", radius_max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Density: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", density);

/*****************************************
 * get topological, geometric neighbours *
 *****************************************/

static_scan_central (in, &g_cn_min, &g_cn_max, &g_cn_average, 
&g_cn_deviation, &t_cn_min, &t_cn_max, &t_cn_average, &t_cn_deviation, rcp);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nGeometrical Neighbours\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.0f\n", g_cn_min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.0f\n", g_cn_max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", g_cn_average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", g_cn_deviation);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nTopological Neighbours\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.0f\n", t_cn_min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.0f\n", t_cn_max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", t_cn_average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", t_cn_deviation);
}

static gamgi_rcp *static_start (gamgi_cell *cell, gamgi_atom *atom, 
int n_atoms, int seed, double relaxation)
{
gamgi_rcp *rcp;
double *width;
double box[3];
double max, density;
int *slices;
int n_cells;

rcp = (gamgi_rcp *) malloc (sizeof (gamgi_rcp));

/***************************
 * use Cell box dimensions *
 ***************************/

box[0] = cell->a; box[1] = cell->b; box[2] = cell->c;
gamgi_math_vector_copy (box, rcp->box);

/**********************************************************
 * predict the final density (depends of relaxation time) *
 *                                                        *
 * given the atom radius, determine the number of atoms   *
 * such that its radius will be approximately the same,   *
 * and make sure this number is an acceptable value       *
 *                                                        *
 * given the number of atoms, predict the final atom      *
 * radius and make sure this is an acceptable value       *
 **********************************************************/

density = static_density (relaxation);
if (n_atoms == 0)
  {
  n_atoms = static_atoms_number (atom, box, density);
  if (n_atoms < GAMGI_PHYS_RCP_ATOMS)
    { free (rcp); return NULL; }
  }
else
  {
  if (static_diameter (n_atoms, density, 
  box) / 2.0 < GAMGI_MATH_TOLERANCE_LENGTH)
    { free (rcp); return NULL; }
  }
rcp->n_atoms = n_atoms;

/*******************************************
 * full density (1.00): save distance to   *
 * cyclically recalculate the out distance *
 *                                         *
 * maximum density (0.74): use distance to *
 * calculate cells width and save squared  *
 * distance to calculate the ranges offset *
 *                                         *
 * minimum image convention: cutoff (max)  *
 * must be smaller than half of the cell   *
 *******************************************/

density = 1.0;
rcp->full = static_diameter (n_atoms, density, box);

density = GAMGI_MATH_PI * sqrt (2) / 6;
max = static_diameter (n_atoms, density, box);
rcp->max = max * max;

if (max >= box[0]/2 || max >= box[1]/2 || max >= box[2]/2)
  { free (rcp); return NULL; }

/*************************
 * number of cell slices *
 *                       *
 * width of cell slices  *
 *************************/

slices = rcp->slices;
slices[0] = floor (box[0] / max);
if (slices[0] == 0) slices[0] = 1;
slices[1] = floor (box[1] / max);
if (slices[1] == 0) slices[1] = 1;
slices[2] = floor (box[2] / max);
if (slices[2] == 0) slices[2] = 1;

width = rcp->width;
width[0] = box[0] / slices[0];
width[1] = box[1] / slices[1];
width[2] = box[2] / slices[2];

/*****************************************************************
 * array of cell lists, array of range lists, array of rod pairs *
 *                                                               *
 *    keep a stock of unused slist nodes for the ranges array    *
 *****************************************************************/

n_cells = slices[0] * slices[1] * slices[2];
rcp->cells = (gamgi_slist **) calloc (n_cells, sizeof (gamgi_slist *));

rcp->ranges = (gamgi_slist **) calloc (n_atoms, sizeof (gamgi_slist *));
rcp->stock = NULL;

rcp->rods = (gamgi_rod **) calloc (n_atoms, sizeof (gamgi_rod *));

/********************************
 * create atoms, rods           *
 * create slists: cells, ranges *
 ********************************/

static_atoms_start (atom, seed, rcp);
static_cells_start (rcp);
static_rods_start (rcp);
static_ranges_start (rcp);

return rcp;
}

static void static_end (gamgi_cell *cell, double in, gamgi_rcp *rcp)
{
/********************************
 * link atoms, remove rods      *
 * remove slists: cells, ranges *
 ********************************/

static_atoms_end (cell, in, rcp);
static_cells_end (rcp);
static_rods_end (rcp);
static_ranges_end (rcp);

free (rcp);
}

gamgi_bool gamgi_phys_rcp (gamgi_cell *cell, gamgi_atom *atom, 
int n_atoms, int seed, double relaxation, GtkWidget *text)
{
gamgi_rcp *rcp;
gamgi_rod *rod;
double in, out;

/*********
 * start *
 *********/

rcp = static_start (cell, atom, n_atoms, seed, relaxation);
if (rcp == NULL) return FALSE;

/*********
 * cycle *
 *********/

rod = static_ranges_next (rcp);
if (rod == NULL) return FALSE;

in = sqrt (rod->min);
out = rcp->full;

while (out - in > GAMGI_PHYS_RCP_TOLERANCE)
  {
  static_separate (rod, out, rcp);

  rod = static_ranges_next (rcp);
  if (rod == NULL) break;

  in = sqrt (rod->min);
  out = static_out (relaxation, in, out, rcp);
  }

/*******
 * end *
 *******/

static_report (text, in, rcp);
static_end (cell, in, rcp);

return TRUE;
}
