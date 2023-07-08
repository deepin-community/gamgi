/*******************************************
 *
 * $GAMGI/src/chem/gamgi_chem_atom.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_remove.h"
#include "gamgi_engine_reparent.h"
#include "gamgi_math_hash.h"
#include "gamgi_chem_property.h"
#include "gamgi_phys_plane.h"
#include "gamgi_phys_direction.h"
#include "gamgi_mesa_start.h"
#include "gamgi_io_token.h"
#include "gamgi_expat_export.h"

static void static_row (GtkListStore *store,
char *label0, char *label1, char *label2, char *label3)
{
GtkTreeIter iter;

gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter,
0, label0, 1, label1, 2, label2, 3, label3, -1);
}

/************** external function **************
 *                                             *
 *             GAMGI_CHEM_ATOM_NAME            *
 *                                             *
 * Get the element name from the atomic number *
 ***********************************************/

void gamgi_chem_atom_name (int number, char *name)
{
static char *element[] = {"Du", 
"H", "He",

"Li", "Be", "B", "C", "N", "O", "F", "Ne",

"Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar",

"K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co",
"Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr",

"Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh",
"Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe",

"Cs", "Ba",

"La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", 
"Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu",

"Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", 
"Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn",

"Fr", "Ra",

"Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm",
"Bk", "Cf", "Es", "Fm", "Md", "No", "Lr",

"Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg"};

/**************************************
 * For speed reasons, this function   *
 * does NOT check if number is valid: *
 * 0 <= number <= GAMGI_CHEM_ATOM_MAX *
 *   In the few cases where this is   *
 *   unsafe, number MUST be checked   *
 *   BEFORE calling this function!    *
 **************************************/

strcpy (name, element[number]);
}

/************* external function ***************
 *                                             *
 *       GAMGI_CHEM_CREATE_ATOM_NUMBER         *
 *                                             *
 * Get the atomic number from the element name *
 ***********************************************/

gamgi_bool gamgi_chem_atom_number (const char *name, int *number)
{
char element[GAMGI_ENGINE_TOKEN];
gamgi_slist *slist;
unsigned int hash;

hash = gamgi_math_hash_value (name,
GAMGI_CHEM_HASH_MULTIPLIER, GAMGI_CHEM_HASH_SIZE);

for (slist = gamgi->element[hash]; slist != NULL; slist = slist->next)
  {
  *number = GAMGI_POINTER_TO_INT (slist->data);
  gamgi_chem_atom_name (*number, element);
  if (strcmp (element, name) == 0) return TRUE;
  }

return FALSE;
}

gamgi_atom *gamgi_chem_atom_create (int element)
{
gamgi_atom *atom;

atom = gamgi_engine_create_atom ();
gamgi_engine_start_atom (atom);
gamgi_mesa_start_atom (atom, gamgi->atom);

atom->element = element;
gamgi_chem_atom_name (atom->element, atom->object.name);
gamgi_io_token_lower (atom->object.name, atom->object.label);
atom->mass = gamgi->atom->mass[atom->element];
atom->radius = gamgi->atom->radius[atom->element];
atom->red = gamgi->atom->red[atom->element];
atom->green = gamgi->atom->green[atom->element];
atom->blue = gamgi->atom->blue[atom->element];

return atom;
}

gamgi_slist **gamgi_chem_atom_element (void)
{
gamgi_slist **table;
unsigned int hash;
char name[GAMGI_ENGINE_TOKEN];
int element;

/*********************************
 * create hash table to speed up *
 * atom element name recognition *
 *********************************/

table = gamgi_math_hash_create (GAMGI_CHEM_HASH_SIZE);

for (element = 0; element <= GAMGI_CHEM_ATOM_MAX; element++)
  {
  gamgi_chem_atom_name (element, name);
  hash = gamgi_math_hash_value (name, GAMGI_CHEM_HASH_MULTIPLIER, GAMGI_CHEM_HASH_SIZE);
  table[hash] = gamgi_math_hash_add (table[hash], GAMGI_INT_TO_POINTER (element));
  }

return table;
}

