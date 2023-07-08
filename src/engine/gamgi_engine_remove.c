/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_remove.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_chem.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_engine_remove_out.h"
#include "gamgi_engine_number.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_gtk_history.h"
#include "gamgi_mesa_text.h"
#include "gamgi_math_hash.h"
#include "gamgi_global_selection.h"
#include "gamgi_global_remove.h"

/***************** external function *********************
 *                                                       *
 *             GAMGI_ENGINE_REMOVE_TEXT                  *
 *                                                       *
 *  Remove all objects which belong to text and remove   *
 * the text. The function calls itself when the child is *
 * also text. Caution: the text must be unlinked before! *
 *********************************************************/

void gamgi_engine_remove_text (gamgi_text *text) 
{
gamgi_dlist *dlist;
gamgi_text *text_next, *text_before;

/********************************
 * remove text global selection *
 ********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_TEXT,
&gamgi->texts, "button_text", text->object.selection);

/********************
 * remove text list *
 ********************/

dlist = text->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 *  free text contents   *
 *                       *
 * unref font, remove it *
 * when no longer needed *
 *************************/

gamgi_mesa_text_font_decrease (text);
text->font = FALSE;
gamgi_mesa_text_remove (text);

/***************
 * remove text *
 ***************/

/*********************************************
 * because recursive removing may occur,     *
 * addressing can be done only after all     *
 * child objects are removed from the lists. *
 *********************************************/

text_next = text->next;
text_before = text->before;

gamgi_engine_number_text_recycle (text->object.number);
gamgi_engine_remove_out_text (text);

if (text_next == NULL) gamgi->text_end = text_before;
if (text_before == NULL) gamgi->text_start = text_next;
}

/******************** external function **********************
 *                                                           *
 *              GAMGI_ENGINE_REMOVE_ORBITAL                  *
 *                                                           *
 * Remove all objects which belong to orbital and remove the *
 *  orbital. Caution: the orbital must be unlinked before!   *
 *************************************************************/

void gamgi_engine_remove_orbital (gamgi_orbital *orbital) 
{
gamgi_dlist *dlist;
gamgi_orbital *orbital_next, *orbital_before;

/***********************************
 * remove orbital global selection *
 ***********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_ORBITAL,
&gamgi->orbitals, "button_orbital", orbital->object.selection);

/********************
 * remove text list *
 ********************/

dlist = orbital->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * free array memory *
 *********************/

free (orbital->points);
free (orbital->lines);

/******************
 * remove orbital *
 ******************/

orbital_next = orbital->next;
orbital_before = orbital->before;

gamgi_engine_number_orbital_recycle (orbital->object.number);
gamgi_engine_remove_out_orbital (orbital);

if (orbital_next == NULL) gamgi->orbital_end = orbital_before;
if (orbital_before == NULL) gamgi->orbital_start = orbital_next;
}

/***************** external function ***************
 *                                                 *
 *              GAMGI_ENGINE_REMOVE_BOND           *
 *                                                 *
 * Remove all objects which belong to bond, unlink *
 *  from the two parent atoms and remove the bond  *
 *                                                 *
 * Caution: the bond must be unlinked before!      *
 ***************************************************/

void gamgi_engine_remove_bond (gamgi_bond *bond)
{
gamgi_dlist *dlist;
gamgi_bond *bond_next, *bond_before;

/********************************
 * remove bond global selection *
 ********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_BOND,
&gamgi->bonds, "button_bond", bond->object.selection);

/********************
 * remove text list *
 ********************/

dlist = bond->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = bond->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***************
 * remove bond *
 ***************/

bond_next = bond->next;
bond_before = bond->before;

gamgi_engine_number_bond_recycle (bond->object.number);
gamgi_engine_remove_out_bond (bond);

if (bond_next == NULL) gamgi->bond_end = bond_before;
if (bond_before == NULL) gamgi->bond_start = bond_next;
}

