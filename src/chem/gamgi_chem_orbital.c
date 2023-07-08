/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_orbital.c
 *
 * Copyright (C); 2012,2013 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"

#include "gamgi_math_vector.h"

/***********************************************************************
 *                       Bibliography                                  *
 *                                                                     *
 * On H-based orbitals:                                                *
 * 1) Introduction To Quantum Mechanics, L. Pauling and B. Wilson,     *
 * McGraw Hill Book Company Inc. (1935)                                *
 * 2) http://winter.group.shef.ac.uk/orbitron/                         *
 * 3) http://www.orbitals.com/orb/ov.htm                               *
 * 4) https://undergrad-ed.chemistry.ohio-state.edu/H-AOs/4fxyz.html   *
 *                                                                     *
 * On tesselation algorithms:                                          *
 * 1) B.P. Carneiro, C.T. Silva and A.E. Kaufman, "Tetra-Cubes: an     *
 * algorithm to generate 3D isosurfaces based upon tetrahedra",        *
 * Anais do IX SIBGRAPI'96 (1996)                                      *
 * 2) W.E. Lorensen and H.E. Cline, "Marching Cubes, a high resolution *
 * 3D surface construction algorithm", Computer Graphics, 21-4 (1987)  *
 * 3) http://en.wikipedia.org/wiki/Marching_cubes                      *
 * 4) http://www.ics.uci.edu/~eppstein/projects/tetra/                 *
 * 5) http://paulbourke.net/geometry/polygonise/                       *
 * 6) http://users.polytech.unice.fr/~lingrand/MarchingCubes/algo.html *
 ***********************************************************************/

typedef struct _static_surface {

/***********************************************************
 * scanning current state:                                 *
 * 1) save x,y,z,f coordinates and wavefunction per vertex *
 * 2) save global [8] and vertices [0..7] state            *
 * 3) save number of triangles built                       *
 ***********************************************************/

double cache_f[8 * 4];
int    cache_v[8 * 1];
int    cache_s;
int triangles;

/*******************************************
 * set which octant faces need to be built *
 *******************************************/

int face_a[3][2][2];
int face_b[2][3][2];
int face_c[2][2][3];

/***************************************************
 * for current octant, save scanning data:         *
 * 1) current cube int coordinates                 *
 * 2) octant origin double coordinates             *
 * 3) bit array to flag points above the density   *
 * 4) bit array to flag points outside the orbital *
 * 5) bit arrays needed bits                       *
 * 6) bit arrays needed chars                      *
 ***************************************************/

int a, b, c;
double origin[3];
char *above, *outside, *signal;
int n_char, n_bit, n_signal;

/**************************************
 * stack arrays, used to avoid        *
 * recursive calls in static_outside; *
 **************************************/

unsigned char *stack_a, *stack_b, *stack_c;

/********************************************
 * pointers to functions building triangles *
 * from tetrahedra, squares and triangles   *
 ********************************************/

void (*triangle[8]) (struct _static_surface *surface, double *f0, double *f1, double *f2);
void (*square[16]) (struct _static_surface *surface, int *v0, int *v1, int *v2, int *v3);
void (*tetrahedron[16]) (struct _static_surface *surface, int *v0, int *v1, int *v2, int *v3);

/**************************************
 * accuracy per octant per direction: *
 * 1) width of sampling cubes         *
 * 2) number of sampling cubes        *
 * 3) number of sampling points       *
 **************************************/

double width;
int slices, n_points;

/**********************
 * wave function data *
 **********************/

gamgi_radial function_r;
gamgi_angular function_a;
double constant, ho;

/****************
 * orbital data *
 ****************/

gamgi_orbital *orbital;

/****************************************
 * save current triangle array position *
 ****************************************/

int offset; } static_surface;

typedef void (*gamgi_tetra) (void *surface, int *v0, int *v1, int *v2, int *v3);

/*************************************************************
 * radial Laguerre polynomials start here: the factor r**l   *
 * (l = angular quantum number) is supressed as this cancels *
 * with the factor 1 / r**l from the Legendre functions      *
 *************************************************************/

static double static_1s (double r)
{
/******************
 * n = 1, l = 0   *
 ******************/

return 1;
}

static double static_2s (double r)
{
/****************
 * n = 2, l = 0 *
 ****************/

return 2 - r;
}

static double static_2p (double r)
{
/****************
 * n = 2, l = 1 *
 ****************/

return 1;
}

static double static_3s (double r)
{
double r2 = r * r;

/****************
 * n = 3, l = 0 *
 ****************/

return 6 - 6 * r + r2;
}

static double static_3p (double r)
{
/****************
 * n = 3, l = 1 *
 ****************/

return 4 - r;
}

static double static_3d (double r)
{
/****************
 * n = 3, l = 2 *
 ****************/

return 1;
}

static double static_4s (double r)
{
double r2 = r * r;
double r3 = r2 * r;

/****************
 * n = 4, l = 0 *
 ****************/

return 24 - 36 * r + 12 * r2 - r3;
}

static double static_4p (double r)
{
double r2 = r * r;

/****************
 * n = 4, l = 1 *
 ****************/ 

return 20 - 10 * r + r2; 
}

static double static_4d (double r)
{
/****************
 * n = 4, l = 2 *
 ****************/

return 6 - r;
}

static double static_4f (double r)
{
/****************
 * n = 4, l = 3 *
 ****************/

return 1;
}

static double static_5s (double r)
{
double r2 = r * r;
double r3 = r2 * r;
double r4 = r3 * r;

/****************
 * n = 5, l = 0 *
 ****************/

return 120 - 240 * r + 120 * r2 - 20 * r3 + r4;
}

static double static_5p (double r)
{
double r2 = r * r;
double r3 = r2 * r;

/****************
 * n = 5, l = 1 *
 ****************/

return 120 - 90 * r + 18 * r2 - r3;
}

static double static_5d (double r)
{
double r2 = r * r;

/****************
 * n = 5, l = 2 *
 ****************/

return 42 - 14 * r + r2;
}

static double static_5f (double r)
{
/****************
 * n = 5, l = 3 *
 ****************/

return 8 - r;
}

static double static_5g (double r)
{
/****************
 * n = 5, l = 4 *
 ****************/

return 1;
}

static double static_6s (double r)
{
double r2 = r * r;
double r3 = r2 * r;
double r4 = r3 * r;
double r5 = r4 * r;

/****************
 * n = 6, l = 0 *
 ****************/

return 720 - 1800 * r + 1200 * r2 - 300 * r3 + 30 * r4 - r5;
}

static double static_6p (double r)
{
double r2 = r * r;
double r3 = r2 * r;
double r4 = r3 * r;

/****************
 * n = 6, l = 1 *
 ****************/

return 840 - 840 * r + 252 * r2 - 28 * r3 + r4;
}

static double static_6d (double r)
{
double r2 = r * r;
double r3 = r2 * r;

/****************
 * n = 6, l = 2 *
 ****************/

return 336 - 168 * r + 24 * r2 - r3;
}

static double static_6f (double r)
{
double r2 = r * r;

/****************
 * n = 6, l = 3 *
 ****************/

return 72 - 18 * r + r2;
}

static double static_6g (double r)
{
/****************
 * n = 6, l = 4 *
 ****************/

return 10 - r;
}

static double static_6h (double r)
{
/****************
 * n = 6, l = 5 *
 ****************/

return 1;
}

static void static_radial (int n, int l, double *c, gamgi_radial *f)
{
if (n == 1) { *c = 0.5; *f = static_1s; }

if (n == 2)
  {
  if (l == 0) { *c = 2 * sqrt (2); *f = static_2s; }
  if (l == 1) { *c = 2 * sqrt (6); *f = static_2p; }
  }

if (n == 3)
  {
  if (l == 0) { *c = 9 * sqrt (3);  *f = static_3s; }
  if (l == 1) { *c = 9 * sqrt (6);  *f = static_3p; }
  if (l == 2) { *c = 9 * sqrt (30); *f = static_3d; }
  }

if (n == 4)
  {
  if (l == 0) { *c = 96;             *f = static_4s; }
  if (l == 1) { *c = 32 * sqrt (15); *f = static_4p; }
  if (l == 2) { *c = 96 * sqrt (5);  *f = static_4d; }
  if (l == 3) { *c = 96 * sqrt (35); *f = static_4f; }
  }

if (n == 5)
  {
  if (l == 0) { *c = 300 * sqrt (5);  *f = static_5s; }
  if (l == 1) { *c = 150 * sqrt (30); *f = static_5p; }
  if (l == 2) { *c = 150 * sqrt (70); *f = static_5d; }
  if (l == 3) { *c = 300 * sqrt (70); *f = static_5f; }
  if (l == 4) { *c = 900 * sqrt (70); *f = static_5g; }
  }

if (n == 6)
  {
  if (l == 0) { *c = 2160 * sqrt (6);   *f = static_6s; }
  if (l == 1) { *c = 432 * sqrt (210);  *f = static_6p; }
  if (l == 2) { *c = 864 * sqrt (105);  *f = static_6d; }
  if (l == 3) { *c = 2592 * sqrt (35);  *f = static_6f; }
  if (l == 4) { *c = 12960 * sqrt (7);  *f = static_6g; }
  if (l == 5) { *c = 12960 * sqrt (77); *f = static_6h; }
  }

*c = 1 / *c;
}

