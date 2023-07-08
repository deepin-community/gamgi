/************************************************
 *
 * $GAMGI/src/expat/gamgi_expat_import_config.h
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

/**********************
 * internal functions *
 **********************

static gamgi_bool static_gamgi_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_window_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_window_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_layer_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_layer_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_light_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_light_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_assembly_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_assembly_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_graph_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_graph_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_shape_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_shape_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_arrow_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_arrow_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_cell_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_cell_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_cluster_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_cluster_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_molecule_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_molecule_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_group_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_group_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_plane_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_plane_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_direction_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_direction_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_atom_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_atom_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_bond_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_bond_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_orbital_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_orbital_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_text_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_text_start (const char *element,
const char **attributes, gamgi_gml *gml);

static gamgi_bool static_help_end (const char *element, gamgi_gml *gml);

static gamgi_bool static_help_start (const char *element,
const char **attributes, gamgi_gml *gml);

 **********************
 * external functions *
 **********************/

void gamgi_expat_import_config_end (void *data, const char *element);

void gamgi_expat_import_config_start (void *data, const char *element, 
const char **attributes);

/*************
 * Rationale *
 *************

Under development.

 *************
 * Rationale *
 *************/

