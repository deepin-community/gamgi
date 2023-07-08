/**********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_topic.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_help_dialog.h"

static void static_interfaces (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces",
"interfaces/interfaces", data, 0, 0);
}

static void static_interfaces_introduction (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction",
"interfaces/introduction/introduction", data, 0, 0);
}

static void static_interfaces_introduction_objects (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Objects", 
"interfaces/introduction/objects", data, 0, 0);
}

static void static_interfaces_introduction_tasks (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Tasks", 
"interfaces/introduction/tasks", data, 0, 0);
}

static void static_interfaces_introduction_windows (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Windows", 
"interfaces/introduction/windows", data, 0, 0);
}

static void static_interfaces_introduction_starting (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Starting", 
"interfaces/introduction/starting", data, 0, 0);
}

static void static_interfaces_introduction_identification (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Identification", 
"interfaces/introduction/identification", data, 0, 0);
}

static void static_interfaces_introduction_selection (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Selection", 
"interfaces/introduction/selection", data, 0, 0);
}

static void static_interfaces_introduction_mouse (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Mouse", 
"interfaces/introduction/mouse", data, 0, 0);
}

static void static_interfaces_introduction_keyboard (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Keyboard", 
"interfaces/introduction/keyboard", data, 0, 0);
}

static void static_interfaces_introduction_menus (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Menus", 
"interfaces/introduction/menus", data, 0, 0);
}

static void static_interfaces_introduction_drawing (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Drawing", 
"interfaces/introduction/drawing", data, 0, 0);
}

static void static_interfaces_introduction_tools (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Tools", 
"interfaces/introduction/tools", data, 0, 0);
}

static void static_interfaces_introduction_commands (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Commands", 
"interfaces/introduction/commands", data, 0, 0);
}

static void static_interfaces_introduction_status (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Introduction Status", 
"interfaces/introduction/status", data, 0, 0);
}

static void static_interfaces_gamgi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Gamgi", 
"keywords/gamgi", data, 0, 0);
}

static void static_interfaces_gamgi_about (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Gamgi About", 
"interfaces/gamgi/about", data, 0, 0);
}

static void static_interfaces_gamgi_reset (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Gamgi Reset", 
"interfaces/gamgi/reset", data, 0, 0);
}

static void static_interfaces_gamgi_exit (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Gamgi Exit", 
"interfaces/gamgi/exit", data, 0, 0);
}

static void static_interfaces_gamgi_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Gamgi Config", 
"interfaces/gamgi/config", data, 3, 1, "usability", 
"precision", "color");
}

static void static_interfaces_file (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces File", 
"keywords/file", data, 0, 0);
}

static void static_interfaces_file_import (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces File Import", 
"interfaces/file/import", data, 2, 1, "location", "filters");
}

static void static_interfaces_file_export (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces File Export", 
"interfaces/file/export", data, 2, 1, "location", "filters");
}

static void static_interfaces_window (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window", 
"keywords/window", data, 0, 0);
}

static void static_interfaces_window_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window Copy",
"interfaces/window/copy", data, 1, 1, "object");
}

static void static_interfaces_window_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window Select", 
"interfaces/window/select", data, 1, 1, "list");
}

static void static_interfaces_window_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window Link",
"interfaces/window/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_window_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window Remove", 
"interfaces/window/remove", data, 1, 1, "object");
}

static void static_interfaces_window_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Window Config", 
"interfaces/window/config", data, 1, 1, "ruler");
}

static void static_interfaces_layer (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer", 
"keywords/layer", data, 0, 0);
}

static void static_interfaces_layer_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Create", 
"interfaces/layer/create", data, 4, 1, "projection", 
"position", "visibility", "view");
}

static void static_interfaces_layer_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Copy",
"interfaces/layer/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_layer_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Select", 
"interfaces/layer/select", data, 1, 1, "list");
}

static void static_interfaces_layer_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Link",
"interfaces/layer/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_layer_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Modify",
"interfaces/layer/modify", data, 4, 1, "projection", "position", 
"visibility", "view");
}

static void static_interfaces_layer_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Remove", 
"interfaces/layer/remove", data, 1, 1, "object");
}

static void static_interfaces_layer_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Layer Config", 
"interfaces/layer/config", data, 3, 1, "projection", "position", "history");
}

static void static_interfaces_light (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light", 
"keywords/light", data, 0, 0);
}

static void static_interfaces_light_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Create", 
"interfaces/light/create", data, 3, 1, "geometry", "color", "attenuation");
}

static void static_interfaces_light_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Copy",
"interfaces/light/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_light_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Select", 
"interfaces/light/select", data, 1, 1, "list");
}

static void static_interfaces_light_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Link",
"interfaces/light/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_light_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Modify",
"interfaces/light/modify", data, 3, 1, "geometry", "color", "attenuation");
}

static void static_interfaces_light_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Remove", 
"interfaces/light/remove", data, 1, 1, "object");
}

static void static_interfaces_light_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Light Config", 
"interfaces/light/config", data, 2, 1, "color", "global");
}

static void static_interfaces_assembly (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly", 
"keywords/assembly", data, 0, 0);
}

static void static_interfaces_assembly_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly Create", 
"interfaces/assembly/create", data, 0, 0);
}

static void static_interfaces_assembly_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly Copy",
"interfaces/assembly/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_assembly_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly Select", 
"interfaces/assembly/select", data, 1, 1, "list");
}

static void static_interfaces_assembly_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly Link",
"interfaces/assembly/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_assembly_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Assembly Remove", 
"interfaces/assembly/remove", data, 1, 1, "object");
}

static void static_interfaces_graph (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Graph", 
"keywords/graph", data, 0, 0);
}

static void static_interfaces_graph_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Graph Select", 
"interfaces/graph/select", data, 0, 0);
}

static void static_interfaces_shape (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Shape", 
"keywords/shape", data, 0, 0);
}

static void static_interfaces_shape_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Shape Select", 
"interfaces/shape/select", data, 0, 0);
}

static void static_interfaces_arrow (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Arrow", 
"keywords/arrow", data, 0, 0);
}

static void static_interfaces_arrow_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Arrow", 
"interfaces/arrow/select", data, 0, 0);
}

static void static_interfaces_cell (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell", 
"keywords/cell", data, 0, 0);
}

static void static_interfaces_cell_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Create", 
"interfaces/cell/create", data, 5, 1, "symmetry", "volume",
"position", "origin", "view");
}

static void static_interfaces_cell_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Copy",
"interfaces/cell/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_cell_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Select", 
"interfaces/cell/select", data, 1, 1, "list");
}

static void static_interfaces_cell_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Link",
"interfaces/cell/link", data, 6, 1, "object", "shift",
"position", "change", "occupancy", "packing");
}

static void static_interfaces_cell_link_rcp (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Link RCP",
"interfaces/cell/rcp", data, 0, 0);
}

static void static_interfaces_cell_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Modify", 
"interfaces/cell/modify", data, 5, 1, "symmetry", "volume",
"position", "origin", "view");
}

static void static_interfaces_cell_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Measure", 
"interfaces/cell/measure", data, 5, 1, "type", "reciprocal",
"voronoi", "tolerance", "view");
}

static void static_interfaces_cell_measure_geometry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Measure Geometry",
"interfaces/cell/geometry", data, 0, 0);
}

static void static_interfaces_cell_measure_voronoi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Measure Voronoi",
"interfaces/cell/voronoi", data, 0, 0);
}

static void static_interfaces_cell_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Remove", 
"interfaces/cell/remove", data, 1, 1, "object");
}

static void static_interfaces_cell_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cell Config",
"interfaces/cell/config", data, 4, 1, "volume", "origin",
"view", "global");
}

static void static_interfaces_cluster (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster", 
"keywords/cluster", data, 0, 0);
}

static void static_interfaces_cluster_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Create", 
"interfaces/cluster/create", data, 0, 0);
}

static void static_interfaces_cluster_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Copy",
"interfaces/cluster/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_cluster_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Select", 
"interfaces/cluster/select", data, 1, 1, "list");
}

static void static_interfaces_cluster_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Link",
"interfaces/cluster/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_cluster_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Measure", 
"interfaces/cluster/measure", data, 5, 1, "type", "symmetry",
"voronoi", "tolerance", "view");
}

static void static_interfaces_cluster_measure_geometry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Measure Geometry",
"interfaces/cluster/geometry", data, 0, 0);
}

static void static_interfaces_cluster_measure_symmetry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Measure Symmetry",
"interfaces/cluster/symmetry", data, 0, 0);
}

static void static_interfaces_cluster_measure_voronoi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Measure Voronoi",
"interfaces/cluster/voronoi", data, 0, 0);
}

static void static_interfaces_cluster_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Cluster Remove", 
"interfaces/cluster/remove", data, 1, 1, "object");
}

static void static_interfaces_molecule (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule", 
"keywords/molecule", data, 0, 0);
}

static void static_interfaces_molecule_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Create", 
"interfaces/molecule/create", data, 1, 1, "type");
}

static void static_interfaces_molecule_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Copy",
"interfaces/molecule/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_molecule_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Select", 
"interfaces/molecule/select", data, 1, 1, "list");
}

static void static_interfaces_molecule_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Link",
"interfaces/molecule/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_molecule_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Measure", 
"interfaces/molecule/measure", data, 5, 1, "type", "symmetry",
"voronoi", "tolerance", "view");
}

static void static_interfaces_molecule_measure_geometry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Measure Geometry",
"interfaces/molecule/geometry", data, 0, 0);
}

static void static_interfaces_molecule_measure_symmetry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Measure Symmetry",
"interfaces/molecule/symmetry", data, 0, 0);
}

static void static_interfaces_molecule_measure_voronoi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Measure Voronoi",
"interfaces/molecule/voronoi", data, 0, 0);
}

static void static_interfaces_molecule_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Molecule Remove", 
"interfaces/molecule/remove", data, 1, 1, "object");
}

static void static_interfaces_group (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group", 
"keywords/group", data, 0, 0);
}

static void static_interfaces_group_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Create",
"interfaces/group/create", data, 3, 1, "type", "scope",
"view");
}

static void static_interfaces_group_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Copy",
"interfaces/group/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_group_create_polytope (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Create Polytope",
"interfaces/group/create_polytope", data, 0, 0);
}

static void static_interfaces_group_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Select", 
"interfaces/group/select", data, 1, 1, "list");
}

static void static_interfaces_group_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Link",
"interfaces/group/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_group_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Modify",
"interfaces/group/modify", data, 4, 1, "type", "scope", 
"position", "view");
}

static void static_interfaces_group_modify_polytope (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Modify Polytope",
"interfaces/group/modify_polytope", data, 0, 0);
}

static void static_interfaces_group_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Measure",
"interfaces/group/measure", data, 5, 1, "type", "symmetry",
"voronoi", "tolerance", "view");
}

static void static_interfaces_group_measure_geometry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Measure Geometry",
"interfaces/group/geometry", data, 0, 0);
}

static void static_interfaces_group_measure_symmetry (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Measure Symmetry",
"interfaces/group/symmetry", data, 0, 0);
}

static void static_interfaces_group_measure_voronoi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Measure Voronoi",
"interfaces/group/voronoi", data, 0, 0);
}

static void static_interfaces_group_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Group Remove", 
"interfaces/group/remove", data, 1, 1, "object");
}

static void static_interfaces_plane (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane", 
"keywords/plane", data, 0, 0);
}

static void static_interfaces_plane_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Create", 
"interfaces/plane/create", data, 4, 1, "type", "projection",
"scope", "view");
}

static void static_interfaces_plane_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Copy",
"interfaces/plane/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_plane_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Select", 
"interfaces/plane/select", data, 1, 1, "list");
}

static void static_interfaces_plane_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Link",
"interfaces/plane/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_plane_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Modify",
"interfaces/plane/modify", data, 5, 1, "type", "projection",
"scope", "position", "view");
}

static void static_interfaces_plane_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Measure", 
"interfaces/plane/measure", data, 3, 1, "length", "area", "angle");
}

static void static_interfaces_plane_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Plane Remove", 
"interfaces/plane/remove", data, 1, 1, "object");
}

static void static_interfaces_direction (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction", 
"keywords/direction", data, 0, 0);
}

static void static_interfaces_direction_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Create", 
"interfaces/direction/create", data, 4, 1, "type", "projection", 
"scope", "view");
}

static void static_interfaces_direction_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Copy",
"interfaces/direction/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_direction_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Select", 
"interfaces/direction/select", data, 1, 1, "list");
}

static void static_interfaces_direction_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Link",
"interfaces/direction/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_direction_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Modify",
"interfaces/direction/modify", data, 5, 1, "type", "projection", 
"scope", "position", "view");
}

static void static_interfaces_direction_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Measure", 
"interfaces/direction/measure", data, 2, 1, "length", "angle");
}

static void static_interfaces_direction_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Direction Remove", 
"interfaces/direction/remove", data, 1, 1, "object");
}

static void static_interfaces_atom (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom", 
"keywords/atom", data, 0, 0);
}

static void static_interfaces_atom_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Create", 
"interfaces/atom/create", data, 5, 1, "type", "property",
"analysis", "position", "view");
}

static void static_interfaces_atom_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Copy",
"interfaces/atom/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_atom_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Select", 
"interfaces/atom/select", data, 1, 1, "list");
}

static void static_interfaces_atom_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Link",
"interfaces/atom/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_atom_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Modify", 
"interfaces/atom/modify", data, 7, 1, "type", "property",
"analysis", "position", "view", "length", "angle");
}

static void static_interfaces_atom_measure (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom measure",
"interfaces/atom/measure", data, 2, 1, "length", "angle");
}

static void static_interfaces_atom_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Remove", 
"interfaces/atom/remove", data, 1, 1, "object");
}

static void static_interfaces_atom_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Atom Config", 
"interfaces/atom/config", data, 4, 1, "type",
"analysis", "view", "global");
}

static void static_interfaces_bond (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond", 
"keywords/bond", data, 0, 0);
}

static void static_interfaces_bond_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Create", 
"interfaces/bond/create", data, 4, 1, "type", "length",
"property", "view");
}

static void static_interfaces_bond_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Select", 
"interfaces/bond/select", data, 1, 1, "list");
}

static void static_interfaces_bond_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Link",
"interfaces/bond/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_bond_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Modify", 
"interfaces/bond/modify", data, 2, 1, "property", "view");
}

static void static_interfaces_bond_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Remove", 
"interfaces/bond/remove", data, 1, 1, "object");
}

static void static_interfaces_bond_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Bond Config", 
"interfaces/bond/config", data, 2, 1, "length", "view");
}

static void static_interfaces_orbital (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital", 
"keywords/orbital", data, 0, 0);
}

static void static_interfaces_orbital_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Create",
"interfaces/orbital/create", data, 5, 1,
"type", "scope", "volume", "position", "view");
}

static void static_interfaces_orbital_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Copy",
"interfaces/orbital/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_orbital_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Select", 
"interfaces/orbital/select", data, 1, 1, "list");
}

static void static_interfaces_orbital_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Link",
"interfaces/orbital/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_orbital_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Modify",
"interfaces/orbital/modify", data, 5, 1,
"type", "scope", "volume", "position", "view");
}

static void static_interfaces_orbital_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Orbital Remove",
"interfaces/orbital/remove", data, 1, 1, "object");
}

static void static_interfaces_text (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text", 
"keywords/text", data, 0, 0);
}

static void static_interfaces_text_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Create", 
"interfaces/text/create", data, 3, 1, "contents", "position", "view");
}

static void static_interfaces_text_copy (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Copy",
"interfaces/text/copy", data, 2, 1, "object", "shift");
}

static void static_interfaces_text_select (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Select", 
"interfaces/text/select", data, 1, 1, "list");
}

static void static_interfaces_text_link (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Link", 
"interfaces/text/link", data, 2, 1, "object", "shift");
}

static void static_interfaces_text_modify (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Modify", 
"interfaces/text/modify", data, 3, 1, "contents", "position", "view");
}

static void static_interfaces_text_remove (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Remove", 
"interfaces/text/remove", data, 1, 1, "object");
}

static void static_interfaces_text_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Text Config", 
"interfaces/text/config", data, 1, 1, "view");
}

static void static_interfaces_help (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help", 
"keywords/help", data, 0, 0);
}

static void static_interfaces_help_current (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Current", 
"interfaces/help/current", data, 0, 0);
}

static void static_interfaces_help_topic (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Topic",
"interfaces/help/topic", data, 0, 0);
}

static void static_interfaces_help_search (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Search", 
"interfaces/help/search", data, 0, 0);
}

static void static_interfaces_help_tutorial (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Tutorial", 
"interfaces/help/tutorial", data, 0, 0);
}

static void static_interfaces_help_tips (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Tips",
"interfaces/help/tips", data, 0, 0);
}

static void static_interfaces_help_start (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Start",
"interfaces/help/start", data, 0, 0);
}

static void static_interfaces_help_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Interfaces Help Config", 
"interfaces/help/config", data, 2, 1, "source", "agent");
}

/****************
 * file formats *
 ****************/