/***************** external function ********************
 *                                                      *
 *              GAMGI_ENGINE_REMOVE_ATOM                *
 *                                                      *
 * Remove all objects which belong to atom and remove   *
 * the atom. Caution: the atom must be unlinked before! *
 ********************************************************/

void gamgi_engine_remove_atom (gamgi_atom *atom) 
{
gamgi_dlist *dlist;
gamgi_atom *atom_next, *atom_before;
gamgi_bond *bond;

/********************************
 * remove atom global selection *
 ********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_ATOM,
&gamgi->atoms, "button_atom", atom->object.selection);

/********************
 * remove text list *
 ********************/

dlist = atom->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = atom->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove bond list *
 ********************/

dlist = atom->bond_start;
while (dlist != NULL)
  { bond = GAMGI_CAST_BOND dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_bond (bond);
    gamgi_engine_remove_bond (bond); }

/***************
 * remove atom *
 ***************/

atom_next = atom->next;
atom_before = atom->before;

gamgi_engine_number_atom_recycle (atom->object.number);
gamgi_engine_remove_out_atom (atom);

if (atom_next == NULL) gamgi->atom_end = atom_before;
if (atom_before == NULL) gamgi->atom_start = atom_next;
}

/******************** external function ************************
 *                                                             *
 *               GAMGI_ENGINE_REMOVE_DIRECTION                 *
 *                                                             *
 * Remove all objects which belong to direction and remove the *
 *  direction. Caution: the direction must be unlinked before! *
 ***************************************************************/

void gamgi_engine_remove_direction (gamgi_direction *direction) 
{
gamgi_dlist *dlist;
gamgi_direction *direction_next, *direction_before;

/*************************************
 * remove direction global selection *
 *************************************/

gamgi_global_selection_clean (GAMGI_ENGINE_DIRECTION,
&gamgi->directions, "button_direction", direction->object.selection);

/********************
 * remove text list *
 ********************/

dlist = direction->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = direction->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = direction->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * free array memory *
 *********************/

free (direction->points);
free (direction->loops);

/********************
 * remove direction *
 ********************/

direction_next = direction->next;
direction_before = direction->before;

gamgi_engine_number_direction_recycle (direction->object.number);
gamgi_engine_remove_out_direction (direction);

if (direction_next == NULL) gamgi->direction_end = direction_before;
if (direction_before == NULL) gamgi->direction_start = direction_next;
}

/******************** external function *******************
 *                                                        *
 *                GAMGI_ENGINE_REMOVE_PLANE               *
 *                                                        *
 *  Remove all objects which belong to plane and remove   *
 * the plane. Caution: the plane must be unlinked before! *
 **********************************************************/

void gamgi_engine_remove_plane (gamgi_plane *plane) 
{
gamgi_dlist *dlist;
gamgi_plane *plane_next, *plane_before;

/********************************
 * remove plane global selection *
 ********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_PLANE,
&gamgi->planes, "button_plane", plane->object.selection);

/********************
 * remove text list *
 ********************/

dlist = plane->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = plane->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = plane->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = plane->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * free array memory *
 *********************/

free (plane->points);
free (plane->loops);

/****************
 * remove plane *
 ****************/

plane_next = plane->next;
plane_before = plane->before;

gamgi_engine_number_plane_recycle (plane->object.number);
gamgi_engine_remove_out_plane (plane);

if (plane_next == NULL) gamgi->plane_end = plane_before;
if (plane_before == NULL) gamgi->plane_start = plane_next;
}

/******************* external function *********************
 *                                                         *
 *               GAMGI_ENGINE_REMOVE_GROUP                 *
 *                                                         *
 *  Remove all objects which belong to group and remove    *
 * the group. The function calls itself when the child is  *
 * also group. Caution: the group must be unlinked before! *
 ***********************************************************/

