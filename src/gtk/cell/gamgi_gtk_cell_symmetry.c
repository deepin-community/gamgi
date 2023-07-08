/*************************************************
 *
 * $GAMGI/src/gtk/cell/gamgi_gtk_cell_symmetry.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_phys.h"
#include "gamgi_math.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_phys_symmetry.h"
#include "gamgi_math_vector.h"
#include "gamgi_io_token.h"

static void static_changed_system (GtkWidget *widget, void *data);
static void static_changed_lattice (GtkWidget *widget, void *data);
static void static_changed_group (GtkWidget *widget, void *data);

gamgi_bool static_parameters_check (gamgi_enum lattice, 
double a, double b, double c, double ab, double ac, double bc)
{
/********************************************
 * check if entered parameters are possible *
 ********************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_P:
  case GAMGI_PHYS_CUBIC_I:
  case GAMGI_PHYS_CUBIC_F:
  if (a > 0 && b > 0 && a != b) return FALSE;
  if (a > 0 && c > 0 && a != c) return FALSE;
  if (b > 0 && c > 0 && b != c) return FALSE;
  if (ab > 0 && ab != 90.0) return FALSE;
  if (ac > 0 && ac != 90.0) return FALSE;
  if (bc > 0 && bc != 90.0) return FALSE;
  break;

  case GAMGI_PHYS_TETRAGONAL_P:
  case GAMGI_PHYS_TETRAGONAL_I:
  if (a > 0 && b > 0 && a != b) return FALSE;
  if (ab > 0 && ab != 90.0) return FALSE;
  if (ac > 0 && ac != 90.0) return FALSE;
  if (bc > 0 && bc != 90.0) return FALSE;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_P:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  if (ab > 0 && ab != 90.0) return FALSE;
  if (ac > 0 && ac != 90.0) return FALSE;
  if (bc > 0 && bc != 90.0) return FALSE;
  break;

  case GAMGI_PHYS_HEXAGONAL_P:
  case GAMGI_PHYS_HEXAGONAL_R:
  if (a > 0 && b > 0 && a != b) return FALSE;
  if (ab > 0 && ab != 120.0) return FALSE;
  if (ac > 0 && ac != 90.0) return FALSE;
  if (bc > 0 && bc != 90.0) return FALSE;
  break;

  case GAMGI_PHYS_MONOCLINIC_P:
  case GAMGI_PHYS_MONOCLINIC_C:
  if (ab > 0 && ab != 90.0) return FALSE;
  if (bc > 0 && bc != 90.0) return FALSE;
  break;
  }

return TRUE;
}

void static_parameters_complete (gamgi_enum lattice,
double *a, double *b, double *c,
double *ab, double *ac, double *bc)
{
/***************************************************
 * complete entered parameters as much as possible *
 ***************************************************/

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_P:
  case GAMGI_PHYS_CUBIC_I:
  case GAMGI_PHYS_CUBIC_F:
  if (*a > 0) { *b = *a; *c = *a; }
  if (*b > 0) { *a = *b; *c = *b; }
  if (*c > 0) { *a = *c; *b = *c; }
  *ab = 90.0;
  *ac = 90.0;
  *bc = 90.0;
  break;
 
  case GAMGI_PHYS_TETRAGONAL_P:
  case GAMGI_PHYS_TETRAGONAL_I:
  if (*a > 0) *b = *a;
  if (*b > 0) *a = *b;
  *ab = 90.0;
  *ac = 90.0;
  *bc = 90.0;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_P:
  case GAMGI_PHYS_ORTHORHOMBIC_C:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  *ab = 90.0;
  *ac = 90.0;
  *bc = 90.0;
  break;

  case GAMGI_PHYS_HEXAGONAL_P:
  case GAMGI_PHYS_HEXAGONAL_R:
  if (*a > 0) *b = *a;
  if (*b > 0) *a = *b;
  *ab = 120.0;
  *ac = 90.0;
  *bc = 90.0;
  break;

  case GAMGI_PHYS_MONOCLINIC_P:
  case GAMGI_PHYS_MONOCLINIC_C:
  *ab = 90.0;
  *bc = 90.0;
  break;
  }
}

void static_parameters_old (gamgi_cell *cell, 
double *a, double *b, double *c, 
double *ab, double *ac, double *bc)
{
if (*a == 0) *a = cell->a;
if (*b == 0) *b = cell->b;
if (*c == 0) *c = cell->c;
if (*ab == 0) *ab = cell->ab;
if (*ac == 0) *ac = cell->ac;
if (*bc == 0) *bc = cell->bc;
}

gamgi_bool static_parameters_valid (double a, double b, 
double c, double ab, double ac, double bc)
{ 
if (a == 0) return FALSE;
if (b == 0) return FALSE;
if (c == 0) return FALSE;
if (ab == 0) return FALSE;
if (ac == 0) return FALSE;
if (bc == 0) return FALSE;

return TRUE;
}

gamgi_bool static_angles_valid (double ab, double ac, double bc)
{
/*********************************************************
 * The three angles together must be smaller than 360,   *
 * each angle cannot be larger than the sum of the other *
 * two, or smaller than the difference of the other two. *
 *********************************************************/

if (ab + ac + bc > 360.0 - GAMGI_MATH_TOLERANCE_ANGLE)
  return FALSE;

if (ab + GAMGI_MATH_TOLERANCE_ANGLE > ac + bc 
|| ac + GAMGI_MATH_TOLERANCE_ANGLE > ab + bc 
|| bc + GAMGI_MATH_TOLERANCE_ANGLE > ab + ac)
  return FALSE;

if (ab - GAMGI_MATH_TOLERANCE_ANGLE < abs (ac - bc) 
|| ac - GAMGI_MATH_TOLERANCE_ANGLE < abs (ab - bc) 
|| bc - GAMGI_MATH_TOLERANCE_ANGLE < abs (ab - ac))
  return FALSE;

return TRUE;
}

