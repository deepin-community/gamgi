/************************************************
 *
 * $GAMGI/src/global/gamgi_global_remove.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_io.h"
#include "gamgi_global.h"

#include "gamgi_mesa_text.h"
#include "gamgi_io_token.h"

/***************** external function ****************
 *                                                  *
 *           GAMGI_ENGINE_HELP_REMOVE               *
 *                                                  *
 ****************************************************/

void gamgi_global_remove_help (gamgi_help_class *help_class)
{
gamgi_io_token_remove (&help_class->local);
gamgi_io_token_remove (&help_class->remote);
gamgi_io_token_remove (&help_class->browser);
free (help_class);
}

/***************** external function ****************
 *                                                  *
 *           GAMGI_ENGINE_TEXT_REMOVE               *
 *                                                  *
 ****************************************************/

void gamgi_global_remove_text (gamgi_text_class *text_class) 
{
gamgi_io_token_remove (&text_class->path);
free (text_class);
}

/******************** external function *****************
 *                                                      *
 *           GAMGI_ENGINE_ORBITAL_REMOVE                *
 *                                                      *
 ********************************************************/

void gamgi_global_remove_orbital (gamgi_orbital_class *orbital_class) 
{
free (orbital_class);
}

/***************** external function ***************
 *                                                 *
 *           GAMGI_ENGINE_BOND_REMOVE              *
 *                                                 *
 ***************************************************/

void gamgi_global_remove_bond (gamgi_bond_class *bond_class)
{
free (bond_class->min);
free (bond_class->max);
free (bond_class);
}

/***************** external function ********************
 *                                                      *
 *           GAMGI_ENGINE_ATOM_REMOVE                   *
 *                                                      *
 ********************************************************/

void gamgi_global_remove_atom (gamgi_atom_class *atom_class) 
{
free (atom_class->mass);
free (atom_class->radius);
free (atom_class->red);
free (atom_class->green);
free (atom_class->blue);
free (atom_class);
}

/******************** external function ************************
 *                                                             *
 *             GAMGI_ENGINE_DIRECTION_REMOVE                   *
 *                                                             *
 ***************************************************************/

void gamgi_global_remove_direction (gamgi_direction_class *direction_class) 
{
free (direction_class);
}

/******************** external function *******************
 *                                                        *
 *           GAMGI_ENGINE_PLANE_REMOVE                    *
 *                                                        *
 **********************************************************/

void gamgi_global_remove_plane (gamgi_plane_class *plane_class) 
{
free (plane_class);
}

/***************** external function ***********************
 *                                                         *
 *           GAMGI_ENGINE_GROUP_REMOVE                     *
 *                                                         *
 ***********************************************************/

void gamgi_global_remove_group (gamgi_group_class *group_class) 
{
free (group_class);
}

/********************** external function *********************
 *                                                            *
 *             GAMGI_ENGINE_MOLECULE_REMOVE                   *
 *                                                            *
 **************************************************************/

void gamgi_global_remove_molecule (gamgi_molecule_class *molecule_class) 
{
free (molecule_class);
}

/************************ external function ******************
 *                                                           *
 *               GAMGI_ENGINE_CLUSTER_REMOVE                 *
 *                                                           *
 *************************************************************/

void gamgi_global_remove_cluster (gamgi_cluster_class *cluster_class) 
{
free (cluster_class);
}

/******************** external function *****************
 *                                                      *
 *             GAMGI_ENGINE_CELL_REMOVE                 *
 *                                                      *
 ********************************************************/

void gamgi_global_remove_cell (gamgi_cell_class *cell_class) 
{
free (cell_class);
}

/******************** external function *******************
 *                                                        *
 *           GAMGI_ENGINE_ARROW_REMOVE                    *
 *                                                        *
 **********************************************************/

void gamgi_global_remove_arrow (gamgi_arrow_class *arrow_class) 
{
free (arrow_class);
}

/******************** external function *******************
 *                                                        *
 *           GAMGI_ENGINE_SHAPE_REMOVE                    *
 *                                                        *
 **********************************************************/

void gamgi_global_remove_shape (gamgi_shape_class *shape_class) 
{
free (shape_class);
}