void gamgi_engine_remove_group (gamgi_group *group) 
{
gamgi_dlist *dlist;
gamgi_group *group_next, *group_before;

/*********************************
 * remove group global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_GROUP,
&gamgi->groups, "button_group", group->object.selection);

/********************
 * remove text list *
 ********************/

dlist = group->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = group->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = group->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = group->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

dlist = group->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = group->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/****************
 * remove group *
 ****************/

/*********************************************
 * because recursive removing may occur,     *
 * addressing can be done only after all     *
 * child objects are removed from the lists. *
 *********************************************/

group_next = group->next;
group_before = group->before;

gamgi_engine_number_group_recycle (group->object.number);
gamgi_engine_remove_out_group (group);

if (group_next == NULL) gamgi->group_end = group_before;
if (group_before == NULL) gamgi->group_start = group_next;

}

/********************** external function *********************
 *                                                            *
 *                GAMGI_ENGINE_REMOVE_MOLECULE                *
 *                                                            *
 * Remove all objects which belong to molecule and remove the * 
 *  molecule. Caution: the molecule must be unlinked before!  *
 **************************************************************/

void gamgi_engine_remove_molecule (gamgi_molecule *molecule) 
{
gamgi_dlist *dlist;
gamgi_molecule *molecule_next, *molecule_before;

/************************************
 * remove molecule global selection *
 ************************************/

gamgi_global_selection_clean (GAMGI_ENGINE_MOLECULE,
&gamgi->molecules, "button_molecule", molecule->object.selection);

/********************
 * remove text list *
 ********************/

dlist = molecule->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

/* remove orbital list */
dlist = molecule->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = molecule->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = molecule->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

/* remove plane list */
dlist = molecule->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = molecule->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*******************
 * remove molecule *
 *******************/

molecule_next = molecule->next;
molecule_before = molecule->before;

gamgi_engine_number_molecule_recycle (molecule->object.number);
gamgi_engine_remove_out_molecule (molecule);

if (molecule_next == NULL) gamgi->molecule_end = molecule_before;
if (molecule_before == NULL) gamgi->molecule_start = molecule_next;

}

/************************ external function ******************
 *                                                           *
 *                   GAMGI_ENGINE_REMOVE_CLUSTER             *
 *                                                           *
 * Remove all objects which belong to cluster and remove the *
 * cluster. The function calls itself when the child is also *
 *  cluster. Caution: the cluster must be unlinked before!   *
 *************************************************************/

