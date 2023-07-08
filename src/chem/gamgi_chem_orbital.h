/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_orbital.h
 *
 * Copyright (C); 2012,2013 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static double static_1s (double r);

static double static_2s (double r);
static double static_2p (double r);

static double static_3s (double r);
static double static_3p (double r);
static double static_3d (double r);

static double static_4s (double r);
static double static_4p (double r);
static double static_4d (double r);
static double static_4f (double r);

static double static_5s (double r);
static double static_5p (double r);
static double static_5d (double r);
static double static_5f (double r);
static double static_5g (double r);

static double static_6s (double r);
static double static_6p (double r);
static double static_6d (double r);
static double static_6f (double r);
static double static_6g (double r);
static double static_6h (double r);

static void static_radial (int n, int l, double *c, gamgi_radial *f);

static double static_s (double x, double y, double z, double r);

static double static_py (double x, double y, double z, double r);
static double static_pz (double x, double y, double z, double r);
static double static_px (double x, double y, double z, double r);

static double static_dxy (double x, double y, double z, double r);
static double static_dyz (double x, double y, double z, double r);
static double static_dz2 (double x, double y, double z, double r);
static double static_dxz (double x, double y, double z, double r);
static double static_dx2_y2 (double x, double y, double z, double r);

static double static_fy_3x2_y2 (double x, double y, double z, double r);
static double static_fxyz (double x, double y, double z, double r);
static double static_fyz2 (double x, double y, double z, double r);
static double static_fz3 (double x, double y, double z, double r);
static double static_fxz2 (double x, double y, double z, double r);
static double static_fz_x2_y2 (double x, double y, double z, double r);
static double static_fx_x2_3y2 (double x, double y, double z, double r);

static double static_gxy_x2_y2 (double x, double y, double z, double r);
static double static_gzy3 (double x, double y, double z, double r);
static double static_gz2_xy (double x, double y, double z, double r);
static double static_gz3y (double x, double y, double z, double r);
static double static_gz4 (double x, double y, double z, double r);
static double static_gz3x (double x, double y, double z, double r);
static double static_gz2_x2_y2 (double x, double y, double z, double r);
static double static_gzx3 (double x, double y, double z, double r);
static double static_gx4_y4 (double x, double y, double z, double r);

static double static_hy5 (double x, double y, double z, double r);
static double static_hxyz_x2_y2 (double x, double y, double z, double r);
static double static_hz2y3 (double x, double y, double z, double r);
static double static_hz3_xy (double x, double y, double z, double r);
static double static_hz4y (double x, double y, double z, double r);
static double static_hz5 (double x, double y, double z, double r);
static double static_hz4x (double x, double y, double z, double r);
static double static_hz3_x2_y2 (double x, double y, double z, double r);
static double static_hz2x3 (double x, double y, double z, double r);
static double static_hz_x4_y4 (double x, double y, double z, double r);
static double static_hx5 (double x, double y, double z, double r);

static void static_angular (int l, int m, double *c, gamgi_angular *f);

static double static_density (double r, double f, gamgi_angular function_a);

static void static_frame (gamgi_orbital *orbital);

static gamgi_bool static_wired (gamgi_orbital *orbital);

static void static_clean (char *array, int n_char);

static int static_offset (int n_points, int a, int b, int c);

static void static_above (static_surface *surface, int a, int b, int c);

static void static_outside (static_surface *surface, int a, int b, int c);

static void static_points (static_surface *surface,
double *f1, double *f2, double *f3);

static void static_signal (static_surface *surface, int positive);

static void static_reset (static_surface *surface);

static void static_3d_copy (static_surface *surface);

static void static_2d_copy (static_surface *surface);

static void static_vertex (static_surface *surface, int *v, double *f);

static void static_edge (static_surface *surface, int *v0, int *v1, double *f);

static void static_zero (double *f0, double *f1, double *g);

static void static_triangle_000 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_001 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_010 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_011 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_100 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_101 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_110 (static_surface *surface,
double *f0, double *f1, double *f2);
static void static_triangle_111 (static_surface *surface,
double *f0, double *f1, double *f2);

static void static_triangle (static_surface *surface,
double *f1, double *f2, double *f3);

static void static_tetrahedron_0000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_0111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_tetrahedron_1111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);

static void static_tetrahedron (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_cubes (static_surface *surface);

static void static_square_0000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_0111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1000 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1001 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3); 
static void static_square_1010 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1011 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1100 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1101 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1110 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);
static void static_square_1111 (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);

static void static_square (static_surface *surface,
int *v0, int *v1, int *v2, int *v3);

static void static_squares_a0 (static_surface *surface);
static void static_squares_a1 (static_surface *surface);
static void static_squares_b0 (static_surface *surface);
static void static_squares_b1 (static_surface *surface);
static void static_squares_c0 (static_surface *surface);
static void static_squares_c1 (static_surface *surface);

static void static_realloc (static_surface *surface);

static void static_swap (static_surface *surface);

static static_surface *static_start (gamgi_orbital *orbital);

static void static_end (static_surface *surface);

static int static_solid (gamgi_orbital *orbital);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_chem_orbital_number (int n, int l, int m);

void gamgi_chem_orbital_m (int l, gamgi_bool *sensitive);

void gamgi_chem_orbital_l (int n, gamgi_bool *sensitive);

int gamgi_chem_orbital_sampling (int n, int style);

double gamgi_chem_orbital_radius (int n, int l, int m, double charge);

void gamgi_chem_orbital_name (int n0, int l0, int m0, char *token);

gamgi_bool gamgi_chem_orbital_create (gamgi_orbital *orbital);

/*************
 * Rationale *
 *************

Implemented wired and solid models. Jun 2013.

 *************
 * Rationale *
 *************/
