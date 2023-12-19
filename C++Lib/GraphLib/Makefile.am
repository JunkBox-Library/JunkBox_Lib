#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../BasicLib/Lib  -I../BasicLib -I../MathLib
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS

AM_CFLAGS += -Wno-class-memaccess


SRC  = gheader.c Jpeg2KTool.cpp Jpeg2KTool2.cpp Gdata.cpp ExGdata.cpp Gio.cpp Gmt.cpp Graph.cpp Morph.cpp Thinning.cpp JpegTool.cpp
SRC += TgaTool.cpp


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libgraphpp.a 

libgraphpp_a_SOURCES = $(SRC)


all:
	ln -sf $(lib_LIBRARIES) libgraph++.a


clean:
	-rm -f *.o *~ gheader.* *.a


clean_deps: 
	-rm -rf .deps 


gheader.c:
	ln -sf  ../../gLib/gheader.h .
	ln -sf  ../../gLib/gheader.c gheader.c


