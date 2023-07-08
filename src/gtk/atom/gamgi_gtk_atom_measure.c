/************************************************
 *
 * $GAMGI/src/gtk/atom/gamgi_gtk_atom_measure.c
 *
 * Copyright (C) Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_math.h"
#include "gamgi_phys.h"
#include "gamgi_global.h"

#include "gamgi_engine_find.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_mesa_select.h"
#include "gamgi_math_measure.h"
#include "gamgi_io_token.h"

static GtkWidget *static_status (gamgi_window *window, int *page, int *property)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;
GtkWidget *vbox;
GtkWidget *combo;

/****************
 * get notebook *
 ****************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
*page = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));
vbox = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), *page);

/****************
 * get property *
 ****************/

combo = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "combo_property");
*property = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));

return vbox;
}

static gamgi_enum static_class (gamgi_window *window)
{
GtkWidget* entry;
GtkWidget* vbox;
int page, property;
const char *name;

/********************************************
 * select object class from notebook status *
 ********************************************/

vbox = static_status (window, &page, &property);

/***********
 * level 0 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object0");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE)
  return GAMGI_ENGINE_ATOM;

/***********
 * level 1 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 3 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_DIRECTION;
  if (property == 4 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_PLANE;
  break;

  case 1:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

/***********
 * level 2 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;

  case 1:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

/***********
 * level 3 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;

  case 1:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return GAMGI_ENGINE_ATOM;
  break;
  }

return GAMGI_ENGINE_ATOM;
}

static GtkWidget *static_entry (GtkWidget *vbox, int page, int property)
{
GtkWidget *entry;
const char *name;

/***********
 * level 0 *
 ***********/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object0");
name = gtk_entry_get_text (GTK_ENTRY (entry));
if (gamgi_io_token_check (name) == FALSE) return entry;

/***********
 * level 1 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 3 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 4 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 1:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

/***********
 * level 2 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 1:
  if (property == 0 && gamgi_io_token_check (name) == FALSE)
    return entry;
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

/***********
 * level 3 *
 ***********/

switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  name = gtk_entry_get_text (GTK_ENTRY (entry));
  }

switch (page)
  {
  case 0:
  if (property == 2 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;

  case 1:
  if (property == 1 && gamgi_io_token_check (name) == FALSE)
    return entry;
  break;
  }

return NULL;
}

static gamgi_plane *static_plane (GtkWidget *vbox,
char *id, gamgi_window *window)
{
gamgi_plane *plane;

/*************
 * Get Plane *
 *************/

plane = GAMGI_CAST_PLANE gamgi_gtk_object_name_number (vbox,
id, GAMGI_ENGINE_PLANE, window);
if (plane == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Plane Object", window);
  return NULL;
  }

return plane;
}

static gamgi_direction *static_direction (GtkWidget *vbox,
char *id, gamgi_window *window)
{
gamgi_direction *direction;

/*****************
 * Get Direction *
 *****************/

direction = GAMGI_CAST_DIRECTION gamgi_gtk_object_name_number (vbox,
id, GAMGI_ENGINE_DIRECTION, window);
if (direction == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Direction Object", window);
  return NULL;
  }

return direction;
}

static gamgi_atom *static_atom (GtkWidget *vbox,
char *id, gamgi_window *window)
{
gamgi_atom *atom;

/************
 * Get Atom *
 ************/

atom = GAMGI_CAST_ATOM gamgi_gtk_object_name_number (vbox,
id, GAMGI_ENGINE_ATOM, window);
if (atom == NULL)
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Object", window);
  return NULL;
  }

return atom;
}

/******************** internal function ****************
 *                                                     *
 *                       STATIC_RESET                  *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_reset (GtkWidget* vbox, int page)
{
GtkWidget *entry;

/**************
 * clean page *
 **************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object0");
gtk_entry_set_text (GTK_ENTRY (entry), "");
switch (page)
  {
  case 0: case 1:
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  gtk_entry_set_text (GTK_ENTRY (entry), "");
  break;
  }
entry = g_object_get_data (G_OBJECT (vbox), "entry_value");
gtk_entry_set_text (GTK_ENTRY (entry), "");
}

static void static_property (GtkWidget *vbox,
int page, int property, gamgi_window *window)
{
GtkWidget *label1, *label2, *label3;
GtkWidget *entry1, *entry2, *entry3;

/****************
 * clean page *
 ****************/

static_reset (vbox, page);

/***********************************
 * this unneeded initialization    *
 * avoids a O3 compilation warning *
 ***********************************/

