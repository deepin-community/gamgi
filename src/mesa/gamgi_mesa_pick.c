/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_pick.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_mesa_select.h"
#include "gamgi_mesa_text.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_render.h"
#include "gamgi_mesa_render.h"
#include "gamgi_mesa_axes.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_quaternion.h"

gamgi_object **gamgi_mesa_pick_text (gamgi_text *text, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];
double rotate[16];

if (class == GAMGI_ENGINE_TEXT)
  {
  *array++ = GAMGI_CAST_OBJECT text;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

glTranslatef (text->center[0], text->center[1], text->center[2]);
glScalef (text->object.scale, text->object.scale, text->object.scale);
glTranslatef (-text->center[0], -text->center[1], -text->center[2]);

glPushMatrix ();

glTranslatef (text->origin[0], text->origin[1], text->origin[2]);

gamgi_math_quaternion_to_matrix4 (text->quaternion, rotate);
glMultMatrixd (rotate);

/********************
 * scan text object *
 ********************/

if (text->style == GAMGI_MESA_WIRED) 
  glLineWidth (GAMGI_MESA_STROKED_WIDTH);
gamgi_mesa_text_draw (text);
if (text->style == GAMGI_MESA_WIRED) 
  glLineWidth (1.0);

glPopMatrix ();

/******************
 * scan text list *
 ******************/

dlist = text->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_TEXT) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_orbital (gamgi_orbital *orbital, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];
double rotate[16];
double *center;
double scale;
int *dots;

