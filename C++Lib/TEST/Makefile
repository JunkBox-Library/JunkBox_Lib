#
#

OPENJPEG_VERSION = 21
OPJINC = /usr/local/include/openjpeg-2.1

OPJLIB = openjp2


#SUBJ = texture_entry primmesher tree 
#SUBJ = jp2k meshdump2 meshdump oarprim primmesher texture_entry 
SUBJ = meshdump rot1 rot2 rot3 rot4 rot5 rot6 rot7


TOPDIR=../..

#
all: $(SUBJ)

#
CC   = g++
AR   = ar
TH	 = touch


LIB_DIR=$(TOPDIR)/C++Lib

LIB_BSP_DIR = $(LIB_DIR)/BasicLib
LIB_EXP_DIR = $(LIB_DIR)/ExtendLib
LIB_GRP_DIR = $(LIB_DIR)/GraphLib
LIB_MTP_DIR = $(LIB_DIR)/MathLib
LIB_MSH_DIR = $(LIB_DIR)/MeshLib
LIB_BRP_DIR = $(LIB_DIR)/BREPLib

LIB_BSC_DIR = $(LIB_DIR)/BasicLib/Lib
LIB_EXT_DIR = $(LIB_DIR)/ExtendLib/xLib
LIB_GRF_DIR = $(LIB_DIR)/gLib

LIB_BSP = $(LIB_BSP_DIR)/libbasicpp.a
LIB_EXP = $(LIB_EXP_DIR)/libextendpp.a
LIB_GRP = $(LIB_GRP_DIR)/libgraphpp.a
LIB_MTP = $(LIB_MTP_DIR)/libmathpp.a
LIB_GRF = $(LIB_GRF_DIR)/libgraph.a
LIB_MSH = $(LIB_MSH_DIR)/libmesh.a
LIB_BRP = $(LIB_BRP_DIR)/libbrep.a


#DEBUG = -DEBUG


CFLAGS = -fPIC -DHAVE_CONFIG_H -I.. -I/usr/local/include -I$(TOPDIR) -I$(LIB_BSC_DIR) -I$(OPJINC) -I$(LIB_BSP_DIR) -I$(LIB_MTP_DIR) -I$(LIB_GRP_DIR) -I$(LIB_GRF_DIR) -I$(LIB_EXP_DIR) -I$(LIB_EXT_DIR) -I$(LIB_BRP_DIR) -I$(LIB_MSH_DIR) $(DEBUG)

CFLAGS += -DENABLE_OPENJPEG -DOPENJPEG_VERSION=$(OPENJPEG_VERSION)
CFLAGS += -DENABLE_JPEGLIB



BLIB = -L$(LIB_BSP_DIR) -lbasicpp -lm 
MLIB = -L$(LIB_MTP_DIR) -lmathpp
ELIB = -L$(LIB_EXP_DIR) -lextendpp
RLIB = -L$(LIB_GRP_DIR) -lgraphpp
GLIB = -L$(LIB_GRF_DIR) -lgraph
PLIB = -L$(LIB_BRP_DIR) -lbrep
HLIB = -L$(LIB_MSH_DIR) -lmesh
#
#
#


.h.c:
	$(TH) $@


.cpp.o: $(LIB_MSH)
	$(CC) $< $(CFLAGS) -c -O2 

.c.o:
	$(CC) $< $(CFLAGS) -c -O2 


clean:
	rm -f *.o *~ $(SUBJ)


#
#
#
#
#export: export.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
#	$(CC) $(@).o $(PLIB) $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz  
#

tree: tree.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz -ljpeg -lopenjp2

jp2k: jp2k.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz -lopenjp2

meshdump2: meshdump2.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz -ljpeg -lopenjp2

meshdump: meshdump.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz  

oarprim: oarprim.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz -ljpeg -lopenjp2

primmesher: primmesher.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz -ljpeg -lopenjp2

texture_entry: texture_entry.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_BRP) $(LIB_MSH) $(LIB_MTP)
	$(CC) $(@).o $(HLIB) $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz  

collada: collada.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_MTP)
	$(CC) $(@).o $(ELIB) $(PLIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz  

#dae: dae.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) $(LIB_MTP)
#	$(CC) $(@).o $(PLIB) $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz  

bvh: bvh.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot1: rot1.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot2: rot2.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot3: rot3.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot4: rot4.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot5: rot5.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot6: rot6.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz

rot7: rot7.o $(LIB_BSP) $(LIB_GRP) $(LIB_EXP) 
	$(CC) $(@).o $(ELIB) $(RLIB) $(MLIB) $(BLIB) -O2 -o $@  -lz







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

$(LIB_MSH):
	(cd $(LIB_MSH_DIR) && make)

$(LIB_BRP):
	(cd $(LIB_BRP_DIR) && make)

