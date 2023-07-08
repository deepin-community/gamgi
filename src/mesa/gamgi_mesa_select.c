/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_select.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_mesa.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_engine_list.h"
#include "gamgi_engine_count.h"
#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_object.h"
#include "gamgi_gtk_text_create.h"
#include "gamgi_gtk_text_copy.h"
#include "gamgi_gtk_text_select.h"
#include "gamgi_gtk_text_link.h"
#include "gamgi_gtk_text_modify.h"
#include "gamgi_gtk_text_remove.h"
#include "gamgi_gtk_orbital_create.h"
#include "gamgi_gtk_orbital_copy.h"
#include "gamgi_gtk_orbital_select.h"
#include "gamgi_gtk_orbital_link.h"
#include "gamgi_gtk_orbital_modify.h"
#include "gamgi_gtk_orbital_remove.h"
#include "gamgi_gtk_bond_create.h"
#include "gamgi_gtk_bond_select.h"
#include "gamgi_gtk_bond_link.h"
#include "gamgi_gtk_bond_modify.h"
#include "gamgi_gtk_bond_remove.h"
#include "gamgi_gtk_atom_create.h"
#include "gamgi_gtk_atom_copy.h"
#include "gamgi_gtk_atom_select.h"
#include "gamgi_gtk_atom_link.h"
#include "gamgi_gtk_atom_modify.h"
#include "gamgi_gtk_atom_measure.h"
#include "gamgi_gtk_atom_remove.h"
#include "gamgi_gtk_direction_create.h"
#include "gamgi_gtk_direction_copy.h"
#include "gamgi_gtk_direction_select.h"
#include "gamgi_gtk_direction_link.h"
#include "gamgi_gtk_direction_modify.h"
#include "gamgi_gtk_direction_measure.h"
#include "gamgi_gtk_direction_remove.h"
#include "gamgi_gtk_plane_create.h"
#include "gamgi_gtk_plane_copy.h"
#include "gamgi_gtk_plane_select.h"
#include "gamgi_gtk_plane_link.h"
#include "gamgi_gtk_plane_modify.h"
#include "gamgi_gtk_plane_measure.h"
#include "gamgi_gtk_plane_remove.h"
#include "gamgi_gtk_group_copy.h"
#include "gamgi_gtk_group_select.h"
#include "gamgi_gtk_group_link.h"
#include "gamgi_gtk_group_create.h"
#include "gamgi_gtk_group_modify.h"
#include "gamgi_gtk_group_measure.h"
#include "gamgi_gtk_group_remove.h"
#include "gamgi_gtk_molecule_copy.h"
#include "gamgi_gtk_molecule_select.h"
#include "gamgi_gtk_molecule_link.h"
#include "gamgi_gtk_molecule_measure.h"
#include "gamgi_gtk_molecule_remove.h"
#include "gamgi_gtk_cluster_copy.h"
#include "gamgi_gtk_cluster_select.h"
#include "gamgi_gtk_cluster_link.h"
#include "gamgi_gtk_cluster_measure.h"
#include "gamgi_gtk_cluster_remove.h"
#include "gamgi_gtk_cell_create.h"
#include "gamgi_gtk_cell_copy.h"
#include "gamgi_gtk_cell_select.h"
#include "gamgi_gtk_cell_modify.h"
#include "gamgi_gtk_cell_link.h"
#include "gamgi_gtk_cell_measure.h"
#include "gamgi_gtk_cell_remove.h"
#include "gamgi_gtk_assembly_copy.h"
#include "gamgi_gtk_assembly_select.h"
#include "gamgi_gtk_assembly_link.h"
#include "gamgi_gtk_assembly_remove.h"
#include "gamgi_gtk_light_copy.h"
#include "gamgi_gtk_light_modify.h"
#include "gamgi_gtk_light_select.h"
#include "gamgi_gtk_light_link.h"
#include "gamgi_gtk_light_remove.h"
#include "gamgi_gtk_layer_copy.h"
#include "gamgi_gtk_layer_modify.h"
#include "gamgi_gtk_layer_select.h"
#include "gamgi_gtk_layer_link.h"
#include "gamgi_gtk_layer_remove.h"
#include "gamgi_gtk_window_copy.h"
#include "gamgi_gtk_window_select.h"
#include "gamgi_gtk_window_link.h"
#include "gamgi_gtk_window_remove.h"
#include "gamgi_mesa_area.h"
#include "gamgi_mesa_pick.h"
#include "gamgi_mesa_grab.h"

