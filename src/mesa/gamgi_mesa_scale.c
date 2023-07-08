/******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_scale.c
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
#include "gamgi_global.h"

#include "gamgi_mesa_draw.h"
#include "gamgi_global_selection.h"

void gamgi_mesa_scale_default (gamgi_object *object, double scale)
{
/*********************************************************
 * this method guarantees that after scaling the SAME    *
 * amount up and down with the mouse or left and right   *
 * with the ruler, the initial and final scaling factors *
 * are exactly the same. For example, if s0 is the       *
 * initial scaling, after scaling +s and -s, we get:     *
 * (s0 + s0 * s) + (s0 + s0 * s) * 1/(1/-s - 1) = s0     *
 *********************************************************/

if (scale > 0.0)
  {
  object->scale *= 1.0 + scale;
  if (object->scale > GAMGI_MESA_SCALE_UPPER)
    object->scale = GAMGI_MESA_SCALE_UPPER;
  }
else
  {
  object->scale *= 1 / (1.0 - scale);
  if (object->scale < GAMGI_MESA_SCALE_LOWER)
    object->scale = GAMGI_MESA_SCALE_LOWER;
  }
}

void gamgi_mesa_scale_light (gamgi_light *light, double scale)
{
/**************
 * spot light *
 **************/

/*********************************************************
 * this method guarantees that after scaling the SAME    *
 * amount up and down with the mouse or left and right   *
 * with the ruler, the initial and final scaling factors *
 * are exactly the equal. For example, if s0 is the      *
 * initial scaling, after scaling +s and -s, we get:     *
 * (s0 + s0 * s) + (s0 + s0 * s) * 1/(1/-s - 1) = s0     *
 *********************************************************/

if (light->angle != 180.0)
  {
  if (scale > 0.0)
    {
    light->angle *= 1.0 + scale;
    if (light->angle > 90.0) light->angle = 90.0;
    }
  else
    {
    light->angle *= 1 / (1.0 - scale);
    if (light->angle < GAMGI_MATH_TOLERANCE_ANGLE)
      light->angle = GAMGI_MATH_TOLERANCE_ANGLE;
    }
  }

}

void gamgi_mesa_scale_window (gamgi_window *window, double scale)
{
gamgi_layer *layer;
gamgi_dlist *dlist;

/********************
 * scale all layers *
 ********************/

for (dlist = window->layer_start; dlist != NULL; dlist = dlist->next)
  {
  layer = GAMGI_CAST_LAYER dlist->data;
  gamgi_mesa_scale_default (GAMGI_CAST_OBJECT layer, scale);
  }
}

void gamgi_mesa_scale_object (gamgi_object *object, double scale)
{
switch (object->class)
  {
  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_scale_light (GAMGI_CAST_LIGHT object, scale);
  break;

  default:
  gamgi_mesa_scale_default (object, scale);
  break;
  }
}

void gamgi_mesa_scale_default_list (gamgi_dlist *start, double scale)
{
gamgi_object *object;
gamgi_dlist *dlist;

/***************************************
 * scale list of non-recursive objects *
 ***************************************/

for (dlist = start; dlist != NULL; dlist = dlist->next)
  {
  object = GAMGI_CAST_OBJECT dlist->data;
  gamgi_mesa_scale_default (object, scale);
  }
}

void gamgi_mesa_scale_text_list (double scale)
{
gamgi_text *text;
gamgi_dlist *dlist;

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, TRUE);

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  {
  text = GAMGI_CAST_TEXT dlist->data;
  if (text->mark == FALSE)
    gamgi_mesa_scale_default (GAMGI_CAST_OBJECT text, scale);
  }

for (dlist = gamgi->texts; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_text (GAMGI_CAST_TEXT dlist->data, FALSE);
}

void gamgi_mesa_scale_group_list (double scale)
{
gamgi_group *group;
gamgi_dlist *dlist;

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, TRUE);

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  {
  group = GAMGI_CAST_GROUP dlist->data;
  if (group->mark == FALSE)
    gamgi_mesa_scale_default (GAMGI_CAST_OBJECT group, scale);
  }

for (dlist = gamgi->groups; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_group (GAMGI_CAST_GROUP dlist->data, FALSE);
}

