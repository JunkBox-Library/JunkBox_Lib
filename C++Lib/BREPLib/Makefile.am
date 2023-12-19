#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../BasicLib/Lib -I../BasicLib -I../MathLib -I../MeshLib
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS


SRC = Brep.cpp STL.cpp TriBrep.cpp BrepLib.cpp


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libbrep.a
#LSO_BRP = libbrep.so

libbrep_a_SOURCES = $(SRC)


clean:
	-rm -f *.o *~ $(lib_LIBRARIES)


clean_deps: 
	-rm -rf .deps 



