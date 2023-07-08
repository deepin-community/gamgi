/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_MESA
#define GAMGI_MESA


typedef void (*gamgi_tesselation) ();
typedef void (*gamgi_net) (double *, double *);

/********** enumerations **********
 **********************************/

/************************************************************
 * All GAMGI enumerations start at 1 and share FALSE for 0. *
 ************************************************************/

/*********
 * style *
 *********/

enum { GAMGI_MESA_WIRED = 1, GAMGI_MESA_SOLID };

enum { GAMGI_MESA_PERSPECTIVE = 1, GAMGI_MESA_ORTHOGRAPHIC };

/*********************************
 * OpenGL pre-compilled lists,   *
 * used to draw atoms and bonds. *
 * These static lists must be    *
 * created BEFORE dynamic lists, *
 * to avoid list number overlap! *
 *********************************/

enum { GAMGI_MESA_CROSS = 1, GAMGI_MESA_SPHERE,
GAMGI_MESA_LINE, GAMGI_MESA_CYLINDER_1, GAMGI_MESA_CYLINDER_3,
GAMGI_MESA_CYLINDER_5, GAMGI_MESA_AXES };

/******************
 * types of light *
 ******************/

enum { GAMGI_MESA_DIRECTIONAL = 1, GAMGI_MESA_POSITIONAL, GAMGI_MESA_SPOT };

/***************************************************
 * types of mask: used to represent planes (cross) *
 * in stereographic projection/reciprocal lattice  *
 * and directions (chess) stereographic projection *
 ***************************************************/

enum { GAMGI_MESA_MASK_CROSS = 1, GAMGI_MESA_MASK_CHESS };

enum { GAMGI_MESA_SELECT_READY = 1, GAMGI_MESA_SELECT_SIZE,
GAMGI_MESA_SELECT_DONE, GAMGI_MESA_SELECT_MOVE,
GAMGI_MESA_SELECT_RESIZE };

/************* macros ************
 *********************************/

/**************************
 * pick mouse paramateres *
 **************************/

#define GAMGI_MESA_PICK_TOLERANCE 3
#define GAMGI_MESA_PICK_TOLERANCE_MAX 5

/******************************************
 * default scaling factor for all objects *
 *                                        *
 * minimum,maxium scale values: values in *
 * the range E+4 or E-4 can crash GAMGI   *
 ******************************************/

#define GAMGI_MESA_SCALE 1.0
#define GAMGI_MESA_SCALE_LOWER 1.0E-3
#define GAMGI_MESA_SCALE_UPPER 1.0E+3

/********************************
 * drag method mouse parameters *
 ********************************/

#define GAMGI_MESA_ROTATE_SPEED   0.8
#define GAMGI_MESA_MOVE_SPEED    40.0
#define GAMGI_MESA_SCALE_SPEED   10.0

/***************************************************
 * width used to draw wired and solid polyhedrons, *
 * such as Voronoi clusters and Polytope groups    *
 ***************************************************/

#define GAMGI_MESA_POLYHEDRON_WIDTH 1.0

/******************************
 * layer modelview parameters *
 ******************************/

#define GAMGI_MESA_LAYER_EYEX 0.0
#define GAMGI_MESA_LAYER_EYEY 0.0
#define GAMGI_MESA_LAYER_EYEZ 10.0
#define GAMGI_MESA_LAYER_CENTERX 0.0
#define GAMGI_MESA_LAYER_CENTERY 0.0
#define GAMGI_MESA_LAYER_CENTERZ 0.0
#define GAMGI_MESA_LAYER_UPX 0.0
#define GAMGI_MESA_LAYER_UPY 1.0
#define GAMGI_MESA_LAYER_UPZ 0.0

/*******************************
 * layer projection parameters *
 *******************************/

#define GAMGI_MESA_LAYER_PERSPECTIVE TRUE
#define GAMGI_MESA_LAYER_TOP 0.5
#define GAMGI_MESA_LAYER_NEAR 1.0
#define GAMGI_MESA_LAYER_FAR 1000.0

/*****************************
 * minimum,maxium top values *
 *****************************/

#define GAMGI_MESA_TOP_MIN 1.0E-5
#define GAMGI_MESA_TOP_MAX 1.0E+5

