/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#ifndef GAMGI_CHEM
#define GAMGI_CHEM

/********** types *********
 **************************/

/*********************************************************************
 * These types define pointers to radial and angular H wavefunctions *
 *********************************************************************/

typedef double (*gamgi_radial) (double r);
typedef double (*gamgi_angular) (double x, double y, double z, double r);

/********** enumerations **********
 **********************************/

/**************************************************************
 * All external enumerations start at 1 and share FALSE for 0 *
 **************************************************************/

enum { GAMGI_CHEM_RADIUS = 1, GAMGI_CHEM_UNIT, GAMGI_CHEM_BOHR,
GAMGI_CHEM_NONE };

/*************************
 * graphene enumerations *
 *************************/

enum { GAMGI_CHEM_CONE_0, GAMGI_CHEM_CONE_60_D, GAMGI_CHEM_CONE_60_O,
GAMGI_CHEM_CONE_120_E, GAMGI_CHEM_CONE_120_H, GAMGI_CHEM_CONE_120_S,
GAMGI_CHEM_CONE_120_O, GAMGI_CHEM_CONE_180_D, GAMGI_CHEM_CONE_180_S,
GAMGI_CHEM_CONE_180_O, GAMGI_CHEM_CONE_240_E, GAMGI_CHEM_CONE_240_H,
GAMGI_CHEM_CONE_240_P, GAMGI_CHEM_CONE_240_A, GAMGI_CHEM_CONE_240_O,
GAMGI_CHEM_CONE_300_E, GAMGI_CHEM_CONE_300_O };

enum { GAMGI_CHEM_GRAPHENE_CAGE_5_0, GAMGI_CHEM_GRAPHENE_CAGE_5_5,
GAMGI_CHEM_GRAPHENE_CAGE_6_6_S, GAMGI_CHEM_GRAPHENE_CAGE_6_6_C };

enum { GAMGI_CHEM_GRAPHENE_CAP_5_0, GAMGI_CHEM_GRAPHENE_CAP_5_5,
GAMGI_CHEM_GRAPHENE_CAP_6_6_S, GAMGI_CHEM_GRAPHENE_CAP_6_6_C };

/************* macros ************
 *********************************/

/***************************************
 * Dummy atom (Z=0) default properties *
 **************************************/

#define GAMGI_CHEM_DU 0

#define GAMGI_CHEM_DU_MASS 1.0
#define GAMGI_CHEM_DU_RADIUS 1.0

#define GAMGI_CHEM_DU_R 0.864
#define GAMGI_CHEM_DU_G 0.864
#define GAMGI_CHEM_DU_B 0.864

enum { GAMGI_CHEM_H = 1, GAMGI_CHEM_HE, GAMGI_CHEM_LI, GAMGI_CHEM_BE, 
GAMGI_CHEM_B, GAMGI_CHEM_C, GAMGI_CHEM_N, GAMGI_CHEM_O, GAMGI_CHEM_F, 
GAMGI_CHEM_NE, GAMGI_CHEM_NA, GAMGI_CHEM_MG, GAMGI_CHEM_AL, GAMGI_CHEM_SI, 
GAMGI_CHEM_P, GAMGI_CHEM_S, GAMGI_CHEM_CL, GAMGI_CHEM_AR, GAMGI_CHEM_K, 
GAMGI_CHEM_CA, GAMGI_CHEM_SC, GAMGI_CHEM_TI, GAMGI_CHEM_V, GAMGI_CHEM_CR, 
GAMGI_CHEM_MN, GAMGI_CHEM_FE, GAMGI_CHEM_CO, GAMGI_CHEM_NI, GAMGI_CHEM_CU, 
GAMGI_CHEM_ZN, GAMGI_CHEM_GA, GAMGI_CHEM_GE, GAMGI_CHEM_AS, GAMGI_CHEM_SE, 
GAMGI_CHEM_BR, GAMGI_CHEM_KR, GAMGI_CHEM_RB, GAMGI_CHEM_SR, GAMGI_CHEM_Y, 
GAMGI_CHEM_ZR, GAMGI_CHEM_NB, GAMGI_CHEM_MO, GAMGI_CHEM_TC, GAMGI_CHEM_RU, 
GAMGI_CHEM_RH, GAMGI_CHEM_PD, GAMGI_CHEM_AG, GAMGI_CHEM_CD, GAMGI_CHEM_IN, 
GAMGI_CHEM_SN, GAMGI_CHEM_SB, GAMGI_CHEM_TE, GAMGI_CHEM_I, GAMGI_CHEM_XE, 
GAMGI_CHEM_CS, GAMGI_CHEM_BA, GAMGI_CHEM_LA, GAMGI_CHEM_CE, GAMGI_CHEM_PR, 
GAMGI_CHEM_ND, GAMGI_CHEM_PM, GAMGI_CHEM_SM, GAMGI_CHEM_EU, GAMGI_CHEM_GD, 
GAMGI_CHEM_TB, GAMGI_CHEM_DY, GAMGI_CHEM_HO, GAMGI_CHEM_ER, GAMGI_CHEM_TM, 
GAMGI_CHEM_YB, GAMGI_CHEM_LU, GAMGI_CHEM_HF, GAMGI_CHEM_TA, GAMGI_CHEM_W, 
GAMGI_CHEM_RE, GAMGI_CHEM_OS, GAMGI_CHEM_IR, GAMGI_CHEM_PT, GAMGI_CHEM_AU, 
GAMGI_CHEM_HG, GAMGI_CHEM_TL, GAMGI_CHEM_PB, GAMGI_CHEM_BI, GAMGI_CHEM_PO, 
GAMGI_CHEM_AT, GAMGI_CHEM_RN, GAMGI_CHEM_FR, GAMGI_CHEM_RA, GAMGI_CHEM_AC, 
GAMGI_CHEM_TH, GAMGI_CHEM_PA, GAMGI_CHEM_U, GAMGI_CHEM_NP, GAMGI_CHEM_PU, 
GAMGI_CHEM_AM, GAMGI_CHEM_CM, GAMGI_CHEM_BK, GAMGI_CHEM_CF, GAMGI_CHEM_ES, 
GAMGI_CHEM_FM, GAMGI_CHEM_MD, GAMGI_CHEM_NO, GAMGI_CHEM_LR, GAMGI_CHEM_RF, 
GAMGI_CHEM_DB, GAMGI_CHEM_SG, GAMGI_CHEM_BH, GAMGI_CHEM_HS, GAMGI_CHEM_MT, 
GAMGI_CHEM_DS, GAMGI_CHEM_RG };

