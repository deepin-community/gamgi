/*************************************************
 *
 * $GAMGI/src/gtk/light/gamgi_gtk_light_select.h
 *
 * Copyright (C) 2004, 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static void static_destroy (GtkWidget *widget, void *data);

static void static_model (gamgi_dlist *dlist, gamgi_window *window);

static gamgi_dlist *static_dlist (int scope, gamgi_window *window);

static void static_scope (GtkWidget *widget, void *data);

static void static_method_type (char *label, gamgi_window *window);

static void static_method (GtkWidget *widget, void *data);

static void static_reset (gamgi_window *window);

static gamgi_dlist *static_ok_name (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_parent (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_enum class, gamgi_window *window);

static gamgi_dlist *static_ok_type (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_type (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_type (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_type (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_direction_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_direction_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_direction_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_position_x (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_position_y (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_position_z (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_angle (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_ambient_r (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_ambient_g (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_ambient_b (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_diffuse_r (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_diffuse_g (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_diffuse_b (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_specular_r (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_specular_g (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_specular_b (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_constant (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_linear (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_quadratic (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static gamgi_dlist *static_ok_radial (gamgi_dlist *dlist_old,
gamgi_bool action, gamgi_window *window);

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

static void static_press (gamgi_object *object, void *data);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_light_select_press (gamgi_window *window_mouse,
GdkEventButton *event, int x, int y, gamgi_window *window_dialog);

void gamgi_gtk_light_select (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Implemented. Nov 2009.

 *************
 * Rationale *
 *************/
