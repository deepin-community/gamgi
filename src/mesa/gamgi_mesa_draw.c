/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_draw.c
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

#include "gamgi_mesa_text.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_render.h"
#include "gamgi_mesa_axes.h"
#include "gamgi_mesa_select.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_quaternion.h"

void gamgi_mesa_draw_text (gamgi_text *text) 
{
gamgi_dlist *dlist;
double rotate[16];

glPushMatrix ();

glTranslatef (text->center[0], text->center[1], text->center[2]);
glScalef (text->object.scale, text->object.scale, text->object.scale);
glTranslatef (-text->center[0], -text->center[1], -text->center[2]);

glPushMatrix ();

glTranslatef (text->origin[0], text->origin[1], text->origin[2]);

gamgi_math_quaternion_to_matrix4 (text->quaternion, rotate);
glMultMatrixd (rotate);

/********************
 * draw text object *
 ********************/

glColor3f (text->red, text->green, text->blue);

if (text->style == GAMGI_MESA_WIRED)
  glLineWidth (GAMGI_MESA_STROKED_WIDTH);
gamgi_mesa_text_draw (text);
if (text->style == GAMGI_MESA_WIRED)
  glLineWidth (1.0);

/******************
 * draw text list *
 ******************/

glPopMatrix ();

dlist = text->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_orbital (gamgi_orbital *orbital) 
{
gamgi_dlist *dlist;
double rotate[16];
double *center;
double scale;
int *dots;

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
 * draw orbital object *
 ***********************/

dots = orbital->dots;

if (orbital->style == GAMGI_MESA_WIRED)
  {
  glLineWidth (4.0);
  if (dots[1] > 0)
    { glColor3f (orbital->base_r, orbital->base_g, orbital->base_b);
      gamgi_mesa_render_points (dots[1],
      orbital->points + 3 * dots[0]); }
  if (dots[2] > 0)
    { glColor3f (orbital->phase_r, orbital->phase_g, orbital->phase_b);
      gamgi_mesa_render_points (dots[2],
      orbital->points + 3 * dots[0] + 3 * dots[1]); }
  glLineWidth (1.0);
  }

if (orbital->style == GAMGI_MESA_SOLID)
  {
  glDisable (GL_CULL_FACE);
  if (dots[1] > 0)
    { glColor3f (orbital->base_r, orbital->base_g, orbital->base_b);
      gamgi_mesa_render_triangles (dots[1] / 3,
      orbital->points + 3 * dots[0]); }
  if (dots[2] > 0)
    { glColor3f (orbital->phase_r, orbital->phase_g, orbital->phase_b);
      gamgi_mesa_render_triangles (dots[2] / 3,
      orbital->points + 3 * dots[0] + 3 * dots[1]); }
  glEnable (GL_CULL_FACE);
  }

if (orbital->frame == TRUE)
  { glColor3f (orbital->frame_r, orbital->frame_g, orbital->frame_b);
    gamgi_mesa_render_lines (orbital->lines, orbital->points); }
if (orbital->axes != GAMGI_CHEM_NONE)
  gamgi_mesa_axes_orbital (orbital, TRUE, GAMGI_MESA_AXES_SIZE);

/******************
 * draw text list *
 ******************/

glPopMatrix ();

dlist = orbital->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_bond (gamgi_bond *bond, gamgi_atom *atom) 
{
gamgi_dlist *dlist;
double vector[3], center[3];
double angle, height, scale;

/**************************************
 * in case second link does not exist *
 **************************************/

if (bond->atom2 == NULL) return;

gamgi_math_vector_sub (bond->atom2->position, bond->atom1->position, vector);

/********************************************
 * complete bonds must have colors between  *
 * 0.0 and 1.0; half-bonds should have -1.0 *
 ********************************************/

glPushMatrix ();

if (bond->color == FALSE)
  {
  /********************************************
   * draw two half-bonds with inherited color *
   ********************************************/

  glColor3f (atom->red, atom->green, atom->blue);

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
  }
else if (bond->first == FALSE)
  {
  /********************************************************
   * second parent atom: draw bond with independent color *
   ********************************************************/

  glColor3f (bond->red, bond->green, bond->blue);

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
  glColor3f (atom->red, atom->green, atom->blue);
  }

glPopMatrix ();
glPushMatrix ();

/******************************************************
 * bond child objects should scale around bond center *
 ******************************************************/

gamgi_math_vector_scale (vector, center, 0.5);
gamgi_math_vector_add (bond->atom1->position, center, center);

glTranslatef (center[0], center[1], center[2]);
glScalef (bond->object.scale, bond->object.scale, bond->object.scale);
glTranslatef (-center[0], -center[1], -center[2]);

/***********************************
 * first parent atom: ignore child *
 * second parent atom: draw child  *
 ***********************************/

if (bond->first == TRUE) bond->first = FALSE;
else
  {
  bond->first = TRUE;

  /*********************
   * draw orbital list *
   *********************/

  dlist = bond->orbital_start;
  while (dlist != NULL)
    { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
      dlist = dlist->next; }

  /******************
   * draw text list *
   ******************/

  dlist = bond->text_start;
  while (dlist != NULL)
    { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
      dlist = dlist->next; }
  }

glPopMatrix ();
}

void gamgi_mesa_draw_atom (gamgi_atom *atom) 
{
gamgi_dlist *dlist;
double variancy, size;

glPushMatrix ();

/*************************************************
 * draw bond list first, to avoid the atom scale *
 *************************************************/

dlist = atom->bond_start;
while (dlist != NULL)
  { gamgi_mesa_draw_bond (GAMGI_CAST_BOND dlist->data, atom);
    dlist = dlist->next; }

glTranslatef (atom->position[0], atom->position[1], atom->position[2]);
glScalef (atom->object.scale, atom->object.scale, atom->object.scale);
glTranslatef (-atom->position[0], -atom->position[1], -atom->position[2]);

glPushMatrix ();

glTranslatef (atom->position[0], atom->position[1], atom->position[2]);

glColor3f (atom->red, atom->green, atom->blue);

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

/*************
 * draw atom *
 *************/

glScalef (size, size, size);
(*atom->draw) (atom);
glPopMatrix ();

/*********************
 * draw orbital list *
 *********************/

dlist = atom->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/******************
 * draw text list *
 ******************/

dlist = atom->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_direction (gamgi_direction *direction) 
{
double rotate[16];
gamgi_dlist *dlist;

glPushMatrix ();

glColor3f (direction->red, direction->green, direction->blue);

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

/* draw atom list */
dlist = direction->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = direction->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = direction->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_plane (gamgi_plane *plane) 
{
gamgi_dlist *dlist;
double rotate[16];
float background[4];

glPushMatrix ();

glColor3f (plane->red, plane->green, plane->blue);

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

    glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glDisable(GL_CULL_FACE);
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1, 1);
    gamgi_mesa_render_polygons (plane->loops, plane->points);
    glDisable (GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glGetFloatv (GL_COLOR_CLEAR_VALUE, background);
    glColor3fv (background);
    glLineWidth (2.0);
    gamgi_mesa_render_loops (plane->loops, plane->points);
    glLineWidth (1.0);
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

/* draw direction list */
dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_group (gamgi_group *group) 
{
gamgi_dlist *dlist;
double rotate[16];
float background[4];
double origin[3];

glPushMatrix ();

if (group->loops == NULL)
  {
  /*****************************************************
   * this contains other objects only, such as atoms:  *
   * scale to layer center, otherwise changing an atom *
   * would change the group center, in turn changing *
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

  glColor3f (group->red, group->green, group->blue);

  if (group->faces == TRUE)
    {
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1, 1);

    glPolygonMode (GL_FRONT, GL_FILL);
    glDisable (GL_CULL_FACE);

    gamgi_mesa_render_polygons (group->loops, group->points);

    glEnable (GL_CULL_FACE);

    glGetFloatv (GL_COLOR_CLEAR_VALUE, background);
    glColor3fv (background);
    }

  gamgi_mesa_render_loops (group->loops, group->points);

  glDisable (GL_POLYGON_OFFSET_FILL);
  glLineWidth (1.0);
  glPointSize (1.0);

  glPopMatrix ();
  }

/* draw group list */
dlist = group->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

/* draw plane list */
dlist = group->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

/* draw direction list */
dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = group->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_molecule (gamgi_molecule *molecule) 
{
gamgi_dlist *dlist;
double origin[3];

glPushMatrix ();

/*********************************
 * get molecule geometric center *
 *********************************/

gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);

glTranslatef (origin[0], origin[1], origin[2]);
glScalef (molecule->object.scale, molecule->object.scale, molecule->object.scale);
glTranslatef (-origin[0], -origin[1], -origin[2]);

/* draw group list */
dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

/* draw plane list */
dlist = molecule->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

/* draw direction list */
dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = molecule->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = molecule->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_cluster (gamgi_cluster *cluster) 
{
gamgi_dlist *dlist;
double rotate[16];
float background[4];
double origin[3];

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
  
  glLineWidth (GAMGI_MESA_POLYHEDRON_WIDTH);
  glPointSize (GAMGI_MESA_POLYHEDRON_WIDTH);

  if (cluster->faces == TRUE)
    {
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonOffset (1, 1);

    glPolygonMode (GL_FRONT, GL_FILL);
    glDisable(GL_CULL_FACE);

    gamgi_mesa_render_polygons_color (cluster->loops, 
    cluster->points, cluster->paints, cluster->colors);

    glEnable(GL_CULL_FACE);

    glGetFloatv (GL_COLOR_CLEAR_VALUE, background);
    glColor3fv (background);
    }
  else glColor3fv (cluster->colors);

  gamgi_mesa_render_loops (cluster->loops, cluster->points);
  if (cluster->atoms > 0)
    gamgi_mesa_render_points (cluster->atoms, cluster->points);

  glDisable (GL_POLYGON_OFFSET_FILL);
  glLineWidth (1.0);
  glPointSize (1.0);

  glPopMatrix ();
  }

/* draw cluster list */
dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = dlist->next; }

/* draw molecule list */
dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_draw_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = dlist->next; }

/* draw group list */
dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

/* draw plane list */
dlist = cluster->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

/* draw direction list */
dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = cluster->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = cluster->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_cell (gamgi_cell *cell) 
{
gamgi_dlist *dlist;
double rotate[16];
float background[4];

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

glColor3f (cell->red,  cell->green, cell->blue);

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

  glGetFloatv (GL_COLOR_CLEAR_VALUE, background);
  glColor3fv (background);
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
  gamgi_mesa_axes_cell (cell, TRUE, gamgi->cell->axes_size);

glPopMatrix ();

/* draw cluster list */
dlist = cell->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = dlist->next; }

/* draw molecule list */
dlist = cell->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_draw_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = dlist->next; }

/* draw group list */
dlist = cell->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

/* draw plane list */
dlist = cell->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

/* draw direction list */
dlist = cell->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = cell->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = cell->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = cell->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_arrow (gamgi_arrow *arrow) 
{
gamgi_dlist *dlist;

/* draw text list */
dlist = arrow->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }
}

void gamgi_mesa_draw_shape (gamgi_shape *shape) 
{
gamgi_dlist *dlist;

/* draw text list */
dlist = shape->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }
}

void gamgi_mesa_draw_graph (gamgi_graph *graph) 
{
gamgi_dlist *dlist;

/* draw text list */
dlist = graph->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }
}

void gamgi_mesa_draw_assembly (gamgi_assembly *assembly) 
{
gamgi_dlist *dlist;
double origin[3];

glPushMatrix ();

/*********************************
 * get assembly geometric center *
 *********************************/

gamgi_math_vector_absolute (origin, 0.0, 0.0, 0.0);
glTranslatef (origin[0], origin[1], origin[2]);
glScalef (assembly->object.scale, assembly->object.scale, assembly->object.scale);
glTranslatef (-origin[0], -origin[1], -origin[2]);

/* draw assembly list */
dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_draw_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    dlist = dlist->next; }

/* draw graph list */
dlist = assembly->graph_start;
while (dlist != NULL)
  { gamgi_mesa_draw_graph (GAMGI_CAST_GRAPH dlist->data);
    dlist = dlist->next; }

/* draw arrow list */
dlist = assembly->arrow_start;
while (dlist != NULL)
  { gamgi_mesa_draw_arrow (GAMGI_CAST_ARROW dlist->data);
    dlist = dlist->next; }

/* draw shape list */
dlist = assembly->shape_start;
while (dlist != NULL)
  { gamgi_mesa_draw_shape (GAMGI_CAST_SHAPE dlist->data);
    dlist = dlist->next; }

/* draw cell list */
dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cell (GAMGI_CAST_CELL dlist->data);
    dlist = dlist->next; }