if (class == GAMGI_ENGINE_ORBITAL)
  {
  *array++ = GAMGI_CAST_OBJECT orbital;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

center = orbital->center;
scale = orbital->object.scale;
glTranslatef (center[0], center[1], center[2]);
glScalef (scale, scale, scale);
glTranslatef (-center[0], -center[1], -center[2]);

glPushMatrix ();

glTranslatef (orbital->origin[0], orbital->origin[1], orbital->origin[2]);

gamgi_math_quaternion_to_matrix4 (orbital->quaternion, rotate);
glMultMatrixd (rotate);

/***********************
 * scan orbital object *
 ***********************/

dots = orbital->dots;

if (orbital->style == GAMGI_MESA_WIRED)
  {
  glLineWidth (4.0);
  if (dots[1] > 0)
    gamgi_mesa_render_points (dots[1],
    orbital->points + 3 * dots[0]);
  if (dots[2] > 0)
    gamgi_mesa_render_points (dots[2],
    orbital->points + 3 * dots[0] + 3 * dots[1]);
  glLineWidth (1.0);
  }

if (orbital->style == GAMGI_MESA_SOLID)
  {
  glDisable (GL_CULL_FACE);
  if (dots[1] > 0)
    gamgi_mesa_render_triangles (dots[1] / 3,
    orbital->points + 3 * dots[0]);
  if (dots[2] > 0)
    gamgi_mesa_render_triangles (dots[2] / 3,
    orbital->points + 3 * dots[0] + 3 * dots[1]);
  glEnable (GL_CULL_FACE);
  }

if (orbital->frame == TRUE)
  gamgi_mesa_render_lines (orbital->lines, orbital->points);
if (orbital->axes != GAMGI_CHEM_NONE)
  gamgi_mesa_axes_orbital (orbital, FALSE, GAMGI_MESA_AXES_SIZE);

glPopMatrix ();

/**********************
 * scan orbital child *
 **********************/

dlist = orbital->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ORBITAL) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_bond (gamgi_bond *bond, 
gamgi_atom *atom, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double vector[3], center[3];
double angle, height;
float scale;

/**************************************
 * in case second link does not exist *
 **************************************/

if (bond->atom2 == NULL) return array;

if (class == GAMGI_ENGINE_BOND)
  {
  /**********************************************************
   * the object to select is a bond: draw bond in one piece *
   **********************************************************/

  if (bond->first == TRUE)
    {
    /**************
     * first scan *
     **************/

    bond->first = FALSE;
    return array;
    }
  else
    { 
    /***************
     * second scan *
     ***************/

    bond->first = TRUE;
    *array++ = GAMGI_CAST_OBJECT bond;

    gamgi_mesa_select_old (bond->color1);
    gamgi_mesa_select_new (bond->color2);
    glColor3ubv (bond->color2);
     
    glPushMatrix ();
    vector[0] = bond->atom2->position[0] - bond->atom1->position[0];
    vector[1] = bond->atom2->position[1] - bond->atom1->position[1];
    vector[2] = bond->atom2->position[2] - bond->atom1->position[2];

    if (atom == bond->atom1)
      glTranslatef (atom->position[0],
      atom->position[1], atom->position[2]);
    else
      glTranslatef (atom->position[0] - vector[0],
      atom->position[1] - vector[1], atom->position[2] - vector[2]);

    /**************************************************
     * as we are just drawing bonds for selection, we *
     * don't draw two half-bonds (one on each scan),  *
     * we draw one bond instead (on the second scan)  *
     **************************************************/

    height = gamgi_math_vector_length (vector);
    angle = acos (vector[2] / height) * GAMGI_MATH_RAD_DEG;
    if (height - fabs (vector[2]) > GAMGI_MATH_TOLERANCE_LENGTH)
      glRotatef (angle, -vector[1], vector[0], 0.0);
    else
      glRotatef (angle, 1.0, 0.0, 0.0);

    scale = bond->object.scale * bond->size * gamgi->atom->min;
    glScalef (scale, scale, height);

    (*bond->draw) (bond);
    glPopMatrix ();
    }
  }
else
  {
  /***************************************************************
   * the object to select is not a bond: draw bond in two pieces *
   ***************************************************************/

  glPushMatrix ();
  vector[0] = bond->atom2->position[0] - bond->atom1->position[0];
  vector[1] = bond->atom2->position[1] - bond->atom1->position[1];
  vector[2] = bond->atom2->position[2] - bond->atom1->position[2];

  if (atom == bond->atom1)
    glTranslatef (atom->position[0],
    atom->position[1], atom->position[2]);
  else
    glTranslatef (atom->position[0] - vector[0]/2.0,
    atom->position[1] - vector[1]/2.0, atom->position[2] - vector[2]/2.0);

  height = gamgi_math_vector_length (vector);
  angle = acos (vector[2] / height) * GAMGI_MATH_RAD_DEG;
  if (height - fabs (vector[2]) > GAMGI_MATH_TOLERANCE_LENGTH)
    glRotatef (angle, -vector[1], vector[0], 0.0);
  else
    glRotatef (angle, 1.0, 0.0, 0.0);
 
  scale = bond->object.scale * bond->size * gamgi->atom->min;
  glScalef (scale, scale, height/2.0);

  (*bond->draw) (bond);
  glPopMatrix ();

  /*******************************************************************
   * each bond has two parent atoms, if bond->color1 != bond->color2 *
   * then it means that the two atoms belong to different selection  *
   * candidates and bond child objects cannot be attributed to one   *
   * of the two selection candidates and should be ignored.          *
   *******************************************************************/

  if (bond->first == TRUE)
    {
    bond->first = FALSE;
    gamgi_mesa_select_old (bond->color1);
    return array;
    }
  else
    {
    bond->first = TRUE;
    gamgi_mesa_select_old (bond->color2);
    if (bond->color1[0] != bond->color2[0]
    || bond->color1[1] != bond->color2[1]
    || bond->color1[2] != bond->color2[2]) return array;
    }
  }

glPushMatrix ();

/******************************************************
 * bond child objects should scale around bond center *
 ******************************************************/

gamgi_math_vector_scale (vector, center, 0.5);
gamgi_math_vector_add (bond->atom1->position, center, center);

glTranslatef (center[0], center[1], center[2]);
glScalef (bond->object.scale, bond->object.scale, bond->object.scale);
glTranslatef (-center[0], -center[1], -center[2]);

/* scan orbital list */
dlist = bond->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital
    (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = bond->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text
    (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_BOND) glColor3ubv (bond->color1);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_atom (gamgi_atom *atom, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];
float variancy, size;

glPushMatrix ();

if (class == GAMGI_ENGINE_ATOM)
  {
  *array++ = GAMGI_CAST_OBJECT atom;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

/*************************************************
 * draw bond list first, to avoid the atom scale *
 *************************************************/

dlist = atom->bond_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_bond (GAMGI_CAST_BOND dlist->data, atom, class, array);
    dlist = dlist->next; }

glTranslatef (atom->position[0], atom->position[1], atom->position[2]);
glScalef (atom->object.scale, atom->object.scale, atom->object.scale);
glTranslatef (-atom->position[0], -atom->position[1], -atom->position[2]);

glPushMatrix ();

glTranslatef (atom->position[0], atom->position[1], atom->position[2]);

/*************************************
 * determine atom size according to  *
 * the rules defined in gamgi_chem.h *
 *************************************/

variancy = atom->variancy;
if (atom->draw == gamgi_mesa_atom_draw_sphere)
  size = atom->size *
  (variancy * atom->radius + (1 - variancy) * gamgi->atom->min);
else
  size = gamgi->atom->size * gamgi->atom->min;

/*****************
 *   draw atom   *
 *****************/

glScalef (size, size, size);
(*atom->draw) (atom);
glPopMatrix ();

/*********************
 * draw orbital list *
 *********************/

dlist = atom->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/******************
 * draw text list *
 ******************/

dlist = atom->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ATOM) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_direction (gamgi_direction *direction, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double rotate[16];
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_DIRECTION)
  {
  *array++ = GAMGI_CAST_OBJECT direction;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

if (direction->model == GAMGI_PHYS_LINE)
  {
  glTranslatef (direction->center[0], direction->center[1], direction->center[2]);
  glScalef (direction->object.scale, direction->object.scale, direction->object.scale);
  glTranslatef (-direction->center[0], -direction->center[1], -direction->center[2]);
  }

glPushMatrix ();

glTranslatef (direction->origin[0], direction->origin[1], direction->origin[2]);

if (direction->model == GAMGI_PHYS_LINE)
  {
  gamgi_math_quaternion_to_matrix4 (direction->quaternion, rotate);
  glMultMatrixd (rotate);

  glLineWidth (4.0);
  gamgi_mesa_render_lines (direction->loops, direction->points);
  glLineWidth (1.0);
  }

else if (direction->model == GAMGI_PHYS_POLE)
  {
  gamgi_mesa_render_points_chess (direction->loops[0],
  direction->points, GAMGI_MESA_POLE_WIDTH * direction->object.scale);
  }

else if (direction->model == GAMGI_PHYS_TRACE)
  {
  glLineWidth (GAMGI_MESA_TRACE_WIDTH * direction->object.scale);
  gamgi_mesa_render_line_strip (direction->loops[0], direction->points);
  glLineWidth (1.0);
  }

glPopMatrix ();

/* scan atom list */
dlist = direction->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = direction->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = direction->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_DIRECTION) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_plane (gamgi_plane *plane, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double rotate[16];
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_PLANE)
  {
  *array++ = GAMGI_CAST_OBJECT plane;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

if (plane->model == GAMGI_PHYS_POLYGON)
  {
  glTranslatef (plane->center[0], plane->center[1], plane->center[2]);
  glScalef (plane->object.scale, plane->object.scale, plane->object.scale);
  glTranslatef (-plane->center[0], -plane->center[1], -plane->center[2]);
  }

glPushMatrix ();

if (plane->model == GAMGI_PHYS_VECTOR)
  {
  gamgi_mesa_render_points_cross (plane->loops[0],
  plane->origin, GAMGI_MESA_POLE_WIDTH * plane->object.scale);
  }
else
  {
  glTranslatef (plane->origin[0], plane->origin[1], plane->origin[2]);

  if (plane->model == GAMGI_PHYS_POLYGON)
    {
    gamgi_math_quaternion_to_matrix4 (plane->quaternion, rotate);
    glMultMatrixd (rotate);

    glDisable(GL_CULL_FACE);
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1, 1);
    gamgi_mesa_render_polygons (plane->loops, plane->points);
    glDisable (GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    }

  else if (plane->model == GAMGI_PHYS_POLE)
    {
    gamgi_mesa_render_points_cross (plane->loops[0],
    plane->points, GAMGI_MESA_POLE_WIDTH * plane->object.scale);
    }

  else if (plane->model == GAMGI_PHYS_TRACE)
    {
    glLineWidth (GAMGI_MESA_TRACE_WIDTH * plane->object.scale);
    gamgi_mesa_render_line_strip (plane->loops[0], plane->points);
    glLineWidth (1.0);
    }
  }

glPopMatrix ();

/* scan direction list */
dlist = plane->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = plane->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = plane->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = plane->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_PLANE) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_group (gamgi_group *group, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double rotate[16];
unsigned char color_old[3], color_new[3];
double origin[3];

if (class == GAMGI_ENGINE_GROUP)
  {
  *array++ = GAMGI_CAST_OBJECT group;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

if (group->loops == NULL)
  {
  /*****************************************************
   * this contains other objects only, such as atoms:  *
   * scale to layer center, otherwise changing an atom *
   * would change the group center, in turn changing   *
   * the aparent position of all the other atoms       *
   *****************************************************/

  gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);

  glTranslatef (origin[0], origin[1], origin[2]);
  glScalef (group->object.scale, group->object.scale, group->object.scale);
  glTranslatef (-origin[0], -origin[1], -origin[2]);
  }
else
  {
  /**********************************************
   * this group is a polytope: change according *
   * to group center, origin and quaternion     *
   **********************************************/

  glTranslatef (group->center[0], group->center[1], group->center[2]);
  glScalef (group->object.scale, group->object.scale, group->object.scale);
  glTranslatef (-group->center[0], -group->center[1], -group->center[2]);

  glPushMatrix ();

  glTranslatef (group->origin[0], group->origin[1], group->origin[2]);

  gamgi_math_quaternion_to_matrix4 (group->quaternion, rotate);
  glMultMatrixd (rotate);

  glLineWidth (GAMGI_MESA_POLYHEDRON_WIDTH);
  glPointSize (GAMGI_MESA_POLYHEDRON_WIDTH);

  if (group->faces == TRUE)
    {
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1, 1);

    glPolygonMode (GL_FRONT, GL_FILL);
    glDisable (GL_CULL_FACE);
    gamgi_mesa_render_polygons (group->loops, group->points);

    glEnable (GL_CULL_FACE);
    }

  gamgi_mesa_render_loops (group->loops, group->points);

  glDisable (GL_POLYGON_OFFSET_FILL);
  glLineWidth (1.0);
  glPointSize (1.0);

  glPopMatrix ();
  }

/* scan group list */
dlist = group->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

/* scan plane list */
dlist = group->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
    dlist = dlist->next; }

