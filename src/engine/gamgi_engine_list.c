/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_list.c  
 *                                         
 * Copyright (C) 2001, 2004 Carlos Pereira  
 *                                    
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"

/***************** external function ********************
 *                                                      *
 *            GAMGI_ENGINE_SLIST_ADD_START              *
 *                                                      *
 * Add a new slist node before the slist_start node and *
 * returns the address of the new node. This is usefull *
 * to add new elements to stacks, but cannot be used to *
 * add new elements in the middle of a slist!           *
 ********************************************************/

gamgi_slist *gamgi_engine_slist_add_start (gamgi_slist *slist_start)
{
gamgi_slist *slist_new;

slist_new = GAMGI_CAST_SLIST malloc (sizeof (gamgi_slist));

slist_new->next = slist_start;

return slist_new;
}

/**************** external function *********************
 *                                                      *
 *            GAMGI_ENGINE_SLIST_ADD_END                *
 *                                                      *
 * Add a new slist node after the slist_end node and    *
 * returns the address of the new node. This is usefull *
 * to add new elements to queues, but can also be used  *
 * to add new elements in the middle of a slist!        *
 ********************************************************/

gamgi_slist *gamgi_engine_slist_add_end (gamgi_slist *slist_end)
{
gamgi_slist *slist_new;

slist_new = GAMGI_CAST_SLIST malloc (sizeof (gamgi_slist));

if (slist_end != NULL)
  {
  slist_new->next = slist_end->next;
  slist_end->next = slist_new;
  }
else slist_new->next = NULL;

return slist_new;
}

/******************* external function ******************
 *                                                      *
 *            GAMGI_ENGINE_SLIST_REMOVE_START           *
 *                                                      *
 * Remove the slist_start node and returns the address  *
 * of the next node. This is usefull to remove elements *
 * from both stacks and queues, but cannot be used to   *
 * remove elements in the middle of a slist!            *
 *                                                      *
 *              slist_start MUST be non null!           *
 ********************************************************/

gamgi_slist *gamgi_engine_slist_remove_start (gamgi_slist *slist_start)
{
gamgi_slist *slist_old;

slist_old = slist_start->next;

free (slist_start);

return slist_old;
}

/********************* external function *********************
 *                                                           *
 *               GAMGI_ENGINE_SLIST_REMOVE_ALL               *
 *                                                           *
 * Remove all elements of a slist, from slist_start to NULL! *
 *************************************************************/

void gamgi_engine_slist_remove_all (gamgi_slist *slist_start)
{
while (slist_start != NULL)
  slist_start = gamgi_engine_slist_remove_start (slist_start);
}

/*************** external function **********************
 *                                                      *
 *            GAMGI_ENGINE_DLIST_ADD_START              *
 *                                                      *
 * Add a new dlist node before the dlist_start node and *
 * returns the address of the new node. This is usefull *
 * to add new elements to stacks, but can also be used  *
 * to add new elements in the middle of a dlist!        *
 *                                                      *
 *  if dlist_start->before is NULL then dlist_start is  *
 *  the start, otherwise is in the middle of the dlist! *
 ********************************************************/

gamgi_dlist *gamgi_engine_dlist_add_start (gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist_new;

dlist_new = GAMGI_CAST_DLIST malloc (sizeof (gamgi_dlist));

dlist_new->next = dlist_start;
if (dlist_start != NULL)
  {
  dlist_new->before = dlist_start->before;
  if (dlist_start->before != NULL) dlist_start->before->next = dlist_new;
  dlist_start->before = dlist_new;
  }
else dlist_new->before = NULL;

return dlist_new;
}

/**************** external function *********************
 *                                                      *
 *            GAMGI_ENGINE_DLIST_ADD_END                *
 *                                                      *
 * Add a new dlist node after the dlist_end node and    *
 * returns the address of the new node. This is usefull *
 * to add new elements to queues, but can also be used  *
 * to add new elements in the middle of a dlist!        *
 *                                                      *
 *  if dlist_end->next is NULL then dlist_end is the    *
 *    end, otherwise is in the middle of the dlist!     *
 ********************************************************/