/******************** external function *******************
 *                                                        *
 *           GAMGI_ENGINE_GRAPH_REMOVE                    *
 *                                                        *
 **********************************************************/

void gamgi_global_remove_graph (gamgi_graph_class *graph_class) 
{
free (graph_class);
}

/************************** external function ******************
 *                                                             *
 *                GAMGI_ENGINE_ASSEMBLY_REMOVE                 *
 *                                                             *
 ***************************************************************/

void gamgi_global_remove_assembly (gamgi_assembly_class *assembly_class) 
{
free (assembly_class);
}

/********* external function ********
 *                                  *
 *   GAMGI_ENGINE_LIGHT_REMOVE      *
 *                                  *
 ************************************/

void gamgi_global_remove_light (gamgi_light_class *light_class) 
{
free (light_class);
}

/******************** external function *******************
 *                                                        *
 *           GAMGI_ENGINE_LAYER_REMOVE                    *
 *                                                        *
 **********************************************************/

void gamgi_global_remove_layer (gamgi_layer_class *layer_class) 
{
free (layer_class);
}

/***************** external function ****************
 *                                                  *
 *         GAMGI_ENGINE_WINDOW_REMOVE               *
 *                                                  *
 ****************************************************/

void gamgi_global_remove_window (gamgi_window_class *window_class) 
{
free (window_class);
}

/*************** external function ***********
 *                                           *
 *        GAMGI_ENGINE_REMOVE_GAMGI          *
 *                                           *
 *********************************************/

void gamgi_global_remove_gamgi (gamgi_gamgi_class *gamgi_class) 
{
free (gamgi_class);
}

/*************** external function ***********
 *                                           *
 *          GAMGI_ENGINE_REMOVE_OBJECT       *
 *                                           *
 *********************************************/

void gamgi_global_remove_object (gamgi_object *object)
{
switch (object->class)
  {
  case GAMGI_ENGINE_TEXT:
  gamgi_global_remove_text ((gamgi_text_class *) object);

  case GAMGI_ENGINE_ORBITAL:
  gamgi_global_remove_orbital ((gamgi_orbital_class *) object);

  case GAMGI_ENGINE_BOND:
  gamgi_global_remove_bond ((gamgi_bond_class *) object);

  case GAMGI_ENGINE_ATOM:
  gamgi_global_remove_atom ((gamgi_atom_class *) object);

  case GAMGI_ENGINE_DIRECTION:
  gamgi_global_remove_direction ((gamgi_direction_class *) object);

  case GAMGI_ENGINE_PLANE:
  gamgi_global_remove_plane ((gamgi_plane_class *) object);

  case GAMGI_ENGINE_GROUP:
  gamgi_global_remove_group ((gamgi_group_class *) object);

  case GAMGI_ENGINE_MOLECULE:
  gamgi_global_remove_molecule ((gamgi_molecule_class *) object);

  case GAMGI_ENGINE_CLUSTER:
  gamgi_global_remove_cluster ((gamgi_cluster_class *) object);

  case GAMGI_ENGINE_CELL:
  gamgi_global_remove_cell ((gamgi_cell_class *) object);

  case GAMGI_ENGINE_ARROW:
  gamgi_global_remove_arrow ((gamgi_arrow_class *) object);

  case GAMGI_ENGINE_SHAPE:
  gamgi_global_remove_shape ((gamgi_shape_class *) object);

  case GAMGI_ENGINE_GRAPH:
  gamgi_global_remove_graph ((gamgi_graph_class *) object);

  case GAMGI_ENGINE_ASSEMBLY:
  gamgi_global_remove_assembly ((gamgi_assembly_class *) object);

  case GAMGI_ENGINE_LIGHT:
  gamgi_global_remove_light ((gamgi_light_class *) object);

  case GAMGI_ENGINE_LAYER:
  gamgi_global_remove_layer ((gamgi_layer_class *) object);

  case GAMGI_ENGINE_WINDOW:
  gamgi_global_remove_window ((gamgi_window_class *) object);

  case GAMGI_ENGINE_GAMGI:
  gamgi_global_remove_gamgi ((gamgi_gamgi_class *) object);
  }
}
