#
#
SUBJ = jpeg2k jpeg_color2mono 


#
all: $(SUBJ)

#
CC   = gcc
#CC   = g++
AR   = ar
TH	 = touch

TOPDIR=../..

LIB_DIR=$(TOPDIR)

LIB_BSC_DIR = $(LIB_DIR)/Lib
LIB_GLB_DIR = $(LIB_DIR)/gLib
LIB_EXT_DIR = $(LIB_DIR)/xLib

LIB_BSC = $(LIB_BSC_DIR)/libbasic.a
LIB_GLB = $(LIB_GLB_DIR)/libgraph.a
LIB_EXT = $(LIB_EXT_DIR)/libextend.a

#DEBUG = -DEBUG

CFLAGS  = -fPIC -DHAVE_CONFIG_H -I$(LIB_DIR) -I$(LIB_BSC_DIR) -I$(LIB_GLB_DIR) -I$(LIB_EXT_DIR) $(DEBUG) -DENABLE_JPEGLIB  -DENABLE_OPENJPEG -DOPENJPEG_VERSION=25 -DHAVE_OPENJPEG_H -I/usr/local/include/openjpeg-2.5


BLIB = -L$(LIB_BSC_DIR) -lbasic -lm 
GLIB = -L$(LIB_GLB_DIR) -lgraph
ELIB = -L$(LIB_EXT_DIR) -lextend
#
#
#

.h.c:
	$(TH) $@


.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ)


#
#
#
#
jpeg2k: jpeg2k.o $(LIB_BSC) $(LIB_GLB) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(GLIB) $(BLIB) -O2 -o $@  -L/usr/local/lib -ljpeg -lssl -lcrypto -lcrypt -lz -lopenjp2

jpeg_color2mono: jpeg_color2mono.o $(LIB_BSC) $(LIB_GLB) $(LIB_EXT) 
	$(CC) $(@).o $(ELIB) $(GLIB) $(BLIB) -O2 -o $@  -L/usr/local/lib -ljpeg -lssl -lcrypto -lcrypt -lz






$(LIB_BSC):
	(cd $(LIB_BSC_DIR) && make)


$(LIB_GLB):
	(cd $(LIB_GLB_DIR) && make)


$(LIB_EXT):
	(cd $(LIB_EXT_DIR) && make)