static void static_select (gamgi_object *object, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GdkWindow *drawable;

if (object->class == GAMGI_ENGINE_WINDOW)
  {
  window = GAMGI_CAST_WINDOW object;
  drawable = gtk_widget_get_window (window->window);
  gdk_window_raise (drawable);
  }

if (object->class == GAMGI_ENGINE_LAYER)
  {
  gamgi_gtk_object_layer_local (GAMGI_CAST_LAYER object, window);
  gtk_widget_queue_draw (window->area);
  }

gamgi_gtk_object_focus_local (object, window);
}

static void static_apply (gamgi_object *object, void *data)
{
gamgi_gtk_dialog_beep ();
(* gamgi->function) (object, data);
}

static gamgi_slist *static_color (gamgi_slist *slist_old, int color_new)
{
gamgi_slist *slist;
int color_old;

slist = slist_old;
while (slist != NULL)
  {
  color_old = GAMGI_POINTER_TO_INT (slist->data);
  if (color_old == color_new) return slist_old;
  slist = slist->next;
  }
slist = gamgi_engine_slist_add_start (slist_old);
slist->data = GAMGI_INT_TO_POINTER (color_new);

return slist;
}

static void static_start (void)
{
unsigned char off[3] = {0, 0, 0};

/********************************
 * switch off colors and lights *
 ********************************/

glClearColor (0.0, 0.0, 0.0, 1.0);
glClear (GL_COLOR_BUFFER_BIT);
glColor3ubv (off);
glDisable (GL_LIGHTING);
glDisable (GL_NORMALIZE);
glDisable (GL_DITHER);

/**********************
 * reset object color *
 **********************/

gamgi->rgb[0] = 0; gamgi->rgb[1] = 0; gamgi->rgb[2] = 0;
}

static gamgi_slist *static_pick_pixels (int x, int y)
{
int i, color, n = gamgi->gamgi->tolerance;
int pixels;
unsigned char *rgb;
gamgi_slist *slist = NULL;
int r0, g0, b0, g1, b1;

pixels = pow (2*n + 1, 2);

/*****************************************************
 * gamgi->r, gamgi->g, gamgi->b represent the number *
 * of missing bits when compared with 8 (True Color) *
 *****************************************************/

r0 = pow (2, gamgi->r);
g0 = pow (2, gamgi->g);
b0 = pow (2, gamgi->b);

g1 = pow (2, 8 - gamgi->g);
b1 = pow (2, 8 - gamgi->b);

/*********************************************************************
 * grab colors for all 2*n+1 pixels in the square going from the     *
 * lower-left corner (x-n, y-n) to the upper-right corner (x+n, y+n) *
 *********************************************************************/

rgb = gamgi_mesa_area_read (x-n, y-n, 2*n+1, 2*n+1);

i = 0;
do
  {
  /**********************************************************
   * shift color bits to the right and then to the left,    *
   * to replace undefined bits by zeros. For example,       *
   * in a graphic system running 16 color bits per pixel,   *
   * distributed as 5 6 5, [rrrrr???] [gggggg??] [bbbbb???] *
   * would be replaced by [rrrrr000] [gggggg00] [bbbbb000]  *
   **********************************************************/

  rgb[i] = (rgb[i] >> gamgi->r) << gamgi->r;
  rgb[i+1] = (rgb[i+1] >> gamgi->g) << gamgi->g;
  rgb[i+2] = (rgb[i+2] >> gamgi->b) << gamgi->b;

  /**********************************************************************
   * r1,g1,b1 represent the number of color sequences recognized by     *
   * the graphic system. When scanning objects, b changes faster (less  *
   * important channel), then g, and finally r (more important channel) *
   *                                                                    *
   * r0,g0,b0 represent the number of sequences that become equal       *
   * for each channel, as the lower bits are ignored, so to get the     *
   * number of real colors scanned we have to divide by r0,g0,b0        *
   **********************************************************************/

  color = b1 * g1 * rgb[3*i]/r0 + b1 * rgb[3*i+1]/g0 + rgb[3*i+2]/b0;
  if (color != 0) slist = static_color (slist, color);
  }
  while (++i < pixels);

free (rgb);

return slist;
}