static void static_formats (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats", 
"formats/formats", data, 0, 0);
}

static void static_formats_introduction (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction", 
"formats/introduction/introduction", data, 0, 0);
}

static void static_formats_introduction_language (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Language", 
"formats/introduction/language", data, 0, 0);
}

static void static_formats_introduction_elements (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Elements", 
"formats/introduction/elements", data, 0, 0);
}

static void static_formats_introduction_attributes (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Attributes", 
"formats/introduction/attributes", data, 0, 0);
}

static void static_formats_introduction_headers (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Headers", 
"formats/introduction/headers", data, 0, 0);
}

static void static_formats_introduction_entities (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Entities", 
"formats/introduction/entities", data, 0, 0);
}

static void static_formats_introduction_comments (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Comments", 
"formats/introduction/comments", data, 0, 0);
}

static void static_formats_introduction_identifiers (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Identifiers", 
"formats/introduction/identifiers", data, 0, 0);
}

static void static_formats_introduction_defaults (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction Defaults", 
"formats/introduction/defaults", data, 0, 0);
}

static void static_formats_introduction_jpeg (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction JPEG",
"formats/introduction/jpeg", data, 0, 0);
}

static void static_formats_introduction_png (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction PNG",
"formats/introduction/png", data, 0, 0);
}

