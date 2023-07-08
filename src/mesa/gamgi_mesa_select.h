/*******************************************
 *
 * $GAMGI/src/mesa/gamgi_mesa_select.h
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

static void static_select (gamgi_object *object, void *data);

static void static_apply (gamgi_object *object, void *data);

static gamgi_slist *static_color (gamgi_slist *slist_old, int color_new);

static void static_start (void);

static gamgi_slist *static_pick_pixels (int x, int y);

static gamgi_slist *static_grab_pixels (int x, int y, int width, int height,
gamgi_object **objects, int *levels, int level, gamgi_slist *slist);

static gamgi_object *static_pick_objects (int x, int y, int class,
gamgi_bool global, gamgi_window *window);

gamgi_slist *static_grab_objects (int *box, gamgi_enum class,
gamgi_window *window);

static void static_activate (GtkWidget *widget, void *data);

static GtkWidget *static_item (GtkWidget *menu,
gamgi_object *object, gamgi_window *window);

static void static_window (gamgi_window *window, gamgi_window *window_start);

static void static_menu (gamgi_window *window, gamgi_dlist *dlist_start);

static void static_light (gamgi_window *window, gamgi_bool global);

static void static_recursive (int x, int y, int class, gamgi_bool global,
gamgi_window *window);

static gamgi_bool static_box (int *box, gamgi_window *window);

 **********************
 * external functions *
 **********************/

gamgi_window *gamgi_mesa_select_window (void);

gamgi_bool gamgi_mesa_select_action (gamgi_window *window_mouse, 
GdkEventButton *event, int x, int y);

void gamgi_mesa_select_old (unsigned char *color);

gamgi_bool gamgi_mesa_select_new (unsigned char *color);

void gamgi_mesa_select_object (gamgi_window *window, int x, int y, 
gamgi_enum class, gamgi_bool global, gamgi_function2 function);

gamgi_slist *gamgi_mesa_select_region (int *box, gamgi_enum class,
gamgi_window *window_mouse, gamgi_window *window_dialog);

void gamgi_mesa_select_reset (gamgi_window *window);

void gamgi_mesa_select_state (char *name, gamgi_window *window);

void gamgi_mesa_select_draw (gamgi_window *window);

void gamgi_mesa_select_press (int x, int y, gamgi_window *window);

void gamgi_mesa_select_release (int x, int y, gamgi_window *window);

void gamgi_mesa_select_notify (int x, int y, gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented. 

 *************
 * Rationale *
 *************/
