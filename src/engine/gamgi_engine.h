/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_ENGINE
#define GAMGI_ENGINE

/******** libraries *******
 **************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

/************* macros ************
 *********************************/

#define GAMGI_ENGINE_TOKEN  20
#define GAMGI_ENGINE_STRING 50
#define GAMGI_ENGINE_LINE  200

/**********************************************
 *              arrays of bits                *
 * source: http://c-faq.com/misc/bitsets.html *
 **********************************************/

#define GAMGI_ENGINE_BITMASK(b) (1 << ((b) % CHAR_BIT))
#define GAMGI_ENGINE_BITSLOT(b) ((b) / CHAR_BIT)
#define GAMGI_ENGINE_BITSET(a, b) ((a)[GAMGI_ENGINE_BITSLOT(b)] |= GAMGI_ENGINE_BITMASK(b))
#define GAMGI_ENGINE_BITCLEAR(a, b) ((a)[GAMGI_ENGINE_BITSLOT(b)] &= ~GAMGI_ENGINE_BITMASK(b))
#define GAMGI_ENGINE_BITTEST(a, b) ((a)[GAMGI_ENGINE_BITSLOT(b)] & GAMGI_ENGINE_BITMASK(b))
#define GAMGI_ENGINE_BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

/********** types *********
 **************************/

/***************************************
 * for the sake of simplicity, the     *
 * folowing GTK and OpenGL types are   *
 * NOT used in GAMGI, the built-in     *
 * C types are applied instead.        *
 *                                     *
 * defined in glib/include/glib.h:     *
 * typedef char            gchar;      *
 * typedef short           gshort;     *
 * typedef long            glong;      *
 * typedef int             gint;       *
 * typedef gint            gboolean;   *
 * typedef void*           gpointer;   *
 * typedef unsigned char   guchar;     *
 * typedef unsigned short  gushort;    *
 * typedef unsigned long   gulong;     *
 * typedef unsigned int    guint;      *
 * typedef float           gfloat;     *
 * typedef double          gdouble;    *
 *                                     *
 * defined in mesa/include/GL/gl.h:    *
 * typedef unsigned int    GLenum;     *
 * typedef unsigned char   GLboolean;  *
 * typedef unsigned int    GLbitfield; *
 * typedef void            GLvoid;     *
 * typedef signed char     GLbyte;     *
 * typedef short           GLshort;    * 
 * typedef int             GLint;      *
 * typedef unsigned char   GLubyte;    *
 * typedef unsigned short  GLushort;   *
 * typedef unsigned int    GLuint;     *
 * typedef float           GLfloat;    *
 * typedef double          GLdouble;   *
 ***************************************/

/****************************************
 * these types improve code readability *
 ****************************************/

typedef int gamgi_enum;
typedef int gamgi_bool;

/********* 
 * lists *
 *********/

typedef struct _gamgi_slist {
void *data;
struct _gamgi_slist *next; } gamgi_slist;

typedef struct _gamgi_dlist {
void *data;
struct _gamgi_dlist *next, *before; } gamgi_dlist;

/******************
 * dynamic arrays *
 ******************/

typedef struct _gamgi_iarray {
int *array, *last;
int size, increment, dimension, offset; } gamgi_iarray;

typedef struct _gamgi_darray {
double *array, *last;
int size, increment, dimension, offset; } gamgi_darray;

/********** 
 * object *
 **********/

typedef struct _gamgi_object {
char name[GAMGI_ENGINE_TOKEN];
char label[GAMGI_ENGINE_TOKEN];
unsigned int number;
int class;
double scale;
unsigned int task;
struct _gamgi_object *object; 
gamgi_dlist *dlist;
gamgi_dlist *selection; } gamgi_object;

/******** 
 * text *
 ********/

typedef struct _gamgi_text {
gamgi_object object;
void (*draw) (struct _gamgi_text *);
int mark;

char *string;
int length;

gamgi_enum style;
gamgi_enum font;

double origin[3];
double center[3];
double quaternion[4];

float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
struct _gamgi_text *next, *before; } gamgi_text;

/*********** 
 * orbital *
 ***********/