/* draw cluster list */
dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = dlist->next; }

/* draw molecule list */
dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_draw_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = dlist->next; }

/* draw group list */
dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

/* draw plane list */
dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

/* draw direction list */
dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

/* draw atom list */
dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

/* draw orbital list */
dlist = assembly->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

/* draw text list */
dlist = assembly->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }

glPopMatrix ();
}

void gamgi_mesa_draw_light (gamgi_light *light, int offset) 
{
/****************
 * set geometry *
 ****************/

glLightfv ((GLenum) GL_LIGHT0 + offset, GL_POSITION, light->position);
glLightfv ((GLenum) GL_LIGHT0 + offset, GL_SPOT_DIRECTION, light->direction);
glLightf ((GLenum) GL_LIGHT0 + offset, GL_SPOT_CUTOFF, light->angle);

/*************
 * set color *
 *************/

glLightfv ((GLenum) GL_LIGHT0 + offset, GL_AMBIENT, light->ambient);
glLightfv ((GLenum) GL_LIGHT0 + offset, GL_DIFFUSE, light->diffuse);
glLightfv ((GLenum) GL_LIGHT0 + offset, GL_SPECULAR, light->specular);

/*******************
 * set attenuation *
 *******************/

glLightf ((GLenum) GL_LIGHT0 + offset, GL_CONSTANT_ATTENUATION, light->constant);
glLightf ((GLenum) GL_LIGHT0 + offset, GL_LINEAR_ATTENUATION, light->linear);
glLightf ((GLenum) GL_LIGHT0 + offset, GL_QUADRATIC_ATTENUATION, light->quadratic);
glLightf ((GLenum) GL_LIGHT0 + offset, GL_SPOT_EXPONENT, light->radial);

/******************************************
 * enable light and increase light offset *
 ******************************************/

glEnable ((GLenum) GL_LIGHT0 + offset);
}

