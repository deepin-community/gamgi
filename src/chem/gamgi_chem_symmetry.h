/************************************************************************* 
 *                                                                        
 * $GAMGI/src/chem/gamgi_chem_symmetry.h                                  
 *                                                                        
 *  Point group symmetry analyzer.                                        
 *                                                                        
 *  Copyright (C) 1996, 2003 Serguei Patchkovskii                         
 *  Serguei.Patchkovskii@nrc.ca                                           
 *                                                                        
 *  This program is free software; you can redistribute it and/or modify  
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 2 of the License, or     
 *  (at your option) any later version.                                   
 *                                                                        
 *  This program is distributed in the hope that it will be useful,       
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         
 *  GNU General Public License for more details.                          
 *                                                                        
 *  You should have received a copy of the GNU General Public License     
 *  along with this program; if not, write to the Free Software           
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             
 *                                                                        
 */

/**********************
 * internal functions *
 **********************

static double pow2 (double x);

static int static_divisors (int number);

static int static_redundant (int n, int p);

static int static_multiplicity (int n);

static void static_invert (gamgi_symmetry *symmetry,
gamgi_element *center, gamgi_atom *from, gamgi_atom *to);

static void static_reflect (gamgi_symmetry *symmetry,
gamgi_element *plane, gamgi_atom *from, gamgi_atom *to);

static void static_rotate (gamgi_symmetry *symmetry,
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to);

static void static_rotate_reflect (gamgi_symmetry *symmetry,
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to);

static int static_apply (gamgi_symmetry *symmetry, gamgi_element *element);

static int static_order (gamgi_symmetry *symmetry, gamgi_element *element);

static int static_same (gamgi_symmetry *symmetry, gamgi_element *a, gamgi_element *b);

static gamgi_element *static_element (void);

static gamgi_element *static_create (gamgi_symmetry *symmetry);

static gamgi_element *static_copy (gamgi_element *old);

static void static_insert (gamgi_element ***elements,
int *n_elements, int new);

static void static_insert_copy (gamgi_element ***elements,
int *n_elements, int old, int new);

static void static_resize (gamgi_element ***elements, int *n_elements);

static void static_remove (gamgi_element *element);

static void static_restrict (gamgi_element ***elements, int *n_elements);

static void static_delete (gamgi_element ***elements, 
int *n_elements, int offset);

static int static_quality (gamgi_symmetry *symmetry, gamgi_element *element);

static int static_evaluate (gamgi_symmetry *symmetry,
gamgi_element *element, double *sum, double *max);

static void static_get (gamgi_element *element, double *parameters);

static void static_set (gamgi_element *element, double *parameters);

static int static_optimize (gamgi_symmetry *symmetry, gamgi_element *element);

static int static_refine (gamgi_symmetry *symmetry, gamgi_element *element, int apply);

static void static_invert (gamgi_symmetry *symmetry, 
gamgi_element *center, gamgi_atom *from, gamgi_atom *to);

static gamgi_element *static_try_i (gamgi_symmetry *symmetry);

static void static_reflect (gamgi_symmetry *symmetry, 
gamgi_element *plane, gamgi_atom *from, gamgi_atom *to);

static gamgi_element *static_try_m (gamgi_symmetry *symmetry, int i, int j);

static gamgi_element *static_try_m0 (gamgi_symmetry *symmetry);

static void static_rotate (gamgi_symmetry *symmetry, 
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to);

static gamgi_element *static_try_C0 (gamgi_symmetry *symmetry);

static gamgi_element *static_try_Cm (gamgi_symmetry *symmetry);

static gamgi_element *static_try_C2 (gamgi_symmetry *symmetry, 
int i, int j, double *support);

static gamgi_element *static_try_axis (gamgi_symmetry *symmetry, 
double a[3], double b[3], double c[3]);

static gamgi_element *static_try_Cn (gamgi_symmetry *symmetry, 
int ia, int ib, int ic);

static void static_rotate_reflect (gamgi_symmetry *symmetry, 
gamgi_element *axis, gamgi_atom *from, gamgi_atom *to);

static gamgi_element *static_try_S0 (gamgi_symmetry *symmetry);

static gamgi_element *static_try_Sn (gamgi_symmetry *symmetry, 
int ia, int ib, int ic);

static void static_center (gamgi_symmetry *symmetry);

static void static_find_i (gamgi_symmetry *symmetry);

static void static_find_m (gamgi_symmetry *symmetry);

static void static_find_C0 (gamgi_symmetry *symmetry);

static void static_find_C2 (gamgi_symmetry *symmetry);

static void static_find_Cn (gamgi_symmetry *symmetry);

static void static_find_S0 (gamgi_symmetry *symmetry);

static void static_find_Sn (gamgi_symmetry *symmetry);

static void static_find (gamgi_symmetry *symmetry);

static int static_compare (const void *a, const void *b);

static void static_sort (gamgi_element **elements, int n_elements);

static void static_max_m (double *direction, gamgi_element **m, int n_m);

static void static_max_C2 (double *direction, gamgi_symmetry *symmetry);

static void static_min_C2 (double *direction, gamgi_element **C, int n_C);

static void static_sum (gamgi_element **elements, int n_elements, int **sum);

static void static_operations (GtkTextBuffer *buffer, 
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_elements_i (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_elements_m (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_elements_C (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_elements_S (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_elements (GtkTextBuffer *buffer,
GtkTextIter *iter, gamgi_symmetry *symmetry);

static void static_report (char *group, gamgi_symmetry *symmetry);

static void static_Cs (double *d_m, double e_m, gamgi_symmetry *symmetry);

static void static_C2 (double *d_C2, double e_C2, gamgi_symmetry *symmetry);

static void static_C3 (double *d_C3, double e_C3, gamgi_symmetry *symmetry);

static void static_C4 (double *d_C4, double *d_C2, double e_C4, double e_C2, 
gamgi_symmetry *symmetry);

static void static_C6 (double *d_C6, double *d_C3, double *d_C2, double e_C6, 
double e_C3, double e_C2, gamgi_symmetry *symmetry);

static void static_S4 (double *d_S4, double *d_C2, double e_S4, double e_C2, 
gamgi_symmetry *symmetry);

static void static_S6 (double *d_S6, double *d_C3, double e_S6, double e_C3, 
double e_i, gamgi_symmetry *symmetry);

static void static_D2 (double *d_C2, double *d_Ch, double e_C2, double e_Ch, 
gamgi_symmetry *symmetry);

static void static_D3 (double *d_C3, double *d_Ch, double e_C3, double e_Ch, 
gamgi_symmetry *symmetry);

static void static_D4 (double *d_C4, double *d_C2, double *d_Ch, double e_C4, 
double e_C2, double e_Ch, gamgi_symmetry *symmetry);
 
static void static_D6 (double *d_C6, double *d_C3, double *d_C2, double *d_Ch,
double e_C6, double e_C3, double e_C2, double e_Ch, gamgi_symmetry *symmetry);

static void static_C3v (double *d_C3, double *d_m, double e_C3, double e_m, 
gamgi_symmetry *symmetry);

static void static_C4v (double *d_C4, double *d_C2, double *d_m, double e_C4, 
double e_C2, double e_m, gamgi_symmetry *symmetry);

static void static_C6v (double *d_C6, double *d_C3, double *d_C2, double *d_m,
double e_C6, double e_C3, double e_C2, double e_m, gamgi_symmetry *symmetry);

static void static_C2h (double *d_C2, double *d_mh, double e_C2, double e_i, 
double e_mh, gamgi_symmetry *symmetry);

static void static_C4h (double *d_C4, double *d_C2, double *d_S4, double *d_mh, 
double e_C4, double e_C2, double e_S4, double e_i, double e_mh, 
gamgi_symmetry *symmetry);

static void static_C6h (double *d_C6, double *d_C3, double *d_C2, double *d_S6,
double *d_S3, double *d_mh, double e_C6, double e_C3, double e_C2, double e_S6, 
double e_S3, double e_i, double e_mh, gamgi_symmetry *symmetry);

static void static_D4h (double *d_C4, double *d_C2, double *d_Ch, double *d_S4, 
double *d_mh, double *d_m, double e_C4, double e_C2, double e_Ch, double e_S4, 
double e_i, double e_mh, double e_m, gamgi_symmetry *symmetry);

static void static_D6h (double *d_C6, double *d_C3, double *d_C2, double *d_Ch, 
double *d_S6, double *d_S3, double *d_mh, double *d_m, double e_C6, double e_C3, 
double e_C2, double e_Ch, double e_S6, double e_S3, double e_i, double e_mh, 
double e_m, gamgi_symmetry *symmetry);

static void static_T (double *rotate, double e_C3, double e_C2, 
gamgi_symmetry *symmetry);

static void static_Th (double *rotate, double e_C3, double e_C2, double e_S6, 
double e_i, double e_m, gamgi_symmetry *symmetry);

static void static_Oh (double *rotate, double e_C4, double e_C3, double e_C2, 
double e_C2d, double e_S6, double e_S4, double e_i, double e_m, double e_md,
gamgi_symmetry *symmetry);

static void static_space_C0v (char *group, gamgi_symmetry *symmetry);

static void static_space_D0h (char *group, gamgi_symmetry *symmetry);

static void static_space_Kh (char *group, gamgi_symmetry *symmetry);

static void static_space_Cn (gamgi_symmetry *symmetry);

static void static_space_Dn (gamgi_symmetry *symmetry);

static void static_space_Sn (gamgi_symmetry *symmetry);

static void static_space_Cnv (gamgi_symmetry *symmetry);

static void static_space_Cnh (gamgi_symmetry *symmetry);

static void static_space_Dnh (gamgi_symmetry *symmetry);

static void static_space_Dnd (gamgi_symmetry *symmetry);

static void static_space_I (gamgi_symmetry *symmetry);

static void static_space_Ih (gamgi_symmetry *symmetry);

static void static_chemical_C1 (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Cs (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Ci (char *group, gamgi_symmetry *symmetry);

static void static_chemical_C0v (char *group, gamgi_symmetry *symmetry);

static void static_chemical_D0h (char *group, gamgi_symmetry *symmetry)

static void static_chemical_Kh (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Cn (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Dn (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Sn (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Cnv (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Cnh (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Dnh (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Dnd (char *group, gamgi_symmetry *symmetry);

static void static_chemical_T (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Td (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Th (char *group, gamgi_symmetry *symmetry);

static void static_chemical_O (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Oh (char *group, gamgi_symmetry *symmetry);

static void static_chemical_I (char *group, gamgi_symmetry *symmetry);

static void static_chemical_Ih (char *group, gamgi_symmetry *symmetry);

static void static_group (char *group, gamgi_symmetry *symmetry);

static gamgi_symmetry *static_start (gamgi_object *object,
gamgi_bool periodic, float initial, float final, GtkWidget *text);

static void static_end (gamgi_symmetry *symmetry);

 **********************
 * external functions *
 **********************/

gamgi_bool gamgi_chem_symmetry (gamgi_object *object, 
gamgi_bool all, float initial, float final, GtkWidget *text);

/*************
 * Rationale *
 *************

Implemented, needs cleaning.

 *************
 * Rationale *
 *************/