static gamgi_slist *static_grab_pixels (int x, int y, int width, int height,
gamgi_object **objects, int *levels, int level, gamgi_slist *slist)
{
int i, color;
int pixels;
unsigned char *rgb;
int r0, g0, b0, g1, b1;

pixels = width * height;

/*****************************************************
 * gamgi->r, gamgi->g, gamgi->b represent the number *
 * of missing bits when compared with 8 (True Color) *
 *****************************************************/

r0 = pow (2, gamgi->r);
g0 = pow (2, gamgi->g);
b0 = pow (2, gamgi->b);

g1 = pow (2, 8 - gamgi->g);
b1 = pow (2, 8 - gamgi->b);

/***********************************************************************
 * grab colors for all width height pixels in the rectangle going from *
 * the lower-left corner (x0, y0) to the upper-right corner (x1, y1)   *
 ***********************************************************************/

rgb = gamgi_mesa_area_read (x, y, width, height);

i = 0;
do
  {
  /**********************************************************
   * shift color bits to the right and then to the left,    *
   * to replace undefined bits by zeros. For example,       *
   * in a graphic system running 16 color bits per pixel,   *
   * distributed as 5 6 5, [rrrrr???] [gggggg??] [bbbbb???] *
   * would be replaced by [rrrrr000] [gggggg00] [bbbbb000]  *
   **********************************************************/

  rgb[i] = (rgb[i] >> gamgi->r) << gamgi->r;
  rgb[i+1] = (rgb[i+1] >> gamgi->g) << gamgi->g;
  rgb[i+2] = (rgb[i+2] >> gamgi->b) << gamgi->b;

  /**********************************************************************
   * r1,g1,b1 represent the number of color sequences recognized by     *
   * the graphic system. When scanning objects, b changes faster (less  *
   * important channel), then g, and finally r (more important channel) *
   *                                                                    *
   * r0,g0,b0 represent the number of sequences that become equal       *
   * for each channel, as the lower bits are ignored, so to get the     *
   * number of real colors scanned we have to divide by r0,g0,b0        *
   **********************************************************************/

  color = b1 * g1 * rgb[3*i]/r0 + b1 * rgb[3*i+1]/g0 + rgb[3*i+2]/b0;
  if (color == 0 || levels[color - 1] == level) continue;

  levels[color - 1] = level; 
  slist = gamgi_engine_slist_add_start (slist);
  slist->data = objects[color - 1];
  }
  while (++i < pixels);

free (rgb);

return slist;
}

static gamgi_object *static_pick_objects (int x, int y, int class,
gamgi_bool global, gamgi_window *window)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (window->area);
GdkGLContext *glcontext = gtk_widget_get_gl_context (window->area);
gamgi_object **objects;
gamgi_object *object;
gamgi_slist *slist;
int index, number;

/*************************************
 * count number of candidate objects *
 *************************************/

number = 0;
if (global == TRUE)
  gamgi_engine_count_window (window, class, &number);
else
  gamgi_engine_count_layer (window->layer, class, &number);
if (number == 0) return NULL;

/*******************************************************
 * allocate array to store candidate objects addresses *
 *******************************************************/

objects = (gamgi_object **) malloc (number * sizeof (object));
if (objects == NULL) return NULL;

/**********************************************************
 * draw candidate objects in back buffer, each one with a *
 * different color, and store their addresses in objects  *
 **********************************************************/

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  static_start ();

  if (global == TRUE)
    gamgi_mesa_pick_window (window, class, objects);
  else
    gamgi_mesa_pick_layer (window->layer, window, class, objects);

  gdk_gl_drawable_gl_end (gldrawable);
  }

/************************************************
 * grab sampling area and build a list with     *
   all the non-background colors in there,      *
   each one corresponding to a different object *
 *                                              *
 *     a selection is invalid when no object    *
 *        or more than one has been found       *
 ************************************************/

slist = static_pick_pixels (x, y);
if (slist == NULL || slist->next != NULL) object = NULL;
else
  {
  index = GAMGI_POINTER_TO_INT (slist->data);
  object = GAMGI_CAST_OBJECT objects[index - 1];
  }

/******************
 * free resources *
 ******************/

free (objects);
gamgi_engine_slist_remove_all (slist);

return object;
}

gamgi_slist *static_grab_objects (int *box, gamgi_enum class,
gamgi_window *window)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (window->area);
GdkGLContext *glcontext = gtk_widget_get_gl_context (window->area);
gamgi_object **objects;
int *levels;
int width, height;
gamgi_object *object;
gamgi_slist *slist_old, *slist_new;
int number;
int level;

/*************************************
 * count number of candidate objects *
 *************************************/

number = 0;
gamgi_engine_count_window (window, class, &number);
if (number == 0) return NULL;

/****************************************************************
 * 1) allocate array to store candidate objects addresses       *
 * 2) allocate array to store candidate objects scanning levels *
 ****************************************************************/

objects = (gamgi_object **) malloc (number * sizeof (object));
if (objects == NULL) return NULL;

levels = (int *) malloc (number * sizeof (int));
if (levels == NULL) return NULL;
for (level = 0; level < number; level++) levels[level] = 0;

/***********************************************
 * render objects sucessively until no more    *
 * objects can be found in selection rectangle *
 ***********************************************/

width = box[2] - box[0] + 1;
height = box[3] - box[1] + 1;
slist_new = NULL;
level = 0;

