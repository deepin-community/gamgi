/******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_geometry.c
 *
 * Copyright (C) 2007 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_dlist.h"
#include "gamgi_engine_array.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_math_hash.h"
#include "gamgi_math_measure.h"
#include "gamgi_chem_atom.h"

#define GAMGI_CHEM_GEOMETRY_SIZE 10
#define GAMGI_CHEM_GEOMETRY_STEP 2
#define GAMGI_CHEM_GEOMETRY_MULTIPLIER 37

typedef struct _gamgi_geometry {
gamgi_dlist *dlist;
gamgi_layer *layer;
gamgi_darray *data;
gamgi_iarray *length_array, *angle_array, *torsion_array;
gamgi_slist **length_table, **angle_table, **torsion_table;
int size, step, multiplier;
int n_atoms;
} gamgi_geometry;

static int static_compare_length (const void *a, const void *b)
{
int *sequence_a = (int *) a;
int *sequence_b = (int *) b;

/*******************************************************
 * elements are swaped if the return value is positive *
 *******************************************************/

if (sequence_a[0] > sequence_b[0]) return 1;

if (sequence_a[0] == sequence_b[0])
  {
  if (sequence_a[1] > sequence_b[1]) return 1;
  }

return -1;
}

static int static_compare_angle (const void *a, const void *b)
{
int *sequence_a = (int *) a;
int *sequence_b = (int *) b;

/*******************************************************
 * elements are swaped if the return value is positive *
 *******************************************************/

if (sequence_a[0] > sequence_b[0]) return 1;

if (sequence_a[0] == sequence_b[0])
  {
  if (sequence_a[1] > sequence_b[1]) return 1;

  if (sequence_a[1] == sequence_b[1])
    {
    if (sequence_a[2] > sequence_b[2]) return 1;
    }
  }


return -1;
}

static int static_compare_torsion (const void *a, const void *b)
{
int *sequence_a = (int *) a;
int *sequence_b = (int *) b;

/*******************************************************
 * elements are swaped if the return value is positive *
 *******************************************************/

if (sequence_a[0] > sequence_b[0]) return 1;

if (sequence_a[0] == sequence_b[0])
  {
  if (sequence_a[1] > sequence_b[1]) return 1;

  if (sequence_a[1] == sequence_b[1])
    { 
    if (sequence_a[2] > sequence_b[2]) return 1;

    if (sequence_a[2] == sequence_b[2])
      {
      if (sequence_a[3] > sequence_b[3]) return 1;
      }
    }
  }

return -1;
}

static void static_report_length (gamgi_geometry *geometry, 
int n, char *token, int *occurrences, int *offset)
{
char element1[5], element2[5];
int *driver;

driver = geometry->length_array->array + 4*n;

gamgi_chem_atom_name (driver[0],  element1);
gamgi_chem_atom_name (driver[1],  element2);

sprintf (token, "%s-%s", element1, element2);

*occurrences = driver[2];
*offset = driver[3];
}

static void static_report_angle (gamgi_geometry *geometry, 
int n, char *token, int *occurrences, int *undefined, int *offset)
{
char element1[5], element2[5], element3[5];
int *driver;

driver = geometry->angle_array->array + 6*n;

gamgi_chem_atom_name (driver[0],  element1);
gamgi_chem_atom_name (driver[1],  element2);
gamgi_chem_atom_name (driver[2],  element3);

sprintf (token, "%s-%s-%s", element1, element2, element3);

*occurrences = driver[3];
*undefined = driver[4];
*offset = driver[5];
}

static void static_report_torsion (gamgi_geometry *geometry, 
int n, char *token, int *occurrences, int *undefined, int *offset)
{
char element1[5], element2[5], element3[5], element4[5];
int *driver;

driver = geometry->torsion_array->array + 7*n;

gamgi_chem_atom_name (driver[0],  element1);
gamgi_chem_atom_name (driver[1],  element2);
gamgi_chem_atom_name (driver[2],  element3);
gamgi_chem_atom_name (driver[3],  element4);

sprintf (token, "%s-%s-%s-%s", element1, element2, element3, element4);

*occurrences = driver[4];
*undefined = driver[5];
*offset = driver[6];
}

