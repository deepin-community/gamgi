/*********************************************
 *
 * $GAMGI/src/engine/gamgi_engine_reparent.c
 *
 * Copyright (C) 2001, 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_global.h"

#include "gamgi_engine_link.h"
#include "gamgi_engine_unlink.h"
#include "gamgi_engine_list.h"
#include "gamgi_engine_number.h"

/*********************** external function **************************
 *                                                                  *
 *                    GAMGI_ENGINE_REPARENT_TEXT                    *
 *                                                                  *
 ********************************************************************/

void gamgi_engine_reparent_text (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_text *child;
gamgi_text *text;
gamgi_orbital *orbital;
gamgi_atom *atom;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/***********************************
 * link text objects to new parent *
 ***********************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_TEXT:
  text = GAMGI_CAST_TEXT parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_text (child, text); }
  break;

  case GAMGI_ENGINE_ORBITAL: 
  orbital = GAMGI_CAST_ORBITAL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_orbital (child, orbital); }
  break;

  case GAMGI_ENGINE_ATOM: 
  atom = GAMGI_CAST_ATOM parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_atom (child, atom); }
  break;
  
  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_direction (child, direction); }
  break;
  
  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_plane (child, plane); }
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_TEXT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_text (child);
    gamgi_engine_link_text_layer (child, layer); }
  break;
  }

}

/*********************** external function **************************
 *                                                                  *
 *                  GAMGI_ENGINE_REPARENT_ORBITAL                   *
 *                                                                  *
 ********************************************************************/

void gamgi_engine_reparent_orbital (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_orbital *child;
gamgi_atom *atom;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/**************************************
 * link orbital objects to new parent *
 **************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ATOM: 
  atom = GAMGI_CAST_ATOM parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_atom (child, atom); }
  break;
  
  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_direction (child, direction); }
  break;
  
  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_plane (child, plane); }
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ORBITAL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_orbital (child);
    gamgi_engine_link_orbital_layer (child, layer); }
  break;
  }

}

/************************* external function *************************
 *                                                                   *
 *                     GAMGI_ENGINE_REPARENT_ATOM                    *
 *                                                                   *
 *********************************************************************/

void gamgi_engine_reparent_atom (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_atom *child;
gamgi_direction *direction;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link atoms objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_DIRECTION:
  direction = GAMGI_CAST_DIRECTION parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_direction (child, direction); }
  break;

  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_plane (child, plane); }
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ATOM dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_atom (child);
    gamgi_engine_link_atom_layer (child, layer); }
  break;
  }

}

/* ************************ external function ************************
 *                                                                   *
 *                  GAMGI_ENGINE_REPARENT_DIRECTION                  *
 *                                                                   *
 *********************************************************************/

void gamgi_engine_reparent_direction (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_direction *child;
gamgi_plane *plane;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/****************************************
 * link direction objects to new parent *
 ****************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_PLANE:
  plane = GAMGI_CAST_PLANE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_plane (child, plane); }
  break;

  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_DIRECTION dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_direction (child);
    gamgi_engine_link_direction_layer (child, layer); }
  break;
  }

}

/************************* external function **************************
 *                                                                    *
 *                  GAMGI_ENGINE_REPARENT_PLANE                      *
 *                                                                    *
 **********************************************************************/

void gamgi_engine_reparent_plane (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_plane *child;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link plane objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_PLANE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_plane (child);
    gamgi_engine_link_plane_layer (child, layer); }
  break;
  }

}

/************************ external function **************************
 *                                                                   *
 *                  GAMGI_ENGINE_REPARENT_GROUP                      *
 *                                                                   *
 *********************************************************************/

void gamgi_engine_reparent_group (gamgi_dlist *dlist, gamgi_object *parent) 
{
gamgi_group *child;
gamgi_group *group;
gamgi_molecule *molecule;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link group objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_GROUP:
  group = GAMGI_CAST_GROUP parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_group (child, group); }
  break;

  case GAMGI_ENGINE_MOLECULE:
  molecule = GAMGI_CAST_MOLECULE parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_molecule (child, molecule); }
  break;

  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GROUP dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_group (child);
    gamgi_engine_link_group_layer (child, layer); }
  break;
  }

}

/************************* external function **********************
 *                                                                *
 *                 GAMGI_ENGINE_REPARENT_MOLECULE                *
 *                                                                *
 ******************************************************************/

