/************************************************
 *
 * $GAMGI/src/gtk/help/gamgi_gtk_help_current.c
 *
 * Copyright (C) 2004 Carlos Pereira
 *
 * Distributed under the terms of the GNU
 * General Public License: $GAMGI/LICENSE
 *
 */

#include "gamgi_engine.h"
#include "gamgi_gtk.h"
#include "gamgi_global.h"

#include "gamgi_gtk_dialog.h"
#include "gamgi_gtk_help_dialog.h"

/************************ external function ************************
 *                                                                 *
 *                    GAMGI_GTK_HELP_CURRENT                       *
 *                                                                 *
 * Describes the features related with the topic currently active. *
 *                                                                 *
 *******************************************************************/

void gamgi_gtk_help_current (GtkWidget *widget, void *data)
{
gamgi_window *window = GAMGI_CAST_WINDOW data;
int n;

n = gamgi_gtk_dialog_page (window);

switch (window->action)
  {
  case GAMGI_GTK_HELP_TOPIC:
  gamgi_gtk_help_dialog_import ("Help Help Topic", 
  "interfaces/help/topic", window, 0, n);
  break;

  case GAMGI_GTK_HELP_SEARCH:
  gamgi_gtk_help_dialog_import ("Help Help Search", 
  "interfaces/help/search", window, 0, n);
  break;

  case GAMGI_GTK_HELP_TUTORIAL:
  gamgi_gtk_help_dialog_import ("Help Help Tutorial",
  "interfaces/help/tutorial", window, 0, n);
  break;

  case GAMGI_GTK_HELP_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Help Config", 
  "interfaces/help/config", window, 2, n, "source", "agent");
  break;

  case GAMGI_GTK_TEXT_CREATE:
  gamgi_gtk_help_dialog_import ("Help Text Create",
  "interfaces/text/create", window, 3, n, "contents", "position", "view");
  break;

  case GAMGI_GTK_TEXT_COPY:
  gamgi_gtk_help_dialog_import ("Help Text Copy", 
  "interfaces/text/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_TEXT_SELECT:
  gamgi_gtk_help_dialog_import ("Help Text Select",  
  "interfaces/text/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_TEXT_LINK:
  gamgi_gtk_help_dialog_import ("Help Text Link",  
  "interfaces/text/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_TEXT_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Text Modify",  
  "interfaces/text/modify", window, 3, n, "contents", "position", "view");
  break;

  case GAMGI_GTK_TEXT_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Text Remove",  
  "interfaces/text/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_TEXT_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Text Config", 
  "interfaces/text/config", window, 1, n, "view");
  break;

  case GAMGI_GTK_ORBITAL_CREATE:
  gamgi_gtk_help_dialog_import ("Help Orbital Create",  
  "interfaces/orbital/create", window, 5, n,
  "type", "scope", "volume", "position", "view");
  break;

  case GAMGI_GTK_ORBITAL_COPY:
  gamgi_gtk_help_dialog_import ("Help Orbital Copy",  
  "interfaces/orbital/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ORBITAL_SELECT:
  gamgi_gtk_help_dialog_import ("Help Orbital Select",  
  "interfaces/orbital/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_ORBITAL_LINK:
  gamgi_gtk_help_dialog_import ("Help Orbital Link",  
  "interfaces/orbital/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ORBITAL_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Orbital Modify",  
  "interfaces/orbital/modify", window, 5, n,
  "type", "scope", "volume", "position", "view");
  break;

  case GAMGI_GTK_ORBITAL_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Orbital Remove",  
  "interfaces/orbital/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_BOND_CREATE:
  gamgi_gtk_help_dialog_import ("Help Bond Create",  
  "interfaces/bond/create", window, 4, n, "type", "length", 
  "property", "view");
  break;

  case GAMGI_GTK_BOND_COPY:
  gamgi_gtk_help_dialog_import ("Help Bond Copy",  
  "interfaces/bond/copy", window, 1, n, "object");
  break;

  case GAMGI_GTK_BOND_SELECT:
  gamgi_gtk_help_dialog_import ("Help Bond Select",  
  "interfaces/bond/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_BOND_LINK:
  gamgi_gtk_help_dialog_import ("Help Bond Link",  
  "interfaces/bond/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_BOND_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Bond Modify",  
  "interfaces/bond/modify", window, 2, n, "property", "view");
  break;

  case GAMGI_GTK_BOND_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Bond Remove",  
  "interfaces/bond/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_BOND_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Bond Config", 
  "interfaces/bond/config", window, 2, n, "length", "view");
  break;

  case GAMGI_GTK_ATOM_CREATE:
  gamgi_gtk_help_dialog_import ("Help Atom Create",  
  "interfaces/atom/create", window, 5, n, "type",
  "property", "analysis", "position", "view");
  break;

  case GAMGI_GTK_ATOM_COPY:
  gamgi_gtk_help_dialog_import ("Help Atom Copy",  
  "interfaces/atom/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ATOM_SELECT:
  gamgi_gtk_help_dialog_import ("Help Atom Select",  
  "interfaces/atom/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_ATOM_LINK:
  gamgi_gtk_help_dialog_import ("Help Atom Link",  
  "interfaces/atom/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ATOM_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Atom Modify",  
  "interfaces/atom/modify", window, 7, n, "type",
  "property", "analysis", "position", "view", "length",
  "angle");
  break;

  case GAMGI_GTK_ATOM_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Atom Measure", 
  "interfaces/atom/measure", window, 2, n, "length", "angle");
  break;

  case GAMGI_GTK_ATOM_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Atom Remove",  
  "interfaces/atom/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_ATOM_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Atom Config",
  "interfaces/atom/config", window, 4, n, "type",
  "analysis", "view", "global");
  break;

  case GAMGI_GTK_DIRECTION_CREATE:
  gamgi_gtk_help_dialog_import ("Help Direction Create",  
  "interfaces/direction/create", window, 4, n, "type", 
  "projection", "scope", "view");
  break;

  case GAMGI_GTK_DIRECTION_COPY:
  gamgi_gtk_help_dialog_import ("Help Direction Copy",  
  "interfaces/direction/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_DIRECTION_SELECT:
  gamgi_gtk_help_dialog_import ("Help Direction Select",  
  "interfaces/direction/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_DIRECTION_LINK:
  gamgi_gtk_help_dialog_import ("Help Direction Link",  
  "interfaces/direction/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_DIRECTION_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Direction Modify",  
  "interfaces/direction/modify", window, 5, n, "type",
  "projection", "scope", "position", "view");
  break;

  case GAMGI_GTK_DIRECTION_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Direction Measure",  
  "interfaces/direction/measure", window, 2, n, "length", "angle");
  break;

  case GAMGI_GTK_DIRECTION_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Direction Remove",  
  "interfaces/direction/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_PLANE_CREATE:
  gamgi_gtk_help_dialog_import ("Help Plane Create",  
  "interfaces/plane/create", window, 4, n, "type", 
  "projection", "scope", "view");
  break;

  case GAMGI_GTK_PLANE_COPY:
  gamgi_gtk_help_dialog_import ("Help Plane Copy",  
  "interfaces/plane/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_PLANE_SELECT:
  gamgi_gtk_help_dialog_import ("Help Plane Select",  
  "interfaces/plane/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_PLANE_LINK:
  gamgi_gtk_help_dialog_import ("Help Plane Link",  
  "interfaces/plane/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_PLANE_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Plane Modify",  
  "interfaces/plane/modify", window, 5, n, "type",
  "projection", "scope", "position", "view");
  break;

  case GAMGI_GTK_PLANE_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Plane Measure",  
  "interfaces/plane/measure", window, 3, n, "length", "area", "angle");
  break;

  case GAMGI_GTK_PLANE_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Plane Remove",  
  "interfaces/plane/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_GROUP_CREATE:
  gamgi_gtk_help_dialog_import ("Help Group Create",  
  "interfaces/group/create", window, 3, n, "type",
  "scope", "view");
  break;

  case GAMGI_GTK_GROUP_CREATE_POLYTOPE:
  gamgi_gtk_help_dialog_import ("Help Group Create Polytope",
  "interfaces/group/create_polytope", window, 0, n);
  break;

  case GAMGI_GTK_GROUP_COPY:
  gamgi_gtk_help_dialog_import ("Help Group Copy",  
  "interfaces/group/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_GROUP_SELECT:
  gamgi_gtk_help_dialog_import ("Help Group Select",  
  "interfaces/group/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_GROUP_LINK:
  gamgi_gtk_help_dialog_import ("Help Group Link",  
  "interfaces/group/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_GROUP_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Group Modify",  
  "interfaces/group/modify", window, 4, n, "type", 
  "scope", "position", "view");
  break;

  case GAMGI_GTK_GROUP_MODIFY_POLYTOPE:
  gamgi_gtk_help_dialog_import ("Help Group Modify Polytope",
  "interfaces/group/modify_polytope", window, 0, n);
  break;

  case GAMGI_GTK_GROUP_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Group Measure",
  "interfaces/group/measure", window, 5, n, "type",
  "symmetry", "voronoi", "tolerance", "view");
  break;

  case GAMGI_GTK_GROUP_MEASURE_GEOMETRY:
  gamgi_gtk_help_dialog_import ("Help Group Measure Geometry",
  "interfaces/group/geometry", window, 0, n);
  break;
  
  case GAMGI_GTK_GROUP_MEASURE_SYMMETRY:
  gamgi_gtk_help_dialog_import ("Help Group Measure Symmetry",
  "interfaces/group/symmetry", window, 0, n);
  break;
  
  case GAMGI_GTK_GROUP_MEASURE_VORONOI:
  gamgi_gtk_help_dialog_import ("Help Group Measure Voronoi",
  "interfaces/group/voronoi", window, 0, n);
  break;

  case GAMGI_GTK_GROUP_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Group Remove",  
  "interfaces/group/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_MOLECULE_CREATE:
  gamgi_gtk_help_dialog_import ("Help Molecule Create",  
  "interfaces/molecule/create", window, 1, n, "type");
  break;

  case GAMGI_GTK_MOLECULE_COPY:
  gamgi_gtk_help_dialog_import ("Help Molecule Copy",  
  "interfaces/molecule/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_MOLECULE_SELECT:
  gamgi_gtk_help_dialog_import ("Help Molecule Select",  
  "interfaces/molecule/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_MOLECULE_LINK:
  gamgi_gtk_help_dialog_import ("Help Molecule Link",  
  "interfaces/molecule/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_MOLECULE_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Molecule Modify",  
  "interfaces/molecule/modify", window, 0, n);
  break;

  case GAMGI_GTK_MOLECULE_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Molecule Measure",  
  "interfaces/molecule/measure", window, 5, n, "type", 
  "symmetry", "voronoi", "tolerance", "view");
  break;

  case GAMGI_GTK_MOLECULE_MEASURE_GEOMETRY:
  gamgi_gtk_help_dialog_import ("Help Molecule Measure Geometry",  
  "interfaces/molecule/geometry", window, 0, n);
  break;

  case GAMGI_GTK_MOLECULE_MEASURE_SYMMETRY:
  gamgi_gtk_help_dialog_import ("Help Molecule Measure Symmetry",
  "interfaces/molecule/symmetry", window, 0, n);
  break;

  case GAMGI_GTK_MOLECULE_MEASURE_VORONOI:
  gamgi_gtk_help_dialog_import ("Help Molecule Measure Voronoi",
  "interfaces/molecule/voronoi", window, 0, n);
  break;

  case GAMGI_GTK_MOLECULE_ANIMATE:
  gamgi_gtk_help_dialog_import ("Help Molecule Animate",  
  "interfaces/molecule/animate", window, 0, n);
  break;

  case GAMGI_GTK_MOLECULE_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Molecule Remove",  
  "interfaces/molecule/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_CLUSTER_CREATE:
  gamgi_gtk_help_dialog_import ("Help Cluster Create",  
  "interfaces/cluster/create", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_COPY:
  gamgi_gtk_help_dialog_import ("Help Cluster Copy",  
  "interfaces/cluster/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_CLUSTER_SELECT:
  gamgi_gtk_help_dialog_import ("Help Cluster Select",  
  "interfaces/cluster/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_CLUSTER_LINK:
  gamgi_gtk_help_dialog_import ("Help Cluster Link",  
  "interfaces/cluster/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_CLUSTER_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Cluster Modify",  
  "interfaces/cluster/modify", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Cluster Measure",  
  "interfaces/cluster/measure", window, 5, n, "type",
  "symmetry", "voronoi", "tolerance", "view");
  break;

  case GAMGI_GTK_CLUSTER_MEASURE_GEOMETRY:
  gamgi_gtk_help_dialog_import ("Help Cluster Measure Geometry",
  "interfaces/cluster/geometry", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_MEASURE_SYMMETRY:
  gamgi_gtk_help_dialog_import ("Help Cluster Measure Symmetry",
  "interfaces/cluster/symmetry", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_MEASURE_VORONOI:
  gamgi_gtk_help_dialog_import ("Help Cluster Measure Voronoi",  
  "interfaces/cluster/voronoi", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_ANIMATE:
  gamgi_gtk_help_dialog_import ("Help Cluster Animate",  
  "interfaces/cluster/animate", window, 0, n);
  break;

  case GAMGI_GTK_CLUSTER_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Cluster Remove",  
  "interfaces/cluster/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_CELL_CREATE:
  gamgi_gtk_help_dialog_import ("Help Cell Create",  
  "interfaces/cell/create", window, 5, n, "symmetry", 
  "volume", "position", "origin", "view");
  break;

  case GAMGI_GTK_CELL_COPY:
  gamgi_gtk_help_dialog_import ("Help Cell Copy",  
  "interfaces/cell/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_CELL_SELECT:
  gamgi_gtk_help_dialog_import ("Help Cell Select",  
  "interfaces/cell/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_CELL_LINK:
  gamgi_gtk_help_dialog_import ("Help Cell Link",  
  "interfaces/cell/link", window, 6, n, "object", 
  "shift", "position", "change", "occupancy", "packing");
  break;

  case GAMGI_GTK_CELL_LINK_RCP:
  gamgi_gtk_help_dialog_import ("Help Cell Link RCP",
  "interfaces/cell/rcp", window, 0, n);
  break;

  case GAMGI_GTK_CELL_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Cell Modify",  
  "interfaces/cell/modify", window, 5, n, "symmetry", 
  "volume", "position", "origin", "view");
  break;

  case GAMGI_GTK_CELL_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Cell Measure",  
  "interfaces/cell/measure", window, 5, n, "type",
  "reciprocal", "voronoi", "tolerance", "view");
  break;

  case GAMGI_GTK_CELL_MEASURE_GEOMETRY:
  gamgi_gtk_help_dialog_import ("Help Cell Measure Geometry",
  "interfaces/cell/geometry", window, 0, n);
  break;

  case GAMGI_GTK_CELL_MEASURE_VORONOI:
  gamgi_gtk_help_dialog_import ("Help Cell Measure Voronoi",
  "interfaces/cell/voronoi", window, 0, n);
  break;

  case GAMGI_GTK_CELL_ANIMATE:
  gamgi_gtk_help_dialog_import ("Help Cell Animate",  
  "interfaces/cell/animate", window, 0, n);
  break;

  case GAMGI_GTK_CELL_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Cell Remove",  
  "interfaces/cell/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_CELL_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Cell Config",
  "interfaces/cell/config", window, 4, n, "volume",
  "origin", "view", "global");
  break;

  case GAMGI_GTK_ARROW_CREATE:
  gamgi_gtk_help_dialog_import ("Help Arrow Create",  
  "interfaces/arrow/create", window, 0, n);
  break;

  case GAMGI_GTK_ARROW_COPY:
  gamgi_gtk_help_dialog_import ("Help Arrow Copy",  
  "interfaces/arrow/copy", window, 0, n);
  break;

  case GAMGI_GTK_ARROW_SELECT:
  gamgi_gtk_help_dialog_import ("Help Arrow Select",  
  "interfaces/arrow/select", window, 0, n);
  break;

  case GAMGI_GTK_ARROW_LINK:
  gamgi_gtk_help_dialog_import ("Help Arrow Link",  
  "interfaces/arrow/link", window, 0, n);
  break;

  case GAMGI_GTK_ARROW_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Arrow Modify",  
  "interfaces/arrow/modify", window, 0, n);
  break;

  case GAMGI_GTK_ARROW_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Arrow Remove",  
  "interfaces/arrow/remove", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_CREATE:
  gamgi_gtk_help_dialog_import ("Help Shape Create",  
  "interfaces/shape/create", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_COPY:
  gamgi_gtk_help_dialog_import ("Help Shape Copy",  
  "interfaces/shape/copy", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_SELECT:
  gamgi_gtk_help_dialog_import ("Help Shape Select",  
  "interfaces/shape/select", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_LINK:
  gamgi_gtk_help_dialog_import ("Help Shape Link",  
  "interfaces/shape/link", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Shape Modify",  
  "interfaces/shape/modify", window, 0, n);
  break;

  case GAMGI_GTK_SHAPE_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Shape Remove",  
  "interfaces/shape/remove", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_CREATE:
  gamgi_gtk_help_dialog_import ("Help Graph Create",  
  "interfaces/graph/create", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_COPY:
  gamgi_gtk_help_dialog_import ("Help Graph Copy",  
  "interfaces/graph/copy", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_SELECT:
  gamgi_gtk_help_dialog_import ("Help Graph Select",  
  "interfaces/graph/select", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_LINK:
  gamgi_gtk_help_dialog_import ("Help Graph Link",  
  "interfaces/graph/link", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Graph Modify",  
  "interfaces/graph/modify", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_MEASURE:
  gamgi_gtk_help_dialog_import ("Help Graph Measure",  
  "interfaces/graph/measure", window, 0, n);
  break;

  case GAMGI_GTK_GRAPH_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Graph Remove",  
  "interfaces/graph/remove", window, 0, n);
  break;

  case GAMGI_GTK_ASSEMBLY_CREATE:
  gamgi_gtk_help_dialog_import ("Help Assembly Create",  
  "interfaces/assembly/create", window, 0, n);
  break;

  case GAMGI_GTK_ASSEMBLY_COPY:
  gamgi_gtk_help_dialog_import ("Help Assembly Copy",  
  "interfaces/assembly/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ASSEMBLY_SELECT:
  gamgi_gtk_help_dialog_import ("Help Assembly Select",  
  "interfaces/assembly/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_ASSEMBLY_LINK:
  gamgi_gtk_help_dialog_import ("Help Assembly Link",  
  "interfaces/assembly/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_ASSEMBLY_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Assembly Modify",  
  "interfaces/assembly/modify", window, 0, n);
  break;

  case GAMGI_GTK_ASSEMBLY_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Assembly Remove",  
  "interfaces/assembly/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_LIGHT_CREATE:
  gamgi_gtk_help_dialog_import ("Help Light Create",  
  "interfaces/light/create", window, 3, n, "geometry", "color", "attenuation");
  break;

  case GAMGI_GTK_LIGHT_COPY:
  gamgi_gtk_help_dialog_import ("Help Light Copy",  
  "interfaces/light/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_LIGHT_SELECT:
  gamgi_gtk_help_dialog_import ("Help Light Select",  
  "interfaces/light/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_LIGHT_LINK:
  gamgi_gtk_help_dialog_import ("Help Light Link",  
  "interfaces/light/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_LIGHT_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Light Modify",  
  "interfaces/light/modify", window, 3, n, "geometry", "color", "attenuation");
  break;

  case GAMGI_GTK_LIGHT_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Light Remove",  
  "interfaces/light/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_LIGHT_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Light Config", 
  "interfaces/light/config", window, 2, n, "color", "global");
  break;

  case GAMGI_GTK_LAYER_CREATE:
  gamgi_gtk_help_dialog_import ("Help Layer Create",  
  "interfaces/layer/create", window, 4, n, "projection", 
  "position", "visibility", "view");
  break;

  case GAMGI_GTK_LAYER_COPY:
  gamgi_gtk_help_dialog_import ("Help Layer Copy",  
  "interfaces/layer/copy", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_LAYER_SELECT:
  gamgi_gtk_help_dialog_import ("Help Layer Select",  
  "interfaces/layer/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_LAYER_LINK:
  gamgi_gtk_help_dialog_import ("Help Layer Link",  
  "interfaces/layer/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_LAYER_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Layer Modify",  
  "interfaces/layer/modify", window, 4, n, "projection",
  "position", "visibility", "view");
  break;

  case GAMGI_GTK_LAYER_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Layer Remove",  
  "interfaces/layer/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_LAYER_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Layer Config", 
  "interfaces/layer/config", window, 3, n, "projection", 
  "position", "history");
  break;

  case GAMGI_GTK_WINDOW_CREATE:
  gamgi_gtk_help_dialog_import ("Help Window Create",  
  "interfaces/window/create", window, 1, n, "position");
  break;

  case GAMGI_GTK_WINDOW_COPY:
  gamgi_gtk_help_dialog_import ("Help Window Copy",  
  "interfaces/window/copy", window, 1, n, "object");
  break;

  case GAMGI_GTK_WINDOW_SELECT:
  gamgi_gtk_help_dialog_import ("Help Window Select",  
  "interfaces/window/select", window, 1, n, "list");
  break;

  case GAMGI_GTK_WINDOW_LINK:
  gamgi_gtk_help_dialog_import ("Help Window Link",  
  "interfaces/window/link", window, 2, n, "object", "shift");
  break;

  case GAMGI_GTK_WINDOW_MODIFY:
  gamgi_gtk_help_dialog_import ("Help Window Modify",  
  "interfaces/window/modify", window, 0, n);
  break;

  case GAMGI_GTK_WINDOW_REMOVE:
  gamgi_gtk_help_dialog_import ("Help Window Remove",  
  "interfaces/window/remove", window, 1, n, "object");
  break;

  case GAMGI_GTK_WINDOW_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Window Config", 
  "interfaces/window/config", window, 1, n, "ruler");
  break;

  case GAMGI_GTK_FILE_IMPORT:
  gamgi_gtk_help_dialog_import ("Help File Import", 
  "interfaces/file/import", window, 2, n, "location", "filters");
  break;

  case GAMGI_GTK_FILE_EXPORT:
  gamgi_gtk_help_dialog_import ("Help File Export", 
  "interfaces/file/export", window, 2, n, "location", "filters");
  break;

  case GAMGI_GTK_GAMGI_ABOUT:
  gamgi_gtk_help_dialog_import ("Help Gamgi About",  
  "interfaces/gamgi/about", window, 0, n);
  break;

  case GAMGI_GTK_GAMGI_RESET:
  gamgi_gtk_help_dialog_import ("Help Gamgi Reset",  
  "interfaces/gamgi/reset", window, 0, n);
  break;

  case GAMGI_GTK_GAMGI_EXIT:
  gamgi_gtk_help_dialog_import ("Help Gamgi Exit", 
  "interfaces/gamgi/exit", window, 0, n);
  break;

  case GAMGI_GTK_GAMGI_CONFIG:
  gamgi_gtk_help_dialog_import ("Help Gamgi Config", 
  "interfaces/gamgi/config", window, 3, n, "usability", "precision", "color");
  break;

  default:
  gamgi_gtk_help_dialog_import ("Help Main",
  "main", window, 0, n);
  break;
  }
}
