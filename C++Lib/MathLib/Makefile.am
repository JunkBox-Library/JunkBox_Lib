#
#
#

AM_CFLAGS = -fPIC -W -Wall -I../BasicLib/Lib  -I../BasicLib
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS


SRC = Rotation.cpp Matrix++.cpp TVector.cpp Tolerance.cpp Vector.cpp


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libmathpp.a 

libmathpp_a_SOURCES = $(SRC)


all:
	ln -sf $(lib_LIBRARIES) libmath++.a

clean:
	-rm -f *.o *~ *.a


clean_deps: 
	-rm -rf .deps 



