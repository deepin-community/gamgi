/***************************************
 *
 * $GAMGI/src/math/gamgi_math_modify.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_position.h"
#include "gamgi_math_cell.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"

static int static_mark (gamgi_atom *atom, gamgi_slist *slist_neighbour)
{
gamgi_slist *slist_branch;
gamgi_bond *bond;
gamgi_atom *atom_new;
gamgi_dlist *dlist_bond;

/******************************************************************
 * Initially all atoms are marked 0. Then atoms that should       *
 * be left unmodified are marked -1 (second, third, fourth atoms) *
 * and atoms to modify are marked +1 (first atom). All neighbours *
 * from the first atom are then considered in turn and their      *
 * branches are recursively scanned. If an atom marked 0 is       *
 * found, it is marked +1 and return FALSE, if a +1 atom is       *
 * found, that branch has already been scanned and return FALSE,  *
 * if a -1 atom is found, the whole branch is marked -1 and       *
 * returns TRUE. In the end, only atoms marked +1 are modified.   *
 ******************************************************************/

if (atom->mark == 1) return FALSE;

if (atom->mark == -1)
  {
  slist_branch = GAMGI_CAST_SLIST slist_neighbour->data;
  while (slist_branch != NULL)
    {
    atom = GAMGI_CAST_ATOM slist_branch->data;
    atom->mark = -1;
    slist_branch = slist_branch->next;
    }

  return TRUE;
  }

/********************************************************
 * mark atom and scan recursively its bonded neighbours *
 ********************************************************/

atom->mark = 1;
slist_branch = GAMGI_CAST_SLIST slist_neighbour->data;
slist_branch = gamgi_engine_slist_add_start (slist_branch);
slist_neighbour->data = slist_branch;
slist_branch->data = atom;

dlist_bond = atom->bond_start;
while (dlist_bond != NULL)
  {
  bond = GAMGI_CAST_BOND dlist_bond->data;
  atom_new = bond->atom1; if (atom_new == atom) atom_new = bond->atom2;

  if (static_mark (atom_new, slist_neighbour) == TRUE) return TRUE;
  dlist_bond = dlist_bond->next;
  }

return FALSE;
}

/******************** internal function ****************
 *                                                     *
 *                      STATIC_UNMARK                  *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_unmark (gamgi_slist *slist_neighbour)
{
gamgi_atom *atom;
gamgi_slist *slist_branch;

/***********************************************
 * remove lists slist_neighbour, slist_branch, * 
 * marking all atoms 0, as in the beginning    *
 ***********************************************/

while (slist_neighbour != NULL)
  {
  slist_branch = GAMGI_CAST_SLIST slist_neighbour->data;
  while (slist_branch != NULL)
    {
    atom = GAMGI_CAST_ATOM slist_branch->data;
    atom->mark = 0;
    slist_branch = gamgi_engine_slist_remove_start (slist_branch);
    }

  slist_neighbour = gamgi_engine_slist_remove_start (slist_neighbour);
  }
}

static void static_translate (gamgi_atom *atom, double *translate)
{
atom->position[0] += translate[0];
atom->position[1] += translate[1];
atom->position[2] += translate[2];
}

static void static_translate_list (gamgi_slist *slist_neighbour, double *translate)
{
gamgi_atom *atom;
gamgi_slist *slist_branch;

slist_branch = GAMGI_CAST_SLIST slist_neighbour->data;
while (slist_branch != NULL)
  {
  atom = GAMGI_CAST_ATOM slist_branch->data;
  static_translate (atom, translate);
  slist_branch = slist_branch->next;
  }
}

static void static_rotate (gamgi_atom *atom, double *origin, double *rotate)
{
double v1[3], v2[3];

v1[0] = atom->position[0];
v1[1] = atom->position[1];
v1[2] = atom->position[2];

gamgi_math_vector_sub (v1, origin, v1);
gamgi_math_matrix_vector (rotate, v1, v2);
gamgi_math_vector_add (v2, origin, v2);

atom->position[0] = v2[0];
atom->position[1] = v2[1];
atom->position[2] = v2[2];
}