typedef struct _gamgi_orbital {
gamgi_object object;
void (*draw) (struct _gamgi_orbital *);

/****************
 * quantum data *
 ****************/

int n, l, m;
double charge;

/**************
 * model data *
 **************/

gamgi_enum style;
double radius, density;
int sampling, seed;

/***************
 * volume data *
 ***************/

gamgi_bool phase, frame;
int octants[8];
gamgi_enum axes;

/**************
 * array data *
 **************/

int dots[3];
double *points;
int *lines;
int n_points, n_lines;

/************
 * position *
 ************/

double origin[3];
double center[3];
double quaternion[4];

/**********
 * colors *
 **********/

float base_r, base_g, base_b;
float phase_r, phase_g, phase_b;
float frame_r, frame_g, frame_b;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
struct _gamgi_orbital *next, *before; } gamgi_orbital;

/******** 
 * atom *
 ********/

typedef struct _gamgi_atom {
gamgi_object object;
void (*draw) (struct _gamgi_atom *);

int element;
double position[3];
double mass, radius, charge;
double temperature, occupancy;
float variancy, size;
float red, green, blue;
int mark;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
struct _gamgi_atom *next, *before; } gamgi_atom;

/******** 
 * bond *
 ********/

typedef struct _gamgi_bond {
gamgi_object object;
void (*draw) (struct _gamgi_bond *);

gamgi_atom *atom1, *atom2;
gamgi_dlist *dlist1, *dlist2;
unsigned char color1[3], color2[3];
int first;
gamgi_atom *copy;

float order;
float covalency;

float size;
gamgi_bool color;
float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
struct _gamgi_bond *next, *before; } gamgi_bond;

/************* 
 * direction *
 *************/

typedef struct _gamgi_direction {
gamgi_object object;

void (*draw) (struct _gamgi_direction *);
void (*net) (double *, double *);

gamgi_enum reference;
gamgi_enum model;

int uvw[3];
gamgi_enum vectors;
int node[4];

double origin[3], center[3];
double quaternion[4];

double *points;
int *loops;
int n_points, n_loops;

float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
struct _gamgi_direction *next, *before; } gamgi_direction;

/********* 
 * plane *
 *********/

typedef struct _gamgi_plane {
gamgi_object object;

void (*draw) (struct _gamgi_plane *);
void (*net) (double *, double *);

gamgi_enum reference;
gamgi_enum model;

int hkl[3];
gamgi_enum vectors;
int order;

double origin[3], center[3];
double quaternion[4];

double *points;
int *loops;
int n_points, n_loops;

float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
struct _gamgi_plane *next, *before; } gamgi_plane;

/********* 
 * group *
 *********/

typedef struct _gamgi_group {
gamgi_object object;
void (*draw) (struct _gamgi_group *);
int mark;

gamgi_enum reference;
double origin[3], center[3];
double quaternion[4];
gamgi_bool faces;

double *points;
int *loops;
int n_points, n_loops;

float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
struct _gamgi_group *next, *before; } gamgi_group;

/************ 
 * molecule *
 ************/

typedef struct _gamgi_molecule {
gamgi_object object;
void (*draw) (struct _gamgi_molecule *);

double origin[3], center[3];
double quaternion[4];

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
struct _gamgi_molecule *next, *before; } gamgi_molecule;

/*********** 
 * cluster *
 ***********/

typedef struct _gamgi_cluster {
gamgi_object object;
int mark;

gamgi_enum reference;
double origin[3], center[3];
double quaternion[4];
gamgi_bool faces;

double *points;
int *loops;
float *colors;
int *paints;

int n_points, n_loops;
int n_colors, n_paints;
int atoms;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
gamgi_dlist *molecule_start;
gamgi_dlist *molecule_end;
gamgi_dlist *cluster_start;
gamgi_dlist *cluster_end;
struct _gamgi_cluster *next, *before; } gamgi_cluster;

/******** 
 * cell *
 ********/

typedef struct _gamgi_cell {
gamgi_object object;

double a, b, c, ab, ac, bc;
double a1[3], a2[3], a3[3];
double p1[3], p2[3], p3[3]; 
double v1, v2, v3, v12, v13, v23;
double reciprocal;
gamgi_enum lattice, model, borders;
gamgi_bool faces, nodes;
int group, n1, n2, n3; 

double origin[3], center[3];
double quaternion[4];

/**************
 * origin node *
 * origin axes *
 **************/

int o1, o2, o3;
gamgi_enum origin_vectors;
gamgi_bool axes;
gamgi_enum axes_vectors;

double *points;
int *loops, *lines;
int n_points, n_loops, n_lines, n_nodes;

float red, green, blue;

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
gamgi_dlist *molecule_start;
gamgi_dlist *molecule_end;
gamgi_dlist *cluster_start;
gamgi_dlist *cluster_end;
struct _gamgi_cell *next, *before; } gamgi_cell;

/********* 
 * arrow *
 *********/

