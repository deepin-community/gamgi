/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_bond.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_chem.h"
#include "gamgi_math.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_remove.h"
#include "gamgi_chem_atom.h"
#include "gamgi_math_sort.h"
#include "gamgi_expat_export.h"

void gamgi_chem_bond_length (gamgi_bond_class *bond_class)
{
double *min = bond_class->min;
double *max = bond_class->max;
int i;

/***********************************************
 *   bond limits between elements will be      *
 *   ordered according to the sequence         *
 *   1-1, 2-2, 2-1, 3-3, 3-2, 3-1, etc...      *
 *                                             *
 * For a given pair of elements higher-lower,  *
 * the right index will be given by:           *
 *                                             *
 * index = higher * (higher + 1)/2 - lower;    *
 *                                             *
 * For example, min and max limits for 3-2     *
 * elements can be found in array position 4.  *
 ***********************************************/

i = GAMGI_CHEM_ATOM_MAX * (GAMGI_CHEM_ATOM_MAX + 1)/2;

while (i-- > 0)
  {
  min[i] = -1.0;
  max[i] = -1.0;
  }
}

/***************** external function *************
 *                                               *
 *            GAMGI_CHEM_GET_BOND_RANGE          *
 *                                               *
 * Get the default minimum and maximum distance  *
 * between two atoms required to form new bonds. *               
 *************************************************/

void gamgi_chem_bond_range (gamgi_bond_class *bond_class,
int element1, int element2, double radius1, 
double radius2, double *min, double *max)
{
int higher, lower, index;

/***********************************************
 *   bond limits between elements are          *
 *   ordered according to the sequence         *
 *   1-1, 2-2, 2-1, 3-3, 3-2, 3-1, etc...      *
 *                                             *
 * For a given pair of elements higher-lower,  *
 * the right index will be given by:           *
 *                                             *
 * index = higher * (higher + 1)/2 - lower;    *
 *                                             *
 * For example, min and max limits for 3-2     *
 * elements can be found in array position 4.  *
 ***********************************************/

/******************
 * stored min,max *
 ******************/

gamgi_math_sort_int2 (element1, element2, &lower, &higher);
index = higher * (higher + 1)/2 - lower;

*min = bond_class->min[index];
*max = bond_class->max[index];

if (*min > -GAMGI_MATH_TOLERANCE && *max > -GAMGI_MATH_TOLERANCE) return;

/**********************
 * calculated min,max *
 **********************/

*min = (radius1 + radius2) * bond_class->lower;
*max = (radius1 + radius2) * bond_class->upper;

if (radius1 > GAMGI_MATH_TOLERANCE && radius2 > GAMGI_MATH_TOLERANCE) return;

/*******************
 * default min,max *
 *******************/

*min = -1.0;
*max = -1.0;
}

void gamgi_chem_bond_name (gamgi_bond *bond, char *name)
{
int element1, element2;
char name1[4], name2[4];

/******************************************************
 * elements should be sorted to guarantee that the    *
 * bond name contains the highest element name first. *
 ******************************************************/

gamgi_math_sort_int2 (bond->atom1->element, bond->atom2->element, &element1, &element2);

/**************************************
 * get atom names from atomic numbers *
 **************************************/

gamgi_chem_atom_name (element1, name1);
gamgi_chem_atom_name (element2, name2);

/**********************************************
 * the name of the higher element comes first *
 **********************************************/

sprintf (name, "%s%s", name2, name1);
}

gamgi_bond *gamgi_chem_bond_get (gamgi_atom *atom1, gamgi_atom *atom2)
{
gamgi_dlist *dlist;
gamgi_bond *bond;

/************************************
 * Get bond between atom1 and atom2 *
 ************************************/

dlist = atom1->bond_start;
while (dlist != NULL)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  if (atom2 == bond->atom1 || atom2 == bond->atom2) return bond;
  dlist = dlist->next;
  }
return NULL;
}

gamgi_bond *gamgi_chem_bond_create (gamgi_atom *atom1, gamgi_atom *atom2)
{
gamgi_bond *bond;

bond = gamgi_engine_create_bond ();
gamgi_engine_start_bond (bond);
gamgi_engine_link_bond_atom (bond, atom1);
gamgi_engine_link_bond_atom (bond, atom2);

return bond;
}

