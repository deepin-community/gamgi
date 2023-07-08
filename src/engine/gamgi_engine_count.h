/*******************************************
 *
 * $GAMGI/src/engine/gamgi_engine_count.h
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

void gamgi_engine_count_text (gamgi_text *text, 
gamgi_enum class, int *number);

void gamgi_engine_count_orbital (gamgi_orbital *orbital, 
gamgi_enum class, int *number);

void gamgi_engine_count_bond (gamgi_bond *bond, 
gamgi_enum class, int *number);

void gamgi_engine_count_atom (gamgi_atom *atom, 
gamgi_enum class, int *number);

void gamgi_engine_count_direction (gamgi_direction *direction, 
gamgi_enum class, int *number);

void gamgi_engine_count_plane (gamgi_plane *plane, 
gamgi_enum class, int *number);

void gamgi_engine_count_group (gamgi_group *group, 
gamgi_enum class, int *number);

void gamgi_engine_count_molecule (gamgi_molecule *molecule, 
gamgi_enum class, int *number);

void gamgi_engine_count_cluster (gamgi_cluster *cluster, 
gamgi_enum class, int *number);

void gamgi_engine_count_cell (gamgi_cell *cell, 
gamgi_enum class, int *number);

void gamgi_engine_count_arrow (gamgi_arrow *arrow, 
gamgi_enum class, int *number);

void gamgi_engine_count_shape (gamgi_shape *shape, 
gamgi_enum class, int *number);

void gamgi_engine_count_graph (gamgi_graph *graph, 
gamgi_enum class, int *number);

void gamgi_engine_count_assembly (gamgi_assembly *assembly, 
gamgi_enum class, int *number);

void gamgi_engine_count_layer (gamgi_layer *layer, 
gamgi_enum class, int *number);

void gamgi_engine_count_window (gamgi_window *window,
gamgi_enum class, int *number);

void gamgi_engine_count_gamgi (gamgi_enum class, int *number);

void gamgi_engine_count_object (gamgi_object *object,
gamgi_enum class, int *number);

/*************
 * Rationale *
 *************



 *************
 * Rationale *
 *************/