static void static_rotate_list (gamgi_slist *slist_neighbour, double *origin, double *rotate)
{
gamgi_atom *atom;
gamgi_slist *slist_branch;

slist_branch = GAMGI_CAST_SLIST slist_neighbour->data;
while (slist_branch != NULL)
  {
  atom = GAMGI_CAST_ATOM slist_branch->data;
  static_rotate (atom, origin, rotate);
  slist_branch = slist_branch->next;
  }
}

gamgi_bool gamgi_math_modify_get_length_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, double length, double *translate, gamgi_layer *layer)
{
double old;

gamgi_math_vector_absolute (translate,
atom1->position[0] - atom2->position[0],
atom1->position[1] - atom2->position[1],
atom1->position[2] - atom2->position[2]);

old = gamgi_math_vector_length (translate);
if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

gamgi_math_vector_scale (translate, translate, length/old - 1.0);

return TRUE;
}

gamgi_bool gamgi_math_modify_get_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double new, double *translate, 
gamgi_layer *layer)
{
double pole[3];
double old;
 
/***********************************
 * get unit vector along direction *
 ***********************************/

gamgi_math_vector_absolute (pole,
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1],
atom3->position[2] - atom2->position[2]);
 
old = gamgi_math_vector_length (pole);
if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
gamgi_math_vector_scale (pole, pole, 1/old);

/**********************************************
 * get vector translate, from direction to    *
 * atom, and project it normal to unit vector *
 *                                            *
 * the difference between translate and       *
 * projected vectors is a vector normal       *
 * to direction, going from direction to atom *
 **********************************************/

gamgi_math_vector_sub (atom1->position, atom2->position, translate);
old = gamgi_math_vector_dot (translate, pole);
if (old < 0) gamgi_math_vector_scale (pole, pole, -1.0);

gamgi_math_vector_scale (pole, pole, fabs (old));

gamgi_math_vector_sub (translate, pole, translate);
old = gamgi_math_vector_length (translate);
if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

gamgi_math_vector_scale (translate, translate, new/old - 1.0);

return TRUE;
}

gamgi_bool gamgi_math_modify_get_length_atom_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double new,
double *translate, gamgi_layer *layer)
{
double v1[3], v2[3];
double normal[3];
double old;

/***********************************
 * get unit vector normal to plane *
 ***********************************/

gamgi_math_vector_absolute (v1,
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1],
atom3->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v2,
atom4->position[0] - atom2->position[0],
atom4->position[1] - atom2->position[1],
atom4->position[2] - atom2->position[2]);

gamgi_math_vector_cross (v1, v2, normal);
old = gamgi_math_vector_length (normal);
if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
gamgi_math_vector_scale (normal, normal, 1/old);

/**************************************
 * project distance along unit vector *
 **************************************/

gamgi_math_vector_sub (atom1->position, atom2->position, translate);
old = gamgi_math_vector_dot (translate, normal);
if (old < 0) gamgi_math_vector_scale (normal, normal, -1.0);
gamgi_math_vector_scale (normal, translate, new - fabs (old));

return TRUE;
}

gamgi_bool gamgi_math_modify_get_length_atom_direction (gamgi_atom *atom,
gamgi_direction *direction, double new, double *translate,
gamgi_layer *layer)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], normal[3];
double r1[3], r2[3];
double old;

/**********************************************
 *   get unit vector along rotated direction  *
 *                                            *
 * get vector translate, from direction to    *
 * atom, and project it normal to unit vector *
 *                                            *
 * the difference between translate and       *
 * projected vectors is a vector normal       *
 * to direction, going from direction to atom *
 *                                            *
 *        rotate yes, normalize yes           *
 **********************************************/

if (direction->model == GAMGI_PHYS_LINE)
  {
  /**************************************
   *    a parent cell is not needed     *
   *                                    *
   * line direction: use true direction *
   * given by transformed points r1, r2 *
   **************************************/

  gamgi_phys_direction_points (direction, r1, r2);
  gamgi_math_vector_sub (r2, r1, normal);
  gamgi_math_vector_normal (normal);
  }