label1 = label2 = label3 = NULL;
entry1 = entry2 = entry3 = NULL;

/***************************************
 * get widgets for all levels except 0 *
 ***************************************/

switch (page)
  {
  case 0: case 1:
  entry1 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object1");
  label1 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object1");
  entry2 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object2");
  label2 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object2");
  entry3 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_object3");
  label3 = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "label_object3");
  }

/**************************************
 * set labels for all levels except 0 *
 **************************************/

switch (page)
  {
  case 0:
  switch (property)
    {
    case 0:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;
 
    case 1:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 2:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_label_set_text (GTK_LABEL (label3), "Atom");
    break;

    case 3:
    gtk_label_set_text (GTK_LABEL (label1), "Direction");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 4:
    gtk_label_set_text (GTK_LABEL (label1), "Plane");
    gtk_entry_set_text (GTK_ENTRY (entry2), "");
    gtk_label_set_text (GTK_LABEL (label2), "Object");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;
    }
  break;

  case 1:
  switch (property)
    {
    case 0:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_entry_set_text (GTK_ENTRY (entry3), "");
    gtk_label_set_text (GTK_LABEL (label3), "Object");
    break;

    case 1:
    gtk_label_set_text (GTK_LABEL (label1), "Atom");
    gtk_label_set_text (GTK_LABEL (label2), "Atom");
    gtk_label_set_text (GTK_LABEL (label3), "Atom");
    break;
    }
  break;
  }

/***************************************
 * set widgets for all levels except 0 *
 ***************************************/

switch (page)
  {
  case 0:
  switch (property)
    {
    case 0:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 1:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 2:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, TRUE);
    gtk_widget_set_sensitive (label3, TRUE);
    break;

    case 3:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 4:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, FALSE);
    gtk_widget_set_sensitive (label2, FALSE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;
    }
  break;

  case 1:
  switch (property)
    {
    case 0:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, FALSE);
    gtk_widget_set_sensitive (label3, FALSE);
    break;

    case 1:
    gtk_widget_set_sensitive (entry1, TRUE);
    gtk_widget_set_sensitive (label1, TRUE);
    gtk_widget_set_sensitive (entry2, TRUE);
    gtk_widget_set_sensitive (label2, TRUE);
    gtk_widget_set_sensitive (entry3, TRUE);
    gtk_widget_set_sensitive (label3, TRUE);
    break;
    }
  break;
  }

}

static void static_activate (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *vbox;
int page, property;

/***********************
 * get notebook status *
 ***********************/

vbox = static_status (window, &page, &property);

/*******************
 * update property *
 *******************/

static_property (vbox, page, property, window);
}

/******************** internal function ****************
 *                                                     *
 *                      STATIC_SWITCH                  *
 *                                                     *
 *                                                     *
 *******************************************************/
  
static void static_switch (GtkNotebook *notebook, 
GtkNotebookPage *notebook_page, int page, void *data)
{ 
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *combo;
GtkWidget* vbox;
  
/***********************************************
 * reset menu, object widgets, object contents *
 ***********************************************/

vbox = gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), page);
combo = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "combo_property");
gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
static_property (vbox, page, 0, window);
}

/******************** internal function ****************
 *                                                     *
 *                        STATIC_OK                    *
 *                                                     *
 *                                                     *
 *******************************************************/

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
gamgi_atom *atom0;
gamgi_atom *atom1;
gamgi_atom *atom2;
gamgi_atom *atom3;
gamgi_plane *plane1;
gamgi_direction *direction1;
GtkWidget* vbox;
GtkWidget *entry;
char token[GAMGI_ENGINE_TOKEN];
double length, angle;
int page, property;

/**************
 * get status *
 **************/

vbox = static_status (window, &page, &property);

/***************
 * get objects *
 ***************/

atom1 = NULL; atom2 = NULL; atom3 = NULL;
plane1 = NULL; direction1 = NULL;

atom0 = static_atom (vbox, "entry_object0", window);
if (atom0 == NULL) return;

switch (page)
  {
  case 0:
  switch (property)
    {
    case 0:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    break;

    case 1:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    break;

    case 2:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    atom3 = static_atom (vbox, "entry_object3", window);
    if (atom3 == NULL) return;
    break;

    case 3:
    direction1 = static_direction (vbox, "entry_object1", window);
    if (direction1 == NULL) return;
    break;

    case 4:
    plane1 = static_plane (vbox, "entry_object1", window);
    if (plane1 == NULL) return;
    break;
    }
  break;

  case 1:
  switch (property)
    {
    case 0:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    break;

    case 1:
    atom1 = static_atom (vbox, "entry_object1", window);
    if (atom1 == NULL) return;
    atom2 = static_atom (vbox, "entry_object2", window);
    if (atom2 == NULL) return;
    atom3 = static_atom (vbox, "entry_object3", window);
    if (atom3 == NULL) return;
    break;
    }
  break;
  }