/***************************************************
 * transitions from perspective to orthographic    *
 * projection and vice-versa should be as smooth   *
 * as possible, therefore objects should have      *
 * almost the same size in both projections. This  *
 * correction parameter is used to guarantee that. *
 ***************************************************/

#define GAMGI_MESA_LAYER_CORRECTION 10.0

/***************************
 * blending factor used to *
 * draw translucid layers  *
 ***************************/

#define GAMGI_MESA_LAYER_BLEND 0.25

/**************************************************************
 * the rectangle used to select visible objects by region:    *
 * the size of the move, resize little squares at the corners *
 * the blending factor used outside the selection rectangle   *
 **************************************************************/

#define GAMGI_MESA_SELECT_SQUARE  20
#define GAMGI_MESA_SELECT_BLEND 0.50

/***********************
 * default layer color *
 ***********************/

#define GAMGI_MESA_LAYER_R 0.0
#define GAMGI_MESA_LAYER_G 0.0
#define GAMGI_MESA_LAYER_B 0.0

/***********************************
 * light default OpenGL parameters *
 ***********************************/

#define GAMGI_MESA_LIGHT_DIRECTION_X 0.0
#define GAMGI_MESA_LIGHT_DIRECTION_Y 0.0
#define GAMGI_MESA_LIGHT_DIRECTION_Z -1.0

#define GAMGI_MESA_LIGHT_ANGLE 90.0

#define GAMGI_MESA_LIGHT_CONSTANT 1.0
#define GAMGI_MESA_LIGHT_LINEAR 0.0
#define GAMGI_MESA_LIGHT_QUADRATIC 0.0
#define GAMGI_MESA_LIGHT_RADIAL 0.0

/**********************************
 * Light default GAMGI parameters *
 **********************************/

#define GAMGI_MESA_LIGHT_POSITION_X 0.0
#define GAMGI_MESA_LIGHT_POSITION_Y 0.0
#define GAMGI_MESA_LIGHT_POSITION_Z 10.0

#define GAMGI_MESA_LIGHT_SHININESS 0.1

#define GAMGI_MESA_LIGHT_AMBIENT_R 0.5
#define GAMGI_MESA_LIGHT_AMBIENT_G 0.5
#define GAMGI_MESA_LIGHT_AMBIENT_B 0.5

#define GAMGI_MESA_LIGHT_DIFFUSE_R 0.4
#define GAMGI_MESA_LIGHT_DIFFUSE_G 0.4
#define GAMGI_MESA_LIGHT_DIFFUSE_B 0.4

#define GAMGI_MESA_LIGHT_SPECULAR_R 0.3
#define GAMGI_MESA_LIGHT_SPECULAR_G 0.3
#define GAMGI_MESA_LIGHT_SPECULAR_B 0.3

/************************************************************
 * line width used to draw borders in wired and solid cells *
 * point width used to draw nodes in wired and solid cells  *
 *                                                          *
 * number of slices used to draw maximum circles in:        *
 * 1) sterographic projection; 2) X-ray diffraction         *
 * pattern; 3) around spherical volumes in cells.           *
 ************************************************************/

#define GAMGI_MESA_CELL_LINE_MIN   1.0
#define GAMGI_MESA_CELL_LINE_MAX  10.0
#define GAMGI_MESA_CELL_LINE       1.0

#define GAMGI_MESA_CELL_POINT_MIN  1.0
#define GAMGI_MESA_CELL_POINT_MAX 10.0
#define GAMGI_MESA_CELL_POINT      3.0

#define GAMGI_MESA_CELL_SLICES 100

/**********************
 * default cell color *
 **********************/

#define GAMGI_MESA_CELL_R 1.0
#define GAMGI_MESA_CELL_G 1.0
#define GAMGI_MESA_CELL_B 1.0

/*****************************************
 *  primitive reciprocal lattice color   *
 *                                       *
 * this color should be clearly visible  *
 * with both white and black backgrounds *
 *****************************************/

#define GAMGI_MESA_CELL_RECIPROCAL_R 0.5
#define GAMGI_MESA_CELL_RECIPROCAL_G 0.5
#define GAMGI_MESA_CELL_RECIPROCAL_B 1.0