/********* external function *********
 *                                   *
 *       GAMGI_CHEM_ATOM_MASS        *
 *                                   *
 * Set default mass for each element *
 *************************************/

void gamgi_chem_atom_mass (gamgi_atom_class *atom_class)
{
double *mass = atom_class->mass;
int offset, element;

/******************************************
 * set default mass for Dummy atoms (Z=0) *
 *                                        *
 *    set default mass for all elements   *
 ******************************************/

mass[GAMGI_CHEM_DU] = GAMGI_CHEM_DU_MASS;

for (element = 1; element <= GAMGI_CHEM_ATOM_MAX; element++)
  {
  offset = gamgi_chem_property_mass_driver[element];
  mass[element] = gamgi_chem_property_mass[offset];
  }
}

/********** external function **********
 *                                     *
 *       GAMGI_CHEM_ATOM_RADIUS        *
 *                                     *
 * Set default radius for each element *
 ***************************************/

void gamgi_chem_atom_radius (gamgi_atom_class *atom_class)
{
double *radius = atom_class->radius;
double value;
int offset, element;

/********************************************
 * set default radius for Dummy atoms (Z=0) *
 *                                          *
 *    set default radius for all elements   *
 ********************************************/

radius[GAMGI_CHEM_DU] = GAMGI_CHEM_DU_RADIUS;

for (element = 1; element <= GAMGI_CHEM_ATOM_MAX; element++)
  {
  /**********************************************
   * when no default radius is defined, use the *
   * default radius of the closer element below *
   **********************************************/

  offset = element - 1;
  do
    {
    /***************************************************************
     * use the type of radius defined by GAMGI_CHEM_RADIUS_DEFAULT *
     * as default (initially these are the natural half-bond       *
     * distances, available up to Cf (98), except Pm, At, Rn, Fr)  *
     ***************************************************************/

    value = gamgi_chem_property_radius[6*offset-- + GAMGI_CHEM_RADIUS_DEFAULT];
    } while (value == 0);
  radius[element] = value;
  }
}

void gamgi_chem_atom_color (gamgi_atom_class *atom_class)
{
float *red = atom_class->red;
float *green = atom_class->green;
float *blue = atom_class->blue;
int element, offset;

/*******************************************
 * set default color for Dummy atoms (Z=0) *
 *                                         *
 *    set default color for all elements   *
 *******************************************/

red[GAMGI_CHEM_DU] = GAMGI_CHEM_DU_R;
green[GAMGI_CHEM_DU] = GAMGI_CHEM_DU_G;
blue[GAMGI_CHEM_DU] = GAMGI_CHEM_DU_B;

/**************************************
 * set all the default element colors *
 **************************************/

for (element = 1; element <= GAMGI_CHEM_ATOM_MAX; element++)
  {
  offset = element - 1;

  red[element] = gamgi_chem_property_color[3*offset + 0];
  green[element] = gamgi_chem_property_color[3*offset + 1];
  blue[element] = gamgi_chem_property_color[3*offset + 2];
  }

}

void gamgi_chem_atom_export (gamgi_window *window, FILE *fp,
int depth_parent, int *depth_last, gamgi_bool *error)
{
double mass, radius;
float red, green, blue;
int element, offset;

gamgi_expat_export_atom (window, fp, depth_parent, depth_last, 
error, GAMGI_CHEM_DU, GAMGI_CHEM_DU_MASS, GAMGI_CHEM_DU_RADIUS, 
GAMGI_CHEM_DU_R, GAMGI_CHEM_DU_G, GAMGI_CHEM_DU_B);

for (element = 1; element <= GAMGI_CHEM_ATOM_MAX; element++)
  {
  offset = gamgi_chem_property_mass_driver[element];
  mass = gamgi_chem_property_mass[offset];

  offset = element - 1;
  red = gamgi_chem_property_color [3*offset + 0];
  green = gamgi_chem_property_color [3*offset + 1];
  blue = gamgi_chem_property_color [3*offset + 2];

  do
    {
    radius = gamgi_chem_property_radius[6*offset-- + GAMGI_CHEM_RADIUS_DEFAULT];
    } while (radius == 0);

  gamgi_expat_export_atom (window, fp, depth_parent, 
  depth_last, error, element, mass, radius, red, green, blue);
  }

}