do
  {
  level++;
  slist_old = slist_new;

  /***********************************************************
   * 3D code: draw candidate objects in back buffer, each    *
   * one with a different color, store their addresses in    *
   * objects and the scans when they were selected in levels *
   *                                                         *
   * when an object is selected, it stops being rendered, so *
   * objects hidden below can be selected in the next scans  *
   ***********************************************************/
         
  if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
    {      
    static_start ();
    gamgi_mesa_grab_window (window, class, level, -1, objects, levels);
    gdk_gl_drawable_gl_end (gldrawable);
    }

  /***********************************************************
   * 2D code: read the selection rectangle and analyse each  *
   * pixel to find all the rendered objects inside. Build    *
   * a list with all the objects found in the various scans. *
   ***********************************************************/

  slist_new = static_grab_pixels (box[0], box[1],
  width, height, objects, levels, level, slist_old);
  } while (slist_new != slist_old);

/******************
 * free resources *
 ******************/

free (objects);
free (levels);

return slist_new;
}

static void static_activate (GtkWidget *widget, void *data)
{
gamgi_object *object;

object = GAMGI_CAST_OBJECT g_object_get_data (G_OBJECT (widget), "object");

static_apply (object, data);
}

static GtkWidget *static_item (GtkWidget *menu,
gamgi_object *object, gamgi_window *window)
{
GtkWidget *item;
char string[GAMGI_ENGINE_STRING];

sprintf (string, "%s %d", object->name, object->number);
item = gtk_menu_item_new_with_label (string);
g_object_set_data (G_OBJECT (item), "object", object);

if (menu == NULL) menu = gtk_menu_new ();
gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

g_signal_connect (item, "activate",
G_CALLBACK (static_activate), window);
gtk_widget_show (item);

return menu;
}

static void static_window (gamgi_window *window, gamgi_window *window_start)
{
GtkWidget *menu = NULL;

do
  {
  menu = static_item (menu, GAMGI_CAST_OBJECT window_start, window);
  } while ((window_start = window_start->next) != NULL);

/********************************
 * list of items cannot be NULL *
 ********************************/

gtk_menu_popup ((GtkMenu *) menu, NULL, NULL, NULL, NULL, 1, 0);
}

static void static_menu (gamgi_window *window, gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist;
GtkWidget *menu = NULL;

for (dlist = dlist_start; dlist != NULL; dlist = dlist->next)
  menu = static_item (menu, GAMGI_CAST_OBJECT dlist->data, window);
  
/*****************************
 * list of items can be NULL *
 *****************************/

if (menu != NULL)
  gtk_menu_popup ((GtkMenu *) menu, NULL, NULL, NULL, NULL, 1, 0);
}

static void static_light (gamgi_window *window, gamgi_bool global)
{
gamgi_layer *layer;
gamgi_object *object;
gamgi_dlist *dlist_w, *dlist_l;
GtkWidget *menu = NULL;

if (global == FALSE)
  { static_menu (window, window->layer->light_start); return; }

for (dlist_w = window->layer_start; dlist_w != NULL; dlist_w = dlist_w->next)
  {
  layer = GAMGI_CAST_LAYER dlist_w->data;
  for (dlist_l = layer->light_start; dlist_l != NULL; dlist_l = dlist_l->next)
    {
    object = GAMGI_CAST_OBJECT dlist_l->data;
    menu = static_item (menu, object, window);
    }
  }

/*****************************
 * list of items can be NULL *
 *****************************/

if (menu != NULL)
  gtk_menu_popup ((GtkMenu *) menu, NULL, NULL, NULL, NULL, 1, 0);
}

static void static_recursive (int x, int y, int class, gamgi_bool global,
gamgi_window *window)
{
gamgi_object *object;
GtkWidget *menu = NULL;

object = static_pick_objects (x, y, class, global, window);
if (object == NULL) return;

if (object->object->class != class)
  { static_apply (object, window); return; }

do
  {
  menu = static_item (menu, object, window);
  object = object->object;
  } while (object->class == class);

/********************************
 * list of items cannot be NULL *
 ********************************/

gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL, 1, 0);
}

static gamgi_bool static_box (int *box, gamgi_window *window)
{
GtkAllocation allocation;
int width, height;

/***********************************************
 * box[0], box[1]: below left x,y coordinates  *
 * box[2], box[3]: above right x,y coordinates *
 ***********************************************/

/*******************************
 * get graphic area dimensions *
 *******************************/

gtk_widget_get_allocation (window->area, &allocation);
width = allocation.width;
height = allocation.height;

/**************************************************
 * the box is completely outside the graphic area *
 **************************************************/

if (box[0] > width - 1) return FALSE;
if (box[1] > height - 1) return FALSE;
if (box[2] < 0) return FALSE;
if (box[3] < 0) return FALSE;

/*************************************************
 * the box is partially outside the graphic area *
 *************************************************/

if (box[0] < 0) box[0] = 0;
if (box[1] < 0) box[1] = 0;
if (box[2] > width - 1) box[2] = width - 1;
if (box[3] > height - 1) box[3] = height - 1;

return TRUE;
}