/* scan direction list */
dlist = group->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = group->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = group->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = group->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GROUP) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_molecule (gamgi_molecule *molecule, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];
double origin[3];

if (class == GAMGI_ENGINE_MOLECULE)
  {
  *array++ = GAMGI_CAST_OBJECT molecule;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

/***************************************** 
 * use zero as molecule geometric center *
 *****************************************/

gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);
glTranslatef (origin[0], origin[1], origin[2]);
glScalef (molecule->object.scale, molecule->object.scale, molecule->object.scale);
glTranslatef (-origin[0], -origin[1], -origin[2]);

dlist = molecule->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

dlist = molecule->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
    dlist = dlist->next; }

dlist = molecule->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = molecule->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

dlist = molecule->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

dlist = molecule->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_MOLECULE) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_cluster (gamgi_cluster *cluster, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double rotate[16];
unsigned char color_old[3], color_new[3];
double origin[3];

if (class == GAMGI_ENGINE_CLUSTER)
  {
  *array++ = GAMGI_CAST_OBJECT cluster;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

if (cluster->loops == NULL)
  {
  /*****************************************************
   * this contains other objects only, such as atoms:  *
   * scale to layer center, otherwise changing an atom *
   * would change the cluster center, in turn changing *
   * the aparent position of all the other atoms       *
   *****************************************************/

  gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);

  glTranslatef (origin[0], origin[1], origin[2]);
  glScalef (cluster->object.scale, cluster->object.scale, cluster->object.scale);
  glTranslatef (-origin[0], -origin[1], -origin[2]);
  }
else
  {
  /************************************************
   * this cluster is a polytope: change according *
   * to cluster center, origin and quaternion     *
   ************************************************/

  glTranslatef (cluster->center[0], cluster->center[1], cluster->center[2]);
  glScalef (cluster->object.scale, cluster->object.scale, cluster->object.scale);
  glTranslatef (-cluster->center[0], -cluster->center[1], -cluster->center[2]);

  glPushMatrix ();

  glTranslatef (cluster->origin[0], cluster->origin[1], cluster->origin[2]);

  gamgi_math_quaternion_to_matrix4 (cluster->quaternion, rotate);
  glMultMatrixd (rotate);

  if (cluster->loops != NULL)
    {
    glLineWidth (GAMGI_MESA_POLYHEDRON_WIDTH);
    glPointSize (GAMGI_MESA_POLYHEDRON_WIDTH);

    if (cluster->faces == TRUE)
      {
      glEnable (GL_POLYGON_OFFSET_FILL);
      glPolygonOffset (1, 1);

      glPolygonMode (GL_FRONT, GL_FILL);
      glDisable(GL_CULL_FACE);
      gamgi_mesa_render_polygons (cluster->loops, cluster->points);

      glEnable(GL_CULL_FACE);
      }

    gamgi_mesa_render_loops (cluster->loops, cluster->points);
    if (cluster->atoms > 0)
      gamgi_mesa_render_points (cluster->atoms, cluster->points);

    glDisable (GL_POLYGON_OFFSET_FILL);
    glLineWidth (1.0);
    glPointSize (1.0);
    }

  glPopMatrix ();
  }

/* scan cluster list */
dlist = cluster->cluster_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cluster (GAMGI_CAST_CLUSTER dlist->data, class, array);
    dlist = dlist->next; }