static gamgi_slist *static_list_system (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
gamgi_slist *slist = NULL;
GtkEntry *entry;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **systems = gamgi_phys_symmetry_systems;
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number, n;

/*****************************
 * retrieve and check system *
 *****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry);

strcpy (system, "");
if (gamgi_io_token_check (name) == TRUE && 
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (system, "") != 0 &&
strcmp (system, "a") != 0 && strcmp (system, "m") != 0 &&
strcmp (system, "o") != 0 && strcmp (system, "t") != 0 &&
strcmp (system, "h") != 0 && strcmp (system, "c")) return slist;

/******************************
 * retrieve and check lattice *
 ******************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry);

strcpy (lattice, "");
if (gamgi_io_token_check (name) == TRUE && 
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (lattice, "") != 0 &&
strcmp (lattice, "P") != 0 && strcmp (lattice, "I") != 0 &&
strcmp (lattice, "F") != 0 && strcmp (lattice, "C") != 0 &&
strcmp (lattice, "R") != 0) return slist;

/****************************
 * retrieve and check group *
 ****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry);

number = 0;
if (gamgi_io_token_check (name) == TRUE  &&
gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE) return slist;

/********************************************************************
 * system,lattice,group are correct or empty AND it is guaranteed   *
 * (by the "changed" signal callbacks) that they are all compatible *
 ********************************************************************/

for (n = 5; n >= 0; n--)
  {
  if (
  (strcmp (system, "") == 0 || strcmp (systems[4*n + 0], system) == 0)
  &&
  (strcmp (lattice, "") == 0 || strstr (systems[4*n + 2], lattice) != NULL)
  &&
  (number == 0 || strcmp (systems[4*n + 0], groups[4*(number - 1) + 2]) == 0)
  )
    {
    slist = gamgi_engine_slist_add_start (slist);
    slist->data = GAMGI_INT_TO_POINTER (n);
    }
  }

return slist;
}

static gamgi_slist *static_list_lattice (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
gamgi_slist *slist = NULL;
GtkEntry *entry;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **lattices = gamgi_phys_symmetry_lattices;
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number, n;

/*****************************
 * retrieve and check system *
 *****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry);

strcpy (system, "");
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (system, "") != 0 &&
strcmp (system, "a") != 0 && strcmp (system, "m") != 0 &&
strcmp (system, "o") != 0 && strcmp (system, "t") != 0 &&
strcmp (system, "h") != 0 && strcmp (system, "c")) return slist;

/******************************
 * retrieve and check lattice *
 ******************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry);

strcpy (lattice, "");
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (lattice, "") != 0 &&
strcmp (lattice, "P") != 0 && strcmp (lattice, "I") != 0 &&
strcmp (lattice, "F") != 0 && strcmp (lattice, "C") != 0 &&
strcmp (lattice, "R") != 0) return slist;

/****************************
 * retrieve and check group *
 ****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry);

number = 0;
if (gamgi_io_token_check (name) == TRUE  &&
gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE) return slist;

/********************************************************************
 * system,lattice,group are correct or empty AND it is guaranteed   *
 * (by the "changed" signal callbacks) that they are all compatible *
 ********************************************************************/

for (n = 4; n >= 0; n--)
  {
  if (
  (strcmp (system, "") == 0 || strstr (lattices[4*n + 2], system) != NULL)
  &&
  (strcmp (lattice, "") == 0 || strcmp (lattices[4*n + 0], lattice) == 0)
  &&
  (number == 0 || strcmp (lattices[4*n + 0], groups[4*(number - 1) + 3]) == 0)
  )
    {
    slist = gamgi_engine_slist_add_start (slist);
    slist->data = GAMGI_INT_TO_POINTER (n);
    }
  }

return slist;
}

static gamgi_slist *static_list_group (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
gamgi_slist *slist = NULL;
GtkEntry *entry;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number, n;

/*****************************
 * retrieve and check system *
 *****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry);

strcpy (system, "");
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (system, "") != 0 &&
strcmp (system, "a") != 0 && strcmp (system, "m") != 0 &&
strcmp (system, "o") != 0 && strcmp (system, "t") != 0 &&
strcmp (system, "h") != 0 && strcmp (system, "c")) return slist;

/******************************
 * retrieve and check lattice *
 ******************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry);

strcpy (lattice, "");
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return slist;

if (strcmp (lattice, "") != 0 &&
strcmp (lattice, "P") != 0 && strcmp (lattice, "I") != 0 &&
strcmp (lattice, "F") != 0 && strcmp (lattice, "C") != 0 &&
strcmp (lattice, "R") != 0) return slist;

/****************************
 * retrieve and check group *
 ****************************/

entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry);

number = 0;
if (gamgi_io_token_check (name) == TRUE  &&
gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE) return slist;

/********************************************************************
 * system,lattice,group are correct or empty AND it is guaranteed   *
 * (by the "changed" signal callbacks) that they are all compatible *
 ********************************************************************/

for (n = 229; n >= 0; n--)
  {
  if (
  (strcmp (system, "") == 0 || strcmp (groups[4*n + 2], system) == 0)
  &&
  (strcmp (lattice, "") == 0 || strcmp (groups[4*n + 3], lattice) == 0)
  &&
  (number == 0 || n + 1 == number)
  )
    {
    slist = gamgi_engine_slist_add_start (slist);
    slist->data = GAMGI_INT_TO_POINTER (n);
    }
  }