void gamgi_chem_atom_mass_list (GtkListStore *store, int element)
{
char label0[GAMGI_ENGINE_TOKEN];
char label1[GAMGI_ENGINE_TOKEN];
char label2[GAMGI_ENGINE_TOKEN];
char label3[GAMGI_ENGINE_TOKEN];
double value0, value1, value2, value3;
gamgi_bool excited;
int start, end;
int i;

if (element == FALSE) return;

/***************************************************
 * get first, last valid positions in double array *
 ***************************************************/

start = gamgi_chem_property_mass_driver[element];
if (element == GAMGI_CHEM_ATOM_MAX) end = GAMGI_CHEM_MASS_ARRAY - 1;
else end = gamgi_chem_property_mass_driver[element + 1] - 1;

/*********************
 * show default mass *
 *********************/

sprintf (label0, "%.*f", gamgi->gamgi->mass, gamgi_chem_property_mass[start]);
strcpy (label1, "");
strcpy (label2, "");
strcpy (label3, "");
static_row (store, label0, label1, label2, label3);

i = start + 1;
do 
  {

  /*****************************************************************
   * show mass (label0), isotope number and excited state (label1) *
   *****************************************************************/

  excited = FALSE;
  value0 = gamgi_chem_property_mass[i];
  if (value0 < 0) { value0 = -value0; excited = TRUE; }

  sprintf (label0, "%.*f", gamgi->gamgi->mass, value0);
  sprintf (label1, "%d", (int) floor (value0 + 0.5));
  if (excited == TRUE) strcat (label1, "*");

  /**************************************************
   * show abundance (label2) and half-time (label3) *
   **************************************************/

  strcpy (label2, "");
  strcpy (label3, "");

  value1 = gamgi_chem_property_mass[i + 1];

  if (i + 2 > end)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    static_row (store, label0, label1, label2, label3);
    break;
    }

  value2 = gamgi_chem_property_mass[i + 2];

  if (value2 == GAMGI_CHEM_MASS_YEAR)
    {
    sprintf (label3, "%.*E y", GAMGI_MATH_DECIMAL_DECAY, value1);
    static_row (store, label0, label1, label2, label3);
    i += 3; continue;
    }
  if (value2 == GAMGI_CHEM_MASS_DAY)
    { 
    sprintf (label3, "%.*E d", GAMGI_MATH_DECIMAL_DECAY, value1);
    static_row (store, label0, label1, label2, label3);
    i += 3; continue;
    }
  if (value2 == GAMGI_CHEM_MASS_HOUR)
    { 
    sprintf (label3, "%.*E h", GAMGI_MATH_DECIMAL_DECAY, value1);
    static_row (store, label0, label1, label2, label3);
    i += 3; continue;
    }
  if (value2 == GAMGI_CHEM_MASS_MINUTE)
    {
    sprintf (label3, "%.*E m", GAMGI_MATH_DECIMAL_DECAY, value1);
    static_row (store, label0, label1, label2, label3);
    i += 3; continue;
    }
  if (value2 == GAMGI_CHEM_MASS_SECOND)
    {
    sprintf (label3, "%.*E s", GAMGI_MATH_DECIMAL_DECAY, value1);
    static_row (store, label0, label1, label2, label3);
    i += 3; continue;
    }

  /**********************************************
   * i + 3 is guaranteed to exist at this stage *
   **********************************************/

  value3 = gamgi_chem_property_mass[i + 3];

  if (value3 == GAMGI_CHEM_MASS_YEAR)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    sprintf (label3, "%.*E y", GAMGI_MATH_DECIMAL_DECAY, value2);
    static_row (store, label0, label1, label2, label3);
    i += 4; continue;
    }
    
  if (value3 == GAMGI_CHEM_MASS_DAY)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    sprintf (label3, "%.*E d", GAMGI_MATH_DECIMAL_DECAY, value2);
    static_row (store, label0, label1, label2, label3);
    i += 4; continue;
    }

  if (value3 == GAMGI_CHEM_MASS_HOUR)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    sprintf (label3, "%.*E h", GAMGI_MATH_DECIMAL_DECAY, value2);
    static_row (store, label0, label1, label2, label3);
    i += 4; continue;
    }
 
  if (value3 == GAMGI_CHEM_MASS_MINUTE)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    sprintf (label3, "%.*E m", GAMGI_MATH_DECIMAL_DECAY, value2);
    static_row (store, label0, label1, label2, label3);
    i += 4; continue;
    }

  if (value3 == GAMGI_CHEM_MASS_SECOND)
    {
    sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
    sprintf (label3, "%.*E s", GAMGI_MATH_DECIMAL_DECAY, value2);
    static_row (store, label0, label1, label2, label3);
    i += 4; continue;
    }

  sprintf (label2, "%.*f", GAMGI_MATH_DECIMAL_ABUNDANCE, value1);
  static_row (store, label0, label1, label2, label3);
  i += 2;
  } while (i < end);

}

