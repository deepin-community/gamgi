/******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_polytope.c
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/*****************************************************
 *                   Bibliography                    *
 *                                                   *
 * Rourke J., Computational Geometry in C,           *
 * Cambridge University Press (1998)                 *
 *                                                   *
 * Berg M., Kreveld M., Overmars M., Schwarzkopf O., *
 * Computational Geometry, Springer Verlag (2000)    *
 *****************************************************/

#include <time.h>

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_copy.h"
#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_remove.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_position.h"
#include "gamgi_chem_atom.h"
#include "gamgi_phys_direction.h"
#include "gamgi_phys_plane.h"

/*******************************************************
 * To support the initial tetrahedron, the start size  *
 * for arrays faces, edges and face should be at least *
 * 4, 6, and 10. All the other start sizes can be 0.   *
 *******************************************************/

#define STATIC_FACES_START 20
#define STATIC_FACES_STEP   5

#define STATIC_EDGES_START 20
#define STATIC_EDGES_STEP   5

#define STATIC_VERTICES_START 20
#define STATIC_VERTICES_STEP   5

#define STATIC_FACE_START 20
#define STATIC_FACE_STEP   5

#define STATIC_VERTEX_START 20
#define STATIC_VERTEX_STEP   5

#define STATIC_LOOPS_START 20
#define STATIC_LOOPS_STEP   5

enum { STATIC_USE = 1, STATIC_KEEP, STATIC_MODIFY, STATIC_REMOVE };

typedef struct _static_face {

gamgi_enum state;
int id;

int n_edges;
int *edges;
int n_conflicts;
int *conflicts;

double center[3];
double normal[3];
double area;
double length;

} static_face;

typedef struct _static_edge {

gamgi_enum state;
int f_start;
int f_end;
int v_start;
int v_end;

double length;

} static_edge;

typedef struct _static_vertex {

int id;
int n_edges;
int edge;

int n_conflicts;
int *conflicts;

} static_vertex;

typedef struct _static_polytope {

gamgi_object *parent;
gamgi_dlist *atoms;
int n_atoms;

gamgi_slist **cells;
double width_x, width_y, width_z;
int n_cells;
double max;

gamgi_group *group;
gamgi_atom *atom;
gamgi_bool global, color;
gamgi_enum structure;
double visible, coplanar;
int element, number;

/*********************************************
 *                  arrays format            *
 *                                           *
 * int **vertices: vertex 1 ... vertex n     *
 *                                           *
 * int **edges: edge 1 ... edge n            *
 *                                           *
 * int **faces: face 1 ... face n            *
 *                                           *
 * int *vertex->conflicts: array size,       *
 * face 1, offset 1 ... face n, offset n     *
 *                                           *
 * int *face->conflicts: array size,         *
 * vertex 1, offset 1 ... vertex n, offset n *
 *                                           *
 * int *face->edges: array size,             *
 * edge 1 ... edge n                         *
 *********************************************/

static_vertex **vertices;
int n_vertices;

static_edge **edges;
int n_edges;

static_face **faces;
int n_faces;

/*******************************************
 * unique identifier to retrieve conflicts *
 *******************************************/

int id;

/********************************************
 * n_vertices = total number of positions   *
 * in arrays vertices, neighbours, distance *
 *                                          *
 * n_neighbours = used number of positions  *
 * in arrays vertices, neighbours, distance *
 *                                          *
 * neighbours = array with neighbour atoms  *
 *                                          *
 * distance = array with distances from     *
 * neighbours to central atom               *
 *******************************************/

int n_neighbours;
gamgi_atom **neighbours;
double *distance;

double center[3];
double volume;

} static_polytope;

static void static_atom (gamgi_atom *atom, static_polytope *polytope)
{
gamgi_object *parent;

/************************
 * set new central atom *
 ************************/

polytope->atom = atom;

/*******************************************************
 * get new central atom parent: it must be at least a  *
 * Group, so it must be above Plane, Direction objects *
 *******************************************************/

parent = polytope->atom->object.object;
if (parent->class == GAMGI_ENGINE_DIRECTION)
  parent = parent->object;
if (parent->class == GAMGI_ENGINE_PLANE)
  parent = parent->object;
polytope->parent = parent;

/***********************************************************
 * When color is set by users or by the neighbour element, *
 * color is known in advance. When color is set by the     *
 * central atom, use each central atom color: this is not  *
 * necessarily the central atom default element color!     *
 ***********************************************************/

if (polytope->color == FALSE)
  {
  polytope->group->red = atom->red;
  polytope->group->green = atom->green;
  polytope->group->blue = atom->blue;
  }

}

static int *static_export_points (gamgi_group *group, static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom;
static_vertex **vertices;
static_vertex *vertex;
double *points;
int *index;
int n_neighbours;
int n, i;

vertices = polytope->vertices;
n_neighbours = polytope->n_neighbours;
neighbours = polytope->neighbours;

/******************************************
 * build the index array to redirect the  *
 * used vertices to their final positions *
 ******************************************/

n = 0;
index = (int *) malloc (n_neighbours * sizeof (int));

for (i = 0; i < n_neighbours; i++)
  {
  vertex = vertices[i];
  if (vertex->n_edges > 0) index[i] = n++;
  }

/********************************************
 * build the points array that will contain *
 * the positions of all the vertices of the *
 * polyhedron contained in the group object *
 ********************************************/

group->n_points = n;
points = (double *) malloc (3 * n * sizeof (double));
group->points = points;

for (i = 0; i < n_neighbours; i++)
  {
  vertex = vertices[i];
  if (vertex->n_edges > 0)
    {
    /*****************
     * save vertices *
     *****************/

    n = index[i];
    atom = neighbours[i];
    points[3 * n + 0] = atom->position[0];
    points[3 * n + 1] = atom->position[1];
    points[3 * n + 2] = atom->position[2];

    /********************
     * calculate center *
     ********************/

    gamgi_math_vector_add (group->center, atom->position, group->center);
    }
  }

/********************
 * calculate center *
 ********************/

if (group->n_points > 0)
  gamgi_math_vector_scale (group->center, 
  group->center, 1.0 / group->n_points);

return index;
}

static void static_export_loops (int *index, 
gamgi_group *group, static_polytope *polytope)
{
static_face *face;
static_edge *edge;
int *loops, *edges;
int v, e;
int total, offset;
int n_faces, n_loops;
int i, j;

n_faces = polytope->n_faces;

n_loops = STATIC_LOOPS_START;
loops = (int *) malloc (n_loops * sizeof (int));
loops[0] = 0;

total = 1;
offset = 1;
for (i = 0; i < n_faces; i++)
  {
  /********************
   * find valid faces *
   ********************/

  face = polytope->faces[i];
  if (face->state != STATIC_KEEP) continue;

  edges = face->edges;
  loops[0]++;

  /**********************************
   * if needed increase loops array *
   **********************************/

  if (++total > n_loops)
    {
    n_loops += STATIC_LOOPS_STEP;
    loops = (int *) realloc (loops, n_loops * sizeof (int));
    }

  loops[offset] = 0;
  for (j = 1; j <= face->n_edges; j++)
    {
    e = edges[j];
    edge = polytope->edges[e];

    /**********************************************
     * edge vertices are properly orientated for  *
     * face f_start, for face f_end reverse order *
     **********************************************/

    if (edge->f_start == i) v = edge->v_start;
    else v = edge->v_end;

    /****************************************************
     * redirect valid vertices to final array positions *
     ****************************************************/

    v = index[v];

    /**********************************
     * if needed increase loops array *
     **********************************/

    if (++total > n_loops)
      {
      n_loops += STATIC_LOOPS_STEP;
      loops = (int *) realloc (loops, n_loops * sizeof (int));
      }

    loops[offset]++;
    loops[offset + loops[offset]] = v;
    }

  offset += loops[offset] + 1;
  }

/**********************************
 * set final size for loops array *
 **********************************/

group->n_loops = total;
group->loops = (int *) realloc (loops, total * sizeof (int));
}

static void static_export (static_polytope *polytope)
{
gamgi_group *group;
gamgi_atom *atom;
double unit[9], rotation[9];
double zero[3], translation[3];
int *index;
int i;

/*********************************************
 * create container group and link to parent *
 *********************************************/

if (polytope->global == TRUE)
  group = gamgi_engine_copy_group (polytope->group);
else
  group = polytope->group;

if (group->object.object == NULL)
  gamgi_engine_link_object_object (GAMGI_CAST_OBJECT group, polytope->parent);

/**********************************
 * start position and orientation *
 **********************************/

gamgi_math_vector_zero (zero);
gamgi_math_position_group_rotation_set (group, zero);
gamgi_math_position_group_translation_set (group, zero);

gamgi_math_position_rotation (group->object.object, rotation);
gamgi_math_position_translation (group->object.object, translation);

gamgi_math_vector_zero (zero);
gamgi_math_matrix_unit (unit);
gamgi_math_position_object_rotation (group->object.object, unit);
gamgi_math_position_object_translation (group->object.object, zero);

/*********************************************
 * copy data and export points, loops arrays *
 *********************************************/

index = static_export_points (group, polytope);
static_export_loops (index, group, polytope);
free (index);

/**************************************************
 *               structure policy:                *
 * COPY: copy neighbour atoms and bonds (default) *
 * LINK: unlink neighbour atoms and link to group *
 * NONE: group has no atoms and bonds             *
 **************************************************/

if (polytope->structure == GAMGI_CHEM_STRUCTURE_COPY)
  {
  /**********************************************
   * copy central, neighbour atoms to group     *
   *                                            *
   * clean bonds in central and neighbour atoms *
   **********************************************/

  atom = gamgi_engine_copy_atom (polytope->atom);
  gamgi_engine_link_atom_group (atom, group);
  for (i = 0; i < polytope->n_neighbours; i++)
    {
    atom = gamgi_engine_copy_atom (polytope->neighbours[i]);
    gamgi_engine_link_atom_group (atom, group);
    }

  gamgi_chem_atom_clean (polytope->atom);
  for (i = 0; i < polytope->n_neighbours; i++)
    gamgi_chem_atom_clean (polytope->neighbours[i]);
  }

if (polytope->structure == GAMGI_CHEM_STRUCTURE_LINK)
  {
  /******************************************
   * link central, neighbour atoms to group *
   ******************************************/

  gamgi_engine_unlink_atom (polytope->atom);
  gamgi_engine_link_atom_group (polytope->atom, group);
  for (i = 0; i < polytope->n_neighbours; i++)
    {
    gamgi_engine_unlink_atom (polytope->neighbours[i]);
    gamgi_engine_link_atom_group (polytope->neighbours[i], group);
    }
  }

/********************************************
 * restore position, orientation for parent *
 * object, including group already linked   *
 ********************************************/

gamgi_math_position_object_rotation (group->object.object, rotation);
gamgi_math_position_object_translation (group->object.object, translation);
}

static int static_dot (int v1, int v2,
double tolerance, static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom1, *atom2;
double u[3];
double dot;

neighbours = polytope->neighbours;

atom1 = neighbours[v1];
atom2 = neighbours[v2];

gamgi_math_vector_sub (atom2->position, atom1->position, u);
dot = gamgi_math_vector_dot (u, u);

if (dot > tolerance) return 1;

return 0;
}

