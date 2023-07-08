/***********************************************
 *
 * $GAMGI/src/gtk/text/gamgi_gtk_text_config.h
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

static void static_ok (GtkWidget *widget, void *data);

static void static_init (gamgi_window *window);

 **********************
 * external functions *
 **********************/

GtkListStore *gamgi_gtk_text_config_wired (GtkWidget *combo);

GtkListStore *gamgi_gtk_text_config_solid (GtkWidget *combo);

GtkListStore *gamgi_gtk_text_config_all (GtkWidget *combo);

void gamgi_gtk_text_config (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Cleaned.

 *************
 * Rationale *
 *************/
