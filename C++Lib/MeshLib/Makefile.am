#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../BasicLib/Lib  -I../BasicLib -I../MathLib -I../BREPLib -I../ExtendLib/xLib -I../ExtendLib -I../GraphLib
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS

AM_CFLAGS += -Wno-class-memaccess


SRC = MeshObjectData.cpp MaterialParam.cpp FacetBaseData.cpp ColladaTool.cpp


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libmesh.a 

libmesh_a_SOURCES = $(SRC)


clean:
	-rm -f *.o *~ $(lib_LIBRARIES)


clean_deps: 
	-rm -rf .deps 