static void static_formats_introduction_ppm (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction PPM",
"formats/introduction/ppm", data, 0, 0);
}

static void static_formats_introduction_ps (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction PS",
"formats/introduction/ps", data, 0, 0);
}

static void static_formats_introduction_tiff (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction TIFF",
"formats/introduction/tiff", data, 0, 0);
}

static void static_formats_introduction_xyz (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Introduction XYZ",
"formats/introduction/xyz", data, 0, 0);
}

static void static_formats_gamgi (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Gamgi",  
"keywords/gamgi", data, 0, 0);
}

static void static_formats_gamgi_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Gamgi Config",
"formats/gamgi/config", data, 3, 1, "usability", "precision", "color");
}

static void static_formats_file (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats File",
"keywords/file", data, 0, 0);
}

static void static_formats_file_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats File Config",
"formats/file/config", data, 0, 0);
}

static void static_formats_window (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Window",
"keywords/window", data, 0, 0);
}

static void static_formats_window_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Window Create",
"formats/window/create", data, 1, 1, "position");
}

static void static_formats_window_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Config Window",
"formats/window/config", data, 1, 1, "ruler");
}

static void static_formats_layer (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Layer",
"keywords/layer", data, 0, 0);
}

static void static_formats_layer_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Layer Create",
"formats/layer/create", data, 4, 1, "projection", "position", 
"visibility", "view");
}