gamgi_window *gamgi_mesa_select_window (void)
{
gamgi_window *window;

/*********************************************************
 * Search for top window with active task dialog:        *
 * Only one task dialog can be active in Gamgi at        *
 * any time, called from any window (the window_dialog). *
 *********************************************************/

for (window = gamgi->window_start; window != NULL; window = window->next)
  if (window->action != FALSE) return window;

return NULL;
}

gamgi_bool gamgi_mesa_select_action (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y)
{
gamgi_window *window_dialog;

/*************************************************
 * search for top window with active task dialog *
 *************************************************/

window_dialog = gamgi_mesa_select_window ();
if (window_dialog == NULL) return FALSE;
gamgi->window_dialog = window_dialog;

switch (window_dialog->action)
  {
  /******************************************************
   * list of all dialog tasks with mouse press activity *
   ******************************************************/

  case GAMGI_GTK_TEXT_CREATE:
  gamgi_gtk_text_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_TEXT_COPY:
  gamgi_gtk_text_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_TEXT_SELECT:
  gamgi_gtk_text_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_TEXT_LINK:
  gamgi_gtk_text_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_TEXT_MODIFY:
  gamgi_gtk_text_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_TEXT_REMOVE:
  gamgi_gtk_text_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_CREATE:
  gamgi_gtk_orbital_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_COPY:
  gamgi_gtk_orbital_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_SELECT:
  gamgi_gtk_orbital_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_LINK:
  gamgi_gtk_orbital_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_MODIFY:
  gamgi_gtk_orbital_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ORBITAL_REMOVE:
  gamgi_gtk_orbital_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_BOND_CREATE:
  gamgi_gtk_bond_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_BOND_SELECT:
  gamgi_gtk_bond_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_BOND_LINK:
  gamgi_gtk_bond_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_BOND_MODIFY:
  gamgi_gtk_bond_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_BOND_REMOVE:
  gamgi_gtk_bond_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_CREATE:
  gamgi_gtk_atom_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_COPY:
  gamgi_gtk_atom_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_SELECT:
  gamgi_gtk_atom_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_LINK:
  gamgi_gtk_atom_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_MODIFY:
  gamgi_gtk_atom_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_MEASURE:
  gamgi_gtk_atom_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ATOM_REMOVE:
  gamgi_gtk_atom_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_CREATE:
  gamgi_gtk_direction_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_COPY:
  gamgi_gtk_direction_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_SELECT:
  gamgi_gtk_direction_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_LINK:
  gamgi_gtk_direction_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_MODIFY:
  gamgi_gtk_direction_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_MEASURE:
  gamgi_gtk_direction_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_DIRECTION_REMOVE:
  gamgi_gtk_direction_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_CREATE:
  gamgi_gtk_plane_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_COPY:
  gamgi_gtk_plane_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_SELECT:
  gamgi_gtk_plane_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_LINK:
  gamgi_gtk_plane_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_MODIFY:
  gamgi_gtk_plane_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_MEASURE:
  gamgi_gtk_plane_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_PLANE_REMOVE:
  gamgi_gtk_plane_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_CREATE:
  gamgi_gtk_group_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_COPY:
  gamgi_gtk_group_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_SELECT:
  gamgi_gtk_group_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_LINK:
  gamgi_gtk_group_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_MODIFY:
  gamgi_gtk_group_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_MEASURE:
  gamgi_gtk_group_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_GROUP_REMOVE:
  gamgi_gtk_group_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_MOLECULE_COPY:
  gamgi_gtk_molecule_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_MOLECULE_SELECT:
  gamgi_gtk_molecule_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_MOLECULE_LINK:
  gamgi_gtk_molecule_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_MOLECULE_MEASURE:
  gamgi_gtk_molecule_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_MOLECULE_REMOVE:
  gamgi_gtk_molecule_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CLUSTER_COPY:
  gamgi_gtk_cluster_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CLUSTER_SELECT:
  gamgi_gtk_cluster_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CLUSTER_LINK:
  gamgi_gtk_cluster_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CLUSTER_MEASURE:
  gamgi_gtk_cluster_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CLUSTER_REMOVE:
  gamgi_gtk_cluster_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_CREATE:
  gamgi_gtk_cell_create_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_COPY:
  gamgi_gtk_cell_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_SELECT:
  gamgi_gtk_cell_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_MODIFY:
  gamgi_gtk_cell_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE; 

  case GAMGI_GTK_CELL_LINK:
  gamgi_gtk_cell_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_MEASURE:
  gamgi_gtk_cell_measure_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_CELL_REMOVE:
  gamgi_gtk_cell_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ASSEMBLY_COPY:
  gamgi_gtk_assembly_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ASSEMBLY_SELECT:
  gamgi_gtk_assembly_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ASSEMBLY_LINK:
  gamgi_gtk_assembly_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_ASSEMBLY_REMOVE:
  gamgi_gtk_assembly_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LIGHT_COPY:
  gamgi_gtk_light_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LIGHT_SELECT:
  gamgi_gtk_light_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LIGHT_LINK:
  gamgi_gtk_light_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LIGHT_MODIFY:
  gamgi_gtk_light_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LIGHT_REMOVE:
  gamgi_gtk_light_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LAYER_COPY:
  gamgi_gtk_layer_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LAYER_SELECT:
  gamgi_gtk_layer_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LAYER_LINK:
  gamgi_gtk_layer_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LAYER_MODIFY:
  gamgi_gtk_layer_modify_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_LAYER_REMOVE:
  gamgi_gtk_layer_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_WINDOW_COPY:
  gamgi_gtk_window_copy_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_WINDOW_SELECT:
  gamgi_gtk_window_select_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_WINDOW_LINK:
  gamgi_gtk_window_link_press (window_mouse, event, x, y, window_dialog);
  return TRUE;

  case GAMGI_GTK_WINDOW_REMOVE:
  gamgi_gtk_window_remove_press (window_mouse, event, x, y, window_dialog);
  return TRUE;
  }

return FALSE;
}