void gamgi_mesa_draw_layer (gamgi_layer *layer, gamgi_window *window) 
{
gamgi_dlist *dlist;
GtkAllocation allocation;
float width, height, aspect;
float top = layer->top;
float off[4] = {0.0, 0.0, 0.0, 1.0};
float on[4] = {1.0, 1.0, 1.0, 1.0};
int light;

gtk_widget_get_allocation (window->area, &allocation);
width = allocation.width;
height = allocation.height;
aspect = width / height;

/************************
 * restart depth buffer *
 ************************/

glClear (GL_DEPTH_BUFFER_BIT);

/*****************************
 * projection transformation *
 *****************************/

glMatrixMode (GL_PROJECTION);
glLoadIdentity ();

if (layer->perspective == TRUE)
  {
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glFrustum (-top * aspect, top * aspect, -top, top, layer->near, layer->far);
  }
else
  glOrtho (-top * aspect * GAMGI_MESA_LAYER_CORRECTION, top * aspect * 
  GAMGI_MESA_LAYER_CORRECTION, -top * GAMGI_MESA_LAYER_CORRECTION, top * 
  GAMGI_MESA_LAYER_CORRECTION, layer->near, layer->far);

/****************************
 * modelview transformation *
 ****************************/

glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();

gluLookAt (layer->eye[0], layer->eye[1], layer->eye[2],
layer->center[0], layer->center[1], layer->center[2],
layer->up[0], layer->up[1], layer->up[2]);

/****************************
 * disable all lights even  *
 * if they were not enabled *
 ****************************/

for (light = 0; light < gamgi->max_lights; light++) glDisable (GL_LIGHT0 + light);

light = 0;
if ((dlist = layer->light_start) != NULL)
  {
  /*********************************
   * lights exist: enable lighting *
   *********************************/

  glEnable (GL_LIGHTING);
  glLightModelfv (GL_LIGHT_MODEL_AMBIENT, off);
  glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
  glEnable (GL_NORMALIZE);
 
  /*****************************************************************
   *          OpenGL shininess accepted values: [0, 128]           *
   *          GAMGI shininess accepted values: [0.0, 1.0]          *
   *                                                               *
   * gamgi->light->shininess is exactly 0.0: shininess is disabled *
   * gamgi->light->shininess is close to 0.0: shininess is minimum *
   * gamgi->light->shininess is 1.0: shininess is maximum          *
   *****************************************************************/

  glEnable (GL_COLOR_MATERIAL);
  if (gamgi->light->shininess > 0.0)
    {
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, on);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, on);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, on);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 
    128  * (1.0 - gamgi->light->shininess));
    }
  else
    {
    glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, off);
    glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, off);
    glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, off);
    }

  /***************
   * draw lights *
   ***************/

  while (dlist != NULL)
    { 
    gamgi_mesa_draw_light (GAMGI_CAST_LIGHT dlist->data, light++);
    dlist = dlist->next;
    }
  }