static int static_cross (int v1, int v2, int v3,
double tolerance, static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom1, *atom2, *atom3;
double u[3], v[3], w[3];
double dot;

neighbours = polytope->neighbours;

atom1 = neighbours[v1];
atom2 = neighbours[v2];
atom3 = neighbours[v3];

gamgi_math_vector_sub (atom2->position, atom1->position, u);
gamgi_math_vector_sub (atom3->position, atom1->position, v);
gamgi_math_vector_cross (u, v, w);
dot = gamgi_math_vector_dot (w, w);

if (dot > tolerance) return 1;

return 0;
}

static int static_mix (int v1, int v2, int v3, int v4, 
double tolerance, static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom1, *atom2, *atom3, *atom4;
double u[3], v[3], w[3];
double mix;

neighbours = polytope->neighbours;

atom1 = neighbours[v1];
atom2 = neighbours[v2];
atom3 = neighbours[v3];
atom4 = neighbours[v4];

gamgi_math_vector_sub (atom2->position, atom1->position, u);
gamgi_math_vector_sub (atom3->position, atom1->position, v);
gamgi_math_vector_sub (atom4->position, atom1->position, w);
mix = gamgi_math_vector_mix (u, v, w);

if (mix > tolerance) return 1;

if (mix < -tolerance) return -1;

return 0;
}

static int static_conflict_face_increase (int f, static_polytope *polytope)
{
static_face *face;
int *conflicts;
int start, i;

face = polytope->faces[f];
conflicts = face->conflicts;

/*****************************************
 * look for a old face conflict position *
 *****************************************/

start = 1 + 2 * face->n_conflicts;
if (start < conflicts[0] - 1) return start;

/*******************************************
 * if that is not available increase array *
 *******************************************/

conflicts[0] += STATIC_FACE_STEP;
conflicts = (int *) realloc (conflicts, conflicts[0] * sizeof (int));
face->conflicts = conflicts;

for (i = start; i < conflicts[0]; i++) conflicts[i] = -1;

return start;
}

static int static_conflict_vertex_increase (int v, static_polytope *polytope)
{
static_vertex *vertex;
int *conflicts;
int i, j;

/******************************************
 * look for a old face conflict position  *
 ******************************************/

vertex = polytope->vertices[v];
conflicts = vertex->conflicts;

for (i = 1; i < conflicts[0] - 1; i+= 2)
  if (conflicts[i] < 0) return i;

/**********************************************************
 * if that is not available increase and initialize array *
 **********************************************************/

conflicts[0] += STATIC_VERTEX_STEP;
conflicts = (int *) realloc (conflicts, conflicts[0] * sizeof (int));
vertex->conflicts = conflicts;

for (j = i; j < conflicts[0]; j++) conflicts[j] = -1;

return i;
}

static int static_conflict_find (int v1, int v2, 
int v3, int v, int f, static_polytope *polytope)
{
static_face *face;
static_vertex *vertex;
int offset_f, offset_v;
int mix;

/***************************************************
 * v1, v2, v3 must be orientated counter-clockwise *
 * when seen from the outside of the polytope!     *
 ***************************************************/

mix = static_mix (v1, v2, v3, v, polytope->visible, polytope);
if (mix > 0)
  {
  /*************************************************
   * vertex v is visible: add to list of conflicts *
   *************************************************/

  offset_v = static_conflict_vertex_increase (v, polytope);
  offset_f = static_conflict_face_increase (f, polytope);

  face = polytope->faces[f];
  face->conflicts[offset_f + 0] = v;
  face->conflicts[offset_f + 1] = offset_v;
  face->n_conflicts++;

  vertex = polytope->vertices[v];
  vertex->conflicts[offset_v + 0] = f;
  vertex->conflicts[offset_v + 1] = offset_f;
  vertex->n_conflicts++;

  return 1;
  }

return 0;
}

static int static_conflict_retrieve (int f, int f_old, 
int v_start, int v_end, int v_new, static_polytope *polytope)
{
static_face *face_old;
static_vertex *vertex;
int *conflicts;
int n_conflicts;
int i, v;

n_conflicts = 0;

face_old = polytope->faces[f_old];
conflicts = face_old->conflicts;

for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  v = conflicts[i];
  if (v < 0) continue;

  vertex = polytope->vertices[v];
  if (vertex->id == polytope->id) continue;

  /***************************************************************
   * vertices v_end, v_start, v_new must be orientated           *
   * counter-clockwise when seen from the outside. As v_start,   *
   * v_end are orientated for the first face (face hidden), they *
   * must be reversed for the second face (face new). The third  *
   * vertex (the new vertex) of this triangular face comes next. *
   ***************************************************************/

  n_conflicts += static_conflict_find (v_end, v_start, v_new, v, f, polytope);
  vertex = polytope->vertices[v];
  vertex->id = polytope->id;
  }

return n_conflicts;
}

static int static_conflict_recreate (int f, int e, int v, 
static_polytope *polytope)
{
static_edge *edge;
static_vertex *vertex;
int f_start, f_end;
int v_start, v_end;
int n_conflicts;
int i;

edge = polytope->edges[e];

/************************************************
 * vertex conflicts to new face must come from  *
 * vertex conflicts with faces adjacent to edge *
 ************************************************/

f_start = edge->f_start;
f_end = edge->f_end;

v_start = edge->v_start;
v_end = edge->v_end;

/************************************************************************
 * Vertices that conflict with the new face must see the visible edge   *
 * so they neccessarily saw one of the faces previously separated by    *
 * the visible edge. Therefore the conflicts for the new face must      *
 * be found among the conflicts for the two faces previously bordering  *
 * the edge. Sometimes, the same conflict for the new face can be found *
 * simultaneously from both bordering faces, so the same conflict is    *
 * wrongly listed twice in the list of conflicts of the new face. To    *
 * avoid wasting time, this duplication is handled only when the new    *
 * face is removed by this vertex or any another vertex before.         *
 ************************************************************************/

n_conflicts = 0;
n_conflicts += static_conflict_retrieve (f, f_start, v_start, v_end, v, polytope);
n_conflicts += static_conflict_retrieve (f, f_end, v_start, v_end, v, polytope);

/************************************************************
 * The same face can be reported twice in the conflict      *
 * list, if new vertex v was in the conflict list of both   *
 * faces previously bordering visible edge e! So, each      *
 * new face f has a unique id to retrieve old conflicts     *
 *                                                          *
 * reset the id counter when the maximum value is reached,  *
 * and clean future vertices, to avoid possible conflicts   *
 ************************************************************/

if (polytope->id == INT_MAX)
  {
  polytope->id = 0;
  for (i = v; i < polytope->n_neighbours; i++)
    { vertex = polytope->vertices[i]; vertex->id = -1; }
  }
else polytope->id += 1;

return n_conflicts;
}

static int static_conflict_create (int v1, int v2, int v3, 
int f, static_polytope *polytope)
{
int n_neighbours, n_conflicts;
int i;

n_conflicts = 0;

n_neighbours = polytope->n_neighbours;
for (i = 4; i < n_neighbours; i++)
  {
  /*************************************************
   * vertices v1, v2, v3 must be orientated        *
   * counter-clockwise when seen from the outside. *
   *************************************************/

  n_conflicts += static_conflict_find (v1, v2, v3, i, f, polytope);
  }

return n_conflicts;
}

static void static_edge_start (int e, static_polytope *polytope)
{
static_edge *edge;

/**************************
 * reset edge information *
 **************************/

edge = polytope->edges[e];
edge->state = STATIC_USE;
}

static static_edge *static_edge_create (int e, static_polytope *polytope)
{
static_edge *edge;

/****************************************
 * create edge structure and initialize *
 ****************************************/

edge = (static_edge *) malloc (sizeof (static_edge));
polytope->edges[e] = edge;

static_edge_start (e, polytope);

return edge;
}

static void static_edge_remove (int e, static_polytope *polytope)
{
static_edge *edge;
static_vertex *vertex;
int v;

edge = polytope->edges[e];

/*********************************************************
 * for both vertices decrease counting (number of edges) *
 *********************************************************/

v = edge->v_start;
vertex = polytope->vertices[v];
vertex->n_edges--;

v = edge->v_end;
vertex = polytope->vertices[v];
vertex->n_edges--;

/*****************************************************
 * this edge is no longer in use and can be recycled *
 *****************************************************/

static_edge_start (e, polytope);
}

static void static_edges_mark (int v_new, static_polytope *polytope)
{
static_face *face_start, *face_end, *face_visible;
static_edge *edge_visible;
static_vertex *vertex_new;
int *conflicts, *edges;
int f_start, f_end, f_visible, e_visible;
int i, j;

/*****************************************************************
 * use the vertex conflict data to scan all the visible faces,   *
 * to get all the edges that are susceptible of changes, without *
 * scanning the whole set of edges                               *
 *                                                               *
 * the edges in visible faces are either in the boundary of the  *
 * visible region (only one face is visible), forming new faces  *
 * with the new vertex, or are inside the visible region (both   *
 * faces are visible), and must be removed.                      *
 *****************************************************************/

vertex_new = polytope->vertices[v_new];
conflicts = vertex_new->conflicts;
for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  f_visible = conflicts[i];
  if (f_visible < 0) continue;

  face_visible = polytope->faces[f_visible];
  edges = face_visible->edges;

  /********************
   * triangular faces *
   ********************/

  for (j = 1; j <= 3; j++)
    {
    e_visible = edges[j];
    edge_visible = polytope->edges[e_visible];

    f_start = edge_visible->f_start;
    face_start = polytope->faces[f_start];
    f_end = edge_visible->f_end;
    face_end = polytope->faces[f_end];

    edge_visible->state = STATIC_MODIFY;
    if (face_start->state == STATIC_REMOVE && face_end->state == STATIC_REMOVE)
      edge_visible->state = STATIC_REMOVE;
    }
  }

}

static int static_edges_increase (static_polytope *polytope)
{
static_edge **edges;
static_edge *edge;
int n_edges, i, j;

edges = polytope->edges;
n_edges = polytope->n_edges;

for (i = 0; i < n_edges; i++)
  {
  /*************************
   * reuse old edges array *
   *************************/

  edge = edges[i];
  if (edge->state == STATIC_USE)
    { edge->state = STATIC_KEEP; return i; }
  }

/************************
 * increase edges array *
 ************************/

n_edges += STATIC_EDGES_STEP;
polytope->n_edges = n_edges;

edges = (static_edge **) realloc (edges, n_edges * sizeof (static_edge *));
polytope->edges = edges;

for (j = i; j < n_edges; j++)
  edges[j] = static_edge_create (j, polytope);

edge = edges[i];
edge->state = STATIC_KEEP;

return i;
}

static void static_edges_clean (int v_new, static_polytope *polytope)
{
static_face *face_visible;
static_edge *edge_visible;
static_vertex *vertex_new;
int *conflicts, *edges;
int f_visible, e_visible;
int i, j;

/*******************************************************************
 * use the vertex conflict data to remove all doubly visible edges *
 *******************************************************************/

vertex_new = polytope->vertices[v_new];
conflicts = vertex_new->conflicts;

for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  f_visible = conflicts[i];
  if (f_visible < 0) continue;

  face_visible = polytope->faces[f_visible];
  edges = face_visible->edges;

  /********************
   * triangular faces *
   ********************/

  for (j = 1; j <= 3; j++)
    {
    e_visible = edges[j];
    edge_visible = polytope->edges[e_visible];

    if (edge_visible->state == STATIC_REMOVE)
      static_edge_remove (e_visible, polytope);
    if (edge_visible->state == STATIC_MODIFY)
      edge_visible->state = STATIC_KEEP;
    }
  }

}

