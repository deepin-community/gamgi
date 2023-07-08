##########################################
#
# $GAMGI/src/expat/makefile
#
# Copyright (C) 2004 Carlos Pereira
#
# Distributed under the terms of the GNU
# General Public License: $GAMGI/LICENSE
#


#========================= Expat files ===============================

GAMGI_EXPAT_h = gamgi_expat.h

GAMGI_EXPAT_o = gamgi_expat_import.o gamgi_expat_import_object.o \
	gamgi_expat_import_config.o gamgi_expat_import_help.o \
	gamgi_expat_export.o gamgi_expat_export_object.o \
	gamgi_expat_export_config.o

#========================= Gamgi paths ===============================

ifndef FLAG
FLAG = 1

SRC = ../

include $(SRC)make_paths

#========================= Gamgi files ===============================

GAMGI_o = $(GAMGI_ENGINE_o) $(GAMGI_GTK_o) $(GAMGI_MESA_o) \
	$(GAMGI_MATH_o) $(GAMGI_CHEM_o) $(GAMGI_PHYS_o) \
	$(GAMGI_IO_o) $(GAMGI_EXPAT_o) \
	gamgi_global_create.o gamgi_global_remove.o gamgi_expat_main.o 

GAMGI_h = $(GAMGI_ENGINE_h) $(GAMGI_GTK_h) $(GAMGI_MESA_h) \
	$(GAMGI_MATH_h) $(GAMGI_CHEM_h) $(GAMGI_PHYS_h) \
	$(GAMGI_IO_h) $(GAMGI_EXPAT_h) # $(GAMGI_GLOBAL_h)

#========================= Gamgi local ===============================

include $(SRC)make_local

#========================= Gamgi rules ===============================

include $(SRC)make_rules

endif