else
  {
  /*****************************************
   * lights do not exist: disable lighting *
   *****************************************/

  glDisable (GL_LIGHTING);
  glDisable (GL_NORMALIZE);
  glDisable (GL_COLOR_MATERIAL);
  }

/*********************************
 * keep number of enabled lights *
 *********************************/

layer->light = light;

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
  gamgi_mesa_axes_layer (layer, TRUE, GAMGI_MESA_AXES_SIZE);

dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_mesa_draw_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    dlist = dlist->next; }

dlist = layer->graph_start;
while (dlist != NULL)
  { gamgi_mesa_draw_graph (GAMGI_CAST_GRAPH dlist->data);
    dlist = dlist->next; }

dlist = layer->shape_start;
while (dlist != NULL)
  { gamgi_mesa_draw_shape (GAMGI_CAST_SHAPE dlist->data);
    dlist = dlist->next; }

dlist = layer->arrow_start;
while (dlist != NULL)
  { gamgi_mesa_draw_arrow (GAMGI_CAST_ARROW dlist->data);
    dlist = dlist->next; }

dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cell (GAMGI_CAST_CELL dlist->data);
    dlist = dlist->next; }

dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_mesa_draw_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = dlist->next; }

dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_mesa_draw_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = dlist->next; }

dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_mesa_draw_group (GAMGI_CAST_GROUP dlist->data);
    dlist = dlist->next; }

dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_mesa_draw_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = dlist->next; }

dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_mesa_draw_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = dlist->next; }

dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_mesa_draw_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = dlist->next; }

dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_mesa_draw_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = dlist->next; }

dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_mesa_draw_text (GAMGI_CAST_TEXT dlist->data);
    dlist = dlist->next; }
}

void gamgi_mesa_draw_window (gamgi_window *window) 
{
gamgi_dlist *dlist;
gamgi_layer *layer;
GtkAllocation allocation;
int x, y;

/************************************************************
 * restart color buffer with current layer background color *
 ************************************************************/

glClearColor (window->layer->color[0], window->layer->color[1], 
window->layer->color[2], 1.0);
glClear (GL_COLOR_BUFFER_BIT);

/*************************************************************
 * The order used to render layers is important, because     *
 * each layer resets the depth buffer, so its objects are    *
 * always represented over the objects of the layers before. *
 *                                                           *
 * 1) draw the translucid layers (ordered as in the list)    *
 * 2) draw the transparent layers (ordered as in the list)   *
 * 3) draw the current layer                                 *
 *************************************************************/

if (window->layer->visibility_out != GAMGI_GTK_NONE)
  {
  /*********************************
   * first, draw translucid layers *
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
      gamgi_mesa_draw_layer (layer, window);
    }

  /****************************************************
   * everything rendered so far is blended with the   *
   * background color of the current layer, using a   *
   * polygon with the viewport dimensions, so objects *
   * in these layers willl look partially visible     *
   *                                                  *
   * lighting must be disabled here, otherwise lights *
   * interfere with blending and the background color *
   ****************************************************/

  glDisable (GL_LIGHTING);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f (window->layer->color[0], window->layer->color[1],
  window->layer->color[2], GAMGI_MESA_LAYER_BLEND);
  
  glClear (GL_DEPTH_BUFFER_BIT);
  gtk_widget_get_allocation (window->area, &allocation);
  x = allocation.width;
  y = allocation.height;

  glBegin (GL_POLYGON);
  glVertex3f (-x, -y, 0.0);
  glVertex3f (x, -y, 0.0);
  glVertex3f (x, y, 0.0);
  glVertex3f (-x, y, 0.0);
  glEnd ();

  glDisable (GL_BLEND);
  
  /***********************************
   * second, draw transparent layers *
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
      gamgi_mesa_draw_layer (layer, window);
    }
  }

/*****************************
 * third, draw current layer *
 *****************************/

gamgi_mesa_draw_layer (window->layer, window);

/************************************
 * fourth, draw selection rectangle *
 ************************************/

gamgi_mesa_select_draw (window);
}

void gamgi_mesa_draw_gamgi (void) 
{
gamgi_window *window;

/********************
 * draw all windows *
 ********************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  gtk_widget_queue_draw (window->area);

}
