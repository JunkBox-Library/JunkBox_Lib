#
#
#

AM_CFLAGS = -fPIC -W -Wall 

AM_CFLAGS += -Wno-stringop-truncation -Wno-stringop-overflow
AM_CFLAGS += -Wno-format-truncation -Wno-tautological-compare 
#AM_CFLAGS += -Wno-implicit-function-declaration


SRC = buffer.c tools.c matrix.c mt.c tlist.c ttree.c txml.c xtools.c jbxl_state_func.c jbxl_state.c tjson.c


if DISABLE_WIN
#    SRC += network.c c password.c cipher.c
    SRC += network.c c password.c 
else
    EXEC = c2doc.exe
endif


AM_CFLAGS += @DEBUG@



lib_LIBRARIES = libbasic.a

libbasic_a_SOURCES = $(SRC)


jbxl_state.c: make_state_c jbxl_state.h
	/bin/bash make_state_c > jbxl_state.c


clean:
	-rm -f *.o *~ libbasic.a  jbxl_state.c


clean_deps: 
	-rm -rf .deps 