void gamgi_chem_atom_radius_list (GtkListStore *store, int element)
{
char label0[GAMGI_ENGINE_TOKEN];
char label1[GAMGI_ENGINE_TOKEN];
char label2[GAMGI_ENGINE_TOKEN];
char label3[GAMGI_ENGINE_TOKEN];
double value;

if (element == FALSE) return;

strcpy (label2, "");
strcpy (label3, "");

/***********************************
 * array starts at 0 for element 1 *
 ***********************************/

element--;

value = gamgi_chem_property_radius[6*element + 0];
if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Half-Distance");
  static_row (store, label0, label1, label2, label3);
  }

value = gamgi_chem_property_radius[6*element + 1];
if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Atomic Eff");
  static_row (store, label0, label1, label2, label3);
  }

value = gamgi_chem_property_radius[6*element + 2];
if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Atomic Cal");
  static_row (store, label0, label1, label2, label3);
  }

value = gamgi_chem_property_radius[6*element + 3];
if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Eff");
  static_row (store, label0, label1, label2, label3);
  }

/**************************************************************************
 * special cases (the first value is        6 C 0.76 sp3 0.73 sp2 0.69 sp *
 * stored in the normal covalent calculated 25 Mn 1.39 LS 1.61  HS        *
 * positions, the remaining are stored here 26 Fe 1.32 LS 1.52  HS        *
 * and handled as special cases:            27 Co 1.26 LS 1.50  HS        *
 **************************************************************************/

value = gamgi_chem_property_radius[6*element + 4];
if (element == GAMGI_CHEM_C - 1)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal sp3");
  static_row (store, label0, label1, label2, label3);

  value = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_ARRAY - 5];

  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal sp2");
  static_row (store, label0, label1, label2, label3);

  value = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_ARRAY - 4];

  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal sp");
  static_row (store, label0, label1, label2, label3);
  }
else if (element == GAMGI_CHEM_MN - 1)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal LS");
  static_row (store, label0, label1, label2, label3);

  value = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_ARRAY - 3];

  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal HS");
  static_row (store, label0, label1, label2, label3);
  }
else if (element == GAMGI_CHEM_FE - 1)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal LS");
  static_row (store, label0, label1, label2, label3);
  
  value = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_ARRAY - 2];

  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal HS");
  static_row (store, label0, label1, label2, label3);
  }
else if (element == GAMGI_CHEM_CO - 1)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal LS");
  static_row (store, label0, label1, label2, label3);
  
  value = gamgi_chem_property_radius[GAMGI_CHEM_RADIUS_ARRAY - 1];

  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal HS");
  static_row (store, label0, label1, label2, label3);
  }
