/*******************************************
 *
 * $GAMGI/src/gtk/gamgi_gtk_dialog.h
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_gtk_dialog_remove (GtkWidget *widget, void *data);

int gamgi_gtk_dialog_delete (GtkWidget *widget,
GdkEventButton *event, void *data);

void gamgi_gtk_dialog_global_remove (void);

void gamgi_gtk_dialog_task0_remove (GtkWidget *widget, void *data);

void gamgi_gtk_dialog_task1_remove (GtkWidget *widget, void *data);

int gamgi_gtk_dialog_task0_delete (GtkWidget *widget, 
GdkEventButton *event, void *data);

int gamgi_gtk_dialog_task1_delete (GtkWidget *widget, 
GdkEventButton *event, void *data);

void gamgi_gtk_dialog_color (GdkColor *color, 
float red, float green, float blue);

void gamgi_gtk_dialog_color_base (GtkWidget *widget, GdkColor color);

void gamgi_gtk_dialog_color_bg (GtkWidget *widget, GdkColor color);

void gamgi_gtk_dialog_color_fg (GtkWidget *widget, GdkColor color);

void gamgi_gtk_dialog_color_default (GtkWidget *widget, gamgi_window *window);

void gamgi_gtk_dialog_beep (void);

int gamgi_gtk_dialog_button_mask (int button);

int gamgi_gtk_dialog_button_unmask (int mask);

GtkWidget *gamgi_gtk_dialog_button_create (char *name_label, char *name_color);

void gamgi_gtk_dialog_titles_scan (GtkCellLayout *layout,
GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, void *data);

void gamgi_gtk_dialog_sensitive_scan (GtkCellLayout *layout,
GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, void *data);

gamgi_bool *gamgi_gtk_dialog_sensitive_create (int rows);

int gamgi_gtk_dialog_page (gamgi_window *window);

void gamgi_gtk_dialog_message_create (char *title, char *message, 
void *data);

void gamgi_gtk_dialog_question_create (char *title, char *question,
gamgi_callback2 function_ok, void *data);

GtkWidget *gamgi_gtk_dialog_text_create (void);

void gamgi_gtk_dialog_text_insert (GtkTextBuffer *buffer,
GtkTextIter *iter, char *size, char *color, char *raw, ...);

GtkWidget *gamgi_gtk_dialog_task0_create (char *title, void *data);

GtkWidget *gamgi_gtk_dialog_task1_create (char *title, void *data);

GtkWidget *gamgi_gtk_dialog_help_create (char *title, void *data);

GtkWidget *gamgi_gtk_dialog_report_create (GtkWidget *text,
char *title, gamgi_enum action, gamgi_window *window);

/*************
 * Rationale *
 *************

Implemented but needs cleaning.

 *************
 * Rationale *
 *************/