typedef struct _gamgi_arrow {
gamgi_object object;
void (*draw) (struct _gamgi_arrow *);
gamgi_dlist *text_start;
gamgi_dlist *text_end;
struct _gamgi_arrow *next, *before; } gamgi_arrow;

/********* 
 * shape *
 *********/

typedef struct _gamgi_shape {
gamgi_object object;
void (*draw) (struct _gamgi_shape *);
gamgi_dlist *text_start;
gamgi_dlist *text_end;
struct _gamgi_shape *next, *before; } gamgi_shape;

/********* 
 * graph *
 *********/

typedef struct _gamgi_graph {
gamgi_object object;
void (*draw) (struct _gamgi_graph *);
gamgi_dlist *text_start;
gamgi_dlist *text_end;
struct _gamgi_graph *next, *before; } gamgi_graph;

/************ 
 * assembly *
 ************/

typedef struct _gamgi_assembly {
gamgi_object object;
void (*draw) (struct _gamgi_assembly *);
int mark;

double origin[3], center[3];
double quaternion[4];

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
gamgi_dlist *molecule_start;
gamgi_dlist *molecule_end;
gamgi_dlist *cluster_start;
gamgi_dlist *cluster_end;
gamgi_dlist *cell_start;
gamgi_dlist *cell_end;
gamgi_dlist *arrow_start;
gamgi_dlist *arrow_end;
gamgi_dlist *shape_start;
gamgi_dlist *shape_end;
gamgi_dlist *graph_start;
gamgi_dlist *graph_end;
gamgi_dlist *assembly_start;
gamgi_dlist *assembly_end;
struct _gamgi_assembly *next, *before; } gamgi_assembly;

/********* 
 * light *
 *********/

typedef struct _gamgi_light {
gamgi_object object;
void (*draw) (struct _gamgi_light *);

/******************
 * rgb color data *
 ******************/

float ambient[4];
float diffuse[4];
float specular[4];

/*****************
 * geometry data *
 *****************/

float position[4];
float direction[3];
float angle;

/********************
 * attenuation data *
 ********************/

float constant;
float linear;
float quadratic;
float radial;

struct _gamgi_light *next, *before; } gamgi_light;

/********* 
 * layer *
 *********/

typedef struct _gamgi_layer {
gamgi_object object;
void (*draw) (struct _gamgi_layer *);

/*************************
 * modelview matrix data *
 *************************/

double eye[3];
double center[3];
double up[3];

double referential[9];

/**************************
 * projection matrix data *
 **************************/

int perspective;
double top, near, far;

/********************
 * visibility data  *
 *                  *
 * referential axes *
 *                  *
 * background color *
 ********************/

gamgi_enum visibility_in;
gamgi_enum visibility_out;
gamgi_bool axes;
float color[4];

/********************
 * number of lights *
 ********************/

int light;

/****************
 * history data *
 ****************/

gamgi_dlist *undo;
gamgi_dlist *save;

/***************
 * child links *
 ***************/

gamgi_dlist *text_start;
gamgi_dlist *text_end;
gamgi_dlist *orbital_start;
gamgi_dlist *orbital_end;
gamgi_dlist *bond_start;
gamgi_dlist *bond_end;
gamgi_dlist *atom_start;
gamgi_dlist *atom_end;
gamgi_dlist *direction_start;
gamgi_dlist *direction_end;
gamgi_dlist *plane_start;
gamgi_dlist *plane_end;
gamgi_dlist *group_start;
gamgi_dlist *group_end;
gamgi_dlist *molecule_start;
gamgi_dlist *molecule_end;
gamgi_dlist *cluster_start;
gamgi_dlist *cluster_end;
gamgi_dlist *cell_start;
gamgi_dlist *cell_end;
gamgi_dlist *arrow_start;
gamgi_dlist *arrow_end;
gamgi_dlist *shape_start;
gamgi_dlist *shape_end;
gamgi_dlist *graph_start;
gamgi_dlist *graph_end;
gamgi_dlist *assembly_start; 
gamgi_dlist *assembly_end; 
gamgi_dlist *light_start;
gamgi_dlist *light_end;
struct _gamgi_layer *next, *before; } gamgi_layer;

/********** 
 * window *
 **********/

