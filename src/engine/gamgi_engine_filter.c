/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_filter.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_remove.h"
#include "gamgi_math_measure.h"

gamgi_dlist *gamgi_engine_filter_bond_length (gamgi_dlist *start, 
double min, double max)
{
gamgi_atom *atom1, *atom2;
gamgi_bond* bond;
gamgi_dlist *dlist, *dlist_next;
double length;

for (dlist = start; dlist != NULL; dlist = dlist_next)
  {
  dlist_next = dlist->next;

  bond = GAMGI_CAST_BOND dlist->data;
  atom1 = bond->atom1;
  atom2 = bond->atom2;
  gamgi_math_measure_length_2atom (atom1, atom2, &length);
  if (length < min || length > max)
    {
    if (dlist->before == NULL) start = dlist_next;
    gamgi_engine_dlist_remove_start (dlist);
    gamgi_engine_unlink_bond (bond);
    gamgi_engine_remove_bond (bond);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_bond_element (gamgi_dlist *start, 
int z1, int z2)
{
gamgi_bond* bond;
gamgi_dlist *dlist, *dlist_next;
int element1, element2;

for (dlist = start; dlist != NULL; dlist = dlist_next)
  {
  dlist_next = dlist->next;

  bond = GAMGI_CAST_BOND dlist->data;
  element1 = bond->atom1->element;
  element2 = bond->atom2->element;
  if ((z1 != element1 && z1 != element2) || (z2 != element1 && z2 != element2))
    {
    if (dlist->before == NULL) start = dlist_next;
    gamgi_engine_dlist_remove_start (dlist);
    gamgi_engine_unlink_bond (bond);
    gamgi_engine_remove_bond (bond);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_atom_element (gamgi_dlist *start, 
int z1, int z2)
{
gamgi_atom *atom;
gamgi_dlist *dlist, *dlist_next;
int element;

for (dlist = start; dlist != NULL; dlist = dlist_next)
  {
  dlist_next = dlist->next;

  atom = GAMGI_CAST_ATOM dlist->data;
  element = atom->element;
  if (element != z1 && element != z2)
    {
    if (dlist->before == NULL) start = dlist_next;
    gamgi_engine_dlist_remove_start (dlist);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_atom_class (gamgi_dlist *start,
int offset, gamgi_enum class)
{
gamgi_object *object;
gamgi_dlist *dlist, *dlist_next;

/***********************************************
 * unlist objects with specific parent classes *
 ***********************************************/

for (dlist = start; dlist != NULL; dlist = dlist_next)
  {
  dlist_next = dlist->next;

  /********************************************
   * ignore first offset elements of the list *
   ********************************************/

  if (offset-- > 0) continue;
  object = GAMGI_CAST_OBJECT dlist->data;
  if (object->object->class == class)
    {
    if (dlist->before == NULL) start = dlist_next;
    gamgi_engine_dlist_remove_start (dlist);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_atom_parent (gamgi_dlist *start,
int offset, gamgi_object *parent)
{
gamgi_object *object;
gamgi_dlist *dlist, *dlist_next;

/**************************************************
 * unlist objects without specific parent objects *
 **************************************************/

for (dlist = start; dlist != NULL; dlist = dlist_next)
  {
  dlist_next = dlist->next;

  /********************************************
   * ignore first offset elements of the list *
   ********************************************/

  if (offset-- > 0) continue;
  object = GAMGI_CAST_OBJECT dlist->data;
  if (object->object != parent)
    {
    if (dlist->before == NULL) start = dlist_next;
    gamgi_engine_dlist_remove_start (dlist);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_atom_identical (gamgi_dlist *start, int offset)
{
gamgi_atom *atom_i, *atom_j;
gamgi_dlist *i, *j, *j_next;
double x, y, z;

for (i = start; i != NULL; i = i->next)
  {
  /********************************************
   * ignore first offset elements of the list *
   ********************************************/

  if (offset-- > 0) continue;
  atom_i = GAMGI_CAST_ATOM i->data;
  for (j = i->next; j != NULL; j = j_next)
    {
    j_next = j->next;

    atom_j = GAMGI_CAST_ATOM j->data;
    x = atom_i->position[0] - atom_j->position[0];
    y = atom_i->position[1] - atom_j->position[1];
    z = atom_i->position[2] - atom_j->position[2];
    if (x * x + y * y + z * z < 
    GAMGI_MATH_TOLERANCE_STRUCTURE * GAMGI_MATH_TOLERANCE_STRUCTURE)
      gamgi_engine_dlist_remove_start (j);
    }
  }

return start;
}

gamgi_dlist *gamgi_engine_filter_atom (gamgi_dlist *start,
int offset, gamgi_enum filter, gamgi_enum class, gamgi_object *parent)
{
if (filter == GAMGI_CHEM_FILTER_CLASS)
  while (class > GAMGI_ENGINE_ATOM)
    start = gamgi_engine_filter_atom_class (start, offset, class--);

if (filter == GAMGI_CHEM_FILTER_PARENT)
  start = gamgi_engine_filter_atom_parent (start, offset, parent);

if (filter == GAMGI_CHEM_FILTER_IDENTICAL)
  start = gamgi_engine_filter_atom_identical (start, offset);

return start;
}
