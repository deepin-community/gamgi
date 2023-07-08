###########################################
#
# $GAMGI/src/mesa/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#========================== Mesa files ===============================

GAMGI_MESA_h = gamgi_mesa.h

GAMGI_MESA_o = gamgi_mesa_area.o gamgi_mesa_axes.o \
	gamgi_mesa_atom.o gamgi_mesa_bond.o \
	gamgi_mesa_center.o gamgi_mesa_draw.o \
	gamgi_mesa_grab.o gamgi_mesa_lists.o \
	gamgi_mesa_pick.o gamgi_mesa_render.o \
	gamgi_mesa_rotate.o gamgi_mesa_rotate_out.o \
	gamgi_mesa_select.o gamgi_mesa_scale.o \
	gamgi_mesa_start.o gamgi_mesa_trackball.o \
	gamgi_mesa_text.o gamgi_mesa_translate.o \
	gamgi_mesa_translate_out.o

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
