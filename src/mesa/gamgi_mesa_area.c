/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_area.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_mesa.h"
#include "gamgi_global.h"

#include "gamgi_mesa_draw.h"
#include "gamgi_mesa_select.h"
#include "gamgi_mesa_rotate.h"
#include "gamgi_mesa_translate.h"
#include "gamgi_mesa_scale.h"
#include "gamgi_mesa_trackball.h"
#include "gamgi_math_vector.h"

void gamgi_mesa_area_project (gamgi_window *window, double x,
double y, double z, double *x_2d, double *y_2d, double *z_2d)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (window->area);
GdkGLContext *glcontext = gtk_widget_get_gl_context (window->area);
double projection[16];
double modelview[16];
int viewport[4];

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

  gluProject (x, y, z, modelview, projection, viewport, x_2d, y_2d, z_2d);

  gdk_gl_drawable_gl_end (gldrawable);
  }
}

void gamgi_mesa_area_unproject (gamgi_window *window,
int x_2d, int y_2d, double *x, double *y, double *z)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (window->area);
GdkGLContext *glcontext = gtk_widget_get_gl_context (window->area);
double *center;
double projection[16];
double modelview[16];
float z_2d;
int viewport[4];

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

  /*************************************************************
   * if z_2d < 1.0 there is other object already at these      *
   * x,y coordinates, so the new object will be immediately    *
   * above, otherwise write the new object with the same depth *
   * as the center of the current layer: window->layer->center *
   *************************************************************/

  glReadPixels (x_2d, y_2d, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z_2d);
  if (z_2d == 1.0)
    {
    center = window->layer->center;
    gluProject (center[0], center[1], center[2],
    modelview, projection, viewport, x, y, z);
    z_2d = *z;
    }

  gluUnProject ((double) x_2d, (double) y_2d, (double) z_2d,
  modelview, projection, viewport, x, y, z);

  gdk_gl_drawable_gl_end (gldrawable);
  }
}

unsigned char *gamgi_mesa_area_read (int x, 
int y, int width, int height)
{
unsigned char *buffer;
int swapbytes, lsbfirst, rowlength;
int skiprows, skippixels, alignment;

buffer = (unsigned char *) malloc (sizeof (unsigned char) * width * height * 3);

/********************************************
 * Read data: adapted from the book OpenGL  *
 * Programming for the X Window System p99, *
 * Mark J. Kilgard, Addison-Wesley, 1996.   *
 ********************************************/

/**********************
 * Save current modes *
 **********************/

glGetIntegerv(GL_PACK_SWAP_BYTES, &swapbytes);
glGetIntegerv(GL_PACK_LSB_FIRST, &lsbfirst);
glGetIntegerv(GL_PACK_ROW_LENGTH, &rowlength);
glGetIntegerv(GL_PACK_SKIP_ROWS, &skiprows);
glGetIntegerv(GL_PACK_SKIP_PIXELS, &skippixels);
glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);

/*********************
 * Set reading modes *
 *********************/

glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
glPixelStorei(GL_PACK_LSB_FIRST, GL_FALSE);
glPixelStorei(GL_PACK_ROW_LENGTH, 0);
glPixelStorei(GL_PACK_SKIP_ROWS, 0);
glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
glPixelStorei(GL_PACK_ALIGNMENT, 1);

/********************
 * Read the pixels. *
 ********************/

glReadPixels (x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);

/************************
 *  Restore saved modes *
 ************************/

glPixelStorei(GL_PACK_SWAP_BYTES, swapbytes);
glPixelStorei(GL_PACK_LSB_FIRST, lsbfirst);
glPixelStorei(GL_PACK_ROW_LENGTH, rowlength);
glPixelStorei(GL_PACK_SKIP_ROWS, skiprows);
glPixelStorei(GL_PACK_SKIP_PIXELS, skippixels);
glPixelStorei(GL_PACK_ALIGNMENT, alignment);

return buffer;
}

gamgi_bool gamgi_mesa_area_configure (GtkWidget *widget, 
GdkEventConfigure *event, void *data)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
GtkAllocation allocation;

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  gdk_gl_drawable_wait_gdk (gldrawable);
  gtk_widget_get_allocation (widget, &allocation);
  glViewport (0, 0, allocation.width, allocation.height);
  gdk_gl_drawable_gl_end (gldrawable);
  }

return TRUE;
}

gamgi_bool gamgi_mesa_area_expose (GtkWidget *widget, 
GdkEventExpose *event, void *data)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
gamgi_window *window = GAMGI_CAST_WINDOW data;

/************************* 
 * draw only last expose *
 *************************/

if (event->count > 0) return TRUE;

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  gamgi_mesa_draw_window (window);
  gdk_gl_drawable_swap_buffers (gldrawable);
  glGetError ();
  gdk_gl_drawable_gl_end (gldrawable);
  }

return TRUE;
}

gamgi_bool gamgi_mesa_area_press (GtkWidget *widget, 
GdkEventButton *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkAllocation allocation;
int x, y;

/*************************************
 * OpenGL puts origin at the bottom, *
 * X Window puts origin at the top.  *
 *************************************/

x = event->x;
gtk_widget_get_allocation (widget, &allocation);
y = allocation.height - event->y;

/******************************
 * handle rectangle selection *
 ******************************/

if (gamgi->state != FALSE)
  gamgi_mesa_select_press (x, y, window);

/********************************************************
 * select object to get focus or for dialog information *
 ********************************************************/

else if (window->class == FALSE)
  gamgi_mesa_select_action (window, event, x, y);
else
  gamgi_mesa_select_object (window, x, y, window->class, FALSE, NULL);

/******************************************* 
 * beginning of drag: reset mouse position *
 *******************************************/

window->begin_x = event->x;
window->begin_y = event->y;

return TRUE;
}

