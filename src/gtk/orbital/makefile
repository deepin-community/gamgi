###########################################
#
# $GAMGI/src/gtk/orbital/makefile
#
# Copyright (C) 2012 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#=========================== Gtk files ===============================

GAMGI_GTK_ORBITAL_h = 

GAMGI_GTK_ORBITAL_o =  gamgi_gtk_orbital_copy.o gamgi_gtk_orbital_create.o \
	gamgi_gtk_orbital_link.o gamgi_gtk_orbital_modify.o \
	gamgi_gtk_orbital_remove.o gamgi_gtk_orbital_select.o

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