return slist;
}

static void static_parameters (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
gamgi_slist *slist;
char **systems = gamgi_phys_symmetry_systems;
int flags[6] = {0};
int n;

/****************************************
 * first part: mark required parameters *
 ****************************************/

/******************************************************************
 * the 6 flags describe the 6 lattice parameters a,b,c,ab,ac,bc.  *
 * Initially all flags are set to 0, then each available system   *
 * sets to 1 the flags corresponding to the parameters it needs.  *
 * In the end, flags that are still set to 0 describe parameters  *
 * that should be inactive and their gtk entries are set to zero. *
 ******************************************************************/
   
slist = static_list_system (window);
while (slist != NULL)
  {
  n = GAMGI_POINTER_TO_INT (slist->data);
  if (strcmp (systems[4*n + 0], "a") == 0)
    {
    /**************************************
     * Triclinic: requires a,b,c,ab,ac,bc *
     **************************************/

    flags[0] = 1; flags[1] = 1; flags[2] = 1; 
    flags[3] = 1; flags[4] = 1; flags[5] = 1;
    }

  else if (strcmp (systems[4*n + 0], "m") == 0)
    {
    /***************************************************
     * Monoclinic: requires a,b,c,ac (unique angle for *
     * axis b, so base-centered lattices are C-based)  *
     ***************************************************/

    flags[0] = 1; flags[1] = 1; flags[2] = 1;
    flags[4] = 1;
    }

  else if (strcmp (systems[4*n + 0], "o") == 0)
    {
    /********************************
     * Orthorhombic: requires a,b,c *
     ********************************/

    flags[0] = 1; flags[1] = 1; flags[2] = 1;
    }

  else if (strcmp (systems[4*n + 0], "t") == 0)
    {
    /****************************
     * Tetragonal: requires a,c *
     ****************************/

    flags[0] = 1; flags[2] = 1;
    }

  else if (strcmp (systems[4*n + 0], "c") == 0)
    {
    /*********************
     * Cubic: requires a *
     *********************/

    flags[0] = 1;
    }

  else if (strcmp (systems[4*n + 0], "h") == 0)
    {
    /*******************************************
     * Hexagonal: requires a,c (hexagonal axes *
     * are used also for rhombohedral cells)   *
     *******************************************/

    flags[0] = 1; flags[2] = 1;
    }

  slist = gamgi_engine_slist_remove_start (slist);
  }

/**********************************************
 * second part: block non-required parameters *
 **********************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_a");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_a");
if (flags[0] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_b");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_b");
if (flags[1] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_c");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_c");
if (flags[2] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ab");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_ab");
if (flags[3] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_ac");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_ac");
if (flags[4] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_bc");
label = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "label_bc");
if (flags[5] == 0)
  {
  gtk_entry_set_text ((GtkEntry *) entry, "");
  gtk_widget_set_sensitive (entry, FALSE);
  gtk_widget_set_sensitive (label, FALSE);
  }
else
  {
  gtk_widget_set_sensitive (entry, TRUE);
  gtk_widget_set_sensitive (label, TRUE);
  }
}

static void static_nodes (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *combo;
gamgi_enum lattice, vectors;
int row;

/*********************************************
 * reset combo box everytime lattice changes *
 *********************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_o4");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

/***************************************************************
 * disable nodes incompatible with current lattice and vectors *
 ***************************************************************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "combo_origin_vectors");
row = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

if (row == GAMGI_PHYS_PRIMITIVE - 1) vectors = GAMGI_PHYS_PRIMITIVE;
else vectors = GAMGI_PHYS_CONVENTIONAL;

lattice = gamgi_gtk_cell_symmetry_lattice (dialog);
gamgi_gtk_cell_symmetry_nodes (lattice, vectors, dialog);
}

static void static_changed_system (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry_system, *entry_lattice, *entry_group;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number;

/************************************
 * retrieve system, return if empty *
 ************************************/

entry_system = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry_system);
if (gamgi_io_token_check (name) == FALSE)
  { static_parameters (window); return; }
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/********************
 * retrieve lattice *
 ********************/

entry_lattice = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry_lattice);
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/******************
 * retrieve group *
 ******************/

entry_group = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry_group);
if (gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE)
  number = 0;

/*****************************************************
 * block changed signal for lattice,group entries    *
 * as changes done in this function to those entries *
 * should not be used to change system entry again!  *
 *****************************************************/

g_signal_handlers_block_by_func (entry_lattice, static_changed_lattice, window);
g_signal_handlers_block_by_func (entry_group, static_changed_group, window);

/**************************************************
 * set lattice,group if system provides no choice *
 *                                                *
 * ignore lattice,group if system is compatible   *
 *                                                *
 * clean lattice,group if system is incompatible  *
 **************************************************/

/*****************************
 * Triclinic/Anorthic system *
 *****************************/

if (strcmp (system, "a") == 0) 
  {
  gtk_entry_set_text (entry_lattice, "P");
  }

/*********************
 * Monoclinic system *
 *********************/

else if (strcmp (system, "m") == 0)
  {
  if (strcmp (lattice, "P") != 0 && strcmp (lattice, "C") != 0)
    gtk_entry_set_text (entry_lattice, "");
  }

/***********************
 * Orthorhombic system *
 ***********************/