/*****************************
 * objects must be different *
 *****************************/

if (atom3 != NULL && (atom3 == atom2 || atom3 == atom1 || atom3 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Data", window);
  return;
  }

if (atom2 != NULL && (atom2 == atom1 || atom2 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Data", window);
  return;
  }

if (atom1 != NULL && (atom1 == atom0))
  {
  gamgi_gtk_dialog_message_create ("Error", "Invalid Atom Data", window);
  return;
  }

/**************
 * get result *
 **************/

switch (page)
  {
  case 0:
  switch (property)
    {
    case 0:
    if (gamgi_math_measure_length_2atom (atom0, 
    atom1, &length) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->length, length);
    break;

    case 1:
    if (gamgi_math_measure_length_atom_2atom (atom0, 
    atom1, atom2, &length) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->length, length);
    break;

    case 2:
    if (gamgi_math_measure_length_atom_3atom (atom0,
    atom1, atom2, atom3, &length) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->length, length);
    break;
    
    case 3:
    if (gamgi_math_measure_length_atom_direction (atom0, 
    direction1, &length) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->length, length);
    break;

    case 4:
    if (gamgi_math_measure_length_atom_plane (atom0, 
    plane1, &length) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->length, length);
    break;
    }
  break;

  case 1:
  switch (property)
    {
    case 0:
    if (gamgi_math_measure_angle_3atom (atom0, 
    atom1, atom2, &angle, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->angle, angle);
    break;

    case 1:
    if (gamgi_math_measure_angle_4atom (atom0, 
    atom1, atom2, atom3, &angle, window->layer) == FALSE)
      {
      gamgi_gtk_dialog_message_create ("Error", "Invalid Geometry Data", window);
      return;
      }
    sprintf (token, "%.*f", gamgi->gamgi->angle, angle);
    break;
    }
  break;
  }

/***************
 * show result *
 ***************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_value");
gtk_entry_set_text (GTK_ENTRY (entry), token);
}

static void static_init (gamgi_window *window)
{
GtkWidget *dialog = window->dialog0;
GtkWidget *notebook;

/**********************************
 * set notebook page and property *
 **********************************/

notebook = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "notebook");
gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), 0);
static_switch (GTK_NOTEBOOK (notebook), NULL, 0, window);

g_signal_connect (notebook, "switch_page",
G_CALLBACK (static_switch), window);
}

static void static_press (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget* vbox;
GtkWidget *entry;
char string[2 * GAMGI_ENGINE_TOKEN];
int page, property;

vbox = static_status (window, &page, &property);

/****************
 * clean output *
 ****************/

entry = (GtkWidget *) g_object_get_data (G_OBJECT (vbox), "entry_value");
gtk_entry_set_text (GTK_ENTRY (entry), "");

/****************
 * enter object *
 ****************/

entry = static_entry (vbox, page, property);
if (entry == NULL)
  {
  static_reset (vbox, page);
  entry = static_entry (vbox, page, property); 
  }

sprintf (string, "%s %d", object->name, object->number);
gtk_entry_set_text (GTK_ENTRY (entry), string);

/********************
 * measure property *
 ********************/

entry = static_entry (vbox, page, property);
if (entry == NULL) static_ok (NULL, window);
}

/****************** external function ******************
 *                                                     *
 *             GAMGI_GTK_ATOM_MEASURE_PRESS            *
 *                                                     *
 *                                                     *
 *******************************************************/

void gamgi_gtk_atom_measure_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog)
{
/******************************
 * local mouse selection only *
 ******************************/

if (window_dialog != window_mouse) return;
gamgi_mesa_select_object (window_mouse, x, y, 
static_class (window_dialog), FALSE, static_press);
}

/****************** external function ******************
 *                                                     *
 *                GAMGI_GTK_ATOM_MEASURE               *
 *                                                     *
 *                                                     *
 *******************************************************/

