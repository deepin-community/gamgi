/******************************************
 *
 * $GAMGI/src/phys/gamgi_phys_voronoi.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/************************************************************
 *                     Bibliography                         *
 *                                                          *
 * Luis Veiga da Cunha, Desenho Tecnico, Fundacao Calouste  *
 * Gulbenkian, Lisboa, 4a Edicao                            *
 * Volume of a triangular non-vertical piramid: V = Ah/3    *
 *                                                          *
 * R. E. Moore and I. O. Angell, Voronoi Polygons and       *
 * Polyhedra, J. of Computational Physics, 105, 301 (1993)  *
 * Describes 2D and 3D Voronoi algorithms similar to ours.  *
 *                                                          *
 * R. Sedgewick, Algorithms in C, 3rd Edition, (1998)       *
 * Describes the merge-sort algorithm to order linked lists *
 *                                                          *
 * A.M. Kraynik, D.A. Reinelt F. Swol, Structure of Random  *
 * Monodisperse Foam, Physical Review E 67, 031403 (2003)   *
 ************************************************************/

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_dlist.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"

/***********************
 * topological objects *
 ***********************/

typedef struct _gamgi_vertex {
double position[3];
double order;
void *last;
int n_polyhedrons, n_faces, n_edges;
struct _gamgi_vertex *vertex; } gamgi_vertex;

typedef struct _gamgi_face {
int local, global;
double position[3];
double area;
gamgi_slist *vertex, *edge;
int n_vertices; } gamgi_face;

typedef struct _gamgi_edge {
double position[3];
double length;
gamgi_vertex *vertex1;
gamgi_vertex *vertex2;
gamgi_face *face1;
gamgi_face *face2;
void *last;
int n_polyhedrons, n_faces;
struct _gamgi_edge *edge; } gamgi_edge;

typedef struct _gamgi_polyhedron {
int local;
double position[3];
double radius;
double volume;
float color[3];
gamgi_slist *face, *edge, *vertex; } gamgi_polyhedron;

/******************
 * main structure *
 ******************/

typedef struct _gamgi_voronoi {

/**************************************
 * input data:                        *
 * periodic: type of boundary         *
 * border: boundary offset            *
 * variancy: tesselation weights      *
 * apex,area,vertex (build tolerance) *
 * color (pre-defined or atom color)  *
 * borders (all or faces)             *
 **************************************/

gamgi_bool periodic;
double offset;
double variancy;
double apex, area, vertex;
float *color; 
gamgi_enum borders;

/**************
 * build data *
 **************/

int n_seeds, n_cells;
gamgi_polyhedron **polyhedron;
gamgi_slist **cells;
gamgi_dlist *dlist;

double size, radius;
double min_x, max_x;
double min_y, max_y;
double min_z, max_z;

/*****************
 * analysis data *
 *****************/

double volume[2];
double anisotropy[9];

/****************
 * build arrays *
 ****************/

int *neighbours;
double *neighbours_xyz;
int *vertices;
double *vertices_xyz;
gamgi_face **faces;
int *shift;
int *update;

/**********
 * status *
 **********/

gamgi_bool valid; } gamgi_voronoi;

static double static_border (int shell, double r2, double border)
{
double scale;

border *= border;
if (shell == 1 || r2 < -border) return 0.0;

scale = 1.0 + r2 / border;
border *= scale * scale;

return border;
}

static void static_pbc (int n, double width, int *n_i, double *shift_i)
{
*shift_i = 0.0;
if (*n_i < 0) { *n_i += n; *shift_i = -width; }
if (*n_i > n - 1) { *n_i -= n; *shift_i = width; }
}