else
  {
  /**************************************
   *      a parent cell is needed       *
   *                                    *
   * projected direction: use direction *
   * passing through the cell origin    *
   **************************************/

  object = GAMGI_CAST_OBJECT direction->object.object;
  if (object->class != GAMGI_ENGINE_CELL) return FALSE;
  cell = GAMGI_CAST_CELL object;

  gamgi_phys_direction_vector (cell, direction->uvw, direction->vectors, pole);
  gamgi_math_cell_vector_rotate (cell, pole, normal);

  gamgi_math_position_cell_translation_get (cell, r1);
  }

gamgi_math_vector_sub (atom->position, r1, translate);
old = gamgi_math_vector_dot (translate, normal);
if (old < 0) gamgi_math_vector_scale (normal, normal, -1.0);

gamgi_math_vector_scale (normal, pole, fabs (old));

gamgi_math_vector_sub (translate, pole, translate);
old = gamgi_math_vector_length (translate);
if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;

gamgi_math_vector_scale (translate, translate, new/old - 1.0);

return TRUE;
}

gamgi_bool gamgi_math_modify_get_length_atom_plane (gamgi_atom *atom,
gamgi_plane *plane, double new, double *translate, gamgi_layer *layer)
{
gamgi_cell *cell;
gamgi_object *object;
double pole[3], normal[3];
double origin[3];
double r1[3];
double old;

/***************************************
 * get unit vector normal to rotated   *
 * plane (cell can be in direct or     *
 * reciprocal space)                   *
 *                                     *
 * get vector translate, from plane to *
 * atom, and project along unit vector *
 *                                     *
 *     rotate yes, normalize yes       *
 ***************************************/

if (plane->model == GAMGI_PHYS_POLYGON)
  {
  /******************************************************
   * polygon planes: 1) use transformed center to enter *
   * the true plane position, for any plane order; 2)   *
   * get plane normal using only the polygon points     *
   ******************************************************/

  gamgi_math_vector_sub (atom->position, plane->center, translate);
  if (gamgi_phys_plane_normal (plane, normal) == FALSE) return FALSE;
  old = gamgi_math_vector_dot (translate, normal);
  if (old < 0) gamgi_math_vector_scale (normal, normal, -1.0);
  gamgi_math_vector_scale (normal, translate, new - fabs (old));
  }
else
  {
  /***********************************************
   * crystallographic plane (in reciprocal space *
   * or projection): the parent must be a cell   *
   ***********************************************/

  object = GAMGI_CAST_OBJECT plane->object.object;
  if (object->class != GAMGI_ENGINE_CELL) return FALSE;
  cell = GAMGI_CAST_CELL object;

  /********************
   * reciprocal space *
   ********************/

  if (plane->model == GAMGI_PHYS_VECTOR)
    {
    /********************
     * reciprocal space *
     ********************/

    if (cell->reciprocal == 0.0) return FALSE;
    gamgi_phys_plane_point (plane, cell, r1);
    gamgi_math_vector_sub (atom->position, r1, translate);
    old = gamgi_math_vector_length (translate);
    if (old < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
    gamgi_math_vector_scale (translate, translate, new / old - 1.0);
    }
  else
    {
    /***********************************
     * projected planes: use plane     *
     * passing through the cell origin *
     ***********************************/

    gamgi_phys_plane_vector (cell, plane->hkl, plane->vectors, pole);
    gamgi_math_cell_vector_rotate (cell, pole, normal);

    gamgi_math_position_cell_translation_get (cell, origin);
    gamgi_math_vector_sub (atom->position, origin, translate);
    old = gamgi_math_vector_dot (translate, normal);
    if (old < 0) gamgi_math_vector_scale (normal, normal, -1.0);
    gamgi_math_vector_scale (normal, translate, new - fabs (old));
    }
  }

return TRUE;
}

gamgi_bool gamgi_math_modify_get_angle_3atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, double new,
double *origin, double *rotate, gamgi_layer *layer)
{
double v21[3], v23[3], axis[3], view[3];
double old, angle, axis_length;

gamgi_math_vector_absolute (origin, 
atom2->position[0], atom2->position[1], atom2->position[2]);

gamgi_math_vector_absolute (v21, 
atom1->position[0] - atom2->position[0],
atom1->position[1] - atom2->position[1], 
atom1->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v23, 
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1], 
atom3->position[2] - atom2->position[2]);