static void static_data (gamgi_geometry *geometry, int offset, 
int occurrences, double *min, double *max, double *average, 
double *deviation)
{
double *data = geometry->data->array + 4 * offset;

/*************************************************************************
 * taking the absolute value is needed to avoid sqrt negative arguments, *
 * which can occur when deviation is 0, due to computer lack of precison *
 *************************************************************************/

*min = data[0];
*max = data[1];
*average = data[2] / occurrences;
*deviation = sqrt (fabs (data[3] / occurrences - (*average) * (*average)));
}

static void static_report (gamgi_geometry *geometry, GtkWidget *text)
{
GtkTextBuffer *buffer;
GtkTextIter iter;
char token[GAMGI_ENGINE_TOKEN];
double min, max, average, deviation;
int total, occurrences, undefined, valid;
int n, offset;

/*********************
 * initialize buffer *
 *********************/

buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nBond Lengths\n");

qsort (geometry->length_array->array, geometry->length_array->offset,
4 * sizeof (int), static_compare_length);

total = geometry->length_array->offset;
for (n = 0; n < total; n++)
  {
  static_report_length (geometry, n, token, &occurrences, &offset);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold", "\n%s\n", token);

  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Occurrences: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", occurrences);

  static_data (geometry, offset, occurrences, &min, &max, &average, &deviation);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
  gamgi->gamgi->length, min);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
  gamgi->gamgi->length, max);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
  gamgi->gamgi->length, average);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
  gamgi->gamgi->length, deviation);
  }

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nBond Angles\n");

qsort (geometry->angle_array->array, geometry->angle_array->offset,
6 * sizeof (int), static_compare_angle);

total = geometry->angle_array->offset;
for (n = 0; n < total; n++)
  {
  static_report_angle (geometry, n, token, &occurrences, &undefined, &offset);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold", "\n%s\n", token);

  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Occurrences: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", occurrences);

  if (undefined > 0)
    {
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Undefined: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", undefined);
    }

  valid = occurrences - undefined;
  if (valid > 0)
    {
    static_data (geometry, offset, valid, &min, &max, &average, &deviation);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, min);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, max);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, average);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, deviation);
    }
  }

gamgi_gtk_dialog_text_insert (buffer, &iter, "size_large", "color_bold", "\nBond Torsions\n");

qsort (geometry->torsion_array->array, geometry->torsion_array->offset,
7 * sizeof (int), static_compare_torsion);

total = geometry->torsion_array->offset;
for (n = 0; n < total; n++)
  {
  static_report_torsion (geometry, n, token, &occurrences, &undefined, &offset);
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_bold", "\n%s\n", token);

  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Occurrences: ");
  gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", occurrences);

  if (undefined > 0)
    {
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Undefined: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%d\n", undefined);
    }

  valid = occurrences - undefined;
  if (valid > 0)
    {
    static_data (geometry, offset, valid, &min, &max, &average, &deviation);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Minimum: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, min);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Maximum: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, max);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Average: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, average);
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_text", "Deviation: ");
    gamgi_gtk_dialog_text_insert (buffer, &iter, "size_normal", "color_link", "%.*f\n", 
    gamgi->gamgi->angle, deviation);
    }
  }

}

static void static_measure_length (gamgi_atom *atom1, gamgi_atom *atom2, 
gamgi_geometry *geometry)
{
gamgi_slist **table;
gamgi_slist *slist;
char label[GAMGI_ENGINE_TOKEN];
double *data;
double value[4];
double length;
int *driver;
int sequence[4];
int offset;
unsigned int hash;

/******************************************************
 * make sure each sequence is counted once, not twice *
 ******************************************************/

if (atom1->mark < atom2->mark) return;

/*********************************************************
 * order each sequence with increasing atom elements, so *
 * sequences with the same elements are grouped together *
 *********************************************************/

if (atom1->element > atom2->element)
  {
  sequence[0] = atom2->element;
  sequence[1] = atom1->element;
  }
else
  {
  sequence[0] = atom1->element;
  sequence[1] = atom2->element;
  }

/********************************************************
 * measure bond length and prepare to accumulate data   *
 * to get minimum, maximum, average, standard deviation *
 ********************************************************/

gamgi_math_measure_length_2atom (atom1, atom2, &length);

value[0] = length;
value[1] = value[0];
value[2] = value[0];
value[3] = value[0] * value[0];

/*******************************
 * create label for hash table *
 *******************************/

sprintf (label, "%d%d", sequence[0], sequence[1]);

/***********************************************
 * if sequence already exists, accumulate data *
 ***********************************************/

table = geometry->length_table;
hash = gamgi_math_hash_value (label, geometry->multiplier, geometry->size);

for (slist = table[hash]; slist != NULL; slist = slist->next)
  {
  offset = GAMGI_POINTER_TO_INT (slist->data);
  driver = geometry->length_array->array + 4 * offset;

  if (driver[0] == sequence[0] && driver[1] == sequence[1])
    {
    /***************************
     * accumulate torsion data *
     ***************************/

    driver[2] += 1;

    data = geometry->data->array + 4 * driver[3];
    if (value[0] < data[0]) data[0] = value[0];
    if (value[1] > data[1]) data[1] = value[1];
    data[2] += value[2];
    data[3] += value[3];
    return;
    }
  }

/***********************************************************
 * if sequence does not exist, enter new sequence and data *
 ***********************************************************/

offset = geometry->length_array->offset;
table[hash] = gamgi_math_hash_add (table[hash], GAMGI_INT_TO_POINTER (offset));

sequence[2] = 1;
sequence[3] = geometry->data->offset;
gamgi_engine_iarray_push (geometry->length_array, sequence);

gamgi_engine_darray_push (geometry->data, value);
}