static int static_compare (const void *sa, const void *sb)
{
gamgi_vertex *va = (gamgi_vertex *) ((gamgi_slist *) sa)->data;
gamgi_vertex *vb = (gamgi_vertex *) ((gamgi_slist *) sb)->data;

if (va->order > vb->order) return 1;

return 0;
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

static gamgi_edge *static_edge (gamgi_edge *edge)
{
/*******************************************
 * edge is unchanged outside this function *
 *******************************************/

while (edge->edge != NULL) edge = edge->edge;

return edge;
}

static gamgi_vertex *static_vertex (gamgi_vertex *vertex)
{
/*********************************************
 * vertex is unchanged outside this function *
 *********************************************/

while (vertex->vertex != NULL) vertex = vertex->vertex;

return vertex;
}

static gamgi_vertex *static_vertex_create (double x, double y, double z)
{
gamgi_vertex *vertex;

/***************
 * vertex data *
 ***************/

vertex = (gamgi_vertex *) malloc (sizeof (gamgi_vertex));
vertex->position[0] = x/2;
vertex->position[1] = y/2;
vertex->position[2] = z/2;

/*******************
 * connection data *
 *******************/

vertex->order = 0.0;
vertex->vertex = NULL;

/*******************
 * collection data *
 *******************/

vertex->last = NULL;
vertex->n_polyhedrons = 0;
vertex->n_faces = 0;
vertex->n_edges = 0;

return vertex;
}

static void static_vertex_remove (gamgi_vertex *vertex)
{
free (vertex);
}

static gamgi_edge *static_edge_create (double x,
double y, double z, double length)
{
gamgi_edge *edge;

/*************
 * edge data *
 *************/

edge = (gamgi_edge *) malloc (sizeof (gamgi_edge));
edge->position[0] = x;
edge->position[1] = y;
edge->position[2] = z;
edge->length = length;

/*******************
 * connection data *
 *******************/

edge->vertex1 = NULL;
edge->vertex2 = NULL;
edge->face1 = NULL;
edge->face2 = NULL;
edge->edge = NULL;

/*******************
 * collection data *
 *******************/

edge->last = NULL;
edge->n_polyhedrons = 0;
edge->n_faces = 0;

return edge;
}

static void static_edge_remove (gamgi_edge *edge)
{
free (edge);
}

static gamgi_face *static_face_create (int local, 
int global, double x, double y, double z) 
{
gamgi_face *face;
 
/*************
 * face data *
 *************/

face = (gamgi_face *) malloc (sizeof (gamgi_face));
face->local = local;
face->global = global;
face->position[0] = x;
face->position[1] = y;
face->position[2] = z;
face->area = 0.0;

/*******************
 * connection data *
 *******************/

face->vertex = NULL;
face->edge = NULL;

/*******************
 * collection data *
 *******************/

face->n_vertices = 0;

return face;
}

static void static_face_remove (gamgi_face *face)
{
gamgi_slist *slist;
gamgi_edge *edge;

/****************************
 * remove vertex nodes only *
 ****************************/

slist = face->vertex;
while (slist != NULL)
  slist = gamgi_engine_slist_remove_start (slist);

/*******************************
 * remove edge nodes AND edges *
 *******************************/

slist = face->edge;
while (slist != NULL)
  {
  edge = (gamgi_edge *) slist->data;
  static_edge_remove (edge);
  slist = gamgi_engine_slist_remove_start (slist);
  }

free (face);
}

static gamgi_polyhedron *static_polyhedron_create (double *position,
double radius, int mark, float red, float green, float blue)
{
gamgi_polyhedron *polyhedron;
 
/*******************
 * polyhedron data *
 *******************/

polyhedron = (gamgi_polyhedron *) malloc (sizeof (gamgi_polyhedron));
polyhedron->local = mark;
polyhedron->position[0] = position[0];
polyhedron->position[1] = position[1];
polyhedron->position[2] = position[2];
polyhedron->radius = radius;
polyhedron->color[0] = red;
polyhedron->color[1] = green;
polyhedron->color[2] = blue;
polyhedron->volume = 0.0;

/*******************
 * connection data *
 *******************/

polyhedron->face = NULL;
polyhedron->edge = NULL;
polyhedron->vertex = NULL;

return polyhedron;
}

static void static_polyhedron_remove (gamgi_polyhedron *polyhedron)
{
gamgi_face *face;
gamgi_vertex *vertex;
gamgi_slist *slist;

/*******************************
 * remove face nodes AND faces *
 *******************************/

slist = polyhedron->face;
while (slist != NULL)
  {
  face = (gamgi_face *) slist->data;
  static_face_remove (face);
  slist = gamgi_engine_slist_remove_start (slist);
  }

/**************************
 * remove edge nodes only *
 **************************/

slist = polyhedron->edge;
while (slist != NULL)
  slist = gamgi_engine_slist_remove_start (slist);

/**************************************
 * remove vertices nodes AND vertices *
 **************************************/

slist = polyhedron->vertex;
while (slist != NULL)
  {
  vertex = (gamgi_vertex *) slist->data;
  static_vertex_remove (vertex);
  slist = gamgi_engine_slist_remove_start (slist);
  }

free (polyhedron);
}

static gamgi_voronoi *static_start (gamgi_object *object, 
gamgi_bool periodic, double offset, double variancy, double apex, 
double area, double vertex, float *color, gamgi_enum borders)
{
gamgi_voronoi *voronoi;
gamgi_dlist *dlist;

/*****************************************
 * allocate main structure and seed list *
 *****************************************/

dlist = gamgi_engine_dlist_atom_object (object, NULL);
if (dlist == NULL) return NULL;

voronoi = (gamgi_voronoi *) malloc (sizeof (gamgi_voronoi));
voronoi->dlist = dlist;

/*******************
 * save input data *
 *******************/

voronoi->periodic = periodic;
voronoi->offset = offset;
voronoi->variancy = variancy * variancy;
voronoi->apex = apex;
voronoi->area = area;
voronoi->vertex = vertex;
voronoi->color = color;
voronoi->borders = borders;

return voronoi;
}

static void static_end (gamgi_voronoi *voronoi)
{
gamgi_dlist *dlist;
int n_seeds, i;

/**************************************
 * remove resources and reset atom    *
 * flags (used to identify the seeds) *
 **************************************/

dlist = voronoi->dlist;
while (dlist != NULL)
  {
  (GAMGI_CAST_ATOM dlist->data)->mark = 0;
  dlist = gamgi_engine_dlist_remove_start (dlist);
  }

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  static_polyhedron_remove (voronoi->polyhedron[i]);

free (voronoi);
}

static void static_face_orientate (gamgi_face *face)
{
gamgi_slist *slist, *start;
gamgi_vertex *vertex;
double center[3], base[3], up[3], normal[3], radial[3];
double length, dot, area;
int n_vertices;

start = face->vertex;

/********************************************************
 * determine the average vector of all vertices: as the *
 * face is convex, this vector must be inside the face  *
 ********************************************************/

n_vertices = 0;
gamgi_math_vector_zero (center);
for (slist = start; slist != NULL; slist = slist->next)
  {
  vertex = (gamgi_vertex *) slist->data;
  gamgi_math_vector_add (center, vertex->position, center);
  n_vertices++;
  }
if (n_vertices < 3) { face->area = 0.0; return; }
gamgi_math_vector_scale (center, center, 1.0/n_vertices);

/*********************************************
 * determine the horizontal reference vector *
 * going from the center to the first vertex *
 *                                           *
 * determine the vector normal to the face,  *
 * pointing from the inside to the outside   *
 *                                           *
 * determine the cross product between the   *
 * first radial vector and the normal vector *
 *                                           *
 * the new vector is on the face and points  *
 * in the up direction, when compared with   *
 * the first radial vector, which points, by *
 * construction, in the horizontal direction *
 *********************************************/

vertex = (gamgi_vertex *) start->data;
gamgi_math_vector_sub (vertex->position, center, base);
length = gamgi_math_vector_length (base);
if (length < GAMGI_MATH_TOLERANCE) { face->area = 0.0; return; }

gamgi_math_vector_scale (base, base, 1.0/length);
gamgi_math_vector_copy (face->position, normal);
gamgi_math_vector_normal (normal);
gamgi_math_vector_cross (base, normal, up);

/***************************************************************
 * determine all the radial vectors, going from the center to  *
 * the vertices (the first radial vector was already handled)  *
 *                                                             *
 * When the radial vectors are oriented clockwise (counter-    *
 * clockwise from the outside), starting with the first radial *
 * vector, the order attribute defined below goes from -2.0 to *
 * +2.0. This order criterium can thus be used to sort the     *
 * vectors and orientate the vertices, which we need to        *
 * properly draw the two-dimension polygon face.               *
 *                                                             *
 * Set vertex->order to +2.0, so the first vertex will be the  *
 * last one in the ordered list, which is convenient to build  *
 * a cyclic list and to measure the area (other vertices can   *
 * be close to +2.0 or exactly -2.0, only). Moving the first   *
 * vertex to the end does not increase the CPU time needed.    *
 ***************************************************************/

vertex->order = +2.0;
for (slist = start->next; slist != NULL; slist = slist->next)
  {
  vertex = (gamgi_vertex *) slist->data;
  gamgi_math_vector_sub (vertex->position, center, radial);
  length = gamgi_math_vector_length (radial);
  if (length < GAMGI_MATH_TOLERANCE) { face->area = 0.0; return; }

  gamgi_math_vector_scale (radial, radial, 1.0/length);
  vertex->order = gamgi_math_vector_dot (radial, base);
  dot = gamgi_math_vector_dot (radial, up);
  if (dot > 0)
    vertex->order = +vertex->order + 1;
  else
    vertex->order = -vertex->order - 1;
  }

/*******************************************************
 *  merge-sort the vertices linked list, to orientate  *
 *  the face counter-clockwise, seen from the outside  *
 *                                                     *
 * to create a cyclic list: start->next = face->vertex *
 *******************************************************/

face->vertex = gamgi_engine_slist_msort (face->vertex, static_compare);

/***********************
 * determine face area *
 ***********************/

area = 0.0;
vertex = (gamgi_vertex *) start->data;
gamgi_math_vector_sub (vertex->position, center, base);
for (slist = face->vertex; slist != NULL; slist = slist->next)
  {
  vertex = (gamgi_vertex *) slist->data;
  gamgi_math_vector_sub (vertex->position, center, radial);
  gamgi_math_vector_cross (base, radial, normal);
  area += gamgi_math_vector_length (normal);

  gamgi_math_vector_copy (radial, base);
  }

face->area = area / 2.0;
}

static void static_update_add (int offset, gamgi_voronoi *voronoi)
{
int *update;
int n;

/************************************************
 * index[0]: total number of positions in array *
 * index[1]: number of used positions in array  *
 ************************************************/

update = voronoi->update;

/**********************
 * realloc face_index *
 **********************/

if (update[0] == update[1])
  {
  update[0] += GAMGI_PHYS_VORONOI_ARRAY;
  n = 2 + update[0];
  update = (int *) realloc (update, n * sizeof (int));
  voronoi->update = update;
  }

update[++update[1] + 1] = offset;
}

static void static_neighbour_add (double x, double y, 
double z, int local, int global, gamgi_voronoi *voronoi)
{
double *neighbours_xyz;
int *neighbours, *shift;
gamgi_face **faces;
int n;

/************************************************
 * index[0]: total number of positions in array *
 * index[1]: number of used positions in array  *
 ************************************************/

neighbours_xyz = voronoi->neighbours_xyz;
neighbours = voronoi->neighbours;
shift = voronoi->shift;
faces = voronoi->faces;

/*********************************************
 * realloc neighbour_coord, neighbour_index, *
 * neighbour_shift, neighbour_address        *
 *********************************************/

if (neighbours[0] == neighbours[1])
  {
  neighbours[0] += GAMGI_PHYS_VORONOI_ARRAY;

  n = 3 * neighbours[0];
  neighbours_xyz = (double *) realloc (neighbours_xyz, n * sizeof (double));
  voronoi->neighbours_xyz = neighbours_xyz;

  n = 2 + 4 * neighbours[0];
  neighbours = (int *) realloc (neighbours, n * sizeof (int));
  voronoi->neighbours = neighbours;

  n = neighbours[0];
  shift = (int *) realloc (shift, n * sizeof (int));
  voronoi->shift = shift;

  n = neighbours[0];
  faces = (gamgi_face **) realloc (faces, n * sizeof (gamgi_face *));
  voronoi->faces = faces;
  }

/********************
 * face coordinates *
 ********************/

neighbours_xyz[3 * neighbours[1] + 0] = x;
neighbours_xyz[3 * neighbours[1] + 1] = y;
neighbours_xyz[3 * neighbours[1] + 2] = z;

/********************************************************
 * initialize number of vertices and add identification *
 ********************************************************/

neighbours[4 * neighbours[1] + 2] = 0;
neighbours[4 * neighbours[1] + 3] = 0;
neighbours[4 * neighbours[1] + 4] = local;
neighbours[4 * neighbours[1] + 5] = global;

/*************************************************
 * auxiliary information to correct vertex to    *
 * neighbours pointing, after neighbour scanning *
 *************************************************/

shift[neighbours[1]] = 0;

/*********************************
 * increase number of neighbours *
 *********************************/
 
neighbours[1]++;
}

static void static_vertex_add (int neighbour1, int neighbour2, int neighbour3, 
double x, double y, double z, gamgi_voronoi *voronoi)
{
double *vertices_xyz;
int *vertices, *neighbours;
int n;

/************************************************
 * index[0]: total number of positions in array *
 * index[1]: number of used positions in array  *
 ************************************************/

vertices_xyz = voronoi->vertices_xyz;
vertices = voronoi->vertices;

/**************************************
 * realloc vertex_coord, vertex_index *
 **************************************/

if (vertices[0] == vertices[1])
  {
  vertices[0] += GAMGI_PHYS_VORONOI_ARRAY;

  n = 3 * vertices[0];
  vertices_xyz = (double *) realloc (vertices_xyz, n * sizeof (double));
  voronoi->vertices_xyz = vertices_xyz;

  n = 2 + 3 * vertices[0];
  vertices = (int *) realloc (vertices, n * sizeof (int));
  voronoi->vertices = vertices;
  }

/**********************
 * vertex coordinates *
 **********************/

vertices_xyz[3 * vertices[1] + 0] = x;
vertices_xyz[3 * vertices[1] + 1] = y;
vertices_xyz[3 * vertices[1] + 2] = z;

/******************************
 * neighbours defining vertex *
 ******************************/

vertices[3 * vertices[1] + 2] = neighbour1;
vertices[3 * vertices[1] + 3] = neighbour2;
vertices[3 * vertices[1] + 4] = neighbour3;

/*******************************
 * increase number of vertices *
 *******************************/

vertices[1]++;

/*********************************************
 * increase neighbour references to vertices *
 *********************************************/

neighbours = voronoi->neighbours;

neighbours[4 * neighbour1 + 2]++;
neighbours[4 * neighbour1 + 3]++;

neighbours[4 * neighbour2 + 2]++;
neighbours[4 * neighbour2 + 3]++;

neighbours[4 * neighbour3 + 2]++;
neighbours[4 * neighbour3 + 3]++;
}

static gamgi_bool static_vertex_find (double *n1, double *n2, double *n3, double *v)
{
double m[9];
double a, b, c, d;
double d1, d2, d3;

/*****************************************
 * determine the apex of the triangular  *
 * piramid defined by neighbours 1,2,3   *
 *                                       *
 * 1) calculate determinants: a, b, c, d *
 * 2) detect and remove aligned points   *
 * 3) use the Cramer's rule to get apex  * 
 *****************************************/

d1 = n1[0] * n1[0] + n1[1] * n1[1] + n1[2] * n1[2];
d2 = n2[0] * n2[0] + n2[1] * n2[1] + n2[2] * n2[2];
d3 = n3[0] * n3[0] + n3[1] * n3[1] + n3[2] * n3[2];

/******************************************************
 * When d=0, the four points (seed,1,2,3) are in the  *
 * same plane, so they cannot define a proper vertex. *
 ******************************************************/

gamgi_math_matrix_absolute (m,
n1[0], n1[1], n1[2], n2[0], n2[1], n2[2], n3[0], n3[1], n3[2]);
d = gamgi_math_matrix_determinant (m);

if (fabs (d) < GAMGI_MATH_TOLERANCE) return FALSE;

/******************************************************
 *                   Cramer's rule:                   *
 *                                                    *
 *   for the system:        get the determinant:      *
 * a1x + b1y + c1z = d1       D = | a1 b1 c1 |        *
 * a2x + b2y + c2z = d2           | a2 b2 c2 |        *
 * a3x + b3y + c3z = d3           | a3 b3 c3 |        *
 *                                                    *
 *   if D = 0 the system has no unique solution,      *
 *                  if D != 0:                        *
 *                                                    *
 * x = | d1 b1 c1 | y = | a1 d1 c1 | z = | a1 b1 d1 | *
 *     | d2 b2 c2 |     | a2 d2 c2 |     | a2 b2 d2 | *
 *     | d3 b3 c3 |     | a3 d3 c3 |     | a3 b3 d3 | *
 *     ------------     ------------     ------------ *
 *          D                D                D       *
 ******************************************************/

gamgi_math_matrix_absolute (m,
d1, n1[1], n1[2], d2, n2[1], n2[2], d3, n3[1], n3[2]);
a = gamgi_math_matrix_determinant (m);

gamgi_math_matrix_absolute (m,
n1[0], d1, n1[2], n2[0], d2, n2[2], n3[0], d3, n3[2]);
b = gamgi_math_matrix_determinant (m);

gamgi_math_matrix_absolute (m,
n1[0], n1[1], d1, n2[0], n2[1], d2, n3[0], n3[1], d3);
c = gamgi_math_matrix_determinant (m);

v[0] = a / d;
v[1] = b / d;
v[2] = c / d;

return TRUE;
}

static gamgi_bool static_face_hide (double *n, double *v, double tolerance)
{
double nn, nv;

nn = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
nv = n[0]*v[0] + n[1]*v[1] + n[2]*v[2];

/****************************************
 * check if face hides vertex from seed *
 *                                      *
 * face does not hide vertex when the   *
 * two vectors point to opposite sides  *
 ****************************************/

if (nn + tolerance < nv) return TRUE;

/*****************************
 * face does not hide vertex *
 *****************************/

return FALSE;
}

static void static_vertex_shift (gamgi_voronoi *voronoi)
{
int *shift;
int *vertices;
int n_vertices, i;

shift = voronoi->shift;
vertices = voronoi->vertices;

/**********************************************
 * update vertex references to its neighbours *
 **********************************************/

n_vertices = vertices[1];
for (i = 0; i < n_vertices; i++)
  {
  vertices[2] -= shift[vertices[2]];
  vertices[3] -= shift[vertices[3]];
  vertices[4] -= shift[vertices[4]];
  vertices += 3;
  }
}

static void static_polyhedron_add (double *size, gamgi_voronoi *voronoi)
{
int i, j, k;

/****************************************
 * 1) initialize number of neighbours:  *
 * 2) add 6 default border neighbors,   *
 * using special face identification to *
 * distinguish border and proper faces. *
 * local: negative integers; global:    *
 * always 0, even with periodic borders * 
 ****************************************/

voronoi->neighbours[1] = 0;
static_neighbour_add (size[0], 0.0, 0.0, -1, 0, voronoi);
static_neighbour_add (size[1], 0.0, 0.0, -2, 0, voronoi);
static_neighbour_add (0.0, size[2], 0.0, -3, 0, voronoi);
static_neighbour_add (0.0, size[3], 0.0, -4, 0, voronoi);
static_neighbour_add (0.0, 0.0, size[4], -5, 0, voronoi);
static_neighbour_add (0.0, 0.0, size[5], -6, 0, voronoi);

/***************************************
 * 1) initialize number of vertices    *
 * 2) add the corresponding 8 vertices *
 ***************************************/

voronoi->vertices[1] = 0;
for (i = 0; i <= 1; i++)
  for (j = 2; j <= 3; j++)
    for (k = 4; k <= 5; k++)
      static_vertex_add (i, j, k, size[i], size[j], size[k], voronoi);
}

static void static_polyhedron_update (gamgi_voronoi *voronoi)
{
double *neighbours_xyz;
double v[3];
double *n1, *n2, *n3, *n4;
int *update;
int i, j, k, l;
gamgi_bool hide;

neighbours_xyz = voronoi->neighbours_xyz;
update = voronoi->update;

/***********************************************
 * update[0]: maximum number of faces in array *
 * update[1]: current number of faces in array *
 ***********************************************/

i = update[1];

/*******************
 * first neighbour *
 *******************/

n1 = neighbours_xyz + 3 * update[i + 1];

/*********************************************
 * the new vertices are formed from the last *
 * neighbour plus two previous neighbours    *
 *********************************************/

for (j = 2; j < i; j++)
  {
  /********************
   * second neighbour *
   ********************/

  n2 = neighbours_xyz + 3 * update[j + 1];

  for (k = 1; k < j; k++)
    {
    /*******************
     * third neighbour *
     *******************/

    n3 = neighbours_xyz + 3 * update[k + 1];

    /**********************************************************
     * determine vertex when the four points are not coplanar *
     **********************************************************/

    if (static_vertex_find (n1, n2, n3, v) == FALSE) continue;

    /**************************************************
     * for each candidate vertex formed by neighbours *
     * i,j,k go through all the other neighbours and  *
     * check if their faces hide the candidate vertex *
     **************************************************/

    hide = FALSE;
    for (l = 1; l < i; l++)
      {
      if (l == j || l == k) continue;

      /********************
       * fourth neighbour *
       ********************/

      n4 = neighbours_xyz + 3 * update[l + 1];

      /****************************************
       * check all faces that are on the      *
       * half-space containing the vertex,    *
       * defined by a plane containing the    *
       * seed and normal to the vertex vector *
       ****************************************/

      hide = static_face_hide (n4, v, voronoi->apex);
      if (hide == TRUE) break;
      }

    /**************
     * new vertex *
     **************/

    if (hide == FALSE)
      static_vertex_add (update[i+1], update[j+1], 
      update[k+1], v[0], v[1], v[2], voronoi);
    }
  }

}

static void static_size_update (gamgi_voronoi *voronoi)
{
double *vertices_xyz;
double size, length;
int n_vertices, i;

vertices_xyz = voronoi->vertices_xyz;

size = 0.0;
n_vertices = voronoi->vertices[1];
for (i = 0; i < n_vertices; i++)
  {
  length = vertices_xyz[0] * vertices_xyz[0] + 
  vertices_xyz[1] * vertices_xyz[1] + vertices_xyz[2] * vertices_xyz[2];
  if (length > size) size = length;
  vertices_xyz += 3;
  }

voronoi->size = size + GAMGI_MATH_TOLERANCE;
}

static void static_polyhedron_copy (gamgi_polyhedron *polyhedron, gamgi_voronoi *voronoi)
{
gamgi_face **faces;
gamgi_face *face;
gamgi_vertex *vertex;
double *neighbours_xyz, *vertices_xyz;
int *vertices, *neighbours;
int n_vertices, n_neighbours, i;

neighbours_xyz = voronoi->neighbours_xyz;
neighbours = voronoi->neighbours;
faces = voronoi->faces;

n_neighbours = neighbours[1];
neighbours += 2;
for (i = 0; i < n_neighbours; i++)
  {
  face = static_face_create (neighbours[2], neighbours[3], 
  neighbours_xyz[0], neighbours_xyz[1], neighbours_xyz[2]);
  polyhedron->face = gamgi_engine_slist_add_start (polyhedron->face);
  polyhedron->face->data = (void *) face;

  faces[0] = face;
  neighbours_xyz += 3; neighbours += 4; faces++;
  }

vertices_xyz = voronoi->vertices_xyz;
vertices = voronoi->vertices;
faces = voronoi->faces;

n_vertices = vertices[1];
vertices += 2;
for (i = 0; i < n_vertices; i++)
  {
  vertex = static_vertex_create (vertices_xyz[0], vertices_xyz[1], vertices_xyz[2]);
  polyhedron->vertex = gamgi_engine_slist_add_start (polyhedron->vertex);
  polyhedron->vertex->data = (void *) vertex;

  face = faces[vertices[0]];
  face->vertex = gamgi_engine_slist_add_start (face->vertex);
  face->vertex->data = (void *) vertex;
  face->n_vertices++;

  face = faces[vertices[1]];
  face->vertex = gamgi_engine_slist_add_start (face->vertex);
  face->vertex->data = (void *) vertex;
  face->n_vertices++;

  face = faces[vertices[2]];
  face->vertex = gamgi_engine_slist_add_start (face->vertex);
  face->vertex->data = (void *) vertex;
  face->n_vertices++;

  vertices_xyz += 3; vertices += 3;
  }
}

static void static_polyhedron_orientate (gamgi_polyhedron *polyhedron, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;

for (slist = polyhedron->face; slist != NULL; slist = slist->next)
  static_face_orientate ((gamgi_face *) slist->data);
}

static void static_neighbour_scan (gamgi_voronoi *voronoi)
{
double *neighbours_xyz;
int *neighbours, *shift;
int bad = 0;
int n_neighbours, i;

/*********************************************
 * initialise number of neighbours to update *
 *********************************************/

voronoi->update[1] = 0;

neighbours_xyz = voronoi->neighbours_xyz;
neighbours = voronoi->neighbours;
shift = voronoi->shift;

n_neighbours = neighbours[1];
neighbours += 2;

for (i = 0; i < n_neighbours; i++)
  {
  /******************************************************
   * bad neighbour (vertices reached 0): increase shift *
   ******************************************************/
  
  if (neighbours[0] == 0) bad++;
  else
    {
    /***************************************************************
     * touched neighbour (vertices decreased): add to update array *
     ***************************************************************/

    if (neighbours[0] < neighbours[1])
      static_update_add (i - bad, voronoi);

    /******************************************
     * update data and shift neighbour arrays *
     ******************************************/

    shift[0] = bad;
    neighbours[1] = neighbours[0];
    memmove (neighbours_xyz - 3 * bad, neighbours_xyz, 3 * sizeof (double));
    memmove (neighbours - 4 * bad, neighbours, 4 * sizeof (int));
    }

  neighbours_xyz += 3; neighbours += 4; shift++;
  }

/******************************************************
 * update vertex references to its shifted neighbours *
 ******************************************************/

static_vertex_shift (voronoi);

/*************************************
 * update total number of neighbours *
 *************************************/

voronoi->neighbours[1] -= bad;
}

static int static_vertex_scan (double *position, gamgi_voronoi *voronoi)
{
double *vertices_xyz;
int *vertices, *neighbours;
int bad = 0;
int n_vertices, i;

vertices_xyz = voronoi->vertices_xyz;
vertices = voronoi->vertices;
neighbours = voronoi->neighbours;

n_vertices = vertices[1];
vertices += 2;
for (i = 0; i < n_vertices; i++)
  {
  if (static_face_hide (position, vertices_xyz, voronoi->apex) == FALSE)
    {
    /************************************
     * good vertex: shift vertex arrays *
     ************************************/

    memmove (vertices_xyz - 3 * bad, vertices_xyz, 3 * sizeof (double));
    memmove (vertices - 3 * bad, vertices, 3 * sizeof (int));
    }
  else
    {
    /*************************************************************
     * bad vertex: unref the 3 neighbours; increase vertex shift *
     *************************************************************/

    neighbours[vertices[0] * 4 + 2]--;
    neighbours[vertices[1] * 4 + 2]--;
    neighbours[vertices[2] * 4 + 2]--;
    bad++;
    }
  
  vertices_xyz += 3; vertices += 3;
  }

/*******************************
 * decrease number of vertices *
 *******************************/

voronoi->vertices[1] -= bad;

/**********************************************
 * return number of bad vertices (to see if   *
 * it is necessary to rebuild the polyhedron) *
 **********************************************/

return bad;
}

static void static_neighbour_try (double *position0, double radius0, 
int mark, double *position1, double radius1, int local, int global, 
gamgi_voronoi *voronoi)
{
double d2, r2;
double scale;

d2 = position1[0] * position1[0] + position1[1] *
position1[1] + position1[2] * position1[2];

r2 = voronoi->variancy;
r2 *= radius0 - radius1;

/**************************
 * neighbour is too close *
 **************************/

if (d2 - r2 < GAMGI_MATH_TOLERANCE_LENGTH)
  { voronoi->valid = FALSE; return; }

if (d2 + r2 < GAMGI_MATH_TOLERANCE_LENGTH)
  { voronoi->valid = FALSE; return; }

/*********************************************************
 * radical tesselation: the tangent distance from the    *
 * dividing plane to the spheres is equal for both seeds *
 *********************************************************/

scale = 1.0 + r2 / d2;
gamgi_math_vector_scale (position1, position1, scale);

/************************
 * neighbour is too far *
 ************************/

if (d2 * scale * scale > voronoi->size) return; 

/********************************************
 * face does not hide any polyhedron vertex *
 ********************************************/

if (static_vertex_scan (position1, voronoi) == 0) return;

/****************************************
 * face hides some polyhedron vertices: *
 * 1) update polyhedron faces           *
 * 2) add new faces to polyhedron       *
 * 2) add new faces to update           *
 * 3) rebuild polyhedron                *
 ****************************************/

static_neighbour_scan (voronoi);
static_neighbour_add (position1[0], position1[1], 
position1[2], local, global, voronoi);
static_update_add (voronoi->neighbours[1] - 1, voronoi);
static_polyhedron_update (voronoi);
static_size_update (voronoi);
}

static void static_neighbour_raw (gamgi_slist *slist, double *position0, 
double *position1, double *radius1, int *local, int *global)
{
gamgi_atom *atom;

/**********************
 * generate neighbour *
 **********************/

atom = GAMGI_CAST_ATOM slist->data;
position1[0] = atom->position[0] - position0[0];
position1[1] = atom->position[1] - position0[1];
position1[2] = atom->position[2] - position0[2];
*radius1 = atom->radius * atom->radius;
*local = atom->mark;

/**********************************************************
 * for raw, global is always 0, the real, central, system *
 **********************************************************/

*global = 0;
}

static void static_neighbour_pbc (gamgi_slist *slist, double *shift,
double *position0, double *position1, double *radius1, int *local, int *global)
{
gamgi_atom *atom;
int id_x, id_y, id_z;

/**********************
 * generate neighbour *
 **********************/

atom = GAMGI_CAST_ATOM slist->data;
position1[0] = atom->position[0] + shift[0] - position0[0];
position1[1] = atom->position[1] + shift[1] - position0[1];
position1[2] = atom->position[2] + shift[2] - position0[2];
*radius1 = atom->radius * atom->radius;
*local = atom->mark;

/*********************************************************************
 * for pbc, global identifies the pbc virtual system, from -13 to 13 *
 *********************************************************************/

id_x = 0;
if (shift[0] < 0) id_x = -1;
if (shift[0] > 0) id_x = 1;

id_y = 0;
if (shift[1] < 0) id_y = -1;
if (shift[1] > 0) id_y = 1;

id_z = 0;
if (shift[2] < 0) id_z = -1;
if (shift[2] > 0) id_z = 1;

*global = 9 * id_z + 3 * id_y + id_x;
}

static void static_build_central (gamgi_slist *start, gamgi_slist *seed,
double *position0, double radius0, int mark, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1;
int local, global;

for (slist = start; slist != seed; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }

for (slist = seed->next; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x0 (int cell_x, int cell_y, int cell_z, 
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, j;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -x faces *
 ********************************************/

size = voronoi->size;
if (borders[0] > size) return;

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

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_x1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, j;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +x faces *
 ********************************************/

size = voronoi->size;
if (borders[1] > size) return;

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

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -y faces *
 ********************************************/

size = voronoi->size;
if (borders[2] > size) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +y faces *
 ********************************************/

size = voronoi->size;
if (borders[3] > size) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -z faces *
 ********************************************/

size = voronoi->size;
if (borders[4] > size) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +z faces *
 ********************************************/

size = voronoi->size;
if (borders[5] > size) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  { 
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < 0 || n_x > n - 1) continue;

    index = n_z * n * n + n_y * n + n_x; 
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_raw (slist, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_raw_x0y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0y0 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_raw_x1y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1y0 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_raw_x0y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0y1 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_raw_x1y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1y1 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < 0 || n_z > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }

}

static void static_raw_x0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_raw_x1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_raw_x0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_raw_x1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < 0 || n_y > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }

}

static void static_raw_y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y0z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_raw_y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y1z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_raw_y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y0z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_raw_y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y1z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < 0 || n_x > n - 1) continue;

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_raw (slist, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_raw_x0y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y0z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}
  