gamgi_bool gamgi_mesa_area_release (GtkWidget *widget,
GdkEventButton *event, void *data)
{
GtkAllocation allocation;
int x, y;

/*************************************
 * OpenGL puts origin at the bottom, *
 * X Window puts origin at the top.  *
 *************************************/

x = event->x;
gtk_widget_get_allocation (widget, &allocation);
y = allocation.height - event->y;

/*************************************
 * OpenGL puts origin at the bottom, *
 * X Window puts origin at the top.  *
 *************************************/

if (gamgi->state != FALSE)
  gamgi_mesa_select_release (x, y, GAMGI_CAST_WINDOW data);

return TRUE;
}

gamgi_bool gamgi_mesa_area_notify (GtkWidget *widget, 
GdkEventMotion *event, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GdkModifierType state;
GtkAllocation allocation;
double axis[3], translate[3];
double angle, scale;
int width, height;
int x, y; 

if (event->is_hint) gdk_window_get_pointer (event->window, &x, &y, &state);
else
  {
  x = event->x;
  y = event->y;
  state = event->state;
  }

gtk_widget_get_allocation (widget, &allocation);
width = allocation.width;
height = allocation.height;

/***********************************
 * handle selection rectangle drag *
 ***********************************/

if (state != FALSE && gamgi->state != FALSE)
  gamgi_mesa_select_notify (x, height - y, window);

/****************************
 * handle normal mouse drag *
 ****************************/

else if (state & gamgi->gamgi->button1)
  {
  /***************************
   * trackball rotating drag *
   ***************************/

  gamgi_mesa_trackball ((2.0*x - width)/width, (height - 2.0*y)/height,
  (2.0*window->begin_x - width)/width, (height - 2.0*window->begin_y)/height,
  axis, &angle);

  /***************************************************
   * when rotating the referential, invert the xy    *
   * rotations: this is much more intuitive than the *
   * normal trackball model used to rotate objects   *
   ***************************************************/
   
  if (window->axes == TRUE)
    { axis[0] = -axis[0]; axis[1] = -axis[1]; }

  gamgi_mesa_rotate (window, window->focus, angle, axis);
  }

else if (state & gamgi->gamgi->button2)
  {
  /***************
   * moving drag *
   ***************/

  gamgi_math_vector_absolute (translate,
  GAMGI_MESA_MOVE_SPEED/2 * (x - window->begin_x) / width,
  -GAMGI_MESA_MOVE_SPEED/2 * (y - window->begin_y) / height, 0.0);

  gamgi_mesa_translate (window, window->focus, translate);
  }

else if (state & gamgi->gamgi->button3)
  {
  /****************
   * scaling drag *
   ****************/

  scale = ((window->begin_y - y) / height) * GAMGI_MESA_SCALE_SPEED;

  gamgi_mesa_scale (window, window->focus, scale);
  }

window->begin_x = x;
window->begin_y = y;

return TRUE;
}

gamgi_bool gamgi_mesa_area_start (GtkWidget *widget, void *data)
{
GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);

if (gdk_gl_drawable_gl_begin (gldrawable, glcontext) == TRUE)
  {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glCullFace (GL_BACK);
  glDisable (GL_DITHER);
  glShadeModel (GL_SMOOTH);

  gdk_gl_drawable_gl_end (gldrawable);
  }

return TRUE;
}

GtkWidget *gamgi_mesa_area_create (gamgi_window *window)
{
GdkGLContext *context;
GdkGLConfig *config;
GtkWidget *area;

if (gamgi->window_start->area == NULL)
  {
  /***********************************************************
   * first GL area: create suitable GL config or return NULL *
   ***********************************************************/

  context = NULL;
  config = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB | 
  GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE);
  if (config == NULL) return NULL;
  }
else
  {
  /******************************************************************
   * other GL areas: at least a top window must exist, so get its   *
   * context to share lists, instead of using the context of the    *
   * first window created, which may no longer exist at this stage! *
   ******************************************************************/

  context = gtk_widget_get_gl_context (gamgi->window_start->area);
  config = gdk_gl_context_get_gl_config (context);
  }

area = gtk_drawing_area_new ();
gtk_widget_set_gl_capability (area, config, context, TRUE, GDK_GL_RGBA_TYPE);

/************************************************************
 * Events for widget must be set before X Window is created *
 ************************************************************/

gtk_widget_set_events (GTK_WIDGET (area), GDK_EXPOSURE_MASK | 
GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

window->begin_x = 0.0;
window->begin_y = 0.0;

/*************************************
 * Redraw image when window exposed. *
 *************************************/

g_signal_connect (area, "expose_event",
G_CALLBACK (gamgi_mesa_area_expose), window);

/**************************************
 * Handle mouse motion in the gl_area *
 **************************************/

g_signal_connect (area, "motion_notify_event",
G_CALLBACK (gamgi_mesa_area_notify), window);

/*************************************
 * Handle mouse press in the gl_area *
 *************************************/

g_signal_connect (area, "button_press_event",
G_CALLBACK (gamgi_mesa_area_press), window);

g_signal_connect (area, "button_release_event",
G_CALLBACK (gamgi_mesa_area_release), window);

/*************************************************************
 * When window is resized viewport needs to be resized also. *
 *************************************************************/

g_signal_connect (area, "configure_event",
G_CALLBACK (gamgi_mesa_area_configure), window);

/****************************************************
 * Do initialization when widget has been realized. *
 ****************************************************/

g_signal_connect (area, "realize",
G_CALLBACK (gamgi_mesa_area_start), window);
  
gtk_widget_show (GTK_WIDGET (area));

return area;
}