/* scan molecule list */
dlist = cluster->molecule_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_molecule (GAMGI_CAST_MOLECULE dlist->data, class, array);
    dlist = dlist->next; }

/* scan group list */
dlist = cluster->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

/* scan plane list */
dlist = cluster->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
  dlist = dlist->next; }

/* scan direction list */
dlist = cluster->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = cluster->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = cluster->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = cluster->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CLUSTER) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_cell (gamgi_cell *cell, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
double rotate[16];
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_CELL)
  {
  *array++ = GAMGI_CAST_OBJECT cell;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

glTranslatef (cell->center[0], cell->center[1], cell->center[2]);
glScalef (cell->object.scale, cell->object.scale, cell->object.scale);
glTranslatef (-cell->center[0], -cell->center[1], -cell->center[2]);

glPushMatrix ();

glTranslatef (cell->origin[0], cell->origin[1], cell->origin[2]);

if (cell->model != GAMGI_PHYS_PROJECTION)
  {
  gamgi_math_quaternion_to_matrix4 (cell->quaternion, rotate);
  glMultMatrixd (rotate);
  }

glLineWidth (gamgi->cell->borders_width);
glPointSize (gamgi->cell->nodes_width);

if (cell->faces == TRUE)
  {
  glEnable (GL_POLYGON_OFFSET_FILL);
  glPolygonOffset (2, 2);

  glPolygonMode (GL_FRONT, GL_FILL);
  glDisable(GL_CULL_FACE);

  gamgi_mesa_render_polygons (cell->loops, cell->points);

  glEnable(GL_CULL_FACE);
  }

if (cell->lines != NULL)
  gamgi_mesa_render_lines (cell->lines, cell->points);

if (cell->loops != NULL && cell->borders != GAMGI_PHYS_NONE &&
(cell->lattice != GAMGI_PHYS_HEXAGONAL_P ||
(cell->model != GAMGI_PHYS_WIGNER && cell->model != GAMGI_PHYS_CONVENTIONAL)) &&
(cell->lattice != GAMGI_PHYS_ORTHORHOMBIC_C || cell->model != GAMGI_PHYS_WIGNER))
  gamgi_mesa_render_loops (cell->loops, cell->points);

if (cell->nodes == TRUE)
  gamgi_mesa_render_points (cell->n_nodes, cell->points);

glDisable (GL_POLYGON_OFFSET_FILL);
glLineWidth (1.0);
glPointSize (1.0);

/*************
 * draw axes *
 *************/

if (cell->axes == TRUE)
  gamgi_mesa_axes_cell (cell, FALSE, gamgi->cell->axes_size);

glPopMatrix ();

/* scan cluster list */
dlist = cell->cluster_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cluster (GAMGI_CAST_CLUSTER dlist->data, class, array);
    dlist = dlist->next; }

