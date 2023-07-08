###########################################
#
# $GAMGI/src/phys/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#========================== Phys files ===============================

GAMGI_PHYS_h = gamgi_phys.h

GAMGI_PHYS_o = gamgi_phys_cell.o gamgi_phys_cell_link.o \
	gamgi_phys_direction.o \
	gamgi_phys_plane.o gamgi_phys_projection.o \
	gamgi_phys_rcp.o gamgi_phys_reciprocal.o \
	gamgi_phys_space.o gamgi_phys_symmetry.o \
	gamgi_phys_voronoi.o gamgi_phys_wigner.o

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