static void static_face_start (int f, static_polytope *polytope)
{
static_face *face;
int *conflicts, *edges;
int i;

face = polytope->faces[f];

/**************************
 * reset face information *
 **************************/

face->state = STATIC_USE;
face->n_conflicts = 0;
face->n_edges = 0;
face->id = f;

conflicts = face->conflicts;
for (i = 1; i < conflicts[0]; i++) conflicts[i] = -1;

edges = face->edges;
for (i = 1; i < edges[0]; i++) edges[i] = -1;
}

static static_face *static_face_create (int f, static_polytope *polytope)
{
static_face *face;
int *conflicts, *edges;
int total;

/****************************************
 * create and initialize face structure *
 ****************************************/

face = (static_face *) malloc (sizeof (static_face));
polytope->faces[f] = face;

total = STATIC_FACE_START;
conflicts = (int *) malloc (total * sizeof (int));
conflicts[0] = total;
face->conflicts = conflicts;

total = STATIC_FACE_START;
edges = (int *) malloc (total * sizeof (int));
edges[0] = total;
face->edges = edges;

static_face_start (f, polytope);

return face;
}

static int static_face_increase (int f, static_polytope *polytope)
{
static_face *face;
int *edges;
int i, start;

face = polytope->faces[f];
edges = face->edges;

/***********************************
 * use available edge if it exists *
 ***********************************/

start = face->n_edges + 1;
if (start < edges[0]) return start;

/**********************************************************
 * if that is not available increase and initialize array *
 **********************************************************/

start = edges[0];

edges[0] += STATIC_FACE_STEP;
edges =  (int *) realloc (edges, edges[0] * sizeof (int));
face->edges = edges;

for (i = start; i < edges[0]; i++) edges[i] = -1;

return start;
}

static void static_face_remove (int f, static_polytope *polytope)
{
static_face *face;
static_vertex *vertex;
int *conflicts_v, *conflicts_f;
int n_conflicts;
int v, i, offset;

/******************************************
 * remove all conflicts for this face     *
 * from vertices before removing the face *
 ******************************************/

face = polytope->faces[f];
conflicts_f = face->conflicts;
n_conflicts = face->n_conflicts;

for (i = 0; i < n_conflicts; i++)
  {
  v = conflicts_f[1 + 2 * i + 0];
  offset = conflicts_f[1 + 2 * i + 1];
  vertex = polytope->vertices[v];
  conflicts_v = vertex->conflicts;
  conflicts_v[offset] = -1;
  conflicts_v[offset + 1] = -1;
  vertex->n_conflicts--;
  }

/*****************************************************
 * this face is no longer in use and can be recycled *
 *****************************************************/

static_face_start (f, polytope);
}

static void static_faces_mark (int v_new, static_polytope *polytope)
{
static_face *face_visible;
static_vertex *vertex_new;
int *conflicts;
int f_visible;
int i;

/*******************************************
 * signal all faces that should be removed *
 *******************************************/

vertex_new = polytope->vertices[v_new];
conflicts = vertex_new->conflicts;
for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  f_visible = conflicts[i];
  if (f_visible < 0) continue;

  face_visible = polytope->faces[f_visible];
  face_visible->state = STATIC_REMOVE;
  }

}

static int static_faces_increase (static_polytope *polytope)
{
static_face **faces;
static_face *face;
int n_faces;
int i, j;

faces = polytope->faces;
n_faces = polytope->n_faces;

for (i = 0; i < n_faces; i++)
  {
  /*************************
   * reuse old faces array *
   *************************/

  face = faces[i];
  if (face->state == STATIC_USE)
    { face->state = STATIC_KEEP; return i; }
  }

/************************
 * increase faces array *
 ************************/

n_faces += STATIC_FACES_STEP;
polytope->n_faces = n_faces;

faces = (static_face **) realloc (faces, n_faces * sizeof (static_face *));
polytope->faces = faces;

for (j = i; j < n_faces; j++)
  faces[j] = static_face_create (j, polytope);

face = faces[i];
face->state = STATIC_KEEP;

return i;
}

static void static_face_merge (int f, int v1, int v2, int v3, 
int e, static_polytope *polytope)
{
static_face *face, *face_start, *face_end, *face_new;
static_edge *edge;
int f_start, f_end, f_new;
int id_start, id_end;
int e1, e2, e3, e_right, e_left;
int v4;
int offset;

edge = polytope->edges[e];

/****************************************************
 * id identifies the new enlarged face that owns    *
 * this triangle that initially was a triangle face *
 ****************************************************/

f_start = edge->f_start;
face_start = polytope->faces[f_start];
id_start = face_start->id;

f_end = edge->f_end;
face_end = polytope->faces[f_end];
id_end = face_end->id;

if (id_start != f) { f_new = f_start; edge->f_end = f; }
else if (id_end != f) { f_new = f_end; edge->f_start = f; }
else
  {
  /***************************************************
   * if id_start = id_end = f then we reached again  *
   * the merging face: stop the scan and remove edge *
   ***************************************************/

  static_edge_remove (e, polytope);
  return;
  }

/***************************************************
 * when the new face has already been handled stop *
 * the scan and add edge to face f list of edges   *
 ***************************************************/

face_new = polytope->faces[f_new];
if (face_new->id < f)
  {
  offset = static_face_increase (f, polytope);
  face = polytope->faces[f];
  face->edges[offset] = e;
  face->n_edges++;
  return;
  }

/**********************************************************
 * get e_right, e_left edges, making sure these edges are *
 * orientated to the right and to the left, respectively  *
 **********************************************************/

e1 = face_new->edges[1];
e2 = face_new->edges[2];
e3 = face_new->edges[3];

e_right = e2; e_left = e3; 
if (e == e2)
  { e_right = e3; e_left = e1; }
else if (e == e3)
  { e_right = e1; e_left = e2; }

/**************************************************
 * make sure vertex v4 is different from vertices *
 * v1, v2, v3, defining the central face, as edge *
 * e can be a border of the central face          *
 **************************************************/

edge = polytope->edges[e_right];
if (edge->f_start == f_new)
  v4 = edge->v_end;
else
  v4 = edge->v_start;

/**********************************************
 * when the new face is not coplanar stop the *
 * scan and add edge to face f list of edges  *
 **********************************************/

if (static_mix (v1, v2, v3, v4, polytope->coplanar, polytope) != 0)
  {
  offset = static_face_increase (f, polytope);
  face = polytope->faces[f];
  face->edges[offset] = e;
  face->n_edges++;
  return;
  }

/*************************************
 * this is a coplanar face: remove   *
 * edge and merge with previous face *
 *************************************/

static_edge_remove (e, polytope);
face_new->state = STATIC_REMOVE;
face_new->id = f;

/***********************************************************************
 * recursively scan adjacent faces, first right, then left, so in the  *
 * end face f edges will be automatically orientated counter-clockwise *
 ***********************************************************************/

static_face_merge (f, v1, v2, v3, e_right, polytope);
static_face_merge (f, v1, v2, v3, e_left, polytope);
}

static void static_faces_merge (static_polytope *polytope)
{
static_face *face;
static_edge *edge;
int e1, e2, e3;
int v1, v2, v3;
int i;

for (i = 0; i < polytope->n_faces; i++)
  {
  /*************************************************************
   * skip faces with new ids as these have been already merged *
   *************************************************************/

  face = polytope->faces[i];
  if (face->state != STATIC_KEEP) continue;
   
  /*****************************************************
   * reset number of edges and save current edges      *
   * as face->edges will be rewritten during the merge *
   *****************************************************/

  face->n_edges = 0;
  e1 = face->edges[1];
  e2 = face->edges[2];
  e3 = face->edges[3];

  /*****************************************************
   * get vertices v1, v2, v3: the vertices orientation *
   * is not relevant because the aim is only to check  *
   * if the vertices of old and new face are coplanar  *
   *****************************************************/

  edge = polytope->edges[e1];
  v1 = edge->v_start;
  v2 = edge->v_end;

  edge = polytope->edges[e2];
  if (edge->f_start == i)
    v3 = edge->v_end;
  else
    v3 = edge->v_start;
  
  /***********************************************
   * scan adjacent faces, first right, then left *
   ***********************************************/

  static_face_merge (i, v1, v2, v3, e1, polytope);
  static_face_merge (i, v1, v2, v3, e2, polytope);
  static_face_merge (i, v1, v2, v3, e3, polytope);
  }
}

static void static_faces_clean (int v_new, static_polytope *polytope)
{
static_vertex *vertex_new;
int *conflicts;
int f_visible, i;

/************************************************************
 * use the vertex conflict data to remove all visible faces *
 ************************************************************/

vertex_new = polytope->vertices[v_new];
conflicts = vertex_new->conflicts;

for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  f_visible = conflicts[i];
  if (f_visible < 0) continue;

  static_face_remove (f_visible, polytope);
  }
}

static void static_face_build (int v_new, int e_visible,
static_polytope *polytope)
{
static_face *face_new;
static_edge *edge_visible, *edge_start, *edge_end;
static_vertex *vertex_start, *vertex_end, *vertex_new;
int *edges;
int f_new;
int e_start, e_end;
int v_start, v_end;

edge_visible = polytope->edges[e_visible];
v_start = edge_visible->v_start;
v_end = edge_visible->v_end;

vertex_new = polytope->vertices[v_new];
vertex_start = polytope->vertices[v_start];
vertex_end = polytope->vertices[v_end];

/******************************************************
 * Create new face, with 3 vertices and 3 edges,      *
 * counter-clockwise: for each edge, the vertices are *
 * orientated counter-clockwise for the first face,   *
 * for the second face the vertices must be reversed  *
 ******************************************************/
 
f_new = static_faces_increase (polytope);
static_conflict_recreate (f_new, e_visible, v_new, polytope);
face_new = polytope->faces[f_new];
face_new->n_edges = 3;
edges = face_new->edges;
edges[1] = e_visible;
edge_visible->f_end = f_new;

if (vertex_start->edge < 0)
  {
  /******************************************************
   * create new edge, pointing to edges before and next *
   ******************************************************/

  e_start = static_edges_increase (polytope);
  edge_start = polytope->edges[e_start];
  edges[2] = e_start;
  edge_start->f_start = f_new;
  edge_start->f_end = -1;
  edge_start->v_start = v_start;
  edge_start->v_end = v_new;

  /**************************************************
   * increase vertex counting (its number of edges) *
   **************************************************/

  vertex_start->n_edges++;
  vertex_new->n_edges++;

  /****************************
   * point vertex to new edge *
   ****************************/

  vertex_start->edge = e_start;
  }
else
  {
  /***************************
   * the edge already exists *
   ***************************/

  e_start = vertex_start->edge;
  edge_start = polytope->edges[e_start];
  edges[2] = e_start;
  edge_start->f_end = f_new;

  /******************************
   * reset vertex edge pointing *
   ******************************/

  vertex_start->edge = -1;
  }

if (vertex_end->edge < 0)
  {
  /******************************************************
   * create new edge, pointing to edges before and next *
   ******************************************************/

  e_end = static_edges_increase (polytope);
  edge_end = polytope->edges[e_end];
  edges[3] = e_end;
  edge_end->f_start = f_new;
  edge_end->f_end = -1;
  edge_end->v_start = v_new;
  edge_end->v_end = v_end;

  /**************************************************
   * increase vertex counting (its number of edges) *
   **************************************************/

  vertex_new->n_edges++;
  vertex_end->n_edges++;

  /****************************
   * point vertex to new edge *
   ****************************/

  vertex_end->edge = e_end;
  }
else
  {
  /***************************
   * the edge already exists *
   ***************************/

  e_end = vertex_end->edge;
  edge_end = polytope->edges[e_end];
  edges[3] = e_end;
  edge_end->f_end = f_new;

  /******************************
   * reset vertex edge pointing *
   ******************************/

  vertex_end->edge = -1;
  }

}