static void static_raw_x1y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y0z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x0y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y1z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x1y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y1z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = -shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x0y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y0z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x1y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y0z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = -shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x0y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y1z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_raw_x1y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y1z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < 0 || n_x > n - 1) return;

n_y = shell + cell_y;
if (n_y < 0 || n_y > n - 1) return;

n_z = shell + cell_z;
if (n_z < 0 || n_z > n - 1) return;

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_raw (slist, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, j;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -x faces *
 ********************************************/

size = voronoi->size;
if (borders[0] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_y = j + cell_y;
    if (n_y < -n || n_y > 2*n - 1) continue;

    static_pbc (n, width[1], &n_y, &shift[1]);

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_x1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, j;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +x faces *
 ********************************************/

size = voronoi->size;
if (borders[1] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  for (j = -shell + 1; j <= shell - 1; j++)
    {
    n_y = j + cell_y;
    if (n_y < -n || n_y > 2*n - 1) continue;

    static_pbc (n, width[1], &n_y, &shift[1]);

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -y faces *
 ********************************************/

size = voronoi->size;
if (borders[2] > size) return;

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < -n || n_x > 2*n - 1) continue;

    static_pbc (n, width[0], &n_x, &shift[0]);

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +y faces *
 ********************************************/

size = voronoi->size;
if (borders[3] > size) return;

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < -n || n_x > 2*n - 1) continue;

    static_pbc (n, width[0], &n_x, &shift[0]);

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in -z faces *
 ********************************************/

size = voronoi->size;
if (borders[4] > size) return;

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < -n || n_x > 2*n - 1) continue;

    static_pbc (n, width[0], &n_x, &shift[0]);

    index = n_z * n * n + n_y * n + n_x;
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j, k;

n = voronoi->n_cells;

/********************************************
 * generate neighbours in cells in +z faces *
 ********************************************/

size = voronoi->size;
if (borders[5] > size) return;

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  { 
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  for (k = -shell + 1; k <= shell - 1; k++)
    {
    n_x = k + cell_x;
    if (n_x < -n || n_x > 2*n - 1) continue;

    static_pbc (n, width[0], &n_x, &shift[0]);

    index = n_z * n * n + n_y * n + n_x; 
    for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
      {
      static_neighbour_pbc (slist, shift, position0, 
      position1, &radius1, &local, &global);
      static_neighbour_try (position0, radius0, mark, 
      position1, radius1, local, global, voronoi);
      }
    }
  }
}

static void static_pbc_x0y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0y0 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_pbc_x1y0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1y0 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_pbc_x0y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0y1 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_pbc_x1y1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int i;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1y1 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

for (i = -shell + 1; i <= shell - 1; i++)
  {
  n_z = i + cell_z;
  if (n_z < -n || n_z > 2*n - 1) continue;

  static_pbc (n, width[2], &n_z, &shift[2]);

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_pbc_x0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  index = n_z * n * n + n_y * n + n_x;
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }
  }
}

static void static_pbc_x1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_pbc_x0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x0z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_pbc_x1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int j;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along x1z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (j = -shell + 1; j <= shell - 1; j++)
  {
  n_y = j + cell_y;
  if (n_y < -n || n_y > 2*n - 1) continue;

  static_pbc (n, width[1], &n_y, &shift[1]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_pbc_y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y0z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < -n || n_x > 2*n - 1) continue;

  static_pbc (n, width[0], &n_x, &shift[0]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    } 
  }
}

static void static_pbc_y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y1z0 edges *
 *************************************************/

size = voronoi->size;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < -n || n_x > 2*n - 1) continue;

  static_pbc (n, width[0], &n_x, &shift[0]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_pbc_y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y0z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < -n || n_x > 2*n - 1) continue;

  static_pbc (n, width[0], &n_x, &shift[0]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_pbc_y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;
int k;

n = voronoi->n_cells;

/*************************************************
 * generate neighbours in cells along y1z1 edges *
 *************************************************/

size = voronoi->size;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

for (k = -shell + 1; k <= shell - 1; k++)
  {
  n_x = k + cell_x;
  if (n_x < -n || n_x > 2*n - 1) continue;

  static_pbc (n, width[0], &n_x, &shift[0]);

  index = n_z * n * n + n_y * n + n_x; 
  for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
    {
    static_neighbour_pbc (slist, shift, position0, 
    position1, &radius1, &local, &global);
    static_neighbour_try (position0, radius0, mark, 
    position1, radius1, local, global, voronoi);
    }  
  }
}

static void static_pbc_x0y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y0z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}
  
static void static_pbc_x1y0z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y0z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x0y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y1z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x1y1z0 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y1z0 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;
if (borders[4] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = -shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x0y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y0z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x1y0z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y0z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[2] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = -shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x0y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x0y1z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[0] > size) return;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_x = -shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_pbc_x1y1z1 (int cell_x, int cell_y, int cell_z,
double *position0, double radius0, int mark, int shell, 
double *borders, double *width, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
double position1[3], shift[3];
double radius1, size;
int n, n_x, n_y, n_z;
int local, global;
int index;

n = voronoi->n_cells;

/******************************************************
 * generate neighbours in cells along x1y1z1 vertices *
 ******************************************************/

size = voronoi->size;
if (borders[1] > size) return;
if (borders[3] > size) return;
if (borders[5] > size) return;

n_x = shell + cell_x;
if (n_x < -n || n_x > 2*n - 1) return;

static_pbc (n, width[0], &n_x, &shift[0]);

n_y = shell + cell_y;
if (n_y < -n || n_y > 2*n - 1) return;

static_pbc (n, width[1], &n_y, &shift[1]);

n_z = shell + cell_z;
if (n_z < -n || n_z > 2*n - 1) return;

static_pbc (n, width[2], &n_z, &shift[2]);

index = n_z * n * n + n_y * n + n_x;
for (slist = voronoi->cells[index]; slist != NULL; slist = slist->next)
  {
  static_neighbour_pbc (slist, shift, position0, 
  position1, &radius1, &local, &global);
  static_neighbour_try (position0, radius0, mark, 
  position1, radius1, local, global, voronoi);
  }
}

static void static_build_polyhedron_raw (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_slist *seed, gamgi_voronoi *voronoi)
{
gamgi_atom *atom;
gamgi_polyhedron *polyhedron;
double borders[6];
double position[3];
double origin[3], cell[3];
double radius, r2;
double size, offset;
int shell, shell_max;
int min, max;
int mark;
int n;

/****************************************
 * for the non-periodic system:         *
 * 1) number of cells on each dimension *
 * 2) distance to outer planes          *
 * 3) origin in absolute coordinates    *
 * 4) cell width for each dimension     *
 ****************************************/

n = voronoi->n_cells;
offset = voronoi->offset;

origin[0] = voronoi->min_x;
origin[1] = voronoi->min_y;
origin[2] = voronoi->min_z;

cell[0] = (voronoi->max_x - voronoi->min_x) / n;
cell[1] = (voronoi->max_y - voronoi->min_y) / n;
cell[2] = (voronoi->max_z - voronoi->min_z) / n;

/*****************
 * generate seed *
 *****************/

atom = GAMGI_CAST_ATOM seed->data;
position[0] = atom->position[0];
position[1] = atom->position[1];
position[2] = atom->position[2];
radius = atom->radius * atom->radius;
mark = atom->mark;

/************************************************
 * initialize polyhedron with background planes *
 ************************************************/

borders[0] = -2 * (position[0] - voronoi->min_x + offset);
borders[1] = +2 * (voronoi->max_x - position[0] + offset);
borders[2] = -2 * (position[1] - voronoi->min_y + offset);
borders[3] = +2 * (voronoi->max_y - position[1] + offset);
borders[4] = -2 * (position[2] - voronoi->min_z + offset);
borders[5] = +2 * (voronoi->max_z - position[2] + offset);
static_polyhedron_add (borders, voronoi);
static_size_update (voronoi);

r2 = voronoi->variancy;
r2 *= radius - voronoi->radius;

/****************************************
 * generate neighbours in the seed cell *
 ****************************************/

static_build_central (start, seed, position, radius, mark, voronoi);

/*******************************************************************
 *       generate neighbours in the outer shell cells              *
 *                                                                 *
 * shell_max is the last shell with cells covering the neighbours  *
 *                                                                 *
 * as smaller is min or larger is max, less centered is the seed   *
 * and more shells are required to cover completely the neighbours *
 *******************************************************************/
   
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
  /*********************************************************
   * determine the closest coordinates that can be         *
   * generated for each shell: when these coordinates      *
   * are larger than the current polyhedron, the remaining *
   * neighbours are too far away to cut the polyhedron     *
   *********************************************************/

  borders[0] = origin[0] + (cell_x - shell + 1) * cell[0] - position[0];
  borders[1] = origin[0] + (cell_x + shell) * cell[0] - position[0];

  borders[2] = origin[1] + (cell_y - shell + 1) * cell[1] - position[1];
  borders[3] = origin[1] + (cell_y + shell) * cell[1] - position[1];

  borders[4] = origin[2] + (cell_z - shell + 1) * cell[2] - position[2];
  borders[5] = origin[2] + (cell_z + shell) * cell[2] - position[2];

  /*********************************************************
   * radical tesselation: the tangent distance from the    *
   * dividing plane to the spheres is equal for both seeds *
   *                                                       *
   * we are comparing the central atom with the largest    *
   * atom so r2 can negative or zero but never positive    *
   *********************************************************/

  borders[0] = static_border (shell, r2, borders[0]);
  borders[1] = static_border (shell, r2, borders[1]);
  borders[2] = static_border (shell, r2, borders[2]);
  borders[3] = static_border (shell, r2, borders[3]);
  borders[4] = static_border (shell, r2, borders[4]);
  borders[5] = static_border (shell, r2, borders[5]);

  size = voronoi->size;
  if (borders[0] > size && borders[1] > size 
  && borders[2] > size && borders[3] > size
  && borders[4] > size && borders[5] > size) break;

  /*********
   * faces *
   *********/

  static_raw_x0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  static_raw_y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  static_raw_z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  /*********
   * edges *
   *********/

  static_raw_x0y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x0y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  static_raw_x0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  static_raw_y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);

  /************
   * vertices *
   ************/

  static_raw_x0y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x0y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x0y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x0y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  static_raw_x1y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, voronoi);
  }

