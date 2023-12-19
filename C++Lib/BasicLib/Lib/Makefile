
SRC_TOP = ../../../Lib

CC  = g++
AR  = ar
TH  = touch
CP  = cp
LN  = ln -fs


#
DEBUG =
#DEBUG = -DEBUG

CFLAGS = -fPIC -W -Wall -DHAVE_CONFIG_H -I../.. $(DEBUG)


LIB_H   = tjson.h buffer.h cipher.h matrix.h mt.h network.h password.h tlist.h ttree.h tools.h txml.h xtools.h jbxl_state.h common.h
LIB_SRC = tjson.c buffer.c cipher.c matrix.c mt.c network.c password.c tlist.c ttree.c tools.c txml.c xtools.c jbxl_state.c jbxl_state_func.c


all: $(LIB_SRC) $(LIB_H)


.c.o: 
	$(CC) $< $(CFLAGS) -c -O2 



clean:
	-rm -f *~ *.o *.h *.c make_state_c


#
#
#
#

make_state_c:
	$(LN) $(SRC_TOP)/$@ .

common.h:
	$(LN) $(SRC_TOP)/$@ .

buffer.h:
	$(LN) $(SRC_TOP)/$@ .

buffer_tool.h:
	$(LN) $(SRC_TOP)/$@ .

cipher.h:
	$(LN) $(SRC_TOP)/$@ .

fdb.h:
	$(LN) $(SRC_TOP)/$@ .

matrix.h:
	$(LN) $(SRC_TOP)/$@ .

mt.h:
	$(LN) $(SRC_TOP)/$@ .

network.h:
	$(LN) $(SRC_TOP)/$@ .

password.h:
	$(LN) $(SRC_TOP)/$@ .

tlist.h:
	$(LN) $(SRC_TOP)/$@ .

ttree.h:
	$(LN) $(SRC_TOP)/$@ .

tools.h:
	$(LN) $(SRC_TOP)/$@ .

txml.h:
	$(LN) $(SRC_TOP)/$@ .

xtools.h:
	$(LN) $(SRC_TOP)/$@ .

jbxl_state.h:
	$(LN) $(SRC_TOP)/$@ .

tjson.h:
	$(LN) $(SRC_TOP)/$@ .


buffer.c:
	$(LN) $(SRC_TOP)/$@ .

buffer_tool.c:
	$(LN) $(SRC_TOP)/$@ .

cipher.c:
	$(LN) $(SRC_TOP)/$@ .

fdb.c: 
	$(LN) $(SRC_TOP)/$@ .

matrix.c:
	$(LN) $(SRC_TOP)/$@ .

mt.c:
	$(LN) $(SRC_TOP)/$@ .

network.c:
	$(LN) $(SRC_TOP)/$@ .

password.c:
	$(LN) $(SRC_TOP)/$@ .

tlist.c:
	$(LN) $(SRC_TOP)/$@ .

ttree.c:
	$(LN) $(SRC_TOP)/$@ .

tools.c:
	$(LN) $(SRC_TOP)/$@ .

txml.c:
	$(LN) $(SRC_TOP)/$@ .

xtools.c:
	$(LN) $(SRC_TOP)/$@ .

jbxl_state_func.c:
	$(LN) $(SRC_TOP)/$@ .

jbxl_state.c: jbxl_state.h make_state_c
	./make_state_c > jbxl_state.c
#	/bin/sh $(SRC_TOP)/make_state_c $(SRC_TOP) > jbxl_state.c

tjson.c:
	$(LN) $(SRC_TOP)/$@ .