static void static_vertex_build (int v_new, static_polytope *polytope)
{
static_face *face_visible, *face_edge;
static_edge *edge_visible;
static_vertex *vertex_new;
int *conflicts, *edges;
int e_visible, f_visible, f_edge;
int i, j, aux;

/*****************************************************
 * mark visible faces first, then mark visible edges *
 *                                                   *
 * initially the state of faces and edges is KEEP,   *
 * then visible faces change to MODIFY, and visible  *
 * edges change to MODIFY (when one face is visible) *
 * or to REMOVE (when both faces are visible)        *
 *****************************************************/

static_faces_mark (v_new, polytope);
static_edges_mark (v_new, polytope);

/*****************************************************************
 * use again the vertex conflict data to scan all the            *
 * visible faces, to get all the edges that are susceptible      *
 * of changes, without scanning the whole set of edges           *
 *                                                               *
 * build a new face for each edge in the boundary of the visible *
 * region (only one face is visible), linking its two vertices   *
 * with the new vertex, and creating the needed edges            *
 *****************************************************************/

vertex_new = polytope->vertices[v_new];
conflicts = vertex_new->conflicts;
for (i = 1; i < conflicts[0] - 1; i += 2)
  {
  f_visible = conflicts[i];
  if (f_visible < 0) continue;

  face_visible = polytope->faces[f_visible];
  edges = face_visible->edges;
  for (j = 1; j <= 3; j++)
    {
    e_visible = edges[j];

    /************************************************
     * either both faces are visible, and the edge  *
     * will be removed in the end, or one face is   *
     * visible, and the second face must be created *
     ************************************************/

    edge_visible = polytope->edges[e_visible];
    if (edge_visible->state == STATIC_REMOVE) continue;

    /*******************************************
     * this edge belongs to the border curve   *
     * (the horizon) of the visible region     *
     *                                         *
     * edge format: vertices v_start v_end are *
     * orientated counter-clockwise for face   *
     * f_start (and clockwise for f_end)       *
     *******************************************/

    f_edge = edge_visible->f_start;
    face_edge = polytope->faces[f_edge];
    if (face_edge->state == STATIC_REMOVE)
      {
      /************************************************************ 
       * The face that survives, face_hidden, should be first     *
       * and vertices should be ordered according to face_hidden. *
       * When face_hidden is not first, swap faces and vertices.  *
       ************************************************************/

      aux = edge_visible->f_start;
      edge_visible->f_start = edge_visible->f_end;
      edge_visible->f_end = aux;

      aux = edge_visible->v_start;
      edge_visible->v_start = edge_visible->v_end;
      edge_visible->v_end = aux;
      }

    /*************************************************************
     * create new face, joining e_visible edge with v_new vertex *
     *************************************************************/

    static_face_build (v_new, e_visible, polytope);
    }
  }

static_edges_clean (v_new, polytope);
static_faces_clean (v_new, polytope);
}

static void static_vertex_start (int v, static_polytope *polytope)
{
static_vertex *vertex;
int *conflicts;
int i;

vertex = polytope->vertices[v];

vertex->id = -1;
vertex->edge = -1;

vertex->n_edges = 0;
vertex->n_conflicts = 0;

conflicts = vertex->conflicts;
for (i = 1; i < conflicts[0]; i++) conflicts[i] = -1;
}

static static_vertex *static_vertex_create (int v, static_polytope *polytope)
{
static_vertex *vertex;
int *conflicts;
int total;

/******************************************
 * create and initialize vertex structure *
 ******************************************/

vertex = (static_vertex *) malloc (sizeof (static_vertex));
polytope->vertices[v] = vertex;

total = STATIC_VERTEX_START;
conflicts = (int *) malloc (total * sizeof (int));
conflicts[0] = total;
vertex->conflicts = conflicts;

static_vertex_start (v, polytope);

return vertex;
}

static void static_vertices_increase (static_polytope *polytope)
{
gamgi_atom **neighbours;
static_vertex **vertices;
double *distance;
int i, start, n_vertices;

start = polytope->n_vertices;
n_vertices = start + STATIC_VERTICES_STEP;
polytope->n_vertices = n_vertices;

distance = polytope->distance;
distance = (double *)
realloc (distance, n_vertices * sizeof (double));
polytope->distance = distance;

neighbours = polytope->neighbours;
neighbours = (gamgi_atom **)
realloc (neighbours, n_vertices * sizeof (gamgi_atom *));
polytope->neighbours = neighbours;

vertices = polytope->vertices;
vertices = (static_vertex **)
realloc (vertices, n_vertices * sizeof (static_vertex *));
polytope->vertices = vertices;

for (i = start; i < n_vertices; i++)
  static_vertex_create (i, polytope);
}

static gamgi_bool static_inside_local (int x, int y, int z, static_polytope *polytope)
{
double min_x, min_y, min_z, min;

/***********************************************************
 * x,y,z = number of cells from central to neighbour cell: *
 * n_x - cell_x = x,  n_y - cell_y = y,  n_z - cell_z = z  *
 ***********************************************************/

if (x < 0) x = -x;
if (y < 0) y = -y;
if (z < 0) z = -z;

x = x - 1; if (x < 0) x = 0;
y = y - 1; if (y < 0) y = 0;
z = z - 1; if (z < 0) z = 0;

min_x = x * polytope->width_x;
min_y = y * polytope->width_y;
min_z = z * polytope->width_z;

min = min_x * min_x + min_y * min_y + min_z * min_z;

/************************************************
 * radius contains the radius distance OR the   *
 * distance for the last neighbour found so far *
 ************************************************/

if (min > polytope->max) return FALSE;

return TRUE;
}

static gamgi_bool static_inside_global (int shell, static_polytope *polytope)
{
double min_x, min_y, min_z;
double max = polytope->max;

min_x = (shell - 1) * polytope->width_x;
min_y = (shell - 1) * polytope->width_y;
min_z = (shell - 1) * polytope->width_z;

/************************************************
 * radius contains the radius distance OR the   *
 * distance for the last neighbour found so far *
 ************************************************/

if (min_x > max && min_y > max && min_z > max) return FALSE;

return TRUE;
}

void static_neighbour_get (int n_x, int n_y, int n_z, static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom;
gamgi_slist *slist;
double *distance;
double x, y, z, d;
int n_neighbours, n;
int index, i;

n_neighbours = polytope->n_neighbours;
neighbours = polytope->neighbours;
distance = polytope->distance;

n = polytope->n_cells;
index = n_z * n * n + n_y * n + n_x;
for (slist = polytope->cells[index]; slist != NULL; slist = slist->next)
  {
  atom = GAMGI_CAST_ATOM slist->data;
  if (atom == polytope->atom) continue;
  if (polytope->element != -1 &&
  polytope->element != atom->element) continue;

  x = atom->position[0] - polytope->atom->position[0];
  y = atom->position[1] - polytope->atom->position[1];
  z = atom->position[2] - polytope->atom->position[2];
  d = x * x + y * y + z * z;

  /******************************************************************
   * when search is determined by distance, max is fixed            *
   * when search is determined by number, max decreases with search *
   ******************************************************************/

  if (d > polytope->max) continue;

  /**************************************************************
   * increase number of neighbours, when determined by distance *
   **************************************************************/

  if (polytope->number == 0 && ++n_neighbours > polytope->n_vertices)
    {
    static_vertices_increase (polytope);
    neighbours = polytope->neighbours;
    distance = polytope->distance;
    }

  /**********************************************
   * add atom to last position in arrays:       *
   * distances = array with neighbour distances *
   * neighbours = array with neighbour atoms    *
   **********************************************/

  distance[n_neighbours - 1] = d;
  neighbours[n_neighbours - 1] = atom;

  /***************************************************************
   * set atom in the right order in arrays according to distance *
   ***************************************************************/

  for (i = n_neighbours - 1; i > 0; i--)
    {
    if (distance[i] > distance[i - 1]) break;

    d = distance[i - 1];
    distance[i - 1] = distance[i];
    distance[i] = d;

    atom = neighbours[i - 1];
    neighbours[i - 1] = neighbours[i];
    neighbours[i] = atom;
    }

  /*************************************************
   * when the number of neighbours is fixed, save  *
   * the distance for last neighbour found so far, *
   * the maximum distance that is worth to scan    *
   *************************************************/

  if (polytope->number > 0) polytope->max = distance[n_neighbours - 1];
  }

polytope->n_neighbours = n_neighbours;
}

static void static_raw_x0 (int cell_x, int cell_y, int cell_z, 
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in -x faces *
 ****************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_y = j + cell_y;
    if (n_y < 0 || n_y > n - 1) continue;

    if (static_inside_local (-shell, j, i, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }                                              
}

static void static_raw_x1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in +x faces *
 ****************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_y = j + cell_y;
    if (n_y < 0 || n_y > n - 1) continue;

    if (static_inside_local (shell, j, i, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }
}

static void static_raw_y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in -y faces *
 ****************************************/

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_x = j + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    if (static_inside_local (j, -shell, i, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }
}

static void static_raw_y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in +y faces *
 ****************************************/

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_x = j + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    if (static_inside_local (j, shell, i, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }
}

static void static_raw_z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in -z faces *
 ****************************************/

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_y = i + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_x = j + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    if (static_inside_local (j, i, -shell, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }
}

static void static_raw_z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i, j;

n = polytope->n_cells;

/****************************************
 * find neighbours in cells in +z faces *
 ****************************************/

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_y = i + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_x = j + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    if (static_inside_local (j, i, shell, polytope) == TRUE)
      static_neighbour_get (n_x, n_y, n_z, polytope);
    }
  }
}