/**************************************************************
 * angular Legendre functions start here: the factor 1 / r**l *
 * (l = angular quantum number) is supressed as this cancels  *
 * with the factor r**l from the Laguerre polynomials         *
 **************************************************************/

static double static_s (double x, double y, double z, double r)
{
/****************
 * l = 0, m = 0 *
 ****************/

return 1;
}

static double static_py (double x, double y, double z, double r)
{
/*****************
 * l = 1, m = -1 *
 *****************/

return y;
}

static double static_pz (double x, double y, double z, double r)
{
/****************
 * l = 1, m = 0 *
 ****************/

return z;
}

static double static_px (double x, double y, double z, double r)
{
/*****************
 * l = 1, m = +1 *
 *****************/

return x;
}

static double static_dxy (double x, double y, double z, double r)
{
/****************
 * l = 2, m = -2 *
 ****************/

return x * y;
}

static double static_dyz (double x, double y, double z, double r)
{
/****************
 * l = 2, m = -1 *
 ****************/

return y * z;
}

static double static_dz2 (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 2, m = 0 *
 ****************/

return 3 * z2 - r2;
}

static double static_dxz (double x, double y, double z, double r)
{
/****************
 * l = 2, m = 1 *
 ****************/

return x * z;
}

static double static_dx2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/****************
 * l = 2, m = 2 *
 ****************/

return x2 - y2;
}

static double static_fy_3x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/*****************
 * l = 3, m = -3 *
 *****************/

return y * (3 * x2 - y2);
}

static double static_fxyz (double x, double y, double z, double r)
{
/*****************
 * l = 3, m = -2 *
 *****************/

return x * y * z;
}

static double static_fyz2 (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/*****************
 * l = 3, m = -1 *
 *****************/

return y * (5 * z2 - r2);
}

static double static_fz3 (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 3, m = 0 *
 ****************/

return z * (5 * z2 - 3 * r2);
}

static double static_fxz2 (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 3, m = 1 *
 ****************/

return x * (5 * z2 - r2);
}

static double static_fz_x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/****************
 * l = 3, m = 2 *
 ****************/

return z * (x2 - y2);
}

static double static_fx_x2_3y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/****************
 * l = 3, m = 3 *
 ****************/

return x * (x2 - 3 * y2);
}

static double static_gxy_x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/*****************
 * l = 4, m = -4 *
 *****************/

return x * y * (x2 - y2);
}

static double static_gzy3 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/*****************
 * l = 4, m = -3 *
 *****************/

return y * z * (3 * x2 - y2);
}

static double static_gz2_xy (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/*****************
 * l = 4, m = -2 *
 *****************/

return x * y * (7 * z2 - r2);
}

static double static_gz3y (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/*****************
 * l = 4, m = -1 *
 *****************/

return y * z * (7 * z2 - 3 * r2);
}

static double static_gz4 (double x, double y, double z, double r)
{
double z2 = z * z;
double z4 = z2 * z2;
double r2 = r * r;
double r4 = r2 * r2;

/****************
 * l = 4, m = 0 *
 ****************/

return (35 * z4 - 30 * z2 * r2 + 3 * r4);
}

static double static_gz3x (double x, double y, double z, double r)
{
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 4, m = 1 *
 ****************/

return x * z * (7 * z2 - 3 * r2);
}

static double static_gz2_x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 4, m = 2 *
 ****************/

return (x2 - y2) * (7 * z2 - r2);
}

static double static_gzx3 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/*****************
 * l = 4, m = 3 *
 *****************/

return x * z * (x2 - 3 * y2);
}

static double static_gx4_y4 (double x, double y, double z, double r)
{
double x2 = x * x;
double x4 = x2 * x2;
double y2 = y * y;
double y4 = y2 * y2;

/*****************
 * l = 4, m = 4 *
 *****************/

return (x4 + y4 - 6 * x2 * y2);
}

static double static_hy5 (double x, double y, double z, double r)
{
double x2 = x * x;
double x4 = x2 * x2;
double y2 = y * y;
double y3 = y2 * y;
double y5 = y3 * y2;

/*****************
 * l = 5, m = -5 *
 *****************/

return (y5 - 10 * x2 * y3 + 5 * x4 * y);
}

static double static_hxyz_x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;

/*****************
 * l = 5, m = -4 *
 *****************/

return x * y * z * (x2 - y2);
}

static double static_hz2y3 (double x, double y, double z, double r)
{
double y3 = y * y * y;
double x2 = x * x;
double z2 = z * z;
double r2 = r * r;

/*****************
 * l = 5, m = -3 *
 *****************/

return (3 * x2 * y - y3) * (9 * z2 - r2);
}

static double static_hz3_xy (double x, double y, double z, double r)
{
double z3 = z * z * z;
double r2 = r * r;

/*****************
 * l = 5, m = -2 *
 *****************/

return x * y * (3 * z3 - z * r2);
}

static double static_hz4y (double x, double y, double z, double r)
{
double z2 = z * z;
double z4 = z2 * z2;
double r2 = r * r;
double r4 = r2 * r2;

/*****************
 * l = 5, m = -1 *
 *****************/

return y * (21 * z4 - 14 * z2 * r2 + r4);
}

static double static_hz5 (double x, double y, double z, double r)
{
double z2 = z * z;
double z3 = z2 * z;
double z5 = z3 * z2;
double r2 = r * r;
double r4 = r2 * r2;

/****************
 * l = 5, m = 0 *
 ****************/

return (63 * z5 - 70 * z3 * r2 + 15 * z * r4);
}

static double static_hz4x (double x, double y, double z, double r)
{
double z2 = z * z;
double z4 = z2 * z2;
double r2 = r * r;
double r4 = r2 * r2;

/****************
 * l = 5, m = 1 *
 ****************/

return x * (21 * z4 - 14 * z2 * r2 + r4);
}

static double static_hz3_x2_y2 (double x, double y, double z, double r)
{
double x2 = x * x;
double y2 = y * y;
double z3 = z * z * z;
double r2 = r * r;

/****************
 * l = 5, m = 2 *
 ****************/

return (x2 - y2) * (3 * z3 - z * r2);
}

static double static_hz2x3 (double x, double y, double z, double r)
{
double x3 = x * x * x;
double y2 = y * y;
double z2 = z * z;
double r2 = r * r;

/****************
 * l = 5, m = 3 *
 ****************/

return (x3 - 3 * x * y2) * (9 * z2 - r2);
}

static double static_hz_x4_y4 (double x, double y, double z, double r)
{
double x2 = x * x;
double x4 = x2 * x2;
double y2 = y * y;
double y4 = y2 * y2;

/****************
 * l = 5, m = 4 *
 ****************/

return z * (x4 + y4 - 6 * x2 * y2);
}

static double static_hx5 (double x, double y, double z, double r)
{
double x2 = x * x;
double x3 = x2 * x;
double x5 = x3 * x2;
double y2 = y * y;
double y4 = y2 * y2;

/****************
 * l = 5, m = 5 *
 ****************/

return (x5 - 10 * x3 * y2 + 5 * x * y4);
}

static void static_angular (int l, int m, double *c, gamgi_angular *f)
{
*f = NULL;

if (l == 0) { *c = 1; *f = static_s; }

if (l == 1)
  {
  if (m == -1) { *c = sqrt (3); *f = static_py; }
  if (m ==  0) { *c = sqrt (3); *f = static_pz; }
  if (m == +1) { *c = sqrt (3); *f = static_px; }
  }

if (l == 2)
  {
  if (m == -2) { *c = sqrt (15);       *f = static_dxy; }
  if (m == -1) { *c = sqrt (15);       *f = static_dyz; }
  if (m ==  0) { *c = sqrt (5 / 4.0);  *f = static_dz2; }
  if (m == +1) { *c = sqrt (15);       *f = static_dxz; }
  if (m == +2) { *c = sqrt (15 / 4.0); *f = static_dx2_y2; }
  }

if (l == 3)
  {
  if (m == -3) { *c = sqrt (35 / 8.0); *f = static_fy_3x2_y2; }
  if (m == -2) { *c = sqrt (105);       *f = static_fxyz; }
  if (m == -1) { *c = sqrt (21 / 8.0);  *f = static_fyz2; }
  if (m ==  0) { *c = sqrt (7 / 4.0);   *f = static_fz3; }
  if (m == +1) { *c = sqrt (21 / 8.0);  *f = static_fxz2; }
  if (m == +2) { *c = sqrt (105 / 4.0); *f = static_fz_x2_y2; }
  if (m == +3) { *c = sqrt (35 / 8.0);  *f = static_fx_x2_3y2; }
  }

if (l == 4)
  {
  if (m == -4) { *c = sqrt (315 / 4.0);  *f = static_gxy_x2_y2; }
  if (m == -3) { *c = sqrt (315 / 8.0);  *f = static_gzy3; }
  if (m == -2) { *c = sqrt (45 / 4.0);   *f = static_gz2_xy; }
  if (m == -1) { *c = sqrt (45 / 8.0);   *f = static_gz3y; }
  if (m ==  0) { *c = 3 / 8.0;           *f = static_gz4; }
  if (m == +1) { *c = sqrt (45 / 8.0);   *f = static_gz3x; }
  if (m == +2) { *c = sqrt (45 / 16.0);  *f = static_gz2_x2_y2; }
  if (m == +3) { *c = sqrt (315 / 8.0);  *f = static_gzx3; }
  if (m == +4) { *c = sqrt (315 / 64.0); *f = static_gx4_y4; }
  }

if (l == 5)
  {
  if (m == -5) { *c = sqrt (693 / 128.0); *f = static_hy5; }
  if (m == -4) { *c = sqrt (3465 / 4.0);  *f = static_hxyz_x2_y2; }
  if (m == -3) { *c = sqrt (385 / 128.0); *f = static_hz2y3; }
  if (m == -2) { *c = sqrt (1155 / 4.0);  *f = static_hz3_xy; }
  if (m == -1) { *c = sqrt (165 / 64.0);  *f = static_hz4y; }
  if (m ==  0) { *c = sqrt (11 / 64.0);   *f = static_hz5; }
  if (m == +1) { *c = sqrt (165 / 64.0);  *f = static_hz4x; }
  if (m == +2) { *c = sqrt (1155 / 16.0); *f = static_hz3_x2_y2; }
  if (m == +3) { *c = sqrt (385 / 128.0); *f = static_hz2x3; }
  if (m == +4) { *c = sqrt (3465 / 64.0); *f = static_hz_x4_y4; }
  if (m == +5) { *c = sqrt (693 / 128.0); *f = static_hx5; }
  }

*c /= sqrt (4.0 * GAMGI_MATH_PI);
}

