/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_list.h
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

gamgi_slist *gamgi_engine_slist_add_start (gamgi_slist *slist_start);

gamgi_slist *gamgi_engine_slist_add_end (gamgi_slist *slist_start);

gamgi_slist *gamgi_engine_slist_remove_start (gamgi_slist *slist_start);

void gamgi_engine_slist_remove_all (gamgi_slist *slist_start);

gamgi_dlist *gamgi_engine_dlist_add_start (gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_add_end (gamgi_dlist *dlist_start);

gamgi_dlist *gamgi_engine_dlist_remove_start (gamgi_dlist *dlist_start);

void gamgi_engine_dlist_remove_all (gamgi_dlist *dlist_start);

gamgi_slist *gamgi_engine_slist_merge (gamgi_slist *slist_a,
gamgi_slist *slist_b, int (* function) (const void *, const void *));

gamgi_dlist *gamgi_engine_dlist_merge (gamgi_dlist *dlist_a,
gamgi_dlist *dlist_b, int (* function) (const void *, const void *));

gamgi_slist *gamgi_engine_slist_msort (gamgi_slist *slist_c,
int (* function) (const void *, const void *));

gamgi_dlist *gamgi_engine_dlist_msort (gamgi_dlist *dlist_c,
int (* function) (const void *, const void *));

gamgi_slist *gamgi_engine_slist_invert (gamgi_slist *start);

gamgi_dlist *gamgi_engine_dlist_invert (gamgi_dlist *start);

gamgi_slist *gamgi_engine_slist_copy (gamgi_slist *start);

gamgi_dlist *gamgi_engine_dlist_copy (gamgi_dlist *start);

int gamgi_engine_slist_nodes (gamgi_slist *start);

int gamgi_engine_dlist_nodes (gamgi_dlist *start);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
