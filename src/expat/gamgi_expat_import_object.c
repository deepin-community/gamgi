/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import_object.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_math.h"
#include "gamgi_chem.h"
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_engine_create.h"
#include "gamgi_engine_start.h"
#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_history.h"
#include "gamgi_gtk_cell_symmetry.h"
#include "gamgi_mesa_start.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_mesa_render.h"
#include "gamgi_mesa_text.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_matrix.h"
#include "gamgi_math_euler.h"
#include "gamgi_math_quaternion.h"
#include "gamgi_math_hash.h"
#include "gamgi_math_cell.h"
#include "gamgi_math_position.h"
#include "gamgi_math_node.h"
#include "gamgi_math_polygon.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_orbital.h"
#include "gamgi_phys_cell.h"
#include "gamgi_phys_projection.h"
#include "gamgi_phys_direction.h"
#include "gamgi_io_token.h"
#include "gamgi_io_error.h"
#include "gamgi_expat_import.h"
#include "gamgi_expat_import_config.h"
#include "gamgi_global_copy.h"

static gamgi_bool static_gml_end (const char *element, gamgi_gml *gml)
{
/************************************************
 * parent stack ends here: output parent = NULL *
 ************************************************/

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

/**********************************************************
 * link objects to external parents indicated by id names *
 **********************************************************/

return gamgi_expat_import_connect (gml);
}

static gamgi_bool static_gml_start (const char *element, 
const char **attributes, gamgi_gml *gml)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************************
 * gml should be the first element *
 ***********************************/

