###########################################
#
# $GAMGI/src/gtk/plane/makefile
#
# Copyright (C) Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#=========================== Gtk files ===============================

GAMGI_GTK_PLANE_h = 

GAMGI_GTK_PLANE_o = gamgi_gtk_plane_create.o \
	gamgi_gtk_plane_copy.o gamgi_gtk_plane_link.o \
	gamgi_gtk_plane_list.o gamgi_gtk_plane_measure.o \
	gamgi_gtk_plane_modify.o gamgi_gtk_plane_node.o \
	gamgi_gtk_plane_remove.o gamgi_gtk_plane_select.o

#========================= Gamgi paths ===============================

ifndef FLAG
FLAG = 1

SRC = ../../

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