polyhedron = static_polyhedron_create (position, 
radius, mark, atom->red, atom->green, atom->blue);
static_polyhedron_copy (polyhedron, voronoi);
static_polyhedron_orientate (polyhedron, voronoi);
voronoi->polyhedron[atom->mark] = polyhedron;
}

static void static_build_polyhedron_pbc (int cell_x, int cell_y, int cell_z, 
gamgi_slist *start, gamgi_slist *seed, gamgi_voronoi *voronoi)
{
gamgi_atom *atom;
gamgi_polyhedron *polyhedron;
double borders[6];
double position[3];
double origin[3], cell[3], width[3];
double radius, r2;
double size, offset;
int shell, shell_max;
int min, max;
int mark;
int n;

/****************************************
 * for the real system (not considering *
 * the PBC 26 virtual systems around):  *
 * - number of cells on each dimension  *
 * - origin in absolute coordinates     *
 * - total width for each dimension     *
 ****************************************/

n = voronoi->n_cells;
offset = voronoi->offset;

origin[0] = voronoi->min_x;
origin[1] = voronoi->min_y;
origin[2] = voronoi->min_z;

cell[0] = (voronoi->max_x - voronoi->min_x) / n;
cell[1] = (voronoi->max_y - voronoi->min_y) / n;
cell[2] = (voronoi->max_z - voronoi->min_z) / n;

width[0] = voronoi->max_x - voronoi->min_x + offset;
width[1] = voronoi->max_y - voronoi->min_y + offset;
width[2] = voronoi->max_z - voronoi->min_z + offset;

/*****************
 * generate seed *
 *****************/

atom = GAMGI_CAST_ATOM seed->data;
position[0] = atom->position[0];
position[1] = atom->position[1];
position[2] = atom->position[2];
radius = atom->radius * atom->radius;
mark = atom->mark;

/************************************************
 * initialize polyhedrum with background planes *
 ************************************************/

borders[0] = -2 * (position[0] - voronoi->min_x + width[0]);
borders[1] = +2 * (voronoi->max_x - position[0] + width[0]);
borders[2] = -2 * (position[1] - voronoi->min_y + width[1]);
borders[3] = +2 * (voronoi->max_y - position[1] + width[1]);
borders[4] = -2 * (position[2] - voronoi->min_z + width[2]);
borders[5] = +2 * (voronoi->max_z - position[2] + width[2]);
static_polyhedron_add (borders, voronoi);
static_size_update (voronoi);

r2 = voronoi->variancy;
r2 *= radius - voronoi->radius;

/****************************************
 * generate neighbours in the seed cell *
 ****************************************/

static_build_central (start, seed, position, radius, mark, voronoi);

/*******************************************************************
 *       generate neighbours in the outer shell cells              *
 *                                                                 *
 * shell_max is the last shell with cells covering the neighbours  *
 *                                                                 *
 * as smaller is min or larger is max, less centered is the seed   *
 * and more shells are required to cover completely the neighbours *
 *******************************************************************/
   
min = cell_x;
if (cell_y < min) min = cell_y;
if (cell_z < min) min = cell_z;

max = cell_x;
if (cell_y > max) max = cell_y;
if (cell_z > max) max = cell_z;

shell_max = max + n;
if (min < n - 1 - max) shell_max = 2*n - 1 - min;
for (shell = 1; shell <= shell_max; shell++)
  {
  /*********************************************************
   * determine the closest coordinates that can be         *
   * generated for each shell: when these coordinates      *
   * are larger than the current polyhedron, the remaining *
   * neighbours are too far away to cut the polyhedron     *
   *                                                       *
   * when periodic boundary conditions are applied, the    *
   * new position is further shifted by offset, so this    *
   * can be taken into account to speed up the calculation *
   *********************************************************/

  borders[0] = origin[0] + (cell_x - shell + 1) * cell[0] - position[0];
  if (cell_x - shell < 0) borders[0] -= offset;
  borders[1] = origin[0] + (cell_x + shell) * cell[0] - position[0];
  if (cell_x + shell > n - 1) borders[1] += offset;

  borders[2] = origin[1] + (cell_y - shell + 1) * cell[1] - position[1];
  if (cell_y - shell < 0) borders[2] -= offset;
  borders[3] = origin[1] + (cell_y + shell) * cell[1] - position[1];
  if (cell_y + shell > n - 1) borders[3] += offset;

  borders[4] = origin[2] + (cell_z - shell + 1) * cell[2] - position[2];
  if (cell_z - shell < 0) borders[4] -= offset;
  borders[5] = origin[2] + (cell_z + shell) * cell[2] - position[2];
  if (cell_z + shell > n - 1) borders[5] += offset;

  /*********************************************************
   * radical tesselation: the tangent distance from the    *
   * dividing plane to the spheres is equal for both seeds *
   *********************************************************/

  borders[0] = static_border (shell, r2, borders[0]);
  borders[1] = static_border (shell, r2, borders[1]);
  borders[2] = static_border (shell, r2, borders[2]);
  borders[3] = static_border (shell, r2, borders[3]);
  borders[4] = static_border (shell, r2, borders[4]);
  borders[5] = static_border (shell, r2, borders[5]);

  size = voronoi->size;
  if (borders[0] > size && borders[1] > size
  && borders[2] > size && borders[3] > size
  && borders[4] > size && borders[5] > size) break;

  /*********
   * faces *
   *********/

  static_pbc_x0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  static_pbc_y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  static_pbc_z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  /*********
   * edges *
   *********/

  static_pbc_x0y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x0y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  static_pbc_x0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  static_pbc_y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);

  /************
   * vertices *
   ************/

  static_pbc_x0y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y0z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x0y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y1z0 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x0y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y0z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x0y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  static_pbc_x1y1z1 (cell_x, cell_y, cell_z, 
  position, radius, mark, shell, borders, width, voronoi);
  }

polyhedron = static_polyhedron_create (position, 
radius, mark, atom->red, atom->green, atom->blue);
static_polyhedron_copy (polyhedron, voronoi);
static_polyhedron_orientate (polyhedron, voronoi);
voronoi->polyhedron[atom->mark] = polyhedron;
}

static void static_vertex_list (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist **cells;
gamgi_slist *slist;
double min_x, max_x;
double min_y, max_y;
double min_z, max_z; 
double width_x, width_y, width_z;
double aux;
int cell_x, cell_y, cell_z;
int n_seeds;
int n, offset;
int i;

n_seeds = voronoi->n_seeds;

/****************************************
 * determine vertex number and position *
 ****************************************/

min_x = DBL_MAX; max_x = -DBL_MAX;
min_y = DBL_MAX; max_y = -DBL_MAX;
min_z = DBL_MAX; max_z = -DBL_MAX;

n = 0;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist = polyhedron->vertex; slist != NULL; slist = slist->next)
    {
    /************************************************
     * convert from relative to absolute oordinates *
     *                                              *
     *   find minimum and maximum cell boundaries   *
     ************************************************/
     
    vertex = (gamgi_vertex *) slist->data;
    gamgi_math_vector_add (vertex->position, 
    polyhedron->position, vertex->position);

    if (vertex->position[0] < min_x) min_x = vertex->position[0];
    if (vertex->position[0] > max_x) max_x = vertex->position[0];

    if (vertex->position[1] < min_y) min_y = vertex->position[1];
    if (vertex->position[1] > max_y) max_y = vertex->position[1];

    if (vertex->position[2] < min_z) min_z = vertex->position[2];
    if (vertex->position[2] > max_z) max_z = vertex->position[2];
    n++;
    }
  }

voronoi->min_x = min_x;
voronoi->max_x = max_x;
voronoi->min_y = min_y;
voronoi->max_y = max_y;
voronoi->min_z = min_z;
voronoi->max_z = max_z;

/*************************************************
 * determine number of cells and cell dimensions *
 *                                               *
 * allocate array of cells, each one containing  * 
 * a list of slist nodes pointing to vertices    *
 *************************************************/

if (modf (pow (n, 1/3.0), &aux) > 0.5) aux++;
voronoi->n_cells = n = aux;

cells = (gamgi_slist **) calloc (pow (n, 3), sizeof (gamgi_slist *));
voronoi->cells = cells;

/****************************************************
 * determine cell dimensions: the set of vertices   *
 * must occupy some volume, so there is no need to  *
 * check if width is large enough on each dimension *
 ****************************************************/

width_x = (max_x - min_x) / n;
width_y = (max_y - min_y) / n;
width_z = (max_z - min_z) / n;

/**********************************************
 * 1) find cell where each vertex belongs     *
 * 2) add vertex to linked list for that cell *
 **********************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist = polyhedron->vertex; slist != NULL; slist = slist->next)
    {
    vertex = (gamgi_vertex *) slist->data;

    cell_x = floor ((vertex->position[0] - min_x) / width_x);
    if (cell_x == n) cell_x--;
    cell_y = floor ((vertex->position[1] - min_y) / width_y);
    if (cell_y == n) cell_y--;
    cell_z = floor ((vertex->position[2] - min_z) / width_z);
    if (cell_z == n) cell_z--;

    offset = cell_z * n * n + cell_y * n + cell_x;
    cells[offset] = gamgi_engine_slist_add_start (cells[offset]);
    cells[offset]->data = vertex;
    }
  }
}

static void static_edge_list (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_edge *edge;
gamgi_slist **cells;
gamgi_slist *slist_f, *slist_e;
double min_x, max_x;
double min_y, max_y;
double min_z, max_z; 
double width_x, width_y, width_z;
double aux;
int cell_x, cell_y, cell_z;
int n_seeds;
int n, offset;
int i;

n_seeds = voronoi->n_seeds;

/**************************************
 * determine edge number and position *
 **************************************/

min_x = DBL_MAX; max_x = -DBL_MAX;
min_y = DBL_MAX; max_y = -DBL_MAX;
min_z = DBL_MAX; max_z = -DBL_MAX;

n = 0;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    for (slist_e = face->edge; slist_e != NULL; slist_e = slist_e->next)
      {
      edge = (gamgi_edge *) slist_e->data;

      if (edge->position[0] < min_x) min_x = edge->position[0];
      if (edge->position[0] > max_x) max_x = edge->position[0];

      if (edge->position[1] < min_y) min_y = edge->position[1];
      if (edge->position[1] > max_y) max_y = edge->position[1];

      if (edge->position[2] < min_z) min_z = edge->position[2];
      if (edge->position[2] > max_z) max_z = edge->position[2];
      n++;
      }
    }
  }

voronoi->min_x = min_x;
voronoi->max_x = max_x;
voronoi->min_y = min_y;
voronoi->max_y = max_y;
voronoi->min_z = min_z;
voronoi->max_z = max_z;

/*************************************************
 * determine number of cells and cell dimensions *
 *                                               *
 * allocate array of cells, each one containing  *
 * a list of slist nodes pointing to edges       *
 *************************************************/

if (modf (pow (n, 1/3.0), &aux) > 0.5) aux++;
voronoi->n_cells = n = aux;

cells = (gamgi_slist **) calloc (pow (n, 3), sizeof (gamgi_slist *));
voronoi->cells = cells;

/****************************************************
 * determine cell dimensions: the set of edges      *
 * must occupy some volume, so there is no need to  *
 * check if width is large enough on each dimension *
 ****************************************************/

width_x = (max_x - min_x) / n;
width_y = (max_y - min_y) / n;
width_z = (max_z - min_z) / n;

/********************************************
 * 1) find cell where each edge belongs     *
 * 2) add edge to linked list for that cell *
 ********************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    for (slist_e = face->edge; slist_e != NULL; slist_e = slist_e->next)
      {
      /********************************************
       * find minimum and maximum cell boundaries *
       ********************************************/

      edge = (gamgi_edge *) slist_e->data;

      cell_x = floor ((edge->position[0] - min_x) / width_x);
      if (cell_x == n) cell_x--;
      cell_y = floor ((edge->position[1] - min_y) / width_y);
      if (cell_y == n) cell_y--;
      cell_z = floor ((edge->position[2] - min_z) / width_z);
      if (cell_z == n) cell_z--;

      offset = cell_z * n * n + cell_y * n + cell_x;
      cells[offset] = gamgi_engine_slist_add_start (cells[offset]);
      cells[offset]->data = edge;
      }
    }
  }

}