static void static_raw_x0y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0y0 edges *
 *********************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  if (static_inside_local (-shell, -shell, i, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x1y0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0y0 edges *
 *********************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  if (static_inside_local (shell, -shell, i, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x0y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0y0 edges *
 *********************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  if (static_inside_local (-shell, shell, i, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x1y1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int i;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0y0 edges *
 *********************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  if (static_inside_local (shell, shell, i, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int j;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0z0 edges *
 *********************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  if (static_inside_local (-shell, j, -shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int j;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x1z0 edges *
 *********************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  if (static_inside_local (shell, j, -shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int j;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x0z1 edges *
 *********************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  if (static_inside_local (-shell, j, shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int j;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along x1z1 edges *
 *********************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  if (static_inside_local (shell, j, shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int k;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along y0z0 edges *
 *********************************************/

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  if (static_inside_local (k, -shell, -shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int k;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along y1z0 edges *
 *********************************************/

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  if (static_inside_local (k, shell, -shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int k;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along y0z1 edges *
 *********************************************/

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  if (static_inside_local (k, -shell, shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;
int k;

n = polytope->n_cells;

/*********************************************
 * find neighbours in cells along y1z1 edges *
 *********************************************/

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  if (static_inside_local (k, shell, shell, polytope) == TRUE)
    static_neighbour_get (n_x, n_y, n_z, polytope);
  }
}

static void static_raw_x0y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x0y0z0 vertices *
 **************************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (-shell, -shell, -shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x1y0z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x1y0z0 vertices *
 **************************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (shell, -shell, -shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x0y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x0y1z0 vertices *
 **************************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (-shell, shell, -shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x1y1z0 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x1y1z0 vertices *
 **************************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (shell, shell, -shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x0y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x0y0z1 vertices *
 **************************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (-shell, -shell, shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x1y0z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x1y0z1 vertices *
 **************************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (shell, -shell, shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x0y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x0y1z1 vertices *
 **************************************************/

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (-shell, shell, shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static void static_raw_x1y1z1 (int cell_x, int cell_y, int cell_z,
int shell, static_polytope *polytope)
{
int n, n_x, n_y, n_z;

n = polytope->n_cells;

/**************************************************
 * find neighbours in cells along x1y1z1 vertices *
 **************************************************/

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

if (static_inside_local (shell, shell, shell, polytope) == TRUE)
  static_neighbour_get (n_x, n_y, n_z, polytope);
}

static gamgi_bool static_neighbours_find (int cell_x, int cell_y, int cell_z,
gamgi_slist *start, gamgi_atom *atom, static_polytope *polytope)
{
int shell, shell_max;
int n, min, max;

/*********************************************************
 * find a fixed number of neighbours around central atom *
 * or find all neighbours within a given distance        *
 *********************************************************/

/*********************************************************
 * go around cells (starting with central cell) and      *
 * create vertices array, with closer vertices first,    *
 * so the vertices more far away are handled first.      *
 *                                                       *
 * neighbours are ordered by increasing distances to     *
 * the central atom, so the last one will define a valid *
 * vertex in the final convex hull. Atoms more far away  *
 * will likely contribute more to the final convex hull. *
 *                                                       *
 * when applying the incremental algorithm, the atoms    *
 * more far away should be handled first, as the others  *
 * may fall inside the convex polyhedron and be removed  *
 * without requiring expensive polyhedra recalculations  *
 *********************************************************/

static_neighbour_get (cell_x, cell_y, cell_z, polytope);

n = polytope->n_cells;

min = cell_x;
if (cell_y < min) min = cell_y;
if (cell_z < min) min = cell_z;

max = cell_x;
if (cell_y > max) max = cell_y;
if (cell_z > max) max = cell_z;

shell_max = max;
if (shell_max < n - 1 - min) shell_max = n - 1 - min;
for (shell = 1; shell <= shell_max; shell++)
  {
  if (static_inside_global (shell, polytope) == FALSE) break;
    
  /*********
   * faces *
   *********/

  static_raw_x0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_y0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_y1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_z1 (cell_x, cell_y, cell_z, shell, polytope);

  /*********
   * edges *
   *********/

  static_raw_x0y0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x0y1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y1 (cell_x, cell_y, cell_z, shell, polytope);

  static_raw_x0z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x0z1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1z1 (cell_x, cell_y, cell_z, shell, polytope);

  static_raw_y0z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_y1z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_y0z1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_y1z1 (cell_x, cell_y, cell_z, shell, polytope);

  /************
   * vertices *
   ************/

  static_raw_x0y0z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y0z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x0y1z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y1z0 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x0y0z1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y0z1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x0y1z1 (cell_x, cell_y, cell_z, shell, polytope);
  static_raw_x1y1z1 (cell_x, cell_y, cell_z, shell, polytope);
  }

/************************************
 * Not enough neighbours were found *
 ************************************/

if (polytope->n_neighbours < 2 || 
polytope->distance[polytope->n_neighbours - 1] == DBL_MAX) return FALSE;

return TRUE;
}

static void static_neighbours_swap (static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom;
int i, n_neighbours, half;

neighbours = polytope->neighbours;
n_neighbours = polytope->n_neighbours;

/************************************************
 * swap atoms, so the last atom (the farthest   *
 * atom) becomes the first one and the first    *
 * atom (the closest atom) becomes the last one *
 *                                              *
 * When n_neighbours is even, half of the atoms *
 * wap with the other half. When n_neighbours   *
 * is odd, the central atom does not swap.      *
 ************************************************/

half = n_neighbours / 2;
for (i = 0; i < half; i++)
  {
  atom = neighbours[i];
  neighbours[i] = neighbours[n_neighbours - 1 - i];
  neighbours[n_neighbours - 1 - i] = atom;
  }

}

static int static_neighbours_independent (static_polytope *polytope)
{
gamgi_atom **neighbours;
gamgi_atom *atom;
int dot, cross, mix;
int n_neighbours, i;

/*******************************************************
 * At least two atoms are guaranteed to exist now,     *
 * but they might be too close to define a polytope!   *
 *                                                     *
 * neighbours are ordered by decreasing distances to   *
 * the central atom, so atom 0  will define a valid    *
 * vertex in the final convex hull. Atoms more far     *
 * away will contribute more to the final convex hull. *
 *******************************************************/

neighbours = polytope->neighbours;
n_neighbours = polytope->n_neighbours;

/**************************
 * look for second vertex *
 **************************/

for (i = 1; i < n_neighbours; i++)
  {
  dot = static_dot (0, i,
  GAMGI_CHEM_POLYTOPE_LENGTH, polytope);
  if (dot > 0)
    {
    /*********************************************
     * valid length: swap valid atom with atom 1 *
     *********************************************/

    atom = neighbours[1];
    neighbours[1] = neighbours[i];
    neighbours[i] = atom;
    break;
    }
  }
if (i == n_neighbours) return 1;

/*************************
 * look for third vertex *
 *************************/

for (i = 2; i < n_neighbours; i++)
  {
  cross = static_cross (0, 1, i,
  GAMGI_CHEM_POLYTOPE_AREA, polytope);
  if (cross > 0)
    {
    /*******************************************
     * valid area: swap valid atom with atom 2 *
     *******************************************/

    atom = neighbours[2];
    neighbours[2] = neighbours[i];
    neighbours[i] = atom;
    break;
    }
  }
if (i == n_neighbours) return 2;

/**************************
 * look for fourth vertex *
 **************************/

for (i = 3; i < n_neighbours; i++)
  {
  mix = static_mix (0, 1, 2, i, 
  GAMGI_CHEM_POLYTOPE_VOLUME, polytope); 
  if (mix != 0)
    {
    /*********************************************
     * valid volume: swap valid atom with atom 3 *
     *********************************************/

    atom = neighbours[3];
    neighbours[3] = neighbours[i];
    neighbours[i] = atom;

    if (mix < 0)
      { 
      /*******************************************************
       * swap atoms 1 and 2, to build a tetrahedron with all *
       * faces orientated counter-clockwise to the outside   *
       *******************************************************/

      atom = neighbours[1];
      neighbours[1] = neighbours[2];
      neighbours[2] = atom;
      }
    break;
    }
  }
if (i == n_neighbours) return 3;

/*****************************************************
 * four valid vertices were found: build tetrahedron *
 *****************************************************/

return 4;
}

static void static_arrays_create (static_polytope *polytope)
{
int i;

polytope->n_faces = STATIC_FACES_START;
polytope->faces = (static_face **)
malloc (polytope->n_faces * sizeof (static_face *));
for (i = 0; i < polytope->n_faces; i++)
  static_face_create (i, polytope);

polytope->n_edges = STATIC_EDGES_START;
polytope->edges = (static_edge **)
malloc (polytope->n_edges * sizeof (static_edge *));
for (i = 0; i < polytope->n_edges; i++)
  static_edge_create (i, polytope);

/*********************************************************************
 * n_vertices is the total size of arrays neighbours, distance and   *
 * vertices, while n_neighbours is the actual number of vertices.    *
 * When neighbours are fixed by number: n_vertices = n_neighbours    *
 * When neighbours are fixed by distance: n_neighbours <= n_vertices *
 *                                                                   *
 * n_vertices is initialized here and increased only when arrays     *
 * are not big enough. n_neighbours is initialized everytime a new   *
 * central atom is defined and either is constant when the number    *
 * of neighbours is fixed or ir given by the number of valid atoms   *
 * that can be found within the specified distance.                  *
 *********************************************************************/

if (polytope->number > 0) polytope->n_vertices = polytope->number;
else polytope->n_vertices = STATIC_VERTICES_START;

polytope->vertices = (static_vertex **) 
malloc (polytope->n_vertices * sizeof (static_vertex *));
for (i = 0; i < polytope->n_vertices; i++)
  static_vertex_create (i, polytope);

polytope->neighbours = (gamgi_atom **) malloc (polytope->n_vertices * sizeof (gamgi_atom *));
polytope->distance = (double *) malloc (polytope->n_vertices * sizeof (double));
}

static void static_arrays_start (static_polytope *polytope)
{
int n_faces, n_edges, n_vertices;
int i;

/****************************
 * reset topological arrays *
 ****************************/

n_edges = polytope->n_edges;
for (i = 0; i < n_edges; i++)
  static_edge_start (i, polytope);

n_vertices = polytope->n_vertices;
for (i = 0; i < n_vertices; i++)
  static_vertex_start (i, polytope);

n_faces = polytope->n_faces;
for (i = 0; i < n_faces; i++)
  static_face_start (i, polytope);

/***************************************
 * start shell data to find neighbours *
 ***************************************/

if (polytope->number > 0)
  {
  polytope->n_neighbours = polytope->n_vertices;

  polytope->max = DBL_MAX;
  for (i = 0; i < polytope->n_vertices; i++)
    polytope->distance[i] = DBL_MAX;
  }
else polytope->n_neighbours = 0;
}

static void static_arrays_remove (static_polytope *polytope)
{
int n_faces, n_edges, n_vertices;
int i;

n_faces = polytope->n_faces;
for (i = 0; i < n_faces; i++) free (polytope->faces[i]);
free (polytope->faces);

n_edges = polytope->n_edges;
for (i = 0; i < n_edges; i++) free (polytope->edges[i]);
free (polytope->edges);

n_vertices = polytope->n_vertices;
for (i = 0; i < n_vertices; i++) free (polytope->vertices[i]);
free (polytope->vertices);

free (polytope->neighbours);
free (polytope->distance);
}

static void static_cells_create (static_polytope *polytope)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_dlist *dlist;
double min_x, max_x;
double min_y, max_y;
double min_z, max_z; 
double width_x, width_y, width_z;
double r_cells, min;
int cell_x, cell_y, cell_z;
int central, neighbour;
int offset;
int n;

central = polytope->atom->element;
neighbour = polytope->element;

/**************************************************
 * determine: 1) system dimensions; 2) number of  *
 * atoms; 3) maximum radius (radical tesselation) *
 **************************************************/

min_x = DBL_MAX; max_x = -DBL_MAX;
min_y = DBL_MAX; max_y = -DBL_MAX;
min_z = DBL_MAX; max_z = -DBL_MAX;

n = 0;
for (dlist = polytope->atoms; dlist != NULL; dlist = dlist->next)
  {
  /*****************************************************
   * give a different, sequential, number to each atom *
   *                                                   *
   *     find minimum and maximum cell boundaries      *
   *                                                   *
   *     find maximum radius (radical tesselation)     *
   *****************************************************/

  atom = GAMGI_CAST_ATOM dlist->data;
  if (neighbour != -1 && 
  atom->element != central && atom->element != neighbour) continue;
  n++;

  if (atom->position[0] < min_x) min_x = atom->position[0];
  if (atom->position[0] > max_x) max_x = atom->position[0];

  if (atom->position[1] < min_y) min_y = atom->position[1];
  if (atom->position[1] > max_y) max_y = atom->position[1];

  if (atom->position[2] < min_z) min_z = atom->position[2];
  if (atom->position[2] > max_z) max_z = atom->position[2];
  }
polytope->n_atoms = n;

/************************************************************
 * determine number of cells on each direction:             *
 * on average, each cell has approximately one atom.        *
 *                                                          *
 * allocate array of cells:                                 *
 * each cell points to a linked list of atoms in that cell. *
 ************************************************************/

if (modf (pow (n, 1/3.0), &r_cells) > 0.5) r_cells++;
polytope->n_cells = n = r_cells;

cells = (gamgi_slist **) calloc (pow (n, 3), sizeof (gamgi_slist *));
polytope->cells = cells;

/***********************************************************
 * determine cell dimensions: when the width is too small  *
 * (for example when the atoms have the same z coordinate) *
 * we need to enlarge the cell dimensions symmetricaly, to *
 * guarantee that the distance from the seeds at old min,  *
 * max coordinates to the new border planes is the same    *
 ***********************************************************/

min = GAMGI_CHEM_POLYTOPE_WIDTH;

if (max_x - min_x < min)
  {
  min_x = (min_x + max_x - min) / 2.0;
  max_x = (min_x + max_x + min) / 2.0;
  }
width_x = (max_x - min_x) / n;

if (max_y - min_y < min)
  {
  min_y = (min_y + max_y - min) / 2.0;
  max_y = (min_y + max_y + min) / 2.0;
  }
width_y = (max_y - min_y) / n;

if (max_z - min_z < min)
  {
  min_z = (min_z + max_z - min) / 2.0;
  max_z = (min_z + max_z + min) / 2.0;
  }
width_z = (max_z - min_z) / n;

polytope->width_x = width_x;
polytope->width_y = width_y;
polytope->width_z = width_z;

/********************************************
 * 1) find cell where each atom belongs     *
 * 2) add atom to linked list for that cell *
 ********************************************/

for (dlist = polytope->atoms; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data; 
  if (neighbour != -1 &&
  atom->element != central && atom->element != neighbour) continue;

  cell_x = floor ((atom->position[0] - min_x) / width_x);
  if (cell_x == n) cell_x--;
  cell_y = floor ((atom->position[1] - min_y) / width_y);
  if (cell_y == n) cell_y--;
  cell_z = floor ((atom->position[2] - min_z) / width_z);
  if (cell_z == n) cell_z--;

  offset = cell_z * n * n + cell_y * n + cell_x;
  cells[offset] = gamgi_engine_slist_add_start (cells[offset]);
  cells[offset]->data = atom;
  }

}

static void static_cells_remove (static_polytope *polytope)
{
gamgi_slist **cells;
gamgi_slist *slist;
int n_cells;
int cell_x, cell_y, cell_z;
int offset;

/********************************************
 * remove lists of nodes, one for each cell *
 ********************************************/

cells = polytope->cells;
n_cells = polytope->n_cells;
for (cell_z = 0; cell_z < n_cells; cell_z++)
  {
  for (cell_y = 0; cell_y < n_cells; cell_y++)
    {
    for (cell_x = 0; cell_x < n_cells; cell_x++)
      {
      offset = cell_z * n_cells * n_cells + cell_y * n_cells + cell_x;
      slist = cells[offset];

      while (slist != NULL)
        slist = gamgi_engine_slist_remove_start (slist);
      }
    }
  }

}

static void static_build_tetrahedron (static_polytope *polytope)
{
static_face *face;
static_edge *edge;
static_vertex *vertex;
int *edges;

/*****************************************************************************
 * build a tetrahedron orientated counter-clockwise, with theses rules:      *
 *                                                                           *
 * set vertices v0, v1, v2, v3, so (v1 - v0) X (v2 - v0) . (v3 - v0) > 0     *
 *                                                                           *
 * set faces f0, f1, f2, f3, so the face number is the excluded vertex       *
 *                                                                           *
 * set edges e0, e1, e2, e3, e4, e5, with the vertex increasing combination: *
 * e0: v0v1  e1: v0v2  e2: v0v3  e3: v1v2  e4: v1v3  e5: v2v3                *
 *****************************************************************************/

/***************************
 * add vertices 0, 1, 2, 3 *
 ***************************/

vertex = polytope->vertices[0];
vertex->n_edges = 3;

vertex = polytope->vertices[1];
vertex->n_edges = 3;

vertex = polytope->vertices[2];
vertex->n_edges = 3;

vertex = polytope->vertices[3];
vertex->n_edges = 3;

/************************************************************
 *                add edges 0, 1, 2, 3, 4, 5                *
 *                                                          *
 * write first the face containing this vertex sequence     *
 * and then the face containing the reverse vertex sequence *
 *                                                          *
 * e0: f2 f3 v0 v1     e2: f1 f2 v0 v3      e4: f2 f0 v1 v3 *
 * e1: f3 f1 v0 v2     e3: f0 f3 v1 v2      e5: f0 f1 v2 v3 *
 ************************************************************/

edge = polytope->edges[0];
edge->state = STATIC_KEEP;
edge->f_start = 2;
edge->f_end = 3;
edge->v_start = 0;
edge->v_end = 1;

edge = polytope->edges[1];
edge->state = STATIC_KEEP;
edge->f_start = 3;
edge->f_end = 1;
edge->v_start = 0;
edge->v_end = 2;

edge = polytope->edges[2];
edge->state = STATIC_KEEP;
edge->f_start = 1;
edge->f_end = 2;
edge->v_start = 0;
edge->v_end = 3;

edge = polytope->edges[3];
edge->state = STATIC_KEEP;
edge->f_start = 0;
edge->f_end = 3;
edge->v_start = 1;
edge->v_end = 2;

edge = polytope->edges[4];
edge->state = STATIC_KEEP;
edge->f_start = 2;
edge->f_end = 0;
edge->v_start = 1;
edge->v_end = 3;

edge = polytope->edges[5];
edge->state = STATIC_KEEP;
edge->f_start = 0;
edge->f_end = 1;
edge->v_start = 2;
edge->v_end = 3;

/**********
 * face 0 *
 **********/

face = polytope->faces[0];
face->state = STATIC_KEEP;
face->n_edges = 3;

static_conflict_create (1, 2, 3, 0, polytope);
edges = face->edges;

edges[1] = 3;
edges[2] = 5;
edges[3] = 4;

/**********
 * face 1 *
 **********/

face = polytope->faces[1];
face->state = STATIC_KEEP;
face->n_edges = 3;

static_conflict_create (3, 2, 0, 1, polytope);
edges = face->edges;

edges[1] = 1;
edges[2] = 2;
edges[3] = 5;

/**********
 * face 2 *
 **********/

face = polytope->faces[2];
face->state = STATIC_KEEP;
face->n_edges = 3;

static_conflict_create (0, 1, 3, 2, polytope);
edges = face->edges;

edges[1] = 0;
edges[2] = 4;
edges[3] = 2;

/**********
 * face 3 *
 **********/

face = polytope->faces[3];
face->state = STATIC_KEEP;
face->n_edges = 3;

static_conflict_create (2, 1, 0, 3, polytope);
edges = face->edges;

edges[1] = 0;
edges[2] = 1;
edges[3] = 3;
}

static void static_build_polyhedron (static_polytope *polytope)
{
int i;

static_build_tetrahedron (polytope);

for (i = 4; i < polytope->n_neighbours; i++)
  static_vertex_build (i, polytope);

if (polytope->coplanar > 0.0)
  static_faces_merge (polytope);

static_export (polytope);
}

static int static_build_central (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_atom *atom, static_polytope *polytope)
{
int independent;

/**************************************
 * 1) set central atom and its parent *
 * 2) reset scanning, building arrays *
 * 3) reset conflict identifier       *
 **************************************/

static_atom (atom, polytope);
static_arrays_start (polytope);
polytope->id = 0;

if (static_neighbours_find (cell_x, cell_y, cell_z, 
start, atom, polytope) == FALSE) return FALSE;

static_neighbours_swap (polytope);

independent = static_neighbours_independent (polytope);
if (independent == 4)
  {
  static_build_polyhedron (polytope);
  return 4;
  }

if (independent == 3)
  {
  gamgi_phys_plane_polytope (polytope->n_neighbours, polytope->neighbours,
  polytope->structure, polytope->group->red, polytope->group->green, 
  polytope->group->blue, polytope->parent, polytope->atom);
  return 3;
  }

if (independent == 2)
  {
  gamgi_phys_direction_polytope (polytope->n_neighbours, polytope->neighbours, 
  polytope->structure, polytope->group->red, polytope->group->green, 
  polytope->group->blue, polytope->parent, polytope->atom);
  return 2;
  }

return 1;
}

static void static_expectancy (double *data, int n)
{
/*************************************************************************
 *                       get average, deviation                          *
 *                                                                       *
 * taking the absolute value is needed to avoid sqrt negative arguments, *
 * which can occur when deviation is 0, due to computer lack of precison *
 *************************************************************************/

data[2] /= n;
data[3] = sqrt (fabs (data[3] / n - data[2] * data[2]));
}

static void static_measure (int n_vertices, static_polytope *polytope)
{
gamgi_atom *atom, *atom_start, *atom_end;
static_face *face;
static_edge *edge;
static_vertex *vertex;
double base[3], radial[3], cross[3];
double length, x, y, z;
int i, j, e;

/*************************
 * get polyhedron center *
 *************************/

gamgi_math_vector_zero (polytope->center);
for (i = 0; i < polytope->n_neighbours; i++)
  {
  vertex = polytope->vertices[i];
  if (vertex->n_edges > 0)
    {
    atom = polytope->neighbours[i];
    gamgi_math_vector_add (polytope->center, atom->position, polytope->center);
    }
  }
gamgi_math_vector_scale (polytope->center, polytope->center, 1.0 / n_vertices);

/********************
 * get faces center *
 ********************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    gamgi_math_vector_zero (face->center);
    for (j = 1; j <= face->n_edges; j++)
      {
      e = face->edges[j];
      edge = polytope->edges[e];
      if (edge->f_start == i) atom = polytope->neighbours[edge->v_start];
      else atom = polytope->neighbours[edge->v_end];
      gamgi_math_vector_add (face->center, atom->position, face->center);
      }
    gamgi_math_vector_scale (face->center, face->center, 1.0 / face->n_edges);
    }
  }
   
/*****************************
 * get faces area and normal *
 *****************************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    face->area = 0.0;
    gamgi_math_vector_zero (face->normal);

    e = face->edges[face->n_edges];
    edge = polytope->edges[e];
    if (edge->f_start == i) atom = polytope->neighbours[edge->v_start];
    else atom = polytope->neighbours[edge->v_end];
    gamgi_math_vector_sub (atom->position, face->center, base);

    for (j = 1; j <= face->n_edges; j++)
      {
      e = face->edges[j];
      edge = polytope->edges[e];
      if (edge->f_start == i) atom = polytope->neighbours[edge->v_start];
      else atom = polytope->neighbours[edge->v_end];

      gamgi_math_vector_sub (atom->position, face->center, radial);
      gamgi_math_vector_cross (base, radial, cross);
      face->area += gamgi_math_vector_length (cross);
      gamgi_math_vector_add (face->normal, cross, face->normal);

      gamgi_math_vector_copy (radial, base);
      }

    /************************************************************
     * Normalize normal vector (perpendicular to face) and      *
     * get face area (the cross product is twice each triangle  *
     * area). For very small faces set area and normal to zero. *
     ************************************************************/

    length = gamgi_math_vector_length (face->normal);
    if (length < GAMGI_MATH_TOLERANCE_LENGTH)
      {
      gamgi_math_vector_zero (face->normal);
      face->area = 0.0;
      }
    else
      {
      gamgi_math_vector_scale (face->normal, face->normal, 1.0 / length);
      face->area /= 2.0;

      }
    }
  }

/*****************************
 * measure polyhedron volume *
 *****************************/

polytope->volume = 0.0;
for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    gamgi_math_vector_sub (face->center, polytope->center, radial);
    length = gamgi_math_vector_dot (radial, face->normal);
    polytope->volume += face->area * length / 3.0;
    }
  }

/***********************
 * measure edge length *
 ***********************/

for (i = 0; i < polytope->n_edges; i++)
  {
  edge = polytope->edges[i];
  if (edge->state == STATIC_KEEP)
    {
    atom_start = polytope->neighbours[edge->v_start];
    atom_end = polytope->neighbours[edge->v_end];
    x = atom_end->position[0] - atom_start->position[0];
    y = atom_end->position[1] - atom_start->position[1];
    z = atom_end->position[2] - atom_start->position[2];
    edge->length = sqrt (x * x + y * y + z * z);
    }
  }

/***********************
 * measure face length *
 ***********************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    face->length = 0.0;
    for (j = 1; j <= face->n_edges; j++)
      {
      e = face->edges[j];
      edge = polytope->edges[e];
      face->length += edge->length;
      }
    }
  }

}

static void static_report_start (int *n_polyhedrons, int *n_faces, int *n_edges,
int *n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles)
{
/*************************************************************
 *                 start topology data                       *
 *                                                           *
 * n_polyhedrons, n_faces, n_edges, n_vertices: number       *
 *                                                           *
 * polyhedron_faces, polyhedron_edges, polyhedron_vertices,  *
 * face_vertices, vertex_faces: min, max, average, deviation *
 *************************************************************/

*n_polyhedrons = 0;
*n_faces = 0;
*n_edges = 0;
*n_vertices = 0;

polyhedron_faces[0] = DBL_MAX;
polyhedron_faces[1] = -DBL_MAX;
polyhedron_faces[2] = 0.0;
polyhedron_faces[3] = 0.0;

polyhedron_edges[0] = DBL_MAX;
polyhedron_edges[1] = -DBL_MAX;
polyhedron_edges[2] = 0.0;
polyhedron_edges[3] = 0.0;

polyhedron_vertices[0] = DBL_MAX;
polyhedron_vertices[1] = -DBL_MAX;
polyhedron_vertices[2] = 0.0;
polyhedron_vertices[3] = 0.0;

face_vertices[0] = DBL_MAX;
face_vertices[1] = -DBL_MAX;
face_vertices[2] = 0.0;
face_vertices[3] = 0.0;

vertex_faces[0] = DBL_MAX;
vertex_faces[1] = -DBL_MAX;
vertex_faces[2] = 0.0;
vertex_faces[3] = 0.0;

/***********************************************************
 *                 start geometry data                     *
 *                                                         *
 * polyhedron_volumes, polyhedron_areas, polyhedron_edges, *
 * spherical_aberration, face_areas, face_lengths,         *
 * face_angles, circular_aberration, edge_lengths          *
 ***********************************************************/

polyhedron_volumes[0] = DBL_MAX;
polyhedron_volumes[1] = -DBL_MAX;
polyhedron_volumes[2] = 0.0;
polyhedron_volumes[3] = 0.0;

polyhedron_areas[0] = DBL_MAX;
polyhedron_areas[1] = -DBL_MAX;
polyhedron_areas[2] = 0.0;
polyhedron_areas[3] = 0.0;

polyhedron_lengths[0] = DBL_MAX;
polyhedron_lengths[1] = -DBL_MAX;
polyhedron_lengths[2] = 0.0;
polyhedron_lengths[3] = 0.0;

polyhedron_aberrations[0] = DBL_MAX;
polyhedron_aberrations[1] = -DBL_MAX;
polyhedron_aberrations[2] = 0.0;
polyhedron_aberrations[3] = 0.0;

face_areas[0] = DBL_MAX;
face_areas[1] = -DBL_MAX;
face_areas[2] = 0.0;
face_areas[3] = 0.0;

face_lengths[0] = DBL_MAX;
face_lengths[1] = -DBL_MAX;
face_lengths[2] = 0.0;
face_lengths[3] = 0.0;

face_aberrations[0] = DBL_MAX;
face_aberrations[1] = -DBL_MAX;
face_aberrations[2] = 0.0;
face_aberrations[3] = 0.0;

edge_lengths[0] = DBL_MAX;
edge_lengths[1] = -DBL_MAX;
edge_lengths[2] = 0.0;
edge_lengths[3] = 0.0;

edge_angles[0] = DBL_MAX;
edge_angles[1] = -DBL_MAX;
edge_angles[2] = 0.0;
edge_angles[3] = 0.0;
}

static void static_report_add (int *n_polyhedrons, int *n_faces, int *n_edges,
int *n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles,
static_polytope *polytope)
{
gamgi_atom *atom_start, *atom_end;
static_face *face;
static_edge *edge;
static_vertex *vertex;
double edge_start[3], edge_end[3];
double area, length;
double aberration, k, angle;
int p_faces, p_edges, p_vertices;
int e, v_start, v_end;
int i, j;

/*********************
 * get topology data *
 *********************/

/*************************
 * number of polyhedrons *
 *************************/

*n_polyhedrons += 1;

/*******************
 * number of faces *
 *******************/

p_faces = 0;
for (i = 0; i < polytope->n_faces; i++)
  { face = polytope->faces[i];
    if (face->state == STATIC_KEEP) p_faces++; }
*n_faces += p_faces;

/*******************
 * number of edges *
 *******************/

p_edges = 0;
for (i = 0; i < polytope->n_edges; i++)
  { edge = polytope->edges[i];
    if (edge->state == STATIC_KEEP) p_edges++; }
*n_edges += p_edges;

/**********************
 * number of vertices *
 **********************/

p_vertices = 0;
for (i = 0; i < polytope->n_neighbours; i++)
  { vertex = polytope->vertices[i];
    if (vertex->n_edges > 0) p_vertices++; }
*n_vertices += p_vertices;

/************************
 * faces per polyhedron *
 ************************/

if (p_faces < polyhedron_faces[0]) polyhedron_faces[0] = p_faces;
if (p_faces > polyhedron_faces[1]) polyhedron_faces[1] = p_faces;
polyhedron_faces[2] += p_faces;
polyhedron_faces[3] += p_faces * p_faces;

/************************
 * edges per polyhedron *
 ************************/

if (p_edges < polyhedron_edges[0]) polyhedron_edges[0] = p_edges;
if (p_edges > polyhedron_edges[1]) polyhedron_edges[1] = p_edges;
polyhedron_edges[2] += p_edges;
polyhedron_edges[3] += p_edges * p_edges;

/***************************
 * vertices per polyhedron *
 ***************************/

if (p_vertices < polyhedron_vertices[0]) polyhedron_vertices[0] = p_vertices;
if (p_vertices > polyhedron_vertices[1]) polyhedron_vertices[1] = p_vertices;
polyhedron_vertices[2] += p_vertices;
polyhedron_vertices[3] += (p_vertices) * (p_vertices);

/***************************
 * vertices|edges per face *
 ***************************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    if (face->n_edges < face_vertices[0]) face_vertices[0] = face->n_edges;
    if (face->n_edges > face_vertices[1]) face_vertices[1] = face->n_edges;
    face_vertices[2] += face->n_edges;
    face_vertices[3] += face->n_edges * face->n_edges;
    }
  }

/**************************
 * faces|edges per vertex *
 **************************/

for (i = 0; i < polytope->n_neighbours; i++)
  {
  vertex = polytope->vertices[i];
  if (vertex->n_edges > 0)
    {
    if (vertex->n_edges < vertex_faces[0]) vertex_faces[0] = vertex->n_edges;
    if (vertex->n_edges > vertex_faces[1]) vertex_faces[1] = vertex->n_edges;
    vertex_faces[2] += vertex->n_edges;
    vertex_faces[3] += vertex->n_edges * vertex->n_edges;
    }
  }

/*********************
 * get geometry data *
 *********************/

static_measure (p_vertices, polytope);

/**********************
 * polyhedron volumes *
 **********************/

if (polytope->volume < polyhedron_volumes[0]) polyhedron_volumes[0] = polytope->volume;
if (polytope->volume > polyhedron_volumes[1]) polyhedron_volumes[1] = polytope->volume;
polyhedron_volumes[2] += polytope->volume;
polyhedron_volumes[3] += polytope->volume * polytope->volume;

/********************
 * polyhedron areas *
 ********************/

area = 0.0;
for (i = 0; i < polytope->n_faces; i++)
  { face = polytope->faces[i];
    if (face->state == STATIC_KEEP) area += face->area; }
if (area < polyhedron_areas[0]) polyhedron_areas[0] = area;
if (area > polyhedron_areas[1]) polyhedron_areas[1] = area;
polyhedron_areas[2] += area;
polyhedron_areas[3] += area * area;

/**********************
 * polyhedron lengths *
 **********************/

length = 0.0;
for (i = 0; i < polytope->n_edges; i++)
  { edge = polytope->edges[i]; 
    if (edge->state == STATIC_KEEP) length += edge->length; }
if (length < polyhedron_lengths[0]) polyhedron_lengths[0] = length;
if (length > polyhedron_lengths[1]) polyhedron_lengths[1] = length;
polyhedron_lengths[2] += length;
polyhedron_lengths[3] += length * length;

/**************************
 * polyhedron aberrations *
 **************************/

k = pow (36 * GAMGI_MATH_PI, 1/3.0);
aberration = area / k / pow (polytope->volume, 2/3.0);
if (aberration < polyhedron_aberrations[0]) polyhedron_aberrations[0] = aberration;
if (aberration > polyhedron_aberrations[1]) polyhedron_aberrations[1] = aberration;
polyhedron_aberrations[2] += aberration;
polyhedron_aberrations[3] += aberration * aberration;

/**************
 * face areas *
 **************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    if (face->area < face_areas[0]) face_areas[0] = face->area;
    if (face->area > face_areas[1]) face_areas[1] = face->area;
    face_areas[2] += face->area;
    face_areas[3] += face->area * face->area;
    }
  }

/****************
 * face lengths *
 ****************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    if (face->length < face_lengths[0]) face_lengths[0] = face->length;
    if (face->length > face_lengths[1]) face_lengths[1] = face->length;
    face_lengths[2] += face->length;
    face_lengths[3] += face->length * face->length;
    }
  }

/********************
 * face aberrations *
 ********************/

k = 2 * sqrt (GAMGI_MATH_PI);
for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    aberration = face->length / k / sqrt (face->area);
    if (aberration < face_aberrations[0]) face_aberrations[0] = aberration;
    if (aberration > face_aberrations[1]) face_aberrations[1] = aberration;
    face_aberrations[2] += aberration;
    face_aberrations[3] += aberration * aberration;
    }
  }

/****************
 * edge lengths *
 ****************/

for (i = 0; i < polytope->n_edges; i++)
  {
  edge = polytope->edges[i];
  if (edge->state == STATIC_KEEP)
    {
    if (edge->length < edge_lengths[0]) edge_lengths[0] = edge->length;
    if (edge->length > edge_lengths[1]) edge_lengths[1] = edge->length;
    edge_lengths[2] += edge->length;
    edge_lengths[3] += edge->length * edge->length;
    }
  }

/***************
 * edge angles *
 ***************/

for (i = 0; i < polytope->n_faces; i++)
  {
  face = polytope->faces[i];
  if (face->state == STATIC_KEEP)
    {
    e = face->edges[face->n_edges];
    edge = polytope->edges[e];
    if (edge->f_start == i) { v_start = edge->v_start; v_end = edge->v_end; }
    else { v_start = edge->v_end; v_end = edge->v_start; }
    atom_start = polytope->neighbours[v_start];
    atom_end = polytope->neighbours[v_end];
    gamgi_math_vector_sub (atom_end->position, atom_start->position, edge_start);

    for (j = 1; j <= face->n_edges; j++)
      {
      e = face->edges[j];
      edge = polytope->edges[e];
      if (edge->f_start == i) { v_start = edge->v_start; v_end = edge->v_end; }
      else { v_start = edge->v_end; v_end = edge->v_start; }
      atom_start = polytope->neighbours[v_start];
      atom_end = polytope->neighbours[v_end];
      gamgi_math_vector_sub (atom_end->position, atom_start->position, edge_end);
      angle = 180 - gamgi_math_vector_angle (edge_start, edge_end);

      if (angle < edge_angles[0]) edge_angles[0] = angle;
      if (angle > edge_angles[1]) edge_angles[1] = angle;
      edge_angles[2] += angle;
      edge_angles[3] += angle * angle;

      gamgi_math_vector_copy (edge_end, edge_start);
      }
    }
  }


}

static void static_report_end (GtkWidget *text, int n_polyhedrons, int n_faces,
int n_edges, int n_vertices, double *polyhedron_faces, double *polyhedron_edges,
double *polyhedron_vertices, double *face_vertices, double *vertex_faces,
double *polyhedron_volumes, double *polyhedron_areas, double *polyhedron_lengths,
double *polyhedron_aberrations, double *face_areas, double *face_lengths,
double *face_aberrations, double *edge_lengths, double *edge_angles)
{
GtkTextBuffer *buffer;
GtkTextIter iter;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

/**********************
 * show topology data *
 **********************/

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedrons: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFaces: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", n_faces);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdges: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", n_edges);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertices: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", n_vertices);

static_expectancy (polyhedron_faces, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Faces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_faces[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_faces[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_faces[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_faces[3]);

static_expectancy (polyhedron_edges, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Edges\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_edges[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_edges[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_edges[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_edges[3]);

static_expectancy (polyhedron_vertices, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Vertices\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_vertices[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_vertices[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_vertices[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_vertices[3]);

static_expectancy (face_vertices, n_faces);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Vertices\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_vertices[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_vertices[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_vertices[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_vertices[3]);

static_expectancy (vertex_faces, n_vertices);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertex Faces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", vertex_faces[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", vertex_faces[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", vertex_faces[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", vertex_faces[3]);

/**********************
 * show geometry data *
 **********************/

static_expectancy (polyhedron_volumes, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Volumes\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_volumes[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_volumes[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_volumes[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_volumes[3]);

static_expectancy (polyhedron_areas, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Areas\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_areas[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_areas[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_areas[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_areas[3]);

static_expectancy (polyhedron_lengths, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_lengths[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_lengths[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_lengths[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_lengths[3]);

static_expectancy (polyhedron_aberrations, n_polyhedrons);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Aberrations\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_aberrations[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_aberrations[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_aberrations[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", polyhedron_aberrations[3]);

static_expectancy (face_areas, n_faces);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Areas\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_areas[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_areas[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_areas[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_areas[3]);

static_expectancy (face_lengths, n_faces);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_lengths[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_lengths[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_lengths[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_lengths[3]);

static_expectancy (face_aberrations, n_faces);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Aberrations\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_aberrations[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_aberrations[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_aberrations[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", face_aberrations[3]);

static_expectancy (edge_lengths, n_edges);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_lengths[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_lengths[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_lengths[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_lengths[3]);

static_expectancy (edge_angles, 2 * n_edges);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Angles\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_angles[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_angles[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_angles[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", edge_angles[3]);
}

static void static_build (int *out, GtkWidget *text, static_polytope *polytope)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_slist *slist, *start;
int n_polyhedrons, n_faces, n_edges, n_vertices;
double polyhedron_faces[4];
double polyhedron_edges[4];
double polyhedron_vertices[4];
double face_vertices[4];
double vertex_faces[4];
double polyhedron_volumes[4];
double polyhedron_areas[4];
double polyhedron_lengths[4];
double spherical_aberration[4];
double face_areas[4];
double face_lengths[4];
double face_angles[4];
double circular_aberration[4];
double edge_lengths[4];
int cell_x, cell_y, cell_z;
int n_cells, offset;
int independent;

out[GAMGI_MATH_POLYTOPE_1] = 0;
out[GAMGI_MATH_POLYTOPE_2] = 0;
out[GAMGI_MATH_POLYTOPE_3] = 0;
out[GAMGI_MATH_POLYTOPE_4] = 0;

if (text != NULL)
  static_report_start (&n_polyhedrons, &n_faces, &n_edges, &n_vertices, 
  polyhedron_faces, polyhedron_edges, polyhedron_vertices, face_vertices,
  vertex_faces, polyhedron_volumes, polyhedron_areas, polyhedron_lengths,
  spherical_aberration, face_areas, face_lengths, face_angles, circular_aberration,
  edge_lengths);

cells = polytope->cells;
n_cells = polytope->n_cells;

for (cell_z = 0; cell_z < n_cells; cell_z++)
  {
  for (cell_y = 0; cell_y < n_cells; cell_y++)
    {
    for (cell_x = 0; cell_x < n_cells; cell_x++)
      {
      offset = cell_z * n_cells * n_cells + cell_y * n_cells + cell_x;
      start = cells[offset];
      for (slist = start; slist != NULL; slist = slist->next)
        {
        /*****************************************************
         * build polyhedron from distance-ordered neighbours *
         *****************************************************/

        atom = GAMGI_CAST_ATOM slist->data;
        if (atom->element == polytope->atom->element && 
        (polytope->global == TRUE || atom == polytope->atom))
          {
          independent = static_build_central (cell_x, 
          cell_y, cell_z, start, atom, polytope);

          if (independent < 2) out[GAMGI_MATH_POLYTOPE_1] += 1;
          if (independent == 2) out[GAMGI_MATH_POLYTOPE_2] += 1;
          if (independent == 3) out[GAMGI_MATH_POLYTOPE_3] += 1;
          if (independent > 3) out[GAMGI_MATH_POLYTOPE_4] += 1;

          if (text != NULL && independent > 3)
            static_report_add (&n_polyhedrons, &n_faces, &n_edges, &n_vertices,
            polyhedron_faces, polyhedron_edges, polyhedron_vertices, face_vertices,
            vertex_faces, polyhedron_volumes, polyhedron_areas, polyhedron_lengths,
            spherical_aberration, face_areas, face_lengths, face_angles,
            circular_aberration, edge_lengths, polytope);

          }
        }

      }
    }
  }

if (text != NULL && out[GAMGI_MATH_POLYTOPE_4] > 0)
  {
  static_report_end (text, n_polyhedrons, n_faces, n_edges, n_vertices,
  polyhedron_faces, polyhedron_edges, polyhedron_vertices, face_vertices,
  vertex_faces, polyhedron_volumes, polyhedron_areas, polyhedron_lengths,
  spherical_aberration, face_areas, face_lengths, face_angles,
  circular_aberration, edge_lengths);
  }
}

static static_polytope *static_start (gamgi_dlist *atoms, gamgi_group *group,
gamgi_atom *atom, gamgi_bool global, gamgi_enum structure, double visible,
double coplanar, int element, int number, double radius)
{
static_polytope *polytope;

/***************************
 * allocate main structure *
 ***************************/

polytope = (static_polytope *) malloc (sizeof (static_polytope));

/********************************************************************
 * save input data: radius is a squared distance, to increase speed *
 ********************************************************************/

polytope->atoms = atoms;
polytope->group = group;
polytope->atom = atom;
polytope->global = global;
polytope->structure = structure;
polytope->visible = visible;
polytope->coplanar = coplanar;
polytope->element = element;
polytope->number = number;
polytope->max = radius;

polytope->color = TRUE;
if (polytope->group->red == -1.0)
  polytope->color = FALSE;

/**********************************************************
 * allocate atoms in cells with lists, for atom searching *
 *                                                        *
 * allocate arrays faces,edges,vertices to build polytope *
 *                                                        *
 *        allocate arrays to find neighbours              *
 *                                                        *
 *   start unique id counter, to retrieve old conflicts   *
 **********************************************************/

static_cells_create (polytope);
static_arrays_create (polytope);

return polytope;
}

static void static_end (static_polytope *polytope)
{
gamgi_dlist *dlist;

/**************************
 * free arrays and shells *
 **************************/

static_arrays_remove (polytope);
static_cells_remove (polytope);

/**********************
 * free list of atoms *
 **********************/

dlist = polytope->atoms;
while (dlist != NULL)
  dlist = gamgi_engine_dlist_remove_start (dlist);

/*****************
 * free template *
 *****************/

if (polytope->global == TRUE)
  gamgi_engine_remove_group (polytope->group);

free (polytope);
}

gamgi_bool gamgi_chem_polytope (gamgi_dlist *atoms, gamgi_group *group,
gamgi_atom *atom, gamgi_bool global, gamgi_bool structure, double visible,
double coplanar, int element, int number, double radius, int *out,
GtkWidget *text)
{
static_polytope *polytope;

polytope = static_start (atoms, group, atom,
global, structure, visible, coplanar, element, number, radius);
if (polytope == NULL) return FALSE;

static_build (out, text, polytope);
static_end (polytope);

return TRUE;
}