else if (strcmp (system, "o") == 0) 
  { 
  if (strcmp (lattice, "P") != 0 && strcmp (lattice, "C") != 0
  && strcmp (lattice, "I") != 0 && strcmp (lattice, "F") != 0)
    gtk_entry_set_text (entry_lattice, "");
  }

/*********************
 * Tetragonal system *
 *********************/

else if (strcmp (system, "t") == 0) 
  { 
  if (strcmp (lattice, "P") != 0 && strcmp (lattice, "I") != 0)
    gtk_entry_set_text (entry_lattice, "");
  }

/******************************************
 * Hexagonal/Trigonal/Rhombohedral system *
 ******************************************/

else if (strcmp (system, "h") == 0) 
  { 
  if (strcmp (lattice, "P") != 0 && strcmp (lattice, "R") != 0)
    gtk_entry_set_text (entry_lattice, "");
  }

/****************
 * Cubic system *
 ****************/

else if (strcmp (system, "c") == 0) 
  { 
  if (strcmp (lattice, "P") != 0 && strcmp (lattice, "I") != 0
  && strcmp (lattice, "F") != 0)
    gtk_entry_set_text (entry_lattice, "");
  }

/****************
 * Wrong system *
 ****************/

else
  {
  gtk_entry_set_text (entry_lattice, "");
  gtk_entry_set_text (entry_group, "");
  }

/******************
 * Spatial groups *
 ******************/

if (number == 0 || strcmp (system, groups[4*(number - 1) + 2]) != 0)
  gtk_entry_set_text (entry_group, "");

/*********************************************************
 * unblock changed signal for lattice,group entries,     *
 * so they accept changes done directly in those entries *
 *********************************************************/

g_signal_handlers_unblock_by_func (entry_lattice, static_changed_lattice, window);
g_signal_handlers_unblock_by_func (entry_group, static_changed_group, window);

static_parameters (window);
}

static void static_changed_lattice (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry_system, *entry_lattice, *entry_group;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number;

/*************************************
 * retrieve lattice, return if empty *
 *************************************/

entry_lattice = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry_lattice);
if (gamgi_io_token_check (name) == FALSE)
  { static_parameters (window); static_nodes (window); return; }
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/*******************
 * retrieve system *
 *******************/

entry_system = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry_system);
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/******************
 * retrieve group *
 ******************/

entry_group = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry_group);
if (gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE)
  number = 0;

/*****************************************************
 * block changed signal for system,group entries     *
 * as changes done in this function to those entries *
 * should not be used to change lattice entry again! *
 *****************************************************/

g_signal_handlers_block_by_func (entry_system, static_changed_system, window);
g_signal_handlers_block_by_func (entry_group, static_changed_group, window);

/**************************************************
 * set system,group if lattice provides no choice *
 *                                                *
 * ignore system,group if lattice is compatible   *
 *                                                *
 * clean system,group if lattice is incompatible  *
 **************************************************/

/*********************
 * Primitive lattice *
 *********************/

if (strcmp (lattice, "P") == 0)
  {
  if (strcmp (system, "a") != 0 && strcmp (system, "m") != 0 &&
  strcmp (system, "o") != 0 && strcmp (system, "t") != 0 &&
  strcmp (system, "h") != 0 && strcmp (system, "c") != 0)
    gtk_entry_set_text (entry_system, "");
  }

/*************************
 * Base centered lattice *
 *************************/

else if (strcmp (lattice, "C") == 0)
  {
  if (strcmp (system, "m") != 0 && strcmp (system, "o") != 0)
    gtk_entry_set_text (entry_system, "");
  }

/************************
 * Rhombohedral lattice *
 ************************/

else if (strcmp (lattice, "R") == 0)
  {
  gtk_entry_set_text (entry_system, "h");
  }

/*************************
 * Body centered lattice *
 *************************/

else if (strcmp (lattice, "I") == 0)
  {
  if (strcmp (system, "o") != 0 && strcmp (system, "t") != 0 &&
  strcmp (system, "c") != 0) gtk_entry_set_text (entry_system, "");
  }

/*************************
 * Face centered lattice *
 *************************/

else if (strcmp (lattice, "F") == 0)
  {
  if (strcmp (system, "o") != 0 && strcmp (system, "c") != 0)
    gtk_entry_set_text (entry_system, "");
  }

/*****************
 * Wrong lattice *
 *****************/

else
  {
  gtk_entry_set_text (entry_system, "");
  gtk_entry_set_text (entry_group, "");
  }

/******************
 * Spatial groups *
 ******************/

if (number == 0 || strcmp (lattice, groups[4*(number - 1) + 3]) != 0)
  gtk_entry_set_text (entry_group, "");

/*********************************************************
 * unblock changed signal for system,group entries,      *
 * so they accept changes done directly in those entries *
 *********************************************************/

g_signal_handlers_unblock_by_func (entry_system, static_changed_system, window);
g_signal_handlers_unblock_by_func (entry_group, static_changed_group, window);

static_parameters (window);
static_nodes (window);
}

static void static_changed_group (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkEntry *entry_system, *entry_lattice, *entry_group;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char **groups = gamgi_phys_symmetry_groups;
const char *name;
int number;

/***********************************
 * retrieve group, return if empty *
 ***********************************/

entry_group = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry_group);
if (gamgi_io_token_check (name) == FALSE)
  { static_parameters (window); return; }
if (gamgi_io_token_int_scan (name, &number, 1, 230) == FALSE)
  number = 0;

/*******************
 * retrieve system *
 *******************/

entry_system = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry_system);
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/********************
 * retrieve lattice *
 ********************/

entry_lattice = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry_lattice);
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);