static void static_formats_layer_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Layer Config",
"formats/layer/config", data, 3, 1, "projection", "position", "history");
}

static void static_formats_light (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Light",
"keywords/light", data, 0, 0);
}

static void static_formats_light_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Light Create",
"formats/light/create", data, 3, 1, "type", "color", "attenuation");
}

static void static_formats_light_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Light Config",
"formats/light/config", data, 2, 1, "color", "global");
}

static void static_formats_assembly (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Assembly",
"keywords/assembly", data, 0, 0);
}

static void static_formats_assembly_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Assembly Create",
"formats/assembly/create", data, 1, 1, "view");
}

static void static_formats_graph (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Graph",
"keywords/graph", data, 0, 0);
}

static void static_formats_graph_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Graph Create",
"formats/graph/create", data, 1, 1, "view");
}

static void static_formats_arrow (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Arrow",
"keywords/arrow", data, 0, 0);
}

static void static_formats_arrow_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Arrow Create",
"formats/arrow/create", data, 1, 1, "view");
}

static void static_formats_shape (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Shape",
"keywords/shape", data, 0, 0);
}

static void static_formats_shape_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Create Shape",
"formats/shape/create", data, 1, 1, "view");
}

static void static_formats_cell (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Cell",
"keywords/cell", data, 0, 0);
}

static void static_formats_cell_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Cell Create",
"formats/cell/create", data, 5, 1, "symmetry", "volume", 
"position", "origin", "view");
}

static void static_formats_cell_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Cell Config",
"formats/cell/config", data, 0, 0);
}

static void static_formats_cluster (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Cluster",
"keywords/cluster", data, 0, 0);
}

static void static_formats_cluster_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Cluster Create",
"formats/cluster/create", data, 4, 1, "type", "scope",
"position", "view");
}

static void static_formats_molecule (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Molecule",
"keywords/molecule", data, 0, 0);
}

static void static_formats_molecule_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Molecule Create",
"formats/molecule/create", data, 1, 1, "view");
}

static void static_formats_group (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Group",
"keywords/group", data, 0, 0);
}

static void static_formats_group_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Group Create",
"formats/group/create", data, 4, 1, "type", "scope",
"position", "view");
}

static void static_formats_plane (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Plane",
"keywords/plane", data, 0, 0);
}

static void static_formats_plane_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Plane Create",
"formats/plane/create", data, 5, 1, "type", "projection", 
"scope", "position", "view");
}

static void static_formats_direction (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Direction",
"keywords/direction", data, 0, 0);
}