static double static_density (double r, double f, gamgi_angular function_a)
{
double theta, phi, end;
double x, y, z, s;
double g;

end = GAMGI_MATH_PI / 2.0;
for (theta = 0.0; theta < end; theta += GAMGI_CHEM_ORBITAL_T_STEP)
  {
  z = r * cos (theta);
  s = r * sin (theta);

  for (phi = 0.0; phi < end; phi += GAMGI_CHEM_ORBITAL_P_STEP)
    {
    x = s * cos (phi);
    y = s * sin (phi);

    g = f * (*function_a) (x, y, z, r);
    if (g * g > GAMGI_CHEM_ORBITAL_DENSITY) return TRUE;
    }
  }

return FALSE;
}

static void static_frame (gamgi_orbital *orbital)
{
double *points;
double radius;
int *lines;
int x, y, z, offset;

points = orbital->points;
radius = orbital->radius;

/**********************
 * add 8 frame points *
 **********************/

offset = 0;
for (z = -1; z <= 1; z += 2)
  {
  for (y = -1; y <= 1; y += 2)
    {
    for (x = -1; x <= 1; x += 2)
      {
      points[offset++] = radius * x;
      points[offset++] = radius * y;
      points[offset++] = radius * z;
      }
    }
  }

/*********************************************************
 * create array with 12 edges: edges x, edges y, edges z *
 *********************************************************/

orbital->lines = lines = (int *) malloc (25 * sizeof (int));
orbital->n_lines = 25;

offset = 0;
lines[offset++] = 12;

lines[offset++] = 0;
lines[offset++] = 1;
lines[offset++] = 2;
lines[offset++] = 3;
lines[offset++] = 4;
lines[offset++] = 5;
lines[offset++] = 6;
lines[offset++] = 7;

lines[offset++] = 0;
lines[offset++] = 2;
lines[offset++] = 1;
lines[offset++] = 3;
lines[offset++] = 4;
lines[offset++] = 6;
lines[offset++] = 5;
lines[offset++] = 7;

lines[offset++] = 0;
lines[offset++] = 4;
lines[offset++] = 1;
lines[offset++] = 5;
lines[offset++] = 2;
lines[offset++] = 6;
lines[offset++] = 3;
lines[offset++] = 7;
}

static gamgi_bool static_wired (gamgi_orbital *orbital)
{
gamgi_radial function_r;
gamgi_angular function_a;
double *points;
double max = RAND_MAX;
double za, r, ho, rho;
double density, radius, f, x, y, z;
double constant, constant_z, constant_r, constant_a;
int *dots;
int offset, offset_negative, offset_positive;
int sampling, n_points, i, bad; 
int a, b, c;

/*******************************************************
 * if required, add frame data to points, lines arrays *
 *******************************************************/

dots = orbital->dots;
density = orbital->density;
radius = orbital->radius;
sampling = orbital->sampling;

orbital->n_points = n_points = sampling + dots[0];
orbital->points = points = (double *) malloc (n_points * 3 * sizeof (double));

static_radial (orbital->n, orbital->l, &constant_r, &function_r);
static_angular (orbital->l, orbital->m, &constant_a, &function_a);

za = orbital->charge / GAMGI_CHEM_A0;
ho = 2 * za / orbital->n;
constant_z = pow (za, 1.5) * pow (ho, orbital->l);
constant = constant_z * constant_r * constant_a;

/***************************************************
 * start negative phase points from the beginning  *
 * of the array (after frame points if available)  *
 * and positive phase points from the end (even if *
 * the same color will be used to show both sets   *
 ***************************************************/

offset_positive = dots[0];
offset_negative = n_points - 1;

/******************************
 * set random number sequence *
 ******************************/

srand (orbital->seed);

/*******************************************
 * i_bad is just to prevent infinite loops *
 *******************************************/

i = bad = 0;
while (i < sampling && bad < GAMGI_CHEM_ORBITAL_BAD)
  {
  x = 2.0 * rand () / max - 1.0;
  y = 2.0 * rand () / max - 1.0;
  z = 2.0 * rand () / max - 1.0;
  r = x * x + y * y + z * z;
  if (r > 1.0) continue;

  a = b = c = 0;
  if (x > 0) a = 1;
  if (y > 0) b = 1;
  if (z > 0) c = 1;
  if (orbital->octants[4*c + 2*b + a] == FALSE) continue;

  r = radius * sqrt (r);
  rho = r * ho;
  x *= radius;
  y *= radius;
  z *= radius;

  f = constant * exp (-rho / 2) * (*function_r) (rho) * (*function_a) (x, y, z, r);
  if (f * f < density) { bad++; continue; }

  i++; bad = 0;
  if (f >= 0) offset = offset_positive++;
  if (f < 0) offset = offset_negative--;

  points[3*offset + 0] = x;
  points[3*offset + 1] = y;
  points[3*offset + 2] = z;
  }

/***************************************
 * set positive (dots[1]) and negative *
 * (dots[2]) number of density points  *
 ***************************************/
 
dots[1] = offset_positive - dots[0];
if (orbital->phase == TRUE)
  dots[2] = n_points - 1 - offset_negative;
else
  dots[1] += n_points - 1 - offset_negative;

if (dots[0] + dots[1] + dots[2] == n_points) return TRUE;

/*************************************
 * clean memory when things go wrong *
 *************************************/

free (orbital->points);
orbital->points = NULL;
orbital->n_points = 0;
return FALSE;
}

static void static_clean (char *array, int n_char)
{
int n;

for (n = 0; n < n_char; n++) array[n] = 0;
}

static int static_offset (int n_points, int a, int b, int c)
{
return c * n_points * n_points + b * n_points + a;
}

static void static_above (static_surface *surface, int a, int b, int c)
{
gamgi_orbital *orbital;
char *above;
gamgi_radial function_r;
gamgi_angular function_a;
double *origin;
double r, ho, rho;
double density, radius, width, f, x, y, z;
double constant;
int n_points, n;

/********************
 * get orbital data *
 ********************/

orbital = surface->orbital;
function_r = surface->function_r;
function_a = surface->function_a;
constant = surface->constant;
ho = surface->ho;

/*****************
 * get user data *
 *****************/

density = orbital->density;
width = surface->width;
n_points = surface->n_points;
radius = orbital->radius;

origin = surface->origin;
origin[0] = radius * (a - 1);
origin[1] = radius * (b - 1);
origin[2] = radius * (c - 1);

/*******************
 * set above array *
 *******************/

above = surface->above;
for (c = 0; c < n_points; c++)
  {
  z = origin[2] + c * width;
  for (b = 0; b < n_points; b++)
    {
    y = origin[1] + b * width;
    for (a = 0; a < n_points; a++)
      {
      x = origin[0] + a * width;
      r = x * x + y * y + z * z;
      r = sqrt (r);

      rho = r * ho;
      f = constant * exp (-rho / 2) * (*function_r) (rho) * (*function_a) (x, y, z, r);
      if (f * f < density) continue;

      n = c * n_points * n_points + b * n_points + a;
      GAMGI_ENGINE_BITSET(above, n);
      }
    }
  }
}