typedef struct _gamgi_window {
gamgi_object object;
void (*draw) (struct _gamgi_window *);

/*************
 * resources *
 *************/

GtkWidget *window;
GtkWidget *dialog0;
GtkWidget *dialog1;
GtkWidget *help;

gamgi_dlist *history;
gamgi_layer *layer;
gamgi_object *focus;

gamgi_enum action;
gamgi_enum mode;
gamgi_enum class;
gamgi_bool axes;

/*********************
 * window dimensions *
 *********************/

int origin_x, origin_y;
int width, height;

/****************
 * menu section *
 ****************/

GtkWidget *top;
GtkWidget *medium;
GtkWidget *bottom;
gamgi_bool top_flag;
gamgi_bool medium_flag;
gamgi_bool bottom_flag;

/*******************
 * graphic section *
 *******************/

GtkWidget *area;
float begin_x;
float begin_y;

/****************
 * tool section *
 ****************/

GtkWidget *entry; 
GtkWidget *ruler; 
gamgi_enum axis;
double total;

/******************
 * status section *
 ******************/

GtkWidget *statusbar_focus, *statusbar_layer;
GtkWidget *statusbar_message, *statusbar_time;
int context_focus, context_layer;
int context_message, context_time;
int timer_focus;
int timer_layer;
int timer_message;
int timer_progress;
GtkWidget *progress_bar;
gamgi_bool demo_flag;

/***************
 * child links *
 ***************/

gamgi_dlist *layer_start;
gamgi_dlist *layer_end;

struct _gamgi_window *next, *before; } gamgi_window;

/*******************************************
 * general functions:                      *
 * 1) called to show a selected Help topic *
 * 2) called after an object is selected   *
 *******************************************/

typedef void (* gamgi_function1) (void *data);
typedef void (* gamgi_function2) (gamgi_object *object, void *data);

/************* macros ************
 *********************************/

#define GAMGI_ENGINE_UNDO 5
#define GAMGI_ENGINE_SAVE 5

#define GAMGI_CAST_SLIST (gamgi_slist *)
#define GAMGI_CAST_DLIST (gamgi_dlist *)
#define GAMGI_CAST_OBJECT (gamgi_object *)

#define GAMGI_CAST_TEXT (gamgi_text *)
#define GAMGI_CAST_ORBITAL (gamgi_orbital *)
#define GAMGI_CAST_BOND (gamgi_bond *)
#define GAMGI_CAST_ATOM (gamgi_atom *)
#define GAMGI_CAST_DIRECTION (gamgi_direction *)
#define GAMGI_CAST_PLANE (gamgi_plane *)
#define GAMGI_CAST_GROUP (gamgi_group *)
#define GAMGI_CAST_MOLECULE (gamgi_molecule *)
#define GAMGI_CAST_CLUSTER (gamgi_cluster *)
#define GAMGI_CAST_CELL (gamgi_cell *)
#define GAMGI_CAST_ARROW (gamgi_arrow *)
#define GAMGI_CAST_SHAPE (gamgi_shape *)
#define GAMGI_CAST_GRAPH (gamgi_graph *)
#define GAMGI_CAST_ASSEMBLY (gamgi_assembly *)
#define GAMGI_CAST_LIGHT (gamgi_light *)
#define GAMGI_CAST_LAYER (gamgi_layer *)
#define GAMGI_CAST_WINDOW (gamgi_window *)
#define GAMGI_CAST_GAMGI (gamgi_gamgi *)

#define GAMGI_INT_TO_POINTER(integer) ((void *)((char *) 0 + (integer)))
#define GAMGI_POINTER_TO_INT(pointer) ((unsigned int)((char *)(pointer) - (char *) 0))

/********** enumerations **********
 **********************************/

/**************************************************************
 * All external enumerations start at 1 and share FALSE for 0 *
 **************************************************************/

enum {GAMGI_ENGINE_HELP = 1, GAMGI_ENGINE_TEXT, GAMGI_ENGINE_ORBITAL,
GAMGI_ENGINE_BOND, GAMGI_ENGINE_ATOM, GAMGI_ENGINE_DIRECTION, GAMGI_ENGINE_PLANE,
GAMGI_ENGINE_GROUP, GAMGI_ENGINE_MOLECULE, GAMGI_ENGINE_CLUSTER, GAMGI_ENGINE_CELL,
GAMGI_ENGINE_ARROW, GAMGI_ENGINE_SHAPE, GAMGI_ENGINE_GRAPH, GAMGI_ENGINE_ASSEMBLY,
GAMGI_ENGINE_LIGHT, GAMGI_ENGINE_LAYER, GAMGI_ENGINE_WINDOW, GAMGI_ENGINE_GAMGI};

enum {GAMGI_ENGINE_SELECT_ELEMENT = 1, GAMGI_ENGINE_SELECT_LENGTH};

#endif
