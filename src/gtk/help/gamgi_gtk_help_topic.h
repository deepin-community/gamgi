/**********************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_topic.h
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

static void static_interfaces (void *data);

static void static_interfaces_introduction (void *data);

static void static_interfaces_introduction_objects (void *data);

static void static_interfaces_introduction_tasks (void *data);

static void static_interfaces_introduction_windows (void *data);

static void static_interfaces_introduction_starting (void *data);

static void static_interfaces_introduction_identification (void *data);

static void static_interfaces_introduction_selection (void *data);

static void static_interfaces_introduction_mouse (void *data);

static void static_interfaces_introduction_keyboard (void *data);

static void static_interfaces_introduction_menus (void *data);

static void static_interfaces_introduction_drawing (void *data);

static void static_interfaces_introduction_tools (void *data);

static void static_interfaces_introduction_commands (void *data);

static void static_interfaces_introduction_status (void *data);

static void static_interfaces_gamgi (void *data);

static void static_interfaces_gamgi_about (void *data);

static void static_interfaces_gamgi_reset (void *data);

static void static_interfaces_gamgi_exit (void *data);

static void static_interfaces_gamgi_config (void *data);

static void static_interfaces_file (void *data);

static void static_interfaces_file_import (void *data);

static void static_interfaces_file_export (void *data);

static void static_interfaces_window (void *data);

static void static_interfaces_window_select (void *data);

static void static_interfaces_window_link (void *data);

static void static_interfaces_window_remove (void *data);

static void static_interfaces_window_config (void *data);

static void static_interfaces_layer (void *data);

static void static_interfaces_layer_create (void *data);

static void static_interfaces_layer_select (void *data);

static void static_interfaces_layer_link (void *data);

static void static_interfaces_layer_modify (void *data);

static void static_interfaces_layer_remove (void *data);

static void static_interfaces_layer_config (void *data);

static void static_interfaces_light (void *data);

static void static_interfaces_light_create (void *data);

static void static_interfaces_light_select (void *data);

static void static_interfaces_light_link (void *data);

static void static_interfaces_light_modify (void *data);

static void static_interfaces_light_remove (void *data);

static void static_interfaces_light_config (void *data);

static void static_interfaces_assembly (void *data);

static void static_interfaces_assembly_create (void *data);

static void static_interfaces_assembly_select (void *data);

static void static_interfaces_assembly_link (void *data);

static void static_interfaces_assembly_remove (void *data);

static void static_interfaces_graph (void *data);

static void static_interfaces_graph_select (void *data);

static void static_interfaces_shape (void *data);

static void static_interfaces_shape_select (void *data);

static void static_interfaces_arrow (void *data);

static void static_interfaces_arrow_select (void *data);

static void static_interfaces_cell (void *data);

static void static_interfaces_cell_create (void *data);

static void static_interfaces_cell_select (void *data);

static void static_interfaces_cell_link (void *data);

static void static_interfaces_cell_link_rcp (void *data);

static void static_interfaces_cell_modify (void *data);

static void static_interfaces_cell_measure (void *data);

static void static_interfaces_cell_remove (void *data);

static void static_interfaces_cluster (void *data);

static void static_interfaces_cluster_create (void *data);

static void static_interfaces_cluster_select (void *data);

static void static_interfaces_cluster_link (void *data);

static void static_interfaces_cluster_measure (void *data);

static void static_interfaces_cluster_measure_geometry (void *data);

static void static_interfaces_cluster_measure_symmetry (void *data);

static void static_interfaces_cluster_measure_voronoi (void *data);

static void static_interfaces_cluster_remove (void *data);

static void static_interfaces_molecule (void *data);

static void static_interfaces_molecule_create (void *data);

static void static_interfaces_molecule_select (void *data);

static void static_interfaces_molecule_link (void *data);

static void static_interfaces_molecule_measure (void *data);

static void static_interfaces_molecule_measure_geometry (void *data);

static void static_interfaces_molecule_measure_symmetry (void *data);

static void static_interfaces_molecule_measure_voronoi (void *data);

static void static_interfaces_molecule_remove (void *data);

static void static_interfaces_group (void *data);

static void static_interfaces_group_create (void *data);

static void static_interfaces_group_create_polytope (void *data);

static void static_interfaces_group_select (void *data);

static void static_interfaces_group_link (void *data);

static void static_interfaces_group_modify (void *data);

static void static_interfaces_group_modify_polytope (void *data);

static void static_interfaces_group_measure (void *data);

static void static_interfaces_group_measure_geometry (void *data);

static void static_interfaces_group_measure_symmetry (void *data);

static void static_interfaces_group_measure_voronoi (void *data);

static void static_interfaces_group_remove (void *data);

static void static_interfaces_plane (void *data);

static void static_interfaces_plane_create (void *data);

static void static_interfaces_plane_select (void *data);

static void static_interfaces_plane_link (void *data);

static void static_interfaces_plane_modify (void *data);

static void static_interfaces_plane_measure (void *data);

static void static_interfaces_plane_remove (void *data);

static void static_interfaces_direction (void *data);

static void static_interfaces_direction_create (void *data);

static void static_interfaces_direction_select (void *data);

static void static_interfaces_direction_link (void *data);

static void static_interfaces_direction_modify (void *data);

static void static_interfaces_direction_measure (void *data);

static void static_interfaces_direction_remove (void *data);

static void static_interfaces_atom (void *data);

static void static_interfaces_atom_create (void *data);

static void static_interfaces_atom_copy (void *data);

static void static_interfaces_atom_select (void *data);

static void static_interfaces_atom_link (void *data);

static void static_interfaces_atom_modify (void *data);

static void static_interfaces_atom_measure (void *data);

static void static_interfaces_atom_remove (void *data);

static void static_interfaces_atom_config (void *data);

static void static_interfaces_bond (void *data);

static void static_interfaces_bond_create (void *data);

static void static_interfaces_bond_select (void *data);

static void static_interfaces_bond_link (void *data);

static void static_interfaces_bond_modify (void *data);

static void static_interfaces_bond_remove (void *data);

static void static_interfaces_bond_config (void *data);

static void static_interfaces_orbital (void *data);

static void static_interfaces_orbital_create (void *data);

static void static_interfaces_orbital_copy (void *data);

static void static_interfaces_orbital_select (void *data);

static void static_interfaces_orbital_link (void *data);

static void static_interfaces_orbital_modify (void *data);

static void static_interfaces_orbital_remove (void *data);

static void static_interfaces_text (void *data);

static void static_interfaces_text_create (void *data);

static void static_interfaces_text_select (void *data);

static void static_interfaces_text_link (void *data);

static void static_interfaces_text_modify (void *data);

static void static_interfaces_text_remove (void *data);

static void static_interfaces_text_config (void *data);

static void static_interfaces_help (void *data);

static void static_interfaces_help_current (void *data);

static void static_interfaces_help_topic (void *data);

static void static_interfaces_help_search (void *data);

static void static_interfaces_help_tutorial (void *data);

static void static_interfaces_help_tips (void *data);

static void static_interfaces_help_start (void *data);

static void static_interfaces_help_config (void *data);

static void static_formats (void *data);

static void static_formats_introduction (void *data);

static void static_formats_introduction_language (void *data);

static void static_formats_introduction_elements (void *data);

static void static_formats_introduction_attributes (void *data);

static void static_formats_introduction_headers (void *data);

static void static_formats_introduction_entities (void *data);

static void static_formats_introduction_comments (void *data);

static void static_formats_introduction_identifiers (void *data);

static void static_formats_introduction_defaults (void *data);

static void static_formats_introduction_jpeg (void *data);

static void static_formats_introduction_png (void *data);

static void static_formats_introduction_ppm (void *data);

static void static_formats_introduction_ps (void *data);

static void static_formats_introduction_tiff (void *data);

static void static_formats_introduction_xyz (void *data);

static void static_formats_gamgi (void *data);

static void static_formats_gamgi_config (void *data);

static void static_formats_file (void *data);

static void static_formats_file_config (void *data);

static void static_formats_window (void *data);

static void static_formats_window_create (void *data);

static void static_formats_window_config (void *data);

static void static_formats_layer (void *data);

static void static_formats_layer_create (void *data);

static void static_formats_layer_config (void *data);

static void static_formats_light (void *data);

static void static_formats_light_create (void *data);

static void static_formats_light_config (void *data);

static void static_formats_assembly (void *data);

static void static_formats_assembly_create (void *data);

static void static_formats_graph (void *data);

static void static_formats_graph_create (void *data);

static void static_formats_arrow (void *data);

static void static_formats_arrow_create (void *data);

static void static_formats_shape (void *data);

static void static_formats_shape_create (void *data);

static void static_formats_cell (void *data);

static void static_formats_cell_create (void *data);

static void static_formats_cell_config (void *data);

static void static_formats_cluster (void *data);

static void static_formats_cluster_create (void *data);

static void static_formats_molecule (void *data);

static void static_formats_molecule_create (void *data);

static void static_formats_group (void *data);

static void static_formats_group_create (void *data);

static void static_formats_plane (void *data);

static void static_formats_plane_create (void *data);

static void static_formats_direction (void *data);

static void static_formats_direction_create (void *data);

static void static_formats_atom (void *data);

static void static_formats_atom_create (void *data);

static void static_formats_atom_config (void *data);

static void static_formats_bond (void *data);

static void static_formats_bond_create (void *data);

static void static_formats_bond_config (void *data);

static void static_formats_orbital (void *data);

static void static_formats_orbital_create (void *data);

static void static_formats_text (void *data);

static void static_formats_text_create (void *data);

static void static_formats_text_config (void *data);

static void static_formats_help (void *data);

static void static_formats_help_config (void *data);

static void static_activated (GtkTreeView *treeview, 
GtkTreePath *path, GtkTreeViewColumn *col, void *data);

static void static_ok (GtkWidget *widget, void *data);

static GtkTreeIter static_row (GtkTreeStore *store, 
GtkTreeIter *parent, char *label, gamgi_function1 function);

static void static_contents (GtkWidget *treeview);

 **********************
 * external functions *
 **********************/

void gamgi_gtk_help_topic (GtkWidget *widget, void *data);

/*************
 * Rationale *
 *************

Done. Ported to Gtk-2.0 at 28/04/2008

 *************
 * Rationale *
 *************/