static void static_outside (static_surface *surface, int a, int b, int c)
{
unsigned char *stack_a, *stack_b, *stack_c;
char *outside, *above;
int a_shift, b_shift, c_shift;
int offset, step, bit, bit_new;
int n_points;

/*************************************************
 * this function must be non-recursive, because  *
 * the number of recursive calls is just too big *
 *************************************************/

n_points = surface->n_points;
outside = surface->outside;
above = surface->above;

stack_a = surface->stack_a;
stack_b = surface->stack_b;
stack_c = surface->stack_c;

a_shift = 1;
b_shift = n_points;
c_shift = n_points * n_points;

offset = -1;
step = 0;

while (1 > 0)
  {
  /*********************
   * controlling block *
   *********************/

  offset += step;
  if (step == +1)
    {
    GAMGI_ENGINE_BITSET(outside, bit_new);
    stack_a[offset] = a;
    stack_b[offset] = b;
    stack_c[offset] = c;
    }
  if (step == -1)
    {
    if (offset < 0) break;
    a = stack_a[offset];
    b = stack_b[offset];
    c = stack_c[offset];
    }
  bit = a * a_shift + b * b_shift + c * c_shift;

  /******************
   * scanning block *
   ******************/

  step = +1;

  bit_new = bit - a_shift;
  if (a > 0 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { a--; continue; }

  bit_new = bit + a_shift;
  if (a < n_points - 1 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { a++; continue; }

  bit_new = bit - b_shift;
  if (b > 0 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { b--; continue; }

  bit_new = bit + b_shift;
  if (b < n_points - 1 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { b++; continue; }

  bit_new = bit - c_shift;
  if (c > 0 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { c--; continue; }

  bit_new = bit + c_shift;
  if (c < n_points - 1 && GAMGI_ENGINE_BITTEST(outside, bit_new) == 0 &&
  GAMGI_ENGINE_BITTEST(above, bit_new) == 0) { c++; continue; }

  step = -1;
  }
}

static void static_points (static_surface *surface,
double *f1, double *f2, double *f3)
{
gamgi_orbital *orbital;
double *points;
int *dots;
int n_points, offset;

orbital = surface->orbital;
dots = orbital->dots;
points = orbital->points;
n_points = orbital->n_points;

/********************************
 * add triangle coordinate data *
 ********************************/

offset = dots[0] + dots[1];
if (offset + 3 > n_points)
  {
  n_points += GAMGI_CHEM_ORBITAL_SEGMENT;
  points = (double *) realloc (points, n_points * 3 * sizeof (double));
  }
offset *= 3;

points[offset + 0] = f1[0];
points[offset + 1] = f1[1];
points[offset + 2] = f1[2];

points[offset + 3] = f2[0];
points[offset + 4] = f2[1];
points[offset + 5] = f2[2];

points[offset + 6] = f3[0];
points[offset + 7] = f3[1];
points[offset + 8] = f3[2];

dots[1] += 3;
surface->triangles += 1;

orbital->points = points;
orbital->n_points = n_points;
}

static void static_signal (static_surface *surface, int positive)
{
int n_char, n_signal, n;
char *signal;

signal = surface->signal;
n_signal = surface->n_signal;

/***********************************************************************
 * realloc triangle signal array: 24 = 3 points/triangle x 8 bits/byte *
 ***********************************************************************/

n = surface->orbital->dots[1] / 3;
n_char = GAMGI_ENGINE_BITNSLOTS(n);
if (n_char > n_signal)
  {
  n_signal += GAMGI_CHEM_ORBITAL_SEGMENT / 24;
  signal = (char *) realloc (signal, n_signal * sizeof (char));
  }

(positive == TRUE)?
GAMGI_ENGINE_BITSET(signal, n - 1):
GAMGI_ENGINE_BITCLEAR(signal, n - 1);

surface->signal = signal;
surface->n_signal = n_signal;
}

static void static_reset (static_surface *surface)
{
int *cache_v = surface->cache_v;

cache_v[0] = FALSE;
cache_v[1] = FALSE;
cache_v[2] = FALSE;
cache_v[3] = FALSE;
cache_v[4] = FALSE;
cache_v[5] = FALSE;
cache_v[6] = FALSE;
cache_v[7] = FALSE;
surface->cache_s = TRUE;
}

static void static_3d_copy (static_surface *surface)
{
double *cache_f = surface->cache_f;
int *cache_v = surface->cache_v;

cache_f[4 * 0 + 0] = cache_f[4 * 4 + 0];
cache_f[4 * 0 + 1] = cache_f[4 * 4 + 1];
cache_f[4 * 0 + 2] = cache_f[4 * 4 + 2];
cache_f[4 * 0 + 3] = cache_f[4 * 4 + 3];

cache_f[4 * 1 + 0] = cache_f[4 * 5 + 0];
cache_f[4 * 1 + 1] = cache_f[4 * 5 + 1];
cache_f[4 * 1 + 2] = cache_f[4 * 5 + 2];
cache_f[4 * 1 + 3] = cache_f[4 * 5 + 3];

cache_f[4 * 2 + 0] = cache_f[4 * 6 + 0];
cache_f[4 * 2 + 1] = cache_f[4 * 6 + 1];
cache_f[4 * 2 + 2] = cache_f[4 * 6 + 2];
cache_f[4 * 2 + 3] = cache_f[4 * 6 + 3];

cache_f[4 * 3 + 0] = cache_f[4 * 7 + 0];
cache_f[4 * 3 + 1] = cache_f[4 * 7 + 1];
cache_f[4 * 3 + 2] = cache_f[4 * 7 + 2];
cache_f[4 * 3 + 3] = cache_f[4 * 7 + 3];

cache_v[0] = cache_v[4];
cache_v[1] = cache_v[5];
cache_v[2] = cache_v[6];
cache_v[3] = cache_v[7];
cache_v[4] = FALSE;
cache_v[5] = FALSE;
cache_v[6] = FALSE;
cache_v[7] = FALSE;
}

static void static_2d_copy (static_surface *surface)
{
double *cache_f = surface->cache_f;
int *cache_v = surface->cache_v;

cache_f[4 * 0 + 0] = cache_f[4 * 1 + 0];
cache_f[4 * 0 + 1] = cache_f[4 * 1 + 1];
cache_f[4 * 0 + 2] = cache_f[4 * 1 + 2];
cache_f[4 * 0 + 3] = cache_f[4 * 1 + 3];

cache_f[4 * 3 + 0] = cache_f[4 * 2 + 0];
cache_f[4 * 3 + 1] = cache_f[4 * 2 + 1];
cache_f[4 * 3 + 2] = cache_f[4 * 2 + 2];
cache_f[4 * 3 + 3] = cache_f[4 * 2 + 3];

cache_v[0] = cache_v[1];
cache_v[1] = FALSE;
cache_v[2] = FALSE;
cache_v[3] = cache_v[2];
}

static void static_vertex (static_surface *surface, int *v, double *f)
{
gamgi_radial function_r;
gamgi_angular function_a;
double *cache_f, *origin;
double width, r, rho;
int *cache_v;
int cache_s, vertex;
int a, b, c;

/*********************
 * get scanning data *
 *********************/

cache_f = surface->cache_f;
cache_v = surface->cache_v;
cache_s = surface->cache_s;

vertex = v[3];
if (cache_s == TRUE && cache_v[vertex] == TRUE)
  {
  f[0] = cache_f[4 * vertex + 0];
  f[1] = cache_f[4 * vertex + 1];
  f[2] = cache_f[4 * vertex + 2];
  f[3] = cache_f[4 * vertex + 3];
  }
else
  {
  if (cache_s == FALSE) static_reset (surface);
  cache_v[vertex] = TRUE;

  function_r = surface->function_r;
  function_a = surface->function_a;
  origin = surface->origin;
  width = surface->width;
  a = surface->a;
  b = surface->b;
  c = surface->c;
  f[0] = origin[0] + width * (a + v[0]);
  f[1] = origin[1] + width * (b + v[1]);
  f[2] = origin[2] + width * (c + v[2]);

  r = f[0] * f[0] + f[1] * f[1] + f[2] * f[2];
  r = sqrt (r);
  rho = r * surface->ho;
  f[3] = surface->constant * exp (-rho / 2) * (*function_r) (rho) *
  (*function_a) (f[0], f[1], f[2], r);

  cache_f[4 * vertex + 0] = f[0];
  cache_f[4 * vertex + 1] = f[1];
  cache_f[4 * vertex + 2] = f[2];
  cache_f[4 * vertex + 3] = f[3];
  }
}

static void static_edge (static_surface *surface, int *v0, int *v1, double *f)
{
double f0[4], f1[4];
double ff0, ff1, fraction;

static_vertex (surface, v0, f0);
static_vertex (surface, v1, f1);

ff0 = f0[3] * f0[3];
ff1 = f1[3] * f1[3];
fraction = (surface->orbital->density - ff0) / (ff1 - ff0);
if (fraction < 0) fraction = 0;
if (fraction > 1) fraction = 1;

f[0] = f0[0] + (f1[0] - f0[0]) * fraction;
f[1] = f0[1] + (f1[1] - f0[1]) * fraction;
f[2] = f0[2] + (f1[2] - f0[2]) * fraction;
f[3] = f0[3] + (f1[3] - f0[3]) * fraction;
}

static void static_zero (double *f0, double *f1, double *g)
{
double fraction;

fraction = (0.0 - f0[3]) / (f1[3] - f0[3]);
g[0] = f0[0] + (f1[0] - f0[0]) * fraction;
g[1] = f0[1] + (f1[1] - f0[1]) * fraction;
g[2] = f0[2] + (f1[2] - f0[2]) * fraction;
}

static void static_triangle_000 (static_surface *surface,
double *f0, double *f1, double *f2)
{
static_points (surface, f0, f1, f2);
static_signal (surface, FALSE);
}

static void static_triangle_001 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f1, f0, g0);
static_zero (f2, f0, g1);
static_points (surface, f0, g0, g1);
static_signal (surface, TRUE);
static_points (surface, g0, f1, f2);
static_signal (surface, FALSE);
static_points (surface, g0, f2, g1);
static_signal (surface, FALSE);
}

static void static_triangle_010 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f2, f1, g0);
static_zero (f0, f1, g1);
static_points (surface, f1, g0, g1);
static_signal (surface, TRUE);
static_points (surface, g0, f2, f0);
static_signal (surface, FALSE);
static_points (surface, g0, f0, g1);
static_signal (surface, FALSE);
}

static void static_triangle_011 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f2, f0, g0);
static_zero (f2, f1, g1);
static_points (surface, f2, g0, g1);
static_signal (surface, FALSE);
static_points (surface, g0, f0, f1);
static_signal (surface, TRUE);
static_points (surface, g0, f1, g1);
static_signal (surface, TRUE);
}

