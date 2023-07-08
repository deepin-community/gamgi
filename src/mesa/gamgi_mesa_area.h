/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_area.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static gamgi_window *static_window (void);

static gamgi_bool static_press (gamgi_window *window_mouse, int x, int y);

 **********************
 * external functions *
 **********************/

void gamgi_mesa_area_project (gamgi_window *window, double x,
double y, double z, double *x_2d, double *y_2d, double *z_2d);

void gamgi_mesa_area_unproject (gamgi_window *window,
int x_2d, int y_2d, double *x, double *y, double *z);

unsigned char *gamgi_mesa_area_read (int x,      
int y, int width, int height);

gamgi_bool gamgi_mesa_area_configure (GtkWidget *widget, 
GdkEventConfigure *event, void *data);

gamgi_bool gamgi_mesa_area_expose (GtkWidget *widget, 
GdkEventExpose *event, void *data);

gamgi_bool gamgi_mesa_area_press (GtkWidget *widget, 
GdkEventButton *event, void *data);

gamgi_bool gamgi_mesa_area_release (GtkWidget *widget,
GdkEventButton *event, void *data);

gamgi_bool gamgi_mesa_area_notify (GtkWidget *widget, 
GdkEventMotion *event, void *data);

gamgi_bool gamgi_mesa_area_start (GtkWidget *widget, void *data);

GtkWidget *gamgi_mesa_area_create (gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented.

 *************
 * Rationale *
 *************/