else if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Covalent Cal");
  static_row (store, label0, label1, label2, label3);
  }

value = gamgi_chem_property_radius[6*element + 5];
if (value != 0)
  {
  sprintf (label0, "%.*f ", gamgi->gamgi->length, value);
  strcpy (label1, "Van der Waals");
  static_row (store, label0, label1, label2, label3);
  }

}

void gamgi_chem_atom_ionic_list (GtkListStore *store, int element)
{
char label0[GAMGI_ENGINE_TOKEN];
char label1[GAMGI_ENGINE_TOKEN];
char label2[GAMGI_ENGINE_TOKEN];
char label3[GAMGI_ENGINE_TOKEN];
int i, offset;
int end;
int value;
double radius;

if (element == FALSE) return;

/**********************************************
 * get start,end limits for int,double arrays *
 **********************************************/

i = gamgi_chem_property_ionic_driver[element];
offset = gamgi_chem_property_ionic_driver[i++];
if (offset == -1) return;

if (element == GAMGI_CHEM_ATOM_MAX) end = GAMGI_CHEM_IONIC_DRIVER_ARRAY - 1;
else end = gamgi_chem_property_ionic_driver[element + 1] - 1;

/***************
 * get valence *
 ***************/

value = gamgi_chem_property_ionic_driver[i++];
while (i <= end)
  { 
  /**********************************
   * show valence, get coordination *
   **********************************/

  sprintf (label2, "%d", value);
  value = gamgi_chem_property_ionic_driver[i++];

  do
    {
    /*********************
     * show coordination *
     *********************/

    sprintf (label3, "%d", value/GAMGI_CHEM_IONIC_COORDINATION);
    if (value%GAMGI_CHEM_IONIC_COORDINATION == 1) strcat (label3, " Py");
    if (value%GAMGI_CHEM_IONIC_COORDINATION == 2) strcat (label3, " Sq");

    /*******************************************************
     * if next value contains HS for high spin, LS for low *
     * spin, or D for hydrogen, update and get next value  *
     *******************************************************/

    strcpy (label1, "Ionic Eff");
    value = gamgi_chem_property_ionic_driver[i++];
    if (value == GAMGI_CHEM_IONIC_HS)
      {
      strcat (label1, " HS");
      value = gamgi_chem_property_ionic_driver[i++];
      }
    else if (value == GAMGI_CHEM_IONIC_LS)
      {
      strcat (label1, " LS");
      value = gamgi_chem_property_ionic_driver[i++];
      }
    else if (value == GAMGI_CHEM_IONIC_D)
      {
      strcat (label1, " D");
      value = gamgi_chem_property_ionic_driver[i++];
      }

    /***************************
     * get radius and show row *
     ***************************/

    radius = gamgi_chem_property_ionic[offset++];
    sprintf (label0, "%.*f", gamgi->gamgi->length, radius);
    static_row (store, label0, label1, label2, label3);

    /***********************************
     * LS HS sequence: get next value, *
     * get radius and show row         *
     *                                 *
     * HS LS sequence: does not exist  *
     ***********************************/

    if (value == GAMGI_CHEM_IONIC_HS)
      {
      strcpy (label1, "Ionic Eff HS");
      value = gamgi_chem_property_ionic_driver[i++];
      radius = gamgi_chem_property_ionic[offset++];
      sprintf (label0, "%.*f", gamgi->gamgi->length, radius);
      static_row (store, label0, label1, label2, label3);
      }

    } while (value >= GAMGI_CHEM_IONIC_COORDINATION);
  }

}

void gamgi_chem_atom_clean (gamgi_atom *atom)
{
gamgi_bond *bond;
gamgi_dlist *dlist;

for (dlist = atom->bond_start; dlist != NULL; dlist = dlist->next)
  {
  bond = GAMGI_CAST_BOND dlist->data;
  bond->copy = NULL;
  }
}