static void static_triangle_100 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f0, f2, g0);
static_zero (f1, f2, g1);
static_points (surface, f2, g0, g1);
static_signal (surface, TRUE);
static_points (surface, g0, f0, f1);
static_signal (surface, FALSE);
static_points (surface, g0, f1, g1);
static_signal (surface, FALSE);
}

static void static_triangle_101 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f1, f2, g0);
static_zero (f1, f0, g1);
static_points (surface, f1, g0, g1);
static_signal (surface, FALSE);
static_points (surface, g0, f2, f0);
static_signal (surface, TRUE);
static_points (surface, g0, f0, g1);
static_signal (surface, TRUE);
}

static void static_triangle_110 (static_surface *surface,
double *f0, double *f1, double *f2)
{
double g0[3], g1[3];

static_zero (f0, f1, g0);
static_zero (f0, f2, g1);
static_points (surface, f0, g0, g1);
static_signal (surface, FALSE);
static_points (surface, g0, f1, f2);
static_signal (surface, TRUE);
static_points (surface, g0, f2, g1);
static_signal (surface, TRUE);
}

static void static_triangle_111 (static_surface *surface,
double *f0, double *f1, double *f2)
{
static_points (surface, f0, f1, f2);
static_signal (surface, TRUE);
}

static void static_triangle (static_surface *surface,
double *f0, double *f1, double *f2)
{
int state;

/***********************************
 * at this stage triangles must    *
 * be orientated counter-clockwise *
 *              2                  *
 *            /   \                *
 *           0  -  1               *
 ***********************************/

if (surface->orbital->phase == TRUE)
  {
  state = 0;
  if (f0[3] > 0) state |= 1;
  if (f1[3] > 0) state |= 2;
  if (f2[3] > 0) state |= 4;

  (*surface->triangle[state]) (surface, f0, f1, f2);
  }
else static_points (surface, f0, f1, f2);
}

/********************************
 * 3D tetrahedron intersections *
 ********************************/

static void static_tetrahedron_0000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
}

static void static_tetrahedron_0001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v1, v0, fe0);
static_edge (surface, v3, v0, fe1);
static_edge (surface, v2, v0, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_0010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v0, v1, fe0);
static_edge (surface, v2, v1, fe1);
static_edge (surface, v3, v1, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_0011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v2, v0, fe0);
static_edge (surface, v3, v0, fe1);
static_edge (surface, v3, v1, fe2);
static_triangle (surface, fe2, fe1, fe0);
static_edge (surface, v2, v1, fe1);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_0100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v1, v2, fe0);
static_edge (surface, v0, v2, fe1);
static_edge (surface, v3, v2, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_0101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v3, v2, fe0);
static_edge (surface, v3, v0, fe1);
static_edge (surface, v1, v0, fe2);
static_triangle (surface, fe2, fe1, fe0);
static_edge (surface, v1, v2, fe1);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_0110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v3, v1, fe0);
static_edge (surface, v0, v1, fe1);
static_edge (surface, v0, v2, fe2);
static_triangle (surface, fe0, fe1, fe2);
static_edge (surface, v3, v2, fe1);
static_triangle (surface, fe2, fe1, fe0);
}

static void static_tetrahedron_0111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v3, v2, fe0);
static_edge (surface, v3, v1, fe1);
static_edge (surface, v3, v0, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v0, v3, fe0);
static_edge (surface, v1, v3, fe1);
static_edge (surface, v2, v3, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v2, v0, fe0);
static_edge (surface, v1, v0, fe1);
static_edge (surface, v1, v3, fe2);
static_triangle (surface, fe0, fe1, fe2);
static_edge (surface, v2, v3, fe1);
static_triangle (surface, fe2, fe1, fe0);
}

static void static_tetrahedron_1010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v0, v1, fe0);
static_edge (surface, v0, v3, fe1);
static_edge (surface, v2, v3, fe2);
static_triangle (surface, fe2, fe1, fe0);
static_edge (surface, v2, v1, fe1);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v2, v3, fe0);
static_edge (surface, v2, v0, fe1);
static_edge (surface, v2, v1, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v1, v3, fe0);
static_edge (surface, v0, v3, fe1);
static_edge (surface, v0, v2, fe2);
static_triangle (surface, fe2, fe1, fe0);
static_edge (surface, v1, v2, fe1);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v1, v3, fe0);
static_edge (surface, v1, v2, fe1);
static_edge (surface, v1, v0, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4];

static_edge (surface, v0, v2, fe0);
static_edge (surface, v0, v3, fe1);
static_edge (surface, v0, v1, fe2);
static_triangle (surface, fe0, fe1, fe2);
}

static void static_tetrahedron_1111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
}

static void static_tetrahedron (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
char *outside;
int n_points, bit;
int a, b, c;
int state;

/***************************************
 * generate triangles from tetrahedron *
 ***************************************/

n_points = surface->n_points;
a = surface->a;
b = surface->b;
c = surface->c;
outside = surface->outside;

state = 0;
bit = static_offset (n_points, a + v0[0], b + v0[1], c + v0[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 1;
bit = static_offset (n_points, a + v1[0], b + v1[1], c + v1[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 2;
bit = static_offset (n_points, a + v2[0], b + v2[1], c + v2[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 4;
bit = static_offset (n_points, a + v3[0], b + v3[1], c + v3[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 8;

(*surface->tetrahedron[state]) (surface, v0, v1, v2, v3);
}

static void static_cubes (static_surface *surface)
{
int n_points, a, b, c;
int v0[4], v1[4], v2[4], v3[4], v4[4], v5[4], v6[4], v7[4];

/*****************************************
 * generate cubes, tetrahedra, triangles *
 *****************************************/

/****************************
 * cube vertices convention *
 *                          *
 *     Back    Front        *
 *      3 2     7 6         *
 *      0 1     4 5         *
 ****************************/

v0[0] = 0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = 0; v1[1] = 1; v1[2] = 0; v1[3] = 1;
v2[0] = 0; v2[1] = 1; v2[2] = 1; v2[3] = 2;
v3[0] = 0; v3[1] = 0; v3[2] = 1; v3[3] = 3;
v4[0] = 1; v4[1] = 0; v4[2] = 0; v4[3] = 4;
v5[0] = 1; v5[1] = 1; v5[2] = 0; v5[3] = 5;
v6[0] = 1; v6[1] = 1; v6[2] = 1; v6[3] = 6;
v7[0] = 1; v7[1] = 0; v7[2] = 1; v7[3] = 7;

/**************
 * scan cubes *
 **************/

n_points = surface->n_points;
for (c = 0; c < n_points - 1; c++)
  {
  surface->c = c;
  for (b = 0; b < n_points - 1; b++)
    {
    surface->b = b;
    for (a = 0; a < n_points - 1; a++)
      {
      surface->a = a;

      if (a == 0 || surface->triangles == 0) surface->cache_s = FALSE;
      else static_3d_copy (surface);
      surface->triangles = 0;

      /*************************************************
       * scan 5 (central plus four) tetrahedra: as in  *
       * a 3D chess board, odd cubes MUST be rotatated *
       * 90 degrees to be consistent with even cubes   *
       *************************************************/

      /****************************************************
       * vertex orientation in tetrahedra: the face       *
       * vertex0-vertex1-vertex2, opposite to vertex3,    *
       * MUST rotate counter-clockwise, seen from outside *
       *                       -0-                        *
       *                        |                         *
       *                   +3+-----+1+                    *
       *                        |                         *
       *                       -2-                        *
       ****************************************************/

      if ((a + b + c) % 2 == 0)
        {
        static_tetrahedron (surface, v0, v5, v7, v2);
        static_tetrahedron (surface, v5, v0, v7, v4);
        static_tetrahedron (surface, v5, v0, v1, v2);
        static_tetrahedron (surface, v7, v2, v5, v6);
        static_tetrahedron (surface, v2, v7, v0, v3);
        }
      else
        {
        static_tetrahedron (surface, v4, v1, v6, v3);
        static_tetrahedron (surface, v4, v1, v5, v6);
        static_tetrahedron (surface, v4, v0, v1, v3);
        static_tetrahedron (surface, v6, v4, v3, v7);
        static_tetrahedron (surface, v6, v3, v1, v2);
        }

      }
    }
  }
}

/***************************
 * 2D square intersections *
 ***************************/

static void static_square_0000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
/******
 * 00 *
 * 00 *
 ******/
}

static void static_square_0001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4];

/******
 * 00 *
 * 10 *
 ******/

static_edge (surface, v1, v0, fe0);
static_edge (surface, v3, v0, fe1);
static_vertex (surface, v0, fv0);
static_triangle (surface, fv0, fe0, fe1);
}

static void static_square_0010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4];

/******
 * 00 *
 * 01 *
 ******/

static_edge (surface, v2, v1, fe0);
static_edge (surface, v0, v1, fe1);
static_vertex (surface, v1, fv0);
static_triangle (surface, fv0, fe0, fe1);
}

static void static_square_0011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4];

/******
 * 00 *
 * 11 *
 ******/

static_edge (surface, v3, v0, fe0);
static_edge (surface, v2, v1, fe1);
static_vertex (surface, v0, fv0);
static_vertex (surface, v1, fv1);
static_triangle (surface, fe0, fv0, fe1);
static_triangle (surface, fe1, fv0, fv1);
}

static void static_square_0100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4];

