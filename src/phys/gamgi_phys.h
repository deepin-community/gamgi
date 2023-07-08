/*******************************************
 *
 * $GAMGI/src/phys/gamgi_phys.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_PHYS
#define GAMGI_PHYS

/********** enumerations **********
 **********************************/

/************************************************************
 * All GAMGI enumerations start at 1 and share FALSE for 0. *
 ************************************************************/

enum { GAMGI_PHYS_TRICLINIC_P = 1, GAMGI_PHYS_MONOCLINIC_P,
GAMGI_PHYS_MONOCLINIC_C, GAMGI_PHYS_ORTHORHOMBIC_P, GAMGI_PHYS_ORTHORHOMBIC_I,
GAMGI_PHYS_ORTHORHOMBIC_F, GAMGI_PHYS_ORTHORHOMBIC_C, GAMGI_PHYS_TETRAGONAL_P,
GAMGI_PHYS_TETRAGONAL_I, GAMGI_PHYS_HEXAGONAL_P, GAMGI_PHYS_HEXAGONAL_R,
GAMGI_PHYS_CUBIC_P, GAMGI_PHYS_CUBIC_I, GAMGI_PHYS_CUBIC_F };

enum { GAMGI_PHYS_LATTICE_P = 1, GAMGI_PHYS_LATTICE_I, GAMGI_PHYS_LATTICE_A, 
GAMGI_PHYS_LATTICE_B, GAMGI_PHYS_LATTICE_C, GAMGI_PHYS_LATTICE_R };

enum { GAMGI_PHYS_CONVENTIONAL = 1, GAMGI_PHYS_PRIMITIVE, GAMGI_PHYS_WIGNER, 
GAMGI_PHYS_PARALLELEPIPED, GAMGI_PHYS_SPHERE, GAMGI_PHYS_PROJECTION };

enum { GAMGI_PHYS_POLE = 1, GAMGI_PHYS_TRACE, GAMGI_PHYS_POLYGON,
GAMGI_PHYS_LINE, GAMGI_PHYS_VECTOR };

enum { GAMGI_PHYS_ALL = 1, GAMGI_PHYS_FACES, GAMGI_PHYS_EDGES, 
GAMGI_PHYS_NONE };

enum { GAMGI_PHYS_CUTOFF_OBJECTS = 1, GAMGI_PHYS_CUTOFF_NODES };

enum { GAMGI_PHYS_WULFF = 1, GAMGI_PHYS_SCHMIDT };

enum { GAMGI_PHYS_011 = 1, GAMGI_PHYS_101, GAMGI_PHYS_110,
GAMGI_PHYS_111, GAMGI_PHYS_122, GAMGI_PHYS_211 };

/************* macros ************
 *********************************/

/******************************************************
 * Crystalographic systems: triclinic/anorthic,       *
 * monoclinic, orthorhombic, tetragonal,              *
 * hexagonal/trigonal/rombohedric, cubic              *
 *                                                    *
 * Crystalographic lattices: primitive, body-centerd, *
 * face-centered, base-centered, rombohedric          *
 *                                                    *
 * Crystalographic groups: 73 symmorphic              *
 * plus 157 non-symorphic space groups                *
 ******************************************************/

#define GAMGI_PHYS_CELL_SYSTEMS 6
#define GAMGI_PHYS_CELL_LATTICES 5
#define GAMGI_PHYS_CELL_GROUPS 230

/************************************
 * default cell origin, orientation *
 ************************************/

#define GAMGI_PHYS_CELL_X 0.0
#define GAMGI_PHYS_CELL_Y 0.0
#define GAMGI_PHYS_CELL_Z 0.0

#define GAMGI_PHYS_CELL_E1 0.0
#define GAMGI_PHYS_CELL_E2 0.0
#define GAMGI_PHYS_CELL_E3 0.0

/***************************
 * default number of cells *
 *   default origin node   *
 ***************************/

#define GAMGI_PHYS_CELL_N1 1
#define GAMGI_PHYS_CELL_N2 1
#define GAMGI_PHYS_CELL_N3 1

#define GAMGI_PHYS_CELL_O1 0
#define GAMGI_PHYS_CELL_O2 0
#define GAMGI_PHYS_CELL_O3 0

/*******************************************************
 * default for cell model, axes, borders, faces, nodes *
 *******************************************************/

#define GAMGI_PHYS_CELL_MODEL   GAMGI_PHYS_CONVENTIONAL
#define GAMGI_PHYS_CELL_AXES    FALSE
#define GAMGI_PHYS_CELL_BORDERS GAMGI_PHYS_ALL
#define GAMGI_PHYS_CELL_FACES   FALSE
#define GAMGI_PHYS_CELL_NODES   TRUE

/*******************************
 * default Wyckoff coordinates *
 *******************************/

#define GAMGI_PHYS_NODE_X 0.0
#define GAMGI_PHYS_NODE_Y 0.0
#define GAMGI_PHYS_NODE_Z 0.0

