/******************************************
 *
 * $GAMGI/src/math/gamgi_math_hash.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

#include "gamgi_engine_list.h"

gamgi_slist **gamgi_math_hash_create (unsigned int size)
{
gamgi_slist **table;

table = (gamgi_slist **) malloc (size * sizeof (gamgi_slist *));

while (size-- > 0) table[size] = NULL;
 
return table;
}

void gamgi_math_hash_remove (gamgi_slist **hash, unsigned int size)
{
gamgi_slist *slist;

while (size-- > 0)
  {
  slist = hash[size];

  while (slist != NULL)
    slist = gamgi_engine_slist_remove_start (slist);
  }

free (hash);
}

unsigned int gamgi_math_hash_value (const char *name,
unsigned int multiplier, unsigned int size)
{
unsigned int hash;
unsigned char *pointer;

hash = 0;

for (pointer = (unsigned char *) name; *pointer != '\0'; pointer++)
  hash = multiplier * hash + *pointer;

return hash % size;
}

gamgi_slist *gamgi_math_hash_add (gamgi_slist *slist, void *data)
{
slist = gamgi_engine_slist_add_start (slist);
slist->data = data;

return slist;
}