static void static_measure_angle (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_geometry *geometry)
{
gamgi_slist **table;
gamgi_slist *slist;
char label[GAMGI_ENGINE_TOKEN];
double *data;
double value[4];
double angle;
int *driver;
int sequence[6];
int offset, undefined;
unsigned int hash;

/******************************************************
 * make sure each sequence is counted once, not twice *
 ******************************************************/

if (atom1->mark < atom3->mark) return;

/*********************************************************
 * order each sequence with increasing atom elements, so *
 * sequences with the same elements are grouped together *
 *********************************************************/

sequence[1] = atom2->element;
if (atom1->element > atom3->element)
  {
  sequence[0] = atom3->element;
  sequence[2] = atom1->element;
  }
else
  {
  sequence[0] = atom1->element;
  sequence[2] = atom3->element;
  }

/********************************************************
 * measure bond angle and prepare to accumulate data    *
 * to get minimum, maximum, average, standard deviation *
 ********************************************************/

undefined = 0;
if (gamgi_math_measure_angle_3atom (atom1,
atom2, atom3, &angle, geometry->layer) == FALSE) undefined = 1;

if (undefined == 0)
  {
  value[0] = fabs (angle);
  value[1] = value[0];
  value[2] = value[0];
  value[3] = value[0] * value[0];
  }
else
  {
  value[0] = DBL_MAX;
  value[1] = -DBL_MAX;
  value[2] = 0;
  value[3] = 0;
  }

/*******************************
 * create label for hash table *
 *******************************/

sprintf (label, "%d%d%d", sequence[0], sequence[1], sequence[2]);

/***********************************************
 * if sequence already exists, accumulate data *
 ***********************************************/

table = geometry->angle_table;
hash = gamgi_math_hash_value (label, geometry->multiplier, geometry->size);

for (slist = table[hash]; slist != NULL; slist = slist->next)
  {
  offset = GAMGI_POINTER_TO_INT (slist->data);
  driver = geometry->angle_array->array + 6 * offset;
  if (driver[0] == sequence[0] && driver[1] == sequence[1] &&
  driver[2] == sequence[2])
    {
    /*************************
     * accumulate angle data *
     *************************/

    driver[3] += 1;
    driver[4] += undefined;

    data = geometry->data->array + 4 * driver[5];
    if (value[0] < data[0]) data[0] = value[0];
    if (value[1] > data[1]) data[1] = value[1];
    data[2] += value[2];
    data[3] += value[3];
    return;
    }
  }

/***********************************************************
 * if sequence does not exist, enter new sequence and data *
 ***********************************************************/

offset = geometry->angle_array->offset;
table[hash] = gamgi_math_hash_add (table[hash], GAMGI_INT_TO_POINTER (offset));

sequence[3] = 1;
sequence[4] = undefined;
sequence[5] = geometry->data->offset;
gamgi_engine_iarray_push (geometry->angle_array, sequence);

gamgi_engine_darray_push (geometry->data, value);
}