/*****************************************************
 * block changed signal for system,lattice entries   *
 * as changes done in this function to those entries *
 * should not be used to change group entry again!   *
 *****************************************************/

g_signal_handlers_block_by_func (entry_system, static_changed_system, window);
g_signal_handlers_block_by_func (entry_lattice, static_changed_lattice, window);

if (number > 0)
  {
  /*************************************************************
   * we assume that each group has a unique system AND lattice *
   *************************************************************/

  gtk_entry_set_text (entry_system, groups[4*(number - 1) + 2]);
  gtk_entry_set_text (entry_lattice, groups[4*(number - 1) + 3]);
  }
else
  {
  /******************************************
   * clean system,lattice if group is wrong *
   ******************************************/

  gtk_entry_set_text (entry_system, "");
  gtk_entry_set_text (entry_lattice, "");
  }

/*********************************************************
 * unblock changed signal for system,lattice entries,    *
 * so they accept changes done directly in those entries *
 *********************************************************/

g_signal_handlers_unblock_by_func (entry_system, static_changed_system, window);
g_signal_handlers_unblock_by_func (entry_lattice, static_changed_lattice, window);

static_parameters (window);
}

static void static_row (GtkListStore *store, GtkTreeIter *iter,
char *label0, char *label1, char *label2, char *label3)
{
gtk_list_store_append (store, iter);
gtk_list_store_set (store, iter, 0, label0, 1, label1, 2, label2, 3, label3, -1);
}