gamgi_math_vector_cross (v21, v23, axis);
axis_length = gamgi_math_vector_length (axis);

/***********************
 * 3 atoms are aligned *
 ***********************/

if (axis_length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
else
  {
  /*************** 
   * normal case *
   ***************/

  gamgi_math_vector_scale (axis, axis, 1.0/axis_length);

  old = gamgi_math_vector_angle (v21, v23);

  /********************************************
   * Define a vector going from the position  *
   * where the user is to the image center.   *
   * Clearly this vector points away from the *
   * user and is perpendicular to the screen. *
   ********************************************/

  gamgi_math_vector_sub (layer->center, layer->eye, view);

  /***********************************************************
   * the angle to rotate should be:                          *
   *    angle = new - old;                                   *
   * if axis is on the other side, the rotation is reversed: *
   *    angle = old - new;                                   *
   * if axis is pointing to us, then old is negative:        *
   *    angle = new + old;                                   *
   ***********************************************************/

  if (gamgi_math_vector_dot (axis, view) > 0)
    angle = old - new;
  else
    angle = old + new;

  gamgi_math_matrix_rotation (angle, axis, rotate);
  }

return TRUE;
}

gamgi_bool gamgi_math_modify_get_angle_4atom (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, 
double new, double *origin, double *rotate, gamgi_layer *layer)
{
double v23[3], v21[3], v34[3], v32[3];
double w1[3], w2[3], axis[3], view[3];
double old, angle, w1_length, w2_length, axis_length;

gamgi_math_vector_absolute (origin, 
atom2->position[0], atom2->position[1], atom2->position[2]);

gamgi_math_vector_absolute (v23, 
atom3->position[0] - atom2->position[0],
atom3->position[1] - atom2->position[1], 
atom3->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v21, 
atom1->position[0] - atom2->position[0],
atom1->position[1] - atom2->position[1], 
atom1->position[2] - atom2->position[2]);

gamgi_math_vector_absolute (v34, 
atom4->position[0] - atom3->position[0],
atom4->position[1] - atom3->position[1], 
atom4->position[2] - atom3->position[2]);

gamgi_math_vector_absolute (v32, 
atom2->position[0] - atom3->position[0],
atom2->position[1] - atom3->position[1], 
atom2->position[2] - atom3->position[2]);

gamgi_math_vector_cross (v23, v21, w1);
gamgi_math_vector_cross (v34, v32, w2);

w1_length = gamgi_math_vector_length (w1);
w2_length = gamgi_math_vector_length (w2);

/****************************
 * 3 or 4 atoms are aligned *
 ****************************/

if (w1_length < GAMGI_MATH_TOLERANCE_LENGTH || 
w2_length < GAMGI_MATH_TOLERANCE_LENGTH) return FALSE;
else
  {
  /********************************************
   * Define a vector going from the position  *
   * where the user is to the image center.   *
   * Clearly this vector points away from the *
   * user and is perpendicular to the screen. *
   ********************************************/

  gamgi_math_vector_sub (layer->center, layer->eye, view);

  gamgi_math_vector_cross (w1, w2, axis);
  axis_length = gamgi_math_vector_length (axis);

  if (axis_length < GAMGI_MATH_TOLERANCE_LENGTH) 
    {
    /************************
     * old = 0 || old = 180 *
     ************************/

    if (gamgi_math_vector_dot (w1, w2) > 0)
      old = 0.0;
    else
      old = 180.0;

    gamgi_math_vector_copy (v23, axis);
    gamgi_math_vector_normal (axis);

    /***********************************************************
     * the angle to rotate should be:                          *
     *    angle = new - old;                                   *
     * if axis is on the other side, the rotation is reversed: *
     *    angle = old - new;                                   *
     ***********************************************************/

     if (gamgi_math_vector_dot (axis, view) > 0)
       angle = old - new;
     else
       angle = new - old;
    }
  else
    { 
    /***************
     * normal case *
     ***************/

    gamgi_math_vector_scale (axis, axis, 1.0/axis_length);
    old = gamgi_math_vector_angle (w1, w2);
  
    /***********************************************************
     * the angle to rotate should be:                          *
     *    angle = new - old;                                   *
     * if axis is on the other side, the rotation is reversed: *
     *    angle = old - new;                                   *
     * if axis is pointing to us, then old is negative:        *
     *    angle = new + old;                                   *
     ***********************************************************/

    if (gamgi_math_vector_dot (axis, view) > 0)
      angle = old - new;
    else
      angle = old + new;
    }
  gamgi_math_matrix_rotation (angle, axis, rotate);
  }

return TRUE;
}