/* scan molecule list */
dlist = cell->molecule_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_molecule (GAMGI_CAST_MOLECULE dlist->data, class, array);
    dlist = dlist->next; }

/* scan group list */
dlist = cell->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

/* scan plane list */
dlist = cell->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
    dlist = dlist->next; }

/* scan direction list */
dlist = cell->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = cell->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = cell->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = cell->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_CELL) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_arrow (gamgi_arrow *arrow, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_ARROW)
  {
  *array++ = GAMGI_CAST_OBJECT arrow;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

/* scan text list */
dlist = arrow->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ARROW) glColor3ubv (color_old);

return array;
}

gamgi_object **gamgi_mesa_pick_shape (gamgi_shape *shape, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_SHAPE)
  {
  *array++ = GAMGI_CAST_OBJECT shape;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

/* scan text list */
dlist = shape->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_SHAPE) glColor3ubv (color_old);

return array;
}

gamgi_object **gamgi_mesa_pick_graph (gamgi_graph *graph, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];

if (class == GAMGI_ENGINE_GRAPH)
  {
  *array++ = GAMGI_CAST_OBJECT graph;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

/* scan text list */
dlist = graph->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_GRAPH) glColor3ubv (color_old);

return array;
}

gamgi_object **gamgi_mesa_pick_assembly (gamgi_assembly *assembly, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
unsigned char color_old[3], color_new[3];
double origin[3];

if (class == GAMGI_ENGINE_ASSEMBLY)
  {
  *array++ = GAMGI_CAST_OBJECT assembly;
  gamgi_mesa_select_old (color_old);
  gamgi_mesa_select_new (color_new);
  glColor3ubv (color_new);
  }

glPushMatrix ();

/*********************************
 * get assembly geometric center *
 *********************************/

gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);
glTranslatef (origin[0], origin[1], origin[2]);
glScalef (assembly->object.scale, assembly->object.scale, assembly->object.scale);
glTranslatef (-origin[0], -origin[1], -origin[2]);