gamgi_dlist *gamgi_engine_dlist_add_end (gamgi_dlist *dlist_end)
{
gamgi_dlist *dlist_new;

dlist_new = GAMGI_CAST_DLIST malloc (sizeof (gamgi_dlist));

dlist_new->before = dlist_end;
if (dlist_end != NULL)
  {
  dlist_new->next = dlist_end->next;
  if (dlist_end->next != NULL) dlist_end->next->before = dlist_new;
  dlist_end->next = dlist_new;
  }
else dlist_new->next = NULL;

return dlist_new;
}

/****************** external function *******************
 *                                                      *
 *            GAMGI_ENGINE_DLIST_REMOVE_START           *
 *                                                      *
 * Remove the dlist_start node and returns the address  *
 * of the next node. This is usefull to remove elements *
 * from both stacks and queues, but can also be used to *
 * remove elements in the middle of a dlist!            *
 *                                                      *
 *       By-pass dlist_start, linking directly          *
 *        left and right nodes to each other!           *
 ********************************************************/

gamgi_dlist *gamgi_engine_dlist_remove_start (gamgi_dlist *dlist_start)
{
gamgi_dlist *dlist_old;

dlist_old = dlist_start->next;
if (dlist_old != NULL) dlist_old->before = dlist_start->before;
if (dlist_start->before != NULL) dlist_start->before->next = dlist_old;

free (dlist_start);

return dlist_old;
}

/******************** external function **********************
 *                                                           *
 *            GAMGI_ENGINE_DLIST_REMOVE_ALL                  *
 *                                                           *
 * Remove all elements of a dlist, from dlist_start to NULL! *
 *************************************************************/

void gamgi_engine_dlist_remove_all (gamgi_dlist *dlist_start)
{
while (dlist_start != NULL)
  dlist_start = gamgi_engine_dlist_remove_start (dlist_start);
}

/*********************** external function *********************
 *                                                             *
 *                    GAMGI_ENGINE_SLIST_MERGE                 *
 *                                                             *
 * Merge two ordered lists of slist nodes in a single ordered  *
 * list, as determined by an external comparing function. When *
 * the function returns positive, the two nodes are swaped.    *
 ***************************************************************/

gamgi_slist *gamgi_engine_slist_merge (gamgi_slist *a, 
gamgi_slist *b, int (* function) (const void *, const void *))
{
gamgi_slist start;
gamgi_slist *c = &start;

while (a != NULL && b != NULL)
  if ((*function) (a, b) > 0)
    { c->next = b; c = b; b = b->next; }
  else
    { c->next = a; c = a; a = a->next; }
c->next = (a == NULL) ? b : a;

return start.next;
}

/*********************** external function *********************
 *                                                             *
 *                    GAMGI_ENGINE_DLIST_MERGE                 *
 *                                                             *
 * Merge two ordered lists of dlist nodes in a single ordered  *
 * list, as determined by an external comparing function. When *
 * the function returns positive, the two nodes are swaped.    *
 ***************************************************************/

gamgi_dlist *gamgi_engine_dlist_merge (gamgi_dlist *a,
gamgi_dlist *b, int (* function) (const void *, const void *))
{
gamgi_dlist start;
gamgi_dlist *c = &start;

while (a != NULL && b != NULL)
  if ((*function) (a, b) > 0)
    { c->next = b; b->before = c; c = b; b = b->next; }
  else
    { c->next = a; a->before = c; c = a; a = a->next; }
c->next = (a == NULL) ? b : a;

return start.next;
}

/*********************** external function ******************
 *                                                          *
 *                    GAMGI_ENGINE_SLIST_SORT               *
 *                                                          *
 * Use the merge-sort recursive algorithm to sort a list of *
 * slist nodes, according to an external comparing function *
 ************************************************************/

gamgi_slist *gamgi_engine_slist_msort (gamgi_slist *c,
int (* function) (const void *, const void *))
{
gamgi_slist *a, *b;

if (c == NULL || c->next == NULL) return c;

/****************************************************************
 *    divide list c in two new lists, a going from the start    *
 *    to the middle, and b going from the middle to the end     *
 *                                                              *
 * pointer b moves along the list at twice the speed of pointer *
 * c, so when b reaches the end of the list, c is at the middle *
 ****************************************************************/

a = c; b = c->next;
while (b != NULL && b->next != NULL)
  { c = c->next; b = b->next->next; }
b = c->next; c->next = NULL;

return gamgi_engine_slist_merge (gamgi_engine_slist_msort (a, function), 
gamgi_engine_slist_msort (b, function), function);
}