void gamgi_mesa_select_old (unsigned char *color)
{
float rgb[4];

glGetFloatv (GL_CURRENT_COLOR, rgb);

color[0] = rgb[0] * 255;
color[1] = rgb[1] * 255;
color[2] = rgb[2] * 255;
}

gamgi_bool gamgi_mesa_select_new (unsigned char *color)
{
unsigned char *rgb = gamgi->rgb;

/************************************************
 * This code works for 16, 24 and 32 rgb color  *
 * bits or even other exotic depth combinations *
 ************************************************/

rgb[2] += 1 << gamgi->b;
if (rgb[2] == 0)
  {
  rgb[1] += 1 << gamgi->g;
  if (rgb[1] == 0)
    {
    rgb[0] += 1 << gamgi->r;
    if (rgb[0] == 0)
      {
      /***********************************************************
       * Error: more than 2**r * 2**g * 2**b objects to select   *
       *                                                         *
       * this is tested before, so GAMGI should never come here  *
       *                                                         *
       * we can remove this rather large limit on the number of  *
       * objects per layer that can be selected, by scanning the *
       * image more than once, and taking down the objects that  *
       * failed each sucessive test until the number of elegible *
       * objects becomes smaller than the number of colors.      *
       ***********************************************************/
       
      return FALSE;
      }
    }
  }

color[0] = rgb[0];
color[1] = rgb[1];
color[2] = rgb[2];

return TRUE;
}

void gamgi_mesa_select_object (gamgi_window *window, int x, int y,
gamgi_enum class, gamgi_bool global, gamgi_function2 function)
{
gamgi_object *object;

/***************************************
 * when no local function is supplied, *
 * normal object selection is assumed  *
 ***************************************/

if (function == NULL)
  gamgi->function = static_select;
else
  gamgi->function = function;

/*************************************
 * select object using a combination *
 * of rendering and menu techniques  *
 *************************************/

switch (class)
  {
  case GAMGI_ENGINE_GAMGI: 
  break;

  case GAMGI_ENGINE_WINDOW: 
  static_window (window, gamgi->window_start);
  break;

  case GAMGI_ENGINE_LAYER: 
  static_menu (window, window->layer_start);
  break;

  case GAMGI_ENGINE_LIGHT:
  static_light (window, global);
  break;
  
  case GAMGI_ENGINE_ASSEMBLY: case GAMGI_ENGINE_CLUSTER:
  case GAMGI_ENGINE_GROUP: case GAMGI_ENGINE_TEXT:
  static_recursive (x, y, class, global, window);
  break;

  default:
  object = static_pick_objects (x, y, class, global, window);
  if (object != NULL) static_apply (object, window);
  break;
  }

/**************************
 * reset object selection *
 **************************/

window->class = FALSE;
}

