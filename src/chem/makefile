###########################################
#
# $GAMGI/src/chem/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#========================== Chem files ===============================

GAMGI_CHEM_h = gamgi_chem.h

GAMGI_CHEM_o = gamgi_chem_atom.o gamgi_chem_bond.o \
	gamgi_chem_bond_create.o gamgi_chem_geometry.o \
	gamgi_chem_graphene.o gamgi_chem_graphene_cone.o \
	gamgi_chem_graphene_tube.o gamgi_chem_orbital.o \
	gamgi_chem_property.o gamgi_chem_symmetry.o \
	gamgi_chem_polytope.o 

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
