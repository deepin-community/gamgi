/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import_config.c
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
#include "gamgi_phys.h"
#include "gamgi_io.h"
#include "gamgi_expat.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_mesa_atom.h"
#include "gamgi_mesa_bond.h"
#include "gamgi_math_vector.h"
#include "gamgi_math_sort.h"
#include "gamgi_chem_atom.h"
#include "gamgi_chem_bond.h"
#include "gamgi_io_token.h"
#include "gamgi_expat_import.h"
#include "gamgi_expat_import_object.h"
#include "gamgi_io_error.h"
#include "gamgi_global_copy.h"

static gamgi_bool static_gamgi_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

/****************************************************
 * parser is leaving a gamgi (configuration) block: *
 * switch the data handlers to data handlers        *
 ****************************************************/

XML_SetStartElementHandler (gml->ml.parser, gamgi_expat_import_object_start);
XML_SetEndElementHandler (gml->ml.parser, gamgi_expat_import_object_end);

return gml->ml.valid;
}

static gamgi_bool static_window_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_window_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_window_class *window_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/**************************************************
 * window_class contains the current config info, *
 * which is gamgi->window unless new config info  *
 * has been previously stored in object->object.  *
 **************************************************/

object = GAMGI_CAST_OBJECT gamgi->window;
if (object->object == NULL)
  {
  window_class = gamgi_global_copy_window (gamgi->window);
  object->object = GAMGI_CAST_OBJECT window_class;
  }
else window_class = (gamgi_window_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = window_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  if (strcmp (attributes[i], "rotate_max") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->rotate_max = gamgi->window->rotate_max;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->rotate_max, GAMGI_MATH_TOLERANCE_ANGLE, 360.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "rotate_step") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->rotate_step = gamgi->window->rotate_step;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->rotate_step, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "move_max") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->move_max = gamgi->window->move_max;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->move_max, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "move_step") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->move_step = gamgi->window->move_step;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->move_step, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "scale_max") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->scale_max = gamgi->window->scale_max;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->scale_max, 1.0 + GAMGI_MATH_TOLERANCE, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "scale_step") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      window_class->scale_step = gamgi->window->scale_step;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &window_class->scale_step, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (window_class->rotate_step > window_class->rotate_max)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (window_class->move_step > window_class->move_max)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (window_class->scale_step > window_class->scale_max - 1.0)
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
gamgi_layer_class *layer_class;
gamgi_object *object;
int fileline, i;
int eye = 0, center = 0, up = 0, color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * layer_class contains the current config info, *
 * which is gamgi->layer unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->layer;
if (object->object == NULL)
  {
  layer_class = gamgi_global_copy_layer (gamgi->layer);
  object->object = GAMGI_CAST_OBJECT layer_class;
  }
