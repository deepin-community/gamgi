###########################################
#
# $GAMGI/src/gtk/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#=========================== Gtk files ===============================

GAMGI_GTK_h = $(GAMGI_GTK_GAMGI_h) $(GAMGI_GTK_FILE_h) \
	$(GAMGI_GTK_WINDOW_h) $(GAMGI_GTK_LAYER_h) \
	$(GAMGI_GTK_LIGHT_h) $(GAMGI_GTK_ASSEMBLY_h) \
	$(GAMGI_GTK_CELL_h) $(GAMGI_GTK_CLUSTER_h) \
	$(GAMGI_GTK_MOLECULE_h) $(GAMGI_GTK_GROUP_h) \
	$(GAMGI_GTK_PLANE_h) $(GAMGI_GTK_DIRECTION_h) \
	$(GAMGI_GTK_ATOM_h) $(GAMGI_GTK_BOND_h) \
	$(GAMGI_GTK_ORBITAL_h) $(GAMGI_GTK_TEXT_h) \
	$(GAMGI_GTK_HELP_h) gamgi_gtk.h

GAMGI_GTK_o = $(GAMGI_GTK_GAMGI_o) $(GAMGI_GTK_FILE_o) \
	$(GAMGI_GTK_WINDOW_o) $(GAMGI_GTK_LAYER_o) \
	$(GAMGI_GTK_LIGHT_o) $(GAMGI_GTK_ASSEMBLY_o) \
	$(GAMGI_GTK_CELL_o) $(GAMGI_GTK_CLUSTER_o) \
	$(GAMGI_GTK_MOLECULE_o) $(GAMGI_GTK_GROUP_o) \
	$(GAMGI_GTK_PLANE_o) $(GAMGI_GTK_DIRECTION_o) \
	$(GAMGI_GTK_ATOM_o) $(GAMGI_GTK_BOND_o) \
	$(GAMGI_GTK_ORBITAL_o) $(GAMGI_GTK_TEXT_o) \
	$(GAMGI_GTK_HELP_o) gamgi_gtk_animate.o \
	gamgi_gtk_copy.o gamgi_gtk_create.o \
	gamgi_gtk_dialog.o gamgi_gtk_history.o \
	gamgi_gtk_key.o gamgi_gtk_link.o \
	gamgi_gtk_measure.o gamgi_gtk_modify.o \
	gamgi_gtk_object.o gamgi_gtk_remove.o \
	gamgi_gtk_select.o gamgi_gtk_statusbar.o

#========================= Gamgi paths ===============================

ifndef FLAG
FLAG = 1

SRC = ../

include $(SRC)make_paths

#========================= Gamgi files ===============================

GAMGI_o = $(GAMGI_ENGINE_o) $(GAMGI_GTK_o) $(GAMGI_MESA_o) \
	$(GAMGI_MATH_o) $(GAMGI_CHEM_o) $(GAMGI_PHYS_o) \
	$(GAMGI_IO_o) $(GAMGI_EXPAT_o) $(GAMGI_GLOBAL_o)

GAMGI_h = $(GAMGI_ENGINE_h) $(GAMGI_GTK_h) $(GAMGI_MESA_h) \
	$(GAMGI_MATH_h) $(GAMGI_CHEM_h) $(GAMGI_PHYS_h) \
	$(GAMGI_IO_h) $(GAMGI_EXPAT_h) $(GAMGI_GLOBAL_h)

#========================= Gamgi local ===============================

include $(SRC)make_local

#========================= Gamgi rules ===============================

include $(SRC)make_rules

endif