/**************************************
 * default color for Voronoi Clusters *
 **************************************/

#define GAMGI_MESA_CLUSTER_R 1.0
#define GAMGI_MESA_CLUSTER_G 1.0
#define GAMGI_MESA_CLUSTER_B 1.0

/**************************************
 * default color for Polytope Groups *
 **************************************/

#define GAMGI_MESA_GROUP_R 0.0
#define GAMGI_MESA_GROUP_G 1.0
#define GAMGI_MESA_GROUP_B 1.0

/*********************************************************
 * POLE: default width used to render Plane, Direction   *
 * objects as cross, chess bitmap masks in Stereographic *
 * Projection and Reciprocal Lattice                     *
 *                                                       *
 * TRACE: default width used to render Plane, Direction  *
 * objects as arcs in Stereographic Projection           *
 *********************************************************/

#define GAMGI_MESA_POLE_WIDTH  3
#define GAMGI_MESA_TRACE_WIDTH 1

/********************************************************************
 * In stereographic projections, increase slightly the z coordinate *
 * in poles (crosss and chess bitmaps) so these appear above traces *
 ********************************************************************/

#define GAMGI_MESA_OFFSET_BITMAP 1E-4

/****************************************
 * number of slices used to draw plane  *
 * traces in Wullf, Schmidt projections *
 * and plane polygons in sphere volumes *
 ****************************************/

#define GAMGI_MESA_PLANE_SLICES 100

/***********************
 * default plane color *
 ***********************/

#define GAMGI_MESA_PLANE_R 1.0
#define GAMGI_MESA_PLANE_G 1.0
#define GAMGI_MESA_PLANE_B 0.0

/***************************
 * default direction color *
 ***************************/

#define GAMGI_MESA_DIRECTION_R 0.0
#define GAMGI_MESA_DIRECTION_G 1.0
#define GAMGI_MESA_DIRECTION_B 1.0

/*******************************************************
 * minimum, maximum and default attributes controlling *
 * atom and bond rendering quality (and drawing speed) *
 *******************************************************/

#define GAMGI_MESA_ATOM_WIDTH_MIN 1
#define GAMGI_MESA_ATOM_WIDTH_MAX 5
#define GAMGI_MESA_ATOM_WIDTH 2

#define GAMGI_MESA_ATOM_SLICES_MIN 4
#define GAMGI_MESA_ATOM_SLICES_MAX 100
#define GAMGI_MESA_ATOM_SLICES 16

/*****************************************
 *   Default drawing options for atoms.  *
 *                                       *
 *          GAMGI_MESA_ATOM_DRAW:        *
 * option 0: gamgi_mesa_draw_atom_cross  *
 * option 1: gamgi_mesa_draw_atom_sphere *
 *****************************************/

#define GAMGI_MESA_ATOM_DRAW gamgi_mesa_atom_draw_cross

/*******************************************
 *  Default drawing options for bonds.     *
 *                                         *
 *         GAMGI_MESA_BOND_DRAW:           *
 * option 1: gamgi_mesa_draw_bond_line     *
 * option 2: gamgi_mesa_draw_bond_cylinder *
 *                                         *
 *         GAMGI_MESA_BOND_COLOR:          *
 * false: draw bonds with inherited colors *
 * true: draw bonds with their own color   *
 *******************************************/

#define GAMGI_MESA_BOND_DRAW gamgi_mesa_bond_draw_line
#define GAMGI_MESA_BOND_COLOR FALSE

/**********************
 * default bond color *
 **********************/

#define GAMGI_MESA_BOND_R 0.0
#define GAMGI_MESA_BOND_G 0.8
#define GAMGI_MESA_BOND_B 0.0

/**************************
 * default orbital colors *
 **************************/

#define GAMGI_MESA_ORBITAL_BASE_R 1.0
#define GAMGI_MESA_ORBITAL_BASE_G 0.5
#define GAMGI_MESA_ORBITAL_BASE_B 0.0

#define GAMGI_MESA_ORBITAL_PHASE_R 0.5
#define GAMGI_MESA_ORBITAL_PHASE_G 0.5
#define GAMGI_MESA_ORBITAL_PHASE_B 1.0

