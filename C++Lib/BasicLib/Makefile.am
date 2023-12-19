#
#
#

AM_CFLAGS = -fPIC -W -Wall -I./Lib 
AM_CFLAGS += @DEBUG@ -DCPLUSPLUS

AM_CFLAGS += -Wno-stringop-overflow -Wno-stringop-truncation
AM_CFLAGS += -Wno-format-truncation -Wno-tautological-compare


SRC  = RingBuffer.cpp tools++.cpp network++.cpp ClassBox.cpp 
#CSRC = Lib/buffer.c Lib/cipher.c Lib/matrix.c Lib/mt.c Lib/network.c Lib/password.c Lib/ttree.c Lib/tlist.c Lib/xtools.c Lib/tools.c Lib/txml.c
CSRC =  Lib/buffer.c Lib/matrix.c Lib/mt.c Lib/network.c Lib/password.c Lib/ttree.c Lib/tlist.c Lib/xtools.c Lib/tools.c Lib/txml.c Lib/jbxl_state_func.c Lib/jbxl_state.c Lib/tjson.c


CC  = g++

AM_CXXFLAGS = $(AM_CFLAGS)


lib_LIBRARIES = libbasicpp.a 

libbasicpp_a_SOURCES = $(CSRC) $(SRC)

all:
	ln -fs $(lib_LIBRARIES) libbasic++.a


Lib/buffer.c:
	cd Lib && make


clean:
	-rm -f *.o *~ *.a
	-(cd Lib && make clean)	


clean_deps: 
	-rm -rf .deps 