static void static_neighbour_list (gamgi_voronoi *voronoi)
{
gamgi_atom *atom;
gamgi_slist **cells;
gamgi_dlist *dlist;
double min_x, max_x;
double min_y, max_y;
double min_z, max_z; 
double width_x, width_y, width_z;
double radius, aux;
int cell_x, cell_y, cell_z;
int offset;
int n;

/**************************************************
 * determine: 1) system dimensions; 2) number of  *
 * atoms; 3) maximum radius (radical tesselation) *
 **************************************************/

min_x = DBL_MAX; max_x = -DBL_MAX;
min_y = DBL_MAX; max_y = -DBL_MAX;
min_z = DBL_MAX; max_z = -DBL_MAX;
radius = 0.0;

n = 0;
for (dlist = voronoi->dlist; dlist != NULL; dlist = dlist->next)
  {
  /*****************************************************
   * give a different, sequential, number to each atom *
   *                                                   *
   *     find minimum and maximum cell boundaries      *
   *                                                   *
   *     find maximum radius (radical tesselation)     *
   *****************************************************/

  atom = GAMGI_CAST_ATOM dlist->data;
  atom->mark = n++;

  if (atom->position[0] < min_x) min_x = atom->position[0];
  if (atom->position[0] > max_x) max_x = atom->position[0];

  if (atom->position[1] < min_y) min_y = atom->position[1];
  if (atom->position[1] > max_y) max_y = atom->position[1];

  if (atom->position[2] < min_z) min_z = atom->position[2];
  if (atom->position[2] > max_z) max_z = atom->position[2];

  if (atom->radius > radius) radius = atom->radius;
  }
voronoi->n_seeds = n;
voronoi->radius = radius * radius;

/*******************************
 * create array of polyhedrons *
 *******************************/

voronoi->polyhedron = (gamgi_polyhedron **) calloc (n, sizeof (gamgi_polyhedron *));

/************************************************************
 * determine number of cells on each direction:             *
 * on average, each cell has approximately one atom.        *
 *                                                          *
 * allocate array of cells:                                 *
 * each cell points to a linked list of atoms in that cell. *
 ************************************************************/

if (modf (pow (n, 1/3.0), &aux) > 0.5) aux++;
voronoi->n_cells = n = aux;

cells = (gamgi_slist **) calloc (pow (n, 3), sizeof (gamgi_slist *));
voronoi->cells = cells;

/***********************************************************
 * determine cell dimensions: when the width is too small  *
 * (for example when the atoms have the same z coordinate) *
 * we need to enlarge the cell dimensions symmetricaly, to *
 * guarantee that the distance from the seeds at old min,  *
 * max coordinates to the new border planes is the same    *
 ***********************************************************/

if (max_x - min_x < GAMGI_PHYS_VORONOI_WIDTH)
  {
  aux = min_x + max_x;
  min_x = (aux - GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  max_x = (aux + GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  }
width_x = max_x - min_x;
width_x /= n;

if (max_y - min_y < GAMGI_PHYS_VORONOI_WIDTH)
  {
  aux = min_y + max_y;
  min_y = (aux - GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  max_y = (aux + GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  }
width_y = max_y - min_y;
width_y /= n;

if (max_z - min_z < GAMGI_PHYS_VORONOI_WIDTH)
  {
  aux = min_z + max_z;
  min_z = (aux - GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  max_z = (aux + GAMGI_PHYS_VORONOI_WIDTH) / 2.0;
  }
width_z = max_z - min_z;
width_z /= n;

voronoi->min_x = min_x;
voronoi->max_x = max_x;
voronoi->min_y = min_y;
voronoi->max_y = max_y;
voronoi->min_z = min_z;
voronoi->max_z = max_z;

/********************************************
 * 1) find cell where each atom belongs     *
 * 2) add atom to linked list for that cell *
 ********************************************/

for (dlist = voronoi->dlist; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data; 

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

/*******************************************************
 * total volume, calculated from the 6 outer planes:   *
 * with planar boundaries, offset means distance       *
 * to outer planes; with periodic boundaries, offset   *
 * means distance to outer systems (so outer planes    *
 * will be at half distance).                          *
 *                                                     *
 * To calculate volume, offset must be applied on both *
 * sides. Therefore, with planar boundaries, the total *
 * length increase is offset + offset, while with      *
 * periodic boundaries is offset/2.0 + offset/2.0      *
 *******************************************************/

aux = voronoi->offset;
if (voronoi->periodic == FALSE) aux *= 2.0;

voronoi->volume[0] = 
(voronoi->max_x - voronoi->min_x + aux) * 
(voronoi->max_y - voronoi->min_y + aux) * 
(voronoi->max_z - voronoi->min_z + aux);
}

static void static_unlist (gamgi_voronoi *voronoi)
{
gamgi_slist **cells;
gamgi_slist *slist;
int n_cells;
int cell_x, cell_y, cell_z;
int offset;

/********************************************
 * remove lists of nodes, one for each cell *
 ********************************************/

cells = voronoi->cells;
n_cells = voronoi->n_cells;
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

static void static_cell (double position, double min,
double width, int n_cells, int *cell)
{
double integral, fraction;

/************************************************
 * When the coordinate equals the right border, *
 * the correct cell is the last one, on the     *
 * left. In this case shift is necessarily 0.0. *
 *                                              *
 * when position is on the half left of the     *
 * cell, the correct cell is the left cell.     *
 *                                              *
 * we only need to try 8 cells, assuming that   *
 * the vertices must be closer than half the    *
 * size of a cell to be considered the same.    *
 ************************************************/

fraction = modf ((position - min) / width, &integral);
if (integral == n_cells || fraction < 0.5) integral--;
*cell = integral;
}

static void static_vertex_equal (gamgi_vertex *vertex0,
gamgi_vertex *vertex1, double tolerance)
{
double sub[3];

/***********************************
 * check if vertices are too close *
 ***********************************/

gamgi_math_vector_sub (vertex0->position, vertex1->position, sub);
if (gamgi_math_vector_length (sub) > tolerance) return;

/******************************************************************
 * colapse one vertex to the other: we must compare all vertices  *
 * (even if they already colapsed) and we should not change their *
 * coordinates (no average values), to guarantee that the results *
 * are independent of the order chosen to compare the vertices    *
 *                                                                *
 *    make sure that each vertex does not point to the other!     *
 ******************************************************************/

if (vertex0->vertex != NULL && vertex1->vertex != NULL) return;

if (vertex0->vertex != NULL)
  {
  if (static_vertex (vertex0->vertex) != vertex1)
    vertex1->vertex = vertex0->vertex;
  return;
  }

if (vertex1->vertex != NULL)
  {
  if (static_vertex (vertex1->vertex) != vertex0)
    vertex0->vertex = vertex1->vertex;
  return;
  }

vertex1->vertex = vertex0;
}

static void static_vertex_near (gamgi_vertex *vertex, int index,
int cell_x, int cell_y, int cell_z, gamgi_voronoi *voronoi)
{
gamgi_vertex *vertex_new;
gamgi_slist **cells;
gamgi_slist *slist;
int n, index_new;

cells = voronoi->cells;
n = voronoi->n_cells;

/************************
 * check if cell exists *
 ************************/

if (cell_x < 0 || cell_x > n - 1) return;
if (cell_y < 0 || cell_y > n - 1) return;
if (cell_z < 0 || cell_z > n - 1) return;

/*********************************************
 * 1) check each vertex pair only once.      *
 * 2) vertices in the same cell are compared *
 *    differently, to avoid repetitions.     *
 *********************************************/

index_new = cell_z * n * n + cell_y * n + cell_x;
if (index_new <= index) return;

for (slist = cells[index_new]; slist != NULL; slist = slist->next)
  {
  vertex_new = (gamgi_vertex *) slist->data;
  static_vertex_equal (vertex, vertex_new, voronoi->vertex);
  }
}

static void static_vertex_central (int cell_x, int cell_y, int cell_z,
double min_x, double min_y, double min_z, double width_x, double width_y,
double width_z, gamgi_voronoi *voronoi)
{
gamgi_vertex *vertex, *vertex_new;
gamgi_slist **cells;
gamgi_slist *slist, *slist_new;
int n_x, n_y, n_z;
int n, index;

cells = voronoi->cells;
n = voronoi->n_cells;
index = cell_z * n * n + cell_y * n + cell_x;

for (slist = cells[index]; slist != NULL; slist = slist->next)
  {
  vertex = (gamgi_vertex *) slist->data;

  /******************************************
   * compare with vertices in the same cell *
   ******************************************/

  for (slist_new = slist->next; slist_new != NULL; slist_new = slist_new->next)
    {
    vertex_new = (gamgi_vertex *) slist_new->data;
    static_vertex_equal (vertex, vertex_new, voronoi->vertex);
    }

  /***********************************
   * Determine cell to start search. *
   *                                 *
   * We need to search 8 cells only, *
   * as long as equal vertices are   *
   * closer than half the cell size. *
   *                                 *
   * the central cell (one of the 8  *
   * cells) is not searched again.   *
   ***********************************/

  static_cell (vertex->position[0], min_x, width_x, n, &n_x);
  static_cell (vertex->position[1], min_y, width_y, n, &n_y);
  static_cell (vertex->position[2], min_z, width_z, n, &n_z);

  static_vertex_near (vertex, index, n_x + 0, n_y + 0, n_z + 0, voronoi);
  static_vertex_near (vertex, index, n_x + 0, n_y + 0, n_z + 1, voronoi);
  static_vertex_near (vertex, index, n_x + 0, n_y + 1, n_z + 0, voronoi);
  static_vertex_near (vertex, index, n_x + 0, n_y + 1, n_z + 1, voronoi);
  static_vertex_near (vertex, index, n_x + 1, n_y + 0, n_z + 0, voronoi);
  static_vertex_near (vertex, index, n_x + 1, n_y + 0, n_z + 1, voronoi);
  static_vertex_near (vertex, index, n_x + 1, n_y + 1, n_z + 0, voronoi);
  static_vertex_near (vertex, index, n_x + 1, n_y + 1, n_z + 1, voronoi);
  }

}

/***************** internal function *****************
 *                                                   *
 *                 STATIC_EDGE_LOCAL                 *
 *                                                   *
 * point edges to other edges in the same polyhedron *
 *                                                   *
 *****************************************************/

static void static_edge_local (gamgi_edge *edge0, gamgi_edge *edge1)
{
/*******************************
 * check if edges are the same *
 *******************************/

if (edge0->vertex1 != edge1->vertex1 && edge0->vertex1 != edge1->vertex2) return;
if (edge0->vertex2 != edge1->vertex1 && edge0->vertex2 != edge1->vertex2) return;

/**************************************************************
 * point edges with decreasing order (listed only in faces)   *
 * to edges with increasing order, (listed also in polyhedra) *
 *                                                            *
 *    make sure that each edge does not point to the other!   *
 **************************************************************/

if (edge0->vertex1->order > edge0->vertex2->order && 
edge0->edge == NULL && static_edge (edge1) != edge0)
  {
  /***************************************
   * edge0: redundant, redirect to edge1 *
   * edge1: real, get other face address *
   ***************************************/

  edge0->edge = edge1;
  edge1->face2 = edge0->face1;
  }
else if (edge1->vertex1->order > edge1->vertex2->order &&
edge1->edge == NULL && static_edge (edge0) != edge1)
  {
  /***************************************
   * edge1: redundant, redirect to edge0 *
   * edge0: real, get other face address *
   ***************************************/

  edge1->edge = edge0;
  edge0->face2 = edge1->face1;
  }
}

/***************** internal function *****************
 *                                                   *
 *                 STATIC_EDGE_GLOBAL                *
 *                                                   *
 * point edges to other edges in different polyhedra *
 *                                                   *
 *****************************************************/

static void static_edge_global (gamgi_edge *edge0, gamgi_edge *edge1)
{
/*******************************
 * check if edges are the same *
 *******************************/

if (edge0->vertex1 != edge1->vertex1 && edge0->vertex1 != edge1->vertex2) return;
if (edge0->vertex2 != edge1->vertex1 && edge0->vertex2 != edge1->vertex2) return;

/************************************************************
 * colapse one edge to the other: we must compare all edges *
 *                                                          *
 *  make sure that each edge does not point to the other!   *
 ************************************************************/

if (edge0->edge != NULL && edge1->edge != NULL) return;

if (edge0->edge != NULL)
  {
  if (static_edge (edge0->edge) != edge1)
    edge1->edge = edge0->edge;
  return;
  }

if (edge1->edge != NULL)
  {
  if (static_edge (edge1->edge) != edge0)
    edge0->edge = edge1->edge;
  return;
  }

/**************************************************************
 * point edges with decreasing order (listed only in faces)   *
 * to edges with increasing order, (listed also in polyhedra) *
 **************************************************************/

if (edge0->vertex1->order > edge0->vertex2->order)
  { edge0->edge = edge1; return; }
 
if (edge1->vertex1->order > edge1->vertex2->order)
  { edge1->edge = edge0; return; }

edge1->edge = edge0;
}

static void static_edge_central (int cell_x, int cell_y, int cell_z,
gamgi_voronoi *voronoi)
{
gamgi_edge *edge0, *edge1;
gamgi_slist **cells;
gamgi_slist *slist0, *slist1;
int n, index0;

cells = voronoi->cells;
n = voronoi->n_cells;

/*******************************************
 * first link edges in the same polyhedron *
 *******************************************/

index0 = cell_z * n * n + cell_y * n + cell_x;
for (slist0 = cells[index0]; slist0 != NULL; slist0 = slist0->next)
  {
  edge0 = (gamgi_edge *) slist0->data;

  /***************************************
   * compare only edges in the same cell *
   ***************************************/

  for (slist1 = slist0->next; slist1 != NULL; slist1 = slist1->next)
    {
    edge1 = (gamgi_edge *) slist1->data;

    if (edge0->last == edge1->last)
      static_edge_local (edge0, edge1);
    }
  }

/********************************************
 * then link edges in different polyhedrons *
 ********************************************/

index0 = cell_z * n * n + cell_y * n + cell_x;
for (slist0 = cells[index0]; slist0 != NULL; slist0 = slist0->next)
  {
  edge0 = (gamgi_edge *) slist0->data;

  /***************************************
   * compare only edges in the same cell *
   ***************************************/

  for (slist1 = slist0->next; slist1 != NULL; slist1 = slist1->next)
    {
    edge1 = (gamgi_edge *) slist1->data;

    if (edge0->last != edge1->last)
      static_edge_global (edge0, edge1);
    }
  }

/***************
 * clean edges *
 ***************/

for (slist0 = cells[index0]; slist0 != NULL; slist0 = slist0->next)
  {
  edge0 = (gamgi_edge *) slist0->data;
  edge0->last = NULL;
  }
}

static void static_vertex_order (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist;
int n_seeds, order, i;

order = 0;
n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist = polyhedron->vertex; slist != NULL; slist = slist->next)
    {
    vertex = (gamgi_vertex *) slist->data;
    if (vertex->vertex != NULL) continue;

    vertex->order = order++;
    }
  }
}

static void static_edge_build (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_edge *edge;
gamgi_vertex *vertex, *old, *new;
gamgi_slist *slist_f, *slist_v;
double sub[3], add[3];
double length;
int n_seeds, i;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    slist_v = face->vertex;
    vertex = (gamgi_vertex *) slist_v->data;
    old = static_vertex (vertex);
    for (slist_v = slist_v->next; slist_v != NULL; slist_v = slist_v->next)
      {
      vertex = (gamgi_vertex *) slist_v->data;
      new = static_vertex (vertex);
      if (new != old)
        {
        gamgi_math_vector_sub (new->position, old->position, sub);
        length = gamgi_math_vector_length (sub);
        gamgi_math_vector_add (new->position, old->position, add);
        gamgi_math_vector_scale (add, add, 0.5);

        /****************************************************
         * To determine the angles between faces, each      *
         * polyhedron edge must know the address of the     *
         * two faces defining it. face1 address is saved    *
         * here, face2 address is obtained later when       *
         * comparing face edges in the same polyhedron.     *
         *                                                  *
         * Edges will be compared later, first in the same  *
         * polyhedron and then between different polyhedra, *
         * so each edge must know its polyhedron address.   *
         ****************************************************/

        edge = static_edge_create (add[0], add[1], add[2], length);
        edge->vertex1 = old;
        edge->vertex2 = new;
        edge->face1 = face;
        edge->last = polyhedron;

        /************************************************************
         * edges with decreasing order are listed only in faces     *
         * edges with increasing order are listed also in polyhedra *
         ************************************************************/

        face->edge = gamgi_engine_slist_add_start (face->edge);
        face->edge->data = (void *) edge;

        if (old->order < new->order)
          {
          polyhedron->edge = gamgi_engine_slist_add_start (polyhedron->edge);
          polyhedron->edge->data = (void *) edge;
          }

        }
      old = new;
      }
    slist_v = face->vertex;
    vertex = (gamgi_vertex *) slist_v->data;
    new = static_vertex (vertex);
    if (new != old)
      {
      gamgi_math_vector_sub (new->position, old->position, sub);
      length = gamgi_math_vector_length (sub);
      gamgi_math_vector_add (new->position, old->position, add);
      gamgi_math_vector_scale (add, add, 0.5);

      /****************************************************
       * To determine the angles between faces, each      *
       * polyhedron edge must know the address of the     *
       * two faces defining it. face1 address is saved    *
       * here, face2 address is obtained later when       *
       * comparing face edges in the same polyhedron.     *
       *                                                  *
       * Edges will be compared later, first in the same  *
       * polyhedron and then between different polyhedra, *
       * so each edge must know its polyhedron address.   *
       ****************************************************/

      edge = static_edge_create (add[0], add[1], add[2], length);
      edge->vertex1 = old;
      edge->vertex2 = new;
      edge->face1 = face;
      edge->last = polyhedron;

      /************************************************************
       * edges with decreasing order are listed only in faces     *
       * edges with increasing order are listed also in polyhedra *
       ************************************************************/

      face->edge = gamgi_engine_slist_add_start (face->edge);
      face->edge->data = (void *) edge;

      if (old->order < new->order)
        {
        polyhedron->edge = gamgi_engine_slist_add_start (polyhedron->edge);
        polyhedron->edge->data = (void *) edge;
        }

      }
    }
  }
}

static void static_merge (gamgi_voronoi *voronoi)
{
double min_x, max_x;
double min_y, max_y;
double min_z, max_z;
double width_x, width_y, width_z;
int cell_x, cell_y, cell_z;
int n_cells;

/***********************
 * create vertex cells *
 ***********************/

static_vertex_list (voronoi);

/***************************
 * find redundant vertices *
 ***************************/

n_cells = voronoi->n_cells;
min_x = voronoi->min_x;
max_x = voronoi->max_x;
min_y = voronoi->min_y;
max_y = voronoi->max_y;
min_z = voronoi->min_z;
max_z = voronoi->max_z;

width_x = (max_x - min_x) / n_cells;
width_y = (max_y - min_y) / n_cells;
width_z = (max_z - min_z) / n_cells;

for (cell_z = 0; cell_z < n_cells; cell_z++)
  {
  for (cell_y = 0; cell_y < n_cells; cell_y++)
    {
    for (cell_x = 0; cell_x < n_cells; cell_x++)
      {
      static_vertex_central (cell_x, cell_y, cell_z, min_x,
      min_y, min_z, width_x, width_y, width_z, voronoi);
      }
    }
  }

/***********************
 * remove vertex cells *
 ***********************/

static_unlist (voronoi);

/***************************
 * order the real vertices *
 ***************************/

static_vertex_order (voronoi);

/***************************
 * create polyhedron edges *
 ***************************/

static_edge_build (voronoi);

/*********************
 * create edge cells *
 *********************/

static_edge_list (voronoi);

/************************
 * find redundant edges *
 ************************/

n_cells = voronoi->n_cells;
for (cell_z = 0; cell_z < n_cells; cell_z++)
  {
  for (cell_y = 0; cell_y < n_cells; cell_y++)
    {
    for (cell_x = 0; cell_x < n_cells; cell_x++)
      {
      static_edge_central (cell_x, cell_y, cell_z, voronoi);
      }
    }
  }

/*********************
 * remove edge cells *
 *********************/

static_unlist (voronoi);
}

static void static_build_start (gamgi_voronoi *voronoi)
{
int n;

/*****************************************
 * create cells with lists of neighbours *
 *****************************************/

static_neighbour_list (voronoi);

/********************************************************************************************
 *          Create and initialize arrays used to determine Voronoi polyhedron.              *
 *                                                                                          *
 * The building arrays are: 1) neighbours, to save information about the neighbours         *
 * of the central atom, each one corresponding to a face of the central polyhedron.         *
 * The first and second positions indicate the maximum and current number of neighbours     *
 * referred in the various arrays. For each neighbour, there are then four positions in     *
 * the array, with the following data: i) the number of vertices after a new neighbour      *
 * was attempted; ii) the number of vertices before a new neighbour was attempted; iii)     *
 * the local neighbour identifier, which is a unique positive integer attributed to each    *
 * atom. The six external virtual neighbours that are initially created have negative       *
 * identifiers, from -1 to -6, to distinguish them from real atoms. iv) the global          *
 * neighbour identifier, with values from -13 to +13, which identifies the parallelepiped   *
 * rectangle where the neighbour is. When global is 0, the neighbour is in the real         *
 * parallelepiped rectangle, otherwise the neighbour is in one of the 26 parallelepiped     *
 * rectangles around that are virtual images of the real one. The six external neighbours   * 
 * have global set to 0; 2) neighbours_xyz, to store the coordinates of the neighbours      *
 * currently recognized. When periodic boundaries are applied, the stored coordinates are   *
 * those effectively used to determine the polyhedron faces, not the original,              *
 * untranslated ones; 3) shift, to indicate how much each neighbour segment has been        *
 * shifted to the beginning in the various arrays, after some neighbours have been          *
 * eliminated (this is needed to point vertices to the correct, shifted, neighbours);       *
 * 4) vertices, to save information about the polyhedron vertices (defined by the           *
 * intersection of 3 adjacent faces). The first and second positions indicate the maximum   *
 * and current number of vertices referred in the two vertex arrays. For each vertex,       *
 * there are then three positions in the array, indicating the position (starting from 0)   *
 * of the three neighbours defining the vertex, in the neighbours array; 5) vertices_xyz,   * 
 * to store the coordinates of the vertices currently referred; 6) update, to indicate      *
 * which polyhedron faces need to be recalculated. The first and second positions indicate  *
 * the maximum and current number of faces referred in the array. For each face, there is   *
 * then one position indicating the order of the corresponding neighbour in the neighbours  *
 * array.                                                                                   *
 ********************************************************************************************/

n = 2 + 4 * GAMGI_PHYS_VORONOI_ARRAY; voronoi->neighbours = (int *) malloc (n * sizeof (int));
voronoi->neighbours[0] = GAMGI_PHYS_VORONOI_ARRAY;

n = 3 * GAMGI_PHYS_VORONOI_ARRAY;
voronoi->neighbours_xyz = (double *) malloc (n * sizeof (double));

n = GAMGI_PHYS_VORONOI_ARRAY;
voronoi->shift = (int *) malloc (n * sizeof (int));

n = GAMGI_PHYS_VORONOI_ARRAY;
voronoi->faces = (gamgi_face **) malloc (n * sizeof (gamgi_face *));

n = 2 + 3 * GAMGI_PHYS_VORONOI_ARRAY;
voronoi->vertices = (int *) malloc (n * sizeof (int));
voronoi->vertices[0] = GAMGI_PHYS_VORONOI_ARRAY;

n = 3 * GAMGI_PHYS_VORONOI_ARRAY;
voronoi->vertices_xyz = (double *) malloc (n * sizeof (double));

n = 2 + GAMGI_PHYS_VORONOI_ARRAY;
voronoi->update = (int *) malloc (n * sizeof (int));
voronoi->update[0] = GAMGI_PHYS_VORONOI_ARRAY;
} 

static void static_build_end (gamgi_voronoi *voronoi)
{
/***********************
 * remove build arrays *
 ***********************/

free (voronoi->neighbours);
free (voronoi->neighbours_xyz);
free (voronoi->vertices);
free (voronoi->vertices_xyz);
free (voronoi->faces);
free (voronoi->shift);
free (voronoi->update);

/**************************
 * remove neighbour lists *
 **************************/

static_unlist (voronoi);
free (voronoi->cells);
voronoi->cells = NULL;
}

static gamgi_bool static_build (gamgi_voronoi *voronoi)
{
gamgi_slist **cells;
gamgi_slist *start, *seed;
int cell_x, cell_y, cell_z;
int n_cells, offset;

voronoi->valid = TRUE;

/**********************************************
 * create neighbour cells and building arrays *
 **********************************************/

static_build_start (voronoi);

cells = voronoi->cells;
n_cells = voronoi->n_cells;
for (cell_z = 0; cell_z < n_cells; cell_z++)
  {
  for (cell_y = 0; cell_y < n_cells; cell_y++)
    {
    for (cell_x = 0; cell_x < n_cells; cell_x++)
      {
      offset = cell_z * n_cells * n_cells + cell_y * n_cells + cell_x;
      start = cells[offset];
      for (seed = start; seed != NULL; seed = seed->next)
        {
        /*****************************************************
         * build polyhedron from distance-ordered neighbours *
         *****************************************************/

        if (voronoi->periodic == TRUE)
          static_build_polyhedron_pbc (cell_x, cell_y, cell_z, start, seed, voronoi);
        else
          static_build_polyhedron_raw (cell_x, cell_y, cell_z, start, seed, voronoi);
        }

      }
    }
  }

/**********************************************
 * remove neighbour cells and building arrays *
 **********************************************/

static_build_end (voronoi);

return voronoi->valid;
}

static void static_scan_vertices (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *vertex;
gamgi_slist *slist_v, *slist_f;
int n_seeds, i;

n_seeds = voronoi->n_seeds;

/**************************************************
 * initialize vertex->last, so this function will *
 * work fine when placed in a different sequence  *
 **************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    vertex = static_vertex (vertex);
    vertex->last = NULL;
    }
  }

/**************************
 * mark external vertices *
 **************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    for (slist_v = face->vertex; slist_v != NULL; slist_v = slist_v->next)
      {
      /*******************************************
       * external vertices are the last          *
       * vertices, so vertex->last points to     *
       * the vertex, otherwise it points to NULL *
       *                                         *
       * vertices contacting removed polyhedra   *
       * are also considered external vertices   *
       *******************************************/

      vertex = (gamgi_vertex *) slist_v->data;
      vertex = static_vertex (vertex);
      if (face->local < 0 || face->global != 0 || 
      voronoi->polyhedron[face->local] == NULL)
        vertex->last = vertex;
      }
    }
  }

}