gamgi_slist *gamgi_mesa_select_region (int *box, gamgi_enum class,
gamgi_window *window_mouse, gamgi_window *window_dialog)
{
gamgi_slist *slist;

/**********************************************************************
 * objects without visual representation cannot be selected by region *
 **********************************************************************/

if (class == GAMGI_ENGINE_GAMGI || class == GAMGI_ENGINE_WINDOW ||
class == GAMGI_ENGINE_LAYER || class == GAMGI_ENGINE_LIGHT) return NULL;

/*************************************************************
 * make sure the sampling box is entirely inside the graphic *
 * area (return NULL only when the box is entirely outside)  *
 *************************************************************/

if (static_box (box, window_mouse) == FALSE) return NULL;

/********************************************************
 * grab objects in selected rectangle that are in scope *
 ********************************************************/

slist = static_grab_objects (box, class, window_mouse);

return slist;
}

void gamgi_mesa_select_reset (gamgi_window *window)
{
gamgi->state = FALSE;
if (gamgi->window_mouse != NULL)
  {
  gtk_widget_queue_draw (gamgi->window_mouse->area);
  gamgi->window_mouse = NULL;
  }
}

void gamgi_mesa_select_state (char *name, gamgi_window *window)
{
if (strcmp (name, "Region") == 0)
  gamgi->state = GAMGI_MESA_SELECT_READY;
else
  gamgi_mesa_select_reset (window);
}

void gamgi_mesa_select_draw (gamgi_window *window)
{
GtkAllocation allocation;
float *color = window->layer->color;
int *box = gamgi->box;
int width, height;
int square = GAMGI_MESA_SELECT_SQUARE;

if (window != gamgi->window_mouse) return;

/*************************************
 * box format: state, x0, y0, x1, y1 *
 *************************************/

if (gamgi->state < GAMGI_MESA_SELECT_SIZE) return;

gtk_widget_get_allocation (window->area, &allocation);
width = allocation.width;
height = allocation.height;

/**************************************************
 * equivalent to gluOrtho2D (0, width, 0, height) *
 **************************************************/

glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
glOrtho (0, width, 0, height, -1, +1);

/************************
 * restart depth buffer *
 ************************/

glMatrixMode (GL_MODELVIEW);
glLoadIdentity ();
glClear (GL_DEPTH_BUFFER_BIT);

/**************************************************
 * use a color that is visible, over the current  *
 * background color, defined by the current layer *
 **************************************************/

if (color[0] + color[1] + color[2] > 1.5)
  glColor3f (0.0, 0.0, 0.0);
else
  glColor3f (1.0, 1.0, 1.0);

/************************************************************************
 * To get accurate 2D pixel rendering, OpenGL polygons such those built *
 * with GL_POLYGON and glRecti should be rendered between initial pixel *
 * and final pixel plus 1 (the box borders), while point and line       *
 * constructions such those built with GL_LINE_LOOP should be rendered  *
 * adding 0.5 to each pixel (to define exactly the center of the pixel) *
 *                                                                      *
 * A simpler alternative, suggested by the OpenGL Programming Guide     *
 * (the red book), Release 1, 1993, pg. 482 (OpenGL correctness Tips),  *
 * is to translate initially (0.375, 0.375, 0.0), and then add both     *
 * polygons and lines always with their natural integer coordinates     *
 *                                                                      *
 * Use 1.0 (a float) default line width to better get 2D pixel accuracy *
 ************************************************************************/

glTranslatef (0.375, 0.375, 0.0);
glLineWidth (1.0);

/**************************
 * draw the selection box *
 **************************/

glBegin (GL_LINE_LOOP);
glVertex2i (box[0], box[1]);
glVertex2i (box[2], box[1]);
glVertex2i (box[2], box[3]);
glVertex2i (box[0], box[3]);
glEnd ();

if (gamgi->state < GAMGI_MESA_SELECT_DONE) return;

/************************
 * draw the move square *
 ************************/

glRecti (box[0], box[3] - square + 1, box[0] + square, box[3] + 1);

/**************************
 * draw the resize square *
 **************************/

glBegin (GL_LINE_LOOP);
glVertex2i (box[2] - square + 1, box[1]);
glVertex2i (box[2], box[1]);
glVertex2i (box[2], box[1] + square - 1);
glVertex2i (box[2] - square + 1, box[1] + square - 1);
glEnd ();

/**********************************************
 * blend everything outside the selection box *
 **********************************************/

glEnable (GL_BLEND);
glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glColor4f (color[0], color[1], color[2], GAMGI_MESA_SELECT_BLEND);

glBegin (GL_POLYGON);
glVertex2i (0, 0);
glVertex2i (width, 0);
glVertex2i (width, box[1]);
glVertex2i (0, box[1]);
glEnd ();

glBegin (GL_POLYGON);
glVertex2i (0, box[3] + 1);
glVertex2i (width, box[3] + 1);
glVertex2i (width, height);
glVertex2i (0, height);
glEnd ();

glBegin (GL_POLYGON);
glVertex2i (0, box[1]);
glVertex2i (box[0], box[1]);
glVertex2i (box[0], box[3] + 1);
glVertex2i (0, box[3] + 1);
glEnd ();

glBegin (GL_POLYGON);
glVertex2i (box[2] + 1, box[1]);
glVertex2i (width, box[1]);
glVertex2i (width, box[3] + 1);
glVertex2i (box[2] + 1, box[3] + 1);
glEnd ();

glDisable (GL_BLEND);
}