enum { GAMGI_CHEM_CONTAINER = 1, GAMGI_CHEM_POLYTOPE };

enum { GAMGI_CHEM_STRUCTURE_COPY = 1, GAMGI_CHEM_STRUCTURE_LINK,
GAMGI_CHEM_STRUCTURE_NONE };

enum { GAMGI_CHEM_FILTER_CLASS = 1, GAMGI_CHEM_FILTER_PARENT,
GAMGI_CHEM_FILTER_IDENTICAL, GAMGI_CHEM_FILTER_NONE };

/******************************************
 * default policies for structure, filter *
 ******************************************/

#define GAMGI_CHEM_STRUCTURE GAMGI_CHEM_STRUCTURE_COPY
#define GAMGI_CHEM_FILTER GAMGI_CHEM_FILTER_CLASS

/*******************************************
 *         number of atomic elements       *
 *                                         *
 * Although GAMGI is currently recognizing *
 * only elements until Rg[111], therefore  *
 * with 2 char names, the source code is   *
 * ready to accept elements with 3 char    *
 * names such as UUo[118] (the periodic    *
 * table will look much larger though).    *
 *******************************************/

#define GAMGI_CHEM_ATOM_MAX 111

/**********************************************************************
 * maximum number of char in atom label, without the final '\0': it   *
 * should be smaller than GAMGI_ENGINE_TOKEN, where labels are stored *
 **********************************************************************/

#define GAMGI_CHEM_ATOM_LABEL 10

/***********************
 * Default atom charge *
 ***********************/

#define GAMGI_CHEM_ATOM_CHARGE 0.0

/*******************************************************
 * sum both radius and multiply by lower, upper factor *
 * to get automatic minimum and maximum bond distance  *
 *******************************************************/

#define GAMGI_CHEM_BOND_LOWER 0.80
#define GAMGI_CHEM_BOND_UPPER 1.10

/****************************************************
 * Hash multipliers: 31 (used by Kernighan), 37     *
 * (used in the Java library); Hash numbers: prime  *
 * numbers. Source: The Practice of Programming,    *
 * Brian Kernighan, Rob Pike, Addison-Wesley, 1999  *
 *                                                  *
 *  Minimum size and multiplier needed to produce   *
 *  lists with a maximum of 3, 2, 1 nodes, for the  *
 *  names of all chemical elements until Mt.        *
 *    (tested only for multiplier < 10,000)         *
 *                                                  *
 *       nodes=3 => size=100 multiplier=15          *
 *                                                  *
 *       nodes=2 => size=127 multiplier=37          *
 *                                                  *
 *       nodes=1 => size=430 multiplier=21          *
 ****************************************************/

#define GAMGI_CHEM_HASH_MULTIPLIER 37
#define GAMGI_CHEM_HASH_SIZE 127