static void static_select_system (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_select_lattice (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_select_group (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
GtkEntry *entry;
char *name;

selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
if (gtk_tree_selection_get_selected (selection, &model, &iter))
  {
  entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
  gtk_tree_model_get (model, &iter, 0, &name, -1);
  gtk_entry_set_text (entry, name);
  free (name);
  }
}

static void static_remove (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_system");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_group");
gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

static int static_delete (GtkWidget *widget,
GdkEventButton *event, void *data)
{
static_remove (widget, data);

/*************************
 * FALSE: destroy window *
 *************************/

return FALSE;
}

static void static_system (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *button;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;
GtkTreeIter iter;
GtkRequisition size;
gamgi_slist *slist;
char **systems = gamgi_phys_symmetry_systems;
int n;

dialog = gamgi_gtk_dialog_task1_create ("Crystalographic Systems", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (vbox_dialog), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_select_system), window);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Systems", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Designations", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Lattices", renderer, "text", 2, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Groups", renderer, "text", 3, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (4, 
G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

slist = static_list_system (window);
while (slist != NULL)
  {
  n = GAMGI_POINTER_TO_INT (slist->data);
  static_row (store, &iter, systems[4*n + 0],
  systems[4*n + 1], systems[4*n + 2], systems[4*n + 3]);
  slist = gamgi_engine_slist_remove_start (slist);
  }

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/************
 * set size *
 ************/

gtk_widget_size_request (treeview, &size);
if (size.height < GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN)
  size.height = GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN;
gtk_widget_set_size_request (treeview, -1, size.height);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_lattice (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *button;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;
GtkTreeIter iter;
GtkRequisition size;
gamgi_slist *slist;
char **lattices = gamgi_phys_symmetry_lattices;
int n;

dialog = gamgi_gtk_dialog_task1_create ("Crystalographic Lattices", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (vbox_dialog), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_select_lattice), window);
gtk_widget_show (treeview);


renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Lattices", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Centering", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Systems", renderer, "text", 2, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Groups", renderer, "text", 3, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (4,
G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

slist = static_list_lattice (window);
while (slist != NULL)
  {
  n = GAMGI_POINTER_TO_INT (slist->data);
  static_row (store, &iter, lattices[4*n + 0],
  lattices[4*n + 1], lattices[4*n + 2], lattices[4*n + 3]);
  slist = gamgi_engine_slist_remove_start (slist);
  }

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/************
 * set size *
 ************/

gtk_widget_size_request (treeview, &size);
if (size.height < GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN)
  size.height = GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN;
gtk_widget_set_size_request (treeview, -1, size.height);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_group (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *button;
GtkWidget *scrolled_window;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;
GtkListStore *store;
GtkTreeIter iter;
GtkRequisition size;
gamgi_slist *slist;
char label[GAMGI_ENGINE_TOKEN];
char **groups = gamgi_phys_symmetry_groups;
int n;

dialog = gamgi_gtk_dialog_task1_create ("Crystalographic Space Groups", window);
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
g_signal_connect (dialog, "delete_event",
G_CALLBACK (static_delete), window);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), scrolled_window, FALSE, FALSE, 0);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
gtk_widget_show (scrolled_window);

treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "cursor-changed",
G_CALLBACK (static_select_group), window);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Groups", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("International", renderer, "text", 1, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Schoenflies", renderer, "text", 2, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
column = gtk_tree_view_column_new_with_attributes ("Systems   ", renderer, "text", 3, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

/****************
 * add contents *
 ****************/

store = gtk_list_store_new (4,
G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

slist = static_list_group (window);
while (slist != NULL)
  {
  n = GAMGI_POINTER_TO_INT (slist->data);
  sprintf (label, "%d", n + 1);

  static_row (store, &iter, label,
  groups[4*n + 0], groups[4*n + 1], groups[4*n + 2]);

  slist = gamgi_engine_slist_remove_start (slist);
  }

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);

/************
 * set size *
 ************/

gtk_widget_size_request (treeview, &size);
if (size.height < GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN) 
  size.height = GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MIN;
if (size.height > GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MAX)
  size.height = GAMGI_GTK_CELL_SYMMETRY_HEIGHT_MAX;
gtk_widget_set_size_request (treeview, -1, size.height);

/*****************
 * Cancel button *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (static_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}

static void static_clicked_system (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/**********************
 * Create System List *
 **********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /*******************************************
   * remove previous (Lattice or Group) List *
   *******************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_group");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_system (widget, window);
  }

/**********************
 * Remove System List *
 **********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

static void static_clicked_lattice (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/***********************
 * Create Lattice List *
 ***********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /******************************************
   * remove previous (System or Group) List *
   ******************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_system");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_group");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_lattice (widget, window);
  }

/***********************
 * Remove Lattice List *
 ***********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

static void static_clicked_group (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *button;

/*********************
 * Create Group List *
 *********************/

if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == TRUE)
  {
  /********************************************
   * remove previous (System or Lattice) List *
   ********************************************/

  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_system");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  button = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "button_lattice");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);

  static_group (widget, window);
  }

/*********************
 * Remove Group List *
 *********************/

else gamgi_gtk_dialog_task1_remove (widget, window);
}

gamgi_bool static_get_bravais (char *system, 
char *lattice, int group, int *bravais)
{
char **groups = gamgi_phys_symmetry_groups;

if (group > 0 && (strcmp (system, groups[4*(group - 1) + 2]) != 0 ||
strcmp (lattice, groups[4*(group - 1) + 3]) != 0)) return FALSE;

if (strcmp (system, "a") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_TRICLINIC_P;

else if (strcmp (system, "m") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_MONOCLINIC_P;

else if (strcmp (system, "m") == 0 && strcmp (lattice, "C") == 0)
  *bravais = GAMGI_PHYS_MONOCLINIC_C;

else if (strcmp (system, "o") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_ORTHORHOMBIC_P;

else if (strcmp (system, "o") == 0 && strcmp (lattice, "I") == 0)
  *bravais = GAMGI_PHYS_ORTHORHOMBIC_I;

else if (strcmp (system, "o") == 0 && strcmp (lattice, "F") == 0)
  *bravais = GAMGI_PHYS_ORTHORHOMBIC_F;

else if (strcmp (system, "o") == 0 && strcmp (lattice, "C") == 0)
  *bravais = GAMGI_PHYS_ORTHORHOMBIC_C;

else if (strcmp (system, "t") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_TETRAGONAL_P;

else if (strcmp (system, "t") == 0 && strcmp (lattice, "I") == 0)
  *bravais = GAMGI_PHYS_TETRAGONAL_I;

else if (strcmp (system, "h") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_HEXAGONAL_P;

else if (strcmp (system, "h") == 0 && strcmp (lattice, "R") == 0)
  *bravais = GAMGI_PHYS_HEXAGONAL_R;

else if (strcmp (system, "c") == 0 && strcmp (lattice, "P") == 0)
  *bravais = GAMGI_PHYS_CUBIC_P;

else if (strcmp (system, "c") == 0 && strcmp (lattice, "I") == 0)
  *bravais = GAMGI_PHYS_CUBIC_I;

else if (strcmp (system, "c") == 0 && strcmp (lattice, "F") == 0)
  *bravais = GAMGI_PHYS_CUBIC_F;

else return FALSE;

return TRUE;
}

static void static_complete (char *system, char *lattice, int group)
{
char **groups = gamgi_phys_symmetry_groups;

/**************************************************
 * the tests for emptyness are just a precaution, *
 * as the automatic change mechanism guarantees   *
 * that if system, lattice or group are defined,  *
 * then the other two must always be compatible   *
 **************************************************/
   
if (strcmp (lattice, "R") == 0 && strcmp (system, "") == 0)
  strcpy (system, "h");

if (strcmp (system, "a") == 0 && strcmp (lattice, "") == 0)
  strcpy (lattice, "P");

if (group > 0 && strcmp (system, "") == 0)
  strcpy (system, groups[4*(group - 1) + 2]);

if (group > 0 && strcmp (lattice, "") == 0)
  strcpy (lattice, groups[4*(group - 1) + 3]);
}

gamgi_bool static_get_symmetry (gamgi_window *window,
char *system, char *lattice, int *group)
{
GtkWidget *dialog = window->dialog0;
GtkEntry *entry;
const char *name;

/*************************************
 * get direct information for system *
 *************************************/

strcpy (system, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_system");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE && 
gamgi_io_token_alpha_scan (name, system,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return FALSE;

/**************************************
 * get direct information for lattice *
 **************************************/

strcpy (lattice, "");
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE && 
gamgi_io_token_alpha_scan (name, lattice,
GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) return FALSE;

/************************************
 * get direct information for group *
 ************************************/

*group = 0;
entry = (GtkEntry *) g_object_get_data (G_OBJECT (dialog), "entry_group");
name = gtk_entry_get_text (entry);
if (gamgi_io_token_check (name) == TRUE &&
gamgi_io_token_int_scan (name, group, 1, 230) == FALSE) return FALSE;

return TRUE;
}

static void static_set_group (int number, char *group)
{
if (number > 0)
  sprintf (group, "%d", number);
else
  strcpy (group, "");
}

static void static_set_system_lattice (int bravais, 
char *system, char *lattice)
{
switch (bravais)
  {
  case GAMGI_PHYS_TRICLINIC_P:
  strcpy (system, "a");
  strcpy (lattice, "P");
  break;
  
  case GAMGI_PHYS_MONOCLINIC_P:
  strcpy (system, "m");
  strcpy (lattice, "P");
  break;

  case GAMGI_PHYS_MONOCLINIC_C:
  strcpy (system, "m");
  strcpy (lattice, "C");
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_P:
  strcpy (system, "o");
  strcpy (lattice, "P");
  break;
  
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  strcpy (system, "o");
  strcpy (lattice, "I");
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_F:
  strcpy (system, "o");
  strcpy (lattice, "F");
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C:
  strcpy (system, "o");
  strcpy (lattice, "C");
  break;

  case GAMGI_PHYS_TETRAGONAL_P:
  strcpy (system, "t");
  strcpy (lattice, "P");
  break;
 
  case GAMGI_PHYS_TETRAGONAL_I:
  strcpy (system, "t");
  strcpy (lattice, "I");
  break;
 
  case GAMGI_PHYS_HEXAGONAL_P:
  strcpy (system, "h");
  strcpy (lattice, "P");
  break;
 
  case GAMGI_PHYS_HEXAGONAL_R:
  strcpy (system, "h");
  strcpy (lattice, "R");
  break;

  case GAMGI_PHYS_CUBIC_P:
  strcpy (system, "c");
  strcpy (lattice, "P");
  break;
 
  case GAMGI_PHYS_CUBIC_I:
  strcpy (system, "c");
  strcpy (lattice, "I");
  break;

  case GAMGI_PHYS_CUBIC_F:
  strcpy (system, "c");
  strcpy (lattice, "F");
  break;
  }
}

gamgi_enum gamgi_gtk_cell_symmetry_lattice (GtkWidget *dialog)
{
GtkWidget *entry;
const char *name;
char token[GAMGI_ENGINE_TOKEN];

/**************************************************************
 * retrieve lattice: to enable/disable nodes, it is not       *
 * necessary to know if the lattice is cubic or orthorhombic, *
 * only the kind of lattice centering matters: P, I, F, C, R  *
 **************************************************************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return 0;

gamgi_io_token_alpha_scan (name, token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN);
if (strcmp (token, "P") == 0) return GAMGI_PHYS_ORTHORHOMBIC_P;
if (strcmp (token, "I") == 0) return GAMGI_PHYS_ORTHORHOMBIC_I;
if (strcmp (token, "F") == 0) return GAMGI_PHYS_ORTHORHOMBIC_F;
if (strcmp (token, "C") == 0) return GAMGI_PHYS_ORTHORHOMBIC_C;
if (strcmp (token, "R") == 0) return GAMGI_PHYS_HEXAGONAL_R;

return -1;
}

void gamgi_gtk_cell_symmetry_nodes (int lattice, 
gamgi_enum vectors, GtkWidget *dialog)
{
int *sensitive = (int *) g_object_get_data (G_OBJECT (dialog), "sensitive_o4");

if (lattice == 0)
  {
  sensitive[GAMGI_PHYS_011] = TRUE;
  sensitive[GAMGI_PHYS_101] = TRUE;
  sensitive[GAMGI_PHYS_110] = TRUE;
  sensitive[GAMGI_PHYS_111] = TRUE;
  sensitive[GAMGI_PHYS_211] = TRUE;
  sensitive[GAMGI_PHYS_122] = TRUE;
  return;
  }

sensitive[GAMGI_PHYS_011] = FALSE;
sensitive[GAMGI_PHYS_101] = FALSE;
sensitive[GAMGI_PHYS_110] = FALSE;
sensitive[GAMGI_PHYS_111] = FALSE;
sensitive[GAMGI_PHYS_211] = FALSE;
sensitive[GAMGI_PHYS_122] = FALSE;

if (lattice == -1 || vectors == GAMGI_PHYS_PRIMITIVE) return;

switch (lattice)
  {
  case GAMGI_PHYS_CUBIC_I:
  case GAMGI_PHYS_TETRAGONAL_I:
  case GAMGI_PHYS_ORTHORHOMBIC_I:
  sensitive[GAMGI_PHYS_111] = TRUE;
  break;

  case GAMGI_PHYS_CUBIC_F:
  case GAMGI_PHYS_ORTHORHOMBIC_F:
  sensitive[GAMGI_PHYS_011] = TRUE;
  sensitive[GAMGI_PHYS_101] = TRUE;
  sensitive[GAMGI_PHYS_110] = TRUE;
  break;

  case GAMGI_PHYS_ORTHORHOMBIC_C:
  case GAMGI_PHYS_MONOCLINIC_C:
  sensitive[GAMGI_PHYS_110] = TRUE;
  break;

  case GAMGI_PHYS_HEXAGONAL_R:
  sensitive[GAMGI_PHYS_211] = TRUE;
  sensitive[GAMGI_PHYS_122] = TRUE;
  break;

  default:
  break;
  }
}

gamgi_bool gamgi_gtk_cell_symmetry_parameters (gamgi_cell *cell,
gamgi_enum lattice, double *a, double *b, double *c,
double *ab, double *ac, double *bc)
{
/*************************************************************
 * This function is called from gamgi_expat_import_object    *
 * (parameters might be insufficient or incompatible) and    *
 * gamgi_gtk_cell_create (parameters might be insufficient), *
 * to create a new cell, and from gamgi_gtk_cell_modify      *
 * (parameters might be incompatible), to modify a old cell  *
 *************************************************************/

if (static_parameters_check (lattice, *a, *b, *c, *ab, *ac, *bc) == FALSE)
  return FALSE;

static_parameters_complete (lattice, a, b, c, ab, ac, bc);
static_parameters_old (cell, a, b, c, ab, ac, bc);

if (static_parameters_valid (*a, *b, *c, *ab, *ac, *bc) == FALSE)
  return FALSE;

if (static_angles_valid (*ab, *ac, *bc) == FALSE)
  return FALSE;

return TRUE;
}

gamgi_bool gamgi_gtk_cell_symmetry_import (char *system,
char *lattice, int *group, int *bravais)
{
static_complete (system, lattice, *group);

if (static_get_bravais (system, lattice,
*group, bravais) == FALSE) return FALSE;

return TRUE;
}

void gamgi_gtk_cell_symmetry_export (gamgi_cell *cell,
char *system, char *lattice)
{
static_set_system_lattice (cell->lattice, system, lattice);
}

gamgi_bool gamgi_gtk_cell_symmetry_create (int *bravais, 
int *group, gamgi_window *window)
{
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];

if (static_get_symmetry (window, system, 
lattice, group) == FALSE) return FALSE;

static_complete (system, lattice, *group);

if (static_get_bravais (system, lattice, 
*group, bravais) == FALSE) return FALSE;

return TRUE;
}

void gamgi_gtk_cell_symmetry_press (gamgi_cell *cell, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
char system[GAMGI_ENGINE_TOKEN];
char lattice[GAMGI_ENGINE_TOKEN];
char group[GAMGI_ENGINE_TOKEN];

/************************
 * show system, lattice *
 ************************/

static_set_system_lattice (cell->lattice, system, lattice);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_system");
gtk_entry_set_text (GTK_ENTRY (entry), system);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_lattice");
gtk_entry_set_text (GTK_ENTRY (entry), lattice);

/**************
 * show group *
 **************/

static_set_group (cell->group, group);
entry = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "entry_group");
gtk_entry_set_text (GTK_ENTRY (entry), group);
}

void gamgi_gtk_cell_symmetry (GtkWidget *notebook, gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *entry;
GtkWidget *label;
GtkWidget *table;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *vbox_page, *vbox_center, *vbox_top;
GtkWidget *button;

vbox_page = gtk_vbox_new (FALSE, 5);
label = gtk_label_new ("Symmetry");
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_center = gtk_vbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_center, TRUE, TRUE, 0);
gtk_widget_show (vbox_center);

vbox_top = gtk_vbox_new (FALSE, 15);
gtk_box_pack_start (GTK_BOX (vbox_center), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

/************************
 * system,lattice,group *
 ************************/

table = gtk_table_new (2, 3, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 0);
gtk_widget_show (table);

label = gtk_label_new ("System");
gtk_table_attach (GTK_TABLE (table), 
label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("Lattice");
gtk_table_attach (GTK_TABLE (table), 
label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

label = gtk_label_new ("Group");
gtk_table_attach (GTK_TABLE (table), 
label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 5);
gtk_widget_show (label);

/**********
 * system *
 **********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_table_attach (GTK_TABLE (table), 
hbox_left, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_clicked_system), window);
g_object_set_data (G_OBJECT (dialog), "button_system", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_changed_system), window);
g_object_set_data (G_OBJECT (dialog), "entry_system", entry);
gtk_widget_show (entry);

/***********
 * lattice *
 ***********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_table_attach (GTK_TABLE (table), 
hbox_left, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_clicked_lattice), window);
g_object_set_data (G_OBJECT (dialog), "button_lattice", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_changed_lattice), window);
g_object_set_data (G_OBJECT (dialog), "entry_lattice", entry);
gtk_widget_show (entry);

/*********
 * group *
 *********/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_table_attach (GTK_TABLE (table), 
hbox_left, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 10, 0);
gtk_widget_show (hbox_left);

button = gtk_toggle_button_new_with_label ("List");
gtk_box_pack_end (GTK_BOX (hbox_left), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_clicked_group), window);
g_object_set_data (G_OBJECT (dialog), "button_group", button);
gtk_widget_show (button);

entry = gtk_entry_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_3);
g_signal_connect (entry, "changed",
G_CALLBACK (static_changed_group), window);
g_object_set_data (G_OBJECT (dialog), "entry_group", entry);
gtk_widget_show (entry);

/**************
 * parameters *
 **************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

table = gtk_table_new (2, 7, FALSE);
gtk_box_pack_start (GTK_BOX (hbox_center), table, FALSE, FALSE, 5);
gtk_widget_show (table);

label = gtk_label_new ("Length");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("A");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_a", label);
gtk_widget_show (label);

label = gtk_label_new ("B");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_b", label);
gtk_widget_show (label);

label = gtk_label_new ("C");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_c", label);
gtk_widget_show (label);

label = gtk_label_new ("Angle");
gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_widget_show (label);

label = gtk_label_new ("BC");
gtk_table_attach (GTK_TABLE (table), label, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_bc", label);
gtk_widget_show (label);

label = gtk_label_new ("AC");
gtk_table_attach (GTK_TABLE (table), label, 4, 5, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_ac", label);
gtk_widget_show (label);

label = gtk_label_new ("AB");
gtk_table_attach (GTK_TABLE (table), label, 6, 7, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
g_object_set_data (G_OBJECT (dialog), "label_ab", label);
gtk_widget_show (label);

/**********
 * length *
 **********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_a", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_b", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 0, 1, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_c", entry);
gtk_widget_show (entry);

/*********
 * angle *
 *********/

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_bc", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 3, 4, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_ac", entry);
gtk_widget_show (entry);

entry = gtk_entry_new ();
gtk_table_attach (GTK_TABLE (table), entry, 5, 6, 1, 2, GTK_FILL, GTK_FILL, 5, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_7);
g_object_set_data (G_OBJECT (dialog), "entry_ab", entry);
gtk_widget_show (entry);
}
