/******************************************
 *
 * $GAMGI/src/global/gamgi_global.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_GLOBAL
#define GAMGI_GLOBAL

typedef struct {
gamgi_object object;

gamgi_bool source_in;
gamgi_bool agent_in;

char *local;
char *remote;
char *browser;

} gamgi_help_class;

typedef struct {
gamgi_object object;

char *path;
gamgi_enum style;
gamgi_enum wired;
gamgi_enum solid;
float red, green, blue;

void (*draw) (gamgi_text *); } gamgi_text_class;

typedef struct {
gamgi_object object; } gamgi_orbital_class;

typedef struct {
gamgi_object object;

void (*draw) (gamgi_bond *);
float size;

double lower;
double upper;

gamgi_bool color;
float red, green, blue;

int element1;
int element2;
double *min;
double *max; } gamgi_bond_class;

typedef struct {
gamgi_object object;

/********************************
 * default values for all atoms *
 ********************************/

void (*draw) (gamgi_atom *);
double temperature; 
float variancy;
float size; 

/*************************************************
 * parameters affecting all atoms simultaneously *
 *************************************************/

int width;
int slices;
double min; 

/****************************************
 * arrays with default mass, radius,    *
 * R,G,B values for each atomic element *
 ****************************************/

int element;
double *mass;
double *radius;
float *red;
float *green;
float *blue; } gamgi_atom_class;

typedef struct {
gamgi_object object;
float red, green, blue; } gamgi_direction_class;

typedef struct {
gamgi_object object;
float red, green, blue; } gamgi_plane_class;

typedef struct {
gamgi_object object; } gamgi_group_class;

typedef struct {
gamgi_object object; } gamgi_molecule_class;

typedef struct {
gamgi_object object; } gamgi_cluster_class;

typedef struct {
gamgi_object object;

/*****************************************************************
 * default parameters: model, axes, borders, faces, nodes, color *
 *****************************************************************/

gamgi_enum model, borders;
gamgi_bool axes, faces, nodes;
float red, green, blue;

/********************************************************************
 * global parameters: axes size, width to render cell lines, points *
 ********************************************************************/

float axes_size;
int borders_width, nodes_width; } gamgi_cell_class;

typedef struct {
gamgi_object object; } gamgi_arrow_class;

typedef struct {
gamgi_object object; } gamgi_shape_class;

typedef struct {
gamgi_object object; } gamgi_graph_class;

typedef struct {
gamgi_object object; } gamgi_assembly_class;

typedef struct {
gamgi_object object;
float shininess;
float ambient[4];
float diffuse[4];
float specular[4]; } gamgi_light_class;
 
typedef struct {
gamgi_object object;

int undo;
int save;

gamgi_bool perspective;
double top;
double near;
double far;

double eye[3];
double center[3];
double up[3]; } gamgi_layer_class;

typedef struct {
gamgi_object object;

double rotate_max;
double rotate_step;
double move_max;
double move_step;
double scale_max;
double scale_step; } gamgi_window_class;

typedef struct {
gamgi_object object;

gamgi_bool beep;
int tolerance;
int button1;
int button2;
int button3;

int length;
int angle;
int charge;
int mass;

float background[3];
float foreground[3];
float title[3];
float bold[3];
float link[3]; } gamgi_gamgi_class;

typedef struct {

/******************************
 * external resources:        *
 * - launching window         *
 * - screen width and height  *
 * - hardware color depth     *
 * - lights allowed per layer *
 * - local time               *
 *******************************/

int width, height;
int r, g, b;
int max_lights;
int timer_time;

/*****************************
 * internal resources:       *
 * - message dialog address  *
 * - question dialog address *
 *                           *
 * hash table to speed up    *
 * atom element recognition  *
 *                           *
 * text glyph information    *
 *****************************/

GtkWidget *message;
GtkWidget *question;

gamgi_slist **element;

int **glyphs;
int *n_glyphs;

/***************************
 * selection tools:        *
 *                         *
 * selection color         *
 * selection function      *
 * Pick selection window   *
 * Region selection window *
 * Region selection state  *
 * Region selection box    *
 ***************************/

unsigned char rgb[3];
gamgi_function2 function;
gamgi_window *window_dialog;
gamgi_window *window_mouse;
gamgi_enum state;
int box[4];

/*****************************
 * lists of selected objects *
 *****************************/

gamgi_dlist *windows;
gamgi_dlist *layers;
gamgi_dlist *lights;
gamgi_dlist *assemblys;
gamgi_dlist *graphs;
gamgi_dlist *shapes;
gamgi_dlist *arrows;
gamgi_dlist *cells;
gamgi_dlist *clusters;
gamgi_dlist *molecules;
gamgi_dlist *groups;
gamgi_dlist *planes;
gamgi_dlist *directions;
gamgi_dlist *atoms;
gamgi_dlist *bonds;
gamgi_dlist *orbitals;
gamgi_dlist *texts;

/**********************************************************************
 * focus (what will rotate,move,scale interactively), can be given    *
 * locally to a single object (indicated by window->focus) or         *
 * globally to a list of selected objects (indicated by gamgi->focus) *
 **********************************************************************/

gamgi_enum focus;

/**********************************
 * pointers to configuration data *
 **********************************/

gamgi_gamgi_class *gamgi;
gamgi_window_class *window;
gamgi_layer_class *layer;
gamgi_light_class *light;
gamgi_assembly_class *assembly;
gamgi_graph_class *graph;
gamgi_shape_class *shape;
gamgi_arrow_class *arrow;
gamgi_cell_class *cell;
gamgi_cluster_class *cluster;
gamgi_molecule_class *molecule;
gamgi_group_class *group;
gamgi_plane_class *plane;
gamgi_direction_class *direction;
gamgi_atom_class *atom;
gamgi_bond_class *bond;
gamgi_orbital_class *orbital;
gamgi_text_class *text;
gamgi_help_class *help;

/***************************
 * pointers to object data *
 ***************************/

gamgi_text *text_start;
gamgi_text *text_end;
gamgi_orbital *orbital_start;
gamgi_orbital *orbital_end;
gamgi_bond *bond_start;
gamgi_bond *bond_end;
gamgi_atom *atom_start;
gamgi_atom *atom_end;
gamgi_direction *direction_start;
gamgi_direction *direction_end;
gamgi_plane *plane_start;
gamgi_plane *plane_end;
gamgi_group *group_start;
gamgi_group *group_end;
gamgi_molecule *molecule_start;
gamgi_molecule *molecule_end;
gamgi_cluster *cluster_start;
gamgi_cluster *cluster_end;
gamgi_cell *cell_start;
gamgi_cell *cell_end;
gamgi_arrow *arrow_start;
gamgi_arrow *arrow_end;
gamgi_shape *shape_start;
gamgi_shape *shape_end;
gamgi_graph *graph_start;
gamgi_graph *graph_end;
gamgi_assembly *assembly_start;
gamgi_assembly *assembly_end;
gamgi_light *light_start;
gamgi_light *light_end;
gamgi_layer *layer_start; 
gamgi_layer *layer_end; 
gamgi_window *window_start;
gamgi_window *window_end; } gamgi_gamgi;

/**************** gamgi *******************
 * this pointer, with the name of the     *
 * program, is the only external variable *
 * in GAMGI and contains the resources    *
 * available for the entire application.  *
 ******************************************/

extern gamgi_gamgi *gamgi;

#endif