static void static_formats_direction_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Direction Create",
"formats/direction/create", data, 5, 1, "type", "projection", 
"scope", "position", "view");
}

static void static_formats_atom (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Atom",
"keywords/atom", data, 0, 0);
}

static void static_formats_atom_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Atom Create",
"formats/atom/create", data, 5, 1, "type", "property",
"analysis", "position", "view");
}

static void static_formats_atom_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Atom Config",
"formats/atom/config", data, 0, 0);
}

static void static_formats_bond (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Bond",
"keywords/bond", data, 0, 0);
}

static void static_formats_bond_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Bond Create",
"formats/bond/create", data, 3, 1, "type", "property", "view");
}

static void static_formats_bond_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Bond Config",
"formats/bond/config", data, 2, 1, "length", "view");
}

static void static_formats_orbital (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Orbital",
"keywords/orbital", data, 0, 0);
}

static void static_formats_orbital_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Orbital Create",
"formats/orbital/create", data, 5, 1, "type", "scope",
"volume", "position", "view");
}

static void static_formats_text (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Text",
"keywords/text", data, 0, 0);
}

static void static_formats_text_create (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Text Create",
"formats/text/create", data, 3, 1, "contents", "position", "view");
}

static void static_formats_text_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Text Config",
"formats/text/config", data, 1, 1, "view");
}

static void static_formats_help (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Help",
"keywords/help", data, 0, 0);
}

static void static_formats_help_config (void *data)
{
gamgi_gtk_help_dialog_import ("Help Formats Help Config",
"formats/help/config", data, 2, 1, "source", "agent");
}

static void static_activated (GtkTreeView *treeview, 
GtkTreePath *path, GtkTreeViewColumn *col, void *data)
{
GtkTreeIter iter;
GtkTreeModel *model;
gamgi_function1 function;

/*********************************
 * a row has been double-clicked *
 *********************************/

model = gtk_tree_view_get_model (treeview);
if (gtk_tree_model_get_iter (model, &iter, path) == FALSE) return;

gtk_tree_model_get (model, &iter, 1, &function, -1);
(* function) (data);
}

static void static_ok (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog = window->dialog0;
GtkWidget *treeview;
GtkTreeSelection *selection;
GtkTreeModel *model;
GtkTreeIter iter;
gamgi_function1 function;

treeview = (GtkWidget *) g_object_get_data (G_OBJECT (dialog), "treeview");
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
if (gtk_tree_selection_get_selected (selection, &model, &iter) == FALSE)
  {
  gamgi_gtk_dialog_message_create ("Error", "Help topic not selected", window);
  return;
  }

gtk_tree_model_get (model, &iter, 1, &function, -1);
(* function) (data);
}

static GtkTreeIter static_row (GtkTreeStore *store, 
GtkTreeIter *parent, char *label, gamgi_function1 function)
{
GtkTreeIter iter;

gtk_tree_store_append (store, &iter, parent);
gtk_tree_store_set (store, &iter, 0, label, 1, function, -1);

return iter;
}

static void static_contents (GtkWidget *treeview)
{
GtkTreeStore *store;
GtkTreeIter iter1, iter2, iter3;

store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);

/*********************
 * dialog interfaces *
 *********************/

iter1 = static_row (store, NULL, "Interfaces", static_interfaces);

iter2 = static_row (store, &iter1, "Introduction", static_interfaces_introduction);
static_row (store, &iter2, "Objects", static_interfaces_introduction_objects);
static_row (store, &iter2, "Tasks", static_interfaces_introduction_tasks);
static_row (store, &iter2, "Windows", static_interfaces_introduction_windows);
static_row (store, &iter2, "Starting", static_interfaces_introduction_starting);
static_row (store, &iter2, "Identification", static_interfaces_introduction_identification);
static_row (store, &iter2, "Selection", static_interfaces_introduction_selection);
static_row (store, &iter2, "Mouse", static_interfaces_introduction_mouse);
static_row (store, &iter2, "Keyboard", static_interfaces_introduction_keyboard);
static_row (store, &iter2, "Menus", static_interfaces_introduction_menus);
static_row (store, &iter2, "Drawing", static_interfaces_introduction_drawing);
static_row (store, &iter2, "Tools", static_interfaces_introduction_tools);
static_row (store, &iter2, "Commands", static_interfaces_introduction_commands);
static_row (store, &iter2, "Status", static_interfaces_introduction_status);

iter2 = static_row (store, &iter1, "Gamgi", static_interfaces_gamgi);
static_row (store, &iter2, "About", static_interfaces_gamgi_about);
static_row (store, &iter2, "Reset", static_interfaces_gamgi_reset);
static_row (store, &iter2, "Exit", static_interfaces_gamgi_exit);
static_row (store, &iter2, "Config", static_interfaces_gamgi_config);

iter2 = static_row (store, &iter1, "File", static_interfaces_file);
static_row (store, &iter2, "Import", static_interfaces_file_import);
static_row (store, &iter2, "Export", static_interfaces_file_export);

iter2 = static_row (store, &iter1, "Window", static_interfaces_window);
static_row (store, &iter2, "Copy", static_interfaces_window_copy);
static_row (store, &iter2, "Select", static_interfaces_window_select);
static_row (store, &iter2, "Link", static_interfaces_window_link);
static_row (store, &iter2, "Remove", static_interfaces_window_remove);
static_row (store, &iter2, "Config", static_interfaces_window_config);