static void static_measure_torsion (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_atom *atom4, gamgi_geometry *geometry)
{
gamgi_slist **table;
gamgi_slist *slist;
char label[GAMGI_ENGINE_TOKEN];
double *data;
double value[4];
double angle;
int *driver;
int sequence[7];
int offset, undefined;
unsigned int hash;

/******************************************************
 * make sure each sequence is counted once, not twice *
 ******************************************************/

if (atom1->mark < atom4->mark) return;

/*********************************************************
 * order each sequence with increasing atom elements, so *
 * sequences with the same elements are grouped together *
 *********************************************************/

if (atom1->element > atom4->element)
  {
  sequence[0] = atom4->element;
  sequence[1] = atom3->element;
  sequence[2] = atom2->element;
  sequence[3] = atom1->element;
  }
else
  {
  sequence[0] = atom1->element;
  sequence[3] = atom4->element;
  if (atom1->element == atom4->element && atom2->element > atom3->element)
    {
    sequence[1] = atom3->element;
    sequence[2] = atom2->element;
    }
  else
    {
    sequence[1] = atom2->element;
    sequence[2] = atom3->element;
    }
  }

/********************************************************
 * measure torsion angle and prepare to accumulate data *
 * to get minimum, maximum, average, standard deviation *
 ********************************************************/

undefined = 0;
if (gamgi_math_measure_angle_4atom (atom1, atom2, atom3, 
atom4, &angle, geometry->layer) == FALSE) undefined = 1;

if (undefined == 0)
  {
  value[0] = fabs (angle);
  value[1] = value[0];
  value[2] = value[0];
  value[3] = value[0] * value[0];
  }
else
  {
  value[0] = DBL_MAX;
  value[1] = -DBL_MAX;
  value[2] = 0;
  value[3] = 0;
  }

/*******************************
 * create label for hash table *
 *******************************/

sprintf (label, "%d%d%d%d", sequence[0], sequence[1], sequence[2], sequence[3]);

/***********************************************
 * if sequence already exists, accumulate data *
 ***********************************************/

table = geometry->torsion_table;
hash = gamgi_math_hash_value (label, geometry->multiplier, geometry->size);

for (slist = table[hash]; slist != NULL; slist = slist->next)
  {
  offset = GAMGI_POINTER_TO_INT (slist->data);
  driver = geometry->torsion_array->array + 7 * offset;
  if (driver[0] == sequence[0] && driver[1] == sequence[1] &&
  driver[2] == sequence[2] && driver[3] == sequence[3])
    {
    /***************************
     * accumulate torsion data *
     ***************************/

    driver[4] += 1;
    driver[5] += undefined;

    data = geometry->data->array + 4 * driver[6];
    if (value[0] < data[0]) data[0] = value[0];
    if (value[1] > data[1]) data[1] = value[1];
    data[2] += value[2];
    data[3] += value[3];
    return;
    }
  }

/***********************************************************
 * if sequence does not exist, enter new sequence and data *
 ***********************************************************/

offset = geometry->torsion_array->offset;
table[hash] = gamgi_math_hash_add (table[hash], GAMGI_INT_TO_POINTER (offset));

sequence[4] = 1;
sequence[5] = undefined;
sequence[6] = geometry->data->offset;
gamgi_engine_iarray_push (geometry->torsion_array, sequence);

gamgi_engine_darray_push (geometry->data, value);
}

static void static_level4 (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_atom *atom4, gamgi_geometry *geometry)
{
atom4->mark = -atom4->mark;

static_measure_torsion (atom1, atom2, atom3, atom4, geometry);

atom4->mark = -atom4->mark;
}

static void static_level3 (gamgi_atom *atom1, gamgi_atom *atom2,
gamgi_atom *atom3, gamgi_geometry *geometry)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

atom3->mark = -atom3->mark;

static_measure_angle (atom1, atom2, atom3, geometry);

for (dlist = atom3->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  if (bond->atom1->mark > 0)
    static_level4 (atom1, atom2, atom3, bond->atom1, geometry);
  if (bond->atom2->mark > 0)
    static_level4 (atom1, atom2, atom3, bond->atom2, geometry);
  } 

atom3->mark = -atom3->mark;
}

static void static_level2 (gamgi_atom *atom1, 
gamgi_atom *atom2, gamgi_geometry *geometry)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

atom2->mark = -atom2->mark;

static_measure_length (atom1, atom2, geometry);

for (dlist = atom2->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  if (bond->atom1->mark > 0)
    static_level3 (atom1, atom2, bond->atom1, geometry);
  if (bond->atom2->mark > 0)
    static_level3 (atom1, atom2, bond->atom2, geometry);
  }

atom2->mark = -atom2->mark;
}

static void static_level1 (gamgi_atom *atom1, gamgi_geometry *geometry)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

atom1->mark = -atom1->mark;