/*************************************************
 * -ATOM_MIN should be equal to the smallest     *
 * atomic radius defined, usually the hydrogen   *
 * atom radius.                                  *
 *                                               *
 * -BOND_SIZE should be <= ATOM_SIZE:            *
 * 1: bond cylinders have exactly the ATOM_MIN   *
 * radius. BOND_SIZE should be smaller than      *
 * ATOM_SIZE, to guarantee that bond cylinders   *
 * are never large than atom spheres.            *
 *                                               *
 * cylinder radius = BOND_SIZE x ATOM_MIN        *
 *                                               *
 * -ATOM_SIZE should be >= BOND_SIZE:            *
 * 1: atom spheres have exactly the atomic       *
   radius, if ATOM_VARIANCY is equal to 1, or    *
   ATOM_MIN if ATOM_VARIANCY is equal to 0.      *
   Increasing ATOM_SIZE, atom spheres become     *
   larger.                                       *
 *                                               *
 * -ATOM_VARIANCY should be in the range [0, 1]: *
 * 0: all atom spheres have the same radius,     *
 * ATOM_MIN multiplied by ATOM_SIZE;             *
 * 1: atom spheres have the atomic radius        *
 * multiplied by ATOM_SIZE. Any ATOM_VARIANCY    *
 * value between 0 and 1 can be safely used, but *
 * the atom spheres radius will no longer become *
 * equal to atomic radius or ATOM_MIN, for       *
 * ATOM_SIZE = 1.                                *
 *                                               *
 * sphere radius = ATOM_SIZE x [ATOM_VARIANCY    * 
 * x radius + (1 - ATOM_VARIANCY) x ATOM_MIN]    *
 *                                               *
 * The sphere radius are equal to the atom       * 
 * radius only when ATOM_SIZE and ATOM_VARIANCY  *
 * are both equal to 1.0.                        *
 *************************************************/

#define GAMGI_CHEM_ATOM_VARIANCY 0.0
#define GAMGI_CHEM_ATOM_SIZE 0.8
#define GAMGI_CHEM_BOND_SIZE 0.5

/****************************************************
 * Atom temperature (X-Ray Debye-Waller factor),    *
 * measured in Angstrom squared (A**2): 0 < T < 100 *
 * Generally, T < 30 signifies confidence in its    *
 * position, while T > 60 signifies disorder.       *
 ****************************************************/

#define GAMGI_CHEM_ATOM_TEMPERATURE_MIN   0.0
#define GAMGI_CHEM_ATOM_TEMPERATURE_MAX 100.0
#define GAMGI_CHEM_ATOM_TEMPERATURE GAMGI_CHEM_ATOM_TEMPERATURE_MIN

/***********************
 * default atom origin *
 ***********************/

#define GAMGI_CHEM_ATOM_X 0.0
#define GAMGI_CHEM_ATOM_Y 0.0
#define GAMGI_CHEM_ATOM_Z 0.0

/**********************************************************************
 * Symmetry operations where differences between initial and          *
 * transformed structures are higher than GAMGI_CHEM_SYMMETRY_INITIAL *
 * are immediately rejected. The others are optimized until the       *
 * fitting is lower than GAMGI_CHEM_SYMMETRY_FINAL or rejected        *
 * if that proves impossible. Making GAMGI_CHEM_SYMMETRY_INITIAL      *
 * equal to GAMGI_CHEM_SYMMETRY_FINAL disables optimization.          *
 **********************************************************************/

#define GAMGI_CHEM_SYMMETRY_TOLERANCE_INITIAL 1.0E-2
#define GAMGI_CHEM_SYMMETRY_TOLERANCE_FINAL   1.0E-3

/******************************************************************
 * Default thickness used to build polygon planes from 3 atoms    *
 * and line directions from 2 atoms. The thickness to link atoms  *
 * to atomic planes and lines cannot be smaller than these values *
 ******************************************************************/

#define GAMGI_CHEM_PLANE_THICKNESS     1.0E-2
#define GAMGI_CHEM_DIRECTION_THICKNESS 1.0E-2

/************************************************************************
 * get default radius from:                                             *
 * 0 - half bond distance; 1 - atomic effective; 2 - atomic calculated; *
 * 3 - covalent effective; 4 - covalent calculated; 5 - van der Waals.  *
 ************************************************************************/

#define GAMGI_CHEM_RADIUS_DEFAULT 0

/*************************************************
 * Size of mass, atomic radius, ionic radius,    *
 * color arrays defined in gamgi_chem_property.c *
 *************************************************/