iter2 = static_row (store, &iter1, "Layer", static_interfaces_layer);
static_row (store, &iter2, "Create", static_interfaces_layer_create);
static_row (store, &iter2, "Copy", static_interfaces_layer_copy);
static_row (store, &iter2, "Select", static_interfaces_layer_select);
static_row (store, &iter2, "Link", static_interfaces_layer_link);
static_row (store, &iter2, "Modify", static_interfaces_layer_modify);
static_row (store, &iter2, "Remove", static_interfaces_layer_remove);
static_row (store, &iter2, "Config", static_interfaces_layer_config);

iter2 = static_row (store, &iter1, "Light", static_interfaces_light);
static_row (store, &iter2, "Create", static_interfaces_light_create);
static_row (store, &iter2, "Copy", static_interfaces_light_copy);
static_row (store, &iter2, "Select", static_interfaces_light_select);
static_row (store, &iter2, "Link", static_interfaces_light_link);
static_row (store, &iter2, "Modify", static_interfaces_light_modify);
static_row (store, &iter2, "Remove", static_interfaces_light_remove);
static_row (store, &iter2, "Config", static_interfaces_light_config);

iter2 = static_row (store, &iter1, "Assembly", static_interfaces_assembly);
static_row (store, &iter2, "Create", static_interfaces_assembly_create);
static_row (store, &iter2, "Copy", static_interfaces_assembly_copy);
static_row (store, &iter2, "Select", static_interfaces_assembly_select);
static_row (store, &iter2, "Link", static_interfaces_assembly_link);
static_row (store, &iter2, "Remove", static_interfaces_assembly_remove);

iter2 = static_row (store, &iter1, "Graph", static_interfaces_graph);
static_row (store, &iter2, "Select", static_interfaces_graph_select);

iter2 = static_row (store, &iter1, "Shape", static_interfaces_shape);
static_row (store, &iter2, "Select", static_interfaces_shape_select);

iter2 = static_row (store, &iter1, "Arrow", static_interfaces_arrow);
static_row (store, &iter2, "Select", static_interfaces_arrow_select);

iter2 = static_row (store, &iter1, "Cell", static_interfaces_cell);
static_row (store, &iter2, "Create", static_interfaces_cell_create);
static_row (store, &iter2, "Copy", static_interfaces_cell_copy);
static_row (store, &iter2, "Select", static_interfaces_cell_select);
iter3 = static_row (store, &iter2, "Link", static_interfaces_cell_link);
static_row (store, &iter3, "RCP", static_interfaces_cell_link_rcp);
static_row (store, &iter2, "Modify", static_interfaces_cell_modify);
iter3 = static_row (store, &iter2, "Measure", static_interfaces_cell_measure);
static_row (store, &iter3, "Geometry", static_interfaces_cell_measure_geometry);
static_row (store, &iter3, "Voronoi", static_interfaces_cell_measure_voronoi);
static_row (store, &iter2, "Remove", static_interfaces_cell_remove);
static_row (store, &iter2, "Config", static_interfaces_cell_config);

iter2 = static_row (store, &iter1, "Cluster", static_interfaces_cluster);
static_row (store, &iter2, "Create", static_interfaces_cluster_create);
static_row (store, &iter2, "Copy", static_interfaces_cluster_copy);
static_row (store, &iter2, "Select", static_interfaces_cluster_select);
static_row (store, &iter2, "Link", static_interfaces_cluster_link);
iter3 = static_row (store, &iter2, "Measure", static_interfaces_cluster_measure);
static_row (store, &iter3, "Geometry", static_interfaces_cluster_measure_geometry);
static_row (store, &iter3, "Symmetry", static_interfaces_cluster_measure_symmetry);
static_row (store, &iter3, "Voronoi", static_interfaces_cluster_measure_voronoi);
static_row (store, &iter2, "Remove", static_interfaces_cluster_remove);

iter2 = static_row (store, &iter1, "Molecule", static_interfaces_molecule);
static_row (store, &iter2, "Create", static_interfaces_molecule_create);
static_row (store, &iter2, "Copy", static_interfaces_molecule_copy);
static_row (store, &iter2, "Select", static_interfaces_molecule_select);
static_row (store, &iter2, "Link", static_interfaces_molecule_link);
iter3 = static_row (store, &iter2, "Measure", static_interfaces_molecule_measure);
static_row (store, &iter3, "Geometry", static_interfaces_molecule_measure_geometry);
static_row (store, &iter3, "Symmetry", static_interfaces_molecule_measure_symmetry);
static_row (store, &iter3, "Voronoi", static_interfaces_molecule_measure_voronoi);
static_row (store, &iter2, "Remove", static_interfaces_molecule_remove);

iter2 = static_row (store, &iter1, "Group", static_interfaces_group);
iter3 = static_row (store, &iter2, "Create", static_interfaces_group_create);
static_row (store, &iter2, "Copy", static_interfaces_group_copy);
static_row (store, &iter3, "Polytope", static_interfaces_group_create_polytope);
static_row (store, &iter2, "Select", static_interfaces_group_select);
static_row (store, &iter2, "Link", static_interfaces_group_link);
iter3 = static_row (store, &iter2, "Modify", static_interfaces_group_modify);
static_row (store, &iter3, "Polytope", static_interfaces_group_modify_polytope);
iter3 = static_row (store, &iter2, "Measure", static_interfaces_group_measure);
static_row (store, &iter3, "Geometry", static_interfaces_group_measure_geometry);
static_row (store, &iter3, "Symmetry", static_interfaces_group_measure_symmetry);
static_row (store, &iter3, "Voronoi", static_interfaces_group_measure_voronoi);
static_row (store, &iter2, "Remove", static_interfaces_group_remove);