/******
 * 01 *
 * 00 *
 ******/

static_edge (surface, v3, v2, fe0);
static_edge (surface, v1, v2, fe1);
static_vertex (surface, v2, fv0);
static_triangle (surface, fv0, fe0, fe1);
}

static void static_square_0101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4], fe3[4];
double fv0[4], fv1[4];

/******
 * 01 *
 * 10 *
 ******/

static_edge (surface, v1, v0, fe0);
static_edge (surface, v1, v2, fe1);
static_edge (surface, v3, v2, fe2);
static_edge (surface, v3, v0, fe3);
static_vertex (surface, v0, fv0);
static_vertex (surface, v2, fv1);
static_triangle (surface, fv0, fe0, fe3);
static_triangle (surface, fv1, fe2, fe1);
}

static void static_square_0110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4];

/******
 * 01 *
 * 01 *
 ******/

static_edge (surface, v0, v1, fe0);
static_edge (surface, v3, v2, fe1);
static_vertex (surface, v1, fv0);
static_vertex (surface, v2, fv1);
static_triangle (surface, fe0, fv0, fe1);
static_triangle (surface, fe1, fv0, fv1);
}

static void static_square_0111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4], fv2[4];

/******
 * 01 *
 * 11 *
 ******/

static_edge (surface, v3, v0, fe0);
static_edge (surface, v3, v2, fe1);
static_vertex (surface, v0, fv0);
static_vertex (surface, v1, fv1);
static_vertex (surface, v2, fv2);
static_triangle (surface, fe0, fv0, fv1);
static_triangle (surface, fe1, fv1, fv2);
static_triangle (surface, fe1, fe0, fv1);
}

static void static_square_1000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4];

/******
 * 10 *
 * 00 *
 ******/

static_edge (surface, v0, v3, fe0);
static_edge (surface, v2, v3, fe1);
static_vertex (surface, v3, fv0);
static_triangle (surface, fv0, fe0, fe1);
}

static void static_square_1001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3) 
{
double fe0[4], fe1[4];
double fv0[4], fv1[4];

/******
 * 10 *
 * 10 *
 ******/

static_edge (surface, v1, v0, fe0);
static_edge (surface, v2, v3, fe1);
static_vertex (surface, v0, fv0);
static_vertex (surface, v3, fv1);
static_triangle (surface, fe0, fe1, fv1);
static_triangle (surface, fv1, fv0, fe0);
}

static void static_square_1010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4], fe2[4], fe3[4];
double fv0[4], fv1[4];

/******
 * 10 *
 * 01 *
 ******/

static_edge (surface, v0, v1, fe0);
static_edge (surface, v2, v1, fe1);
static_edge (surface, v2, v3, fe2);
static_edge (surface, v0, v3, fe3);
static_vertex (surface, v1, fv0);
static_vertex (surface, v3, fv1);
static_triangle (surface, fv0, fe1, fe0);
static_triangle (surface, fv1, fe3, fe2);
}

static void static_square_1011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4], fv2[4];

/******
 * 10 *
 * 11 *
 ******/

static_edge (surface, v2, v3, fe0);
static_edge (surface, v2, v1, fe1);
static_vertex (surface, v0, fv0);
static_vertex (surface, v1, fv1);
static_vertex (surface, v3, fv2);
static_triangle (surface, fe0, fv2, fv0);
static_triangle (surface, fv0, fv1, fe1);
static_triangle (surface, fe1, fe0, fv0);
}

static void static_square_1100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4];

/******
 * 11 *
 * 00 *
 ******/

static_edge (surface, v1, v2, fe0);
static_edge (surface, v0, v3, fe1);
static_vertex (surface, v2, fv0);
static_vertex (surface, v3, fv1);
static_triangle (surface, fe0, fv0, fv1);
static_triangle (surface, fv1, fe1, fe0);
}

static void static_square_1101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4], fv2[4];

/******
 * 11 *
 * 10 *
 ******/

static_edge (surface, v1, v2, fe0);
static_edge (surface, v1, v0, fe1);
static_vertex (surface, v0, fv0);
static_vertex (surface, v2, fv1);
static_vertex (surface, v3, fv2);
static_triangle (surface, fe0, fv1, fv2);
static_triangle (surface, fv2, fv0, fe1);
static_triangle (surface, fe1, fe0, fv2);
}

static void static_square_1110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fe0[4], fe1[4];
double fv0[4], fv1[4], fv2[4];

/******
 * 11 *
 * 01 *
 ******/

static_edge (surface, v0, v1, fe0);
static_edge (surface, v0, v3, fe1);
static_vertex (surface, v1, fv0);
static_vertex (surface, v2, fv1);
static_vertex (surface, v3, fv2);
static_triangle (surface, fe0, fv0, fv1);
static_triangle (surface, fv1, fv2, fe1);
static_triangle (surface, fe1, fe0, fv1);
}

static void static_square_1111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
double fv0[4], fv1[4], fv2[4], fv3[4];

static_vertex (surface, v0, fv0);
static_vertex (surface, v1, fv1);
static_vertex (surface, v2, fv2);
static_vertex (surface, v3, fv3);
static_triangle (surface, fv0, fv1, fv2);
static_triangle (surface, fv2, fv3, fv0);
}