/*********************** external function ******************
 *                                                          *
 *                    GAMGI_ENGINE_DLIST_SORT               *
 *                                                          *
 * Use the merge-sort recursive algorithm to sort a list of *
 * dlist nodes, according to an external comparing function *
 ************************************************************/

gamgi_dlist *gamgi_engine_dlist_msort (gamgi_dlist *c,
int (* function) (const void *, const void *))
{
gamgi_dlist *a, *b;

if (c == NULL || c->next == NULL) return c;

/****************************************************************
 *    divide list c in two new lists, a going from the start    *
 *    to the middle, and b going from the middle to the end     *
 *                                                              *
 * pointer b moves along the list at twice the speed of pointer *
 * c, so when b reaches the end of the list, c is at the middle *
 ****************************************************************/

a = c; b = c->next;
while (b != NULL && b->next != NULL)
  { c = c->next; b = b->next->next; }
b = c->next; c->next = NULL;

return gamgi_engine_dlist_merge (gamgi_engine_dlist_msort (a, function),
gamgi_engine_dlist_msort (b, function), function);
}

/*********************** external function ********************
 *                                                            *
 *                   GAMGI_ENGINE_SLIST_INVERT                *
 *                                                            *
 * Invert the list, so the first node becomes the last and    *
 * vice-versa. The input should be the first node of a single *
 * list, and the output is the first node of the new list     *
 **************************************************************/

gamgi_slist *gamgi_engine_slist_invert (gamgi_slist *start)
{
gamgi_slist *slist, *slist_before, *slist_next;

slist_before = NULL;
for (slist = start; slist != NULL; slist = slist_next)
  {
  slist_next = slist->next;
  slist->next = slist_before;
  slist_before = slist;
  }

return slist_before;
}

/*********************** external function ********************
 *                                                            *
 *                   GAMGI_ENGINE_DLIST_INVERT                *
 *                                                            *
 * Invert the list, so the first node becomes the last and    *
 * vice-versa. The input should be the first node of a double *
 * list, and the output is the first node of the new list     *
 **************************************************************/

gamgi_dlist *gamgi_engine_dlist_invert (gamgi_dlist *start)
{
gamgi_dlist *dlist, *dlist_before;

dlist_before = NULL;
for (dlist = start; dlist != NULL; dlist = dlist->before)
  {
  dlist_before = dlist->before;
  dlist->before = dlist->next;
  dlist->next = dlist_before;

  dlist_before = dlist;
  }

return dlist_before;
}

gamgi_slist *gamgi_engine_slist_copy (gamgi_slist *start)
{
gamgi_slist *slist, *slist_new, *start_new;

if (start == NULL) return NULL;

start_new = gamgi_engine_slist_add_end (NULL);
start_new->data = start->data;

slist_new = start_new;
for (slist = start->next; slist != NULL; slist = slist->next)
  {
  slist_new = gamgi_engine_slist_add_end (slist_new);
  slist_new->data = slist->data;
  }

return start_new;
}

gamgi_dlist *gamgi_engine_dlist_copy (gamgi_dlist *start)
{
gamgi_dlist *dlist, *dlist_new, *start_new;

if (start == NULL) return NULL;

start_new = gamgi_engine_dlist_add_end (NULL);
start_new->data = start->data;

dlist_new = start_new;
for (dlist = start->next; dlist != NULL; dlist = dlist->next)
  {
  dlist_new = gamgi_engine_dlist_add_end (dlist_new);
  dlist_new->data = dlist->data;
  }

return start_new;
}

int gamgi_engine_slist_nodes (gamgi_slist *start)
{
gamgi_slist *slist;
int i = 0;

for (slist = start; slist != NULL; slist = slist->next) i++;

return i;
}

int gamgi_engine_dlist_nodes (gamgi_dlist *start)
{
gamgi_dlist *dlist;
int i = 0;

for (dlist = start; dlist != NULL; dlist = dlist->next) i++;

return i;
}
