/**********************************************
 *
 * $GAMGI/src/global/gamgi_global_selection.h
 *
 * Copyright (C) 2009 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

void gamgi_global_selection_text (gamgi_text *parent, int mark);

void gamgi_global_selection_group (gamgi_group *parent, int mark);

void gamgi_global_selection_cluster (gamgi_cluster *parent, int mark);

void gamgi_global_selection_assembly (gamgi_assembly *parent, int mark);

gamgi_dlist *gamgi_global_selection_list (gamgi_enum class);

void gamgi_global_selection_layer (void);

void gamgi_global_selection_focus (void);

void gamgi_global_selection_undo (void);

void gamgi_global_selection_hide (char *name, gamgi_window *window);

void gamgi_global_selection_show (char *name, gamgi_window *window);

void gamgi_global_selection_update (gamgi_dlist **start,
char *label, gamgi_window *window);

void gamgi_global_selection_update_objects (gamgi_window *window);

void gamgi_global_selection_update_windows (gamgi_dlist **start, char *label);

void gamgi_global_selection_link (gamgi_dlist *start);

void gamgi_global_selection_unlink (gamgi_dlist *start);

void gamgi_global_selection_remove (gamgi_dlist **start, char *label);

void gamgi_global_selection_remove_all (void);

void gamgi_global_selection_clean (gamgi_enum class,
gamgi_dlist **start, char *label, gamgi_dlist *selection);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