void gamgi_chem_bond_copy (gamgi_bond *bond, gamgi_bond *bond_new)
{
if (strcmp (bond->object.name, "") == 0)
  gamgi_chem_bond_name (bond_new, bond_new->object.name);
else
  strcpy (bond->object.name, bond_new->object.name);

bond_new->order = bond->order;
bond_new->covalency = bond->covalency;

bond_new->draw = bond->draw;
bond_new->object.scale = bond->object.scale;
bond_new->size = bond->size;

bond_new->color = bond->color;
bond_new->red = bond->red;
bond_new->green = bond->green;
bond_new->blue = bond->blue;
}

void gamgi_chem_bond_half_clean (gamgi_dlist *atom_start)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_dlist *dlist_a, *dlist_b;

for (dlist_a = atom_start; dlist_a != NULL; dlist_a = dlist_a->next)
  {
  atom = GAMGI_CAST_ATOM dlist_a->data;
  for (dlist_b = atom->bond_start; dlist_b != NULL; dlist_b = dlist_b->next)
    {
    bond = GAMGI_CAST_BOND dlist_b->data;
    bond->copy = NULL;
    }
  }
}

void gamgi_chem_bond_half_remove (gamgi_object *scope)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_dlist *atom_start, *dlist_a, *dlist_b;

/***************************************************
 * get a list of all atoms in the scope object:    *
 * as only atoms can own bonds, this list gives    *
 * access to all bonds formed by the scope object. *
 ***************************************************/
   
atom_start = gamgi_engine_dlist_atom_object (scope, NULL);

/*****************************************************
 * scan the bonds from the atoms: bonds that belong  *
 * entirely to the scope object will be visited      *
 * twice, while half-bonds will be visited only once *
 *                                                   *
 * initially bond->first must be TRUE for all bonds: *
 * this is safe as this flag is used to draw bonds   *
 *****************************************************/

for (dlist_a = atom_start; dlist_a != NULL; dlist_a = dlist_a->next)
  {
  atom = GAMGI_CAST_ATOM dlist_a->data;

  dlist_b = atom->bond_start;
  while (dlist_b != NULL)
    {
    bond = GAMGI_CAST_BOND dlist_b->data;
    dlist_b = dlist_b->next;

    if (bond->first == TRUE)
      bond->first = FALSE;
    else
      bond->first = TRUE;
    }
  }

/**********************************************
 *      discard half-bonds, that are not      *
 *      entirely inside the scope object      *
 *                                            *
 * unlinking a bond removes the atom dlist_b  *
 * node, so get the next dlist_b node before! *
 **********************************************/

for (dlist_a = atom_start; dlist_a != NULL; dlist_a = dlist_a->next)
  {
  atom = GAMGI_CAST_ATOM dlist_a->data;

  dlist_b = atom->bond_start;
  while (dlist_b != NULL)
    {
    bond = GAMGI_CAST_BOND dlist_b->data;
    dlist_b = dlist_b->next;

    if (bond->first == FALSE) 
      { 
      gamgi_engine_unlink_bond (bond);
      gamgi_engine_remove_bond (bond);
      }

    }
  }

/************************
 * remove list of atoms *
 ************************/

gamgi_engine_dlist_remove_all (atom_start);
}

void gamgi_chem_bond_export (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *error)
{
gamgi_bond_class *bond_class = gamgi->bond;
gamgi_atom_class *atom_class = gamgi->atom;
double tabulated_min, tabulated_max;
double automatic_min, automatic_max;
double radius1, radius2;
int index, element1, element2;

for (element1 = 1; element1 <= GAMGI_CHEM_ATOM_MAX; element1++)
  {
  for (element2 = element1; element2 > 0; element2--)
    {
    /********************************
     * element1 must be >= element2 *
     ********************************/

    index = element1 * (element1 + 1)/2 - element2;

    tabulated_min = bond_class->min[index];
    tabulated_max = bond_class->max[index];

    if (tabulated_min < -GAMGI_MATH_TOLERANCE
    || tabulated_max < -GAMGI_MATH_TOLERANCE) continue;

    radius1 = atom_class->radius[element1];
    radius2 = atom_class->radius[element2];

    if (radius1 != 0.0 && radius2 != 0.0)
      {
      automatic_min = (radius1 + radius2) * bond_class->lower;
      automatic_max = (radius1 + radius2) * bond_class->upper;

      if (fabs (tabulated_min - automatic_min) < GAMGI_MATH_TOLERANCE
      && fabs (tabulated_max - automatic_max) < GAMGI_MATH_TOLERANCE) continue;
      }

    gamgi_expat_export_bond (window, fp, depth_parent, depth_last, error,
    element1, element2, tabulated_min, tabulated_max);
    }
  }
}