/* scan assembly list */
dlist = assembly->assembly_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_assembly (GAMGI_CAST_ASSEMBLY dlist->data, class, array);
    dlist = dlist->next; }

/* scan graph list */
dlist = assembly->graph_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_graph (GAMGI_CAST_GRAPH dlist->data, class, array);
    dlist = dlist->next; }

/* scan shape list */
dlist = assembly->shape_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_shape (GAMGI_CAST_SHAPE dlist->data, class, array);
    dlist = dlist->next; }

/* scan arrow list */
dlist = assembly->arrow_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_arrow (GAMGI_CAST_ARROW dlist->data, class, array);
    dlist = dlist->next; }

/* scan cell list */
dlist = assembly->cell_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cell (GAMGI_CAST_CELL dlist->data, class, array);
    dlist = dlist->next; }

/* scan cluster list */
dlist = assembly->cluster_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cluster (GAMGI_CAST_CLUSTER dlist->data, class, array);
    dlist = dlist->next; }

/* scan molecule list */
dlist = assembly->molecule_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_molecule (GAMGI_CAST_MOLECULE dlist->data, class, array);
    dlist = dlist->next; }

/* scan group list */
dlist = assembly->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

/* scan plane list */
dlist = assembly->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
    dlist = dlist->next; }

/* scan direction list */
dlist = assembly->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

/* scan atom list */
dlist = assembly->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

/* scan orbital list */
dlist = assembly->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

/* scan text list */
dlist = assembly->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

if (class == GAMGI_ENGINE_ASSEMBLY) glColor3ubv (color_old);

glPopMatrix ();

return array;
}