void gamgi_gtk_atom_measure (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *notebook;
GtkWidget *vbox_dialog, *vbox_page, *vbox_top;
GtkWidget *hbox_center, *hbox_left;
GtkWidget *entry;
GtkWidget *button;
GtkWidget *label;
GtkWidget *combo;
GtkListStore *store;
GtkCellRenderer *renderer;
GtkTreeIter iter;

/******************
 * dialog level 0 *
 ******************/

dialog = gamgi_gtk_dialog_task0_create ("Atom Measure", window);
window->action = GAMGI_GTK_ATOM_MEASURE;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

/********************
 * global container *
 ********************/

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

/*******************
 * create notebook *
 *******************/

notebook = gtk_notebook_new ();
gtk_box_pack_start (GTK_BOX (vbox_dialog), notebook, FALSE, FALSE, 0);
gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
g_object_set_data (G_OBJECT (dialog), "notebook", notebook);
gtk_widget_show (notebook);

/***************
 * Length page *
 ***************/

label = gtk_label_new ("Length");
vbox_page = gtk_vbox_new (FALSE, 0);
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * Property menu *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 15);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Property");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_activate), window);
g_object_set_data (G_OBJECT (vbox_page), "combo_property", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "2-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom 2-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom 3-Atom Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom Direction Distance", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "Atom Plane Distance", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/******************
 * Object0: Atom *
 ******************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object0", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Atom");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/************
 * Object 1 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object1", label);
gtk_widget_show (label);

/************
 * Object 2 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object2", label);
gtk_widget_show (label);

/************
 * Object 3 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object3", label);
gtk_widget_show (label);

/*********
 * Value *
 *********/

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (vbox_page), "entry_value", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Value");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

/**************
 * Angle page *
 **************/

label = gtk_label_new ("Angle");
vbox_page = gtk_vbox_new (FALSE, 0);
gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox_page, label);
gtk_container_set_border_width (GTK_CONTAINER (vbox_page), 10);
gtk_widget_show (vbox_page);

vbox_top = gtk_vbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_page), vbox_top, FALSE, FALSE, 0);
gtk_widget_show (vbox_top);

/*****************
 * Property menu *
 *****************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (hbox_center), hbox_left, FALSE, FALSE, 15);
gtk_widget_show (hbox_left);

label = gtk_label_new ("Property");
gtk_box_pack_start (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

combo = gtk_combo_box_new ();
gtk_box_pack_start (GTK_BOX (hbox_left), combo, FALSE, FALSE, 0);
g_signal_connect (combo, "changed",
G_CALLBACK (static_activate), window);
g_object_set_data (G_OBJECT (vbox_page), "combo_property", combo);
gtk_widget_show (combo);

store = gtk_list_store_new (1, G_TYPE_STRING);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "3-Atom Angle", -1);
gtk_list_store_append (store, &iter);
gtk_list_store_set (store, &iter, 0, "4-Atom Torsion", -1);
gtk_combo_box_set_model (GTK_COMBO_BOX (combo), GTK_TREE_MODEL (store));
g_object_unref (store);

renderer = gtk_cell_renderer_text_new ();
gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), renderer, FALSE);
gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer, "text", 0, NULL);

/*****************
 * Object0: Atom *
 *****************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object0", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Atom");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
gtk_widget_show (label);

/************
 * Object 1 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object1", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object1", label);
gtk_widget_show (label);

/************
 * Object 2 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object2", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object2", label);
gtk_widget_show (label);

/************
 * Object 3 *
 ************/

hbox_left = gtk_hbox_new (FALSE, 5);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_20);
g_object_set_data (G_OBJECT (vbox_page), "entry_object3", entry);
gtk_widget_show (entry);

label = gtk_label_new ("");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 0);
g_object_set_data (G_OBJECT (vbox_page), "label_object3", label);
gtk_widget_show (label);

/*********
 * Value *
 *********/

hbox_left = gtk_hbox_new (FALSE, 0);
gtk_box_pack_start (GTK_BOX (vbox_top), hbox_left, FALSE, FALSE, 0);
gtk_widget_show (hbox_left);

entry = gtk_entry_new ();
gtk_box_pack_end (GTK_BOX (hbox_left), entry, FALSE, FALSE, 0);
gtk_entry_set_width_chars (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
gtk_entry_set_max_length (GTK_ENTRY (entry), GAMGI_GTK_CHAR_10);
g_object_set_data (G_OBJECT (vbox_page), "entry_value", entry);
gtk_widget_show (entry);

label = gtk_label_new ("Value");
gtk_box_pack_end (GTK_BOX (hbox_left), label, FALSE, FALSE, 5);
gtk_widget_show (label);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_end (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

static_init (window);
gtk_widget_show (dialog);
}
