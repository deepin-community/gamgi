/********************************************
 *
 * $GAMGI/src/chem/gamgi_chem_bond_create.c
 *
 * Copyright (C) 2000 José Luis Martins
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_chem.h"
#include "gamgi_math.h"
#include "gamgi_global.h"

#include "gamgi_engine_remove.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_gtk_bond_create.h"
#include "gamgi_chem_bond.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_measure.h"

gamgi_dlist *gamgi_chem_bond_create_length (gamgi_window *window,
gamgi_dlist *dlist_atom, double d1, double d2)
{
gamgi_dlist *dlist_bond = NULL;
gamgi_dlist *dlisti, *dlistj;
gamgi_atom *atomi, *atomj;
gamgi_bond *bond;
double d, min, max;

if (d1 < 0 && d2 < 0)
  {
  for (dlisti = dlist_atom; dlisti != NULL; dlisti = dlisti->next)
    {
    atomi = GAMGI_CAST_ATOM dlisti->data;
    for (dlistj = dlisti->next; dlistj != NULL; dlistj = dlistj->next)
      {
      atomj = GAMGI_CAST_ATOM dlistj->data;
      if (gamgi_chem_bond_get (atomi, atomj) != NULL) continue;
      gamgi_math_measure_length_2atom (atomi, atomj, &d);
  
      gamgi_chem_bond_range (gamgi->bond, atomi->element, atomj->element, 
      atomi->radius, atomj->radius, &min, &max);

      if (d < min || d > max) continue;

      /***************
       * create bond *
       ***************/

      bond = gamgi_chem_bond_create (atomi, atomj);

      /********************
       * update bond list *
       ********************/

      dlist_bond = gamgi_engine_dlist_add_start (dlist_bond);
      dlist_bond->data = GAMGI_CAST_OBJECT bond;
      }
    }
  }
else
  {
  for (dlisti = dlist_atom; dlisti != NULL; dlisti = dlisti->next)
    {
    atomi = GAMGI_CAST_ATOM dlisti->data;
    for (dlistj = dlisti->next; dlistj != NULL; dlistj = dlistj->next)
      {
      atomj = GAMGI_CAST_ATOM dlistj->data;
      if (gamgi_chem_bond_get (atomi, atomj) != NULL) continue;
      gamgi_math_measure_length_2atom (atomi, atomj, &d);
      if (d < d1 || d > d2) continue;

      /*************** 
       * create bond *
       ***************/

      bond = gamgi_chem_bond_create (atomi, atomj);

      /********************
       * update bond list *
       ********************/

      dlist_bond = gamgi_engine_dlist_add_start (dlist_bond);
      dlist_bond->data = GAMGI_CAST_OBJECT bond;
      }
    } 
  }

return dlist_bond;
}

/*****************************************
 * Code taken from the subroutine BONDWS *
 *                        Carlos Pereira *
 *****************************************/