void gamgi_engine_reparent_molecule (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_molecule *child;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/***************************************
 * link molecule objects to new parent *
 ***************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_MOLECULE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_molecule (child);
    gamgi_engine_link_molecule_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_MOLECULE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_molecule (child);
    gamgi_engine_link_molecule_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_MOLECULE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_molecule (child);
    gamgi_engine_link_molecule_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_MOLECULE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_molecule (child);
    gamgi_engine_link_molecule_layer (child, layer); }
  break;
  }

}

/************************* external function *******************************
 *                                                                         *
 *                  GAMGI_ENGINE_REPARENT_CLUSTER                         *
 *                                                                         *
 ***************************************************************************/

void gamgi_engine_reparent_cluster (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_cluster *child;
gamgi_cluster *cluster;
gamgi_cell *cell;
gamgi_assembly *assembly;
gamgi_layer *layer;

/**************************************
 * link cluster objects to new parent *
 **************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_CLUSTER:
  cluster = GAMGI_CAST_CLUSTER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CLUSTER dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cluster (child);
    gamgi_engine_link_cluster_cluster (child, cluster); }
  break;

  case GAMGI_ENGINE_CELL:
  cell = GAMGI_CAST_CELL parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CLUSTER dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cluster (child);
    gamgi_engine_link_cluster_cell (child, cell); }
  break;

  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CLUSTER dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cluster (child);
    gamgi_engine_link_cluster_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CLUSTER dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cluster (child);
    gamgi_engine_link_cluster_layer (child, layer); }
  break;
  }

}

/************************ external function ************************
 *                                                                 *
 *                  GAMGI_ENGINE_REPARENT_CELL                     *
 *                                                                 *
 *******************************************************************/

void gamgi_engine_reparent_cell (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_cell *child;
gamgi_assembly *assembly;
gamgi_layer *layer;

/***********************************
 * link cell objects to new parent *
 ***********************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CELL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cell (child);
    gamgi_engine_link_cell_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_CELL dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_cell (child);
    gamgi_engine_link_cell_layer (child, layer); }
  break;
  }

}

/************************** external function *******************
 *                                                              *
 *                     GAMGI_ENGINE_REPARENT_ARROW              *
 *                                                              *
 ****************************************************************/

void gamgi_engine_reparent_arrow (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_arrow *child;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link arrow objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ARROW dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_arrow (child);
    gamgi_engine_link_arrow_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ARROW dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_arrow (child);
    gamgi_engine_link_arrow_layer (child, layer); }
  break;
  }

}

/************************** external function ********************
 *                                                               *
 *                     GAMGI_ENGINE_REPARENT_SHAPE               *
 *                                                               *
 *****************************************************************/

void gamgi_engine_reparent_shape (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_shape *child;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link shape objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_SHAPE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_shape (child);
    gamgi_engine_link_shape_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_SHAPE dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_shape (child);
    gamgi_engine_link_shape_layer (child, layer); }
  break;
  }

}

/************************** external function ********************
 *                                                               *
 *                      GAMGI_ENGINE_REPARENT_GRAPH              *
 *                                                               *
 *****************************************************************/

void gamgi_engine_reparent_graph (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_graph *child;
gamgi_assembly *assembly;
gamgi_layer *layer;

/************************************
 * link graph objects to new parent *
 ************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GRAPH dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_graph (child);
    gamgi_engine_link_graph_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_GRAPH dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_graph (child);
    gamgi_engine_link_graph_layer (child, layer); }
  break;
  }

}

/************************** external function ********************
 *                                                               *
 *                  GAMGI_ENGINE_REPARENT_ASSEMBLY               *
 *                                                               *
 *****************************************************************/

void gamgi_engine_reparent_assembly (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_assembly *child;
gamgi_assembly *assembly;
gamgi_layer *layer;

/***************************************
 * link assembly objects to new parent *
 ***************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_ASSEMBLY:
  assembly = GAMGI_CAST_ASSEMBLY parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ASSEMBLY dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_assembly (child);
    gamgi_engine_link_assembly_assembly (child, assembly); }
  break;

  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_ASSEMBLY dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_assembly (child);
    gamgi_engine_link_assembly_layer (child, layer); }
  break;
  }

}

/************************** external function ********************
 *                                                               *
 *                     GAMGI_ENGINE_REPARENT_LIGHT               *
 *                                                               *
 *****************************************************************/

void gamgi_engine_reparent_light (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_light *child;
gamgi_layer *layer;

/***************************************
 * link assembly objects to new parent *
 ***************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_LAYER:
  layer = GAMGI_CAST_LAYER parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_LIGHT dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_light (child);
    gamgi_engine_link_light_layer (child, layer); }
  break;
  }

}

/************************** external function ********************
 *                                                               *
 *                     GAMGI_ENGINE_REPARENT_LAYER               *
 *                                                               *
 *****************************************************************/

void gamgi_engine_reparent_layer (gamgi_dlist *dlist, gamgi_object *parent)
{
gamgi_layer *child;
gamgi_window *window;

/***************************************
 * link assembly objects to new parent *
 ***************************************/

switch (parent->class)
  {
  case GAMGI_ENGINE_WINDOW: 
  window = GAMGI_CAST_WINDOW parent;
  while (dlist != NULL) {
    child = GAMGI_CAST_LAYER dlist->data;
    dlist = dlist->next;
    gamgi_engine_unlink_layer (child);
    gamgi_engine_link_layer_window (child, window); }
  break;
  }

}