/****************************************
 * default node shift coordinates, axis *
 ****************************************/
 
#define GAMGI_PHYS_SHIFT_X 0.0
#define GAMGI_PHYS_SHIFT_Y 0.0
#define GAMGI_PHYS_SHIFT_Z 0.0

#define GAMGI_PHYS_SHIFT_U 0
#define GAMGI_PHYS_SHIFT_V 0
#define GAMGI_PHYS_SHIFT_W 1

/**************************************
 * Voronoi tesselation:               *
 * 1) array block size                *
 * 2) minimum cell width              *
 * 3) border offset                   *
 * 4) maximum tesselation variancy    *
 * 5) minimum polygonal area          *
 * 6) apex hiding tolerance           *
 * 7) vertices minimum distance: must *
 * be smaller than half the cell size *
 * 8) maximum tesselation tolerance   *
 **************************************/
   
#define GAMGI_PHYS_VORONOI_ARRAY 50
#define GAMGI_PHYS_VORONOI_WIDTH 1.0
#define GAMGI_PHYS_VORONOI_BORDER 1.0
#define GAMGI_PHYS_VORONOI_VARIANCY 10.0
#define GAMGI_PHYS_VORONOI_AREA 1.0E-10
#define GAMGI_PHYS_VORONOI_APEX 1.0E-10
#define GAMGI_PHYS_VORONOI_VERTEX 1.0E-4
#define GAMGI_PHYS_VORONOI_TOLERANCE 1.0E-2

/****************************************
 * Random Close Packing:                *
 * default time relaxation              *
 * default random number generator seed *
 ****************************************/

#define GAMGI_PHYS_RCP_ATOMS 10
#define GAMGI_PHYS_RCP_TOLERANCE 1.0E-3
#define GAMGI_PHYS_RCP_TIME 1.0E+3
#define GAMGI_PHYS_RCP_TIME_MIN 1.0E+2
#define GAMGI_PHYS_RCP_TIME_MAX 1.0E+5
#define GAMGI_PHYS_RCP_SEED 1

/************************************************************
 * Tolerance used when looking for equal Wyckoff positions, *
 * when generating structures from space group information  *
 ************************************************************/

#define GAMGI_PHYS_WYCKOFF_TOLERANCE 1.0E-2

/************************************
 * Reciprocal lattice constant:     *
 * Minimum and default fixed values *
 * Fixed scaling or Bravais lattice *
 ************************************/

#define GAMGI_PHYS_RECIPROCAL_MIN     0.1
#define GAMGI_PHYS_RECIPROCAL_DEFAULT 6.2831853
#define GAMGI_PHYS_RECIPROCAL_BRAVAIS FALSE

/****************************************
 * Default origin and axes cell vectors *
 ****************************************/

#define GAMGI_PHYS_CELL_VECTORS_ORIGIN GAMGI_PHYS_CONVENTIONAL
#define GAMGI_PHYS_CELL_VECTORS_AXES   GAMGI_PHYS_CONVENTIONAL

/****************************************************
 * Default crystallographic Plane,Direction vectors *
 ****************************************************/

#define GAMGI_PHYS_PLANE_VECTORS     GAMGI_PHYS_CONVENTIONAL
#define GAMGI_PHYS_DIRECTION_VECTORS GAMGI_PHYS_CONVENTIONAL

/***********************************
 * Default range of planes used    *
 * to represent a family of        *
 * crystallographic planes         *
 *                                 *
 * Default node used to pass a     *
 * direction belonging to a family *
 * of crystallographic directions  *
 ***********************************/

#define GAMGI_PHYS_PLANE_RANGE_START 1
#define GAMGI_PHYS_PLANE_RANGE_END   1

#define GAMGI_PHYS_NODE_O1 0
#define GAMGI_PHYS_NODE_O2 0
#define GAMGI_PHYS_NODE_O3 0

/*************************************************
 * Default range of Directions used to represent *
 * a family of crystallographic Directions       *
 *************************************************/

#define GAMGI_PHYS_DIRECTIONS_FROM 0
#define GAMGI_PHYS_DIRECTIONS_TO   0

/************************************
 * Default stereographic projection *
 ************************************/

#define GAMGI_PHYS_PROJECTION_NET gamgi_phys_projection_wulff

/****************************************
 * Default alignment directions for     *
 * epitaxial crystallographic relations *
 * in Cell Create and Cell Modify       *
 ****************************************/

#define GAMGI_PHYS_ALIGN_H 0
#define GAMGI_PHYS_ALIGN_K 0
#define GAMGI_PHYS_ALIGN_L 1

#define GAMGI_PHYS_ALIGN_U 1
#define GAMGI_PHYS_ALIGN_V 0
#define GAMGI_PHYS_ALIGN_W 0

/***********************
 * default atom origin *
 ***********************/

#define GAMGI_PHYS_ATOM_X 0.0
#define GAMGI_PHYS_ATOM_Y 0.0
#define GAMGI_PHYS_ATOM_Z 0.0

#endif