else layer_class = (gamgi_layer_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = layer_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /***********************
   * history parameters: *
   * undo, save levels   *
   ***********************/

  if (strcmp (attributes[i], "undo") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->undo = gamgi->layer->undo;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1], 
    &layer_class->undo, 0, INT_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "save") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->save = gamgi->layer->save;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &layer_class->save, 0, INT_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************
   * projection parameters: *
   * top, near, far planes  *
   **************************/

  else if (strcmp (attributes[i], "perspective") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->perspective = gamgi->layer->perspective;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_bool_scan (attributes[i + 1],
    &layer_class->perspective) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "top") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->top = gamgi->layer->top;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer_class->top, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "near") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->near = gamgi->layer->near;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->near, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "far") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->far = gamgi->layer->far;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->far, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************
   * modelview parameters: *
   * eye_x, eye_y, eye_z   *
   *************************/

  else if (strcmp (attributes[i], "eye_x") == 0)
    {
    eye++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->eye[0] = gamgi->layer->eye[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->eye[0], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "eye_y") == 0)
    {
    eye++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->eye[1] = gamgi->layer->eye[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->eye[1], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "eye_z") == 0)
    {
    eye++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->eye[2] = gamgi->layer->eye[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->eye[2], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************************
   * modelview parameters:        *
   * center_x, center_y, center_z *
   ********************************/

  else if (strcmp (attributes[i], "center_x") == 0)
    {
    center++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->center[0] = gamgi->layer->center[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->center[0], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_y") == 0)
    {
    center++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->center[1] = gamgi->layer->center[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->center[1], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "center_z") == 0)
    {
    center++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->center[2] = gamgi->layer->center[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &layer_class->center[2], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************
   * modelview parameters: *
   * up_x, up_y, up_z      *
   *************************/

  else if (strcmp (attributes[i], "up_x") == 0)
    {
    up++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->up[0] = gamgi->layer->up[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer_class->up[0], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "up_y") == 0)
    {
    up++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->up[1] = gamgi->layer->up[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer_class->up[1], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "up_z") == 0)
    {
    up++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      layer_class->up[2] = gamgi->layer->up[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1], 
    &layer_class->up[2], -DBL_MAX, DBL_MAX) == FALSE) 
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (layer_class->near + GAMGI_MATH_TOLERANCE_LENGTH > layer_class->far)
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

if (gamgi_math_vector_length (layer_class->up) < GAMGI_MATH_TOLERANCE_LENGTH)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_light_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_light_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_light_class *light_class;
gamgi_object *object;
int fileline, i;
int ambient = 0, diffuse = 0, specular = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * light_class contains the current config info, *
 * which is gamgi->light unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->light;
if (object->object == NULL)
  {
  light_class = gamgi_global_copy_light (gamgi->light);
  object->object = GAMGI_CAST_OBJECT light_class;
  }
else light_class = (gamgi_light_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = light_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /***************************
   * get shininess parameter *
   ***************************/

  if (strcmp (attributes[i], "shininess") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->shininess = gamgi->light->shininess;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->shininess, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************
   * get ambient parameters *
   **************************/

  else if (strcmp (attributes[i], "ambient_r") == 0)
    {
    ambient++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->ambient[0] = gamgi->light->ambient[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->ambient[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ambient_g") == 0)
    {
    ambient++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->ambient[1] = gamgi->light->ambient[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->ambient[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "ambient_b") == 0)
    {
    ambient++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->ambient[2] = gamgi->light->ambient[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->ambient[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**************************
   * get diffuse parameters *
   **************************/

  else if (strcmp (attributes[i], "diffuse_r") == 0)
    {
    diffuse++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->diffuse[0] = gamgi->light->diffuse[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->diffuse[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "diffuse_g") == 0)
    {
    diffuse++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->diffuse[1] = gamgi->light->diffuse[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->diffuse[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "diffuse_b") == 0)
    {
    diffuse++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->diffuse[2] = gamgi->light->diffuse[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->diffuse[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /***************************
   * get specular parameters *
   ***************************/

  else if (strcmp (attributes[i], "specular_r") == 0)
    {
    specular++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->specular[0] = gamgi->light->specular[0];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->specular[0], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "specular_g") == 0)
    {
    specular++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->specular[1] = gamgi->light->specular[1];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->specular[1], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "specular_b") == 0)
    {
    specular++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      light_class->specular[2] = gamgi->light->specular[2];

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &light_class->specular[2], 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (ambient == 1 || ambient == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (diffuse == 1 || diffuse == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (specular == 1 || specular == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
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
gamgi_assembly_class *assembly_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/****************************************************
 * assembly_class contains the current config info, *
 * which is gamgi->assembly unless new config info  *
 * has been previously stored in object->object.    *
 ****************************************************/

object = GAMGI_CAST_OBJECT gamgi->assembly;
if (object->object == NULL)
  {
  assembly_class = gamgi_global_copy_assembly (gamgi->assembly);
  object->object = GAMGI_CAST_OBJECT assembly_class;
  }
else assembly_class = (gamgi_assembly_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = assembly_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_graph_class *graph_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * graph_class contains the current config info, *
 * which is gamgi->graph unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->graph;
if (object->object == NULL)
  {
  graph_class = gamgi_global_copy_graph (gamgi->graph);
  object->object = GAMGI_CAST_OBJECT graph_class;
  }
else graph_class = (gamgi_graph_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = graph_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_shape_class *shape_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * shape_class contains the current config info, *
 * which is gamgi->shape unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->shape;
if (object->object == NULL)
  {
  shape_class = gamgi_global_copy_shape (gamgi->shape);
  object->object = GAMGI_CAST_OBJECT shape_class;
  }
else shape_class = (gamgi_shape_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = shape_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_arrow_class *arrow_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * arrow_class contains the current config info, *
 * which is gamgi->arrow unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->arrow;
if (object->object == NULL)
  {
  arrow_class = gamgi_global_copy_arrow (gamgi->arrow);
  object->object = GAMGI_CAST_OBJECT arrow_class;
  }
else arrow_class = (gamgi_arrow_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = arrow_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_cell_class *cell_class;
gamgi_object *object;
int fileline, i;
char token[GAMGI_ENGINE_TOKEN];
int color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * cell_class contains the current config info,  *
 * which is gamgi->cell unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->cell;
if (object->object == NULL)
  {
  cell_class = gamgi_global_copy_cell (gamgi->cell);
  object->object = GAMGI_CAST_OBJECT cell_class;
  }
else cell_class = (gamgi_cell_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = cell_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  if (strcmp (attributes[i], "model") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->model = gamgi->cell->model;
    
    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "conventional") == 0)
      cell_class->model = GAMGI_PHYS_CONVENTIONAL;
    else if (strcmp (token, "primitive") == 0)
      cell_class->model = GAMGI_PHYS_PRIMITIVE;
    else if (strcmp (token, "wigner") == 0)
      cell_class->model = GAMGI_PHYS_WIGNER;
    else if (strcmp (token, "parallelepiped") == 0)
      cell_class->model = GAMGI_PHYS_PARALLELEPIPED;
    else if (strcmp (token, "sphere") == 0)
      cell_class->model = GAMGI_PHYS_SPHERE;
    else if (strcmp (token, "projection") == 0)
      cell_class->model = GAMGI_PHYS_PROJECTION;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "axes") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->axes = gamgi->cell->axes;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "yes") == 0)
      cell_class->axes = TRUE;
    else if (strcmp (token, "no") == 0)
      cell_class->axes = FALSE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "borders") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->borders = gamgi->cell->borders;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "all") == 0)
      cell_class->borders = GAMGI_PHYS_ALL;
    else if (strcmp (token, "faces") == 0)
      cell_class->borders = GAMGI_PHYS_FACES;
    else if (strcmp (token, "edges") == 0)
      cell_class->borders = GAMGI_PHYS_EDGES;
    else if (strcmp (token, "none") == 0)
      cell_class->borders = GAMGI_PHYS_NONE;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  /****************
   * nodes, faces *
   ****************/

  else if (strcmp (attributes[i], "faces") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->faces = gamgi->cell->faces;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_bool_scan (attributes[i + 1], &cell_class->faces) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    }

  else if (strcmp (attributes[i], "nodes") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->nodes = gamgi->cell->nodes;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_bool_scan (attributes[i + 1], &cell_class->nodes) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /******************
   * red,green,blue *
   ******************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->red = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell_class->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->green = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell_class->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->blue = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell_class->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "axes_size") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->axes_size = gamgi->cell->axes_size;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &cell_class->axes_size, GAMGI_MESA_AXES_MIN,
    GAMGI_MESA_AXES_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "borders_width") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->borders_width = gamgi->cell->borders_width;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell_class->borders_width, GAMGI_MESA_CELL_LINE_MIN,
    GAMGI_MESA_CELL_LINE_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "nodes_width") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      cell_class->nodes_width = gamgi->cell->nodes_width;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &cell_class->nodes_width, GAMGI_MESA_CELL_POINT_MIN,
    GAMGI_MESA_CELL_POINT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_cluster_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_cluster_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_cluster_class *cluster_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/***************************************************
 * cluster_class contains the current config info, *
 * which is gamgi->cluster unless new config info  *
 * has been previously stored in object->object.   *
 ***************************************************/

object = GAMGI_CAST_OBJECT gamgi->cluster;
if (object->object == NULL)
  {
  cluster_class = gamgi_global_copy_cluster (gamgi->cluster);
  object->object = GAMGI_CAST_OBJECT cluster_class;
  }
else cluster_class = (gamgi_cluster_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = cluster_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_molecule_class *molecule_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/****************************************************
 * molecule_class contains the current config info, *
 * which is gamgi->molecule unless new config info  *
 * has been previously stored in object->object.    *
 ****************************************************/

object = GAMGI_CAST_OBJECT gamgi->molecule;
if (object->object == NULL)
  {
  molecule_class = gamgi_global_copy_molecule (gamgi->molecule);
  object->object = GAMGI_CAST_OBJECT molecule_class;
  }
else molecule_class = (gamgi_molecule_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = molecule_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

return gml->ml.valid;
}

static gamgi_bool static_group_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_group_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_group_class *group_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * group_class contains the current config info, *
 * which is gamgi->group unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->group;
if (object->object == NULL)
  {
  group_class = gamgi_global_copy_group (gamgi->group);
  object->object = GAMGI_CAST_OBJECT group_class;
  }
else group_class = (gamgi_group_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = group_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

return gml->ml.valid;
}

static gamgi_bool static_plane_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_plane_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_plane_class *plane_class;
gamgi_object *object;
int fileline, i;
int color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * plane_class contains the current config info, *
 * which is gamgi->plane unless new config info  *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->plane;
if (object->object == NULL)
  {
  plane_class = gamgi_global_copy_plane (gamgi->plane);
  object->object = GAMGI_CAST_OBJECT plane_class;
  }
else plane_class = (gamgi_plane_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = plane_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /**************************************
   * color parameters: red, green, blue *
   **************************************/

  if (strcmp (attributes[i], "red") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      plane_class->red = gamgi->text->red;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane_class->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      plane_class->green = gamgi->text->green;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane_class->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      plane_class->blue = gamgi->text->blue;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &plane_class->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

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
gamgi_direction_class *direction_class;
gamgi_object *object;
int fileline, i;
int color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*****************************************************
 * direction_class contains the current config info, *
 * which is gamgi->direction unless new config info  *
 * has been previously stored in object->object.     *
 *****************************************************/

object = GAMGI_CAST_OBJECT gamgi->direction;
if (object->object == NULL)
  {
  direction_class = gamgi_global_copy_direction (gamgi->direction);
  object->object = GAMGI_CAST_OBJECT direction_class;
  }
else direction_class = (gamgi_direction_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = direction_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /**************************************
   * color parameters: red, green, blue *
   **************************************/

  if (strcmp (attributes[i], "red") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      direction_class->red = gamgi->text->red;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction_class->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      direction_class->green = gamgi->text->green;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction_class->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      direction_class->blue = gamgi->text->blue;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &direction_class->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

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
gamgi_atom_class *atom_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
double mass = -1.0, radius = -1.0;
float red = -1.0, green = -1.0, blue = -1.0;
int number = -1, color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * atom_class contains the current config info,  *
 * which is gamgi->atom unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->atom;
if (object->object == NULL)
  {
  atom_class = gamgi_global_copy_atom (gamgi->atom);
  object->object = GAMGI_CAST_OBJECT atom_class;
  }
else atom_class = (gamgi_atom_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = atom_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /***********
   * element *
   ***********/

  if (strcmp (attributes[i], "element") == 0)
    {
    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (gamgi_chem_atom_number (token, &number) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********
   * mass *
   ********/

  else if (strcmp (attributes[i], "mass") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      mass = -2.0;
    
    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &mass, GAMGI_MATH_TOLERANCE_MASS, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********
   * radius *
   **********/

  else if (strcmp (attributes[i], "radius") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      radius = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &radius, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /******************
   * red,green,blue *
   ******************/

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      red = -2.0;
    
    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      green = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      blue = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /************************************
   * analysis parameters: temperature *
   ************************************/

  else if (strcmp (attributes[i], "temperature") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->temperature = gamgi->atom->temperature;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom_class->temperature, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * look parameters: *
   * style, slices    *
   ********************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->draw = gamgi->atom->draw;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1], 
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "wired") == 0)
      atom_class->draw = gamgi_mesa_atom_draw_cross;
    else if (strcmp (token, "solid") == 0) 
      atom_class->draw = gamgi_mesa_atom_draw_sphere;

    else return gamgi_io_error_value (attributes[i + 1], 
    gml->ml.filename, fileline, gml->ml.window);
    }

  /*************************
   * dimension parameters: *
   * variancy, size, min   *
   *************************/

  else if (strcmp (attributes[i], "variancy") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->variancy = gamgi->atom->variancy;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom_class->variancy, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "size") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->size = gamgi->atom->size;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &atom_class->size, GAMGI_MATH_TOLERANCE, FLT_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "min") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->min = gamgi->atom->min;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &atom_class->min, GAMGI_MATH_TOLERANCE_LENGTH, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  /**********************
   * visual parameters: *
   * slices, width      *
   **********************/

  else if (strcmp (attributes[i], "slices") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->slices = gamgi->atom->slices;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &atom_class->slices, GAMGI_MESA_ATOM_SLICES_MIN, 
    GAMGI_MESA_ATOM_SLICES_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "width") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      atom_class->width = gamgi->atom->width;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_int_scan (attributes[i + 1],
    &atom_class->width, GAMGI_MESA_ATOM_WIDTH_MIN,
    GAMGI_MESA_ATOM_WIDTH_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

/******************************
 * properties without element *
 ******************************/

if (number == -1 && (mass != -1.0 || radius != -1.0 || color != 0))
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/******************************
 * element without properties *
 ******************************/

if (number != -1 && (mass == -1.0 && radius == -1.0 && color == 0))
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

/**********************************************************
 * global: set atom configuration (changing width, slices *
 * requires a change in the pre-compiled OpenGL lists,    *
 * which is done in the end, if everything succeds        *
 **********************************************************/

if (mass == -2.0)
  atom_class->mass[number] = gamgi->atom->mass[number];
else if (mass != -1.0)
  atom_class->mass[number] = mass;

if (radius == -2.0)
  atom_class->radius[number] = gamgi->atom->radius[number];
else if (radius != -1.0)
  atom_class->radius[number] = radius;

if (color == 3)
  {
  if (red == -2.0)
    atom_class->red[number] = gamgi->atom->red[number];
  else
    atom_class->red[number] = red;

  if (green == -2.0)
    atom_class->green[number] = gamgi->atom->green[number];
  else
    atom_class->green[number] = green;

  if (blue == -2.0)
    atom_class->blue[number] = gamgi->atom->blue[number];
  else
    atom_class->blue[number] = blue;
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
gamgi_bond_class *bond_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int i, fileline;
int lower_upper = 0, min_max = 0, color = 0;
int number1, number2;
double min = -1.0, max = -1.0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * bond_class contains the current config info,  *
 * which is gamgi->bond unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->bond;
if (object->object == NULL)
  {
  bond_class = gamgi_global_copy_bond (gamgi->bond);
  object->object = GAMGI_CAST_OBJECT bond_class;
  }
else bond_class = (gamgi_bond_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = bond_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /************************************
   * general length parameters:       *
   * lower, upper (optional, coupled) *
   ************************************/

  if (strcmp (attributes[i], "lower") == 0)
    {
    lower_upper++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->lower = gamgi->bond->lower;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &bond_class->lower, 0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "upper") == 0)
    {
    lower_upper++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->upper = gamgi->bond->upper;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &bond_class->upper, 0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /****************************************************
   * specific length parameters:                      *
   * element1, element2, min, max (optional, coupled) *
   ****************************************************/

  else if (strcmp (attributes[i], "element1") == 0)
    {
    min_max++;

    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (gamgi_chem_atom_number (token, &number1) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "element2") == 0)
    {
    min_max++;

    if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    if (gamgi_chem_atom_number (token, &number2) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "min") == 0)
    {
    min_max++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      min = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &min, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "max") == 0)
    {
    min_max++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      max = -2.0;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_double_scan (attributes[i + 1],
    &max, 0.0, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /********************
   * look parameters: *
   * style, size      *
   ********************/

  else if (strcmp (attributes[i], "style") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->draw = gamgi->bond->draw;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1], token,
    GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "wired") == 0)
      bond_class->draw = gamgi_mesa_bond_draw_line;
    else if (strcmp (token, "solid") == 0)
      bond_class->draw = gamgi_mesa_bond_draw_cylinder;

    else return gamgi_io_error_value (attributes[i + 1], 
    gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "size") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->size = gamgi->bond->size;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond_class->size, GAMGI_MATH_TOLERANCE, DBL_MAX) == FALSE)
      return gamgi_io_error_value (attributes[i + 1], 
      gml->ml.filename, fileline, gml->ml.window);
    }

  /************************
   * color parameters:    *
   * color,red,green,blue *
   ************************/

  else if (strcmp (attributes[i], "color") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->color = gamgi->bond->color;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_bool_scan (attributes[i + 1],
    &bond_class->color) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "red") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->red = gamgi->bond->red;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond_class->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->green = gamgi->bond->green;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond_class->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      bond_class->blue = gamgi->bond->blue;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &bond_class->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i], 
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (lower_upper == 1)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (min_max == 1 || min_max == 2 || min_max == 3)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (bond_class->lower > bond_class->upper)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (min > max)
  return gamgi_io_error_incompatible (gml->ml.filename, 
  fileline, gml->ml.window);

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

/**********************************
 * global: set bond configuration *
 **********************************/

if (min_max == 4)
  {
  gamgi_math_sort_int2 (number1, number2, &number1, &number2);
  i = number2 * (number2 + 1)/2 - number1;

  if (min == -2.0)
    bond_class->min[i] = gamgi->bond->min[i];
  else
    bond_class->min[i] = min;

  if (max == -2.0)
    bond_class->max[i] = gamgi->bond->max[i];
  else
    bond_class->max[i] = max;
  }

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
gamgi_orbital_class *orbital_class;
gamgi_object *object;
int fileline, i;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/***************************************************
 * orbital_class contains the current config info, *
 * which is gamgi->orbital unless new config info  *
 * has been previously stored in object->object.   *
 ***************************************************/

object = GAMGI_CAST_OBJECT gamgi->orbital;
if (object->object == NULL)
  {
  orbital_class = gamgi_global_copy_orbital (gamgi->orbital);
  object->object = GAMGI_CAST_OBJECT orbital_class;
  }
else orbital_class = (gamgi_orbital_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = orbital_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

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
gamgi_text_class *text_class;
gamgi_object *object;
char token[GAMGI_ENGINE_TOKEN];
int fileline, i;
int color = 0;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename, 
  fileline, gml->ml.window);

/*************************************************
 * text_class contains the current config info,  *
 * which is gamgi->text unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->text;
if (object->object == NULL)
  {
  text_class = gamgi_global_copy_text (gamgi->text);
  object->object = GAMGI_CAST_OBJECT text_class;
  }
else text_class = (gamgi_text_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = text_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /****************************************
   * font parameters: style, wired, solid *
   ****************************************/

  if (strcmp (attributes[i], "style") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->style = gamgi->text->style;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "wired") == 0)
      text_class->style = GAMGI_MESA_WIRED;
    else if (strcmp (token, "solid") == 0)
      text_class->style = GAMGI_MESA_SOLID;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "wired") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->wired = gamgi->text->wired;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "roman") == 0)
      text_class->wired = GAMGI_IO_SUN_ROMAN;
    else if (strcmp (token, "roman_mono") == 0)
      text_class->wired = GAMGI_IO_SUN_ROMAN_MONO;
    else if (strcmp (token, "sans") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SANS;
    else if (strcmp (token, "sans_bold") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SANS_BOLD;
    else if (strcmp (token, "serif") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SERIF;
    else if (strcmp (token, "serif_bold") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SERIF_BOLD;
    else if (strcmp (token, "script") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SCRIPT;
    else if (strcmp (token, "script_bold") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SCRIPT_BOLD;
    else if (strcmp (token, "gothic_english") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_GOTHIC_ENGLISH;
    else if (strcmp (token, "gothic_german") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_GOTHIC_GERMAN;
    else if (strcmp (token, "gothic_italian") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_GOTHIC_ITALIAN;
    else if (strcmp (token, "symbol_sans") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SYMBOL_SANS;
    else if (strcmp (token, "symbol_serif") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SYMBOL_SERIF;
    else if (strcmp (token, "symbol_serif_bold") == 0)
      text_class->wired = GAMGI_IO_HERSHEY_SYMBOL_SERIF_BOLD;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "solid") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->solid = gamgi->text->solid;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "sans") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SANS;
    else if (strcmp (token, "sans_bold") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SANS_BOLD;
    else if (strcmp (token, "sans_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SANS_ITALIC;
    else if (strcmp (token, "sans_bold_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SANS_BOLD_ITALIC;
    else if (strcmp (token, "serif") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SERIF;
    else if (strcmp (token, "serif_bold") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SERIF_BOLD;
    else if (strcmp (token, "serif_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SERIF_ITALIC;
    else if (strcmp (token, "serif_bold_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_SERIF_BOLD_ITALIC;
    else if (strcmp (token, "mono") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_MONO;
    else if (strcmp (token, "mono_bold") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_MONO_BOLD;
    else if (strcmp (token, "mono_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_MONO_ITALIC;
    else if (strcmp (token, "mono_bold_italic") == 0)
      text_class->solid = GAMGI_IO_DEJAVU_MONO_BOLD_ITALIC;

    else return gamgi_io_error_value (attributes[i + 1],
    gml->ml.filename, fileline, gml->ml.window);
    }

   /**************************************
    * color parameters: red, green, blue *
    **************************************/

   else if (strcmp (attributes[i], "red") == 0)
    {
    color++; 
 
    /*********
     * reset *
     *********/
  
    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->red = gamgi->text->red;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &text_class->red, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

   else if (strcmp (attributes[i], "green") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->green = gamgi->text->green;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &text_class->green, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "blue") == 0)
    {
    color++;

    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      text_class->blue = gamgi->text->blue;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_float_scan (attributes[i + 1],
    &text_class->blue, 0.0, 1.0) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

if (color == 1 || color == 2)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

return gml->ml.valid;
}

static gamgi_bool static_help_end (const char *element, gamgi_gml *gml)
{
gml->ml.parent = gamgi_engine_slist_remove_start (gml->ml.parent);

return gml->ml.valid;
}

static gamgi_bool static_help_start (const char *element,
const char **attributes, gamgi_gml *gml)
{
gamgi_help_class *help_class;
gamgi_object *object;
char token[GAMGI_IO_PATHNAME];
int i, fileline;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (gml->ml.parser);

/***********************
 * parent stack: check *
 ***********************/

if ((GAMGI_CAST_OBJECT gml->ml.parent->data)->class != GAMGI_ENGINE_GAMGI)
  return gamgi_io_error_child (gml->ml.filename,
  fileline, gml->ml.window);

/*************************************************
 * help_class contains the current config info,  *
 * which is gamgi->help unless new config info   *
 * has been previously stored in object->object. *
 *************************************************/

object = GAMGI_CAST_OBJECT gamgi->help;
if (object->object == NULL)
  {
  help_class = gamgi_global_copy_help (gamgi->help);
  object->object = GAMGI_CAST_OBJECT help_class;
  }
else help_class = (gamgi_help_class *) object->object;

/************************
 * parent stack: update *
 ************************/

gml->ml.parent = gamgi_engine_slist_add_start (gml->ml.parent);
gml->ml.parent->data = help_class;

/**********************************
 * attribute array: check, update *
 **********************************/

for (i = 0; attributes[i] != NULL; i += 2)
  {
  /**************************************
   * control parameters: local, browser *
   **************************************/

  if (strcmp (attributes[i], "source") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      help_class->source_in = gamgi->help->source_in;

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "local") == 0)
       help_class->source_in = TRUE;
    else if (strcmp (token, "remote") == 0)
       help_class->source_in = FALSE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "agent") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      help_class->agent_in = gamgi->help->agent_in;
   
    /**********
     * update *
     **********/
    
    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_TEXT, GAMGI_ENGINE_TOKEN) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);

    else if (strcmp (token, "gamgi") == 0)
       help_class->agent_in = TRUE;
    else if (strcmp (token, "browser") == 0)
       help_class->agent_in = FALSE;
    else return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    }

  else if (strcmp (attributes[i], "local") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      {
      gamgi_io_token_remove (&help_class->local);
      gamgi_io_token_create (gamgi->help->local, &help_class->local);
      }

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    else
      {
      gamgi_io_token_remove (&help_class->local);
      gamgi_io_token_create (token, &help_class->local);
      }
    }

  else if (strcmp (attributes[i], "remote") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      {
      gamgi_io_token_remove (&help_class->remote);
      gamgi_io_token_create (gamgi->help->remote, &help_class->remote);
      }

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    else
      {
      gamgi_io_token_remove (&help_class->remote);
      gamgi_io_token_create (token, &help_class->remote);
      }
    }

  else if (strcmp (attributes[i], "browser") == 0)
    {
    /*********
     * reset *
     *********/

    if (gamgi_io_token_check (attributes[i + 1]) == FALSE)
      {
      gamgi_io_token_remove (&help_class->browser);
      gamgi_io_token_create (gamgi->help->browser, &help_class->browser);
      }

    /**********
     * update *
     **********/

    else if (gamgi_io_token_alpha_scan (attributes[i + 1],
    token, GAMGI_IO_FONT_ASCII, GAMGI_IO_PATHNAME) == FALSE)
      return gamgi_io_error_value (attributes[i + 1],
      gml->ml.filename, fileline, gml->ml.window);
    else
      {
      gamgi_io_token_remove (&help_class->browser);
      gamgi_io_token_create (token, &help_class->browser);
      }
    }

  else return gamgi_io_error_attribute (attributes[i],
  gml->ml.filename, fileline, gml->ml.window);
  }

/***********************************
 * global: incompatible attributes *
 ***********************************/

/***************************************************
 * Check if local == NULL when source_in is TRUE   *
 * Check if remote == NULL when source_in is FALSE *
 * Check if browser == NULL when agent_in is FALSE *
 ***************************************************/

if (help_class->source_in == TRUE && help_class->local == NULL)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (help_class->source_in == FALSE && help_class->remote == NULL)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

if (help_class->agent_in == FALSE && help_class->browser == NULL)
  return gamgi_io_error_incompatible (gml->ml.filename,
  fileline, gml->ml.window);

return gml->ml.valid;
}

void gamgi_expat_import_config_end (void *data,
const char *element)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

/********************************************************
 * static_gml_end is not needed because </gml> is       *
 * always handled by gamgi_expat_gml_data_end, in       *
 * data mode. A </gml> token in config mode is          *
 * flagged down as an error by the parser and the       *
 * error function gamgi_expat_gml_parse will handle it. *
 ********************************************************/

if (strcmp (element, "gamgi") == 0)
  ml->valid = static_gamgi_end (element, data);

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

else if (strcmp (element, "help") == 0)
  ml->valid = static_help_end (element, data);

else ml->valid = gamgi_io_error_element (element,
ml->filename, fileline, ml->window);

if (ml->valid == FALSE) XML_SetElementHandler (ml->parser, NULL, NULL);
}

void gamgi_expat_import_config_start (void *data,
const char *element, const char **attributes)
{
gamgi_ml *ml = GAMGI_CAST_ML data;
int fileline;

/**********************************************
 * used only in error messages: file name and *
 * file line say where current element starts *
 **********************************************/

fileline = XML_GetCurrentLineNumber (ml->parser);

/***************************************************
 * in config mode (inside a <gamgi> block) a <gml> *
 * or <gamgi> element is always a wrong child.     *
 ***************************************************/

if (strcmp (element, "gml") == 0)
  ml->valid = gamgi_io_error_child (ml->filename, 
  fileline, ml->window);

else if (strcmp (element, "gamgi") == 0)
  ml->valid = gamgi_io_error_child (ml->filename, 
  fileline, ml->window);
  
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

else if (strcmp (element, "help") == 0)
  ml->valid = static_help_start (element, attributes, data);

else ml->valid = gamgi_io_error_element (element,
ml->filename, fileline, ml->window);

/**********************************
 * disable Expat parser callbacks *
 **********************************/

if (ml->valid == FALSE)
  XML_SetElementHandler (ml->parser, NULL, NULL);
}