if (gamgi_expat_import_parent_gml (GAMGI_EXPAT_GML, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/****************************
 * object queue starts here *
 ****************************/

gml->object_end = gamgi_engine_dlist_add_end (NULL);
gml->object_end->data = NULL;

gml->object_start = gml->object_end;

/****************************
 * parent stack starts here *
 ****************************/

gml->ml.parent = gamgi_engine_slist_add_start (NULL);
gml->ml.parent->data = NULL;

/*********************************
 * gml should have no attributes *
 *********************************/

if (attributes[0] != NULL)
  return gamgi_io_error_attribute (attributes[0],
  gml->ml.filename, fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_gamgi_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_gamgi_class *gamgi_class;
gamgi_object *object;
int fileline, i;
int button = 0;
int background = 0, foreground = 0;
int title = 0, bold = 0, link = 0;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_GAMGI, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * gamgi_class contains the current config info, *
 * which is gamgi->gamgi unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->gamgi;
if (object->object == NULL)
  {
  gamgi_class = gamgi_global_copy_gamgi (gamgi->gamgi);
  object->object = GAMGI_CAST_OBJECT gamgi_class;
  }
else gamgi_class = (gamgi_gamgi_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = gamgi_class;

/******************************************************
 * parser is entering a gamgi (configuration) block:  *
 * switch the data handlers to configuration handlers *
 ******************************************************/

XML_SetStartElementHandler (gml->ml.parser, gamgi_expat_import_config_start);
XML_SetEndElementHandler (gml->ml.parser, gamgi_expat_import_config_end);

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************************
   * mouse parameters:           *
   * beep sound, pick tolerance, *
   * left, middle, right buttons *
   *******************************/

  if (strcmp (attributes[i], "sound") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->beep = gamgi->gamgi->beep;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_bool_scan (attributes[i + 1],
    &gamgi_class->beep) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "tolerance") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->tolerance = gamgi->gamgi->tolerance;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->tolerance, 0, GAMGI_MESA_PICK_TOLERANCE_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "button1") == 0)
    {
    button++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->button1 = gamgi->gamgi->button1;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->button1, 1, 3) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    /***********************************************************
     * convert 1,2,3 to BUTTON1_MASK,BUTTON2_MASK,BUTTON3_MASK *
     ***********************************************************/

    else gamgi_class->button1 = 
    gamgi_gtk_dialog_button_mask (gamgi_class->button1);
    }

  else if (strcmp (attributes[i], "button2") == 0)
    {
    button++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->button2 = gamgi->gamgi->button2;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->button2, 1, 3) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    /***********************************************************
     * convert 1,2,3 to BUTTON1_MASK,BUTTON2_MASK,BUTTON3_MASK *
     ***********************************************************/

    else gamgi_class->button2 = 
    gamgi_gtk_dialog_button_mask (gamgi_class->button2);
    }

  else if (strcmp (attributes[i], "button3") == 0)
    {
    button++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->button3 = gamgi->gamgi->button3;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->button3, 1, 3) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    /***********************************************************
     * convert 1,2,3 to BUTTON1_MASK,BUTTON2_MASK,BUTTON3_MASK *
     ***********************************************************/

    else gamgi_class->button3 = 
    gamgi_gtk_dialog_button_mask (gamgi_class->button3);
    }

  /*******************************
   * number of decimal figures:  *
   * length, angle, charge, mass *
   *******************************/

  else if (strcmp (attributes[i], "length") == 0) 
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->length = gamgi->gamgi->length;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->length, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "angle") == 0) 
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->angle = gamgi->gamgi->angle;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->angle, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "mass") == 0) 
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->mass = gamgi->gamgi->mass;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->mass, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "charge") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->charge = gamgi->gamgi->charge;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &gamgi_class->charge, 0, GAMGI_MATH_DECIMAL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*********************************
   * color parameters: background, *
   * foreground, title, bold, link *
   *********************************/

  /****************************************
   * red,green,blue background (optional) *
   ****************************************/

  else if (strcmp (attributes[i], "base_r") == 0)
    {
    background++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->background[0] = gamgi->gamgi->background[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->background[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "base_g") == 0)
    {
    background++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->background[1] = gamgi->gamgi->background[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->background[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "base_b") == 0)
    {
    background++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->background[2] = gamgi->gamgi->background[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->background[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /****************************************
   * red,green,blue foreground (optional) *
   ****************************************/

  else if (strcmp (attributes[i], "text_r") == 0)
    {
    foreground++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->foreground[0] = gamgi->gamgi->foreground[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->foreground[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "text_g") == 0)
    {
    foreground++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->foreground[1] = gamgi->gamgi->foreground[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->foreground[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "text_b") == 0)
    {
    foreground++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->foreground[2] = gamgi->gamgi->foreground[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->foreground[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***********************************
   * red,green,blue title (optional) *
   ***********************************/

  else if (strcmp (attributes[i], "title_r") == 0)
    {
    title++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->title[0] = gamgi->gamgi->title[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->title[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "title_g") == 0)
    {
    title++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->title[1] = gamgi->gamgi->title[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->title[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "title_b") == 0)
    {
    title++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->title[2] = gamgi->gamgi->title[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->title[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************
   * red,green,blue bold (optional) *
   **********************************/

  else if (strcmp (attributes[i], "bold_r") == 0)
    {
    bold++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->bold[0] = gamgi->gamgi->bold[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->bold[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "bold_g") == 0)
    {
    bold++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->bold[1] = gamgi->gamgi->bold[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->bold[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "bold_b") == 0)
    {
    bold++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->bold[2] = gamgi->gamgi->bold[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->bold[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************
   * red,green,blue link (optional) *
   **********************************/

  else if (strcmp (attributes[i], "link_r") == 0)
    {
    link++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->link[0] = gamgi->gamgi->link[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->link[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "link_g") == 0)
    {
    link++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->link[1] = gamgi->gamgi->link[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->link[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "link_b") == 0)
    {
    link++;
 
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      gamgi_class->link[2] = gamgi->gamgi->link[2];

    /**********
     * update * 
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &gamgi_class->link[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/******************************
 * global: missing attributes *
 ******************************/

/*************************************************
 * button1, button2, button3 (coupled, optional) *
 *************************************************/

if (button == 1 || button == 2)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

/************************************************************************
 * background, foreground, title, bold, link colors (coupled, optional) *
 ************************************************************************/

if (background == 1 || background == 2)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (foreground == 1 || foreground == 2) 
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (title == 1 || title == 2)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window); 

if (bold == 1 || bold == 2) 
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (link == 1 || link == 2)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

/***********************************
 * global: incompatible attributes *
 ***********************************/

/*******************************************
 * (button1,button2,button3) must point to *
 * one of the six valid (1,2,3) sequences. *
 *******************************************/

if (gamgi_class->button1 == gamgi_class->button2 ||
gamgi_class->button1 == gamgi_class->button3 ||
gamgi_class->button2 == gamgi_class->button3)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_window_end (const char *element, gamgi_gml *gml)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/*****************************************************
 * <window/> or <window></window> is not acceptable, *
 * windows must own at least an empty layer. This    *
 * error is not detected by parent checking, so we   *
 * need to compare the parent (which must be window) *
   with the last object (which must not be window).  *
 *****************************************************/

if (gml->object_end->data == gml->ml.parent->data)
  return gamgi_io_error_empty (gml->ml.filename, 
  fileline, gml->ml.window);

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_window_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_window *window;
gamgi_window_class *window_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int origin = 0, size = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_WINDOW, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/**************************************************
 * window_class contains the current config info, *
 * which is gamgi->window unless new config info  *
 * has been previously stored in object->object.  *
 **************************************************/

object = GAMGI_CAST_OBJECT gamgi->window;
if (object->object == NULL) window_class = gamgi->window;
else window_class = (gamgi_window_class *) object->object;

/******************
 * object: create *
 ******************/

window = gamgi_engine_create_window ();
gamgi_engine_start_window (window);
gamgi_mesa_start_window (window, window_class);

window->object.object = GAMGI_CAST_OBJECT gml->ml.parent->data;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = window;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = window;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /********
   * name *
   ********/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    window->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    
    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /****************************************************
   *                geometry parameters               *
   *                                                  *
   * origin: x, y must be in the ranges               *
   * [-screen_width, +screen_width]                   *
   * [-screen_height, +screen_height]                 *
   *                                                  *
   * size: w, h must be in the ranges                 *
   * [GAMGI_GTK_WINDOW_WIDTH_MIN, 2 * screen_width]   *
   * [GAMGI_GTK_WINDOW_HEIGHT_MIN, 2 * screen_height] *
   ****************************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    origin++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &window->origin_x, -gamgi->width, gamgi->width) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    origin++;
    if (gamgi_io_token_int_scan (attributes[i + 1], 
    &window->origin_y, -gamgi->height, gamgi->height) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "width") == 0)
    {
    size++;
    if (gamgi_io_token_int_scan (attributes[i + 1], &window->width, 
    GAMGI_GTK_WINDOW_WIDTH_MIN, 2 * gamgi->width) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "height") == 0)
    {
    size++;
    if (gamgi_io_token_int_scan (attributes[i + 1], &window->height, 
    GAMGI_GTK_WINDOW_HEIGHT_MIN, 2 * gamgi->height) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***********************
   * show parameters:    *
   * top, medium, bottom *
   ***********************/

  else if (strcmp (attributes[i], "top") == 0) 
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1], 
    &window->top_flag) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "medium") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &window->medium_flag) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "bottom") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &window->bottom_flag) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

if (origin == 1)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (size == 1)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_layer_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_layer_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_layer *layer;
gamgi_layer_class *layer_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
double x[3], z[3], up_z[3];
double length;
int fileline, i;
int eye = 0, center = 0, up = 0, color = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_LAYER, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * layer_class contains the current config info, *
 * which is gamgi->layer unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->layer;
if (object->object == NULL) layer_class = gamgi->layer;
else layer_class = (gamgi_layer_class *) object->object;

/******************
 * object: create *
 ******************/

layer = gamgi_engine_create_layer ();
gamgi_engine_start_layer (layer);
gamgi_mesa_start_layer (layer, layer_class);

/************************************************************
 * by default, each new layer belongs to the current window *
 ************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window;
layer->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = layer;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = layer;

/**********************************
 * start undo and save mechanisms *
 **********************************/

gamgi_gtk_history_start (layer);

/*************************************************************
 * new top layer: we cannot update gml->ml.window->layer     *
 * untill we are sure that the file is good. For new windows *
 * this problem does not exist, as its whole contents will   *
 * be removed if the file is bad. When the import operation  *
 * ends ok, the top layer becomes also the curret object     *
 *************************************************************/

if (object != GAMGI_CAST_OBJECT gml->ml.window)
  (GAMGI_CAST_WINDOW object)->layer = layer;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    layer->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /*************************************************
   * perspective, top, near, far planes (optional) *
   *************************************************/

  else if (strcmp (attributes[i], "perspective") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &layer->perspective) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "top") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer->top, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "near") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->near, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "far") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->far, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************
   * eye_x, eye_y, eye_z (optional) *
   **********************************/

  else if (strcmp (attributes[i], "eye_x") == 0)
    {
    eye++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->eye[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "eye_y") == 0)
    {
    eye++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->eye[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "eye_z") == 0)
    {
    eye++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->eye[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************************************
   * center_x, center_y, center_z (optional) *
   *******************************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->center[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->center[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->center[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************************
   * up_x, up_y, up_z (optional) *
   *******************************/

  else if (strcmp (attributes[i], "up_x") == 0)
    {
    up++;
    if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer->up[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "up_y") == 0)
    {
    up++;
    if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer->up[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "up_z") == 0)
    {
    up++;
    if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer->up[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

   /*******************************************
    * visibility_in,visibility_out (optional) *
    *******************************************/

   else if (strcmp (attributes[i], "visibility_in") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "all") == 0)
       layer->visibility_in = GAMGI_GTK_ALL;
    else if (strcmp (token, "partial") == 0)
       layer->visibility_in = GAMGI_GTK_PARTIAL;
    else if (strcmp (token, "none") == 0)
       layer->visibility_in = GAMGI_GTK_NONE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "visibility_out") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "all") == 0)
       layer->visibility_out = GAMGI_GTK_ALL;
    else if (strcmp (token, "partial") == 0)
       layer->visibility_out = GAMGI_GTK_PARTIAL;
    else if (strcmp (token, "none") == 0)
       layer->visibility_out = GAMGI_GTK_NONE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &layer->color[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &layer->color[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &layer->color[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * axes (optional) *
   *******************/

  else if (strcmp (attributes[i], "axes") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &layer->axes) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (layer->near + GAMGI_MATH_TOLERANCE_LENGTH > layer->far)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (eye == 1 || eye == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (center == 1 || center == 2) 
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (up == 1 || up == 2) 
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

/************************************************************
 * check view parameters:                                   *
 * 1) the distance eye-center must be in the near-far range *
 * 2) the up component normal to eye-center cannot be zero  *
 ************************************************************/

gamgi_math_vector_sub (layer->eye, layer->center, z);
length = gamgi_math_vector_length (z);
if (length < layer->near || length > layer->far)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);
gamgi_math_vector_normal (z);

length = gamgi_math_vector_dot (layer->up, z);
gamgi_math_vector_scale (z, up_z, length);
gamgi_math_vector_sub (layer->up, up_z, layer->up);
length = gamgi_math_vector_length (layer->up);
if (length < GAMGI_MATH_TOLERANCE_LENGTH)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);
gamgi_math_vector_normal (layer->up);

gamgi_math_vector_cross (layer->up, z, x);
gamgi_math_matrix_absolute (layer->referential,
x[0], layer->up[0], z[0], x[1], layer->up[1], z[1], x[2], layer->up[2], z[2]);

return gml->ml.valid;
}

static gamgi_bool static_light_end (const char *element, gamgi_gml *gml)
{
/**********************
 * handle parent list *
 **********************/

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_light_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_light *light;
gamgi_light_class *light_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int ref = 0;
int direction = 0, position = 0;
int ambient = 0, diffuse = 0, specular = 0;
int constant = 0, linear = 0, quadratic = 0, radial = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_LIGHT, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * light_class contains the current config info, *
 * which is gamgi->light unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->light;
if (object->object == NULL) light_class = gamgi->light;
else light_class = (gamgi_light_class *) object->object;

/******************
 * object: create *
 ******************/

light = gamgi_engine_create_light ();
gamgi_engine_start_light (light);
gamgi_mesa_start_light (light, light_class);

/***********************************************************
 * by default, each new light belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
light->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = light;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = light;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    light->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename,
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    ref++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /***************************************
   * red,green,blue ambient (coupled,    *
   * one type required, others optional) *
   ***************************************/

  else if (strcmp (attributes[i], "ambient_r") == 0)
    {
    ambient++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->ambient[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ambient_g") == 0)
    {
    ambient++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->ambient[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ambient_b") == 0)
    {
    ambient++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->ambient[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************
   * red,green,blue diffuse (coupled,    *
   * one type required, others optional) *
   ***************************************/

  else if (strcmp (attributes[i], "diffuse_r") == 0)
    {
    diffuse++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->diffuse[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "diffuse_g") == 0)
    {
    diffuse++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->diffuse[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "diffuse_b") == 0)
    {
    diffuse++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->diffuse[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************
   * red,green,blue specular (coupled,   *
   * one type required, others optional) *
   ***************************************/

  else if (strcmp (attributes[i], "specular_r") == 0)
    {
    specular++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->specular[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "specular_g") == 0)
    {
    specular++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->specular[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "specular_b") == 0)
    {
    specular++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->specular[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

   /*****************************************
   * geometry: direction, position, angle   *
   * (coupled, required, according to type) *
   ******************************************/

  else if (strcmp (attributes[i], "direction_x") == 0)
    {
    direction++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->direction[0], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "direction_y") == 0)
    {
    direction++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->direction[1], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "direction_z") == 0)
    {
    direction++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->direction[2], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "position_x") == 0)
    {
    position++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->position[0], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "position_y") == 0)
    {
    position++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->position[1], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "position_z") == 0)
    {
    position++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->position[2], -FLT_MAX, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "angle") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->angle, 0.0, 90.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************************************
   * attenuation: constant, linear, quadratic, radial (optional) *
   ***************************************************************/

  else if (strcmp (attributes[i], "constant") == 0)
    {
    constant++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->constant, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "linear") == 0)
    {
    linear++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->linear, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "quadratic") == 0)
    {
    quadratic++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->quadratic, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "radial") == 0)
    {
    radial++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &light->radial, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/******************************
 * global: missing attributes *
 ******************************/

if (light->ambient[0] + light->ambient[1] + light->ambient[2] == 0
&& light->diffuse[0] + light->diffuse[1] + light->diffuse[2] == 0
&& light->specular[0] + light->specular[1] + light->specular[2] == 0)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

if (ambient == 1 || diffuse == 1 || specular == 1 ||
ambient == 2 || diffuse == 2 || specular == 2)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

if (position == 0 && direction == 0)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

if (position == 1 || direction == 1 ||
position == 2 || direction == 2)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (position == 3) light->position[3] = 1.0;
else
  {
  /***************************************************
   * OpenGL saves geometric data for directional and *
   * positional lights in the same array (position), *
   * using only a second array (direction) for spot  *
   * lights. For directional lights, the direction   *
   * data, collected in the direction array, must    *
   * therefore be transfered to the position array   *
   * and the direction array must be re-initialised. *
   ***************************************************/

  /************************************************************
   * invert the vector direction, so it has the same physical *
   * meaning in directional and positional lights, always     *
   * pointing from the light source to the light destination  *
   ************************************************************/

  light->position[0] = -light->direction[0];
  light->position[1] = -light->direction[1];
  light->position[2] = -light->direction[2];

  light->position[3] = 0.0;

  light->direction[0] = GAMGI_MESA_LIGHT_DIRECTION_X;
  light->direction[1] = GAMGI_MESA_LIGHT_DIRECTION_Y;
  light->direction[2] = GAMGI_MESA_LIGHT_DIRECTION_Z;
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (position == 0 &&
(light->angle != 180.0 || constant > 0
|| linear > 0 || quadratic || radial > 0))
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (position == 3 && light->angle == 180.0 &&
(direction > 0 || radial > 0))
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (light->direction[0] == 0.0 && 
light->direction[1] == 0.0 && light->direction[2] == 0.0)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

 /***************************************************
  * each layer cannot have more than gamgi->lights  *
  * (at least 8, as defined by OpenGL).             *
  *                                                 *
  * layer->light is automatically updated everytime *
  * layer is rendered, so even if an error is found *
  * there is no danger of corrupting the layer data *
  ***************************************************/

if (gamgi->max_lights == (GAMGI_CAST_LAYER light->object.object)->light++)
  return gamgi_io_error_resources (gml->ml.filename, 
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_assembly_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_assembly_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_assembly *assembly;
gamgi_assembly_class *assembly_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_ASSEMBLY, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/****************************************************
 * assembly_class contains the current config info, *
 * which is gamgi->assembly unless new config info  *
 * has been previously stored in object->object.    *
 ****************************************************/

object = GAMGI_CAST_OBJECT gamgi->assembly;
if (object->object == NULL) assembly_class = gamgi->assembly;
else assembly_class = (gamgi_assembly_class *) object->object;

/*****************
 * object create *
 *****************/

assembly = gamgi_engine_create_assembly ();
gamgi_engine_start_assembly (assembly);
gamgi_mesa_start_assembly (assembly, assembly_class);

/**************************************************************
 * by default, each new assembly belongs to the current layer *
 **************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
assembly->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = assembly;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = assembly;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /********
   * name *
   ********/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    assembly->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &assembly->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

return gml->ml.valid;
}

static gamgi_bool static_graph_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_graph_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_graph *graph;
gamgi_graph_class *graph_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_GRAPH, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * graph_class contains the current config info, *
 * which is gamgi->graph unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->graph;
if (object->object == NULL) graph_class = gamgi->graph;
else graph_class = (gamgi_graph_class *) object->object;

/*****************
 * object create *
 *****************/

graph = gamgi_engine_create_graph ();
gamgi_engine_start_graph (graph);
gamgi_mesa_start_graph (graph, graph_class);

/***********************************************************
 * by default, each new graph belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
graph->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = graph;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = graph;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    graph->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &graph->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

return gml->ml.valid;
}

static gamgi_bool static_shape_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_shape_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_shape *shape;
gamgi_shape_class *shape_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_SHAPE, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * shape_class contains the current config info, *
 * which is gamgi->shape unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->shape;
if (object->object == NULL) shape_class = gamgi->shape;
else shape_class = (gamgi_shape_class *) object->object;

/*****************
 * object create *
 *****************/

shape = gamgi_engine_create_shape ();
gamgi_engine_start_shape (shape);
gamgi_mesa_start_shape (shape, shape_class);

/***********************************************************
 * by default, each new shape belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
shape->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = shape;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = shape;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    shape->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &shape->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

return gml->ml.valid;
}

static gamgi_bool static_arrow_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_arrow_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_arrow *arrow;
gamgi_arrow_class *arrow_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_ARROW, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * arrow_class contains the current config info, *
 * which is gamgi->arrow unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->arrow;
if (object->object == NULL) arrow_class = gamgi->arrow;
else arrow_class = (gamgi_arrow_class *) object->object;

/*****************
 * object create *
 *****************/

arrow = gamgi_engine_create_arrow ();
gamgi_engine_start_arrow (arrow);
gamgi_mesa_start_arrow (arrow, arrow_class);

/***********************************************************
 * by default, each new arrow belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
arrow->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = arrow;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = arrow;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    arrow->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &arrow->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

return gml->ml.valid;
}

static gamgi_bool static_cell_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_cell_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_cell *cell;
gamgi_cell_class *cell_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
char system[GAMGI_ENGINE_TOKEN] = "";
char lattice[GAMGI_ENGINE_TOKEN] = "";
double origin[3], euler[3];
int o1 = 0, o2 = 0, o3 = 0;
int o123 = 0;
gamgi_enum o4 = FALSE;
int fileline, i;
int number = 0, volume = 0;
int position = 0, angle = 0, color = 0;
int nodes = 0, faces = 0, borders = 0;
int origin_vectors = 0, axes_vectors = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_CELL, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * cell_class contains the current config info,  *
 * which is gamgi->cell unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->cell;
if (object->object == NULL) cell_class = gamgi->cell;
else cell_class = (gamgi_cell_class *) object->object;

/*****************
 * object create *
 *****************/

cell = gamgi_engine_create_cell ();
gamgi_engine_start_cell (cell);
gamgi_mesa_start_cell (cell, cell_class);

/**********************************************************
 * by default, each new cell belongs to the current layer *
 **********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
cell->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = cell;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = cell;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    cell->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * Symmetry section *
   ********************/

  /*********************
   * system (optional) *
   *********************/

  else if (strcmp (attributes[i], "system") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], system,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************
   * lattice (optional) *
   **********************/

  else if (strcmp (attributes[i], "lattice") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], lattice,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * group (optional) *
   ********************/

  else if (strcmp (attributes[i], "group") == 0)
    {
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell->group, 0, 230) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************************
   * a,b,c,ab,ac,bc (required, depending on lattice) *
   ***************************************************/

  else if (strcmp (attributes[i], "a") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->a, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "b") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->b, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "c") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->c, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ab") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->ab, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ac") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->ac, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "bc") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->bc, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "reciprocal") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->reciprocal, GAMGI_MATH_TOLERANCE_LENGTH,
    DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /******************
   * Volume section *
   ******************/

  /*********
   * model *
   *********/

  else if (strcmp (attributes[i], "model") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "conventional") == 0)
      cell->model = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      cell->model = GAMGI_PHYS_PRIMITIVE;
    else if (strcmp (token, "wigner") == 0)
      cell->model = GAMGI_PHYS_WIGNER;
    else if (strcmp (token, "parallelepiped") == 0)
      cell->model = GAMGI_PHYS_PARALLELEPIPED;
    else if (strcmp (token, "sphere") == 0)
      cell->model = GAMGI_PHYS_SPHERE;
    else if (strcmp (token, "projection") == 0)
      cell->model = GAMGI_PHYS_PROJECTION;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /********************************
   * n1,n2,n3 (optional, coupled) *
   ********************************/

  else if (strcmp (attributes[i], "n1") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell->n1, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "n2") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell->n2, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "n3") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell->n3, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************************************
   * volume: v1,v2,v3,v12,13,v23 (required, depending of volume) *
   ***************************************************************/

  else if (strcmp (attributes[i], "v1") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v1, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v2") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v2, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v3") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v3, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v12") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v12, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v13") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v13, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v23") == 0)
    {
    volume++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->v23, GAMGI_MATH_TOLERANCE_ANGLE, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*********************
   * Positions section *
   *********************/

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /******************
   * Origin section *
   ******************/

  /***************************************
   * o1,o2,o3 offset (optional, coupled) *
   ***************************************/

  else if (strcmp (attributes[i], "o1") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &o1, -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "o2") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &o2, -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "o3") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &o3, -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /************************
   * o4 offset (optional) *
   ************************/

  else if (strcmp (attributes[i], "o4") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "000") == 0)
      o4 = FALSE;
    else if (strcmp (token, "110") == 0)
      o4 = GAMGI_PHYS_110;
    else if (strcmp (token, "101") == 0)
      o4 = GAMGI_PHYS_101;
    else if (strcmp (token, "011") == 0)
      o4 = GAMGI_PHYS_011;
    else if (strcmp (token, "111") == 0)
      o4 = GAMGI_PHYS_111;
    else if (strcmp (token, "211") == 0)
      o4 = GAMGI_PHYS_211;
    else if (strcmp (token, "122") == 0)
      o4 = GAMGI_PHYS_122;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * origin vectors (optional) *
   *****************************/

  else if (strcmp (attributes[i], "origin_vectors") == 0)
    {
    origin_vectors++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "conventional") == 0)
      cell->origin_vectors = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      cell->origin_vectors = GAMGI_PHYS_PRIMITIVE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * axes (optional) *
   *******************/

  else if (strcmp (attributes[i], "axes") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &cell->axes) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************
   * axes vectors (optional) *
   ***************************/

  else if (strcmp (attributes[i], "axes_vectors") == 0)
    {
    axes_vectors++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "conventional") == 0)
      cell->axes_vectors = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      cell->axes_vectors = GAMGI_PHYS_PRIMITIVE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /****************
   * View section *
   ****************/

  /**********************
   * borders (optional) *
   **********************/

  else if (strcmp (attributes[i], "borders") == 0)
    {
    borders++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "all") == 0)
      cell->borders = GAMGI_PHYS_ALL;
    else if (strcmp (token, "faces") == 0)
      cell->borders = GAMGI_PHYS_FACES;
    else if (strcmp (token, "edges") == 0)
      cell->borders = GAMGI_PHYS_EDGES;
    else if (strcmp (token, "none") == 0)
      cell->borders = GAMGI_PHYS_NONE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************
   * nodes, faces (optional) *
   ***************************/

  else if (strcmp (attributes[i], "nodes") == 0)
    {
    nodes++;
    if (gamgi_io_token_bool_scan (attributes[i + 1], &cell->nodes) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "faces") == 0)
    {
    faces++;
    if (gamgi_io_token_bool_scan (attributes[i + 1], &cell->faces) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cell->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/******************************
 * global: missing attributes *
 ******************************/

if (cell->model == GAMGI_PHYS_PARALLELEPIPED && volume != 6)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (cell->model == GAMGI_PHYS_SPHERE && cell->v1 == 0.0)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (gamgi_gtk_cell_symmetry_import (system,
lattice, &cell->group, &cell->lattice) == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (gamgi_gtk_cell_symmetry_parameters (cell, cell->lattice, 
&cell->a, &cell->b, &cell->c, &cell->ab, &cell->ac, &cell->bc) == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (number == 1 || number == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (o123 == 1 || o123 == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (o4 != FALSE && cell->origin_vectors == GAMGI_PHYS_PRIMITIVE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (o4 != FALSE && gamgi_math_node_check (cell->lattice, o4) == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);
  
/****************************************
 * Wigner-Seitz cells are not available *
 * yet for these more complex lattices  *
 ****************************************/

if (cell->model == GAMGI_PHYS_WIGNER)
  {
  if (cell->lattice == GAMGI_PHYS_TRICLINIC_P ||
  cell->lattice == GAMGI_PHYS_MONOCLINIC_P ||
  cell->lattice == GAMGI_PHYS_MONOCLINIC_C)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

if (cell->model == GAMGI_PHYS_PARALLELEPIPED)
  {
  /*********************************************************
   * the 3 angles together cannot be too close to 0 or 360 *
   *                                                       *
   * each angle cannot be larger than the sum of the other *
   * two, or smaller than the difference of the other two  *
   *********************************************************/

  if (cell->v12 + cell->v13 + cell->v23 > 360.0 - GAMGI_MATH_TOLERANCE_ANGLE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (cell->v12 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v13 + cell->v23
  || cell->v13 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v12 + cell->v23
  || cell->v23 + GAMGI_MATH_TOLERANCE_ANGLE > cell->v12 + cell->v13)
    return gamgi_io_error_incompatible (gml->ml.filename, 
    fileline, gml->ml.window);

  if (cell->v12 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v13 - cell->v23)
  || cell->v13 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v12 - cell->v23)
  || cell->v23 - GAMGI_MATH_TOLERANCE_ANGLE < abs (cell->v12 - cell->v13))
    return gamgi_io_error_incompatible (gml->ml.filename, 
    fileline, gml->ml.window);
  }

if (cell->model != GAMGI_PHYS_PARALLELEPIPED && 
cell->model != GAMGI_PHYS_SPHERE && volume != 0)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (cell->model == GAMGI_PHYS_SPHERE && volume != 1)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (cell->model == GAMGI_PHYS_PARALLELEPIPED
|| cell->model == GAMGI_PHYS_SPHERE)
  {
  if (number != 0)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (cell->borders == GAMGI_PHYS_ALL 
  || cell->borders == GAMGI_PHYS_FACES)
    return gamgi_io_error_incompatible (gml->ml.filename, 
    fileline, gml->ml.window);

  if (cell->faces == TRUE)
    return gamgi_io_error_incompatible (gml->ml.filename, 
    fileline, gml->ml.window);
  }

if (cell->model == GAMGI_PHYS_PROJECTION)
  {
  if (number != 0)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (cell->reciprocal != 0.0)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (cell->axes == TRUE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (o123 != 0 || o4 != FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (borders == 1 && cell->borders != GAMGI_PHYS_EDGES)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  cell->borders = GAMGI_PHYS_EDGES;

  if (cell->faces == TRUE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (nodes == 1 && cell->nodes != FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  cell->nodes = FALSE;
  }

if (cell->faces == TRUE && cell->borders == GAMGI_PHYS_ALL)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

/*****************************************
 * Something in the cell must be visible *
 *****************************************/

if (faces + nodes + borders == 3 && cell->faces == FALSE &&
cell->nodes == FALSE && cell->borders == GAMGI_PHYS_NONE)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (color == 1 || color == 2)
   return gamgi_io_error_incompatible (gml->ml.filename, 
   fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (position == 0) gamgi_math_vector_zero (origin);
if (angle == 0) gamgi_math_vector_zero (euler);

if (origin_vectors == 0)
  {
  if (cell->model == GAMGI_PHYS_CONVENTIONAL ||
  cell->model == GAMGI_PHYS_PARALLELEPIPED ||
  cell->model == GAMGI_PHYS_SPHERE ||
  cell->model == GAMGI_PHYS_PROJECTION || o4 != FALSE)
    cell->origin_vectors = GAMGI_PHYS_CONVENTIONAL;

  else cell->origin_vectors = GAMGI_PHYS_PRIMITIVE;
  }

if (axes_vectors == 0)
  {
  if (cell->model == GAMGI_PHYS_CONVENTIONAL ||
  cell->model == GAMGI_PHYS_PARALLELEPIPED ||
  cell->model == GAMGI_PHYS_SPHERE ||
  cell->model == GAMGI_PHYS_PROJECTION)
    cell->axes_vectors = GAMGI_PHYS_CONVENTIONAL;

  else cell->axes_vectors = GAMGI_PHYS_PRIMITIVE;
  }

if (cell->model == GAMGI_PHYS_CONVENTIONAL ||
cell->model == GAMGI_PHYS_PRIMITIVE ||
cell->model == GAMGI_PHYS_WIGNER)
  {
  if (borders == 0 && cell->faces == TRUE && cell->borders == GAMGI_PHYS_ALL)
    cell->borders = GAMGI_PHYS_FACES;
  }

if (cell->model == GAMGI_PHYS_PARALLELEPIPED ||
cell->model == GAMGI_PHYS_SPHERE)
  {
  if (faces == 0) cell->faces = FALSE;
  if (borders == 0 && cell->borders != GAMGI_PHYS_NONE)
    cell->borders = GAMGI_PHYS_EDGES;
  }

if (cell->model == GAMGI_PHYS_PROJECTION)
  {
  if (nodes == 0) cell->nodes = FALSE;
  if (faces == 0) cell->faces = FALSE;
  if (borders == 0) cell->borders = GAMGI_PHYS_EDGES;
  }

/**************
 * build cell *
 **************/

if (cell->origin_vectors == GAMGI_PHYS_PRIMITIVE)
  { cell->o1 = o1; cell->o2 = o2; cell->o3 = o3; }
if (cell->origin_vectors == GAMGI_PHYS_CONVENTIONAL)
  gamgi_math_node_primitive (cell->lattice, 
  o1, o2, o3, o4, &cell->o1, &cell->o2, &cell->o3);

gamgi_phys_cell_create (cell);
gamgi_math_position_cell_rotation_set (cell, euler);
gamgi_math_position_cell_translation_set (cell, origin);

return gml->ml.valid;
}

static gamgi_bool static_cluster_end (const char *element, gamgi_gml *gml)
{
gamgi_cluster *cluster;
int fileline;

cluster = GAMGI_CAST_CLUSTER gml->ml.parent->data;
if (cluster->reference == GAMGI_CHEM_POLYTOPE)
  {
  /******************************************
   * used only in error messages: file line *
   * indicates where current element starts *
   ******************************************/

  fileline = XML_GetCurrentLineNumber (gml->ml.parser);

  /****************************************
   * check arrays built from cdata blocks *
   ****************************************/

  if (gamgi_math_polygon_check_loops (cluster->loops,
  cluster->n_loops, cluster->points, cluster->n_points) == FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (gamgi_math_polygon_check_colors (cluster->loops,
  cluster->n_loops, cluster->colors, cluster->n_colors,
  cluster->paints, cluster->n_paints) == FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_cluster_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_cluster *cluster;
gamgi_cluster_class *cluster_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
double rotation[9];
double euler[3];
int position = 0, center = 0, angle = 0;
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_CLUSTER, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/***************************************************
 * cluster_class contains the current config info, *
 * which is gamgi->cluster unless new config info  *
 * has been previously stored in object->object.   *
 ***************************************************/

object = GAMGI_CAST_OBJECT gamgi->cluster;
if (object->object == NULL) cluster_class = gamgi->cluster;
else cluster_class = (gamgi_cluster_class *) object->object;

/*****************
 * object create *
 *****************/

cluster = gamgi_engine_create_cluster ();
gamgi_engine_start_cluster (cluster);
gamgi_mesa_start_cluster (cluster, cluster_class);

/*************************************************************
 * by default, each new cluster belongs to the current layer *
 *************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
cluster->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = cluster;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = cluster;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    cluster->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /************************
   * reference (optional) *
   ************************/

  else if (strcmp (attributes[i], "reference") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "container") == 0)
       cluster->reference = GAMGI_CHEM_CONTAINER;
    else if (strcmp (token, "polytope") == 0)
       cluster->reference = GAMGI_CHEM_POLYTOPE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * center: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->center[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->center[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->center[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************
   * atoms, faces (optional) *
   ***************************/

  else if (strcmp (attributes[i], "atoms") == 0)
    {
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &cluster->atoms, 0, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "faces") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1], &cluster->faces) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &cluster->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

/*****************************************
 * coupled parameters: enter all or none *
 *****************************************/

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (center == 1 || center == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (angle == 0)
  { euler[0] = 0.0; euler[1] = 0.0; euler[2] = 0.0; }

/*****************************
 * set quaternion from euler *
 *****************************/

gamgi_math_euler_to_matrix (euler, rotation);
gamgi_math_quaternion_from_matrix (rotation, cluster->quaternion);

return gml->ml.valid;
}

static gamgi_bool static_molecule_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_molecule_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_molecule *molecule;
gamgi_molecule_class *molecule_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_MOLECULE, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/****************************************************
 * molecule_class contains the current config info, *
 * which is gamgi->molecule unless new config info  *
 * has been previously stored in object->object.    *
 ****************************************************/

object = GAMGI_CAST_OBJECT gamgi->molecule;
if (object->object == NULL) molecule_class = gamgi->molecule;
else molecule_class = (gamgi_molecule_class *) object->object;

/*****************
 * object create *
 *****************/

molecule = gamgi_engine_create_molecule ();
gamgi_engine_start_molecule (molecule);
gamgi_mesa_start_molecule (molecule, molecule_class);

/**************************************************************
 * by default, each new molecule belongs to the current layer *
 **************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
molecule->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = molecule;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = molecule;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    molecule->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &molecule->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

return gml->ml.valid;
}

static gamgi_bool static_group_end (const char *element, gamgi_gml *gml)
{
gamgi_group *group;
int fileline;

group = GAMGI_CAST_GROUP gml->ml.parent->data;
if (group->reference == GAMGI_CHEM_POLYTOPE)
  {
  /******************************************
   * used only in error messages: file line *
   * indicates where current element starts *
   ******************************************/

  fileline = XML_GetCurrentLineNumber (gml->ml.parser);

  /****************************************
   * check arrays built from cdata blocks *
   ****************************************/

  if (gamgi_math_polygon_check_loops (group->loops,
  group->n_loops, group->points, group->n_points) == FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_group_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_group *group;
gamgi_group_class *group_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
double rotation[9];
double euler[3];
int position = 0, center = 0, angle = 0, color = 0;
int fileline, i;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_GROUP, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * group_class contains the current config info, *
 * which is gamgi->group unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->group;
if (object->object == NULL) group_class = gamgi->group;
else group_class = (gamgi_group_class *) object->object;

/*****************
 * object create *
 *****************/

group = gamgi_engine_create_group ();
gamgi_engine_start_group (group);
gamgi_mesa_start_group (group, group_class);

/***********************************************************
 * by default, each new group belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
group->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = group;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = group;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    group->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /************************
   * reference (optional) *
   ************************/

  else if (strcmp (attributes[i], "reference") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "container") == 0)
       group->reference = GAMGI_CHEM_CONTAINER;
    else if (strcmp (token, "polytope") == 0)
       group->reference = GAMGI_CHEM_POLYTOPE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * center: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->center[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->center[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->center[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * style (optional) *
   ********************/

  else if (strcmp (attributes[i], "faces") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1], &group->faces) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &group->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &group->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &group->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &group->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

/*****************************************
 * coupled parameters: enter all or none *
 *****************************************/

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (center == 1 || center == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (angle == 0)
  { euler[0] = 0.0; euler[1] = 0.0; euler[2] = 0.0; }

/*****************************
 * set quaternion from euler *
 *****************************/

gamgi_math_euler_to_matrix (euler, rotation);
gamgi_math_quaternion_from_matrix (rotation, group->quaternion);

return gml->ml.valid;
}

static gamgi_bool static_plane_end (const char *element, gamgi_gml *gml)
{
gamgi_plane *plane;
int fileline;

plane = GAMGI_CAST_PLANE gml->ml.parent->data;
if (plane->model == GAMGI_PHYS_POLYGON)
  {
  /******************************************
   * used only in error messages: file line *
   * indicates where current element starts *
   ******************************************/

  fileline = XML_GetCurrentLineNumber (gml->ml.parser);

  /****************************************
   * check arrays built from cdata blocks *
   ****************************************/

  if (plane->loops == NULL || plane->loops[0] > 1)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (gamgi_math_polygon_check_loops (plane->loops,
  plane->n_loops, plane->points, plane->n_points) == FALSE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_plane_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_plane *plane;
gamgi_plane_class *plane_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
double rotation[9], euler[3];
int fileline, i;
int parent = 0;
int hkl = 0, color = 0, order = 0;
int vectors = 0, net = 0;
int position = 0, center = 0, angle = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_PLANE, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * plane_class contains the current config info, *
 * which is gamgi->plane unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->plane;
if (object->object == NULL) plane_class = gamgi->plane;
else plane_class = (gamgi_plane_class *) object->object;

/*****************
 * object create *
 *****************/

plane = gamgi_engine_create_plane ();
gamgi_engine_start_plane (plane);
gamgi_mesa_start_plane (plane, plane_class);

/***********************************************************
 * by default, each new plane belongs to the current layer *
 ***********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
plane->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = plane;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = plane;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    plane->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

     hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /************************
   * reference (optional) *
   ***********************/

  else if (strcmp (attributes[i], "reference") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "cell") == 0)
      plane->reference = GAMGI_ENGINE_CELL;
    else if (strcmp (token, "atoms") == 0)
      plane->reference = GAMGI_ENGINE_ATOM;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * hkl (required for crystallographic planes) *
   **********************************************/

  else if (strcmp (attributes[i], "h") == 0)
    {
    hkl++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &plane->hkl[0], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "k") == 0)
    {
    hkl++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &plane->hkl[1], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "l") == 0)
    {
    hkl++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &plane->hkl[2], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************
   * vectors (optional) *
   **********************/

  else if (strcmp (attributes[i], "vectors") == 0)
    {
    vectors++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "conventional") == 0)
      plane->vectors = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      plane->vectors = GAMGI_PHYS_PRIMITIVE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * model (optional) *
   ********************/

  else if (strcmp (attributes[i], "model") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "polygon") == 0)
      plane->model = GAMGI_PHYS_POLYGON;
    else if (strcmp (token, "trace") == 0)
      plane->model = GAMGI_PHYS_TRACE;
    else if (strcmp (token, "pole") == 0)
      plane->model = GAMGI_PHYS_POLE;
    else if (strcmp (token, "vector") == 0)
      plane->model = GAMGI_PHYS_VECTOR;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * projection net (optional) *
   *****************************/

   else if (strcmp (attributes[i], "net") == 0)
    {
    net++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wulff") == 0)
      plane->net = gamgi_phys_projection_wulff;
    else if (strcmp (token, "schmidt") == 0)
      plane->net = gamgi_phys_projection_schmidt;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * order (optional) *
   ********************/

  else if (strcmp (attributes[i], "order") == 0)
    {
    order++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &plane->order, -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * center: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->center[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->center[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->center[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &plane->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

if (plane->reference == GAMGI_ENGINE_CELL)
  {
  if (hkl != 3)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (gamgi_phys_direction_zero (plane->hkl) == TRUE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

if (plane->reference == GAMGI_ENGINE_ATOM)
  {
  if (hkl != 0 || vectors != 0 || net != 0 || order != 0)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (plane->model == GAMGI_PHYS_POLE || plane->model == GAMGI_PHYS_TRACE
  || plane->model == GAMGI_PHYS_VECTOR)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (plane->net != NULL)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (plane->model == FALSE) plane->model = GAMGI_PHYS_POLYGON;
  }

if (parent == 0 && gamgi_expat_import_plane (plane,
object, gml->ml.filename, fileline, gml) == FALSE) return FALSE;

/*****************************
 * check position parameters *
 *****************************/

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (center == 1 || center == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/********************************************************
 * by default origin is 0,0,0 and quaternion is 0,0,0,1 *
 * so it is not needed to initialize both before using  *
 * them to temporarily store the origin and euler data. *
 ********************************************************/

/******************************
 * global: default attributes *
 ******************************/

if (strcmp (plane->object.name, "") == 0)
  {
  if (plane->reference == GAMGI_ENGINE_CELL)
    sprintf (token, "(%d.%d.%d)", plane->hkl[0], plane->hkl[1], plane->hkl[2]);
  else
    sprintf (token, "Plane");

  strcpy (plane->object.name, token);
  }

/************************************************
 * the plane (vector, pole, trace, polygon) can *
 * be built only when the cell parent is known  *
 ************************************************/

/*********************************************************************
 * rotation/translation data is now set (origin, center, quaternion) *
 *********************************************************************/

gamgi_math_euler_to_matrix (euler, rotation);
gamgi_math_quaternion_from_matrix (rotation, plane->quaternion);

return gml->ml.valid;
}

static gamgi_bool static_direction_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_direction_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_direction *direction;
gamgi_direction_class *direction_class;
gamgi_object *object;
double rotation[9], euler[3];
double points[6];
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int parent = 0;
int xyz = 0;
int uvw = 0, color = 0;
int vectors = 0, net = 0;
int o123 = 0, o4 = 0;
int position = 0, center = 0, angle = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_DIRECTION, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*****************************************************
 * direction_class contains the current config info, *
 * which is gamgi->direction unless new config info  *
 * has been previously stored in object->object.     *
 *****************************************************/

object = GAMGI_CAST_OBJECT gamgi->direction;
if (object->object == NULL) direction_class = gamgi->direction;
else direction_class = (gamgi_direction_class *) object->object;

/*****************
 * object create *
 *****************/

direction = gamgi_engine_create_direction ();
gamgi_engine_start_direction (direction);
gamgi_mesa_start_direction (direction, direction_class);

/***************************************************************
 * by default, each new direction belongs to the current layer *
 ***************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
direction->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = direction;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = direction;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    direction->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    parent++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /************************
   * reference (optional) *
   ***********************/

  else if (strcmp (attributes[i], "reference") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "cell") == 0)
      direction->reference = GAMGI_ENGINE_CELL;
    else if (strcmp (token, "atoms") == 0)
      direction->reference = GAMGI_ENGINE_ATOM;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /******************
   * uvw (optional) *
   ******************/

  else if (strcmp (attributes[i], "u") == 0)
    {
    uvw++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->uvw[0], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "v") == 0)
    {
    uvw++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->uvw[1], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "w") == 0)
    {
    uvw++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->uvw[2], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************
   * vectors (optional) *
   **********************/

  else if (strcmp (attributes[i], "vectors") == 0)
    {
    vectors++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "conventional") == 0)
      direction->vectors = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      direction->vectors = GAMGI_PHYS_PRIMITIVE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * model (optional) *
   ********************/

  else if (strcmp (attributes[i], "model") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "line") == 0)
      direction->model = GAMGI_PHYS_LINE;
    else if (strcmp (token, "trace") == 0)
      direction->model = GAMGI_PHYS_TRACE;
    else if (strcmp (token, "pole") == 0)
      direction->model = GAMGI_PHYS_POLE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * projection net (optional) *
   *****************************/

   else if (strcmp (attributes[i], "net") == 0)
    {
    net++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wulff") == 0)
      direction->net = gamgi_phys_projection_wulff;
    else if (strcmp (token, "schmidt") == 0)
      direction->net = gamgi_phys_projection_schmidt;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************************
   * o1,o2,o3 offset (optional, coupled) *
   ***************************************/

  else if (strcmp (attributes[i], "o1") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->node[0], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "o2") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->node[1], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "o3") == 0)
    {
    o123++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &direction->node[2], -INT_MAX, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /************************
   * o4 offset (optional) *
   ************************/

  else if (strcmp (attributes[i], "o4") == 0)
    {
    o4++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "000") == 0)
      direction->node[3] = FALSE;
    else if (strcmp (token, "011") == 0)
      direction->node[3] = GAMGI_PHYS_011;
    else if (strcmp (token, "101") == 0)
      direction->node[3] = GAMGI_PHYS_101;
    else if (strcmp (token, "110") == 0)
      direction->node[3] = GAMGI_PHYS_110;
    else if (strcmp (token, "111") == 0)
      direction->node[3] = GAMGI_PHYS_111;
    else if (strcmp (token, "122") == 0)
      direction->node[3] = GAMGI_PHYS_122;
    else if (strcmp (token, "211") == 0)
      direction->node[3] = GAMGI_PHYS_211;
   
    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************************************
   * segment: x0,y0,z0,x1,y1,z1 (required, coupled) *
   **************************************************/

  else if (strcmp (attributes[i], "begin_x") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "begin_y") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "begin_z") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "end_x") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[3], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "end_y") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[4], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "end_z") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &points[5], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

   /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * center: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->center[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->center[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->center[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &direction->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

if (direction->reference == GAMGI_ENGINE_CELL)
  {
  if (uvw != 3)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (gamgi_phys_direction_zero (direction->uvw) == TRUE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (o123 == 1 || o123 == 2)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (direction->node[3] != FALSE && direction->vectors == GAMGI_PHYS_PRIMITIVE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

if (direction->reference == GAMGI_ENGINE_ATOM)
  {
  if (uvw != 0 || vectors != 0 || net != 0 || o123 != 0 || o4 != 0)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (direction->model == GAMGI_PHYS_POLE || direction->model == GAMGI_PHYS_TRACE)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (direction->net != NULL)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);

  if (direction->model == FALSE) direction->model = GAMGI_PHYS_LINE;
  }

if (xyz != 0 && xyz != 6)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (xyz == 6 && direction->model == FALSE) direction->model = GAMGI_PHYS_LINE;
if (xyz == 0 && direction->model == FALSE) direction->model = GAMGI_PHYS_POLE;

if (xyz == 6 && direction->model != GAMGI_PHYS_LINE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (xyz == 0 && direction->model == GAMGI_PHYS_LINE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (xyz == 6 && gamgi_phys_direction_length (points, points + 3) == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (parent == 0 && gamgi_expat_import_direction (direction,
object, gml->ml.filename, fileline, gml) == FALSE) return FALSE;

/*****************************
 * check position parameters *
 *****************************/

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (center == 1 || center == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/********************************************************
 * by default origin is 0,0,0 and quaternion is 0,0,0,1 *
 * so it is not needed to initialize both before using  *
 * them to temporarily store the origin and euler data. *
 ********************************************************/

/******************************
 * global: default attributes *
 ******************************/

if (strcmp (direction->object.name, "") == 0)
  {
  if (direction->reference == GAMGI_ENGINE_CELL)
    sprintf (token, "[%d.%d.%d]", 
    direction->uvw[0], direction->uvw[1], direction->uvw[2]);
  else
    sprintf (token, "Direction");

  strcpy (direction->object.name, token);
  }

/************************************************
 * build line directions now (poles, traces can *
 * be built only when the cell parent is known) *
 ************************************************/

if (direction->model == GAMGI_PHYS_LINE)
  gamgi_phys_direction_line (direction, points);

/*********************************************************************
 * rotation/translation data is now set (origin, center, quaternion) *
 *********************************************************************/

gamgi_math_euler_to_matrix (euler, rotation);
gamgi_math_quaternion_from_matrix (rotation, direction->quaternion);

return gml->ml.valid;
}

static gamgi_bool static_atom_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_atom_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_atom *atom;
gamgi_atom_class *atom_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int number = 0, xyz = 0, color = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_ATOM, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * atom_class contains the current config info,  *
 * which is gamgi->atom unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->atom;
if (object->object == NULL) atom_class = gamgi->atom;
else atom_class = (gamgi_atom_class *) object->object;

/*****************
 * object create *
 *****************/

atom = gamgi_engine_create_atom ();
gamgi_engine_start_atom (atom);
gamgi_mesa_start_atom (atom, atom_class);

/**********************************************************
 * by default, each new atom belongs to the current layer *
 **********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
atom->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = atom;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = atom;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    atom->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * id (optional, required if bonded) *
   *************************************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /**********************
   * element (required) *
   **********************/

  else if (strcmp (attributes[i], "element") == 0)
    {
    number++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (gamgi_chem_atom_number (token, &atom->element) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * x,y,z (required) *
   ********************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->position[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->position[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    xyz++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->position[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * label (optional) *
   ********************/

  else if (strcmp (attributes[i], "label") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    atom->object.label, GAMGI_IO_TEXT, GAMGI_CHEM_ATOM_LABEL + 1) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * mass (optional) *
   *******************/

  else if (strcmp (attributes[i], "mass") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->mass, GAMGI_MATH_TOLERANCE_MASS, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*********************
   * radius (optional) *
   *********************/

  else if (strcmp (attributes[i], "radius") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*********************
   * charge (optional) *
   *********************/

  else if (strcmp (attributes[i], "charge") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1], &atom->charge, 
    -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************
   * temperature (optional) *
   **************************/

  else if (strcmp (attributes[i], "temperature") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1], &atom->temperature,
    GAMGI_CHEM_ATOM_TEMPERATURE_MIN, GAMGI_CHEM_ATOM_TEMPERATURE_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /************************
   * occupancy (optional) *
   ************************/

  else if (strcmp (attributes[i], "occupancy") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1], &atom->occupancy,
    0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * style (optional) *
   ********************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wired") == 0)
      atom->draw = gamgi_mesa_atom_draw_cross;
    else if (strcmp (token, "solid") == 0)
      atom->draw = gamgi_mesa_atom_draw_sphere;

    else return gamgi_io_error_value (attributes[i + 1], 
    gml->ml.filename, fileline, gml->ml.window);
    }

  /****************************
   * variancy,size (optional) *
   ****************************/

  else if (strcmp (attributes[i], "variancy") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom->variancy, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "size") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom->size, GAMGI_MATH_TOLERANCE, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/******************************
 * global: missing attributes *
 ******************************/

if (number != 1)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

if (xyz != 3)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

/**********************************************************
 * defaults can be set only after atom->element is known. *
 **********************************************************/

gamgi_chem_atom_name (atom->element, token);

if (strcmp (atom->object.name, "") == 0) 
  strcpy (atom->object.name, token);

if (strcmp (atom->object.label, "") == 0)
  {
  gamgi_io_token_lower (token, token+10);
  strcpy (atom->object.label, token+10);
  }

if (atom->mass == 0.0)
  atom->mass = atom_class->mass[atom->element];

if (atom->radius == 0.0)
  atom->radius = atom_class->radius[atom->element];

if (color == 0)
  {
  atom->red = atom_class->red[atom->element];
  atom->green = atom_class->green[atom->element];
  atom->blue = atom_class->blue[atom->element];
  }

return gml->ml.valid;
}

static gamgi_bool static_bond_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_bond_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_bond *bond;
gamgi_bond_class *bond_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int ref = 0, color = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_BOND, gml) == FALSE) 
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * bond_class contains the current config info,  *
 * which is gamgi->bond unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->bond;
if (object->object == NULL) bond_class = gamgi->bond;
else bond_class = (gamgi_bond_class *) object->object;

/*****************
 * object create *
 *****************/

bond = gamgi_engine_create_bond ();
gamgi_engine_start_bond (bond);
gamgi_mesa_start_bond (bond, bond_class);

/**********************************************************
 * by default, each new bond belongs to the current layer *
 **********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
bond->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = bond;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = bond;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    bond->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*******************************
   * parent1, parent2 (required) *
   *******************************/

  else if (strcmp (attributes[i], "parent1") == 0)
    {
    ref++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  else if (strcmp (attributes[i], "parent2") == 0)
    {
    ref++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /************************
   * covalency (optional) *
   ************************/

  else if (strcmp (attributes[i], "covalency") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->covalency, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * order (optional) *
   ********************/

  else if (strcmp (attributes[i], "order") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->order, 0.0, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * style (optional) *
   ********************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wired") == 0)
      bond->draw = gamgi_mesa_bond_draw_line;
    else if (strcmp (token, "solid") == 0)
      bond->draw = gamgi_mesa_bond_draw_cylinder;

    else return gamgi_io_error_value (attributes[i + 1], 
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * size (optional) *
   *******************/

  else if (strcmp (attributes[i], "size") == 0)
    {
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->size, GAMGI_MATH_TOLERANCE, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***********************************
   * color,red,green,blue (optional) *
   ***********************************/

  else if (strcmp (attributes[i], "color") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1],
    &bond->color) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &bond->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/******************************
 * global: missing attributes *
 ******************************/

if (ref != 2)
  return gamgi_io_error_missing (gml->ml.filename, 
  fileline, gml->ml.window);

/***********************************
 * global: incompatible attributes *
 ***********************************/

i = gml->ref_used;
if (strcmp (gml->ref + GAMGI_ENGINE_TOKEN * (i - 1),
gml->ref + GAMGI_ENGINE_TOKEN * (i - 2)) == 0)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (color == 3 && bond->color == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

/****************************************************
 * default name can be set only after bond parents  *
 * are known, which may happen in the very end only *
 ****************************************************/

return gml->ml.valid;
}

static gamgi_bool static_orbital_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_orbital_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_orbital *orbital;
gamgi_orbital_class *orbital_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
double origin[3], euler[3];
int fileline, i, j;
int number = 0;
int position = 0, angle = 0;
int base = 0, phase = 0, frame = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_ORBITAL, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/***************************************************
 * orbital_class contains the current config info, *
 * which is gamgi->orbital unless new config info  *
 * has been previously stored in object->object.   *
 ***************************************************/

object = GAMGI_CAST_OBJECT gamgi->orbital;
if (object->object == NULL) orbital_class = gamgi->orbital;
else orbital_class = (gamgi_orbital_class *) object->object;

/*****************
 * object create *
 *****************/

orbital = gamgi_engine_create_orbital ();
gamgi_engine_start_orbital (orbital);
gamgi_mesa_start_orbital (orbital, orbital_class);

/*************************************************************
 * by default, each new orbital belongs to the current layer *
 *************************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
orbital->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = orbital;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = orbital;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    orbital->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }

  /**********************
   * n, l, m (required) *
   **********************/

  else if (strcmp (attributes[i], "n") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &orbital->n, 1, 6) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "l") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &orbital->l, 0, 5) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "m") == 0)
    {
    number++;
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &orbital->m, -5, 5) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*********************
   * charge (optional) *
   *********************/

  else if (strcmp (attributes[i], "charge") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &orbital->charge, 1.0, GAMGI_CHEM_ATOM_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * style (optional) *
   ********************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wired") == 0)
      orbital->style = GAMGI_MESA_WIRED;
    else if (strcmp (token, "solid") == 0)
      orbital->style = GAMGI_MESA_SOLID;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * density, radius, sampling, seed (optional) *
   **********************************************/

  else if (strcmp (attributes[i], "density") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &orbital->density, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "radius") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &orbital->radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "sampling") == 0)
    {
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &orbital->sampling, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "seed") == 0)
    {
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &orbital->seed, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }
  
  /******************************************
   * phase, frame, octants, axes (optional) *
   ******************************************/

  else if (strcmp (attributes[i], "phase") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1], &orbital->phase) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "frame") == 0)
    {
    if (gamgi_io_token_bool_scan (attributes[i + 1], &orbital->frame) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "octants") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, "01", 8 + 1) == FALSE || strlen (token) < 8)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    
    for (j = 0; j < 8; j++)
      orbital->octants[j] = (token[j] == '0') ? 0 : 1;
    }

  else if (strcmp (attributes[i], "axes") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "radius") == 0)
      orbital->axes = GAMGI_CHEM_RADIUS;
    else if (strcmp (token, "unit") == 0)
      orbital->axes = GAMGI_CHEM_UNIT;
    else if (strcmp (token, "bohr") == 0)
      orbital->axes = GAMGI_CHEM_BOHR;
    else if (strcmp (token, "none") == 0)
      orbital->axes = GAMGI_CHEM_NONE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************************************
   * red,green,blue base color (coupled, optional) *
   *************************************************/

  else if (strcmp (attributes[i], "base_r") == 0)
    {
    base++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->base_r, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "base_g") == 0)
    {
    base++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->base_g, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "base_b") == 0)
    {
    base++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->base_b, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************************************
   * red,green,blue phase color (coupled, optional) *
   **************************************************/

  else if (strcmp (attributes[i], "phase_r") == 0)
    {
    phase++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->phase_r, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "phase_g") == 0)
    {
    phase++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->phase_g, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "phase_b") == 0)
    {
    phase++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->phase_b, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************************************
   * red,green,blue frame color (coupled, optional) *
   **************************************************/

  else if (strcmp (attributes[i], "frame_r") == 0)
    {
    frame++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->frame_r, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "frame_g") == 0)
    {
    frame++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->frame_g, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "frame_b") == 0)
    {
    frame++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &orbital->frame_b, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &orbital->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

if (number != 3)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

if (gamgi_chem_orbital_number (orbital->n, orbital->l, orbital->m) == FALSE)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (base == 1 || base == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (phase == 1 || phase == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (frame == 1 || frame == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (orbital->style == GAMGI_MESA_SOLID &&
orbital->sampling > GAMGI_CHEM_ORBITAL_SLICES_MAX)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (orbital->radius == 0.0)
  orbital->radius = gamgi_chem_orbital_radius (orbital->n,
  orbital->l, orbital->m, orbital->charge);

if (orbital->sampling == 0)
  orbital->sampling = gamgi_chem_orbital_sampling (orbital->n,
  orbital->style);

if (position == 0)
  { origin[0] = 0.0; origin[1] = 0.0; origin[2] = 0.0; }

if (angle == 0)
  { euler[0] = 0.0; euler[1] = 0.0; euler[2] = 0.0; }

/*****************
 * build orbital *
 *****************/

gamgi_chem_orbital_create (orbital);
gamgi_math_position_orbital_rotation_set (orbital, euler);
gamgi_math_position_orbital_translation_set (orbital, origin);

return gml->ml.valid;
}

static gamgi_bool static_text_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_text_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_text *text;
gamgi_text_class *text_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
char line[GAMGI_ENGINE_LINE];
double origin[3], euler[3];
int fileline, i;
int font = 0, position = 0, angle = 0, color = 0;
unsigned int hash;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/******************************
 * parent list: check, update *
 ******************************/

if (gamgi_expat_import_parent_gml (GAMGI_ENGINE_TEXT, gml) == FALSE)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * text_class contains the current config info,  *
 * which is gamgi->text unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->text;
if (object->object == NULL) text_class = gamgi->text;
else text_class = (gamgi_text_class *) object->object;

/*****************
 * object create *
 *****************/

text = gamgi_engine_create_text ();
gamgi_engine_start_text (text);
gamgi_mesa_start_text (text, text_class);

/**********************************************************
 * by default, each new text belongs to the current layer *
 **********************************************************/

object = GAMGI_CAST_OBJECT gml->ml.parent->data;
if (object == NULL) object = GAMGI_CAST_OBJECT gml->ml.window->layer;
text->object.object = object;

/************************
 * object queue: update *
 ************************/

gml->object_end = gamgi_engine_dlist_add_end (gml->object_end);
gml->object_end->data = text;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = text;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (optional) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    text->object.name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************
   * id (optional) *
   *****************/

  else if (strcmp (attributes[i], "id") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    hash = gamgi_math_hash_value (token,
    GAMGI_EXPAT_HASH_MULTIPLIER, GAMGI_EXPAT_HASH_SIZE);

    if (gamgi_expat_import_id_get (token, gml, hash) != NULL)
      return gamgi_io_error_multiple (gml->ml.filename, 
      fileline, gml->ml.window);

    gamgi_expat_import_id_add (token, gml, hash);
    }

  /*********************
   * parent (optional) *
   *********************/

  else if (strcmp (attributes[i], "parent") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    gamgi_expat_import_ref_add (token, gml);
    }
  
  /***********************
   * contents (required) *
   ***********************/

  else if (strcmp (attributes[i], "contents") == 0)
    {
    if (gamgi_io_token_check (attributes[i + 1]) == FALSE || 
    gamgi_io_token_alpha_check (attributes[i + 1], 
    GAMGI_IO_FONT_ASCII, GAMGI_ENGINE_LINE) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    /*******************************************
     * in case an error occurs, text->string   *
     * is free inside gamgi_engine_remove_text *
     *******************************************/

    gamgi_mesa_text_create (text, attributes[i + 1]);
    }

  /*************************************
   * origin: x,y,z (optional, coupled) *
   *************************************/

  else if (strcmp (attributes[i], "x") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[0], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "y") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[1], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "z") == 0)
    {
    position++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &origin[2], -DBL_MAX, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************************************
   * Euler angles: e1,e2,e3 (optional, coupled) *
   **********************************************/

  else if (strcmp (attributes[i], "e1") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[0], 0.0, 180.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e2") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[1], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "e3") == 0)
    {
    angle++;
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &euler[2], 0.0, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************
   * font style (optional) *
   *************************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    if (strcmp (token, "wired") == 0)
      text->style = GAMGI_MESA_WIRED;
    else if (strcmp (token, "solid") == 0)
      text->style = GAMGI_MESA_SOLID;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }
    
  /************************
   * font name (optional) *
   ************************/

  else if (strcmp (attributes[i], "font") == 0)
    {
    font++;
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    /*********************************************
     * when the same name is used for wired and  *
     * solid fonts, start with the wired font    *
     * and correct it later, if necessary, as    *
     * only in the end the chosen style is known *
     *********************************************/

    if (strcmp (token, "roman") == 0)
      font = GAMGI_IO_SUN_ROMAN;
    else if (strcmp (token, "roman_mono") == 0)
      font = GAMGI_IO_SUN_ROMAN_MONO;
    else if (strcmp (token, "sans") == 0)
      font = GAMGI_IO_HERSHEY_SANS;
    else if (strcmp (token, "sans_bold") == 0)
      font = GAMGI_IO_HERSHEY_SANS_BOLD;
    else if (strcmp (token, "sans_italic") == 0)
      font = GAMGI_IO_DEJAVU_SANS_ITALIC;
    else if (strcmp (token, "sans_bold_italic") == 0)
      font = GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC;
    else if (strcmp (token, "serif") == 0)
      font = GAMGI_IO_HERSHEY_SERIF;
    else if (strcmp (token, "serif_bold") == 0)
      font = GAMGI_IO_HERSHEY_SERIF_BOLD;
    else if (strcmp (token, "serif_italic") == 0)
      font = GAMGI_IO_DEJAVU_SERIF_ITALIC;
    else if (strcmp (token, "serif_bold_italic") == 0)
      font = GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC;
    else if (strcmp (token, "script") == 0)
      font = GAMGI_IO_HERSHEY_SCRIPT;
    else if (strcmp (token, "script_bold") == 0)
      font = GAMGI_IO_HERSHEY_SCRIPT_BOLD;
    else if (strcmp (token, "gothic_english") == 0)
      font = GAMGI_IO_HERSHEY_GOTHIC_ENGLISH;
    else if (strcmp (token, "gothic_german") == 0)
      font = GAMGI_IO_HERSHEY_GOTHIC_GERMAN;
    else if (strcmp (token, "gothic_italian") == 0)
      font = GAMGI_IO_HERSHEY_GOTHIC_ITALIAN;
    else if (strcmp (token, "symbol_sans") == 0)
      font = GAMGI_IO_HERSHEY_SYMBOL_SANS;
    else if (strcmp (token, "symbol_serif") == 0)
      font = GAMGI_IO_HERSHEY_SYMBOL_SERIF;
    else if (strcmp (token, "symbol_serif_bold") == 0)
      font = GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD;
    else if (strcmp (token, "mono") == 0)
      font = GAMGI_IO_DEJAVU_MONO;
    else if (strcmp (token, "mono_bold") == 0)
      font = GAMGI_IO_DEJAVU_MONO_BOLD;
    else if (strcmp (token, "mono_italic") == 0)
      font = GAMGI_IO_DEJAVU_MONO_ITALIC;
    else if (strcmp (token, "mono_bold_italic") == 0)
      font = GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*****************************
   * red,green,blue (optional) *
   *****************************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &text->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &text->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;
    if (gamgi_io_token_float_scan (attributes[i + 1],
    &text->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * scale (optional) *
   ********************/

  else if (strcmp (attributes[i], "scale") == 0)
    {
    if (gamgi_io_token_double_scan (attributes[i + 1],
    &text->object.scale, GAMGI_MESA_SCALE_LOWER,
    GAMGI_MESA_SCALE_UPPER) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

if (text->string == NULL)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

/*****************************************
 * global tests: incompatible attributes *
 *****************************************/

if (text->style == GAMGI_MESA_WIRED)
  {
  if (font == GAMGI_IO_DEJAVU_SANS_ITALIC ||
  font == GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC ||
  font == GAMGI_IO_DEJAVU_SERIF_ITALIC ||
  font == GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC ||
  font == GAMGI_IO_DEJAVU_MONO ||
  font == GAMGI_IO_DEJAVU_MONO_BOLD ||
  font == GAMGI_IO_DEJAVU_MONO_ITALIC ||
  font == GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

if (text->style == GAMGI_MESA_SOLID)
  {
  if (font == GAMGI_IO_SUN_ROMAN ||
  font == GAMGI_IO_SUN_ROMAN_MONO ||
  font == GAMGI_IO_HERSHEY_SCRIPT ||
  font == GAMGI_IO_HERSHEY_SCRIPT_BOLD ||
  font == GAMGI_IO_HERSHEY_GOTHIC_ENGLISH ||
  font == GAMGI_IO_HERSHEY_GOTHIC_GERMAN ||
  font == GAMGI_IO_HERSHEY_GOTHIC_ITALIAN ||
  font == GAMGI_IO_HERSHEY_SYMBOL_SANS ||
  font == GAMGI_IO_HERSHEY_SYMBOL_SERIF ||
  text->font == GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD)
    return gamgi_io_error_incompatible (gml->ml.filename,
    fileline, gml->ml.window);
  }

/*****************************************************
 * when the font name is common to both wired and    *
 * solid styles, the correct font can be set only    *
 * after reading all parameters, when style is known *
 *****************************************************/

if (text->style == GAMGI_MESA_SOLID)
  {
  if (font == GAMGI_IO_HERSHEY_SANS)
    font = GAMGI_IO_DEJAVU_SANS;
  else if (font == GAMGI_IO_HERSHEY_SANS_BOLD)
    font = GAMGI_IO_DEJAVU_SANS_BOLD;
  else if (font == GAMGI_IO_HERSHEY_SERIF)
    font = GAMGI_IO_DEJAVU_SERIF;
  else if (font == GAMGI_IO_HERSHEY_SERIF_BOLD)
    font = GAMGI_IO_DEJAVU_SERIF_BOLD;
  }

/*****************************************
 * coupled parameters: enter all or none *
 *****************************************/

if (position == 1 || position == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (angle == 1 || angle == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * global: default attributes *
 ******************************/

if (position == 0)
  { origin[0] = 0.0; origin[1] = 0.0; origin[2] = 0.0; }

if (angle == 0)
  { euler[0] = 0.0; euler[1] = 0.0; euler[2] = 0.0; }

if (font == 0)
  {
  if (text->style == GAMGI_MESA_WIRED) font = text_class->wired;
  if (text->style == GAMGI_MESA_SOLID) font = text_class->solid;
  }

/*******************
 * assign new font *
 *******************/

text->font = font;
if (gamgi_mesa_text_center (text->font,
text->string, text->length, text->center, line) == FALSE)
  {
  gamgi_mesa_text_data_solid (text->font, token);

  text->font = FALSE;
  return gamgi_io_error_font (token, gml->ml.filename,
  fileline, gml->ml.window);
  }
gamgi_mesa_text_font_increase (text, NULL);

gamgi_math_position_text_rotation_set (text, euler);
gamgi_math_position_text_translation_set (text, origin);

return gml->ml.valid;
}

static gamgi_bool static_data_end (const char *element, gamgi_gml *gml)
{
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

if (gml->size != gml->count)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

gml->type = FALSE;
gml->array = NULL;
gml->size = 0;
gml->count = 0;
gml->used = 0;

return gml->ml.valid;
}

static gamgi_bool static_data_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_ml *ml = GAMGI_CAST_ML gml;
gamgi_object *parent;
gamgi_plane *plane = NULL;
gamgi_group *group = NULL;
gamgi_cluster *cluster = NULL;
gamgi_enum class;
void **array;
char name[GAMGI_ENGINE_TOKEN] = "";
char type[GAMGI_ENGINE_TOKEN] = "";
int size = 0;
int fileline, i;
int *n_array;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/**************
 * get parent *
 **************/

if (gml->type != FALSE)
  return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);

if (ml == NULL || ml->parent == NULL || ml->parent->data == NULL)
  return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);

parent = GAMGI_CAST_OBJECT ml->parent->data;
class = parent->class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /*******************
   * name (required) *
   *******************/

  if (strcmp (attributes[i], "name") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    name, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * type (required) *
   *******************/

  else if (strcmp (attributes[i], "type") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    type, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*******************
   * size (required) *
   *******************/

  else if (strcmp (attributes[i], "size") == 0)
    {
    if (gamgi_io_token_int_scan (attributes[i + 1],
    &size, 1, INT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/************************************
 * global tests: missing attributes *
 ************************************/

if (strcmp (name, "") == 0)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (strcmp (type, "") == 0)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

if (size == 0)
  return gamgi_io_error_missing (gml->ml.filename,
  fileline, gml->ml.window);

/********************************************************
 * global tests: incompatible attributes (points have 3 *
 * coordinates x,y,z and colors have 3 components r,g,b *
 ********************************************************/

if (strcmp (name, "points") == 0 && size % 3 != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);
  
if (strcmp (name, "colors") == 0 && size % 3 != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (strcmp (name, "points") == 0 && strcmp (type, "double") != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (strcmp (name, "colors") == 0 && strcmp (type, "float") != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (strcmp (name, "points_driver") == 0 && strcmp (type, "int") != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (strcmp (name, "colors_driver") == 0 && strcmp (type, "int") != 0)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/*************
 * get array *
 *************/

array = NULL;
n_array = NULL;

if (class == GAMGI_ENGINE_PLANE) plane = GAMGI_CAST_PLANE parent;
if (class == GAMGI_ENGINE_GROUP) group = GAMGI_CAST_GROUP parent;
if (class == GAMGI_ENGINE_CLUSTER) cluster = GAMGI_CAST_CLUSTER parent;

if (strcmp (name, "points") == 0)
  {
  if (class == GAMGI_ENGINE_PLANE)
    { array = (void **) &plane->points; n_array = &plane->n_points; }
  else if (class == GAMGI_ENGINE_GROUP)
    { array = (void **) &group->points; n_array = &group->n_points; }
  else if (class == GAMGI_ENGINE_CLUSTER)
    { array = (void **) &cluster->points; n_array = &cluster->n_points; }
  else return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);
  }
else if (strcmp (name, "points_driver") == 0)
  {
  if (class == GAMGI_ENGINE_PLANE)
    { array = (void **) &plane->loops; n_array = &plane->n_loops; }
  else if (class == GAMGI_ENGINE_GROUP)
    { array = (void **) &group->loops; n_array = &group->n_loops; }
  else if (class == GAMGI_ENGINE_CLUSTER)
    { array = (void **) &cluster->loops; n_array = &cluster->n_loops; }
  else return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);
  }
else if (strcmp (name, "colors") == 0)
  {
  if (class == GAMGI_ENGINE_CLUSTER)
    { array = (void **) &cluster->colors; n_array = &cluster->n_colors; }
  else return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);
  }
else if (strcmp (name, "colors_driver") == 0)
  {
  if (class == GAMGI_ENGINE_CLUSTER)
    { array = (void **) &cluster->paints; n_array = &cluster->n_paints; }
  else return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);
  }
else return gamgi_io_error_value (name,
gml->ml.filename, fileline, gml->ml.window);

/******************
 * allocate array *
 ******************/

if (strcmp (type, "double") == 0)
  {
  *array = (double *) malloc (sizeof (double) * size);
  gml->type = GAMGI_EXPAT_DOUBLE;
  *n_array = size / 3;
  }
else if (strcmp (type, "float") == 0)
  {
  *array = (float *) malloc (sizeof (float) * size);
  gml->type = GAMGI_EXPAT_FLOAT;
  *n_array = size / 3;
  }
else if (strcmp (type, "int") == 0)
  {
  *array = (int *) malloc (sizeof (int) * size);
  gml->type = GAMGI_EXPAT_INT;
  *n_array = size;
  }
else return gamgi_io_error_value (type,
gml->ml.filename, fileline, gml->ml.window);

gml->size = size;
gml->array = *array;

return gml->ml.valid;
}

void gamgi_expat_import_object_end (void *data,
const char *element)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (element, "gml") == 0)
  ml->valid = static_gml_end (element, data);

/********************************************************
 * static_gamgi_end is not needed because </gamgi> is   *
 * always handled by gamgi_expat_gml_config_end, in     *
 * config mode. A </gamgi> token in data mode is        *
 * flagged down as an error by the parser and the       *
 * error function gamgi_expat_gml_parse will handle it. *
 ********************************************************/

else if (strcmp (element, "window") == 0)
  ml->valid = static_window_end (element, data);

else if (strcmp (element, "layer") == 0)
  ml->valid = static_layer_end (element, data);

else if (strcmp (element, "light") == 0)
  ml->valid = static_light_end (element, data);

else if (strcmp (element, "assembly") == 0)
  ml->valid = static_assembly_end (element, data);

else if (strcmp (element, "graph") == 0)
  ml->valid = static_graph_end (element, data);

else if (strcmp (element, "shape") == 0)
  ml->valid = static_shape_end (element, data);

else if (strcmp (element, "arrow") == 0)
  ml->valid = static_arrow_end (element, data);

else if (strcmp (element, "cell") == 0)
  ml->valid = static_cell_end (element, data);

else if (strcmp (element, "cluster") == 0)
  ml->valid = static_cluster_end (element, data);

else if (strcmp (element, "molecule") == 0)
  ml->valid = static_molecule_end (element, data);

else if (strcmp (element, "group") == 0)
  ml->valid = static_group_end (element, data);

else if (strcmp (element, "plane") == 0)
  ml->valid = static_plane_end (element, data);

else if (strcmp (element, "direction") == 0)
  ml->valid = static_direction_end (element, data);

else if (strcmp (element, "atom") == 0)
  ml->valid = static_atom_end (element, data);

else if (strcmp (element, "bond") == 0)
  ml->valid = static_bond_end (element, data);

else if (strcmp (element, "orbital") == 0)
  ml->valid = static_orbital_end (element, data);

else if (strcmp (element, "text") == 0)
  ml->valid = static_text_end (element, data);

else if (strcmp (element, "data") == 0)
  ml->valid = static_data_end (element, data);

else ml->valid = gamgi_io_error_element (element, 
ml->filename, fileline, ml->window);

if (ml->valid == FALSE) XML_SetElementHandler (ml->parser, NULL, NULL);
}

void gamgi_expat_import_object_start (void *data,
const char *element, const char **attributes)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/******************************************
 * used only in error messages: file line *
 * indicates where current element starts *
 ******************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

if (strcmp (element, "gml") == 0)
  ml->valid = static_gml_start (element, attributes, data);

else if (strcmp (element, "gamgi") == 0)
  ml->valid = static_gamgi_start (element, attributes, data);

else if (strcmp (element, "window") == 0)
  ml->valid = static_window_start (element, attributes, data);

else if (strcmp (element, "layer") == 0)
  ml->valid = static_layer_start (element, attributes, data);

else if (strcmp (element, "light") == 0)
  ml->valid = static_light_start (element, attributes, data);

else if (strcmp (element, "assembly") == 0)
  ml->valid = static_assembly_start (element, attributes, data);

else if (strcmp (element, "graph") == 0)
  ml->valid = static_graph_start (element, attributes, data);

else if (strcmp (element, "shape") == 0)
  ml->valid = static_shape_start (element, attributes, data);

else if (strcmp (element, "arrow") == 0)
  ml->valid = static_arrow_start (element, attributes, data);

else if (strcmp (element, "cell") == 0)
  ml->valid = static_cell_start (element, attributes, data);

else if (strcmp (element, "cluster") == 0)
  ml->valid = static_cluster_start (element, attributes, data);

else if (strcmp (element, "molecule") == 0)
  ml->valid = static_molecule_start (element, attributes, data);

else if (strcmp (element, "group") == 0)
  ml->valid = static_group_start (element, attributes, data);

else if (strcmp (element, "plane") == 0)
  ml->valid = static_plane_start (element, attributes, data);

else if (strcmp (element, "direction") == 0)
  ml->valid = static_direction_start (element, attributes, data);

else if (strcmp (element, "atom") == 0)
  ml->valid = static_atom_start (element, attributes, data);

else if (strcmp (element, "bond") == 0)
  ml->valid = static_bond_start (element, attributes, data);

else if (strcmp (element, "orbital") == 0)
  ml->valid = static_orbital_start (element, attributes, data);

else if (strcmp (element, "text") == 0)
  ml->valid = static_text_start (element, attributes, data);

else if (strcmp (element, "data") == 0)
  ml->valid = static_data_start (element, attributes, data);

else ml->valid = gamgi_io_error_element (element, 
ml->filename, fileline, ml->window);

/**********************************
 * disable Expat parser callbacks *
 **********************************/

if (ml->valid == FALSE)
  XML_SetElementHandler (ml->parser, NULL, NULL);
}