void gamgi_math_modify_set_length_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, double *translate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_translate (atom1, translate);

/**********************************
 * change atom1 terminal branches *
 **********************************/

atom1->mark = +1;
atom2->mark = -1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_translate_list (slist_neighbour, translate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;
atom2->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_length_atom_2atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *translate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_translate (atom1, translate);

/**********************************
 * change atom1 terminal branches *
 **********************************/

atom1->mark = +1;
atom2->mark = -1;
atom3->mark = -1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_translate_list (slist_neighbour, translate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;
atom2->mark = 0;
atom3->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_length_atom_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, gamgi_atom *atom4, double *translate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_translate (atom1, translate);

/**********************************
 * change atom1 terminal branches *
 **********************************/

atom1->mark = +1;
atom2->mark = -1;
atom3->mark = -1;
atom4->mark = -1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_translate_list (slist_neighbour, translate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;
atom2->mark = 0;
atom3->mark = 0;
atom4->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_length_atom_direction (gamgi_atom *atom1,
gamgi_direction *direction1, double *translate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_translate (atom1, translate);

/*****************************
 * change all atom1 branches *
 *****************************/

atom1->mark = +1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_translate_list (slist_neighbour, translate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_length_atom_plane (gamgi_atom *atom1,
gamgi_plane *plane1, double *translate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_translate (atom1, translate);

/*****************************
 * change all atom1 branches *
 *****************************/

atom1->mark = +1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_translate_list (slist_neighbour, translate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_angle_3atom (gamgi_atom *atom1,
gamgi_atom *atom2, gamgi_atom *atom3, double *origin, double *rotate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_rotate (atom1, origin, rotate);

/**********************************
 * change atom1 terminal branches *
 **********************************/

atom1->mark = +1;
atom2->mark = -1;
atom3->mark = -1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_rotate_list (slist_neighbour, origin, rotate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;
atom2->mark = 0;
atom3->mark = 0;

static_unmark (slist_neighbour);
}

void gamgi_math_modify_set_angle_4atom (gamgi_atom *atom1, gamgi_atom *atom2, 
gamgi_atom *atom3, gamgi_atom *atom4, double *origin, double *rotate)
{
gamgi_atom *atom;
gamgi_bond *bond;
gamgi_slist *slist_neighbour;
gamgi_dlist *dlist;

/****************
 * change atom1 *
 ****************/

static_rotate (atom1, origin, rotate);

/**********************************
 * change atom1 terminal branches *
 **********************************/

atom1->mark = +1;
atom2->mark = -1;
atom3->mark = -1;
atom4->mark = -1;

slist_neighbour = NULL;
for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom1) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_rotate_list (slist_neighbour, origin, rotate);
  }

/**************************************************************
 * change atom2 terminal branches: torsion rotations change   *
 * atom1 and its branches, plus atom2 branches but not atom2! *
 **************************************************************/

atom1->mark = -1;
atom2->mark = +1;

slist_neighbour = NULL;
for (dlist = atom2->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  atom = bond->atom1; if (atom == atom2) atom = bond->atom2;

  slist_neighbour = gamgi_engine_slist_add_start (slist_neighbour);
  slist_neighbour->data = NULL;

  if (static_mark (atom, slist_neighbour) == FALSE)
    static_rotate_list (slist_neighbour, origin, rotate);
  }

/*********
 * clean *
 *********/

atom1->mark = 0;
atom2->mark = 0;
atom3->mark = 0;
atom4->mark = 0;

static_unmark (slist_neighbour);
}
