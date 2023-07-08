###########################################
#
# $GAMGI/src/io/makefile
#
# Copyright (C) 2001, 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#============================ Io files ===============================

GAMGI_IO_h = gamgi_io.h

GAMGI_IO_o = gamgi_io_error.o gamgi_io_file.o \
	gamgi_io_font_hershey.o gamgi_io_font_sun.o \
	gamgi_io_socket.o gamgi_io_token.o gamgi_io_x3d.o \
	gamgi_io_x3d_render.o gamgi_io_xyz.o

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