iter2 = static_row (store, &iter1, "Plane", static_interfaces_plane);
static_row (store, &iter2, "Create", static_interfaces_plane_create);
static_row (store, &iter2, "Copy", static_interfaces_plane_copy);
static_row (store, &iter2, "Select", static_interfaces_plane_select);
static_row (store, &iter2, "Link", static_interfaces_plane_link);
static_row (store, &iter2, "Modify", static_interfaces_plane_modify);
static_row (store, &iter2, "Measure", static_interfaces_plane_measure);
static_row (store, &iter2, "Remove", static_interfaces_plane_remove);

iter2 = static_row (store, &iter1, "Direction", static_interfaces_direction);
static_row (store, &iter2, "Create", static_interfaces_direction_create);
static_row (store, &iter2, "Copy", static_interfaces_direction_copy);
static_row (store, &iter2, "Select", static_interfaces_direction_select);
static_row (store, &iter2, "Link", static_interfaces_direction_link);
static_row (store, &iter2, "Modify", static_interfaces_direction_modify);
static_row (store, &iter2, "Measure", static_interfaces_direction_measure);
static_row (store, &iter2, "Remove", static_interfaces_direction_remove);

iter2 = static_row (store, &iter1, "Atom", static_interfaces_atom);
static_row (store, &iter2, "Create", static_interfaces_atom_create);
static_row (store, &iter2, "Copy", static_interfaces_atom_copy);
static_row (store, &iter2, "Select", static_interfaces_atom_select);
static_row (store, &iter2, "Link", static_interfaces_atom_link);
static_row (store, &iter2, "Modify", static_interfaces_atom_modify);
static_row (store, &iter2, "Measure", static_interfaces_atom_measure);
static_row (store, &iter2, "Remove", static_interfaces_atom_remove);
static_row (store, &iter2, "Config", static_interfaces_atom_config);

iter2 = static_row (store, &iter1, "Bond", static_interfaces_bond);
static_row (store, &iter2, "Create", static_interfaces_bond_create);
static_row (store, &iter2, "Select", static_interfaces_bond_select);
static_row (store, &iter2, "Link", static_interfaces_bond_link);
static_row (store, &iter2, "Modify", static_interfaces_bond_modify);
static_row (store, &iter2, "Remove", static_interfaces_bond_remove);
static_row (store, &iter2, "Config", static_interfaces_bond_config);

iter2 = static_row (store, &iter1, "Orbital", static_interfaces_orbital);
static_row (store, &iter2, "Create", static_interfaces_orbital_create);
static_row (store, &iter2, "Copy", static_interfaces_orbital_copy);
static_row (store, &iter2, "Select", static_interfaces_orbital_select);
static_row (store, &iter2, "Link", static_interfaces_orbital_link);
static_row (store, &iter2, "Modify", static_interfaces_orbital_modify);
static_row (store, &iter2, "Remove", static_interfaces_orbital_remove);

iter2 = static_row (store, &iter1, "Text", static_interfaces_text);
static_row (store, &iter2, "Create", static_interfaces_text_create);
static_row (store, &iter2, "Copy", static_interfaces_text_copy);
static_row (store, &iter2, "Select", static_interfaces_text_select);
static_row (store, &iter2, "Link", static_interfaces_text_link);
static_row (store, &iter2, "Modify", static_interfaces_text_modify);
static_row (store, &iter2, "Remove", static_interfaces_text_remove);
static_row (store, &iter2, "Config", static_interfaces_text_config);

iter2 = static_row (store, &iter1, "Help", static_interfaces_help);
static_row (store, &iter2, "Current", static_interfaces_help_current);
static_row (store, &iter2, "Topic", static_interfaces_help_topic);
static_row (store, &iter2, "Search", static_interfaces_help_search);
static_row (store, &iter2, "Tutorial", static_interfaces_help_tutorial);
static_row (store, &iter2, "Tips", static_interfaces_help_tips);
static_row (store, &iter2, "Start", static_interfaces_help_start);
static_row (store, &iter2, "Config", static_interfaces_help_config);

/****************
 * file formats *
 ****************/

iter1 = static_row (store, NULL, "Formats", static_formats);

iter2 = static_row (store, &iter1, "Introduction", static_formats_introduction);
static_row (store, &iter2, "Language", static_formats_introduction_language);
static_row (store, &iter2, "Elements", static_formats_introduction_elements);
static_row (store, &iter2, "Attributes", static_formats_introduction_attributes);
static_row (store, &iter2, "Headers", static_formats_introduction_headers);
static_row (store, &iter2, "Entities", static_formats_introduction_entities);
static_row (store, &iter2, "Comments", static_formats_introduction_comments);
static_row (store, &iter2, "Identifiers", static_formats_introduction_identifiers);
static_row (store, &iter2, "Defaults", static_formats_introduction_defaults);

static_row (store, &iter2, "JPEG", static_formats_introduction_jpeg);
static_row (store, &iter2, "PNG", static_formats_introduction_png);
static_row (store, &iter2, "PPM", static_formats_introduction_ppm);
static_row (store, &iter2, "PS", static_formats_introduction_ps);
static_row (store, &iter2, "TIFF", static_formats_introduction_tiff);
static_row (store, &iter2, "XYZ", static_formats_introduction_xyz);

iter2 = static_row (store, &iter1, "Gamgi", static_formats_gamgi);
static_row (store, &iter2, "Config", static_formats_gamgi_config);

iter2 = static_row (store, &iter1, "File", static_formats_file);
static_row (store, &iter2, "Config", static_formats_file_config);

iter2 = static_row (store, &iter1, "Window", static_formats_window);
static_row (store, &iter2, "Create", static_formats_window_create);
static_row (store, &iter2, "Config", static_formats_window_config);