void gamgi_mesa_scale_cluster_list (double scale)
{
gamgi_cluster *cluster;
gamgi_dlist *dlist;

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, TRUE);

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  {
  cluster = GAMGI_CAST_CLUSTER dlist->data;
  if (cluster->mark == FALSE)
    gamgi_mesa_scale_default (GAMGI_CAST_OBJECT cluster, scale);
  }

for (dlist = gamgi->clusters; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_cluster (GAMGI_CAST_CLUSTER dlist->data, FALSE);
}

void gamgi_mesa_scale_assembly_list (double scale)
{
gamgi_assembly *assembly;
gamgi_dlist *dlist;

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, TRUE);

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  {
  assembly = GAMGI_CAST_ASSEMBLY dlist->data;
  if (assembly->mark == FALSE)
    gamgi_mesa_scale_default (GAMGI_CAST_OBJECT assembly, scale);
  }

for (dlist = gamgi->assemblys; dlist != NULL; dlist = dlist->next)
  gamgi_global_selection_assembly (GAMGI_CAST_ASSEMBLY dlist->data, FALSE);
}

void gamgi_mesa_scale_light_list (double scale)
{
gamgi_light *light;
gamgi_dlist *dlist;

for (dlist = gamgi->lights; dlist != NULL; dlist = dlist->next)
  {
  light = GAMGI_CAST_LIGHT dlist->data;
  gamgi_mesa_scale_light (light, scale);
  }
}

void gamgi_mesa_scale_window_list (double scale)
{
gamgi_window *window;
gamgi_dlist *dlist;

for (dlist = gamgi->windows; dlist != NULL; dlist = dlist->next)
  {
  window = GAMGI_CAST_WINDOW dlist->data;
  gamgi_mesa_scale_window (window, scale);
  }
}

void gamgi_mesa_scale_gamgi_list (double scale)
{
gamgi_window *window;

for (window = gamgi->window_start; window != NULL; window = window->next)
  {
  gamgi_mesa_scale_window (window, scale);
  }
}

void gamgi_mesa_scale_list (double scale)
{
switch (gamgi->focus)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_mesa_scale_text_list (scale);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_mesa_scale_default_list (gamgi->orbitals, scale);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_mesa_scale_default_list (gamgi->bonds, scale);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_mesa_scale_default_list (gamgi->atoms, scale);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_mesa_scale_default_list (gamgi->directions, scale);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_mesa_scale_default_list (gamgi->planes, scale);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_mesa_scale_group_list (scale);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_mesa_scale_default_list (gamgi->molecules, scale);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_mesa_scale_cluster_list (scale);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_mesa_scale_default_list (gamgi->cells, scale);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_mesa_scale_default_list (gamgi->arrows, scale);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_mesa_scale_default_list (gamgi->shapes, scale);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_mesa_scale_default_list (gamgi->graphs, scale);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_mesa_scale_assembly_list (scale);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_mesa_scale_light_list (scale);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_mesa_scale_default_list (gamgi->layers, scale);
  break;

  case GAMGI_ENGINE_WINDOW:
  gamgi_mesa_scale_window_list (scale);
  break;

  case GAMGI_ENGINE_GAMGI:
  gamgi_mesa_scale_gamgi_list (scale);
  break;

  default:
  return;
  }

/****************************
 * redraw all OpenGL images *
 ****************************/

gamgi_mesa_draw_gamgi ();
}

void gamgi_mesa_scale (gamgi_window *window,
gamgi_object *object, double scale)
{
/************************************************************
 * scale list of objects, including the Gamgi global object *
 ************************************************************/

if (gamgi->focus != FALSE)
  { gamgi_mesa_scale_list (scale); return; }

/*********************************************
 * scale object with focus, which can be a   *
 * window, a layer or any other local object *
 *********************************************/

switch (object->class)
  {
  /**************************************
   * scale all layers in current window *
   **************************************/

  case GAMGI_ENGINE_WINDOW:
  gamgi_mesa_scale_window (window, scale);
  break;

  /*********************************************
   * translate current object in current layer *
   *********************************************/

  default:
  if (window->axes == FALSE)
    gamgi_mesa_scale_object (object, scale);
  else
    gamgi_mesa_scale_default (GAMGI_CAST_OBJECT window->layer, scale);
  }

/***********************
 * redraw OpenGL image *
 ***********************/

gtk_widget_queue_draw (window->area);
}