#define GAMGI_CHEM_MASS_ARRAY 1255
#define GAMGI_CHEM_RADIUS_ARRAY 671
#define GAMGI_CHEM_IONIC_ARRAY 493
#define GAMGI_CHEM_IONIC_DRIVER_ARRAY 937
#define GAMGI_CHEM_COLOR_ARRAY 489

/************************************************
 * labels used in mass and ionic radius drivers *
 ************************************************/

#define GAMGI_CHEM_MASS_YEAR   1000
#define GAMGI_CHEM_MASS_DAY    2000
#define GAMGI_CHEM_MASS_HOUR   3000
#define GAMGI_CHEM_MASS_MINUTE 4000
#define GAMGI_CHEM_MASS_SECOND 5000

#define GAMGI_CHEM_IONIC_D  -10
#define GAMGI_CHEM_IONIC_LS -20
#define GAMGI_CHEM_IONIC_HS -30
#define GAMGI_CHEM_IONIC_COORDINATION 1000

/***********************************************
 * tolerances used in polytope code:           *
 * width: minimum width for cells box          *
 *                                             *
 * length: minimum length to build directions  *
 * area: minimum area to build planes          *
 * volume: minimum volume to build polyhedrons *
 *                                             *
 * visible: minimum value for visible faces    *
 * coplanar: maximum range for coplanar faces  *
 ***********************************************/

#define GAMGI_CHEM_POLYTOPE_WIDTH  1.0E-1

#define GAMGI_CHEM_POLYTOPE_LENGTH 1.0E-2
#define GAMGI_CHEM_POLYTOPE_AREA   1.0E-2
#define GAMGI_CHEM_POLYTOPE_VOLUME 1.0E-2

#define GAMGI_CHEM_POLYTOPE_VISIBLE  1.0E-10
#define GAMGI_CHEM_POLYTOPE_COPLANAR 1.0E-02

#define GAMGI_CHEM_POLYTOPE_VISIBLE_MAX  1.0E-05
#define GAMGI_CHEM_POLYTOPE_COPLANAR_MAX 1.0E-01

/*************************
 * Bohr first radius     *
 * default atomic charge *
 *************************/

#define GAMGI_CHEM_A0 0.52917720858
#define GAMGI_CHEM_ORBITAL_CHARGE 1.0

/**************************************************
 * used to calculate optimum radius in H orbitals *
 **************************************************/

#define GAMGI_CHEM_ORBITAL_R_SHIFT 2.0
#define GAMGI_CHEM_ORBITAL_R_MAX  50.0
#define GAMGI_CHEM_ORBITAL_R_STEP 0.01

/*************************************
 * tested against H orbital (6,5,-5) *
 *************************************/

#define GAMGI_CHEM_ORBITAL_T_STEP 0.04
#define GAMGI_CHEM_ORBITAL_P_STEP 0.04

/*************************************************************
 * used to build wired H orbitals: default density,          *
 * default number of points per n (main quantum number),     *
 * number of acceptable bad points (below density threshold) *
 *************************************************************/

#define GAMGI_CHEM_ORBITAL_DENSITY 1.0E-6
#define GAMGI_CHEM_ORBITAL_POINTS   50000
#define GAMGI_CHEM_ORBITAL_BAD       1000

/**************************************************************
 * used to build solid H orbitals: default number of slices   *
 * per octant, maximum number of slices per octant, initial   *
 * size and increment for points arrays (should be a multiple *
   of 3 (points per triangle) x 8 (bits per byte) = 24        *
 **************************************************************/

#define GAMGI_CHEM_ORBITAL_SLICES      50
#define GAMGI_CHEM_ORBITAL_SLICES_MAX 255
#define GAMGI_CHEM_ORBITAL_SEGMENT    120

/***************************************
 * default orbital origin, orientation *
 ***************************************/

#define GAMGI_CHEM_ORBITAL_X 0.0
#define GAMGI_CHEM_ORBITAL_Y 0.0
#define GAMGI_CHEM_ORBITAL_Z 0.0

#define GAMGI_CHEM_ORBITAL_E1 0.0
#define GAMGI_CHEM_ORBITAL_E2 0.0
#define GAMGI_CHEM_ORBITAL_E3 0.0

/**********************************************
 * default parameters for graphene structures *
 **********************************************/

#define GAMGI_CHEM_GRAPHENE_RINGS       10
#define GAMGI_CHEM_GRAPHENE_RINGS_IN     0
#define GAMGI_CHEM_GRAPHENE_RINGS_OUT   10
#define GAMGI_CHEM_GRAPHENE_ANGLE       60

#define GAMGI_CHEM_GRAPHENE_ELEMENT_A GAMGI_CHEM_C
#define GAMGI_CHEM_GRAPHENE_ELEMENT_B GAMGI_CHEM_C

#endif