gamgi_object **gamgi_mesa_pick_layer (gamgi_layer *layer,
gamgi_window *window, int class, gamgi_object **array) 
{
gamgi_dlist *dlist;
GtkAllocation allocation;
float width, height, aspect;
float top = layer->top;

gtk_widget_get_allocation (window->area, &allocation);
width = allocation.width;
height = allocation.height;
aspect = width / height;

/************************************************
 * reset depth buffer and projection parameters *
 ************************************************/

glClear (GL_DEPTH_BUFFER_BIT);
glMatrixMode (GL_PROJECTION);
glLoadIdentity ();

if (layer->perspective == TRUE)
  {
  /***********
   * speedup *
   ***********/

  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glFrustum (-top * aspect, top * aspect, -top, top, layer->near, layer->far);
  }
else
  glOrtho (-top * aspect * GAMGI_MESA_LAYER_CORRECTION, top * aspect * 
  GAMGI_MESA_LAYER_CORRECTION, -top * GAMGI_MESA_LAYER_CORRECTION, top * 
  GAMGI_MESA_LAYER_CORRECTION, layer->near, layer->far);

glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();

gluLookAt (layer->eye[0], layer->eye[1], layer->eye[2],
layer->center[0], layer->center[1], layer->center[2],
layer->up[0], layer->up[1], layer->up[2]);

/**********************
 * scale about center *
 **********************/

glTranslatef (layer->center[0], layer->center[1], layer->center[2]);
glScalef (layer->object.scale, layer->object.scale, layer->object.scale);
glTranslatef (-layer->center[0], -layer->center[1], -layer->center[2]);

/*************
 * draw axes *
 *************/

if (layer->axes == TRUE)
  gamgi_mesa_axes_layer (layer, FALSE, GAMGI_MESA_AXES_SIZE);

dlist = layer->text_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_text (GAMGI_CAST_TEXT dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_orbital (GAMGI_CAST_ORBITAL dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_atom (GAMGI_CAST_ATOM dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_direction (GAMGI_CAST_DIRECTION dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_plane (GAMGI_CAST_PLANE dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_group (GAMGI_CAST_GROUP dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_molecule (GAMGI_CAST_MOLECULE dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cluster (GAMGI_CAST_CLUSTER dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_cell (GAMGI_CAST_CELL dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->arrow_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_arrow (GAMGI_CAST_ARROW dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->graph_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_graph (GAMGI_CAST_GRAPH dlist->data, class, array);
    dlist = dlist->next; }

dlist = layer->assembly_start;
while (dlist != NULL)
  { array = gamgi_mesa_pick_assembly (GAMGI_CAST_ASSEMBLY dlist->data, class, array);
    dlist = dlist->next; }

return array;
}

void gamgi_mesa_pick_window (gamgi_window *window,
int class, gamgi_object **array)
{
gamgi_dlist *dlist;
gamgi_layer *layer;

/*************************************************************
 * The order used to render layers is important, because     *
 * each layer resets the depth buffer, so its objects are    *
 * always represented over the objects of the layers before. *
 *                                                           *
 * 1) scan the translucid layers (ordered as in the list)    *
 * 2) scan the transparent layers (ordered as in the list)   *
 * 3) scan the current layer                                 *
 *************************************************************/

if (window->layer->visibility_out != GAMGI_GTK_NONE)
  {
  /*********************************
   * first, scan translucid layers *
   *********************************/

  dlist = window->layer_start;
  while (dlist != NULL)
    {
    layer = GAMGI_CAST_LAYER dlist->data;
    dlist = dlist->next;

    if (layer->visibility_in == GAMGI_GTK_NONE || layer == window->layer)
      continue;

    if (layer->visibility_in == GAMGI_GTK_PARTIAL ||
    window->layer->visibility_out == GAMGI_GTK_PARTIAL)
      array = gamgi_mesa_pick_layer (layer, window, class, array);
    }

  /***********************************
   * second, scan transparent layers *
   ***********************************/

  dlist = window->layer_start;
  while (dlist != NULL)
    {
    layer = GAMGI_CAST_LAYER dlist->data;
    dlist = dlist->next;

    if (layer->visibility_in == GAMGI_GTK_NONE || layer == window->layer)
      continue;

    if (layer->visibility_in == GAMGI_GTK_ALL &&
    window->layer->visibility_out == GAMGI_GTK_ALL)
      array = gamgi_mesa_pick_layer (layer, window, class, array);
    }
  }

/*****************************
 * third, scan current layer *
 *****************************/

gamgi_mesa_pick_layer (window->layer, window, class, array);
}