static void static_measure_polyhedrons (int *real,
int *border, int *total, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, i;
gamgi_bool inside;
 
*real = 0;
*border = 0;
*total = 0;

/*******************
 * count polyhedra *
 *******************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  inside = TRUE;
  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;

    /**************************************
     * one external vertex is enough to   *
     * qualify the polyhedron as external *
     **************************************/

    vertex = static_vertex (vertex);
    if (vertex->last != NULL)
      { inside = FALSE; break; }
    }

  if (inside == TRUE) *real += 1;
  else *border += 1;
  }

*total = *real + *border;
}

static void static_measure_faces (int *real,
int *border, int *total, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
int n_seeds, i;

*real = 0;
*border = 0;
*total = 0;

/***************
 * count faces *
 ***************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    if (face->local < 0 || face->global != 0) *border += 1;
    else *real += 1;
    }
  }

*total = *real + *border;
}

static void static_measure_edges (int *real,
int *border, int *total, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_vertex *vertex1, *vertex2;
gamgi_slist *slist_e;
int n_seeds, i;

*real = 0;
*border = 0;
*total = 0;

/***************
 * count edges *
 ***************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;

    /*********************************
     * the edge is external only     *
     * if both vertices are external *
     *********************************/

    if (edge->edge != NULL) continue;
    vertex1 = static_vertex (edge->vertex1);
    vertex2 = static_vertex (edge->vertex2);
    if (vertex1->last == NULL || vertex2->last == NULL) *real += 1;
    else *border += 1;
    }
  }

*total = *real + *border;
}

static void static_measure_vertices (int *real,
int *border, int *total, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, i;

*real = 0;
*border = 0;
*total = 0;

/******************
 * count vertices *
 ******************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;

    /*************************************
     * if vertex->last is NULL then the  *
     * vertex is not the last (external) *
     *************************************/

    if (vertex->vertex != NULL) continue;
    if (vertex->last == NULL) *real += 1;
    else *border += 1;
    }
  }

*total = *real + *border;
}

static void static_measure_polyhedron_faces (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
int n_seeds, n_polyhedrons, n_faces, i;
 
*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

/***************
 * count faces *
 ***************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  n_faces = 0;
  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;
    n_faces++;
    }

  /**************
   * statistics *
   **************/

  if (n_faces < *min) *min = n_faces;
  if (n_faces > *max) *max = n_faces;
  *average += n_faces;
  *deviation += n_faces * n_faces;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_edges (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
int n_seeds, n_polyhedrons, n_edges, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

n_seeds = voronoi->n_seeds;

/*************************************************
 * initialize edge->last, so this function will  *
 * work fine when placed in a different sequence *
 *************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;
    edge = static_edge (edge);
    edge->last = NULL;
    }
  }

/***************
 * count edges *
 ***************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  n_edges = 0;
  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;

    /******************************************************
     * edges in the same polyhedron are counted only once *
     ******************************************************/

    edge = static_edge (edge);
    if (edge->last == polyhedron) continue;

    edge->last = polyhedron;
    n_edges++;
    }

  /**************
   * statistics *
   **************/

  if (n_edges < *min) *min = n_edges;
  if (n_edges > *max) *max = n_edges;
  *average += n_edges;
  *deviation += n_edges * n_edges;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_vertices (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, n_polyhedrons, n_vertices, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

n_seeds = voronoi->n_seeds;

/***************************************************
 * initialize vertex->last, so this function will  *
 * work fine when placed in a different sequence   *
 ***************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    vertex = static_vertex (vertex);
    vertex->last = NULL;
    }
  }

/******************
 * count vertices *
 ******************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  n_vertices = 0;
  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;

    /*********************************************************
     * vertices in the same polyhedron are counted only once *
     *********************************************************/

    vertex = static_vertex (vertex);
    if (vertex->last == polyhedron) continue;

    vertex->last = polyhedron;
    n_vertices++;
    }

  /**************
   * statistics *
   **************/

  if (n_vertices < *min) *min = n_vertices;
  if (n_vertices > *max) *max = n_vertices;
  *average += n_vertices;
  *deviation += n_vertices * n_vertices;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_face_vertices (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *vertex;
gamgi_slist *slist_v, *slist_f;
int n_seeds, n_faces, n_vertices, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_faces = 0;

n_seeds = voronoi->n_seeds;

/***************************************************
 * initialize vertex->last, so this function will  *
 * work fine when placed in a different sequence   *
 ***************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    vertex = static_vertex (vertex);
    vertex->last = NULL;
    }
  }

/******************
 * count vertices *
 ******************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    n_faces++;
    n_vertices = 0;
    for (slist_v = face->vertex; slist_v != NULL; slist_v = slist_v->next)
      {
      vertex = (gamgi_vertex *) slist_v->data;

      /***************************************************
       * vertices in the same face are counted only once *
       ***************************************************/

      vertex = static_vertex (vertex);
      if (vertex->last == face) continue;

      vertex->last = face;
      n_vertices++;
      }

    /**************
     * statistics *
     **************/

    if (n_vertices < *min) *min = n_vertices;
    if (n_vertices > *max) *max = n_vertices;
    *average += n_vertices;
    *deviation += n_vertices * n_vertices;
    }

  }

static_expectancy (average, deviation, n_faces);
}

static void static_scan_edge_polyhedrons (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
int n_seeds, i;

n_seeds = voronoi->n_seeds;

/*************************************************
 * initialize edge->last, so this function will  *
 * work fine when placed in a different sequence *
 *************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;
    edge = static_edge (edge);
    edge->last = NULL;
    }
  }

/************************
 * accumulate polyhedra *
 ************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;

    /******************************************************
     * edges in the same polyhedron are counted only once *
     ******************************************************/

    edge = static_edge (edge);
    if (edge->last == polyhedron) continue;

    edge->last = polyhedron;
    edge->n_polyhedrons++;
    }
  }

}