gamgi_dlist *gamgi_chem_bond_create_voronoi (gamgi_window *window,
gamgi_dlist *dlist_atom)
{
gamgi_dlist *dlist_bond = NULL;
gamgi_dlist *i, *j, *k;
gamgi_bond *bond;
gamgi_atom *atomi, *atomj, *atomk;
double ijij, ikik, jkjk, ikij, ikjk, ijkj;

/*****************************************
 * Zero or one atoms: no bonds to create *
 *****************************************/

if (dlist_atom == NULL || dlist_atom->next == NULL) return NULL;

/*********************************
 * Two atoms: one bond to create *
 *********************************/

if (dlist_atom->next->next == NULL)
  {
  atomi = GAMGI_CAST_ATOM dlist_atom->data;
  atomj = GAMGI_CAST_ATOM dlist_atom->next->data;

  bond = gamgi_chem_bond_create (atomi, atomj);

  dlist_bond = gamgi_engine_dlist_add_start (dlist_bond);
  dlist_bond->data = bond;
  return dlist_bond;
  }

/************************************
 * Generate all different triangles *
 *   formed by atoms i, j, k        *
 ************************************/

for (i = dlist_atom; i->next->next != NULL; i = i->next)
  {
  atomi = GAMGI_CAST_ATOM i->data;

  for (j = i->next; j->next != NULL; j = j->next)
    {
    atomj = GAMGI_CAST_ATOM j->data;

    for (k = j->next; k != NULL; k = k->next)
      {
      atomk = GAMGI_CAST_ATOM k->data;

      ijij = (atomi->position[0] - atomj->position[0]) * 
             (atomi->position[0] - atomj->position[0]) +
             (atomi->position[1] - atomj->position[1]) * 
             (atomi->position[1] - atomj->position[1]) +
             (atomi->position[2] - atomj->position[2]) * 
             (atomi->position[2] - atomj->position[2]);

      ikik = (atomi->position[0] - atomk->position[0]) * 
             (atomi->position[0] - atomk->position[0]) +
             (atomi->position[1] - atomk->position[1]) * 
             (atomi->position[1] - atomk->position[1]) +
             (atomi->position[2] - atomk->position[2]) * 
             (atomi->position[2] - atomk->position[2]);

      jkjk = (atomj->position[0] - atomk->position[0]) * 
             (atomj->position[0] - atomk->position[0]) +
             (atomj->position[1] - atomk->position[1]) * 
             (atomj->position[1] - atomk->position[1]) +
             (atomj->position[2] - atomk->position[2]) * 
             (atomj->position[2] - atomk->position[2]);
      
      ikij = (atomi->position[0] - atomk->position[0]) * 
             (atomi->position[0] - atomj->position[0]) +
             (atomi->position[1] - atomk->position[1]) * 
             (atomi->position[1] - atomj->position[1]) +
             (atomi->position[2] - atomk->position[2]) * 
             (atomi->position[2] - atomj->position[2]);

      ikjk = (atomi->position[0] - atomk->position[0]) * 
             (atomj->position[0] - atomk->position[0]) +
             (atomi->position[1] - atomk->position[1]) * 
             (atomj->position[1] - atomk->position[1]) +
             (atomi->position[2] - atomk->position[2]) * 
             (atomj->position[2] - atomk->position[2]);

      ijkj = (atomi->position[0] - atomj->position[0]) * 
             (atomk->position[0] - atomj->position[0]) +
             (atomi->position[1] - atomj->position[1]) * 
             (atomk->position[1] - atomj->position[1]) +
             (atomi->position[2] - atomj->position[2]) * 
             (atomk->position[2] - atomj->position[2]);

      /*******************
       * handle j k bond *
       *******************/

      bond = gamgi_chem_bond_get (atomj, atomk);
      if (bond == NULL && i == dlist_atom)
        {
        bond = gamgi_chem_bond_create (atomj, atomk);

        /* protect previous bonds */
        bond->first = FALSE;
        }
      if (bond != NULL && bond->first == FALSE &&
      (ikjk > ikik - GAMGI_MATH_TOLERANCE_LENGTH 
      || ijkj > ijij - GAMGI_MATH_TOLERANCE_LENGTH))
        {
        gamgi_engine_unlink_bond (bond);
        gamgi_engine_remove_bond (bond);
        }

     /*******************
      * handle i k bond *
      *******************/

      bond = gamgi_chem_bond_get (atomi, atomk);
      if (bond == NULL && i == dlist_atom && j == dlist_atom->next)
        {
        /* create bond */
        bond = gamgi_chem_bond_create (atomi, atomk);

        /* protect previous bonds */
        bond->first = FALSE;
        }
      if (bond != NULL && bond->first == FALSE &&
      (ikij > ijij - GAMGI_MATH_TOLERANCE_LENGTH 
      || ikjk > jkjk - GAMGI_MATH_TOLERANCE_LENGTH))
        {
        gamgi_engine_unlink_bond (bond);
        gamgi_engine_remove_bond (bond);
        }

      /*******************
       * handle i j bond *
       *******************/

      bond = gamgi_chem_bond_get (atomi, atomj);
      if (bond == NULL && i == dlist_atom && j == dlist_atom->next && 
      k == dlist_atom->next->next)
        {
        bond = gamgi_chem_bond_create (atomi, atomj);

        /***************************************************
         * New bonds are marked FALSE, to distinguish them *
         * from previous bonds, which are marked TRUE.     *
         ***************************************************/

        bond->first = FALSE;
        }
      if (bond != NULL && bond->first == FALSE &&
      (ikij > ikik - GAMGI_MATH_TOLERANCE_LENGTH 
      || ijkj > jkjk - GAMGI_MATH_TOLERANCE_LENGTH))
        {
        gamgi_engine_unlink_bond (bond);
        gamgi_engine_remove_bond (bond);
        }
      }
    }
  }


/******************************************************
 * Make a list with the new bonds and mark them TRUE *
 ******************************************************/

for (i = dlist_atom; i->next != NULL; i = i->next)
  {
  atomi = GAMGI_CAST_ATOM i->data;

  for (j = i->next; j != NULL; j = j->next)
    {
    atomj = GAMGI_CAST_ATOM j->data;

    bond = gamgi_chem_bond_get (atomi, atomj);
    if  (bond != NULL && bond->first == FALSE)
      {
      bond->first = TRUE;

      dlist_bond = gamgi_engine_dlist_add_start (dlist_bond);
      dlist_bond->data = GAMGI_CAST_OBJECT bond;
      }
    }
  }
return dlist_bond;
}
