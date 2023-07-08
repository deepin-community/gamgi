###########################################
#
# $GAMGI/src/math/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#========================== Math files ===============================

GAMGI_MATH_h = gamgi_math.h

GAMGI_MATH_o = gamgi_math_cell.o gamgi_math_diophantine.o \
	gamgi_math_euler.o gamgi_math_measure.o \
	gamgi_math_modify.o gamgi_math_matrix.o \
	gamgi_math_node.o gamgi_math_polygon.o \
	gamgi_math_position.o gamgi_math_quaternion.o \
	gamgi_math_sort.o gamgi_math_vector.o \
	gamgi_math_hash.o

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
