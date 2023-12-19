#
#
#SUBJ = jpeg create_jpeg cut_photo
SUBJ =  jpeg create_jpeg


#
all: $(SUBJ)

#
#CC   = gcc
CC   = g++
AR   = ar
TH	 = touch

TOPDIR=../..

LIB_DIR=$(TOPDIR)/C++Lib


LIB_BSP_DIR = $(LIB_DIR)/BasicLib
LIB_EXP_DIR = $(LIB_DIR)/ExtendLib
LIB_GRP_DIR = $(LIB_DIR)/GraphLib
LIB_MTP_DIR = $(LIB_DIR)/MathLib

LIB_BSC_DIR = $(LIB_DIR)/BasicLib/Lib
LIB_EXT_DIR = $(LIB_DIR)/ExtendLib/xLib

LIB_BSP = $(LIB_BSP_DIR)/libbasic++.a
LIB_EXP = $(LIB_EXP_DIR)/libextend++.a
LIB_GRP = $(LIB_GRP_DIR)/libgraph++.a
LIB_MTP = $(LIB_MTP_DIR)/libmath++.a
LIB_GRF = $(LIB_GRF_DIR)/libgraph.a


#DEBUG = -DEBUG

CFLAGS = -fPIC -DHAVE_CONFIG_H -I$(TOPDIR) -I$(LIB_BSC_DIR) -I$(LIB_BSP_DIR) -I$(LIB_MTP_DIR) -I$(LIB_GRP_DIR) -I$(LIB_GRF_DIR) -I$(LIB_EXT_DIR) $(DEBUG)

BLIB = -L$(LIB_BSP_DIR) -lbasic++ -lm 
MLIB = -L$(LIB_MTP_DIR) -lmath++
ELIB = -L$(LIB_EXP_DIR) -lextend++
RLIB = -L$(LIB_GRP_DIR) -lgraph++
GLIB = -L$(LIB_GRF_DIR) -lgraph
#
#
#


.h.c:
	$(TH) $@


.cpp.o:
	$(CC) $< $(CFLAGS) -c -O2 

.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ)


#
#
#
#
cut_photo: cut_photo.o extract_graph.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o extract_graph.o $(ELIB) $(RLIB) $(BLIB) -O2 -o $@  -ljpeg -lssl -lcrypto -lcrypt -lz

jpeg: jpeg.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(BLIB) -O2 -o $@  -ljpeg -lssl -lcrypto -lcrypt -lz

create_jpeg: create_jpeg.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(BLIB) -O2 -o $@  -ljpeg -lssl -lcrypto -lcrypt -lz






$(LIB_BSP):
	(cd $(LIB_BSP_DIR) && make)


$(LIB_GRP):
	(cd $(LIB_GRP_DIR) && make)


$(LIB_GRF):
	(cd $(LIB_GRF_DIR) && make)


$(LIB_EXP):
	(cd $(LIB_EXP_DIR) && make)


$(LIB_MTP):
	(cd $(LIB_MTP_DIR) && make)