#define GAMGI_MESA_ORBITAL_FRAME_R 1.0
#define GAMGI_MESA_ORBITAL_FRAME_G 1.0
#define GAMGI_MESA_ORBITAL_FRAME_B 1.0

/*************************
 * default orbital style *
 *************************/

#define GAMGI_MESA_ORBITAL_STYLE GAMGI_MESA_WIRED

/**************************************************************************
 * Width recommended by Plotutils (the GNU package) to render western     *
 * Hershey fonts. They say:                                               *
 *                                                                        *
 * Our choice for stroke width, in terms of virtual pixels. This number   *
 * is magic: just slightly greater than sqrt(2), so that adjacent strokes *
 * that are inclined at a 45 degree angle will overlap as they should.    *
 *                                                                        *
 * (According to ``Calligraphy for Computers'', the Hershey fonts were    *
 * designed to be drawn by an electron beam the intensity of which fell   *
 * to 50% at a transverse displacement of one unit.)                      *
 **************************************************************************/

#define GAMGI_MESA_STROKED_WIDTH 1.42

/***************************************************
 * fonts are usually designed in a large scale, to *
 * decrease errors, thus they must be scaled down  *
 ***************************************************/

#define GAMGI_MESA_STROKED_SCALE 140.0
#define GAMGI_MESA_EXTRUDED_SCALE 1.0

/*********************************
 * parameters for extruded fonts *
 *********************************/

#define GAMGI_MESA_EXTRUDED_START 100
#define GAMGI_MESA_EXTRUDED_STEP 10
#define GAMGI_MESA_EXTRUDED_DEPTH 0.45
#define GAMGI_MESA_EXTRUDED_STEPS 4
#define GAMGI_MESA_EXTRUDED_TOLERANCE 2.0E-6

/**********************
 * default text style *
 **********************/

#define GAMGI_MESA_TEXT_STYLE GAMGI_MESA_WIRED

/***********************
 * default text origin *
 ***********************/

#define GAMGI_MESA_TEXT_X 0.0
#define GAMGI_MESA_TEXT_Y 0.0
#define GAMGI_MESA_TEXT_Z 0.0

/****************************
 * default text orientation *
 ****************************/

#define GAMGI_MESA_TEXT_E1 0.0
#define GAMGI_MESA_TEXT_E2 0.0
#define GAMGI_MESA_TEXT_E3 0.0

/**********************
 * default text color *
 **********************/

#define GAMGI_MESA_TEXT_R 0.0
#define GAMGI_MESA_TEXT_G 1.0
#define GAMGI_MESA_TEXT_B 0.0

/**************************************************
 * layer,cell,orbital axes: red,green,blue colors *
 **************************************************/

#define GAMGI_MESA_AXIS_1_R 1.0
#define GAMGI_MESA_AXIS_1_G 0.0
#define GAMGI_MESA_AXIS_1_B 0.0

#define GAMGI_MESA_AXIS_2_R 0.0
#define GAMGI_MESA_AXIS_2_G 1.0
#define GAMGI_MESA_AXIS_2_B 0.0

#define GAMGI_MESA_AXIS_3_R 0.0
#define GAMGI_MESA_AXIS_3_G 0.0
#define GAMGI_MESA_AXIS_3_B 1.0

/*************************************************
 * layer,cell,orbital axes size: default,min,max *
 *************************************************/

#define GAMGI_MESA_AXES_SIZE 0.03
#define GAMGI_MESA_AXES_MIN  0.01
#define GAMGI_MESA_AXES_MAX  0.10

/**********************************
 * object weight, to determine    *
 * the center of a set of objects *
 * (used in gamgi_mesa_center.c)  *
 **********************************/

#define GAMGI_MESA_TEXT_WEIGHT      5
#define GAMGI_MESA_ORBITAL_WEIGHT   3
#define GAMGI_MESA_ATOM_WEIGHT      1
#define GAMGI_MESA_DIRECTION_WEIGHT 2
#define GAMGI_MESA_PLANE_WEIGHT     3
#define GAMGI_MESA_CELL_WEIGHT     10
#define GAMGI_MESA_LAYER_WEIGHT     0

#endif