void gamgi_mesa_select_press (int x, int y, gamgi_window *window)
{
int *box = gamgi->box;
int square = GAMGI_MESA_SELECT_SQUARE;

if (gamgi->state == GAMGI_MESA_SELECT_READY)
  {
  gamgi->window_mouse = window;
  gamgi->state = GAMGI_MESA_SELECT_SIZE;
  box[0] = box[2] = x;
  box[1] = box[3] = y;
  }

else if (gamgi->state == GAMGI_MESA_SELECT_DONE)
  {
  if (window != gamgi->window_mouse)
    {
    gamgi->state = GAMGI_MESA_SELECT_READY;
    gtk_widget_queue_draw (gamgi->window_mouse->area);
    gamgi_mesa_select_press (x, y, window);
    }

  else if ((x >= box[0] && x <= box[0] + square - 1) &&
  (y >= box[3] - square + 1 && y <= box[3]))
    {
    gamgi->state = GAMGI_MESA_SELECT_MOVE;
    }

  else if ((x >= box[2] - square + 1 && x <= box[2]) &&
  (y >= box[1] && y <= box[1] + square - 1))
    {
    gamgi->state = GAMGI_MESA_SELECT_RESIZE;
    }

  else
    {
    gamgi->state = GAMGI_MESA_SELECT_READY;
    gtk_widget_queue_draw (gamgi->window_mouse->area);
    gamgi_mesa_select_press (x, y, window);
    }
  }
}

void gamgi_mesa_select_release (int x, int y, gamgi_window *window)
{
int *box = gamgi->box;
int square = GAMGI_MESA_SELECT_SQUARE;
int swap;

if (gamgi->state == GAMGI_MESA_SELECT_SIZE ||
gamgi->state == GAMGI_MESA_SELECT_MOVE ||
gamgi->state == GAMGI_MESA_SELECT_RESIZE)
  gamgi->state = GAMGI_MESA_SELECT_DONE;

/*******************************************************************
 * swap coordinates if necessary: box[0],box[1] should contain the *
 * lower left corner, and box[2],box[3] the higher right corner    *
 *******************************************************************/

if (box[2] < box[0])
  { swap = box[0]; box[0] = box[2]; box[2] = swap; }
if (box[3] < box[1])
  { swap = box[1]; box[1] = box[3]; box[3] = swap; }

/**************************************************
 * set minimum dimensions for selection rectangle *
 **************************************************/

if (box[2] < box[0] + 2 * square - 1)
  box[2] = box[0] + 2 * square - 1;
if (box[3] < box[1] + 2 * square - 1)
  box[3] = box[1] + 2 * square - 1;

gtk_widget_queue_draw (window->area);
}

void gamgi_mesa_select_notify (int x, int y, gamgi_window *window)
{
GtkAllocation allocation;
int *box = gamgi->box;
int height;
int square = GAMGI_MESA_SELECT_SQUARE;
int dx, dy;

gtk_widget_get_allocation (window->area, &allocation);
height = allocation.height;

if (gamgi->state == GAMGI_MESA_SELECT_SIZE)
  {
  box[2] = x;
  box[3] = y;
  }

else if (gamgi->state == GAMGI_MESA_SELECT_MOVE)
  {
  dx = x - window->begin_x;
  box[0] += dx;
  box[2] += dx;

  dy = y - (height - window->begin_y);
  box[1] += dy;
  box[3] += dy;
  }

else if (gamgi->state == GAMGI_MESA_SELECT_RESIZE)
  {
  dx = x - window->begin_x;
  box[2] += dx;
  if (box[2] < box[0] + 2 * square - 1)
    box[2] = box[0] + 2 * square - 1;

  dy = y - (height - window->begin_y);
  box[1] += dy;
  if (box[1] > box[3] - 2 * square + 1)
    box[1] = box[3] - 2 * square + 1;
  }

gtk_widget_queue_draw (window->area);
}