static void static_measure_edge_polyhedrons (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
int n_seeds, n_edges, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_edges = 0;

/**********************
 * get back polyhedra *
 **********************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;
    if (edge->edge != NULL) continue;

    n_edges++;
    n_polyhedrons = edge->n_polyhedrons;

    /**************
     * statistics *
     **************/

    if (n_polyhedrons < *min) *min = n_polyhedrons;
    if (n_polyhedrons > *max) *max = n_polyhedrons;
    *average += n_polyhedrons;
    *deviation += n_polyhedrons * n_polyhedrons;
    }
  }

static_expectancy (average, deviation, n_edges);
}

static void static_scan_edge_faces (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_edge *edge;
gamgi_slist *slist_e, *slist_f;
int n_seeds, i;

n_seeds = voronoi->n_seeds;

/*************************************************
 * initialize edge->last, so this function will  *
 * work fine when placed in a different sequence *
 *************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    for (slist_e = face->edge; slist_e != NULL; slist_e = slist_e->next)
      {
      edge = (gamgi_edge *) slist_e->data;
      edge->last = NULL;
      }
    }
  }

/********************
 * accumulate faces *
 ********************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    for (slist_e = face->edge; slist_e != NULL; slist_e = slist_e->next)
      {
      edge = (gamgi_edge *) slist_e->data;

      /************************************************
       * edges in the same face are counted only once *
       ************************************************/

      edge = static_edge (edge);
      if (edge->last == face) continue;

      edge->last = face;
      edge->n_faces++;
      }
    }
  }

}

static void static_measure_edge_faces (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_edge *edge;
gamgi_slist *slist_e, *slist_f;
int n_seeds, n_edges, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_edges = 0;

/******************
 * get back faces *
 ******************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    for (slist_e = face->edge; slist_e != NULL; slist_e = slist_e->next)
      {
      edge = (gamgi_edge *) slist_e->data;

      /**************************************************************
       * In badly formed systems, where common faces are different, *
       * edges might not be seen in scan_edge_faces, as inner faces *
       * are scanned only once, and be seen in measure_edge_faces,  *
       * where inner faces are scanned twice, resulting in n_faces  *
       * equal to 0. In well formed systems, either the scanned     *
       * edges point to the zero edges, or the zero edges point to  *
       * the scanned ones, so the faces are never zero. In badly    *
       * formed systems, we can assume that the zero edges would    *
       * point to the scanned ones, so the scanned are counted and  *
       * the zero edges should be ignored.                          *
       ***************************************************************/ 

      if (edge->edge != NULL) continue;
      if (edge->n_faces == 0) continue;

      n_edges++;
      n_faces = edge->n_faces;

      /**************
       * statistics *
       **************/

      if (n_faces < *min) *min = n_faces;
      if (n_faces > *max) *max = n_faces;
      *average += n_faces;
      *deviation += n_faces * n_faces;
      }
    }
  }

static_expectancy (average, deviation, n_edges);
}

static void static_scan_vertex_polyhedrons (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, i;

n_seeds = voronoi->n_seeds;

/**************************************************
 * initialize vertex->last, so this function will *
 * work fine when placed in a different sequence  *
 **************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    vertex = static_vertex (vertex);
    vertex->last = NULL;
    }
  }

/************************
 * accumulate polyhedra *
 ************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;

    /*********************************************************
     * vertices in the same polyhedron are counted only once *
     *********************************************************/

    vertex = static_vertex (vertex);
    if (vertex->last == polyhedron) continue;

    vertex->last = polyhedron;
    vertex->n_polyhedrons++;
    }
  }

}

static void static_measure_vertex_polyhedrons (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, n_vertices, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_vertices = 0;

/**********************
 * get back polyhedra *
 **********************/
 
n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    if (vertex->vertex != NULL) continue;

    n_vertices++;
    n_polyhedrons = vertex->n_polyhedrons;

    /**************
     * statistics *
     **************/

    if (n_polyhedrons < *min) *min = n_polyhedrons;
    if (n_polyhedrons > *max) *max = n_polyhedrons;
    *average += n_polyhedrons;
    *deviation += n_polyhedrons * n_polyhedrons;
    }

  }

static_expectancy (average, deviation, n_vertices);
}

static void static_scan_vertex_faces (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *vertex;
gamgi_slist *slist_v, *slist_f;
int n_seeds, i;

n_seeds = voronoi->n_seeds;

/**************************************************
 * initialize vertex->last, so this function will *
 * work fine when placed in a different sequence  *
 **************************************************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    vertex = static_vertex (vertex);
    vertex->last = NULL;
    }
  }

/********************
 * accumulate faces *
 ********************/

for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    for (slist_v = face->vertex; slist_v != NULL; slist_v = slist_v->next)
      {
      vertex = (gamgi_vertex *) slist_v->data;

      /***************************************************
       * vertices in the same face are counted only once *
       ***************************************************/

      vertex = static_vertex (vertex);
      if (vertex->last == face) continue;

      vertex->last = face;
      vertex->n_faces++;
      }
    }
  }

}

static void static_measure_vertex_faces (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, n_vertices, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_vertices = 0;

/******************
 * get back faces *
 ******************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    if (vertex->vertex != NULL) continue;

    n_vertices++;
    n_faces = vertex->n_faces;

    /**************
     * statistics *
     **************/

    if (n_faces < *min) *min = n_faces;
    if (n_faces > *max) *max = n_faces;
    *average += n_faces;
    *deviation += n_faces * n_faces;
    }

  }

static_expectancy (average, deviation, n_vertices);
}

static void static_scan_vertex_edges (gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
int n_seeds, i;

/********************
 * accumulate edges *
 ********************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;
    if (edge->edge != NULL) continue;

    edge->vertex1->n_edges++;
    edge->vertex2->n_edges++;
    }
  }

}

static void static_measure_vertex_edges (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_vertex *vertex;
gamgi_slist *slist_v;
int n_seeds, n_vertices, n_edges, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_vertices = 0;

/******************
 * get back edges *
 ******************/

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_v = polyhedron->vertex; slist_v != NULL; slist_v = slist_v->next)
    {
    vertex = (gamgi_vertex *) slist_v->data;
    if (vertex->vertex != NULL) continue;

    n_vertices++;
    n_edges = vertex->n_edges;

    /**************
     * statistics *
     **************/

    if (n_edges < *min) *min = n_edges;
    if (n_edges > *max) *max = n_edges;
    *average += n_edges;
    *deviation += n_edges * n_edges;
    }

  }

static_expectancy (average, deviation, n_vertices);
}

static void static_measure_polyhedron_volumes (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double length, volume;
int n_seeds, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  volume = 0.0;
  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    length = gamgi_math_vector_length (face->position);
    volume += face->area * length / 6.0;
    }
  polyhedron->volume = volume;

  /**************
   * statistics *
   **************/

  if (volume < *min) *min = volume;
  if (volume > *max) *max = volume;
  *average += volume;
  *deviation += volume * volume;
  }

/*************************************************
 * volume calculated for all polyhedrons (with   *
 * outer planes or periodic boundary conditions) *
 *************************************************/

voronoi->volume[1] = *average;

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_areas (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double area; 
int n_seeds, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  area = 0.0;
  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    area += face->area;
    }

  /**************
   * statistics *
   **************/

  if (area < *min) *min = area;
  if (area > *max) *max = area;
  *average += area;
  *deviation += area * area;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
double length;
int n_seeds, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  length = 0.0;
  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    edge = (gamgi_edge *) slist_e->data;
    length += edge->length;
    }

  /**************
   * statistics *
   **************/

  if (length < *min) *min = length;
  if (length > *max) *max = length;
  *average += length;
  *deviation += length * length;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_anisotropy (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi) 
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double q[9], q_face[9], n[3];
double anisotropy;
int n_seeds, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

gamgi_math_matrix_zero (voronoi->anisotropy);

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  gamgi_math_matrix_zero (q);

  n_polyhedrons++;
  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    gamgi_math_vector_copy (face->position, n);
    gamgi_math_vector_normal (n);

    gamgi_math_matrix_absolute (q_face, 
    n[0]*n[0] - 1/3.0, n[0]*n[1], n[0]*n[2],
    n[1]*n[0], n[1]*n[1] - 1/3.0, n[1]*n[2],
    n[2]*n[0], n[2]*n[1], n[2]*n[2] - 1/3.0);

    gamgi_math_matrix_scale (q_face, q_face, -face->area);
    gamgi_math_matrix_add (q_face, q, q);
    }

  /****************************
   * system tensor anisotropy *
   ****************************/

  gamgi_math_matrix_scale (q, q, pow (polyhedron->volume, 1/3.0));
  gamgi_math_matrix_add (q, voronoi->anisotropy, voronoi->anisotropy);

  /********************************
   * polyhedron scalar anisotropy *
   ********************************/

  gamgi_math_matrix_scale (q, q, 1/polyhedron->volume);
  anisotropy = sqrt ((q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3] + 
  q[4]*q[4] + q[5]*q[5] + q[6]*q[6] + q[7]*q[7] + q[8]*q[8]) / 9);

  /**************
   * statistics *
   **************/

  if (anisotropy < *min) *min = anisotropy;
  if (anisotropy > *max) *max = anisotropy;
  *average += anisotropy;
  *deviation += anisotropy * anisotropy;
  }

/********************************************
 * divide tensor by total volume calculated *
 ********************************************/

gamgi_math_matrix_scale (voronoi->anisotropy, 
voronoi->anisotropy, 1/voronoi->volume[1]);

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_polyhedron_aberration (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double area;
double k, aberration;
int n_seeds, n_polyhedrons, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_polyhedrons = 0;

k = pow (36 * GAMGI_MATH_PI, 1/3.0);

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  n_polyhedrons++;
  area = 0.0;
  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;
    if (face->area < voronoi->area) continue;

    area += face->area;
    }

  aberration = area / k / pow (polyhedron->volume, 2/3.0);

  /**************
   * statistics *
   **************/

  if (aberration < *min) *min = aberration;
  if (aberration > *max) *max = aberration;
  *average += aberration;
  *deviation += aberration * aberration;
  }

static_expectancy (average, deviation, n_polyhedrons);
}

static void static_measure_face_areas (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double area;
int n_seeds, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_faces = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    n_faces++;
    area = face->area;

    /**************
     * statistics *
     **************/

    if (area < *min) *min = area;
    if (area > *max) *max = area;
    *average += area;
    *deviation += area * area;
    }
  }

static_expectancy (average, deviation, n_faces);
}

static void static_measure_face_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *old, *new;
gamgi_slist *slist_f, *slist_v;
double sub[3];
double length;
int n_seeds, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_faces = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /*************************************
     * inner faces are counted only once *
     *************************************/
    
    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    n_faces++;
    length = 0.0;
    slist_v = face->vertex;
    old = (gamgi_vertex *) slist_v->data;
    for (slist_v = slist_v->next; slist_v != NULL; slist_v = slist_v->next)
      {
      new = (gamgi_vertex *) slist_v->data;
      gamgi_math_vector_sub (new->position, old->position, sub);
      length += gamgi_math_vector_length (sub);
      old = new;
      }
    slist_v = face->vertex;
    new = (gamgi_vertex *) slist_v->data;
    gamgi_math_vector_sub (new->position, old->position, sub);
    length += gamgi_math_vector_length (sub);

    /**************
     * statistics *
     **************/

    if (length < *min) *min = length;
    if (length > *max) *max = length;
    *average += length;
    *deviation += length * length;
    }
  }

static_expectancy (average, deviation, n_faces);
}

static void static_measure_face_angles (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
double angle;
int n_seeds, n_angles, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_angles = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    /*********************************************
     *      count all edges per polyhedron       *
     *                                           *
     * for correct edges, both faces are known:  *
     * this test is needed only for wrong edges, *
     *********************************************/
       
    edge = (gamgi_edge *) slist_e->data;
    if (edge->face2 == NULL) continue;
    n_angles++;
 
    angle = 180.0 - gamgi_math_vector_angle 
    (edge->face1->position, edge->face2->position);

    /**************
     * statistics *
     **************/

    if (angle < *min) *min = angle;
    if (angle > *max) *max = angle;
    *average += angle;
    *deviation += angle * angle;
     
    }
  }

static_expectancy (average, deviation, n_angles);
}

static void static_measure_face_aberration (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *vertex;
gamgi_slist *slist_f, *slist_v;
double position[3], edge[3]; 
double length;
double k, aberration;
int n_seeds, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_faces = 0;

k = 2 * sqrt (GAMGI_MATH_PI);

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /********************************************
     * inner faces must be considered only once *
     ********************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    n_faces++;
    length = 0.0;
    slist_v = face->vertex;
    vertex = (gamgi_vertex *) slist_v->data;
    gamgi_math_vector_copy (vertex->position, position);
    for (slist_v = slist_v->next; slist_v != NULL; slist_v = slist_v->next)
      {
      vertex = (gamgi_vertex *) slist_v->data;
      gamgi_math_vector_sub (vertex->position, position, edge);
      length += gamgi_math_vector_length (edge);
      gamgi_math_vector_copy (vertex->position, position);
      }
    slist_v = face->vertex;
    vertex = (gamgi_vertex *) slist_v->data;
    gamgi_math_vector_sub (vertex->position, position, edge);
    length += gamgi_math_vector_length (edge);

    aberration = length / k / sqrt (face->area);

    /**************
     * statistics *
     **************/

    if (aberration < *min) *min = aberration;
    if (aberration > *max) *max = aberration;
    *average += aberration;
    *deviation += aberration * aberration;
    }
  }

static_expectancy (average, deviation, n_faces);
}

static void static_measure_edge_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_edge *edge;
gamgi_slist *slist_e;
double length;
int n_seeds, n_edges, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_edges = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_e = polyhedron->edge; slist_e != NULL; slist_e = slist_e->next)
    {
    /*************************************************
     * polyhedron edges must be considered only once *
     *************************************************/

    edge = (gamgi_edge *) slist_e->data;
    if (edge->edge != NULL) continue;
    n_edges++;

    length = edge->length;

    /**************
     * statistics *
     **************/

    if (length < *min) *min = length;
    if (length > *max) *max = length;
    *average += length;
    *deviation += length * length;
    }
  }

static_expectancy (average, deviation, n_edges);
}