iter2 = static_row (store, &iter1, "Layer", static_formats_layer);
static_row (store, &iter2, "Create", static_formats_layer_create);
static_row (store, &iter2, "Config", static_formats_layer_config);

iter2 = static_row (store, &iter1, "Light", static_formats_light);
static_row (store, &iter2, "Create", static_formats_light_create);
static_row (store, &iter2, "Config", static_formats_light_config);

iter2 = static_row (store, &iter1, "Assembly", static_formats_assembly);
static_row (store, &iter2, "Create", static_formats_assembly_create);

iter2 = static_row (store, &iter1, "Graph", static_formats_graph);
static_row (store, &iter2, "Create", static_formats_graph_create);

iter2 = static_row (store, &iter1, "Arrow", static_formats_arrow);
static_row (store, &iter2, "Create", static_formats_arrow_create);

iter2 = static_row (store, &iter1, "Shape", static_formats_shape);
static_row (store, &iter2, "Create", static_formats_shape_create);

iter2 = static_row (store, &iter1, "Cell", static_formats_cell);
static_row (store, &iter2, "Create", static_formats_cell_create);
static_row (store, &iter2, "Config", static_formats_cell_config);

iter2 = static_row (store, &iter1, "Cluster", static_formats_cluster);
static_row (store, &iter2, "Create", static_formats_cluster_create);

iter2 = static_row (store, &iter1, "Molecule", static_formats_molecule);
static_row (store, &iter2, "Create", static_formats_molecule_create);

iter2 = static_row (store, &iter1, "Group", static_formats_group);
static_row (store, &iter2, "Create", static_formats_group_create);

iter2 = static_row (store, &iter1, "Plane", static_formats_plane);
static_row (store, &iter2, "Create", static_formats_plane_create);

iter2 = static_row (store, &iter1, "Direction", static_formats_direction);
static_row (store, &iter2, "Create", static_formats_direction_create);

iter2 = static_row (store, &iter1, "Atom", static_formats_atom);
static_row (store, &iter2, "Create", static_formats_atom_create);
static_row (store, &iter2, "Config", static_formats_atom_config);

iter2 = static_row (store, &iter1, "Bond", static_formats_bond);
static_row (store, &iter2, "Create", static_formats_bond_create);
static_row (store, &iter2, "Config", static_formats_bond_config);

iter2 = static_row (store, &iter1, "Orbital", static_formats_orbital);
static_row (store, &iter2, "Create", static_formats_orbital_create);

iter2 = static_row (store, &iter1, "Text", static_formats_text);
static_row (store, &iter2, "Create", static_formats_text_create);
static_row (store, &iter2, "Config", static_formats_text_config);

iter2 = static_row (store, &iter1, "Help", static_formats_help);
static_row (store, &iter2, "Config", static_formats_help_config);

gtk_tree_view_set_model (GTK_TREE_VIEW (treeview), GTK_TREE_MODEL (store));
g_object_unref (store);
}

/********************* external function ************************
 *                                                              *
 *                   GAMGI_GTK_HELP_TOPIC                       *
 *                                                              *
 *                                                              *
 ****************************************************************/

void gamgi_gtk_help_topic (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
GtkWidget *dialog;
GtkWidget *scrolled_window;
GtkWidget *button;
GtkWidget *vbox_dialog;
GtkWidget *hbox_center;
GtkWidget *treeview;
GtkCellRenderer *renderer;
GtkTreeViewColumn *column;
GtkTreeSelection *selection;

dialog = gamgi_gtk_dialog_task0_create ("Help Topic", window);
window->action = GAMGI_GTK_HELP_TOPIC;
gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);

vbox_dialog = gtk_vbox_new (FALSE, 5);
gtk_container_add (GTK_CONTAINER (dialog), vbox_dialog);
gtk_widget_show (vbox_dialog);

scrolled_window = gtk_scrolled_window_new (NULL, NULL);
gtk_box_pack_start (GTK_BOX (vbox_dialog), scrolled_window, TRUE, TRUE, 0);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC); 
gtk_widget_show (scrolled_window);
  
treeview = gtk_tree_view_new ();
gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
gtk_widget_set_size_request (treeview,
GAMGI_GTK_HELP_TOPIC_WIDTH, GAMGI_GTK_HELP_TOPIC_HEIGHT);
selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview));
gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
g_signal_connect (treeview, "row-activated",
G_CALLBACK (static_activated), window);
g_object_set_data (G_OBJECT (dialog), "treeview", treeview);
gtk_widget_show (treeview);

renderer = gtk_cell_renderer_text_new ();
column = gtk_tree_view_column_new_with_attributes ("Object", renderer, "text", 0, NULL);
gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);
gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (treeview), FALSE);

static_contents (treeview);

/*********************
 * Ok/Cancel buttons *
 *********************/

hbox_center = gtk_hbox_new (TRUE, 0);
gtk_box_pack_start (GTK_BOX (vbox_dialog), hbox_center, FALSE, FALSE, 0);
gtk_widget_show (hbox_center);

button = gamgi_gtk_dialog_button_create ("Ok", NULL);
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
g_signal_connect (button, "clicked",
G_CALLBACK (static_ok), window);
gtk_widget_show (button);

button = gamgi_gtk_dialog_button_create ("Cancel", "red");
gtk_box_pack_start (GTK_BOX (hbox_center), button, FALSE, FALSE, 0);
gtk_widget_set_size_request (button, GAMGI_GTK_BUTTON_WIDTH, -1);
gtk_widget_grab_focus (button);
g_signal_connect (button, "clicked",
G_CALLBACK (gamgi_gtk_dialog_task0_remove), window);
gtk_widget_show (button);

gtk_widget_show (dialog);
}