for (dlist = atom1->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  if (bond->atom1->mark > 0)
    static_level2 (atom1, bond->atom1, geometry);
  if (bond->atom2->mark > 0)
    static_level2 (atom1, bond->atom2, geometry);
  }

atom1->mark = -atom1->mark;
}

static void static_geometry (gamgi_geometry *geometry)
{
gamgi_dlist *dlist;

for (dlist = geometry->dlist; dlist != NULL; dlist = dlist->next)
  {
  static_level1 (GAMGI_CAST_ATOM dlist->data, geometry);
  }

/*******************************
 * after collecting data, hash *
 * tables are no longer needed *
 *******************************/

gamgi_math_hash_remove (geometry->length_table, geometry->size);
gamgi_math_hash_remove (geometry->angle_table, geometry->size);
gamgi_math_hash_remove (geometry->torsion_table, geometry->size);

/****************************************
 * order length, angle, torsion values  *
 * in increasing order of atom elements *
 ****************************************/

}

static gamgi_geometry *static_start (gamgi_object *object, gamgi_layer *layer)
{
gamgi_geometry *geometry;
gamgi_atom *atom;
gamgi_dlist *dlist;
int n;

/*****************************************
 * allocate main structure and atom list *
 *****************************************/

dlist = gamgi_engine_dlist_atom_object (object, NULL);
if (dlist == NULL) return NULL;

geometry = (gamgi_geometry *) malloc (sizeof (gamgi_geometry));
geometry->dlist = dlist;

geometry->layer = layer;

/*****************************************************
 * number and count atoms: numbers must be positive, *
 * non-zero, because later on, atoms already in use  *
 * will be signaled by reversing its number signal   *
 *****************************************************/

n = 0;
for (dlist = geometry->dlist; dlist != NULL; dlist = dlist->next)
  {
  atom = GAMGI_CAST_ATOM dlist->data;
  atom->mark = ++n;
  }
geometry->n_atoms = n;

/*************************************************
 * set dynamic arrays and hash tables parameters *
 *************************************************/

geometry->size = 10 + n / GAMGI_CHEM_GEOMETRY_SIZE;
geometry->step = GAMGI_CHEM_GEOMETRY_STEP;
geometry->multiplier = GAMGI_CHEM_GEOMETRY_MULTIPLIER;

/****************************************
 * store 4-coordinate points: minimum,  *
 * maximum, average, standard deviation *
 ****************************************/

geometry->data = gamgi_engine_darray_create (3 * geometry->size, geometry->step, 4);

/*********************************************
 * store 4-coordinate points: element1,      *
 * element2, array offset, occurrences       *
 *                                           *
 * create hash table to speed up recognition *
 * of sequences of element names             *
 *********************************************/

geometry->length_array = gamgi_engine_iarray_create (geometry->size, geometry->step, 4);
geometry->length_table = gamgi_math_hash_create (geometry->size);

/**************************************************
 * store 6-coordinate points: element1, element2, *
 * element3, array offset, occurrences, undefined *
 * (when at least two atoms are at same position) *
 *                                                *
 * create hash table to speed up recognition      *
 * of sequences of element names                  *
 **************************************************/

geometry->angle_array = gamgi_engine_iarray_create (geometry->size, geometry->step, 6);
geometry->angle_table = gamgi_math_hash_create (geometry->size);

/**************************************************
 * store 7-coordinate points: element1, element2, *
   element3, element4, array offset, occurrences, *
   undefined (when the four atoms are colinear)   *
 *                                                *
 * create hash table to speed up recognition      *
 * of sequences of element names                  *
 **************************************************/

geometry->torsion_array = gamgi_engine_iarray_create (geometry->size, geometry->step, 7);
geometry->torsion_table = gamgi_math_hash_create (geometry->size);

return geometry;
}

static void static_end (gamgi_geometry *geometry)
{
/************************
 * remove vector arrays *
 ************************/
 
gamgi_engine_iarray_remove (geometry->length_array);
gamgi_engine_iarray_remove (geometry->angle_array);
gamgi_engine_iarray_remove (geometry->torsion_array);

gamgi_engine_darray_remove (geometry->data);

free (geometry);
}

gamgi_bool gamgi_chem_geometry (gamgi_object *object, GtkWidget *text,
gamgi_layer *layer)
{
gamgi_geometry *geometry;

geometry = static_start (object, layer);
if (geometry == NULL) return FALSE;

static_geometry (geometry);
static_report (geometry, text);

static_end (geometry);

return TRUE;
}