void gamgi_engine_remove_cluster (gamgi_cluster *cluster) 
{
gamgi_dlist *dlist;
gamgi_cluster *cluster_next, *cluster_before; 

/***********************************
 * remove cluster global selection *
 ***********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_CLUSTER,
&gamgi->clusters, "button_cluster", cluster->object.selection);

/********************
 * remove text list *
 ********************/

dlist = cluster->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = cluster->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = cluster->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = cluster->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

dlist = cluster->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = cluster->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove molecule list *
 ************************/

dlist = cluster->molecule_start;
while (dlist != NULL)
  { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove clustier list *
 ************************/

dlist = cluster->cluster_start;
while (dlist != NULL)
  { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * free array memory *
 *********************/

free (cluster->points);
free (cluster->loops);
free (cluster->colors);
free (cluster->paints);

/******************
 * remove cluster *
 ******************/

/*********************************************
 * because recursive removing may occur,     *
 * addressing can be done only after all     *
 * child objects are removed from the lists. *
 *********************************************/

cluster_next = cluster->next;
cluster_before = cluster->before;

gamgi_engine_number_cluster_recycle (cluster->object.number);
gamgi_engine_remove_out_cluster (cluster);

if (cluster_next == NULL) gamgi->cluster_end = cluster_before;
if (cluster_before == NULL) gamgi->cluster_start = cluster_next;
}

/******************** external function *****************
 *                                                      *
 *                  GAMGI_ENGINE_REMOVE_CELL            *
 *                                                      *
 * Remove all objects which belong to cell and remove   *
 * the cell. Caution: the cell must be unlinked before! *
 ********************************************************/

void gamgi_engine_remove_cell (gamgi_cell *cell) 
{
gamgi_dlist *dlist;
gamgi_cell *cell_next, *cell_before;

/********************************
 * remove cell global selection *
 ********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_CELL,
&gamgi->cells, "button_cell", cell->object.selection);

/********************
 * remove text list *
 ********************/

dlist = cell->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = cell->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = cell->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = cell->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

dlist = cell->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = cell->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove molecule list *
 ************************/

/* remove molecule list */
dlist = cell->molecule_start;
while (dlist != NULL)
  { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove cluster list *
 ***********************/

/* remove cluster list */
dlist = cell->cluster_start;
while (dlist != NULL)
  { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * free array memory *
 *********************/

free (cell->points);
free (cell->loops);
free (cell->lines);

/***************
 * remove cell *
 ***************/

cell_next = cell->next;
cell_before = cell->before;

gamgi_engine_number_cell_recycle (cell->object.number);
gamgi_engine_remove_out_cell (cell);

if (cell_next == NULL) gamgi->cell_end = cell_before;
if (cell_before == NULL) gamgi->cell_start = cell_next;
}

/******************** external function *******************
 *                                                        *
 *               GAMGI_ENGINE_REMOVE_ARROW                *
 *                                                        *
 *  Remove all objects which belong to arrow and remove   *
 * the arrow. Caution: the arrow must be unlinked before! *
 **********************************************************/

void gamgi_engine_remove_arrow (gamgi_arrow *arrow) 
{
gamgi_dlist *dlist;
gamgi_arrow *arrow_next, *arrow_before;

/*********************************
 * remove arrow global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_ARROW,
&gamgi->arrows, "button_arrow", arrow->object.selection);

/********************
 * remove text list *
 ********************/

dlist = arrow->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/****************
 * remove arrow *
 ****************/

arrow_next = arrow->next;
arrow_before = arrow->before;

gamgi_engine_number_arrow_recycle (arrow->object.number);
gamgi_engine_remove_out_arrow (arrow);

if (arrow_next == NULL) gamgi->arrow_end = arrow_before;
if (arrow_before == NULL) gamgi->arrow_start = arrow_next;
}

/******************** external function *******************
 *                                                        *
 *               GAMGI_ENGINE_REMOVE_SHAPE                *
 *                                                        *
 *  Remove all objects which belong to shape and remove   *
 * the shape. Caution: the shape must be unlinked before! *
 **********************************************************/

void gamgi_engine_remove_shape (gamgi_shape *shape) 
{
gamgi_dlist *dlist;
gamgi_shape *shape_next, *shape_before;

/*********************************
 * remove shape global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_SHAPE,
&gamgi->shapes, "button_shape", shape->object.selection);

/********************
 * remove text list *
 ********************/

dlist = shape->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/****************
 * remove shape *
 ****************/

shape_next = shape->next;
shape_before = shape->before;

gamgi_engine_number_shape_recycle (shape->object.number);
gamgi_engine_remove_out_shape (shape);

if (shape_next == NULL) gamgi->shape_end = shape_before;
if (shape_before == NULL) gamgi->shape_start = shape_next;
}

/******************** external function *******************
 *                                                        *
 *                GAMGI_ENGINE_REMOVE_GRAPH               *
 *                                                        *
 *  Remove all objects which belong to graph and remove   *
 * the graph. Caution: the graph must be unlinked before! *
 **********************************************************/

void gamgi_engine_remove_graph (gamgi_graph *graph) 
{
gamgi_dlist *dlist;
gamgi_graph *graph_next, *graph_before;

/*********************************
 * remove graph global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_GRAPH,
&gamgi->graphs, "button_graph", graph->object.selection);

/********************
 * remove text list *
 ********************/

dlist = graph->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/****************
 * remove graph *
 ****************/

graph_next = graph->next;
graph_before = graph->before;

gamgi_engine_number_graph_recycle (graph->object.number);
gamgi_engine_remove_out_graph (graph);

if (graph_next == NULL) gamgi->graph_end = graph_before;
if (graph_before == NULL) gamgi->graph_start = graph_next;
}

/************************ external function *******************
 *                                                            *
 *                  GAMGI_ENGINE_REMOVE_ASSEMBLY              *
 *                                                            *
 * Remove all objects which belong to assembly and remove the * 
 * assembly. The function calls itself when the child is also *
 *  assembly. Caution: the assembly must be unlinked before!  *
 **************************************************************/

void gamgi_engine_remove_assembly (gamgi_assembly *assembly) 
{
gamgi_dlist *dlist;
gamgi_assembly *assembly_next, *assembly_before;

/************************************
 * remove assembly global selection *
 ************************************/

gamgi_global_selection_clean (GAMGI_ENGINE_ASSEMBLY,
&gamgi->assemblys, "button_assembly", assembly->object.selection);

/********************
 * remove text list *
 ********************/

dlist = assembly->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = assembly->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = assembly->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = assembly->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

dlist = assembly->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = assembly->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove molecule list *
 ************************/

dlist = assembly->molecule_start;
while (dlist != NULL)
  { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove cluster list *
 ***********************/

/* remove cluster list */
dlist = assembly->cluster_start;
while (dlist != NULL)
  { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove cell list *
 ********************/

dlist = assembly->cell_start;
while (dlist != NULL)
  { gamgi_engine_remove_cell (GAMGI_CAST_CELL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove arrow list *
 *********************/

dlist = assembly->arrow_start;
while (dlist != NULL)
  { gamgi_engine_remove_arrow (GAMGI_CAST_ARROW dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove shape list *
 *********************/

dlist = assembly->shape_start;
while (dlist != NULL)
  { gamgi_engine_remove_shape (GAMGI_CAST_SHAPE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove graph list *
 *********************/

dlist = assembly->graph_start;
while (dlist != NULL)
  { gamgi_engine_remove_graph (GAMGI_CAST_GRAPH dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove assembly list *
 ************************/

dlist = assembly->assembly_start;
while (dlist != NULL)
  { gamgi_engine_remove_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*******************
 * remove assembly *
 *******************/

/*********************************************
 * because recursive removing may occur,     *
 * addressing can be done only after all     *
 * child objects are removed from the lists. *
 *********************************************/

assembly_next = assembly->next;
assembly_before = assembly->before;

gamgi_engine_number_assembly_recycle (assembly->object.number);
gamgi_engine_remove_out_assembly (assembly);

if (assembly_next == NULL) gamgi->assembly_end = assembly_before;
if (assembly_before == NULL) gamgi->assembly_start = assembly_next;
}

/********* external function ********
 *                                  *
 *     GAMGI_ENGINE_REMOVE_LIGHT    *
 *                                  *
 *  Remove the light. Caution: the  *
 *  light must be unlinked before!  *
 ************************************/

void gamgi_engine_remove_light (gamgi_light *light) 
{
gamgi_light *light_next, *light_before;

/*********************************
 * remove light global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_LIGHT,
&gamgi->lights, "button_light", light->object.selection);

/****************
 * remove light *
 ****************/

light_next = light->next;
light_before = light->before;

gamgi_engine_number_light_recycle (light->object.number);
gamgi_engine_remove_out_light (light);

if (light_next == NULL) gamgi->light_end = light_before;
if (light_before == NULL) gamgi->light_start = light_next;
}

/******************** external function *******************
 *                                                        *
 *               GAMGI_ENGINE_REMOVE_LAYER                *
 *                                                        *
 *  Remove all objects which belong to layer and remove   *
 * the layer. Caution: the layer must be unlinked before! *
 **********************************************************/

void gamgi_engine_remove_layer (gamgi_layer *layer) 
{
gamgi_dlist *dlist;
gamgi_layer *layer_next, *layer_before;

/**************************************
 * clean Undo,Save history mechanisms *
 **************************************/

if (layer->undo != NULL) gamgi_gtk_history_layer_reset (layer->undo);
if (layer->save != NULL) gamgi_gtk_history_layer_reset (layer->save);

/*********************************
 * remove layer global selection *
 *********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_LAYER,
&gamgi->layers, "button_layer", layer->object.selection);

/********************
 * remove text list *
 ********************/

dlist = layer->text_start;
while (dlist != NULL)
  { gamgi_engine_remove_text (GAMGI_CAST_TEXT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove orbital list *
 ***********************/

dlist = layer->orbital_start;
while (dlist != NULL)
  { gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove atom list *
 ********************/

dlist = layer->atom_start;
while (dlist != NULL)
  { gamgi_engine_remove_atom (GAMGI_CAST_ATOM dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*************************
 * remove direction list *
 *************************/

dlist = layer->direction_start;
while (dlist != NULL)
  { gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove plane list *
 *********************/

dlist = layer->plane_start;
while (dlist != NULL)
  { gamgi_engine_remove_plane (GAMGI_CAST_PLANE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove group list *
 *********************/

dlist = layer->group_start;
while (dlist != NULL)
  { gamgi_engine_remove_group (GAMGI_CAST_GROUP dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove molecule list *
 ************************/

dlist = layer->molecule_start;
while (dlist != NULL)
  { gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/***********************
 * remove cluster list *
 ***********************/

dlist = layer->cluster_start;
while (dlist != NULL)
  { gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/********************
 * remove cell list *
 ********************/

dlist = layer->cell_start;
while (dlist != NULL)
  { gamgi_engine_remove_cell (GAMGI_CAST_CELL dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove arrow list *
 *********************/

dlist = layer->arrow_start;
while (dlist != NULL)
  { gamgi_engine_remove_arrow (GAMGI_CAST_ARROW dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove shape list *
 *********************/

dlist = layer->shape_start;
while (dlist != NULL)
  { gamgi_engine_remove_shape (GAMGI_CAST_SHAPE dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }
  
/*********************
 * remove graph list *
 *********************/

dlist = layer->graph_start;
while (dlist != NULL)
  { gamgi_engine_remove_graph (GAMGI_CAST_GRAPH dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/************************
 * remove assembly list *
 ************************/

dlist = layer->assembly_start;
while (dlist != NULL)
  { gamgi_engine_remove_assembly (GAMGI_CAST_ASSEMBLY dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*********************
 * remove light list *
 *********************/

dlist = layer->light_start;
while (dlist != NULL)
  { gamgi_engine_remove_light (GAMGI_CAST_LIGHT dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/****************
 * remove layer *
 ****************/

/****************************************************************
 * addressing must be done after removing the undo/save layers. *
 ****************************************************************/

layer_next = layer->next;
layer_before = layer->before;

gamgi_engine_number_layer_recycle (layer->object.number);
gamgi_engine_remove_out_layer (layer);

if (layer_next == NULL) gamgi->layer_end = layer_before;
if (layer_before == NULL) gamgi->layer_start = layer_next;
}

/***************** external function ****************
 *                                                  *
 *             GAMGI_ENGINE_REMOVE_WINDOW           *
 *                                                  *
 * Remove all layers which belong to window and     *
 * remove the window. A window object does not      *
 * have a link, so it does not need to be unlinked. *
 ****************************************************/

void gamgi_engine_remove_window (gamgi_window *window) 
{
gamgi_dlist *dlist;
gamgi_window *window_next, *window_before;

/**************************************
 * clean Undo,Save history mechanisms *
 **************************************/

if (window->history != NULL)
  gamgi_gtk_history_window_reset (window->history);

/**********************************
 * remove window global selection *
 **********************************/

gamgi_global_selection_clean (GAMGI_ENGINE_WINDOW,
&gamgi->windows, "button_window", window->object.selection);

/*********************
 * remove layer list *
 *********************/

dlist = window->layer_start;
while (dlist != NULL)
  { gamgi_engine_remove_layer (GAMGI_CAST_LAYER dlist->data);
    dlist = gamgi_engine_dlist_remove_start (dlist); }

/*****************
 * remove window *
 *****************/

window_next = window->next;
window_before = window->before;

gamgi_engine_number_window_recycle (window->object.number);
gamgi_engine_remove_out_window (window);

if (window_next == NULL) gamgi->window_end = window_before;
if (window_before == NULL) gamgi->window_start = window_next;
}

/*************** external function ***********
 *                                           *
 *          GAMGI_ENGINE_REMOVE_OBJECT       *
 *                                           *
 *********************************************/

void gamgi_engine_remove_object (gamgi_object *object)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_engine_remove_text (GAMGI_CAST_TEXT object);
  break;

  case GAMGI_ENGINE_ORBITAL:
  gamgi_engine_remove_orbital (GAMGI_CAST_ORBITAL object);
  break;

  case GAMGI_ENGINE_BOND:
  gamgi_engine_remove_bond (GAMGI_CAST_BOND object);
  break;

  case GAMGI_ENGINE_ATOM:
  gamgi_engine_remove_atom (GAMGI_CAST_ATOM object);
  break;

  case GAMGI_ENGINE_DIRECTION:
  gamgi_engine_remove_direction (GAMGI_CAST_DIRECTION object);
  break;

  case GAMGI_ENGINE_PLANE:
  gamgi_engine_remove_plane (GAMGI_CAST_PLANE object);
  break;

  case GAMGI_ENGINE_GROUP:
  gamgi_engine_remove_group (GAMGI_CAST_GROUP object);
  break;

  case GAMGI_ENGINE_MOLECULE:
  gamgi_engine_remove_molecule (GAMGI_CAST_MOLECULE object);
  break;

  case GAMGI_ENGINE_CLUSTER:
  gamgi_engine_remove_cluster (GAMGI_CAST_CLUSTER object);
  break;

  case GAMGI_ENGINE_CELL:
  gamgi_engine_remove_cell (GAMGI_CAST_CELL object);
  break;

  case GAMGI_ENGINE_ARROW:
  gamgi_engine_remove_arrow (GAMGI_CAST_ARROW object);
  break;

  case GAMGI_ENGINE_SHAPE:
  gamgi_engine_remove_shape (GAMGI_CAST_SHAPE object);
  break;

  case GAMGI_ENGINE_GRAPH:
  gamgi_engine_remove_graph (GAMGI_CAST_GRAPH object);
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_engine_remove_assembly (GAMGI_CAST_ASSEMBLY object);
  break;

  case GAMGI_ENGINE_LIGHT:
  gamgi_engine_remove_light (GAMGI_CAST_LIGHT object);
  break;

  case GAMGI_ENGINE_LAYER:
  gamgi_engine_remove_layer (GAMGI_CAST_LAYER object);
  break;

  case GAMGI_ENGINE_WINDOW:
  gamgi_engine_remove_window (GAMGI_CAST_WINDOW object);
  break;
  }
}

/*************** external function ***********
 *                                           *
 *           GAMGI_ENGINE_REMOVE_GAMGI       *
 *                                           *
 * Deallocate memory for all GAMGI resources *
 *  currently available that are directly    *
 *   controlled by the GAMGI engine.         *
 *                                           *
 *********************************************/

void gamgi_engine_remove_gamgi () 
{
/******************************
 * remove all linked objects: *
 * go through the window list *
 ******************************/

while (gamgi->window_end != NULL)
  gamgi_engine_remove_window (gamgi->window_end);

/***********************************
 * remove all unlinked objects:    *
 *                                 *
 * this is just a precaution, ALL  *
 * GAMGI objects must be linked!   *
 *                                 *
 * remove parents before children! *
 ***********************************/

while (gamgi->layer_end != NULL)
  gamgi_engine_remove_layer (gamgi->layer_end);

while (gamgi->light_end != NULL)
  gamgi_engine_remove_light (gamgi->light_end);

while (gamgi->assembly_end != NULL)
  gamgi_engine_remove_assembly (gamgi->assembly_end);

while (gamgi->graph_end != NULL)
  gamgi_engine_remove_graph (gamgi->graph_end);

while (gamgi->shape_end != NULL)
  gamgi_engine_remove_shape (gamgi->shape_end);

while (gamgi->arrow_end != NULL)
  gamgi_engine_remove_arrow (gamgi->arrow_end);

while (gamgi->cell_end != NULL)
  gamgi_engine_remove_cell (gamgi->cell_end);

while (gamgi->cluster_end != NULL)
  gamgi_engine_remove_cluster (gamgi->cluster_end);

while (gamgi->molecule_end != NULL)
  gamgi_engine_remove_molecule (gamgi->molecule_end);

while (gamgi->group_end != NULL)
  gamgi_engine_remove_group (gamgi->group_end);

while (gamgi->plane_end != NULL)
  gamgi_engine_remove_plane (gamgi->plane_end);

while (gamgi->direction_end != NULL)
  gamgi_engine_remove_direction (gamgi->direction_end);

while (gamgi->atom_end != NULL)
  gamgi_engine_remove_atom (gamgi->atom_end);
  
while (gamgi->bond_end != NULL)
  gamgi_engine_remove_bond (gamgi->bond_end);

while (gamgi->orbital_end != NULL)
  gamgi_engine_remove_orbital (gamgi->orbital_end);

while (gamgi->text_end != NULL)
  gamgi_engine_remove_text (gamgi->text_end);

/**************************************
 * remove object number recycle lists *
 **************************************/

gamgi_engine_number_text_reset ();
gamgi_engine_number_orbital_reset ();
gamgi_engine_number_bond_reset ();
gamgi_engine_number_atom_reset ();
gamgi_engine_number_direction_reset ();
gamgi_engine_number_plane_reset ();
gamgi_engine_number_group_reset ();
gamgi_engine_number_molecule_reset ();
gamgi_engine_number_cluster_reset ();
gamgi_engine_number_cell_reset ();
gamgi_engine_number_arrow_reset ();
gamgi_engine_number_shape_reset ();
gamgi_engine_number_graph_reset ();
gamgi_engine_number_assembly_reset ();
gamgi_engine_number_light_reset ();
gamgi_engine_number_layer_reset ();
gamgi_engine_number_window_reset ();

/************************
 * remove configuration *
 ************************/

gamgi_global_remove_help (gamgi->help);
gamgi_global_remove_text (gamgi->text);
gamgi_global_remove_orbital (gamgi->orbital);
gamgi_global_remove_bond (gamgi->bond);
gamgi_global_remove_atom (gamgi->atom);
gamgi_global_remove_direction (gamgi->direction);
gamgi_global_remove_plane (gamgi->plane);
gamgi_global_remove_group (gamgi->group);
gamgi_global_remove_molecule (gamgi->molecule);
gamgi_global_remove_cluster (gamgi->cluster);
gamgi_global_remove_cell (gamgi->cell);
gamgi_global_remove_arrow (gamgi->arrow);
gamgi_global_remove_shape (gamgi->shape);
gamgi_global_remove_graph (gamgi->graph);
gamgi_global_remove_assembly (gamgi->assembly);
gamgi_global_remove_light (gamgi->light);
gamgi_global_remove_layer (gamgi->layer);
gamgi_global_remove_window (gamgi->window);
gamgi_global_remove_gamgi (gamgi->gamgi);

/****************************
 * remove font glyph arrays *
 ****************************/

gamgi_mesa_text_font_end ();

/*********************************
 * remove hash table to speed up *
 * atom element name recognition *
 *********************************/

gamgi_math_hash_remove (gamgi->element, GAMGI_CHEM_HASH_SIZE);

/****************************************
 * remove the singleton gamgi structure *
 ****************************************/

free (gamgi);
}