static void static_square (static_surface *surface,
int *v0, int *v1, int *v2, int *v3)
{
char *outside;
int n_points, bit;
int a, b, c;
int state;

/**********************************
 * generate triangles from square *
 **********************************/

/**********************************
 * square vertices convention 3 2 *
 * when seen from outside:    0 1 *
 **********************************/

n_points = surface->n_points;
a = surface->a;
b = surface->b;
c = surface->c;
outside = surface->outside;

state = 0;
bit = static_offset (n_points, a + v0[0], b + v0[1], c + v0[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 1;
bit = static_offset (n_points, a + v1[0], b + v1[1], c + v1[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 2;
bit = static_offset (n_points, a + v2[0], b + v2[1], c + v2[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 4;
bit = static_offset (n_points, a + v3[0], b + v3[1], c + v3[2]);
if (GAMGI_ENGINE_BITTEST(outside, bit) == 0) state |= 8;

(*surface->square[state]) (surface, v0, v1, v2, v3);
}

static void static_squares_a0 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int b, c;

n_points = surface->n_points;
v0[0] = 0; v0[1] =  0; v0[2] = 0; v0[3] = 0;
v1[0] = 0; v1[1] = -1; v1[2] = 0; v1[3] = 1;
v2[0] = 0; v2[1] = -1; v2[2] = 1; v2[3] = 2;
v3[0] = 0; v3[1] =  0; v3[2] = 1; v3[3] = 3;

surface->a = 0;
for (c = 0; c < n_points - 1; c++)
  {
  surface->c = c;
  for (b = n_points - 1; b > 0; b--)
    {
    surface->b = b;

    if (b == n_points - 1 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_squares_a1 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int b, c;

n_points = surface->n_points;
v0[0] = 0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = 0; v1[1] = 1; v1[2] = 0; v1[3] = 1;
v2[0] = 0; v2[1] = 1; v2[2] = 1; v2[3] = 2;
v3[0] = 0; v3[1] = 0; v3[2] = 1; v3[3] = 3;

surface->a = n_points - 1;
for (c = 0; c < n_points - 1; c++)
  {
  surface->c = c;
  for (b = 0; b < n_points - 1; b++)
    {
    surface->b = b;

    if (b == 0 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_squares_b0 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int a, c;

n_points = surface->n_points;
v0[0] = 0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = 1; v1[1] = 0; v1[2] = 0; v1[3] = 1;
v2[0] = 1; v2[1] = 0; v2[2] = 1; v2[3] = 2;
v3[0] = 0; v3[1] = 0; v3[2] = 1; v3[3] = 3;

surface->b = 0;
for (c = 0; c < n_points - 1; c++)
  {
  surface->c = c;
  for (a = 0; a < n_points - 1; a++)
    {
    surface->a = a;

    if (a == 0 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_squares_b1 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int a, c;

n_points = surface->n_points;
v0[0] =  0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = -1; v1[1] = 0; v1[2] = 0; v1[3] = 1;
v2[0] = -1; v2[1] = 0; v2[2] = 1; v2[3] = 2;
v3[0] =  0; v3[1] = 0; v3[2] = 1; v3[3] = 3;

surface->b = n_points - 1;
for (c = 0; c < n_points - 1; c++)
  {
  surface->c = c;
  for (a = n_points - 1; a > 0; a--)
    {
    surface->a = a;

    if (a == n_points - 1 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_squares_c0 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int a, b;

n_points = surface->n_points;
v0[0] =  0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = -1; v1[1] = 0; v1[2] = 0; v1[3] = 1;
v2[0] = -1; v2[1] = 1; v2[2] = 0; v2[3] = 2;
v3[0] =  0; v3[1] = 1; v3[2] = 0; v3[3] = 3;

surface->c = 0;
for (b = 0; b < n_points - 1; b++)
  {
  surface->b = b;
  for (a = n_points - 1; a > 0; a--)
    {
    surface->a = a;

    if (a == n_points - 1 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_squares_c1 (static_surface *surface)
{
int v0[4], v1[4], v2[4], v3[4];
int n_points;
int a, b;

n_points = surface->n_points;
v0[0] = 0; v0[1] = 0; v0[2] = 0; v0[3] = 0;
v1[0] = 1; v1[1] = 0; v1[2] = 0; v1[3] = 1;
v2[0] = 1; v2[1] = 1; v2[2] = 0; v2[3] = 2;
v3[0] = 0; v3[1] = 1; v3[2] = 0; v3[3] = 3;

surface->c = n_points - 1;
for (b = 0; b < n_points - 1; b++)
  {
  surface->b = b;
  for (a = 0; a < n_points - 1; a++)
    {
    surface->a = a;

    if (a == 0 || surface->triangles == 0) surface->cache_s = FALSE;
    else static_2d_copy (surface);
    surface->triangles = 0;

    static_square (surface, v0, v1, v2, v3);
    }
  }
}

static void static_realloc (static_surface *surface)
{
gamgi_orbital *orbital;
int *dots;

/**************************************************
 * realloc array to remove spare space in the end *
 **************************************************/

orbital = surface->orbital;
dots = orbital->dots;

orbital->n_points = dots[0] + dots[1];
orbital->points = (double *) realloc (orbital->points,
orbital->n_points * 3 * sizeof (double));
}

static void static_swap (static_surface *surface)
{
gamgi_orbital *orbital;
double *points, *negative, *positive;
int *dots;
int start, end;
char *signal;

/********************************************************** 
 * signal is a bit array where 0 means negative triangle, *
 * and 1 means positive triangle. The disordered array    *
 * looks like 1010111000101 while the ordered array looks *
 * like 1111111000000, so positive triangles come first.  *
 **********************************************************/

orbital = surface->orbital;
dots = orbital->dots;
points = orbital->points + 3 * dots[0];
signal = surface->signal;

start = 0;
end = dots[1] / 3 - 1;

while (1 > 0)
  {
  /************************************************
   * find negative and positive triangles to swap *
   ************************************************/

  while (start < end && GAMGI_ENGINE_BITTEST(signal, start) != 0) start++;
  while (start < end && GAMGI_ENGINE_BITTEST(signal, end) == 0) end--;
  if (start >= end) break;

  /*****************************************
   * swap negative with positive triangles *
   *****************************************/

  negative = points + 9 * start;
  positive = points + 9 * end;
  gamgi_math_vector_swap (negative + 0, positive + 0);
  gamgi_math_vector_swap (negative + 3, positive + 3);
  gamgi_math_vector_swap (negative + 6, positive + 6);
  start++; end--;
  }

/*****************************************************************
 * start must equal the number of positive triangles, so it must *
 * point to the first position AFTER all the positive triangles! *
 *****************************************************************/

if (GAMGI_ENGINE_BITTEST(signal, start) != 0) start++;
dots[2] = dots[1] - 3 * start;
dots[1] = dots[1] - dots[2];
}

static static_surface *static_start (gamgi_orbital *orbital)
{
static_surface *surface;
double constant_r, constant_a, constant_z, za;
int n_points, n_bit, n_char;
int a, b, c;

/***************************
 * allocate main structure *
 ***************************/

surface = (static_surface *) malloc (sizeof (static_surface));
surface->orbital = orbital;

/*************************
 * get orbital constants *
 **************************/

static_radial (orbital->n, orbital->l, &constant_r, &surface->function_r);
static_angular (orbital->l, orbital->m, &constant_a, &surface->function_a);

za = orbital->charge / GAMGI_CHEM_A0;
surface->ho = 2 * za / orbital->n;
constant_z = pow (za, 1.5) * pow (surface->ho, orbital->l);
surface->constant = constant_z * constant_r * constant_a;

/*****************************
 * set sampling slices width *
 *****************************/

surface->slices = orbital->sampling;
surface->width = orbital->radius / surface->slices;

/*************************
 * allocate points array *
 *************************/

orbital->n_points = orbital->dots[0] + GAMGI_CHEM_ORBITAL_SEGMENT;
orbital->points  = (double *) malloc (orbital->n_points * 3 * sizeof (double));

/**********************************************************************
 * malloc triangle signal array: 24 = 3 points/triangle x 8 bits/byte *
 **********************************************************************/

surface->n_signal = GAMGI_CHEM_ORBITAL_SEGMENT / 24;
surface->signal = (char *) malloc (surface->n_signal * sizeof (char));

surface->n_signal = 0;
surface->signal = NULL;

/***********************
 * allocate bit arrays *
 ***********************/

surface->n_points = n_points = 1 + surface->slices;
surface->n_bit = n_bit = n_points * n_points * n_points;
surface->n_char = n_char = GAMGI_ENGINE_BITNSLOTS(n_bit);
surface->above = (char *) malloc (n_char * sizeof (char));
surface->outside = (char *) malloc (n_char * sizeof (char));

/*************************
 * allocate stack arrays *
 *************************/

surface->stack_a = (unsigned char *) malloc (n_bit * sizeof (unsigned char));
surface->stack_b = (unsigned char *) malloc (n_bit * sizeof (unsigned char));
surface->stack_c = (unsigned char *) malloc (n_bit * sizeof (unsigned char));

/************************************************************
 * Initialize, accumulate, how many times each octant face  *
 * will be rendered. When face = 0 the face does not exist, *
 * when face = 2 the face is internal and can be ignored,   *
 * when face = 1 the face is external and must be rendered. *
 ************************************************************/

for (c = 0; c <= 1; c++)
  {
  for (b = 0; b <= 1; b++)
    {
    for (a = 0; a <= 1; a++)
      {
      surface->face_a[a + 0][b][c] = 0;
      surface->face_a[a + 1][b][c] = 0;
      surface->face_b[a][b + 0][c] = 0;
      surface->face_b[a][b + 1][c] = 0;
      surface->face_c[a][b][c + 0] = 0;
      surface->face_c[a][b][c + 1] = 0;
      }
    }
  }

for (c = 0; c <= 1; c++)
  {
  for (b = 0; b <= 1; b++)
    {
    for (a = 0; a <= 1; a++)
      {
      if (orbital->octants[4 * c + 2 * b + a] == TRUE)
        {
        surface->face_a[a + 0][b][c]++;
        surface->face_a[a + 1][b][c]++;
        surface->face_b[a][b + 0][c]++;
        surface->face_b[a][b + 1][c]++;
        surface->face_c[a][b][c + 0]++;
        surface->face_c[a][b][c + 1]++;
        }
      }
    }
  }

surface->tetrahedron[ 0] = static_tetrahedron_0000;
surface->tetrahedron[ 1] = static_tetrahedron_0001;
surface->tetrahedron[ 2] = static_tetrahedron_0010;
surface->tetrahedron[ 3] = static_tetrahedron_0011;
surface->tetrahedron[ 4] = static_tetrahedron_0100;
surface->tetrahedron[ 5] = static_tetrahedron_0101;
surface->tetrahedron[ 6] = static_tetrahedron_0110;
surface->tetrahedron[ 7] = static_tetrahedron_0111;
surface->tetrahedron[ 8] = static_tetrahedron_1000;
surface->tetrahedron[ 9] = static_tetrahedron_1001;
surface->tetrahedron[10] = static_tetrahedron_1010;
surface->tetrahedron[11] = static_tetrahedron_1011;
surface->tetrahedron[12] = static_tetrahedron_1100;
surface->tetrahedron[13] = static_tetrahedron_1101;
surface->tetrahedron[14] = static_tetrahedron_1110;
surface->tetrahedron[15] = static_tetrahedron_1111;

surface->square[ 0] = static_square_0000;
surface->square[ 1] = static_square_0001;
surface->square[ 2] = static_square_0010;
surface->square[ 3] = static_square_0011;
surface->square[ 4] = static_square_0100;
surface->square[ 5] = static_square_0101;
surface->square[ 6] = static_square_0110;
surface->square[ 7] = static_square_0111;
surface->square[ 8] = static_square_1000;
surface->square[ 9] = static_square_1001;
surface->square[10] = static_square_1010;
surface->square[11] = static_square_1011;
surface->square[12] = static_square_1100;
surface->square[13] = static_square_1101;
surface->square[14] = static_square_1110;
surface->square[15] = static_square_1111;

surface->triangle[0] = static_triangle_000;
surface->triangle[1] = static_triangle_001;
surface->triangle[2] = static_triangle_010;
surface->triangle[3] = static_triangle_011;
surface->triangle[4] = static_triangle_100;
surface->triangle[5] = static_triangle_101;
surface->triangle[6] = static_triangle_110;
surface->triangle[7] = static_triangle_111;

return surface;
}

static void static_end (static_surface *surface)
{
free (surface->stack_a);
free (surface->stack_b);
free (surface->stack_c);

free (surface->above);
free (surface->outside);
free (surface->signal);

free (surface);
}

static int static_solid (gamgi_orbital *orbital)
{
static_surface *surface;
char *outside, *above;
int slices, a, b, c;

/*****************
 * allocate data *
 *****************/

surface = static_start (orbital);

/*****************
 * build octants *
 *****************/

outside = surface->outside;
above = surface->above;
slices = surface->slices;

for (c = 0; c <= 1; c++)
  {
  for (b = 0; b <= 1; b++)
    {
    for (a = 0; a <= 1; a++)
      {
      if (orbital->octants[4 * c + 2 * b + a] == TRUE)
        {
        static_clean (above, surface->n_char);
        static_above (surface, a, b, c);

        static_clean (outside, surface->n_char);
        static_outside (surface, a * slices, b * slices, c * slices);

        /******************************************
         * scan cubes to build orbital 3d surface *
         ******************************************/

        static_cubes (surface);

        /*************************************************************
         * scan squares to build octant faces a-, a+, b-, b+, c-, c+ *
         *************************************************************/

        if (surface->face_a[a + 0][b][c] == TRUE) static_squares_a0 (surface);
        if (surface->face_a[a + 1][b][c] == TRUE) static_squares_a1 (surface);
        if (surface->face_b[a][b + 0][c] == TRUE) static_squares_b0 (surface);
        if (surface->face_b[a][b + 1][c] == TRUE) static_squares_b1 (surface);
        if (surface->face_c[a][b][c + 0] == TRUE) static_squares_c0 (surface);
        if (surface->face_c[a][b][c + 1] == TRUE) static_squares_c1 (surface);
        }
      }
    }
  }

/*****************************************************************
 * 1) realloc points array to eliminate spare space at the end,  *
 * 2) swap triangles, so positive triangles come before negative *
 *****************************************************************/

static_realloc (surface);
if (orbital->phase == TRUE) static_swap (surface);

/***************
 * remove data *
 ***************/

static_end (surface);

return TRUE;
}

gamgi_bool gamgi_chem_orbital_number (int n, int l, int m)
{
/************************************************
 * accepted range for n [1,6], l [0,5], m[-5,5] *
 ************************************************/

if (l > n - 1) return FALSE;
if (m < -l || m > l) return FALSE;

return TRUE;
}

void gamgi_chem_orbital_m (int l, gamgi_bool *sensitive)
{
switch (l)
  {
  case 0:
  sensitive[0] = FALSE;
  sensitive[1] = FALSE;
  sensitive[2] = FALSE;
  sensitive[3] = FALSE;
  sensitive[4] = FALSE;
  sensitive[6] = FALSE;
  sensitive[7] = FALSE;
  sensitive[8] = FALSE;
  sensitive[9] = FALSE;
  sensitive[10] = FALSE;
  break;

  case 1:
  sensitive[0] = FALSE;
  sensitive[1] = FALSE;
  sensitive[2] = FALSE;
  sensitive[3] = FALSE;
  sensitive[4] = TRUE;
  sensitive[6] = TRUE;
  sensitive[7] = FALSE;
  sensitive[8] = FALSE;
  sensitive[9] = FALSE;
  sensitive[10] = FALSE;
  break;

  case 2:
  sensitive[0] = FALSE;
  sensitive[1] = FALSE;
  sensitive[2] = FALSE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[6] = TRUE;
  sensitive[7] = TRUE;
  sensitive[8] = FALSE;
  sensitive[9] = FALSE;
  sensitive[10] = FALSE;
  break;

  case 3:
  sensitive[0] = FALSE;
  sensitive[1] = FALSE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[6] = TRUE;
  sensitive[7] = TRUE;
  sensitive[8] = TRUE;
  sensitive[9] = FALSE;
  sensitive[10] = FALSE;
  break;

  case 4:
  sensitive[0] = FALSE;
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[6] = TRUE;
  sensitive[7] = TRUE;
  sensitive[8] = TRUE;
  sensitive[9] = TRUE;
  sensitive[10] = FALSE;
  break;

  case 5:
  sensitive[0] = TRUE;
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[6] = TRUE;
  sensitive[7] = TRUE;
  sensitive[8] = TRUE;
  sensitive[9] = TRUE;
  sensitive[10] = TRUE;
  break;
  }
}

void gamgi_chem_orbital_l (int n, gamgi_bool *sensitive)
{
switch (n)
  {
  case 1:
  sensitive[1] = FALSE;
  sensitive[2] = FALSE;
  sensitive[3] = FALSE;
  sensitive[4] = FALSE;
  sensitive[5] = FALSE;
  break;

  case 2:
  sensitive[1] = TRUE;
  sensitive[2] = FALSE;
  sensitive[3] = FALSE;
  sensitive[4] = FALSE;
  sensitive[5] = FALSE;
  break;

  case 3:
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = FALSE;
  sensitive[4] = FALSE;
  sensitive[5] = FALSE;
  break;

  case 4:
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = FALSE;
  sensitive[5] = FALSE;
  break;

  case 5:
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[5] = FALSE;
  break;

  case 6:
  sensitive[1] = TRUE;
  sensitive[2] = TRUE;
  sensitive[3] = TRUE;
  sensitive[4] = TRUE;
  sensitive[5] = TRUE;
  break;
  }
}

int gamgi_chem_orbital_sampling (int n, int style)
{
if (style == GAMGI_MESA_WIRED)
  return GAMGI_CHEM_ORBITAL_POINTS * n;

if (style == GAMGI_MESA_SOLID)
  return GAMGI_CHEM_ORBITAL_SLICES;

/*****************************************
 * control should never reach this point *
 *****************************************/

return FALSE;
}

double gamgi_chem_orbital_radius (int n, int l, int m, double charge)
{
gamgi_radial function_r;
gamgi_angular function_a;
double za, r, ho, rho, r_max;
double constant, constant_a, constant_z, constant_r;
double d, d_old, d_old_old;
double f;

if (n < 1 || n > 6) return 0.0;
if (l < 0 || l > n - 1) return 0.0;
if (m < -l || m > l) return 0.0;

za = charge / GAMGI_CHEM_A0;
ho = 2 * za / n;
constant_z = pow (za, 1.5) * pow (ho, l);

static_radial (n, l, &constant_r, &function_r);
static_angular (l, m, &constant_a, &function_a);

/***********************************************
 * get the last maximum for the radial density *
 ***********************************************/

r_max = GAMGI_CHEM_ORBITAL_R_MAX;

d_old = d_old_old = 0.0;
constant = constant_r * constant_z;
for (r = 0.0; r < GAMGI_CHEM_ORBITAL_R_MAX; r += GAMGI_CHEM_ORBITAL_R_STEP)
  {
  rho = r * ho;
  f = constant * exp (-rho / 2) * pow (r, l) * (*function_r) (rho);
  d = f * f * r * r;
  if (d < d_old && d_old > d_old_old)
    r_max = r - GAMGI_CHEM_ORBITAL_R_STEP;
  d_old_old = d_old;
  d_old = d;
  }

/**********************************************
 * start checking the default maximum density *
 * some distance after the last maximum       *
 **********************************************/

r_max += GAMGI_CHEM_ORBITAL_R_SHIFT;

/*********************************************
 * find the radius where the density becomes *
 * lower than the default maximum density    *
 *********************************************/

constant *= constant_a;
for (r = r_max; r < GAMGI_CHEM_ORBITAL_R_MAX; r += GAMGI_CHEM_ORBITAL_R_STEP)
  {
  rho = r * ho;
  f = constant * exp (-rho / 2) * (*function_r) (rho);
  if (static_density (r, f, function_a) == FALSE) return r;
  }

return GAMGI_CHEM_ORBITAL_R_MAX;
}

void gamgi_chem_orbital_name (int n0, int l0, int m0, char *token)
{
char *name[36] = {
"s",
"py", "pz", "px",
"dxy", "dyz", "dz2", "dxz", "dx2_y2",
"fy_3x2_y2", "fxyz", "fyz2", "fz3", "fxz2", "fz_x2_y2", "fx_x2_3y2",
"gxy_x2_y2", "gzy3", "gz2_xy", "gz3y", "gz4", "gz3x", "gz2_x2_y2", "gzx3", "gx4_y4",
"hy5", "hxyz_x2_y2", "hz2y3", "hz3_xy", "hz4y", "hz5", "hz4x", "hz3_x2_y2", "hz2x3", "hz_x4_y4", "hx5"};
int l, m, offset = 0;

for (l = 0; l <= 5; l++)
  {
  for (m = -l; m <= l; m++)
    {
    if (l == l0 && m == m0)
      { sprintf (token, "%d%s", n0, name[offset]); return; }
    offset++;
    }
  }

sprintf (token, "Orbital");
}

gamgi_bool gamgi_chem_orbital_create (gamgi_orbital *orbital)
{
if (orbital->frame == TRUE) orbital->dots[0] = 8;

if (orbital->style == GAMGI_MESA_WIRED && static_wired (orbital) == FALSE)
  return FALSE;
if (orbital->style == GAMGI_MESA_SOLID && static_solid (orbital) == FALSE)
  return FALSE;

if (orbital->frame == TRUE) static_frame (orbital);

return TRUE;
}