static void static_measure_edge_angles (double *min, double *max,
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_vertex *vertex1, *vertex2, *vertex3;
gamgi_slist *slist_f, *slist_v;
double v1[3], v2[3];
double angle;
int n_seeds, n_angles, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_angles = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /********************************************
     * inner faces must be considered only once *
     ********************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    slist_v = face->vertex;
    vertex1 = (gamgi_vertex *) slist_v->data;
    vertex1 = static_vertex (vertex1);
    do
      {
      slist_v = slist_v->next;
      vertex2 = (gamgi_vertex *) slist_v->data;
      vertex2 = static_vertex (vertex2);
      }
    while (vertex2 == vertex1 && slist_v != NULL);
    if (slist_v == NULL) continue;

    gamgi_math_vector_sub (vertex2->position, vertex1->position, v2);
    for (slist_v = slist_v->next; slist_v != NULL; slist_v = slist_v->next)
      {
      vertex3 = (gamgi_vertex *) slist_v->data;
      vertex3 = static_vertex (vertex3);
      if (vertex3 == vertex2) continue;

      n_angles++;
      gamgi_math_vector_scale (v2, v1, -1.0);
      gamgi_math_vector_sub (vertex3->position, vertex2->position, v2);
      angle = gamgi_math_vector_angle (v1, v2);
      vertex2 = vertex3;

      /**************
       * statistics *
       **************/

      if (angle < *min) *min = angle;
      if (angle > *max) *max = angle;
      *average += angle;
      *deviation += angle * angle;
      }
    slist_v = face->vertex;
    do
      {
      vertex3 = (gamgi_vertex *) slist_v->data;
      vertex3 = static_vertex (vertex3);
      slist_v =  slist_v->next;
      }
    while (vertex3 == vertex2 && slist_v != NULL);

    if (vertex3 != vertex2)
      {
      n_angles++;
      gamgi_math_vector_scale (v2, v1, -1.0);
      gamgi_math_vector_sub (vertex3->position, vertex2->position, v2);
      angle = gamgi_math_vector_angle (v1, v2);
      vertex2 = vertex3;

      /**************
       * statistics *
       **************/

      if (angle < *min) *min = angle;
      if (angle > *max) *max = angle;
      *average += angle;
      *deviation += angle * angle;
      }
    if (slist_v == NULL) continue;

    do
      {
      vertex3 = (gamgi_vertex *) slist_v->data;
      vertex3 = static_vertex (vertex3);
      slist_v =  slist_v->next;
      }
    while (vertex3 == vertex2 && slist_v != NULL);
    if (vertex3 != vertex2)
      {
      n_angles++;
      gamgi_math_vector_scale (v2, v1, -1.0);
      gamgi_math_vector_sub (vertex3->position, vertex2->position, v2);
      angle = gamgi_math_vector_angle (v1, v2);
      vertex2 = vertex3;

      /**************
       * statistics *
       **************/

      if (angle < *min) *min = angle;
      if (angle > *max) *max = angle;
      *average += angle;
      *deviation += angle * angle;
      }
     
    }
  }

static_expectancy (average, deviation, n_angles);
}

static void static_measure_seed_lengths (double *min, double *max, 
double *average, double *deviation, gamgi_voronoi *voronoi)
{
gamgi_polyhedron *polyhedron;
gamgi_face *face;
gamgi_slist *slist_f;
double length;
int n_seeds, n_faces, i;

*min = DBL_MAX;
*max = -DBL_MAX;
*average = 0.0;
*deviation = 0.0;
n_faces = 0;

n_seeds = voronoi->n_seeds;
for (i = 0; i < n_seeds; i++)
  {
  polyhedron = voronoi->polyhedron[i];
  if (polyhedron == NULL) continue;

  for (slist_f = polyhedron->face; slist_f != NULL; slist_f = slist_f->next)
    {
    face = (gamgi_face *) slist_f->data;

    /********************************************
     * inner faces must be considered only once *
     ********************************************/

    if (face->area < voronoi->area) continue;
    if (face->global == 0 && face->local > polyhedron->local &&
    voronoi->polyhedron[face->local] != NULL) continue;

    n_faces++;
    length = gamgi_math_vector_length (face->position);

    /**************
     * statistics *
     **************/

    if (length < *min) *min = length;
    if (length > *max) *max = length;
    *average += length;
    *deviation += length * length;
    }
  }
  
static_expectancy (average, deviation, n_faces);
}

static void static_report (GtkWidget *text, gamgi_voronoi *voronoi)
{
GtkTextBuffer *buffer;
GtkTextIter iter;
double min, max, average, deviation;
int internal, external, total;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

/***************************
 * report topological data *
 ***************************/

/**********************************************
 * mark vertices that participate in at       *
 * least one border face, to help find real   *
 * and border polyhedrons, edges and vertices *
 **********************************************/

static_scan_vertices (voronoi);

static_measure_polyhedrons (&internal, &external, &total, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "Polyhedrons\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Internal: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", internal);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "External: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", external);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Total: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", total);

static_measure_faces (&internal, &external, &total, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFaces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Internal: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", internal);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "External: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", external);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Total: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", total);

static_measure_edges (&internal, &external, &total, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdges\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Internal: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", internal);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "External: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", external);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Total: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", total);

static_measure_vertices (&internal, &external, &total, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertices\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Internal: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", internal);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "External: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", external);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Total: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", total);

static_measure_polyhedron_faces (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Faces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_edges (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Edges\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_vertices (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Vertices\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_face_vertices (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Vertices\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_scan_edge_polyhedrons (voronoi);
static_measure_edge_polyhedrons (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Polyhedrons\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_scan_edge_faces (voronoi);
static_measure_edge_faces (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Faces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_scan_vertex_polyhedrons (voronoi);
static_measure_vertex_polyhedrons (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertex Polyhedrons\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_scan_vertex_faces (voronoi);
static_measure_vertex_faces (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertex Faces\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_scan_vertex_edges (voronoi);
static_measure_vertex_edges (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nVertex Edges\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

/***************************
 * report geometrical data *
 ***************************/

static_measure_polyhedron_volumes (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nSystem Volume\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Real: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->volume[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Calculated: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->volume[1]);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Volumes\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_areas (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Areas\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_lengths (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_anisotropy (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nSystem Anisotropy\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A11: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[0]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A12: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[1]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A13: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[2]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A21: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[3]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A22: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[4]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A23: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[5]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A31: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[6]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A32: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[7]);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "A33: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", voronoi->anisotropy[8]);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Anisotropy\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_polyhedron_aberration (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nPolyhedron Aberration\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_face_areas (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Areas\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_face_lengths (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_face_angles (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Angles\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_face_aberration (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nFace Aberration\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_edge_lengths (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_edge_angles (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nEdge Angles\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);

static_measure_seed_lengths (&min, &max, &average, &deviation, voronoi);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nSeed Lengths\n");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", min);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", max);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", average);
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%f\n", deviation);
}

static void static_export_start (gamgi_cluster *cluster)
{
cluster->reference = GAMGI_CHEM_POLYTOPE;

/********************************************************
 * initialize arrays (points, loops, colors and paints) *
 ********************************************************/

cluster->n_points = GAMGI_PHYS_VORONOI_ARRAY;
cluster->points = (double *) malloc (3 * cluster->n_points * sizeof (double));

cluster->n_loops = GAMGI_PHYS_VORONOI_ARRAY;
cluster->loops = (int *) malloc (cluster->n_loops * sizeof (int));
cluster->loops[0] = 0;

cluster->n_colors = GAMGI_PHYS_VORONOI_ARRAY;
cluster->colors = (float *) malloc (3 * cluster->n_colors * sizeof (float));

cluster->n_paints = GAMGI_PHYS_VORONOI_ARRAY;
cluster->paints = (int *) malloc (cluster->n_paints * sizeof (int));
}

static void static_export_color (gamgi_cluster *cluster,
float *color, int n_loops, int n_polyhedrons)
{
/****************************************
 * reallocate color arrays if necessary *
 ****************************************/

if (n_polyhedrons + 1 > cluster->n_colors)
  {
  cluster->n_colors += GAMGI_PHYS_VORONOI_ARRAY;
  cluster->colors = (float *) realloc (cluster->colors,
  3 * cluster->n_colors * sizeof (float));

  cluster->n_paints += GAMGI_PHYS_VORONOI_ARRAY;
  cluster->paints = (int *) realloc (cluster->paints,
  cluster->n_paints * sizeof (int));
  }

/**************************
 * give the atom color to *
 * the polyhedron faces   *
 **************************/

cluster->colors[3 * n_polyhedrons + 0] = color[0];
cluster->colors[3 * n_polyhedrons + 1] = color[1];
cluster->colors[3 * n_polyhedrons + 2] = color[2];
cluster->paints[n_polyhedrons] = n_loops;
}

static void static_export_loop (gamgi_cluster *cluster, 
gamgi_face *face, int *n_loops, int *n_points)
{
gamgi_vertex *vertex;
gamgi_slist *slist;
double *points;
int *loops;
int offset;

/*********************************
 * reallocate array if necessary *
 *********************************/

offset = 1 + *n_loops + face->n_vertices;
while (offset >= cluster->n_loops)
  {
  cluster->n_loops += GAMGI_PHYS_VORONOI_ARRAY;
  cluster->loops = (int *) realloc (cluster->loops, 
  cluster->n_loops * sizeof (int));
  }

/*********************************
 * reallocate array if necessary *
 *********************************/

offset = *n_points + face->n_vertices;
while (offset >= cluster->n_points)
  {
  cluster->n_points += GAMGI_PHYS_VORONOI_ARRAY;
  cluster->points = (double *) realloc (cluster->points, 
  3 * cluster->n_points * sizeof (double));
  }

loops = cluster->loops;
points = cluster->points;

/****************************************
 * add face data to loops,points arrays *
 ****************************************/

loops[0] += 1;

loops[*n_loops] = face->n_vertices;
*n_loops += 1;

/***********************************************
 * collect all vertices belonging to this face *
 ***********************************************/

for (slist = face->vertex; slist != NULL; slist = slist->next)
  {
  vertex = (gamgi_vertex *) slist->data;

  /****************************************
   * add face data to loops,points arrays *
   ****************************************/

  points[*n_points * 3 + 0] = vertex->position[0];
  points[*n_points * 3 + 1] = vertex->position[1];
  points[*n_points * 3 + 2] = vertex->position[2];
  loops[*n_loops] = *n_points;

  *n_points += 1;
  *n_loops += 1;
  }

}

static void static_export_point (gamgi_cluster *cluster, 
double *position, int *n_points)
{
double *points;
int offset;

/*********************************
 * reallocate array if necessary *
 *********************************/

offset = *n_points + 1;
while (offset >= cluster->n_points)
  {
  cluster->n_points += GAMGI_PHYS_VORONOI_ARRAY;
  cluster->points = (double *) realloc (cluster->points,
  3 * cluster->n_points * sizeof (double));
  }

points = cluster->points;

/*********************************
 * add seed data to points array *
 *********************************/

points[*n_points * 3 + 0] = position[0];
points[*n_points * 3 + 1] = position[1];
points[*n_points * 3 + 2] = position[2];

*n_points += 1;
}

static void static_export_seed (gamgi_cluster *cluster, 
gamgi_polyhedron *polyhedron, int *n_polyhedrons, int *n_points, 
gamgi_voronoi *voronoi)
{
if (polyhedron == NULL) return;

static_export_point (cluster, polyhedron->position, n_points);
*n_polyhedrons += 1;
}

static void static_export_polyhedron (gamgi_cluster *cluster, 
gamgi_polyhedron *polyhedron, float *color, int *n_loops, 
int *n_points, int *n_polyhedrons, gamgi_voronoi *voronoi)
{
gamgi_slist *slist;
gamgi_face *face;
gamgi_bool first = TRUE;

if (polyhedron == NULL) return;

for (slist = polyhedron->face; slist != NULL; slist = slist->next)
  {
  face = (gamgi_face *) slist->data;

  /***************************************
   * do not export:                      *
   * 1) faces too small                  *
   * 2) inner faces when borders = FACES *
   * 3) repeated inner faces             *
   ***************************************/

  if (face->area < voronoi->area) continue;
  if (voronoi->borders == GAMGI_PHYS_FACES && 
  face->local >= 0 && face->global == 0) continue;
  if (face->global == 0 && face->local > polyhedron->local &&
  voronoi->polyhedron[face->local] != NULL) continue;

  if (first == TRUE)
    {
    if (color == NULL)
      static_export_color (cluster, polyhedron->color, *n_loops, *n_polyhedrons);

    gamgi_math_vector_add (polyhedron->position, cluster->center, cluster->center);
    *n_polyhedrons += 1;

    first = FALSE;
    }

  static_export_loop (cluster, face, n_loops, n_points);
  }

}

static void static_export_end (gamgi_cluster *cluster,
int n_loops, int n_points, int n_polyhedrons, int n_colors)
{
/********************************************
 *  complete cluster center calculation     *
 *                                          *
 * reallocate arrays (loops, points, colors *
 * and paints) to their exact, final, size  *
 ********************************************/

if (n_polyhedrons > 0)
  gamgi_math_vector_scale (cluster->center,
  cluster->center, 1.0/n_polyhedrons);

cluster->n_points = n_points;
cluster->points = (double *) realloc (cluster->points, 
3 * cluster->n_points * sizeof (double));

cluster->n_loops = n_loops;
cluster->loops = (int *) realloc (cluster->loops, 
cluster->n_loops * sizeof (int));

cluster->n_colors = n_colors;
cluster->colors = (float *) realloc (cluster->colors, 
3 * cluster->n_colors * sizeof (float));

cluster->n_paints = n_colors;
cluster->paints = (int *) realloc (cluster->paints, 
cluster->n_paints * sizeof (int));
}

static void static_export (gamgi_cluster *cluster, gamgi_voronoi *voronoi)
{
float *color;
int n_points, n_loops;
int n_polyhedrons;
int i, n, n_colors;

static_export_start (cluster);

/***********************************
 * n_loops, n_points:              *
 * size of arrays loops, points    *
 *                                 *
 * loops[0] is the number of faces *
 ***********************************/

n = voronoi->n_seeds;
color = voronoi->color;

/*************
 * add seeds *
 *************/

n_points = 0;
n_polyhedrons = 0;
if (cluster->atoms == TRUE)
  {
  for (i = 0; i < n; i++)
    {
    static_export_seed (cluster, voronoi->polyhedron[i],
    &n_points, &n_polyhedrons, voronoi);
    }
  cluster->atoms = n_polyhedrons;
  }

/*****************
 * add polyhedra *
 *****************/

n_loops = 1;
n_polyhedrons = 0;
for (i = 0; i < n; i++)
  {
  static_export_polyhedron (cluster, voronoi->polyhedron[i], 
  color, &n_loops, &n_points, &n_polyhedrons, voronoi);
  }

/*************
 * add color *
 *************/

if (color == NULL) n_colors = n_polyhedrons;
else
  {
  static_export_color (cluster, color, 1, 0);
  n_colors = 1;
  }

static_export_end (cluster, n_loops, n_points, n_polyhedrons, n_colors);
}

gamgi_bool gamgi_phys_voronoi (gamgi_object *object,
gamgi_cluster *cluster, gamgi_bool periodic, double offset,
double variancy, double apex, double area, double vertex, 
float *color, gamgi_enum borders, GtkWidget *text)
{
gamgi_voronoi *voronoi;
gamgi_bool valid;

/***********************************************************
 * 1) start resources: create seed list and main structure *
 * 2) build tesselation:                                   *
 * 3) merge tesselation: link faces, vertices and edges    *
 * 4) measure tesselation: get geometry and topology data  *
 * 5) export tesselation: build visualization arrays       *
 * 6) end resources: remove seed list and main structure   *
 ***********************************************************/
  
voronoi = static_start (object, periodic, offset, 
variancy, apex, area, vertex, color, borders);
if (voronoi == NULL) return FALSE;

valid = static_build (voronoi);
if (valid == TRUE)
  {
  static_merge (voronoi);
  static_report (text, voronoi);
  static_export (cluster, voronoi);
  }

static_end (voronoi);
return valid;
}
