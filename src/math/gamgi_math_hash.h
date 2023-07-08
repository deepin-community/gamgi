/******************************************
 *
 * $GAMGI/src/math/gamgi_math_hash.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * external functions *
 **********************/

gamgi_slist **gamgi_math_hash_create (unsigned int size);

void gamgi_math_hash_dump (gamgi_slist **hash, unsigned int size);

void gamgi_math_hash_remove (gamgi_slist **hash, unsigned int size);

unsigned int gamgi_math_hash_value (const char *name,
unsigned int multiplier, unsigned int size);

gamgi_slist *gamgi_math_hash_add (gamgi_slist *slist, void *data);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
